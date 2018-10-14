/* Copyright (C) 2017 Micael Oliveira <micael.oliveira@mpsd.mpg.de>
 *
 * This file is part of ESCDF.
 *
 * ESCDF is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, version 2.1 of the License, or (at your option) any
 * later version.
 *
 * ESCDF is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ESCDF.  If not, see <http://www.gnu.org/licenses/> or write to
 * the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA.
 */

#include <assert.h>


#include "escdf_attributes.h"
#include "escdf_datasets.h"
#include "utils_hdf5.h"
#include "escdf_datatransfer.h"

/**
 * @brief Dataset data structure
 * 
 */

struct escdf_dataset {

 
    const escdf_dataset_specs_t *specs;

    /* bool is_set; */
    bool is_ordered;

    bool ordered_flag_set;

    bool transfer_on_disk;


    /**
     * @brief effective number of dimensions
     * 
     * This can differ from specs->ndims in case of compact storage or sparse datasets,
     * where the usual value is 1.
     */
    unsigned int ndims_effective;


    /**
     * @brief Dimensions: array holding the number of elements in each dimension.
     * 
     */
    unsigned int *dims;


    /**
     * @brief dims_attr is an array of pointers to the attributes defining the dimensions of the dataset
     */
    escdf_attribute_t **dims_attr;



    /* In case of irregular arrays (only two-dimensional allowed) the data is stored
     * as one dimensional array, and we need the additional index array to map to the array.
     * 
     * Whether or not this compact storage is used, is determined by compact.
     */
    hsize_t *index_array; /* only used for compact storage */

    /**
     * @brief re-ordering table
     * 
     */
    escdf_datatransfer_t *transfer;
    
    hid_t type_id;
    hid_t xfer_id;

    hid_t dtset_id;

};


size_t escdf_dataset_specs_sizeof(const escdf_dataset_specs_t *specs)
{
    assert(specs != NULL);

    switch (specs->datatype) {
    case ESCDF_DT_BOOL:
        return sizeof(bool);
    case ESCDF_DT_UINT:
        return sizeof(unsigned int);
    case ESCDF_DT_INT:
        return sizeof(int);
    case ESCDF_DT_DOUBLE:
        return sizeof(double);
    case ESCDF_DT_STRING:
        return specs->stringlength * sizeof(char);
    default:
        return 0;
    }
}

/*
hid_t escdf_dataset_specs_hdf5_disk_type(const escdf_dataset_specs_t *specs)
{
    assert(specs != NULL);

    switch (specs->datatype) {
        case ESCDF_DT_BOOL:
            return H5T_C_S1;
        case ESCDF_DT_UINT:
            return H5T_NATIVE_UINT;
        case ESCDF_DT_INT:
            return H5T_NATIVE_INT;
        case ESCDF_DT_DOUBLE:
            return H5T_NATIVE_DOUBLE;
        case ESCDF_DT_STRING:
            return H5T_C_S1;
        default:
            return 0;
    }
}

hid_t escdf_dataset_specs_hdf5_mem_type(const escdf_dataset_specs_t *specs)
{
    assert(specs != NULL);

    switch (specs->datatype) {
        case ESCDF_DT_BOOL:
            return H5T_C_S1;
        case ESCDF_DT_UINT:
            return H5T_NATIVE_UINT;
        case ESCDF_DT_INT:
            return H5T_NATIVE_INT;
        case ESCDF_DT_DOUBLE:
            return H5T_NATIVE_DOUBLE;
        case ESCDF_DT_STRING:
            return H5T_C_S1;
        default:
            return 0;
    }
}
*/

bool escdf_dataset_specs_is_present(const escdf_dataset_specs_t *specs, hid_t loc_id)
{
    assert(specs != NULL);

    return utils_hdf5_check_present(loc_id, specs->name);
}

unsigned int escdf_dataset_get_number_of_dimensions(const escdf_dataset_t *data)
{
    assert(data != NULL);

    return data->specs->ndims;
}


