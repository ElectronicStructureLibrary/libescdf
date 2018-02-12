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

#include "escdf_datasets.h"
#include "utils_hdf5.h"

struct escdf_dataset {
    const escdf_dataset_specs_t *specs;
    bool is_set;
    hsize_t *dims;

/*
Datasets do not hold the data directly. But we must include a handle to the HDF5 dataset !!

    void *buf;
*/

    hid_t dtset_id;
    hid_t xfer_id;

    hid_t *dtset_ptr;

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

/*
bool escdf_dataset_specs_is_present(const escdf_dataset_specs_t *specs, hid_t loc_id)
{
    assert(specs != NULL);

    return utils_hdf5_check_present_attr(loc_id, specs->name);
}
*/


escdf_dataset_t * escdf_dataset_new(const escdf_dataset_specs_t *specs, escdf_attribute_t **attr_dims)
{
    escdf_dataset_t *data = NULL;
    unsigned int ii, *dim;
    size_t len;

 
    /* Check input */
    assert(specs != NULL);
    if (specs->ndims > 0) {
        assert(attr_dims != NULL);
        for (ii = 0; ii < specs->ndims; ii++) {
            assert(attr_dims[ii]->is_set);
            assert(attr_dims[ii]->specs->datatype == ESCDF_DT_UINT);
            assert(attr_dims[ii]->specs->id == specs->dims_specs[ii]->id);
        }
    }

    /* Allocate memory and set default values */
    data = (escdf_dataset_t *) malloc(sizeof(escdf_dataset_t));
    if (data == NULL)
        return data;
    data->specs = specs;
    data->is_set = false;
    data->dims = NULL;

    /* Get the size of the attribute and allocate buffer memory */
    if (data->specs->ndims > 0) {
        data->dims = (hsize_t *) malloc(data->specs->ndims * sizeof(hsize_t));
        if (data->dims == NULL) {
            data(attr);
            return NULL;
        }
    }
    len = 1;
    for (ii = 0; ii < data->specs->ndims; ii++) {
        dim = (unsigned int *) (data_dims[ii]->buf);
        len *= *dim;
        data->dims[ii] = *dim;
    }

    return data;
}

void escdf_dataset_free(escdf_dataset_t *data)
{
    if (data != NULL) {
        free(data->dims);
    }
    free(data);
}


escdf_errno_t escdf_dataset_read(escdf_attribute_t *data, hid_t loc_id, void *buf)
{
    escdf_errno_t err;
    _bool_set_t tmpb;
    char *tmpc;

    assert(data != NULL);
    assert(escdf_dataset_specs_is_present(data->specs, loc_id));


    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_dataset_write(escdf_attribute_t *attr, hid_t loc_id)
{
    escdf_errno_t err;
    unsigned int i, len=1;

    assert(attr != NULL);
    assert(attr->is_set);


    return ESCDF_SUCCESS;
}

size_t escdf_dataset_sizeof(const escdf_attribute_t *attr)
{
    unsigned int ii;
    size_t len;

    assert(attr != NULL);

    len = 1;
    for (ii = 0; ii < attr->specs->ndims; ii++)
        len *= attr->dims[ii];

    return len * escdf_attribute_specs_sizeof(attr->specs);
}


