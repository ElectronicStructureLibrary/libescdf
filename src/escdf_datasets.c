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
    size_t *dims;

    /**
     * @brief dims_attr is an array of pointers to the attributes defining the dimensions of the dataset
     */
    escdf_attribute_t **dims_attr;

    /* In case of irregular arrays (only two-dimensional allowed) the data is stored
     * as one dimensional array, and we need the additional index array to map to the array.
     * 
     * Whether or not this compact storage is used, is determined by compact.
     */
    size_t *index_array; /* only used for compact storage */

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

bool escdf_dataset_specs_is_present(const escdf_dataset_specs_t *specs, hid_t loc_id)
{
    assert(specs != NULL);

    return utils_hdf5_check_present(loc_id, specs->name);
}

bool escdf_dataset_specs_disordered_storage_allowed(const escdf_dataset_specs_t *specs)
{
    assert(specs != NULL);
 
    return specs->disordered_storage_allowed;
}

bool escdf_dataset_specs_is_compact(const escdf_dataset_specs_t *specs) {

    assert(specs != NULL);

    return specs->compact;
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


#ifdef DEBUG
    printf("escdf_dataset_new: start.\n");
#endif

     /* Check input */
    assert(specs != NULL);
    assert(attr_dims != NULL);
   
    /* Allocate memory */ 
    data = (escdf_dataset_t *) malloc(sizeof(escdf_dataset_t));
    
    if (data == NULL) {
        return data;
    }

    
    /* set default values */

    /* Check whether dimensions are regular shaped */
    if (specs->compact) {
        if (specs->ndims == 2) {

            /*
             * Store the data as one dimensional array with an additional indexing array.
             */
            ndims_effective = 1;
            error = escdf_attribute_get(attr_dims[0], &dims0);

            dims =  (unsigned int*) malloc(ndims_effective * sizeof(unsigned int));
            dims1 = (unsigned int*) malloc(escdf_attribute_sizeof(attr_dims[1])); 

            error = escdf_attribute_get(attr_dims[1], dims1);

            data->index_array = (size_t*) malloc((dims0 * sizeof(size_t)));

            for (ii=0, j=0; ii<dims0; ii++) {
                data->index_array[ii] = j;
                j += dims1[ii];
            }
            dims[0] = j;
            /* dims[1] = 0; // was 1! */ 
        } else { 
            REGISTER_ERROR(ESCDF_ERROR_DIM); 

            return NULL;
        } 

    } else {
        ndims_effective = specs->ndims;

        if (ndims_effective > 0) {
            dims = (unsigned int*) malloc(ndims_effective * sizeof(unsigned int));

            assert(attr_dims != NULL);
            for (ii = 0; ii < specs->ndims; ii++) {
                assert(escdf_attribute_is_set(attr_dims[ii]));
                assert(specs->dims_specs[ii]->datatype == ESCDF_DT_UINT);

                if (specs->dims_specs[ii]->ndims) {
                    size_t attr_size = escdf_attribute_sizeof(attr_dims[ii]);
                    unsigned int *at_dims = (unsigned int*) malloc(attr_size);
                    SUCCEED_OR_BREAK(escdf_attribute_get(attr_dims[ii], at_dims));
                    /* In the 1D attribute case, we currently implement only
                       the product of values, but later on we may need to
                       introduce more reductions like summation or maximum. */
                    dims[ii] = 1;
                    for (j = 0; j < attr_size / sizeof(unsigned int); j++)
                        dims[ii] *= at_dims[j];
                    free(at_dims);
                } else {
                    SUCCEED_OR_BREAK(escdf_attribute_get(attr_dims[ii], &(dims[ii])));
                }
            }
        }

    }

    data->specs = specs;
    data->ndims_effective = ndims_effective;
    data->dims = (size_t*) malloc(ndims_effective * sizeof(size_t));

    assert(data->dims != NULL);

    for (ii = 0; ii<ndims_effective; ii++) {
        data->dims[ii] = dims[ii];  
    }
    
    free(dims);

    data->dims_attr = (escdf_attribute_t**) malloc(data->specs->ndims * sizeof(escdf_attribute_t*));

    for (ii=0; ii<data->specs->ndims; ii++) {
        data->dims_attr[ii] = attr_dims[ii];
    }    

    data->dtset_id = ESCDF_UNDEFINED_ID;
    data->is_ordered = true;
    data->transfer = NULL;
    data->transfer_on_disk = false;

    data->type_id = utils_hdf5_disk_type(specs->datatype);

    if (data->type_id == H5T_C_S1) {
        data->type_id = H5Tcopy(H5T_C_S1);
        assert(H5Tset_size(data->type_id, (size_t) data->specs->stringlength)>=0);
        assert(H5Tset_strpad(data->type_id, H5T_STR_NULLTERM)>=0);
    }

    /* QUESTION: Where do we define the xfer_id? */
    data->xfer_id = ESCDF_UNDEFINED_ID;
   
#ifdef DEBUG
    printf("escdf_dataset_new: end.\n");
#endif

    return data;
}