const unsigned int * escdf_dataset_get_dimensions(const escdf_dataset_t *data)
{
    /* this routine assumes regular dimensions of the dataset */

    assert(data != NULL);

    return data->dims;
}

/* not strictly necessary as specs are public */

bool escdf_dataset_is_disordered_storage_allowed(const escdf_dataset_t *data)
{
    assert(data != NULL);

    return data->specs->disordered_storage_allowed;
}

bool escdf_dataset_is_ordered(const escdf_dataset_t *data)
{
    assert(data != NULL);

    return data->is_ordered;
}

bool escdf_dataset_is_compact(const escdf_dataset_t *data) {

    assert(data != NULL);

    return data->specs->compact;
}


escdf_errno_t escdf_dataset_set_ordered(escdf_dataset_t *data, bool ordered)
{
    if( !data->specs->disordered_storage_allowed && !ordered ) 
        RETURN_WITH_ERROR(ESCDF_ERROR);

    data->is_ordered = ordered;

    return ESCDF_SUCCESS;
}


escdf_datatransfer_t * escdf_dataset_get_datatransfer(const escdf_dataset_t * data)
{
    assert(data != NULL);

    return data->transfer;
}

escdf_errno_t escdf_dataset_set_datatransfer(escdf_dataset_t *data, escdf_datatransfer_t *transfer)
{
    int ii, fast_dim;

    assert(data != NULL);

    if(!data->specs->disordered_storage_allowed)
        RETURN_WITH_ERROR(ESCDF_ERROR);

    data->transfer = transfer;

    data->is_ordered = false;

    return ESCDF_SUCCESS;
}


