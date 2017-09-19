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


size_t _attribute_specs_sizeof(const _attribute_specs_t *specs)
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
            return sizeof(char);
        default:
            return 0;
    }
}

hid_t _attribute_specs_hdf5_disk_type(const _attribute_specs_t *specs)
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

hid_t _attribute_specs_hdf5_mem_type(const _attribute_specs_t *specs)
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

bool _attribute_specs_is_present(const _attribute_specs_t *specs, hid_t loc_id)
{
    assert(specs != NULL);

    return utils_hdf5_check_present_attr(loc_id, specs->name);
}



_attribute_t * _attribute_new(const _attribute_specs_t *specs, _attribute_t **attr_dims)
{
    _attribute_t *attr = NULL;
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
    attr = malloc(sizeof(_attribute_t));
    if (attr == NULL)
        return attr;
    attr->specs = specs;
    attr->is_set = false;
    attr->buf = NULL;
    attr->dims = NULL;

    /* Get the size of the attribute and allocate buffer memory */
    if (attr->specs->ndims > 0) {
        attr->dims = malloc(attr->specs->ndims * sizeof(hsize_t));
        if (attr->dims == NULL) {
            free(attr);
            return NULL;
        }
    }
    len = 1;
    for (ii = 0; ii < attr->specs->ndims; ii++) {
        dim = (unsigned int *) (attr_dims[ii]->buf);
        len *= *dim;
        attr->dims[ii] = *dim;
    }
    attr->buf = malloc(len * _attribute_specs_sizeof(attr->specs));
    if (attr->buf == NULL) {
        free(attr->dims);
        free(attr);
        attr = NULL;
    }

    return attr;
}

void _attribute_free(_attribute_t *attr)
{
    if (attr != NULL) {
        free(attr->dims);
        free(attr->buf);
    }
    free(attr);
}


escdf_errno_t _attribute_set(_attribute_t *attr, void *buf)
{
    assert(attr != NULL);

    memcpy(attr->buf, buf, _attribute_sizeof(attr));
    attr->is_set = true;

    return ESCDF_SUCCESS;
}


escdf_errno_t _attribute_get(_attribute_t *attr, void *buf)
{
    assert(attr != NULL);
    assert(attr->is_set);

    memcpy(buf, attr->buf, _attribute_sizeof(attr));

    return ESCDF_SUCCESS;
}


escdf_errno_t _attribute_read(_attribute_t *attr, hid_t loc_id)
{
    escdf_errno_t err;
    _bool_set_t tmpb;
    char *tmpc;

    assert(attr != NULL);
    assert(_attribute_specs_is_present(attr->specs, loc_id));

    switch (attr->specs->datatype) {
        case ESCDF_DT_BOOL:
            err = utils_hdf5_read_bool(loc_id, attr->specs->name, &tmpb );
            *((bool *)attr->buf) = tmpb.value;
            break;
        case ESCDF_DT_STRING:
            tmpc = (char *)attr->buf;
            err = utils_hdf5_read_string(loc_id, attr->specs->name, &tmpc, attr->dims[0]);
            break;
        default:
            err = utils_hdf5_read_attr(loc_id, attr->specs->name, _attribute_specs_hdf5_mem_type(attr->specs), attr->dims,
                                       attr->specs->ndims, attr->buf);
    }
    attr->is_set = err == ESCDF_SUCCESS;

    return err;
}


escdf_errno_t _attribute_write(_attribute_t *attr, hid_t loc_id)
{
    escdf_errno_t err;

    assert(attr != NULL);
    assert(attr->is_set);

    switch (attr->specs->datatype) {
        case ESCDF_DT_BOOL:
            err = utils_hdf5_write_bool(loc_id, attr->specs->name, *((bool *)attr->buf) );
            break;
        case ESCDF_DT_STRING:
            err = utils_hdf5_write_string(loc_id, attr->specs->name, (char *)attr->buf, attr->dims[0]);
            break;
        default:
            err = utils_hdf5_write_attr(loc_id, attr->specs->name, _attribute_specs_hdf5_disk_type(attr->specs),
                                        attr->dims, attr->specs->ndims, _attribute_specs_hdf5_mem_type(attr->specs),
                                        attr->buf);
    }

    return err;
}

size_t _attribute_sizeof(const _attribute_t *attr)
{
    unsigned int ii;
    size_t len;

    assert(attr != NULL);

    len = 1;
    for (ii = 0; ii < attr->specs->ndims; ii++)
        len *= attr->dims[ii];

    return len * _attribute_specs_sizeof(attr->specs);
}
