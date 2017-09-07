/* Copyright (C) 2016-2017 Fabiano Corsetti <fabiano.corsetti@gmail.com>
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

#include <stddef.h>

#include "escdf_system.h"

#include "escdf_error.h"
#include "utils.h"
#include "utils_hdf5.h"

/******************************************************************************
 * Data structures                                                            *
 ******************************************************************************/

/**
 * System data type. This type stores all the metadata that can be found in
 * the ESCDF system group. It also stores if any of the other data are
 * present in the group or not.
*/
struct escdf_system {
    hid_t group_id; /**< Handle for HDF5 group */

    /* The metadata */
    char *system_name;
    _uint_set_t number_of_physical_dimensions;
    int *dimension_types;
    _bool_set_t embedded_system;
    _uint_set_t number_of_species;
    _uint_set_t number_of_sites;
    _uint_set_t number_of_symmetry_operations;
    double *lattice_vectors;
    _uint_set_t spacegroup_3D_number;
    _bool_set_t symmorphic;
    _bool_set_t time_reversal_symmetry;
    double *bulk_regions_for_semi_infinite_dimension;

    /* Information about which data is present in the group */
    bool species_at_sites_is_present;
    bool cartesian_site_positions_is_set;
    bool fractional_site_positions_is_set;
    bool species_names_is_set;
    bool chemical_symbols_is_set;
    bool atomic_numbers_is_set;
    bool reduced_symmetry_matrices_is_set;
    bool reduced_symmetry_translations_is_set;
    bool number_of_species_at_site_is_set;
    bool concentration_of_species_at_site_is_set;
    bool local_rotations_is_set;
    bool magnetic_moments_is_set;
    bool site_regions_is_set;
    bool cell_in_host_is_set;
    bool site_in_host_is_set;
    bool forces_is_set;
    bool stress_tensor_is_set;

};


/******************************************************************************
 * Low-level creators and destructors                                         *
 ******************************************************************************/

escdf_system_t * escdf_system_new()
{
    escdf_system_t *system;

    system = (escdf_system_t *) malloc(sizeof(escdf_system_t));

    if (system == NULL)
        return NULL;

    /* no metadata set at the moment */
    system->system_name = NULL;
    system->number_of_physical_dimensions.is_set = false;
    system->dimension_types = NULL;
    system->embedded_system.is_set = false;
    system->number_of_species.is_set = false;
    system->number_of_sites.is_set = false;
    system->number_of_symmetry_operations.is_set = false;
    system->lattice_vectors = NULL;
    system->spacegroup_3D_number.is_set = false;
    system->symmorphic.is_set = false;
    system->time_reversal_symmetry.is_set = false;
    system->bulk_regions_for_semi_infinite_dimension = NULL;

    /* no data is present at the moment*/
    system->species_at_sites_is_present = false;
    system->cartesian_site_positions_is_set = false;
    system->fractional_site_positions_is_set = false;
    system->species_names_is_set = false;
    system->chemical_symbols_is_set = false;
    system->atomic_numbers_is_set = false;
    system->reduced_symmetry_matrices_is_set = false;
    system->reduced_symmetry_translations_is_set = false;
    system->number_of_species_at_site_is_set = false;
    system->concentration_of_species_at_site_is_set = false;
    system->local_rotations_is_set = false;
    system->magnetic_moments_is_set = false;
    system->site_regions_is_set = false;
    system->cell_in_host_is_set = false;
    system->site_in_host_is_set = false;
    system->forces_is_set = false;
    system->stress_tensor_is_set = false;

    return system;
}

void escdf_system_free(escdf_system_t *system)
{
    if (system != NULL) {
        free(system->system_name);
        free(system->dimension_types);
        free(system->lattice_vectors);
        free(system->bulk_regions_for_semi_infinite_dimension);

        free(system);
    }
}