escdf_dataset_t * escdf_dataset_new(const escdf_dataset_specs_t *specs, escdf_attribute_t **attr_dims)
{
    escdf_dataset_t *data = NULL;
    escdf_errno_t error;
    unsigned int ii, j;


    unsigned int dims0;
    unsigned int *dims;
    unsigned int *dims1;
    unsigned int ndims_effective;
    unsigned int len;

 
    /* Check input */
    assert(specs != NULL);
    assert(attr_dims != NULL);
   
    /*
    printf("escdf_dataset_new: name = %s\n",specs->name); fflush(stdout); 
    */

    /* ndims = specs->ndims; */ 

    /* Allocate memory */ 
    data = (escdf_dataset_t *) malloc(sizeof(escdf_dataset_t));
    
    if (data == NULL) {
        /* printf("escdf_dataset_new: malloc failed!\n"); */
        return data;
    }

    /* printf("escdf_dataset_new: malloc succesful!\n"); */


    /* set default values */


    /* Check whether dimensions are regular shaped */

    if (specs->compact) {
        
        printf("escdf_dataset_new: name = %s, compact flag set. \n",specs->name); fflush(stdout); 
        

        if (specs->ndims == 2) {

            /**
             * Store the data as one dimensional array with an additional indexing array.
             * 
             */
            ndims_effective = 1;

            /*
            printf("escdf_dataset_new: name = %s, First dimension: %s, %d. \n", specs->name, specs->dims_specs[0]->name, specs->dims_specs[0]->ndims); fflush(stdout);
            printf("escdf_dataset_new: name = %s, First dimension: %d, %d. \n", specs->name, escdf_attribute_sizeof(attr_dims[0]), sizeof(dims0)); fflush(stdout);
            */

            error = escdf_attribute_get(attr_dims[0], &dims0);

            /*
            printf("escdf_dataset_new: name = %s, dims0 = %d. error = %d \n", specs->name, dims0, error); fflush(stdout); fflush(stdout);
            printf("escdf_dataset_new: name = %s, sizeof(attr_dims[1]) = %d. \n", specs->name, escdf_attribute_sizeof(attr_dims[1])); fflush(stdout);
            */

            dims =  (unsigned int*) malloc(ndims_effective * sizeof(unsigned int));
            dims1 = (unsigned int*) malloc(escdf_attribute_sizeof(attr_dims[1])); 

            /*
            printf("escdf_dataset_new: name = %s, Second dimension: %s, %d. \n", specs->name, specs->dims_specs[1]->name, specs->dims_specs[1]->ndims); fflush(stdout);
            printf("escdf_dataset_new: name = %s, Second dimension: %d, %d. \n", specs->name, escdf_attribute_sizeof(attr_dims[1]), dims0 * escdf_attribute_specs_sizeof(specs->dims_specs[1])); fflush(stdout);

            DEFER_FUNC_ERROR(escdf_attribute_get(attr_dims[1], dims1));

            */

            error = escdf_attribute_get(attr_dims[1], dims1);

            /*
            printf("escdf_dataset_new: name = %s, error = %d \n", specs->name, error); fflush(stdout); 

            for(ii=0; ii<dims0; ii++) {
                printf("escdf_dataset_new: name = %s, dims1[%d] = %d. \n", specs->name, ii, dims1[ii]);
            }

            printf("escdf_dataset_new: name = %s, before malloc index_array: %d. \n", specs->name, dims0 * sizeof(hsize_t)); fflush(stdout);
            */

            data->index_array = (unsigned int*) malloc((dims0 * sizeof(unsigned int)));

            /* printf("escdf_dataset_new: name = %s, after malloc index_array. \n", specs->name); fflush(stdout); */

            for(ii=0, j=0; ii<dims0; ii++) {
                data->index_array[ii] = j;
                j += dims1[ii];
            }
            dims[0] = j;
            /* dims[1] = 0; // was 1! */ 

            /* printf("escdf_dataset_new: name = %s, dims set: %d %d \n",specs->name, dims[0], dims[1]); fflush(stdout); */

        }
        else { 
             
            printf("escdf_dataset_new: name = %s, compact flag set for ndims /= 2. Return NULL!\n",specs->name); fflush(stdout);
            
            REGISTER_ERROR(ESCDF_ERROR_DIM); 

            return NULL;
        } 

    }
    else {
        /*
        printf("escdf_dataset_new: created memory for dims for %s\n",specs->name); fflush(stdout); 
        */

        ndims_effective = specs->ndims;

        if (ndims_effective > 0) {
            dims = (unsigned int*) malloc(ndims_effective * sizeof(unsigned int));

            assert(attr_dims != NULL);
            for (ii = 0; ii < specs->ndims; ii++) {
                assert(escdf_attribute_is_set(attr_dims[ii]));
                assert(escdf_attribute_get_specs_id(attr_dims[ii]) == specs->dims_specs[ii]->id);
                assert(specs->dims_specs[ii]->datatype == ESCDF_DT_UINT);

                SUCCEED_OR_BREAK(escdf_attribute_get(attr_dims[ii], &(dims[ii])));

                
                printf("escdf_dataset_new: read dim[%d] for %s. dim[%d] = %d.\n", ii, specs->name, ii, dims[ii] ); fflush(stdout); 
                
            }
        }

    }

    /* printf("escdf_dataset_new: created memory for %s\n",specs->name); fflush(stdout); */
    data->specs = specs;

    /* printf("escdf_dataset_new: will create %d dims for %s\n",ndims, specs->name); fflush(stdout); */
    data->ndims_effective = ndims_effective;
    data->dims = (unsigned int*) malloc(ndims_effective * sizeof(unsigned int));
    /* printf("escdf_dataset_new: succesfully created %d dims for %s\n",data->ndims_effective, specs->name); fflush(stdout); */

    assert(data->dims != NULL);

    for(ii = 0; ii<ndims_effective; ii++) {
        data->dims[ii] = dims[ii];  
        printf("escdf_dataset_new: created dims for %s: dim[%d] = %d \n",specs->name, ii, data->dims[ii]); fflush(stdout); 
    }
    
    free(dims);

    data->dims_attr = (escdf_attribute_t**) malloc(data->specs->ndims * sizeof(escdf_attribute_t*));

    for(ii=0; ii<data->specs->ndims; ii++) {
        data->dims_attr[ii] = attr_dims[ii];
    }    

    data->dtset_id = ESCDF_UNDEFINED_ID;
    data->is_ordered = true;
    data->transfer = NULL;
    data->transfer_on_disk = false;

    /* data->type_id = escdf_dataset_specs_hdf5_disk_type(specs); */
    data->type_id = utils_hdf5_disk_type(specs->datatype);

    /* printf("escdf_dataset_new: for %s: type_id = %lld \n",specs->name, data->type_id); fflush(stdout); */
    
    if(data->type_id == H5T_C_S1) {
        /* printf("escdf_dataset_new: resizing string to = %d\n",data->specs->stringlength ); */

        data->type_id = H5Tcopy(H5T_C_S1);
        assert(H5Tset_size(data->type_id, (size_t) data->specs->stringlength)>=0);
        assert(H5Tset_strpad(data->type_id, H5T_STR_NULLTERM)>=0);

        /* printf("escdf_dataset_new: for %s: new type_id = %lld \n",specs->name, data->type_id); fflush(stdout); */
    
    }
    

    /* QUESTION: Where do we define the xfer_id? */

    data->xfer_id = ESCDF_UNDEFINED_ID;

/*
    for(ii = 0; ii<ndims_effective; ii++) {
        data->dims[ii] = dims[ii];  
        printf("escdf_dataset_new: created dims for %s: dim[%d] = %d \n",specs->name, ii, data->dims[ii]); fflush(stdout); 
    }
*/
    
    /* printf("escdf_dataset_new: completed for %s\n",data->specs->name); fflush(stdout); */
    
   
    return data;
}

