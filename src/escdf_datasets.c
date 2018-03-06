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

struct escdf_dataset {

    const escdf_dataset_specs_t *specs;

    /* bool is_set; */
    bool is_ordered;
    bool ordered_flag_set;

    hsize_t *dims;
    /**
     * It seems easier to only keep the dataset_ID of the reordering table.
     * This eliminates the need to search for the escdf_dataset by name or ID.
     * There is no need to keep a full dataset structure for the reordering tables, as they themselves
     * cannot be disordered and always inherit the dimension from the dataset to which they are attached.
     * 
     * escdf_dataset_t *reordering_table;
     */

    hid_t reordering_table_id;
    
    hid_t type_id;
    hid_t xfer_id;

    hid_t dtset_id;

/*    hid_t *dtset_ptr; */

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



hsize_t * escdf_dataset_get_dimensions(const escdf_dataset_t *data)
{
    assert(data != NULL);

    return data->dims;
}

/* not strictly necessary as specs are public */

bool escdf_dataset_is_disordered_storage_allowed(const escdf_dataset_t *data)
{
    assert( data != NULL );

    return data->specs->disordered_storage_allowed;
}

bool escdf_dataset_is_ordered(const escdf_dataset_t *data)
{
    assert( data != NULL );

    return data->is_ordered;
}

escdf_errno_t escdf_dataset_set_ordered(escdf_dataset_t *data, bool ordered)
{
    if( !data->specs->disordered_storage_allowed && !ordered ) 
        RETURN_WITH_ERROR(ESCDF_ERROR);

    data->is_ordered = ordered;

    return ESCDF_SUCCESS;
}


escdf_dataset_t * escdf_dataset_get_reordering_table(const escdf_dataset_t * data)
{
    assert( data != NULL );

    return data->reordering_table;
}

escdf_errno_t escdf_dataset_set_reordering_table(escdf_dataset_t *data, escdf_dataset_t *table)
{
    assert(data != NULL);

    if(!data->specs->disordered_storage_allowed)
        RETURN_WITH_ERROR(ESCDF_ERROR);

    data->reordering_table = table;
    data->is_ordered = false;

    return ESCDF_SUCCESS;
}


escdf_dataset_t * escdf_dataset_new(const escdf_dataset_specs_t *specs, escdf_attribute_t **attr_dims)
{
    escdf_dataset_t *data = NULL;
    unsigned int ii, ndims;
    unsigned int *dims;
    size_t len;

 
    /* Check input */
    assert(specs != NULL);

    ndims = specs->ndims; 

    /* Check dimensions */

    dims = malloc(ndims * sizeof(unsigned int));
    assert(dims != NULL);

    if (ndims > 0) {
        assert(attr_dims != NULL);
        for (ii = 0; ii < ndims; ii++) {
            assert(escdf_attribute_is_set(attr_dims[ii]));
            assert(escdf_attribute_get_specs_id(attr_dims[ii]) == specs->dims_specs[ii]->id);
            assert(specs->dims_specs[ii]->datatype == ESCDF_DT_UINT);

            /* Shall we set the dimensions here? */

            SUCCEED_OR_BREAK(escdf_attribute_get(attr_dims[ii], &(dims[ii])));

        }
    }

    /* Allocate memory and set default values */
    data = (escdf_dataset_t *) malloc(sizeof(escdf_dataset_t));
    if (data == NULL)
        return data;

    data->specs = specs;

    data->dims = malloc(ndims * sizeof(hsize_t));
    assert (data->dims != NULL);

    for(ii = 0; ii<ndims; ii++) 
        data->dims[ii] = dims[ii];  
    
    free(dims);

    data->dtset_id = 0;
    data->is_ordered = true;
    data->reordering_table = NULL;
    data->type_id = escdf_dataset_specs_hdf5_disk_type(specs);

    return data;
}

escdf_errno_t escdf_dataset_create(escdf_dataset_t *data, hid_t loc_id)
{

    assert(data != NULL);

    if(data->dtset_id == ESCDF_UNDEFINED_ID ) {
        SUCCEED_OR_RETURN( utils_hdf5_create_dataset(loc_id, data->specs->name, data->type_id, data->dims, 
                                        data->specs->ndims, &data->dtset_id) );
    }
    else {
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }
    

    SUCCEED_OR_RETURN( utils_hdf5_write_bool(data->dtset_id, "is_ordered", data->is_ordered) );


    if(data->reordering_table != NULL) {
        SUCCEED_OR_RETURN( utils_hdf5_write_string( data->dtset_id, "reordering_table", 
                            data->reordering_table->specs->name, 80) );   
    }
    else {
        SUCCEED_OR_RETURN( utils_hdf5_write_string( data->dtset_id, "reordering_table", "none", 80) );   
    }
    
    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_dataset_open(escdf_dataset_t *data, hid_t loc_id)
{
    _bool_set_t tmp_bool;

    char **table_name;

    assert(data != NULL);

    if(data->dtset_id == ESCDF_UNDEFINED_ID ) {
        SUCCEED_OR_RETURN( utils_hdf5_check_present(loc_id, data->specs->name));

        /* Do we bother about access property lists? If so, change to H5DOpen2() */
        data->dtset_id = H5Dopen1(loc_id, data->specs->name);
    }
    else {
        RETURN_WITH_ERROR(ESCDF_ERROR);
    }
 
    SUCCEED_OR_RETURN( utils_hdf5_read_bool(data->dtset_id, "is_ordered", &tmp_bool ) );

    data->is_ordered = tmp_bool.value;
    data->ordered_flag_set = tmp_bool.is_set;

    SUCCEED_OR_RETURN( utils_hdf5_read_string(data->dtset_id, "reordering_table", table_name, 80) );

    if( strcmp(*table_name, "none") ) {
        data->reordering_table = NULL;
        if(!data->is_ordered)
            RETURN_WITH_ERROR(ESCDF_ERROR);
    }
    else {
        /* we need to search for the dataset named *table_name. */

        data->reordering_table = NULL;
        H5Dopen1(loc_id, *table_name);

    }

    return ESCDF_SUCCESS;
}


void escdf_dataset_free(escdf_dataset_t *data)
{
    if (data != NULL) 
        free(data->dims);
    
    free(data);
}




escdf_errno_t escdf_dataset_read(escdf_dataset_t *data, hid_t loc_id, void *buf)
{
    escdf_errno_t err;
    _bool_set_t tmpb;

    hid_t mem_type_id;
    hsize_t num_points;
    hsize_t start, count, stride;

    char *tmpc;

    assert(data != NULL);
    assert(escdf_dataset_specs_is_present(data->specs, loc_id));

    

    /* check that the buffer exists. (Unfortunately, we can't check that it is big enough) */

    assert(buf != NULL);


    utils_hdf5_read_dataset(data->dtset_id, data->xfer_id, buf, mem_type_id, &start, &count, &stride);

    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_dataset_write(escdf_dataset_t *data, hid_t loc_id, void *buf)
{
    escdf_errno_t err;
    herr_t h5err;

    unsigned int i, len=1;
    hid_t type_id;
    hid_t *attr_ptr;



    assert(data != NULL);

    /* Only here, we create the actual dataset in the file (if necessary) */


    return ESCDF_SUCCESS;
}

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


