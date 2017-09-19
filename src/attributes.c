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

#include "attributes.h"
#include "utils_hdf5.h"


size_t _attribute_sizeof(const _attribute_t *attr)
{
    assert(attr != NULL);

    switch (attr->datatype) {
        case ESCDF_DT_BOOL:
            return sizeof(bool);
        case ESCDF_DT_UINT:
            return sizeof(unsigned int);
        case ESCDF_DT_INT:
            return sizeof(int);
        case ESCDF_DT_DOUBLE:
            return sizeof(double);
        case ESCDF_DT_STRING:
            return sizeof(char);
        default:
            return 0;
    }
}

hid_t _attribute_hdf5_disk_type(const _attribute_t *attr)
{
    assert(attr != NULL);

    switch (attr->datatype) {
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

hid_t _attribute_hdf5_mem_type(const _attribute_t *attr)
{
    assert(attr != NULL);

    switch (attr->datatype) {
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

bool _attribute_is_present(const _attribute_t *attr, hid_t loc_id)
{
    assert(attr != NULL);

    return utils_hdf5_check_present_attr(loc_id, attr->name);
}



_attribute_data_t * _attribute_data_new(const _attribute_t *attr, _attribute_data_t **data_dims)
{
    _attribute_data_t *data = NULL;
    unsigned int ii, *dim;
    size_t len;

    /* Check input */
    assert(attr != NULL);
    if (attr->ndims > 0) {
        assert(data_dims != NULL);
        for (ii = 0; ii < attr->ndims; ii++) {
            assert(data_dims[ii]->is_set);
            assert(data_dims[ii]->attr->datatype == ESCDF_DT_UINT);
            assert(data_dims[ii]->attr->id == attr->dims_attr[ii]->id);
        }
    }

    /* Allocate memory and set default values */
    data = malloc(sizeof(_attribute_data_t));
    if (data == NULL)
        return data;
    data->attr = attr;
    data->is_set = false;
    data->buf = NULL;
    data->dims = NULL;

    /* Get the size of the attribute and allocate buffer memory */
    if (data->attr->ndims > 0) {
        data->dims = malloc(data->attr->ndims * sizeof(hsize_t));
        if (data->dims == NULL) {
            free(data);
            return NULL;
        }
    }
    len = 1;
    for (ii = 0; ii < data->attr->ndims; ii++) {
        dim = (unsigned int *) (data_dims[ii]->buf);
        len *= *dim;
        data->dims[ii] = *dim;
    }
    data->buf = malloc(len * _attribute_sizeof(data->attr));
    if (data->buf == NULL) {
        free(data->dims);
        free(data);
        data = NULL;
    }

    return data;
}

void _attribute_data_free(_attribute_data_t *data)
{
    if (data != NULL) {
        free(data->dims);
        free(data->buf);
    }
    free(data);
}


escdf_errno_t _attribute_data_set(_attribute_data_t *data, void *buf)
{
    assert(data != NULL);

    memcpy(data->buf, buf, _attribute_data_sizeof(data));
    data->is_set = true;

    return ESCDF_SUCCESS;
}


escdf_errno_t _attribute_data_get(_attribute_data_t *data, void *buf)
{
    assert(data != NULL);
    assert(data->is_set);

    memcpy(buf, data->buf, _attribute_data_sizeof(data));

    return ESCDF_SUCCESS;
}


escdf_errno_t _attribute_data_read(_attribute_data_t *data, hid_t loc_id)
{
    escdf_errno_t err;
    _bool_set_t tmpb;
    char *tmpc;

    assert(data != NULL);
    assert(_attribute_is_present(data->attr, loc_id));

    switch (data->attr->datatype) {
        case ESCDF_DT_BOOL:
            err = utils_hdf5_read_bool(loc_id, data->attr->name, &tmpb );
            *((bool *)data->buf) = tmpb.value;
            break;
        case ESCDF_DT_STRING:
            tmpc = (char *)data->buf;
            err = utils_hdf5_read_string(loc_id, data->attr->name, &tmpc, data->dims[0]);
            break;
        default:
            err = utils_hdf5_read_attr(loc_id, data->attr->name, _attribute_hdf5_mem_type(data->attr), data->dims,
                                       data->attr->ndims, data->buf);
    }
    data->is_set = err == ESCDF_SUCCESS;

    return err;
}


escdf_errno_t _attribute_data_write(_attribute_data_t *data, hid_t loc_id)
{
    escdf_errno_t err;

    assert(data != NULL);
    assert(data->is_set);

    switch (data->attr->datatype) {
        case ESCDF_DT_BOOL:
            err = utils_hdf5_write_bool(loc_id, data->attr->name, *((bool *)data->buf) );
            break;
        case ESCDF_DT_STRING:
            err = utils_hdf5_write_string(loc_id, data->attr->name, (char *)data->buf, data->dims[0]);
            break;
        default:
            err = utils_hdf5_write_attr(loc_id, data->attr->name, _attribute_hdf5_disk_type(data->attr),
                                        data->dims, data->attr->ndims, _attribute_hdf5_mem_type(data->attr),
                                        data->buf);
    }

    return err;
}

size_t _attribute_data_sizeof(const _attribute_data_t *data)
{
    unsigned int ii;
    size_t len;

    assert(data != NULL);

    len = 1;
    for (ii = 0; ii < data->attr->ndims; ii++)
        len *= data->dims[ii];

    return len * _attribute_sizeof(data->attr);
}
