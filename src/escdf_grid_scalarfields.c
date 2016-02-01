/*
  Copyright (C) 2016 D. Caliste, F. Corsetti, M. Oliveira, Y. Pouillon, and D. Strubbe

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

#include "escdf_grid_scalarfields.h"

#include "utils.h"

#include <math.h>

typedef struct {
    /* The metadata */
    _uint_set_t number_of_physical_dimensions;
    unsigned int *dimension_types;

    double *lattice_vectors;
} _cell_t;

struct _escdf_grid_scalarfield_t {
    char *path;
    /* The metadata */
    _cell_t cell;
    unsigned int *number_of_grid_points;
    _uint_set_t number_of_components;
    _uint_set_t real_or_complex;

    /* The data */
    bool values_on_grid_is_present;
    bool grid_ordering_is_present;
};

escdf_grid_scalarfield_t* escdf_grid_scalarfield_new(const char *path)
{
    escdf_grid_scalarfield_t *scalarfield;

    scalarfield = malloc(sizeof(escdf_grid_scalarfield_t));
    if (!path || !path[0]) {
        scalarfield->path = strdup("density");
    } else {
        scalarfield->path = strdup(path);
    }
    scalarfield->cell.number_of_physical_dimensions.value = 0;
    scalarfield->cell.number_of_physical_dimensions.is_set = false;

    scalarfield->cell.dimension_types = NULL;

    scalarfield->cell.lattice_vectors = NULL;

    scalarfield->number_of_grid_points = NULL;

    scalarfield->number_of_components.value = 0;
    scalarfield->number_of_components.is_set = false;

    scalarfield->real_or_complex.value = 0;
    scalarfield->real_or_complex.is_set = false;

    scalarfield->values_on_grid_is_present = false;
    
    scalarfield->grid_ordering_is_present = false;

    return scalarfield;
}

void escdf_grid_scalarfield_free(escdf_grid_scalarfield_t *scalarfield)
{
    if (!scalarfield)
        return;

    free(scalarfield->path);
    free(scalarfield->cell.dimension_types);
    free(scalarfield->cell.lattice_vectors);
    free(scalarfield->number_of_grid_points);

    free(scalarfield);
}

static bool escdf_hdf5_check_present(hid_t loc_id, const char *name)
{
    htri_t bool_id;

    if ((bool_id = H5Lexists(loc_id, name, H5P_DEFAULT)) < 0 || !bool_id)
        return false;
    if ((bool_id = H5Oexists_by_name(loc_id, name, H5P_DEFAULT)) < 0 || !bool_id)
        return false;
    return true;
}

