/* Copyright (C) 2016-2017 Damien Caliste <dcaliste@free.fr>
 *                         Micael Oliveira <micael.oliveira@mpsd.mpg.de>
 *                         Yann Pouillon <devops@materialsevolution.es>
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

#include <math.h>

#include "escdf_grid_scalarfields.h"

#include "utils.h"
#include "utils_hdf5.h"


typedef struct {
    /* The metadata */
    _uint_set_t number_of_physical_dimensions;
    int *dimension_types;

    double *lattice_vectors;
} _cell_t;

struct _escdf_grid_scalarfield_t {
    char *path;
    /* The metadata */
    _cell_t cell;
    unsigned int *number_of_grid_points;
    _uint_set_t number_of_components;
    _uint_set_t real_or_complex;
    _bool_set_t use_default_ordering;

    /* The data */
    bool values_on_grid_is_present;
    bool grid_ordering_is_present;
};

escdf_grid_scalarfield_t* escdf_grid_scalarfield_new(const char *path)
{
    escdf_grid_scalarfield_t *scalarfield;

    scalarfield = calloc(1, sizeof(escdf_grid_scalarfield_t));
    if (!path || !path[0]) {
        scalarfield->path = strdup("density");
    } else {
        scalarfield->path = strdup(path);
    }

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

escdf_errno_t escdf_grid_scalarfield_read_metadata(escdf_grid_scalarfield_t *scalarfield,
                                                   escdf_handle_t *file_id)
{
    escdf_errno_t err;
    unsigned int i;
    unsigned int rgPhys[2] = {1, 3};
    int rgDim[2] = {0, 2};
    double rgCell[2] = {0., HUGE_VAL};
    unsigned int rgGrid[2] = {1, 1024 * 1024};
    unsigned int rgComp[2] = {1, 4};
    unsigned int rgCplx[2] = {1, 2};
    hsize_t oneDims[1];
    hsize_t lattDims[2];
    hsize_t valDims[3];
    hid_t loc_id;
    
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);

    if ((loc_id = H5Gopen(file_id->group_id, scalarfield->path, H5P_DEFAULT)) < 0)
        RETURN_WITH_ERROR(loc_id);

    if ((err = utils_hdf5_read_uint(loc_id, "number_of_physical_dimensions",
                                    &scalarfield->cell.number_of_physical_dimensions,
                                    rgPhys)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
    
    oneDims[0] = scalarfield->cell.number_of_physical_dimensions.value;
    if ((err = utils_hdf5_read_int_array(loc_id, "dimension_types",
                                         &scalarfield->cell.dimension_types,
                                         oneDims, 1, rgDim)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }

    lattDims[0] = lattDims[1] = scalarfield->cell.number_of_physical_dimensions.value;
    if ((err = utils_hdf5_read_dbl_array(loc_id, "lattice_vectors",
                                         &scalarfield->cell.lattice_vectors,
                                         lattDims, 2, rgCell)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }

    oneDims[0] = scalarfield->cell.number_of_physical_dimensions.value;
    if ((err = utils_hdf5_read_uint_array(loc_id, "number_of_grid_points",
                                          &scalarfield->number_of_grid_points,
                                          oneDims, 1, rgGrid)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
        
    if ((err = utils_hdf5_read_uint(loc_id, "number_of_components",
                                    &scalarfield->number_of_components,
                                    rgComp)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
    
    if ((err = utils_hdf5_read_uint(loc_id, "real_or_complex",
                                    &scalarfield->real_or_complex,
                                    rgCplx)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }

    if ((err = utils_hdf5_read_bool(loc_id, "use_default_ordering",
                                    &scalarfield->use_default_ordering))
        != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }

    valDims[0] = scalarfield->number_of_components.value;
    valDims[1] = scalarfield->number_of_grid_points[0];
    for (i = 1; i < scalarfield->cell.number_of_physical_dimensions.value; i++) {
        valDims[1] *= scalarfield->number_of_grid_points[i];
    }
    valDims[2] = scalarfield->real_or_complex.value;
    if ((err = utils_hdf5_check_dataset(loc_id, "values_on_grid", valDims, 3, NULL)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
    scalarfield->values_on_grid_is_present = true;

    if (!scalarfield->use_default_ordering.value) {
        if ((err = utils_hdf5_check_dataset(loc_id, "grid_ordering", valDims + 1, 1, NULL)) != ESCDF_SUCCESS) {
            H5Gclose(loc_id);
            return err;
        }
        scalarfield->grid_ordering_is_present = true;
    }

    H5Gclose(loc_id);
    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_grid_scalarfield_write_metadata(const escdf_grid_scalarfield_t *scalarfield, escdf_handle_t *loc_id)
{
    hid_t gid;
    escdf_errno_t err;
    hsize_t dims[3];
    unsigned int i;
    int value;
    
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);

    /* Check all mandatory attributes. */
    FULFILL_OR_RETURN(scalarfield->cell.number_of_physical_dimensions.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->cell.dimension_types, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->cell.lattice_vectors, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->number_of_grid_points, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->number_of_components.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->real_or_complex.is_set, ESCDF_EUNINIT);

    gid = H5Gcreate(loc_id->group_id, scalarfield->path, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    FULFILL_OR_RETURN(gid >= 0, gid);

    /* Write to file. */
    if ((err = utils_hdf5_write_attr
         (gid, "number_of_physical_dimensions", H5T_STD_U32LE, NULL, 0, H5T_NATIVE_INT,
          &scalarfield->cell.number_of_physical_dimensions.value)) != ESCDF_SUCCESS) {
        H5Gclose(gid);
        return err;
    }

    dims[0] = scalarfield->cell.number_of_physical_dimensions.value;
    if ((err = utils_hdf5_write_attr
         (gid, "dimension_types", H5T_STD_I32LE, dims, 1, H5T_NATIVE_INT,
          scalarfield->cell.dimension_types)) != ESCDF_SUCCESS) {
        H5Gclose(gid);
        return err;
    }

    dims[0] = dims[1] = scalarfield->cell.number_of_physical_dimensions.value;
    if ((err = utils_hdf5_write_attr
         (gid, "lattice_vectors", H5T_IEEE_F64LE, dims, 2, H5T_NATIVE_DOUBLE,
          scalarfield->cell.lattice_vectors)) != ESCDF_SUCCESS) {
        H5Gclose(gid);
        return err;
    }

    dims[0] = scalarfield->cell.number_of_physical_dimensions.value;
    if ((err = utils_hdf5_write_attr
         (gid, "number_of_grid_points", H5T_STD_U32LE, dims, 1, H5T_NATIVE_INT,
          scalarfield->number_of_grid_points)) != ESCDF_SUCCESS) {
        H5Gclose(gid);
        return err;
    }

    if ((err = utils_hdf5_write_attr
         (gid, "number_of_components", H5T_STD_U32LE, NULL, 0, H5T_NATIVE_INT,
          &scalarfield->number_of_components.value)) != ESCDF_SUCCESS) {
        H5Gclose(gid);
        return err;
    }

    if ((err = utils_hdf5_write_attr
         (gid, "real_or_complex", H5T_STD_U32LE, NULL, 0, H5T_NATIVE_INT,
          &scalarfield->real_or_complex.value)) != ESCDF_SUCCESS) {
        H5Gclose(gid);
        return err;
    }

    if ((err = utils_hdf5_write_bool
         (gid, "use_default_ordering", scalarfield->use_default_ordering.value)) != ESCDF_SUCCESS) {
        H5Gclose(gid);
        return err;
    }

    /* Only create shapes for data. */
    dims[0] = scalarfield->number_of_components.value;
    dims[1] = scalarfield->number_of_grid_points[0];
    for (i = 1; i < scalarfield->cell.number_of_physical_dimensions.value; i++) {
        dims[1] *= scalarfield->number_of_grid_points[i];
    }
    dims[2] = scalarfield->real_or_complex.value;
    if ((err = utils_hdf5_create_dataset
         (gid, "values_on_grid", H5T_IEEE_F64LE, dims, 3, NULL)) != ESCDF_SUCCESS) {
        H5Gclose(gid);
        return err;
    }
    if (!scalarfield->use_default_ordering.value) {
        if ((err = utils_hdf5_create_dataset
             (gid, "grid_ordering", H5T_STD_U32LE, dims + 1, 1, NULL)) != ESCDF_SUCCESS) {
            H5Gclose(gid);
            return err;
        }
    }

    H5Gclose(gid);
    return ESCDF_SUCCESS;
}

/************/
/* Getters. */
/************/
unsigned int escdf_grid_scalarfield_get_number_of_physical_dimensions(const escdf_grid_scalarfield_t *scalarfield)
{
    FULFILL_OR_RETURN_VAL(scalarfield, ESCDF_EOBJECT, 0);
    FULFILL_OR_RETURN_VAL(scalarfield->cell.number_of_physical_dimensions.is_set, ESCDF_EUNINIT, 0);
    
    return scalarfield->cell.number_of_physical_dimensions.value;
}
escdf_errno_t escdf_grid_scalarfield_get_dimension_types(const escdf_grid_scalarfield_t *scalarfield,
                                                         escdf_direction_type *dimension_types,
                                                         const size_t len)
{
    unsigned i;

    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(scalarfield->cell.dimension_types, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(len == scalarfield->cell.number_of_physical_dimensions.value, ESCDF_ESIZE);

    for (i = 0; i < len; i++) {
        dimension_types[i] = (escdf_direction_type)scalarfield->cell.dimension_types[i];
    }
    return ESCDF_SUCCESS;
}
const escdf_direction_type* escdf_grid_scalarfield_ptr_dimension_types(const escdf_grid_scalarfield_t *scalarfield)
{
    FULFILL_OR_RETURN_VAL(scalarfield, ESCDF_EOBJECT, NULL);

    return (escdf_direction_type*)scalarfield->cell.dimension_types;
}
escdf_errno_t escdf_grid_scalarfield_get_lattice_vectors(const escdf_grid_scalarfield_t *scalarfield,
                                                         double *lattice_vectors,
                                                         const size_t len)
{
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(scalarfield->cell.lattice_vectors, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(len == scalarfield->cell.number_of_physical_dimensions.value *
                      scalarfield->cell.number_of_physical_dimensions.value, ESCDF_ESIZE);

    memcpy(lattice_vectors, scalarfield->cell.lattice_vectors, sizeof(double) * len);
    return ESCDF_SUCCESS;
}
const double* escdf_grid_scalarfield_ptr_lattice_vectors(const escdf_grid_scalarfield_t *scalarfield)
{
    FULFILL_OR_RETURN_VAL(scalarfield, ESCDF_EOBJECT, NULL);

    return scalarfield->cell.lattice_vectors;
}
escdf_errno_t escdf_grid_scalarfield_get_number_of_grid_points(const escdf_grid_scalarfield_t *scalarfield,
                                                               unsigned int *number_of_grid_points,
                                                               const size_t len)
{
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(scalarfield->number_of_grid_points, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(len == scalarfield->cell.number_of_physical_dimensions.value, ESCDF_ESIZE);

    memcpy(number_of_grid_points, scalarfield->number_of_grid_points, sizeof(unsigned int) * len);
    return ESCDF_SUCCESS;
}
const unsigned int* escdf_grid_scalarfield_ptr_number_of_grid_points(const escdf_grid_scalarfield_t *scalarfield)
{
    FULFILL_OR_RETURN_VAL(scalarfield, ESCDF_EOBJECT, NULL);

    return scalarfield->number_of_grid_points;
}
unsigned int escdf_grid_scalarfield_get_number_of_components(const escdf_grid_scalarfield_t *scalarfield)
{
    FULFILL_OR_RETURN_VAL(scalarfield, ESCDF_EOBJECT, 0);
    FULFILL_OR_RETURN_VAL(scalarfield->number_of_components.is_set, ESCDF_EUNINIT, 0);
    
    return scalarfield->number_of_components.value;
}
escdf_real_or_complex escdf_grid_scalarfield_get_real_or_complex(const escdf_grid_scalarfield_t *scalarfield)
{
    FULFILL_OR_RETURN_VAL(scalarfield, ESCDF_EOBJECT, ESCDF_REAL);
    FULFILL_OR_RETURN_VAL(scalarfield->real_or_complex.is_set, ESCDF_EUNINIT, ESCDF_REAL);
    
    return (escdf_real_or_complex)scalarfield->real_or_complex.value;
}
bool escdf_grid_scalarfield_get_use_default_ordering(const escdf_grid_scalarfield_t *scalarfield)
{
    FULFILL_OR_RETURN_VAL(scalarfield, ESCDF_EOBJECT, true);
    FULFILL_OR_RETURN_VAL(scalarfield->use_default_ordering.is_set, ESCDF_EUNINIT, true);
    
    return scalarfield->use_default_ordering.value;
}


/************/
/* Setters. */
/************/
escdf_errno_t escdf_grid_scalarfield_set_number_of_physical_dimensions(escdf_grid_scalarfield_t *scalarfield,
                                                                       const unsigned int number_of_physical_dimensions)
{
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(number_of_physical_dimensions > 0 &&
                      number_of_physical_dimensions < 4, ESCDF_ERANGE);

    scalarfield->cell.number_of_physical_dimensions =
        _uint_set(number_of_physical_dimensions);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_grid_scalarfield_set_dimension_types(escdf_grid_scalarfield_t *scalarfield,
                                                         const escdf_direction_type *dimension_types,
                                                         const size_t len)
{
    unsigned int i;
    
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(scalarfield->cell.number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING);
    FULFILL_OR_RETURN(len == scalarfield->cell.number_of_physical_dimensions.value, ESCDF_ESIZE);
    for (i = 0; i < len; i++) {
        FULFILL_OR_RETURN(dimension_types[i] >= ESCDF_DIRECTION_FREE &&
                          dimension_types[i] < ESCDF_N_DIRECTION_TYPE, ESCDF_ERANGE);
    }

    free(scalarfield->cell.dimension_types);
    scalarfield->cell.dimension_types = malloc(sizeof(int) * len);
    for (i = 0; i < len; i++) {
        scalarfield->cell.dimension_types[i] = (int)dimension_types[i];
    }

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_grid_scalarfield_set_lattice_vectors(escdf_grid_scalarfield_t *scalarfield,
                                                         const double *lattice_vectors,
                                                         const size_t len)
{
    unsigned int i;
    
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(scalarfield->cell.number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING);
    FULFILL_OR_RETURN(len == scalarfield->cell.number_of_physical_dimensions.value * scalarfield->cell.number_of_physical_dimensions.value, ESCDF_ESIZE);
    for (i = 0; i < len; i++) {
        FULFILL_OR_RETURN(lattice_vectors[i] >= 0., ESCDF_ERANGE);
    }

    free(scalarfield->cell.lattice_vectors);
    scalarfield->cell.lattice_vectors = malloc(sizeof(double) * len);
    memcpy(scalarfield->cell.lattice_vectors, lattice_vectors, sizeof(double) * len);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_grid_scalarfield_set_number_of_grid_points(escdf_grid_scalarfield_t *scalarfield,
                                                               const unsigned int *number_of_grid_points,
                                                               const size_t len)
{
    unsigned int i;
    
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(scalarfield->cell.number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING);
    FULFILL_OR_RETURN(len == scalarfield->cell.number_of_physical_dimensions.value, ESCDF_ESIZE);
    for (i = 0; i < len; i++) {
        FULFILL_OR_RETURN(number_of_grid_points[i] > 0, ESCDF_ERANGE);
    }

    free(scalarfield->number_of_grid_points);
    scalarfield->number_of_grid_points = malloc(sizeof(unsigned int) * len);
    memcpy(scalarfield->number_of_grid_points, number_of_grid_points, sizeof(unsigned int) * len);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_grid_scalarfield_set_number_of_components(escdf_grid_scalarfield_t *scalarfield,
                                                              const unsigned int number_of_components)
{
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(number_of_components > 0 &&
                      number_of_components < 5, ESCDF_ERANGE);

    scalarfield->number_of_components = _uint_set(number_of_components);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_grid_scalarfield_set_real_or_complex(escdf_grid_scalarfield_t *scalarfield,
                                                         const escdf_real_or_complex real_or_complex)
{
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(real_or_complex > 0 &&
                      real_or_complex < 3, ESCDF_ERANGE);

    scalarfield->real_or_complex = _uint_set((unsigned int)real_or_complex);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_grid_scalarfield_set_use_default_ordering(escdf_grid_scalarfield_t *scalarfield,
                                                              const bool use_default_ordering)
{
    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);

    scalarfield->use_default_ordering = _bool_set(use_default_ordering);

    return ESCDF_SUCCESS;
}

/*******************/
/* Data accessors. */
/*******************/
static escdf_errno_t _get_proc_grid_offset(hsize_t *my_offset,
                                           escdf_handle_t *file_id,
                                           unsigned int number_of_physical_dimensions,
                                           unsigned int *number_of_grid_points,
                                           hsize_t my_len)
{
    unsigned long long int *proclens;
    unsigned long long int len_;
    hsize_t nValues, nGridPoints;
    int i;

    proclens = malloc(sizeof(unsigned long long int) * file_id->mpi_size);
    len_ = (unsigned long long int)my_len;
    if (file_id->mpi_size == 1) {
        proclens[0] = len_;
    }
#ifdef HAVE_MPI
    else {
        MPI_Allgather(&len_, 1, MPI_UNSIGNED_LONG_LONG,
                      proclens, 1, MPI_UNSIGNED_LONG_LONG, file_id->comm);
    }
#endif

    *my_offset = 0;
    for (i = 0; i < file_id->mpi_rank; i++) {
        *my_offset += proclens[i];
    }

    /* Check that sum(proclens) == product(number_of_grid_points). */
    nValues = proclens[0];
    for (i = 1; i < file_id->mpi_size; i++) {
        nValues += proclens[i];
    }
    free(proclens);

    nGridPoints = number_of_grid_points[0];
    for (i = 1; i < number_of_physical_dimensions; i++) {
        nGridPoints *= number_of_grid_points[i];
    }

    FULFILL_OR_RETURN(nValues == nGridPoints, ESCDF_ESIZE);

    return ESCDF_SUCCESS;
}

static escdf_errno_t _get_values_on_grid(const escdf_grid_scalarfield_t *scalarfield,
                                         const hid_t loc_id, hid_t *dtset_id)
{
    hsize_t bounds[3];
    unsigned int i;

    /* Check that variable on disk is consistent with metadata in scalarfield. */
    /* Create the global distribution bounds. */
    bounds[0] = scalarfield->number_of_components.value;
    bounds[1] = scalarfield->number_of_grid_points[0];
    for (i = 1; i < scalarfield->cell.number_of_physical_dimensions.value; i++) {
        bounds[1] *= scalarfield->number_of_grid_points[i];
    }
    bounds[2] = scalarfield->real_or_complex.value;
    /* Get the dataset for this variable and check its dimensions. */
    FULFILL_OR_RETURN(utils_hdf5_check_dataset(loc_id, "values_on_grid",
                                               bounds, 3, dtset_id) == ESCDF_SUCCESS,
                      ESCDF_ERROR);
    return ESCDF_SUCCESS;
}

static escdf_errno_t _get_g2d(const escdf_grid_scalarfield_t *scalarfield,
                              hid_t loc_id, unsigned int **g2d)
{
    hsize_t len;
    unsigned int i;
    hid_t dtset_id;
    escdf_errno_t err;
    unsigned int *d2g;
    
    *g2d = NULL;

    if (scalarfield->use_default_ordering.value) {
        return ESCDF_SUCCESS;
    }

    len = scalarfield->number_of_grid_points[0];
    for (i = 1; i < scalarfield->cell.number_of_physical_dimensions.value; i++) {
        len *= scalarfield->number_of_grid_points[i];
    }

    /* Get the lookup table for this variable and check its dimensions. */
    if ((err = utils_hdf5_check_dataset(loc_id, "grid_ordering",
                                        &len, 1, &dtset_id)) != ESCDF_SUCCESS) {
        return err;
    }
    /* Actual read of all the lookup table. */
    d2g = malloc(sizeof(unsigned int) * len);
    if ((err = utils_hdf5_read_dataset(dtset_id, H5P_DEFAULT,
                                       d2g, H5T_NATIVE_UINT,
                                       NULL, NULL, NULL)) != ESCDF_SUCCESS) {
        free(d2g);
        H5Dclose(dtset_id);
        return err;
    }
    H5Dclose(dtset_id);
    /* Invert d2g into g2d. */
    *g2d = malloc(sizeof(unsigned int) * len);
    for (i = 0; i < len; i++) {
        (*g2d)[d2g[i]] = i;
    }
    free(d2g);
    
    return ESCDF_SUCCESS;
}

static escdf_errno_t _read_at(const escdf_grid_scalarfield_t *scalarfield,
                              escdf_handle_t *file_id, hid_t loc_id,
                              double *buf,
                              const unsigned int *indirect,
                              const hsize_t glen)
{
    escdf_errno_t err;
    hid_t dtset_id;
    hsize_t *coord;
    size_t num_elements;
    unsigned int i, j;

    /* To limit the size of coord array, only MAX_BLOCK_SIZE grid
       points are read at once. Thus the memory footprint of this
       function is MAX_BLOCK_SIZE * 16 bytes (times 2 if complex). */
    size_t iblock, nblock, j0;
    size_t blocksize, offset;
#define MAX_BLOCK_SIZE (1024 * 1024)

    /* Check that variable on disk is consistent with metadata in scalarfield. */
    if ((err = _get_values_on_grid(scalarfield, loc_id, &dtset_id)) != ESCDF_SUCCESS) {
        return err;
    }

    /* We generate an element selection in the disk dataspace. To
       limit the size of the coord array, the various scalarfield
       components are read separately. */
    num_elements = glen * scalarfield->real_or_complex.value;
    
    nblock = glen / MAX_BLOCK_SIZE;
    if (nblock * MAX_BLOCK_SIZE < glen) {
        nblock += 1;
    }

    coord = malloc(sizeof(hsize_t) * MAX_BLOCK_SIZE *
                   scalarfield->real_or_complex.value * 3);
    for (i = 0; i < scalarfield->number_of_components.value; i++) {
        j0 = 0;
        for (iblock = 0; iblock < nblock; iblock++) {
            blocksize = (glen - j0 < MAX_BLOCK_SIZE) ? glen - j0 : MAX_BLOCK_SIZE;
            if (scalarfield->real_or_complex.value == ESCDF_COMPLEX) {
                for (j = 0; j < blocksize; j++) {
                    coord[(j * 2 + 0) * 3 + 0] = i;
                    coord[(j * 2 + 0) * 3 + 1] = indirect[j0 + j];
                    coord[(j * 2 + 0) * 3 + 2] = 0;
                    coord[(j * 2 + 1) * 3 + 0] = i;
                    coord[(j * 2 + 1) * 3 + 1] = indirect[j0 + j];
                    coord[(j * 2 + 1) * 3 + 2] = 1;
                }
            } else {
                for (j = 0; j < blocksize; j++) {
                    coord[j * 3 + 0] = i;
                    coord[j * 3 + 1] = indirect[j0 + j];
                    coord[j * 3 + 2] = 0;
                }
            }
            offset = i * num_elements + j0 * scalarfield->real_or_complex.value;
            if ((err = utils_hdf5_read_dataset_at(dtset_id, file_id->transfer_mode,
                                                  buf + offset,
                                                  H5T_NATIVE_DOUBLE,
                                                  blocksize * scalarfield->real_or_complex.value, coord)) != ESCDF_SUCCESS) {
                free(coord);
                H5Dclose(dtset_id);
                return err;
            }
            j0 += blocksize;
        }
    }
    free(coord);
    H5Dclose(dtset_id);
    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_grid_scalarfield_write_values_on_grid_ordered(const escdf_grid_scalarfield_t *scalarfield,
                                                                  escdf_handle_t *file_id,
                                                                  const double *buf,
                                                                  const hsize_t *start,
                                                                  const hsize_t *count,
                                                                  const hsize_t *stride)
{
    return escdf_grid_scalarfield_write_values_on_grid
        (scalarfield, file_id, buf, NULL, start, count, stride);
}
escdf_errno_t escdf_grid_scalarfield_write_values_on_grid(const escdf_grid_scalarfield_t *scalarfield,
                                                          escdf_handle_t *file_id,
                                                          const double *buf,
                                                          const unsigned int *tbl,
                                                          const hsize_t *start,
                                                          const hsize_t *count,
                                                          const hsize_t *stride)
{
    escdf_errno_t err;
    hid_t dtset_id, loc_id;
    hsize_t len;
    unsigned int i;

    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(scalarfield->cell.number_of_physical_dimensions.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->number_of_components.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->number_of_grid_points, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->real_or_complex.is_set, ESCDF_EUNINIT);
    if (tbl != NULL) {
        FULFILL_OR_RETURN(scalarfield->use_default_ordering.is_set &&
                          !scalarfield->use_default_ordering.value, ESCDF_EUNINIT);
    } else {
        FULFILL_OR_RETURN(scalarfield->use_default_ordering.is_set &&
                          scalarfield->use_default_ordering.value, ESCDF_EUNINIT);
    }
    
    if ((loc_id = H5Gopen(file_id->group_id, scalarfield->path, H5P_DEFAULT)) < 0) {
        RETURN_WITH_ERROR(loc_id);
    }

    /* Write buf in dataset "values_on_grid". */
    if ((err = _get_values_on_grid(scalarfield, loc_id, &dtset_id)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
    if ((err = utils_hdf5_write_dataset(dtset_id, file_id->transfer_mode,
                                        buf, H5T_NATIVE_DOUBLE,
                                        start, count, stride)) != ESCDF_SUCCESS) {
        H5Dclose(dtset_id);
        H5Gclose(loc_id);
        return err;
    }
    H5Dclose(dtset_id);

    /* Write the lookup table. */
    if (tbl != NULL) {
        len = scalarfield->number_of_grid_points[0];
        for (i = 1; i < scalarfield->cell.number_of_physical_dimensions.value; i++) {
            len *= scalarfield->number_of_grid_points[i];
        }

        if ((err = utils_hdf5_check_dataset(loc_id, "grid_ordering",
                                            &len, 1, &dtset_id)) != ESCDF_SUCCESS) {
            H5Gclose(loc_id);
            return err;
        }

        /* Actual write action. */
        if ((err = utils_hdf5_write_dataset(dtset_id, file_id->transfer_mode,
                                            tbl, H5T_NATIVE_INT,
                                            (start) ? start + 1 : NULL,
                                            (count) ? count + 1 : NULL,
                                            (stride) ? stride + 1 : NULL)) != ESCDF_SUCCESS) {
            H5Dclose(dtset_id);
            H5Gclose(loc_id);
            return err;
        }
        H5Dclose(dtset_id);
    }

    H5Gclose(loc_id);
    return ESCDF_SUCCESS;
}

/**
 * This method is used to write values known on a slice of grid
 * points. The union of all slices among processors should correspond
 * to the box itself. Then each slices are written on disk in a packed
 * way, ordered by processor id. This is a collective call.
 *
 * The values @buf can use a non-default ordering, as defined by
 * @tbl, or a default ordering if @tbl is NULL. The size of @buf is
 * implicit and corresponds to the product of @len,
 * @scalarfield(real_or_complex) and
 * @scalarfield(number_of_components). If given, the size of @tbl in
 * given by @len.
 *
 * @param[in] scalarfield: instance of the scalarfield group.
 * @param[in] file_id: the handle on the opened HDF5 file.
 * @param[in] buf: values of the scalarfield on a slice of grid
 * points, with the full component values for each points.
 * @param[in] tbl: a lookup table that provides for each points in the
 * slice its index in the global zyx ordering.
 * @param[in] len: the size of the slice.
 * @return error code.
 */
escdf_errno_t escdf_grid_scalarfield_write_values_on_grid_sliced(const escdf_grid_scalarfield_t *scalarfield,
                                                                 escdf_handle_t *file_id,
                                                                 const double *buf,
                                                                 const unsigned int *tbl,
                                                                 const hsize_t len)
{
    escdf_errno_t err;
    hsize_t start[3], count[3];

    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(scalarfield->cell.number_of_physical_dimensions.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->number_of_components.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->number_of_grid_points, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->real_or_complex.is_set, ESCDF_EUNINIT);

    start[0] = 0;
    start[1] = 0;
    start[2] = 0;
    count[0] = scalarfield->number_of_components.value;
    count[1] = len;
    count[2] = scalarfield->real_or_complex.value;

    /* Modify the start[1] value from the gather of len. */
    err = _get_proc_grid_offset(&start[1], file_id,
                                scalarfield->cell.number_of_physical_dimensions.value,
                                scalarfield->number_of_grid_points, len);
    FULFILL_OR_RETURN(err == ESCDF_SUCCESS, err);

    return escdf_grid_scalarfield_write_values_on_grid
        (scalarfield, file_id, buf, tbl, start, count, NULL);
}

escdf_errno_t escdf_grid_scalarfield_read_values_on_grid(const escdf_grid_scalarfield_t *scalarfield,
                                                         escdf_handle_t *file_id, double *buf,
                                                         const hsize_t *start,
                                                         const hsize_t *count,
                                                         const hsize_t *stride)
{
    escdf_errno_t err;
    hid_t dtset_id, loc_id;

    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(scalarfield->number_of_components.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->number_of_grid_points, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->real_or_complex.is_set, ESCDF_EUNINIT);
    
    if ((loc_id = H5Gopen(file_id->group_id, scalarfield->path, H5P_DEFAULT)) < 0) {
        RETURN_WITH_ERROR(loc_id);
    }
    
    if ((err = _get_values_on_grid(scalarfield, loc_id, &dtset_id)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
    if ((err = utils_hdf5_read_dataset(dtset_id, file_id->transfer_mode,
                                       buf, H5T_NATIVE_DOUBLE,
                                       start, count, stride)) != ESCDF_SUCCESS) {
        H5Dclose(dtset_id);
        H5Gclose(loc_id);
        return err;
    }

    H5Dclose(dtset_id);
    H5Gclose(loc_id);
    return ESCDF_SUCCESS;
}
escdf_errno_t escdf_grid_scalarfield_read_values_on_grid_sliced(const escdf_grid_scalarfield_t *scalarfield,
                                                                escdf_handle_t *file_id,
                                                                double *buf,
                                                                const unsigned int *tbl,
                                                                const hsize_t len)
{
    escdf_errno_t err;
    hid_t loc_id;
    hsize_t goffset;
    unsigned int i;
    hsize_t start[3], count[3];

    unsigned int *g2d, *indirect;

    FULFILL_OR_RETURN(scalarfield, ESCDF_EOBJECT);
    FULFILL_OR_RETURN(scalarfield->number_of_components.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->number_of_grid_points, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(scalarfield->real_or_complex.is_set, ESCDF_EUNINIT);
    
    if ((loc_id = H5Gopen(file_id->group_id, scalarfield->path, H5P_DEFAULT)) < 0) {
        RETURN_WITH_ERROR(loc_id);
    }

    if ((err = _get_g2d(scalarfield, loc_id, &g2d)) != ESCDF_SUCCESS) {
        H5Gclose(loc_id);
        return err;
    }
        
    if (tbl && g2d) {
        /* Case where ask for a disordered subset of points in a
           disordered storage. */
        indirect = malloc(sizeof(hsize_t) * len);
        for (i = 0; i < len; i++) {
            indirect[i] = g2d[tbl[i]];
        }
        free(g2d);
        if ((err = _read_at(scalarfield, file_id, loc_id,
                            buf, indirect, len)) != ESCDF_SUCCESS) {
            free(indirect);
            H5Gclose(loc_id);
            return err;
        }
        free(indirect);
    } else if (tbl && !g2d) {
        /* Case where ask for a disordered subset of points in an
           ordered storage. */
        if ((err = _read_at(scalarfield, file_id, loc_id,
                            buf, tbl, len)) != ESCDF_SUCCESS) {
            H5Gclose(loc_id);
            return err;
        }
    } else if (!tbl && g2d) {
        /* Case where ask for an ordered subset of points in a
           disordered storage. */
        if ((err = _get_proc_grid_offset(&goffset, file_id,
                                         scalarfield->cell.number_of_physical_dimensions.value,
                                         scalarfield->number_of_grid_points,
                                         len)) != ESCDF_SUCCESS) {
            free(g2d);
            H5Gclose(loc_id);
            return err;
        }

        indirect = malloc(sizeof(hsize_t) * len);
        for (i = 0; i < len; i++) {
            indirect[i] = g2d[goffset + i];
        }
        free(g2d);

        if ((err = _read_at(scalarfield, file_id, loc_id,
                            buf, indirect, len)) != ESCDF_SUCCESS) {
            free(indirect);
            H5Gclose(loc_id);
            return err;
        }
        free(indirect);
    } else { /* !tbl && !g2d */
        /* Case where ask for an ordered subset of points in an
           ordered storage. */
        start[0] = 0;
        start[1] = 0;
        start[2] = 0;
        count[0] = scalarfield->number_of_components.value;
        count[1] = len;
        count[2] = scalarfield->real_or_complex.value;

        if ((err = _get_proc_grid_offset
             (&start[1], file_id, scalarfield->cell.number_of_physical_dimensions.value,
              scalarfield->number_of_grid_points, len)) != ESCDF_SUCCESS) {
            H5Gclose(loc_id);
            return err;
        }

        if ((err = escdf_grid_scalarfield_read_values_on_grid
             (scalarfield, file_id, buf, start, count, NULL)) != ESCDF_SUCCESS) {
            H5Gclose(loc_id);
            return err;
        }
    }
    
    H5Gclose(loc_id);
    return ESCDF_SUCCESS;
}

/***************/
/* IO streams. */
/***************/
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
    if (scalarfield->use_default_ordering.is_set) {
        fprintf(f, "  use_default_ordering: %s\n",
                (scalarfield->use_default_ordering.value) ? "yes" : "no");
    }
    if (scalarfield->grid_ordering_is_present) {
        fprintf(f, "  grid_ordering_is_present: yes\n");
    }

    return ESCDF_SUCCESS;
}