escdf_errno_t escdf_dataset_create(escdf_dataset_t *data, hid_t loc_id)
{

    int transfer_id;         /**< Handle of the data transfer (re-ordering) table */

    assert(data != NULL);

    /*    
    printf("escdf_dataset_create attempting to create %s; %lld %lld\n", data->specs->name, data->dtset_id, loc_id);
    printf("escdf_dataset_create attempting to create %s; ndims = %lld (%lld) \n", data->specs->name, data->specs->ndims, data->ndims_effective);
    
    fflush(stdout);
    */

    if(data->dtset_id == ESCDF_UNDEFINED_ID ) {
    
        /*
        printf("escdf_dataset_create attempting to create hdf5 dataset %s\n", data->specs->name) ;
        printf("  dtset_id = %lld\n", data->dtset_id);
        printf("  type_id = %lld\n", data->type_id);
        printf("  loc_id = %lld\n", loc_id);

        for(int i=0; i<data->ndims_effective; i++) {
            printf("  dims[%d] = %lld\n", i, data->dims[i]);
        }

        fflush(stdout);
        */

        SUCCEED_OR_RETURN(utils_hdf5_create_dataset(loc_id, data->specs->name, \
                                        data->type_id, data->dims, data->ndims_effective, &data->dtset_id));
        /*
        printf("escdf_dataset_create after creating hdf5 dataset %s; %d\n", data->specs->name, data->dtset_id);
        fflush(stdout);
        */
    }
    else {
        RETURN_WITH_ERROR(ESCDF_ERROR);
        /* alternatively we could close and reopen the dataset? */
    }

    SUCCEED_OR_RETURN(utils_hdf5_write_attr_bool(data->dtset_id, "is_ordered", NULL, 0, &(data->is_ordered)));


    /* Flag error if data is not ordered but there is no reordering table */

    if( !data->is_ordered || data->transfer!=NULL ) 
        RETURN_WITH_ERROR(ESCDF_ERROR);

    /* write reordering table as dataset within the dataset. Write even is data is ordered (?) */

    if(data->transfer!=NULL)
        transfer_id = escdf_datatransfer_get_id(data->transfer);
    else
        transfer_id = ESCDF_UNDEFINED_ID;

    SUCCEED_OR_RETURN(utils_hdf5_write_attr(data->dtset_id, "transfer", H5T_NATIVE_INT, NULL, 0, H5T_NATIVE_INT, &transfer_id));
    /*   
    printf("escdf_dataset_create done.\n");
    */

    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_dataset_open(escdf_dataset_t *data, hid_t loc_id)
{
    _bool_set_t tmp_bool;
    escdf_errno_t err;
    bool result;
    hid_t dtset_pt;

    assert(data != NULL);

    /* printf("escdf_dataset_open: attempting to open %s, %d.\n", data->specs->name, loc_id ); */


    if(data->dtset_id == ESCDF_UNDEFINED_ID ) {

        if(utils_hdf5_check_present(loc_id, data->specs->name)==false) {
            printf("escdf_dataset_open: check_present == false.\n"); fflush(stdout);
            RETURN_WITH_ERROR(ESCDF_ERROR);
        }
        printf("escdf_dataset_open: after check_present.\n"); fflush(stdout);

        SUCCEED_OR_RETURN(utils_hdf5_open_dataset(loc_id, data->specs->name, &(dtset_pt)));
        printf("escdf_dataset_open: after util_hdf5_open_dataset.\n"); fflush(stdout);
    
        data->dtset_id = dtset_pt;
    }
    else {
        /* printf("escdf_dataset_open: %s is is already open with dtset id %d .\n", data->specs->name, data->dtset_id ); */
 
        /* Should we return with error, if the dataset is already open? */
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    /* printf("escdf_dataset_open: %s attempting to read attribute is_ordered.\n", data->specs->name ); */
 
    SUCCEED_OR_RETURN(utils_hdf5_read_attr_bool(data->dtset_id, "is_ordered", NULL, 0, &tmp_bool));

    /* printf("escdf_dataset_open: %s attribute is_ordered read.\n", data->specs->name ); */


    data->is_ordered = tmp_bool.value;
    data->ordered_flag_set = tmp_bool.is_set;

    /* check whether a reordering table is present in the file */

    result = utils_hdf5_check_present_attr(data->dtset_id, "transfer");

    if(result) {
        /* printf("escdf_dataset_open: %s transfer table present.\n", data->specs->name ); */
    }
    else {
        /* printf("escdf_dataset_open: %s transfer table NOT present.\n", data->specs->name ); */

        if(!data->is_ordered) {
            RETURN_WITH_ERROR(ESCDF_ERROR);
        }
    }

    /* printf("escdf_dataset_open: %s opened.\n", data->specs->name ); */
    
    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_dataset_close(escdf_dataset_t *data)
{
    escdf_errno_t err;


    if( data->transfer != NULL && data->transfer_on_disk ) {
        /* TODO: need to write transfer table */
    }

    /* close dataset on disk */
    
    FULFILL_OR_RETURN(utils_hdf5_close_dataset(data->dtset_id)==ESCDF_SUCCESS, ESCDF_ERROR);

    return ESCDF_SUCCESS;
}


void escdf_dataset_free(escdf_dataset_t *data)
{
    if (data != NULL) {
        free(data->dims);
        free(data);
    }
}


escdf_errno_t escdf_dataset_read(const escdf_dataset_t *data, hsize_t *start, hsize_t *count, hsize_t *stride, void *buf)
{
    escdf_errno_t err;
    _bool_set_t tmpb;

    unsigned int i;

    hid_t mem_type_id;

    char *tmpc;

    bool compact;
    hsize_t start_compact[1];
    hsize_t count_compact[1];
    hsize_t stride_compact[1];
    hsize_t *start_ptr, *count_ptr, *stride_ptr;

    assert(data != NULL);
    assert(buf != NULL);

    /* check that the dataset in the file is open */

    if(data->dtset_id == ESCDF_UNDEFINED_ID) {
        printf("dataset not opened.");
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    /* check whether we need to re-order on read */

    /* QUESTION: If is_ordered = false and no reordering present, shall we fail or use normal order ? */

    if(!data->is_ordered) {
        printf("escdf_dataset_read_simple: %s is not stored in normal order.\n", data->specs->name); 
        fflush(stdout);
        if(data->transfer == NULL) RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    compact = escdf_dataset_is_compact(data);

    /* we need to re-shape the data in case of comact storage */

    if(compact)  {

        start_compact[0] = data->index_array[start[0]];
        count_compact[0] = count[1];    
        stride_compact[0] = 1;

        start_ptr = start_compact;
        count_ptr = count_compact;
        stride_ptr = stride_compact;

    } 
    else{

        start_ptr = start;
        count_ptr = count;
        stride_ptr = stride;

    }

    mem_type_id = utils_hdf5_mem_type(data->specs->datatype);

    if(mem_type_id == H5T_C_S1) {
        /* printf("escdf_datasets_write_simple: resizing string to = %d\n",data->specs->stringlength ); */

        mem_type_id = H5Tcopy(H5T_C_S1);
        H5Tset_size(mem_type_id, data->specs->stringlength);
        H5Tset_strpad(mem_type_id, H5T_STR_NULLTERM);
    }

    utils_hdf5_read_dataset(data->dtset_id, data->xfer_id, buf, mem_type_id, start_ptr, count_ptr, stride_ptr);

    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_dataset_read_simple(const escdf_dataset_t *data, void *buf)
{
    escdf_errno_t err;
    _bool_set_t tmpb;

    unsigned int i;

    hid_t mem_type_id;
    hsize_t *start, *count, *stride;

    char *tmpc;

    assert(data != NULL);
    assert(buf != NULL);

    /* printf("escdf_dataset_read_simple: attempting to read from %s.\n", data->specs->name); fflush(stdout);  */

    /* check whether we need to re-order on read */

    /* QUESTION: If is_ordered = false and no reordering present, shall we fail or use normal order ? */

    if(!data->is_ordered) {
        /* printf("escdf_dataset_read_simple: %s is not stored in normal order.\n", data->specs->name); fflush(stdout); */
        if(data->transfer == NULL) RETURN_WITH_ERROR(ESCDF_ERROR);

    }

    mem_type_id = utils_hdf5_mem_type(data->specs->datatype);

    if(mem_type_id == H5T_C_S1) {
        /* printf("escdf_datasets_write_simple: resizing string to = %d\n",data->specs->stringlength ); */

        mem_type_id = H5Tcopy(H5T_C_S1);
        H5Tset_size(mem_type_id, data->specs->stringlength);
        H5Tset_strpad(mem_type_id, H5T_STR_NULLTERM);
    }

    start = (hsize_t *) malloc(data->specs->ndims * sizeof(hsize_t));
    count = (hsize_t *) malloc(data->specs->ndims * sizeof(hsize_t));
    stride = (hsize_t *) malloc(data->specs->ndims * sizeof(hsize_t));
    
    for(i=0; i<data->specs->ndims; i++) {
        start[i] = 0;
        count[i] = data->dims[i];
        stride[i] = 1;
    }
    
    utils_hdf5_read_dataset(data->dtset_id, data->xfer_id, buf, mem_type_id, start, count, stride);

    return ESCDF_SUCCESS;
}



escdf_errno_t escdf_dataset_write_simple(escdf_dataset_t *data, void *buf)
{
    escdf_errno_t err;
    herr_t h5err;

    unsigned int i, j, len=1;
    hid_t type_id;
    hid_t *attr_ptr;

    hid_t mem_type_id;
    hsize_t *start, *count, *stride;

    assert(data != NULL);

    /* check that the dataset in the file is open */

    if(data->dtset_id == ESCDF_UNDEFINED_ID) {
        printf("dataset not opened.");
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    if(!data->is_ordered) {
        printf("disordered write not permitted in write_simple.");
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    printf("escdf_dataset_write_simple: %s, ndims = %d, %s \n", data->specs->name, data->specs->ndims, data->specs->compact?"compact":"");

    for(i=0; i<data->specs->ndims; i++) {

        printf("dim[%d] = %d\n", i, data->dims[i]);
    }

    start =  (hsize_t *) malloc(data->specs->ndims * sizeof(hsize_t));
	count =  (hsize_t *) malloc(data->specs->ndims * sizeof(hsize_t));
   	stride = (hsize_t *) malloc(data->specs->ndims * sizeof(hsize_t));

    if(data->specs->compact) {

        printf("escdf_dataset_write_simple: unsupported for compact storage.\n");

        /* The question here is what data structure we expect in *buf? */
        /*
         * In order to correctly address (and remap) a part of the original array
         * we need to know how the array is stored, and in particular the size of each element in memory 
         *
         **/

    } 
    else {
    
	    for(i=0; i<data->specs->ndims; i++) {
    	    start[i] = 0;
    	    count[i] = data->dims[i];
    	    stride[i] = 1;
    	}

    	/* printf("escdf_datasets_write_simple: data-type = %d\n",data->specs->datatype ); */

    	mem_type_id = utils_hdf5_mem_type(data->specs->datatype);

    	if(mem_type_id == H5T_C_S1) {
    	    /* printf("escdf_datasets_write_simple: resizing string to = %d\n",data->specs->stringlength ); */

    	    mem_type_id = H5Tcopy(H5T_C_S1);
    	    H5Tset_size(mem_type_id, data->specs->stringlength);
    	    H5Tset_strpad(mem_type_id, H5T_STR_NULLTERM);
    	}

	    utils_hdf5_write_dataset(data->dtset_id, data->xfer_id, buf, mem_type_id, start, count, stride);
        
    }

    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_dataset_write(const escdf_dataset_t *data, hsize_t *start, hsize_t *count, hsize_t *stride, void *buf)
{
    escdf_errno_t err;
    herr_t h5err;

    unsigned int i, len=1;
    hid_t type_id;
    hid_t *attr_ptr;
    hid_t mem_type_id;

    bool compact;
    hsize_t start_compact[1];
    hsize_t count_compact[1];
    hsize_t stride_compact[1];
    hsize_t *start_ptr, *count_ptr, *stride_ptr;

    assert(data != NULL);

    if(data->dtset_id == ESCDF_UNDEFINED_ID) {
        printf("dataset not opened.");
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    if(!data->is_ordered) {
        printf("disordered write not permitted in write_simple.");
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    compact = escdf_dataset_is_compact(data);

    /* we need to re-shape the data in case of comact storage */

    if(compact)  {

        start_compact[0] = data->index_array[start[0]];
        count_compact[0] = count[1];
        stride_compact[0] = 1;

        start_ptr = start_compact;
        count_ptr = count_compact;
        stride_ptr = stride_compact;

    } 
    else{

        start_ptr = start;
        count_ptr = count;
        stride_ptr = stride;

    }

    mem_type_id = utils_hdf5_mem_type(data->specs->datatype);

    if(mem_type_id == H5T_C_S1) {

        mem_type_id = H5Tcopy(H5T_C_S1);
        H5Tset_size(mem_type_id, data->specs->stringlength);
        H5Tset_strpad(mem_type_id, H5T_STR_NULLTERM);

    }

    err = utils_hdf5_write_dataset(data->dtset_id, data->xfer_id, buf, mem_type_id, start_ptr, count_ptr, stride_ptr);

    return ESCDF_SUCCESS;
}



/**********************************************************************************************/
/**********************************************************************************************/
/**********************************************************************************************/


size_t escdf_dataset_sizeof(const escdf_dataset_t* data)
{
    unsigned int ii;
    size_t len;

    assert(data != NULL);

    len = 1;
    for (ii = 0; ii < data->specs->ndims; ii++)
        len *= data->dims[ii];

    return len * escdf_dataset_specs_sizeof(data->specs);
}


hid_t escdf_dataset_get_id(const escdf_dataset_t *data)
{
    assert(data!=NULL);
    return data->specs->id;
}

hid_t escdf_dataset_get_dtset_id(const escdf_dataset_t *data)
{
    assert(data!=NULL);
    return data->dtset_id;
}

hid_t escdf_dataset_get_type_id(const escdf_dataset_t *data){
    assert(data!=NULL);
    return data->type_id;
}


const char * escdf_dataset_get_name(const escdf_dataset_t *data)
{
    assert(data!=NULL);
    return data->specs->name;
}

escdf_errno_t escdf_dataset_print(const escdf_dataset_t *data)
{
    unsigned int i, j;
    escdf_errno_t error;

    char datatype_name[20], isSet[6];

    unsigned int *dims;
    unsigned int *dims_from_specs;

    if( data == NULL ) {

        printf("Dataset not defined! \n"); fflush(stdout);
        return ESCDF_ERROR;

    } else {

        if( data->specs == NULL ) {

        printf("Dataset Specs not defined! \n"); fflush(stdout);
        return ESCDF_ERROR;
      
        } else {
            switch(data->specs->datatype) {
            case ESCDF_DT_NONE:
                strcpy(datatype_name, "ESCDF_DT_NONE");
                break;
            case ESCDF_DT_UINT:
                strcpy(datatype_name, "ESCDF_DT_UINT");
                break;
            case ESCDF_DT_INT:
                strcpy(datatype_name, "ESCDF_DT_INT");
                break;
            case ESCDF_DT_BOOL:
                strcpy(datatype_name, "ESCDF_DT_BOOL");
                break;
            case ESCDF_DT_DOUBLE:
                strcpy(datatype_name, "ESCDF_DT_DOUBLE");
                break;
            case ESCDF_DT_STRING:
                strcpy(datatype_name, "ESCDF_DT_STRING");
                break;
            default :
                strcpy(datatype_name, "UNDEFINED");
                break;
            }

            printf("Dataset dump for: %s with ID = %i \n", data->specs->name, data->specs->id);
            printf("  Data type = %s (%i) \n", datatype_name, data->specs->datatype );

            printf("  Dimensions According to specs = %i \n", data->specs->ndims );
            
            for(i = 0; i < data->specs->ndims; i++) {

	            printf("  Dimensions (%i) = %s %s \n", i,  data->specs->dims_specs[i]->name, escdf_attribute_get_name(data->dims_attr[i]) );

                dims_from_specs = escdf_attribute_get_dimensions(data->dims_attr[i]);

                for(j=0; j<data->specs->dims_specs[i]->ndims; j++) {

                    printf("  Dimensions (%i): dims_from_spec[%i] = %i \n", i, j, dims_from_specs[j] );
        
                }

	            printf("  Dimensions (%i): ndims = %i ", i, data->specs->dims_specs[i]->ndims );

                dims = (unsigned int*) malloc( escdf_attribute_sizeof (data->dims_attr[i]) );
                error = escdf_attribute_get(data->dims_attr[i], dims);

                assert(error == 0);

                if(dims_from_specs) {
                    for(j=0; j<dims_from_specs[0]; j++) {

                        printf(" %i, ", dims[j]);
                    }
                }    

                free(dims);
                printf("\n");
            }

            printf("  Number of Dimensions = %i \n", data->specs->ndims );
            for(i = 0; i < data->specs->ndims; i++) {
	            printf("  Dimensions (%i) = %i \n", i, (int) data->dims[i] );

            }
            
            if(data->specs->compact) {

                    printf("Compact storage used. \n");

                    assert(data->index_array);

                    for(i=0; i<dims_from_specs[0]; i++) {
                        printf("index[%i] = %i \n", i, data->index_array[i]);
                    }
            }
            

            printf("\n");
            fflush(stdout);
        }
  }
  return ESCDF_SUCCESS;

}