static escdf_errno_t escdf_hdf5_check_shape(hid_t loc_id, const char *name,
                                          hsize_t *dims, unsigned int ndims,
                                          hid_t *dtset_pt)
{
    hid_t dtset_id, dtspace_id;
    htri_t bool_id;
    int ndims_id;
    hsize_t *dims_v, *maxdims_v;
    unsigned int i;

    if ((dtset_id = H5Dopen(loc_id, name, H5P_DEFAULT)) < 0)
        RETURN_WITH_ERROR(dtset_id);
    
    /* Check space dimensions. */
    if ((dtspace_id = H5Dget_space(dtset_id)) < 0) {
        DEFER_FUNC_ERROR(dtspace_id);
        goto cleanup_dtset;
    }
    if ((bool_id = H5Sis_simple(dtspace_id)) < 0) {
        DEFER_FUNC_ERROR(bool_id);
        goto cleanup_dtspace;
    }
    if (!bool_id) {
        DEFER_FUNC_ERROR(ESCDF_ERROR_DIM);
        goto cleanup_dtspace;
    }
    if ((ndims_id = H5Sget_simple_extent_ndims(dtspace_id)) < 0) {
        DEFER_FUNC_ERROR(ndims_id);
        goto cleanup_dtspace;
    }
    if ((unsigned int)ndims_id != ndims) {
        DEFER_FUNC_ERROR(ESCDF_ERROR_DIM);
        goto cleanup_dtspace;
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
    H5Sclose(dtspace_id);
    if (dtset_pt)
        *dtset_pt = dtset_id;
    else
        H5Dclose(dtset_id);
    return ESCDF_SUCCESS;

 cleanup_dims:
    free(dims_v);
    free(maxdims_v);
 cleanup_dtspace:
    H5Sclose(dtspace_id);
 cleanup_dtset:
    H5Dclose(dtset_id);
    return ESCDF_ERROR;
}

static escdf_errno_t escdf_hdf5_readall_array(hid_t loc_id, const char *name,
                                            hid_t mem_type_id, hsize_t *dims,
                                            unsigned int ndims, void *buf)
{
    escdf_errno_t err;

    hid_t dtset_id;
    herr_t err_id;

    if ((err = escdf_hdf5_check_shape(loc_id, name, dims, ndims, &dtset_id)) != ESCDF_SUCCESS) {
        return err;
    }

    err = ESCDF_SUCCESS;
    if ((err_id = H5Dread(dtset_id, mem_type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf)) < 0) {
        DEFER_FUNC_ERROR(err_id);
        err = ESCDF_ERROR;
    }
    
    H5Dclose(dtset_id);
    return err;
}
static escdf_errno_t escdf_hdf5_read_uint(hid_t loc_id, const char *name,
                                        _uint_set_t *scalar, unsigned int range[2])
{
    escdf_errno_t err;
    int value;
    hsize_t dims[1] = {1};
    
    if ((err = escdf_hdf5_readall_array(loc_id, name, H5T_NATIVE_INT, dims, 1, &value)) != ESCDF_SUCCESS) {
        return err;
    }
    if ((unsigned int)value < range[0] || (unsigned int)value > range[1]) {
        RETURN_WITH_ERROR(ESCDF_ERROR_RANGE);
    }
    scalar->value = (unsigned int)value;
    scalar->is_set = true;

    return ESCDF_SUCCESS;
}
static escdf_errno_t escdf_hdf5_read_uint_array(hid_t loc_id, const char *name,
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

    if ((err = escdf_hdf5_readall_array(loc_id, name, H5T_NATIVE_INT, dims, ndims,
                                        (void*)*array)) != ESCDF_SUCCESS) {
        free(*array);
        return err;
    }
    for (i = 0; i < len; i++) {
        if ((*array)[i] < range[0] || (*array)[i] > range[1]) {
            free(*array);
            RETURN_WITH_ERROR(ESCDF_ERROR_RANGE);
        }
    }
    return ESCDF_SUCCESS;
}
static escdf_errno_t escdf_hdf5_read_dbl_array(hid_t loc_id, const char *name,
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
    
    if ((err = escdf_hdf5_readall_array(loc_id, name, H5T_NATIVE_DOUBLE, dims, ndims,
                                        (void*)*array)) != ESCDF_SUCCESS) {
        free(*array);
        return err;
    }
    for (i = 0; i < len; i++) {
        if ((*array)[i] < range[0] || (*array)[i] > range[1]) {
            free(*array);
            RETURN_WITH_ERROR(ESCDF_ERROR_RANGE);
        }
    }
    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_grid_scalarfield_read_metadata(escdf_grid_scalarfield_t **scalarfield,
                                                 hid_t file_id, const char *path)
{
    escdf_errno_t err;
    unsigned int i;
    unsigned int rgPhys[2] = {1, 3};
    unsigned int rgDim[2] = {0, 2};
    double rgCell[2] = {0., HUGE_VAL};
    unsigned int rgGrid[2] = {1, 1024 * 1024};
    unsigned int rgComp[2] = {1, 4};
    unsigned int rgCplx[2] = {1, 2};
    hsize_t oneDims[1];
    hsize_t lattDims[2];
    hsize_t valDims[3];
    hid_t loc_id;
    
    /* escdf_return_val_if_fails(scalarfield, ESCDF_ERROR_ARGS); */

    *scalarfield = escdf_grid_scalarfield_new(path);

    if ((loc_id = H5Gopen(file_id, path, H5P_DEFAULT)) < 0)
        RETURN_WITH_ERROR(loc_id);

    if ((err = escdf_hdf5_read_uint(loc_id, "number_of_physical_dimensions",
                                    &(*scalarfield)->cell.number_of_physical_dimensions,
                                    rgPhys)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
    
    oneDims[0] = (*scalarfield)->cell.number_of_physical_dimensions.value;
    if ((err = escdf_hdf5_read_uint_array(loc_id, "dimension_types",
                                          &(*scalarfield)->cell.dimension_types,
                                          oneDims, 1, rgDim)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }

    lattDims[0] = lattDims[1] = (*scalarfield)->cell.number_of_physical_dimensions.value;
    if ((err = escdf_hdf5_read_dbl_array(loc_id, "lattice_vectors",
                                         &(*scalarfield)->cell.lattice_vectors,
                                         lattDims, 2, rgCell)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }

    oneDims[0] = (*scalarfield)->cell.number_of_physical_dimensions.value;
    if ((err = escdf_hdf5_read_uint_array(loc_id, "number_of_grid_points",
                                          &(*scalarfield)->number_of_grid_points,
                                          oneDims, 1, rgGrid)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
        
    if ((err = escdf_hdf5_read_uint(loc_id, "number_of_components",
                                    &(*scalarfield)->number_of_components,
                                    rgComp)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
    
    if ((err = escdf_hdf5_read_uint(loc_id, "real_or_complex",
                                    &(*scalarfield)->real_or_complex,
                                    rgCplx)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }

    valDims[0] = (*scalarfield)->number_of_components.value;
    valDims[1] = 1;
    for (i = 0; i < (*scalarfield)->cell.number_of_physical_dimensions.value; i++) {
        valDims[1] *= (*scalarfield)->number_of_grid_points[i];
    }
    valDims[2] = (*scalarfield)->real_or_complex.value;
    if ((err = escdf_hdf5_check_shape(loc_id, "values_on_grid", valDims, 3, NULL)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
    (*scalarfield)->values_on_grid_is_present = true;

    if (escdf_hdf5_check_present(loc_id, "grid_ordering")) {
        if ((err = escdf_hdf5_check_shape(loc_id, "grid_ordering", valDims + 1, 1, NULL)) != ESCDF_SUCCESS) {
            H5Gclose(loc_id);
            return err;
        }
        (*scalarfield)->grid_ordering_is_present = true;
    }

    H5Gclose(loc_id);
    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_grid_scalarfield_serialise(escdf_grid_scalarfield_t *scalarfield, FILE *f)
{
    unsigned int i, j;
    
    fprintf(f, "scalarfield:\n");
    fprintf(f, "  type: grid\n");
    fprintf(f, "  cell:\n");
    if (scalarfield->cell.number_of_physical_dimensions.is_set) {
        fprintf(f, "    number_of_physical_dimensions: %d\n",
                scalarfield->cell.number_of_physical_dimensions.value);
    }
    if (scalarfield->cell.dimension_types) {
        fprintf(f, "    dimension_types: [ %d", scalarfield->cell.dimension_types[0]);
        for (i = 1; i < scalarfield->cell.number_of_physical_dimensions.value; i++) {
            fprintf(f, ", %d", scalarfield->cell.dimension_types[i]);
        }
        fprintf(f, "]\n");
    }
    if (scalarfield->cell.lattice_vectors) {
        fprintf(f, "    lattice_vectors:\n");
        for (i = 0; i < scalarfield->cell.number_of_physical_dimensions.value; i++) {
            fprintf(f, "    - [ %g", scalarfield->cell.lattice_vectors[i * 3]);
            for (j = 1; j < scalarfield->cell.number_of_physical_dimensions.value; j++) {
                fprintf(f, ", %g", scalarfield->cell.lattice_vectors[i * 3 + j]);
            }
            fprintf(f, "]\n");
        }
    }
    if (scalarfield->number_of_grid_points) {
        fprintf(f, "  number_of_grid_points: [ %d", scalarfield->number_of_grid_points[0]);
        for (i = 1; i < scalarfield->cell.number_of_physical_dimensions.value; i++) {
            fprintf(f, ", %d", scalarfield->number_of_grid_points[i]);
        }
        fprintf(f, "]\n");
    }
    if (scalarfield->number_of_components.is_set) {
        fprintf(f, "  number_of_components: %d\n",
                scalarfield->number_of_components.value);
    }
    if (scalarfield->real_or_complex.is_set) {
        fprintf(f, "  real_or_complex: %d\n",
                scalarfield->real_or_complex.value);
    }
    if (scalarfield->values_on_grid_is_present) {
        fprintf(f, "  values_on_grid_is_present: yes\n");
    }
    if (scalarfield->grid_ordering_is_present) {
        fprintf(f, "  grid_ordering_is_present: yes\n");
    }

    return ESCDF_SUCCESS;
}