escdf_errno_t escdf_system_open_group(escdf_system_t *system, escdf_handle_t *handle, const char *path)
{
    char group_name[ESCDF_STRLEN_GROUP];


    if (path == NULL) {
        sprintf(group_name, "%s", "system");
    } else {
        sprintf(group_name, "%s/%s", "system", path);
    }

    system->group_id = H5Gopen(handle->group_id, group_name, H5P_DEFAULT);

    FULFILL_OR_RETURN(system->group_id >= 0, system->group_id);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_create_group(escdf_system_t *system, escdf_handle_t *handle, const char *path)
{
    char group_name[ESCDF_STRLEN_GROUP];

    if (path == NULL) {
        sprintf(group_name, "%s", "system");
    } else {
        sprintf(group_name, "%s/%s", "system", path);
    }
    utils_hdf5_create_group(handle->file_id, group_name, &(system->group_id));

    FULFILL_OR_RETURN(system->group_id >= 0, system->group_id);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_close_group(escdf_system_t *system)
{
    herr_t herr_status;

    /* close the group */
    herr_status = H5Gclose(system->group_id);
    FULFILL_OR_RETURN(herr_status >= 0, herr_status);

    return ESCDF_SUCCESS;
}

/******************************************************************************
 * Global functions                                                           *
 ******************************************************************************/

escdf_system_t * escdf_system_open(const escdf_handle_t *handle,
                                   const char *name)
{
    escdf_system_t *system;

    system = (escdf_system_t *) malloc(sizeof(escdf_system_t));
    //FULFILL_OR_RETURN(system != NULL, ESCDF_ENOMEM)

    /* check if "system" group exists; if not, create it */
    if (!utils_hdf5_check_present(handle->group_id, "system")) {
        system->group_id = H5Gcreate(handle->group_id, "system", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    } else {
        system->group_id = H5Gopen(handle->group_id, "system", H5P_DEFAULT);
    }

    /* check if specific system group exists and open it; if not, create it */
    if (!utils_hdf5_check_present(system->group_id, name)) {
        system->group_id = H5Gcreate(system->group_id, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    else {
        system->group_id = H5Gopen(system->group_id, name, H5P_DEFAULT);
    }

    return system;
}

escdf_errno_t escdf_system_close(escdf_system_t *system)
{
    herr_t herr_status;

    /* close the group */
    herr_status = H5Gclose(system->group_id);
    FULFILL_OR_RETURN(herr_status >= 0, herr_status);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_read_metadata(escdf_system_t *system)
{
    escdf_errno_t err;
    hsize_t dims_1D[1], dims_2D[2];

    unsigned int number_of_physical_dimensions_range[2] = {3, 3};
    int dimension_types_range[2] = {0, 2};
    unsigned int number_of_species_range[2] = {1, 1000};
    unsigned int number_of_sites_range[2] = {1, 1000};
    unsigned int number_of_symmetry_operations_range[2] = {1, 1000};
    unsigned int spacegroup_3D_number_range[2] = {1, 230};
    double lattice_vectors_range[2] = {-1000, 1000};
    double bulk_regions_for_semi_infinite_dimension_range[2] = {0, 1000};


    /* --system_name */
    if (utils_hdf5_check_present_attr(system->group_id, "system_name") &&
        (err = utils_hdf5_read_string(system->group_id, "system_name",
                                      &system->system_name, 80)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --number_of_physical_dimensions */
    if (utils_hdf5_check_present_attr(system->group_id, "number_of_physical_dimensions") &&
        (err = utils_hdf5_read_uint(system->group_id, "number_of_physical_dimensions",
                                    &system->number_of_physical_dimensions,
                                    number_of_physical_dimensions_range)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --dimension_types */
    if (system->number_of_physical_dimensions.is_set) {
        dims_1D[0] = system->number_of_physical_dimensions.value;
        if (utils_hdf5_check_present_attr(system->group_id, "dimension_types") &&
            (err = utils_hdf5_read_int_array(system->group_id, "dimension_types",
                                             &system->dimension_types,
                                             dims_1D, 1, dimension_types_range)) != ESCDF_SUCCESS) {
            return err;
        }
    }

    /* --embedded_system */
    if (utils_hdf5_check_present_attr(system->group_id, "embedded_system") &&
        (err = utils_hdf5_read_bool(system->group_id, "embedded_system",
                                    &system->embedded_system)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --number_of_species */
    if (utils_hdf5_check_present_attr(system->group_id, "number_of_species") &&
        (err = utils_hdf5_read_uint(system->group_id, "number_of_species",
                                    &system->number_of_species,
                                    number_of_species_range)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --number_of_sites */
    if (utils_hdf5_check_present_attr(system->group_id, "number_of_sites") &&
        (err = utils_hdf5_read_uint(system->group_id, "number_of_sites",
                                    &system->number_of_sites,
                                    number_of_sites_range)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --number_of_symmetry_operations */
    if (utils_hdf5_check_present_attr(system->group_id, "number_of_symmetry_operations") &&
        (err = utils_hdf5_read_uint(system->group_id, "number_of_symmetry_operations",
                                    &system->number_of_symmetry_operations,
                                    number_of_symmetry_operations_range)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --lattice_vectors */
    if (system->number_of_physical_dimensions.is_set) {
        dims_2D[0] = system->number_of_physical_dimensions.value;
        dims_2D[1] = system->number_of_physical_dimensions.value;
        if (utils_hdf5_check_present_attr(system->group_id, "lattice_vectors") &&
            (err = utils_hdf5_read_dbl_array(system->group_id, "lattice_vectors",
                                             &system->lattice_vectors, dims_2D, 2,
                                             lattice_vectors_range)) != ESCDF_SUCCESS) {
            return err;
        }
    }

    /* --spacegroup_3D_number */
    if (utils_hdf5_check_present_attr(system->group_id, "spacegroup_3D_number") &&
        (err = utils_hdf5_read_uint(system->group_id, "spacegroup_3D_number",
                                    &system->spacegroup_3D_number,
                                    spacegroup_3D_number_range)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --symmorphic */
    if (utils_hdf5_check_present_attr(system->group_id, "symmorphic") &&
        (err = utils_hdf5_read_bool(system->group_id, "symmorphic",
                                    &system->symmorphic)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --time_reversal_symmetry */
    if (utils_hdf5_check_present_attr(system->group_id, "time_reversal_symmetry") &&
        (err = utils_hdf5_read_bool(system->group_id, "time_reversal_symmetry",
                                    &system->time_reversal_symmetry)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --bulk_regions_for_semi_infinite_dimension */
    dims_1D[0] = 2;
    if (utils_hdf5_check_present_attr(system->group_id, "bulk_regions_for_semi_infinite_dimension") &&
        (err = utils_hdf5_read_dbl_array(system->group_id, "bulk_regions_for_semi_infinite_dimension",
                                         &system->bulk_regions_for_semi_infinite_dimension, dims_1D, 1,
                                         bulk_regions_for_semi_infinite_dimension_range)) != ESCDF_SUCCESS) {
        return err;
    }

    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_system_set_system_name(
    escdf_system_t *system, const char system_name[81])
{
    herr_t err;

    if ((err = utils_hdf5_write_string(system->group_id, "system_name", system_name, 80)) != ESCDF_SUCCESS)
        return err;

    free(system->system_name);
    system->system_name = (char *)malloc(80*sizeof(char));
    FULFILL_OR_EXIT(system->system_name != NULL, ESCDF_ENOMEM);

    strcpy(system->system_name, system_name);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_system_name(
    const escdf_system_t *system, char *system_name)
{
    FULFILL_OR_RETURN(system->system_name != NULL, ESCDF_ESIZE_MISSING);
    strcpy(system_name, system->system_name);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_number_of_physical_dimensions(
    escdf_system_t *system, unsigned int number_of_physical_dimensions)
{
    herr_t err;
    if ((err = utils_hdf5_write_attr(system->group_id, "number_of_physical_dimensions", H5T_NATIVE_UINT, NULL, 0,
                                     H5T_NATIVE_UINT, &number_of_physical_dimensions)) != ESCDF_SUCCESS)
        return err;

    system->number_of_physical_dimensions = _uint_set(number_of_physical_dimensions);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_number_of_physical_dimensions(
    const escdf_system_t *system, unsigned int *number_of_physical_dimensions)
{
    FULFILL_OR_RETURN(system->number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING);

    *number_of_physical_dimensions = system->number_of_physical_dimensions.value;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_dimension_types(
    escdf_system_t *system, const int *dimension_types)
{
    herr_t err;
    hsize_t dim;

    FULFILL_OR_RETURN(system->number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING);
    dim = system->number_of_physical_dimensions.value;

    if ((err = utils_hdf5_write_attr(system->group_id, "dimension_types", H5T_NATIVE_INT, &dim, 1,
                                     H5T_NATIVE_INT, &dimension_types)) != ESCDF_SUCCESS)
        return err;

    free(system->dimension_types);
    system->dimension_types = (int *)malloc(dim * sizeof(int));
    FULFILL_OR_EXIT(system->dimension_types != NULL, ESCDF_ENOMEM);
    memcpy(system->dimension_types, dimension_types, dim * sizeof(int));

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_dimension_types(
    const escdf_system_t *system, int *dimension_types)
{
    FULFILL_OR_RETURN(system->dimension_types != NULL, ESCDF_ESIZE_MISSING);
    FULFILL_OR_RETURN(system->number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING);

    memcpy(dimension_types, system->dimension_types,
           sizeof(int) * system->number_of_physical_dimensions.value);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_embedded_system(
    escdf_system_t *system, bool embedded_system)
{
    herr_t err;
    if ((err = utils_hdf5_write_bool(system->group_id, "embedded_system",
                                     embedded_system)) != ESCDF_SUCCESS)
        return err;

    system->embedded_system = _bool_set(embedded_system);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_embedded_system(
    const escdf_system_t *system, bool *embedded_system)
{
    FULFILL_OR_RETURN(system->embedded_system.is_set, ESCDF_ESIZE_MISSING);

    *embedded_system = system->embedded_system.value;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_number_of_species(
    escdf_system_t *system, unsigned int number_of_species)
{
    herr_t err;
    if ((err = utils_hdf5_write_attr(system->group_id, "number_of_species", H5T_NATIVE_UINT, NULL, 0,
                                     H5T_NATIVE_UINT, &number_of_species)) != ESCDF_SUCCESS)
        return err;

    system->number_of_species = _uint_set(number_of_species);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_number_of_species(
    const escdf_system_t *system, unsigned int *number_of_species)
{
    FULFILL_OR_RETURN(system->number_of_species.is_set, ESCDF_ESIZE_MISSING);

    *number_of_species = system->number_of_species.value;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_number_of_sites(
    escdf_system_t *system, unsigned int number_of_sites)
{
    herr_t err;
    if ((err = utils_hdf5_write_attr(system->group_id, "number_of_sites", H5T_NATIVE_UINT, NULL, 0,
                                     H5T_NATIVE_UINT, &number_of_sites)) != ESCDF_SUCCESS)
        return err;

    system->number_of_sites = _uint_set(number_of_sites);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_number_of_sites(
    const escdf_system_t *system, unsigned int *number_of_sites)
{
    FULFILL_OR_RETURN(system->number_of_sites.is_set, ESCDF_ESIZE_MISSING);

    *number_of_sites = system->number_of_sites.value;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_number_of_symmetry_operations(
    escdf_system_t *system, unsigned int number_of_symmetry_operations)
{
    herr_t err;
    if ((err = utils_hdf5_write_attr(system->group_id, "number_of_symmetry_operations", H5T_NATIVE_UINT, NULL, 0,
                                     H5T_NATIVE_UINT, &number_of_symmetry_operations)) != ESCDF_SUCCESS)
        return err;

    system->number_of_symmetry_operations = _uint_set(number_of_symmetry_operations);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_number_of_symmetry_operations(
    const escdf_system_t *system, unsigned int *number_of_symmetry_operations)
{
    FULFILL_OR_RETURN(system->number_of_symmetry_operations.is_set, ESCDF_ESIZE_MISSING);

    *number_of_symmetry_operations = system->number_of_symmetry_operations.value;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_lattice_vectors(
    escdf_system_t *system, const double *lattice_vectors)
{
    herr_t err;
    hsize_t dim[2];

    FULFILL_OR_RETURN(system->number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING);
    dim[0] = system->number_of_physical_dimensions.value;
    dim[1] = system->number_of_physical_dimensions.value;

    if ((err = utils_hdf5_write_attr(system->group_id, "lattice_vectors", H5T_NATIVE_DOUBLE, dim, 2,
                                     H5T_NATIVE_DOUBLE, &lattice_vectors)) != ESCDF_SUCCESS)
        return err;

    free(system->lattice_vectors);
    system->lattice_vectors = (double *)malloc(dim[0]*dim[1] * sizeof(double));
    FULFILL_OR_EXIT(system->lattice_vectors != NULL, ESCDF_ENOMEM);
    memcpy(system->lattice_vectors, lattice_vectors, dim[0] * dim[1] * sizeof(double));

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_lattice_vectors(
    const escdf_system_t *system, double *lattice_vectors)
{
    FULFILL_OR_RETURN(system->lattice_vectors != NULL, ESCDF_ESIZE_MISSING);
    FULFILL_OR_RETURN(system->number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING);

    memcpy(lattice_vectors, system->lattice_vectors,
           sizeof(double) *  system->number_of_physical_dimensions.value *
               system->number_of_physical_dimensions.value);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_spacegroup_3D_number(
    escdf_system_t *system, unsigned int spacegroup_3D_number)
{
    herr_t err;
    if ((err = utils_hdf5_write_attr(system->group_id, "spacegroup_3D_number", H5T_NATIVE_UINT, NULL, 0,
                                     H5T_NATIVE_UINT, &spacegroup_3D_number)) != ESCDF_SUCCESS)
        return err;

    system->spacegroup_3D_number = _uint_set(spacegroup_3D_number);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_spacegroup_3D_number(
    const escdf_system_t *system, unsigned int *spacegroup_3D_number)
{
    FULFILL_OR_RETURN(system->spacegroup_3D_number.is_set, ESCDF_ESIZE_MISSING);

    *spacegroup_3D_number = system->spacegroup_3D_number.value;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_symmorphic(
    escdf_system_t *system, bool symmorphic)
{
    herr_t err;
    if ((err = utils_hdf5_write_bool(system->group_id, "symmorphic", symmorphic)) != ESCDF_SUCCESS)
        return err;

    system->symmorphic = _bool_set(symmorphic);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_symmorphic(
    const escdf_system_t *system, bool *symmorphic)
{
    FULFILL_OR_RETURN(system->symmorphic.is_set, ESCDF_ESIZE_MISSING);

    *symmorphic = system->symmorphic.value;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_time_reversal_symmetry(
    escdf_system_t *system, bool time_reversal_symmetry)
{
    herr_t err;
    if ((err = utils_hdf5_write_bool(system->group_id, "time_reversal_symmetry",
                                     time_reversal_symmetry)) != ESCDF_SUCCESS)
        return err;

    system->time_reversal_symmetry = _bool_set(time_reversal_symmetry);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_time_reversal_symmetry(
    const escdf_system_t *system, bool *time_reversal_symmetry)
{
    FULFILL_OR_RETURN(system->time_reversal_symmetry.is_set, ESCDF_ESIZE_MISSING);

    *time_reversal_symmetry = system->time_reversal_symmetry.value;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_bulk_regions_for_semi_infinite_dimension(
    escdf_system_t *system, const double *bulk_regions_for_semi_infinite_dimension)
{
    herr_t err;
    hsize_t dim = 2;

    if ((err = utils_hdf5_write_attr(system->group_id, "bulk_regions_for_semi_infinite_dimension",
                                     H5T_NATIVE_DOUBLE, &dim, 1, H5T_NATIVE_DOUBLE,
                                     &bulk_regions_for_semi_infinite_dimension)) != ESCDF_SUCCESS)
        return err;

    free(system->bulk_regions_for_semi_infinite_dimension);
    system->bulk_regions_for_semi_infinite_dimension = (double *)malloc(2 * sizeof(double));
    FULFILL_OR_EXIT(system->bulk_regions_for_semi_infinite_dimension!= NULL, ESCDF_ENOMEM);
    memcpy(system->bulk_regions_for_semi_infinite_dimension,
           bulk_regions_for_semi_infinite_dimension, 2 * sizeof(double));

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_bulk_regions_for_semi_infinite_dimension(
    const escdf_system_t *system, double *bulk_regions_for_semi_infinite_dimension)
{
    FULFILL_OR_RETURN(system->bulk_regions_for_semi_infinite_dimension != NULL, ESCDF_ESIZE_MISSING);

    memcpy(bulk_regions_for_semi_infinite_dimension, system->bulk_regions_for_semi_infinite_dimension,
           2 * sizeof(double));

    return ESCDF_SUCCESS;
}




/*
escdf_errno_t escdf_system_set_dimension_types(
        escdf_system_t *system, const int *dimension_types, const size_t len)
{
    unsigned int i, c;

    FULFILL_OR_RETURN(system->number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING);
    FULFILL_OR_RETURN(len == system->number_of_physical_dimensions.value, ESCDF_ESIZE);
    c = 0;
    for (i = 0; i < len; i++) {
        FULFILL_OR_RETURN(dimension_types[i] >= 0 && dimension_types[i] < 3, ESCDF_ERANGE);
        if (dimension_types[i] == 2) c++;
    }
    FULFILL_OR_RETURN(c <= 1, ESCDF_ERANGE);

    free(system->dimension_types);
    system->dimension_types = malloc(sizeof(int) * len);
    memcpy(system->dimension_types, dimension_types, sizeof(int) * len);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_get_dimension_types(
        const escdf_system_t *system, int *dimension_types, const size_t len)
{
    FULFILL_OR_RETURN(system->dimension_types, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(len == system->number_of_physical_dimensions.value, ESCDF_ESIZE);

    memcpy(dimension_types, system->dimension_types, sizeof(int) * len);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_system_set_embedded_system(escdf_system_t *system,
                                                 const bool embedded_system)
{
    system->embedded_system = _bool_set(embedded_system);

    return ESCDF_SUCCESS;
}

bool escdf_system_get_embedded_system(const escdf_system_t *system)
{
    FULFILL_OR_RETURN_VAL(system->embedded_system.is_set, ESCDF_EUNINIT, true);
    
    return system->embedded_system.value;
}

escdf_errno_t escdf_system_set_number_of_species(
        escdf_system_t *system, const int number_of_species)
{
    system->number_of_species = _uint_set(number_of_species);

    return ESCDF_SUCCESS;
}

int escdf_system_get_number_of_species(
        const escdf_system_t *system)
{
    FULFILL_OR_RETURN_VAL(system->number_of_species.is_set, ESCDF_ESIZE_MISSING, 0);

    return system->number_of_species.value;
}

escdf_errno_t escdf_system_set_number_of_sites(
        escdf_system_t *system, const int number_of_sites)
{
    system->number_of_sites = _uint_set(number_of_sites);

    return ESCDF_SUCCESS;
}

int escdf_system_get_number_of_sites(
        const escdf_system_t *system)
{
    FULFILL_OR_RETURN_VAL(system->number_of_sites.is_set, ESCDF_ESIZE_MISSING, 0);

    return system->number_of_sites.value;
}

escdf_errno_t escdf_system_set_number_of_symmetry_operations(
        escdf_system_t *system, const int number_of_symmetry_operations)
{
    system->number_of_symmetry_operations = _uint_set(number_of_symmetry_operations);

    return ESCDF_SUCCESS;
}

int escdf_system_get_number_of_symmetry_operations(
        const escdf_system_t *system)
{
    FULFILL_OR_RETURN_VAL(system->number_of_symmetry_operations.is_set, ESCDF_ESIZE_MISSING, 0);

    return system->number_of_symmetry_operations.value;
}

*/