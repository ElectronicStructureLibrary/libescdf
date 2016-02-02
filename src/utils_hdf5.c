/*
  Copyright (C) 2016 D. Caliste, M. Oliveira

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <stdbool.h>
#include <hdf5.h>

#include "escdf_error.h"
#include "utils_hdf5.h"


bool utils_hdf5_check_present(hid_t loc_id, const char *name)
{
    htri_t bool_id;

    if ((bool_id = H5Lexists(loc_id, name, H5P_DEFAULT)) < 0 || !bool_id)
        return false;
    if ((bool_id = H5Oexists_by_name(loc_id, name, H5P_DEFAULT)) < 0 || !bool_id)
        return false;

    return true;
}

escdf_errno_t utils_hdf5_check_shape(hid_t dtspace_id, hsize_t *dims,
                                     unsigned int ndims)
{
    htri_t bool_id;
    int ndims_id;
    hsize_t *dims_v, *maxdims_v;
    unsigned int i;

    if ((bool_id = H5Sis_simple(dtspace_id)) < 0) {
        RETURN_WITH_ERROR(bool_id);
    }
    if (!bool_id) {
        RETURN_WITH_ERROR(ESCDF_ERROR_DIM);
    }
    if ((ndims_id = H5Sget_simple_extent_ndims(dtspace_id)) < 0) {
        RETURN_WITH_ERROR(ndims_id);
    }
    if ((unsigned int)ndims_id != ndims) {
        RETURN_WITH_ERROR(ESCDF_ERROR_DIM);
    }
    dims_v = malloc(sizeof(hsize_t) * ndims);
    maxdims_v = malloc(sizeof(hsize_t) * ndims);
    if ((ndims_id = H5Sget_simple_extent_dims(dtspace_id, dims_v, maxdims_v)) < 0) {
        DEFER_FUNC_ERROR(ndims_id);
        goto cleanup_dims;
    }
    for (i = 0; i < ndims; i++) {
        if (dims_v[i] != dims[i] || maxdims_v[i] != dims[i]) {
            DEFER_FUNC_ERROR(ESCDF_ERROR_DIM);
            goto cleanup_dims;
        }
    }
    free(dims_v);
    free(maxdims_v);
    return ESCDF_SUCCESS;

    cleanup_dims:
    free(dims_v);
    free(maxdims_v);
    return ESCDF_ERROR;
}

escdf_errno_t utils_hdf5_check_attr(hid_t loc_id, const char *name,
                                    hsize_t *dims, unsigned int ndims,
                                    hid_t *attr_pt)
{
    hid_t attr_id, dtspace_id;

    if ((attr_id = H5Aopen(loc_id, name, H5P_DEFAULT)) < 0)
        RETURN_WITH_ERROR(attr_id);

    /* Check space dimensions. */
    if ((dtspace_id = H5Aget_space(attr_id)) < 0) {
        DEFER_FUNC_ERROR(dtspace_id);
        goto cleanup_attr;
    }
    if (utils_hdf5_check_shape(dtspace_id, dims, ndims) != ESCDF_SUCCESS) {
        goto cleanup_dtspace;
    }
    H5Sclose(dtspace_id);
    if (attr_pt)
        *attr_pt = attr_id;
    else
        H5Aclose(attr_id);
    return ESCDF_SUCCESS;

    cleanup_dtspace:
    H5Sclose(dtspace_id);
    cleanup_attr:
    H5Aclose(attr_id);
    return ESCDF_ERROR;
}

escdf_errno_t utils_hdf5_check_dtset(hid_t loc_id, const char *name,
                                     hsize_t *dims, unsigned int ndims,
                                     hid_t *dtset_pt)
{
    hid_t dtset_id, dtspace_id;

    if ((dtset_id = H5Dopen(loc_id, name, H5P_DEFAULT)) < 0)
        RETURN_WITH_ERROR(dtset_id);

    /* Check space dimensions. */
    if ((dtspace_id = H5Dget_space(dtset_id)) < 0) {
        DEFER_FUNC_ERROR(dtspace_id);
        goto cleanup_dtset;
    }
    if (utils_hdf5_check_shape(dtspace_id, dims, ndims) != ESCDF_SUCCESS) {
        goto cleanup_dtspace;
    }
    H5Sclose(dtspace_id);
    if (dtset_pt)
        *dtset_pt = dtset_id;
    else
        H5Dclose(dtset_id);
    return ESCDF_SUCCESS;

    cleanup_dtspace:
    H5Sclose(dtspace_id);
    cleanup_dtset:
    H5Dclose(dtset_id);
    return ESCDF_ERROR;
}

escdf_errno_t utils_hdf5_read_attr(hid_t loc_id, const char *name,
                                   hid_t mem_type_id, hsize_t *dims,
                                   unsigned int ndims, void *buf)
{
    escdf_errno_t err;

    hid_t attr_id;
    herr_t err_id;

    if ((err = utils_hdf5_check_attr(loc_id, name, dims, ndims, &attr_id)) != ESCDF_SUCCESS) {
        return err;
    }

    err = ESCDF_SUCCESS;
    if ((err_id = H5Aread(attr_id, mem_type_id, buf)) < 0) {
        DEFER_FUNC_ERROR(err_id);
        err = ESCDF_ERROR;
    }

    H5Aclose(attr_id);
    return err;
}