void escdf_dataset_free(escdf_dataset_t *data)
{
    if (data != NULL) {
        free(data->dims);
        free(data);
    }
}




const size_t * escdf_dataset_get_dimensions(const escdf_dataset_t *data)
{
    /* this routine assumes regular dimensions of the dataset */
    assert(data != NULL);

    return data->dims;
}

bool escdf_dataset_is_ordered(const escdf_dataset_t *data)
{
    assert(data != NULL);

    return data->is_ordered;
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
    assert(data != NULL);

    if(!data->specs->disordered_storage_allowed)
        RETURN_WITH_ERROR(ESCDF_ERROR);

    data->transfer = transfer;

    data->is_ordered = false;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_dataset_create(escdf_dataset_t *data, hid_t loc_id)
{
    int transfer_id; /**< Handle of the data transfer (re-ordering) table */

    assert(data != NULL);

    if (data->dtset_id == ESCDF_UNDEFINED_ID) {
	SUCCEED_OR_RETURN(utils_hdf5_create_dataset(loc_id, data->specs->name, 
						    data->type_id, data->dims, data->ndims_effective, &data->dtset_id));
    } else {
        RETURN_WITH_ERROR(ESCDF_ERROR);
        /* alternatively we could close and reopen the dataset? */
    }

    SUCCEED_OR_RETURN(utils_hdf5_write_attr_bool(data->dtset_id, "is_ordered", NULL, 0, &(data->is_ordered)));

    /* Flag error if data is not ordered but there is no reordering table */
    if (!data->is_ordered || data->transfer != NULL) 
        RETURN_WITH_ERROR(ESCDF_ERROR);

    /* write reordering table as dataset within the dataset. Write even is data is ordered (?) */
    if (data->transfer != NULL) {
        transfer_id = escdf_datatransfer_get_id(data->transfer);
    } else {
        transfer_id = ESCDF_UNDEFINED_ID;
    }
	
    SUCCEED_OR_RETURN(utils_hdf5_write_attr(data->dtset_id, "transfer", H5T_NATIVE_INT, NULL, 0, H5T_NATIVE_INT, &transfer_id));

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_dataset_open(escdf_dataset_t *data, hid_t loc_id)
{
    _bool_set_t tmp_bool;
    hid_t dtset_pt;

    assert(data != NULL);

    if (data->dtset_id == ESCDF_UNDEFINED_ID ) {
        if (!utils_hdf5_check_present(loc_id, data->specs->name)) {
            RETURN_WITH_ERROR(ESCDF_ERROR);
        }

        SUCCEED_OR_RETURN(utils_hdf5_open_dataset(loc_id, data->specs->name, &(dtset_pt)));
    
        data->dtset_id = dtset_pt;
    } else {
        /* Should we return with error, if the dataset is already open? */
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    SUCCEED_OR_RETURN(utils_hdf5_read_attr_bool(data->dtset_id, "is_ordered", NULL, 0, &tmp_bool));
    data->is_ordered = tmp_bool.value;
    data->ordered_flag_set = tmp_bool.is_set;

    /* check whether a reordering table is present in the file */
    if (!utils_hdf5_check_present_attr(data->dtset_id, "transfer") && !data->is_ordered) {
	RETURN_WITH_ERROR(ESCDF_ERROR);
    }
    
    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_dataset_close(escdf_dataset_t *data)
{
    assert(data != NULL);

    if (data->transfer != NULL && data->transfer_on_disk) {
        /* TODO: need to write transfer table */
    }

    /* close dataset on disk */    
    FULFILL_OR_RETURN(utils_hdf5_close_dataset(data->dtset_id)==ESCDF_SUCCESS, ESCDF_ERROR);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_dataset_read(const escdf_dataset_t *data, const size_t *start, const size_t *count, const size_t *stride, void *buf)
{
    hid_t mem_type_id;
    bool compact;
    size_t start_compact[1];
    size_t count_compact[1];
    size_t stride_compact[1];
    const size_t *start_ptr, *count_ptr, *stride_ptr;

    assert(data != NULL);
    assert(buf != NULL);

    /* check that the dataset in the file is open */
    if (data->dtset_id == ESCDF_UNDEFINED_ID) {
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    /* check whether we need to re-order on read */

    /* QUESTION: If is_ordered = false and no reordering present, shall we fail or use normal order ? */

    if (!data->is_ordered && data->transfer == NULL) {
	RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    compact = escdf_dataset_specs_is_compact(data->specs);

    /* we need to re-shape the data in case of comact storage */
    if (compact) {
        start_compact[0] = data->index_array[start[0]];
        count_compact[0] = count[1];    
        stride_compact[0] = 1;

        start_ptr = start_compact;
        count_ptr = count_compact;
        stride_ptr = stride_compact;

    }  else {
        start_ptr = start;
        count_ptr = count;
        stride_ptr = stride;
    }

    mem_type_id = utils_hdf5_mem_type(data->specs->datatype);

    if (mem_type_id == H5T_C_S1) {
        mem_type_id = H5Tcopy(H5T_C_S1);
        H5Tset_size(mem_type_id, data->specs->stringlength);
        H5Tset_strpad(mem_type_id, H5T_STR_NULLTERM);
    }

    utils_hdf5_read_dataset(data->dtset_id, data->xfer_id, buf, mem_type_id, start_ptr, count_ptr, stride_ptr);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_dataset_read_simple(const escdf_dataset_t *data, void *buf)
{
    unsigned int i;
    hid_t mem_type_id;
    size_t *start, *count, *stride;

    assert(data != NULL);
    assert(buf != NULL);

    /* check whether we need to re-order on read */

    /* QUESTION: If is_ordered = false and no reordering present, shall we fail or use normal order ? */

    if (!data->is_ordered && data->transfer == NULL) {
	RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    mem_type_id = utils_hdf5_mem_type(data->specs->datatype);

    if (mem_type_id == H5T_C_S1) {
        mem_type_id = H5Tcopy(H5T_C_S1);
        H5Tset_size(mem_type_id, data->specs->stringlength);
        H5Tset_strpad(mem_type_id, H5T_STR_NULLTERM);
    }

    start = (size_t *) malloc(data->specs->ndims * sizeof(size_t));
    count = (size_t *) malloc(data->specs->ndims * sizeof(size_t));
    stride = (size_t *) malloc(data->specs->ndims * sizeof(size_t));
    
    for (i=0; i<data->specs->ndims; i++) {
        start[i] = 0;
        count[i] = data->dims[i];
        stride[i] = 1;
    }
    
    utils_hdf5_read_dataset(data->dtset_id, data->xfer_id, buf, mem_type_id, start, count, stride);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_dataset_write_simple(escdf_dataset_t *data, const void *buf)
{
    unsigned int i;
    hid_t mem_type_id;
    size_t *start, *count, *stride;

    assert(data != NULL);

    /* check that the dataset in the file is open */
    if (data->dtset_id == ESCDF_UNDEFINED_ID) {
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    if (!data->is_ordered) {
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    start = (size_t *) malloc(data->specs->ndims * sizeof(unsigned int));
    count = (size_t *) malloc(data->specs->ndims * sizeof(unsigned int));
    stride = (size_t *) malloc(data->specs->ndims * sizeof(unsigned int));

    if (data->specs->compact) {
        /* The question here is what data structure we expect in *buf? */
        /*
         * In order to correctly address (and remap) a part of the original array
         * we need to know how the array is stored, and in particular the size of each element in memory 
         *
         **/
    } else {    
	for(i=0; i<data->specs->ndims; i++) {
    	    start[i] = 0;
    	    count[i] = data->dims[i];
    	    stride[i] = 1;
    	}
        fflush(stdout);

    	mem_type_id = utils_hdf5_mem_type(data->specs->datatype);

    	if (mem_type_id == H5T_C_S1) {
    	    mem_type_id = H5Tcopy(H5T_C_S1);
    	    H5Tset_size(mem_type_id, data->specs->stringlength);
    	    H5Tset_strpad(mem_type_id, H5T_STR_NULLTERM);
    	}

	utils_hdf5_write_dataset(data->dtset_id, data->xfer_id, buf, mem_type_id, start, count, stride);
    }

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_dataset_write(const escdf_dataset_t *data, const size_t *start, const size_t *count, const size_t *stride, const void *buf)
{
    escdf_errno_t err;
    hid_t mem_type_id;
    bool compact;
    size_t start_compact[1];
    size_t count_compact[1];
    size_t stride_compact[1];
    const size_t *start_ptr, *count_ptr, *stride_ptr;

    assert(data != NULL);

    if (data->dtset_id == ESCDF_UNDEFINED_ID) {
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    if (!data->is_ordered) {
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }

    compact = escdf_dataset_specs_is_compact(data->specs);

    /* we need to re-shape the data in case of comact storage */
    if (compact) {
        start_compact[0] = data->index_array[start[0]];
        count_compact[0] = count[1];
        stride_compact[0] = 1;

        start_ptr = start_compact;
        count_ptr = count_compact;
        stride_ptr = stride_compact;
    } else {
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

    char datatype_name[20];

    size_t *dims;
    const size_t *dims_from_specs;

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
            
            for (i = 0; i < data->specs->ndims; i++) {

		printf("  Dimensions (%i) = %s \n", i,  data->specs->dims_specs[i]->name );

                dims_from_specs = escdf_attribute_get_dimensions(data->dims_attr[i]);

                for (j=0; j<data->specs->dims_specs[i]->ndims; j++) {
                    printf("  Dimensions (%u): dims_from_spec[%u] = %lu \n", i, j, dims_from_specs[j] );       
                }

		printf("  Dimensions (%i): ndims = %i ", i, data->specs->dims_specs[i]->ndims );

                dims = (size_t*) malloc( escdf_attribute_sizeof (data->dims_attr[i]) );
                error = escdf_attribute_get(data->dims_attr[i], dims);

                assert(error == 0);

                if (dims_from_specs) {
                    for (j=0; j<dims_from_specs[0]; j++) {
                        printf(" %lu, ", dims[j]);
                    }
                }

                free(dims);
                printf("\n");
            }

            printf("  Number of Dimensions = %i \n", data->specs->ndims );
            for (i = 0; i < data->specs->ndims; i++) {
		printf("  Dimensions (%i) = %i \n", i, (int) data->dims[i] );
            }
            
            if (data->specs->compact) {
		printf("Compact storage used. \n");

		assert(data->index_array);

		for (i=0; i<dims_from_specs[0]; i++) {
		    printf("index[%i] = %lu \n", i, data->index_array[i]);
		}
            }
            

            printf("\n");
            fflush(stdout);
        }
    }
    return ESCDF_SUCCESS;
}