escdf_errno_t utils_hdf5_read_bool(hid_t loc_id, const char *name,
                                          _bool_set_t *scalar)
{
    escdf_errno_t err;
    int value;
    hsize_t dims[1] = {1};

    if ((err = utils_hdf5_read_attr(loc_id, name, H5T_NATIVE_INT, dims, 1, &value)) != ESCDF_SUCCESS) {
        return err;
    }
    *scalar = _bool_set((bool)value);

    return ESCDF_SUCCESS;
}

escdf_errno_t utils_hdf5_read_uint(hid_t loc_id, const char *name,
                                          _uint_set_t *scalar, unsigned int range[2])
{
    escdf_errno_t err;
    int value;
    hsize_t dims[1] = {1};

    if ((err = utils_hdf5_read_attr(loc_id, name, H5T_NATIVE_INT, dims, 1, &value)) != ESCDF_SUCCESS) {
        return err;
    }
    if ((unsigned int)value < range[0] || (unsigned int)value > range[1]) {
        RETURN_WITH_ERROR(ESCDF_ERANGE);
    }
    *scalar = _uint_set((unsigned int)value);

    return ESCDF_SUCCESS;
}

escdf_errno_t utils_hdf5_read_uint_array(hid_t loc_id, const char *name,
                                         unsigned int **array, hsize_t *dims,
                                         unsigned int ndims, unsigned int range[2])
{
    escdf_errno_t err;
    unsigned int i;
    hsize_t len;

    len = 1;
    for (i = 0; i < ndims; i++) {
        len *= dims[i];
    }
    *array = malloc(sizeof(unsigned int) * len);

    if ((err = utils_hdf5_read_attr(loc_id, name, H5T_NATIVE_INT, dims, ndims,
                                    (void*)*array)) != ESCDF_SUCCESS) {
        free(*array);
        return err;
    }
    for (i = 0; i < len; i++) {
        if ((*array)[i] < range[0] || (*array)[i] > range[1]) {
            free(*array);
            RETURN_WITH_ERROR(ESCDF_ERANGE);
        }
    }
    return ESCDF_SUCCESS;
}

escdf_errno_t utils_hdf5_read_dbl_array(hid_t loc_id, const char *name,
                                        double **array, hsize_t *dims,
                                        unsigned int ndims, double range[2])
{
    escdf_errno_t err;
    unsigned int i;
    hsize_t len;

    len = 1;
    for (i = 0; i < ndims; i++) {
        len *= dims[i];
    }
    *array = malloc(sizeof(double) * len);

    if ((err = utils_hdf5_read_attr(loc_id, name, H5T_NATIVE_DOUBLE, dims, ndims,
                                    (void*)*array)) != ESCDF_SUCCESS) {
        free(*array);
        return err;
    }
    for (i = 0; i < len; i++) {
        if ((*array)[i] < range[0] || (*array)[i] > range[1]) {
            free(*array);
            RETURN_WITH_ERROR(ESCDF_ERANGE);
        }
    }
    return ESCDF_SUCCESS;
}

escdf_errno_t utils_hdf5_create_dataset(hid_t loc_id, const char *name,
                                        hid_t type_id, hsize_t *dims,
                                        unsigned int ndims, hid_t *dtset_pt)
{
    hid_t dtset_id, dtspace_id;

    /* Create space dimensions. */
    if ((dtspace_id = H5Screate_simple(ndims, dims, NULL)) < 0) {
        RETURN_WITH_ERROR(dtspace_id);
    }

    if ((dtset_id = H5Dcreate(loc_id, name, type_id, dtspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0) {
        DEFER_FUNC_ERROR(dtset_id);
        goto cleanup_dtspace;
    }

    H5Sclose(dtspace_id);
    if (dtset_pt)
        *dtset_pt = dtset_id;
    else
        H5Dclose(dtset_id);
    return ESCDF_SUCCESS;

    cleanup_dtspace:
    H5Sclose(dtset_id);
    return ESCDF_ERROR;
}

escdf_errno_t utils_hdf5_create_attr(hid_t loc_id, const char *name,
                                     hid_t type_id, hsize_t *dims,
                                     unsigned int ndims, hid_t *attr_pt)
{
    hid_t attr_id, dtspace_id;

    /* Create space dimensions. */
    if ((dtspace_id = H5Screate_simple(ndims, dims, NULL)) < 0) {
        RETURN_WITH_ERROR(dtspace_id);
    }

    if ((attr_id = H5Acreate(loc_id, name, type_id, dtspace_id, H5P_DEFAULT, H5P_DEFAULT)) < 0) {
        DEFER_FUNC_ERROR(attr_id);
        goto cleanup_dtspace;
    }

    H5Sclose(dtspace_id);
    if (attr_pt)
        *attr_pt = attr_id;
    else
        H5Aclose(attr_id);
    return ESCDF_SUCCESS;

    cleanup_dtspace:
    H5Sclose(attr_id);
    return ESCDF_ERROR;
}

escdf_errno_t utils_hdf5_write_attr(hid_t loc_id, const char *name,
                                    hid_t disk_type_id, hsize_t *dims,
                                    unsigned int ndims, hid_t mem_type_id,
                                    const void *buf)
{
    escdf_errno_t err;

    hid_t attr_id;
    herr_t err_id;

    if ((err = utils_hdf5_create_attr(loc_id, name, disk_type_id, dims, ndims, &attr_id)) != ESCDF_SUCCESS) {
        return err;
    }

    err = ESCDF_SUCCESS;
    if ((err_id = H5Awrite(attr_id, mem_type_id, buf)) < 0) {
        DEFER_FUNC_ERROR(err_id);
        err = ESCDF_ERROR;
    }

    H5Aclose(attr_id);
    return err;
}

