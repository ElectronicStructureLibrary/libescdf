/*
 Copyright (C) 2016 F. Corseti, M. Oliveira

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

#include <stddef.h>

#include "escdf_geometry.h"

#include "escdf_error.h"
#include "utils.h"
#include "utils_hdf5.h"

/******************************************************************************
 * Data structures                                                            *
 ******************************************************************************/

/**
 * Geometry data type. This type stores all the metadata that can be found in
 * the ESCDF geometry group. It also stores if any of the other data are
 * present in the group or not.
*/
struct escdf_geometry {
    hid_t group_id; /**< Handle for HDF5 group */

    /* The metadata */
    char *system_name;
    _int_set_t number_of_physical_dimensions;
    int *dimension_types;
    _bool_set_t embedded_system;
    _int_set_t number_of_species;
    _int_set_t number_of_sites;
    _int_set_t number_of_symmetry_operations;
    double *lattice_vector;
    _int_set_t spacegroup_3D_number;
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

escdf_geometry_t * escdf_geometry_new()
{
    escdf_geometry_t *geometry;

    geometry = (escdf_geometry_t *) malloc(sizeof(escdf_geometry_t));

    if (geometry == NULL)
        return NULL;

    /* no metadata set at the moment */
    geometry->system_name = NULL;
    geometry->number_of_physical_dimensions.is_set = false;
    geometry->dimension_types = NULL;
    geometry->embedded_system.is_set = false;
    geometry->number_of_species.is_set = false;
    geometry->number_of_sites.is_set = false;
    geometry->number_of_symmetry_operations.is_set = false;
    geometry->lattice_vector = NULL;
    geometry->spacegroup_3D_number.is_set = false;
    geometry->symmorphic.is_set = false;
    geometry->time_reversal_symmetry.is_set = false;
    geometry->bulk_regions_for_semi_infinite_dimension = NULL;

    /* no data is present at the moment*/
    geometry->species_at_sites_is_present = false;
    geometry->cartesian_site_positions_is_set = false;
    geometry->fractional_site_positions_is_set = false;
    geometry->species_names_is_set = false;
    geometry->chemical_symbols_is_set = false;
    geometry->atomic_numbers_is_set = false;
    geometry->reduced_symmetry_matrices_is_set = false;
    geometry->reduced_symmetry_translations_is_set = false;
    geometry->number_of_species_at_site_is_set = false;
    geometry->concentration_of_species_at_site_is_set = false;
    geometry->local_rotations_is_set = false;
    geometry->magnetic_moments_is_set = false;
    geometry->site_regions_is_set = false;
    geometry->cell_in_host_is_set = false;
    geometry->site_in_host_is_set = false;
    geometry->forces_is_set = false;
    geometry->stress_tensor_is_set = false;

    return geometry;
}

void escdf_geometry_free(escdf_geometry_t *geometry)
{
    if (geometry != NULL) {
        free(geometry->system_name);
        free(geometry->dimension_types);
        free(geometry->lattice_vector);
        free(geometry->bulk_regions_for_semi_infinite_dimension);

        free(geometry);
    }
}

escdf_errno_t escdf_geometry_open_group(escdf_geometry_t *geometry, escdf_handle_t *handle, const char *path)
{
    char group_name[ESCDF_STRLEN_GROUP];


    if (path == NULL) {
        sprintf(group_name, "%s", "geometries");
    } else {
        sprintf(group_name, "%s/%s", "geometries", path);
    }

    geometry->group_id = H5Gopen(handle->group_id, group_name, H5P_DEFAULT);

    FULFILL_OR_RETURN(geometry->group_id >= 0, geometry->group_id);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_geometry_create_group(escdf_geometry_t *geometry, escdf_handle_t *handle, const char *path)
{
    char group_name[ESCDF_STRLEN_GROUP];

    if (path == NULL) {
        sprintf(group_name, "%s", "geometries");
    } else {
        sprintf(group_name, "%s/%s", "geometries", path);
    }
    utils_hdf5_create_group(handle->file_id, group_name, &(geometry->group_id));

    FULFILL_OR_RETURN(geometry->group_id >= 0, geometry->group_id);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_geometry_close_group(escdf_geometry_t *geometry)
{
    herr_t herr_status;

    /* close the group */
    herr_status = H5Gclose(geometry->group_id);
    FULFILL_OR_RETURN(herr_status >= 0, herr_status);

    return ESCDF_SUCCESS;
}

/******************************************************************************
 * Global functions                                                           *
 ******************************************************************************/

escdf_geometry_t * escdf_geometry_open(const escdf_handle_t *handle,
        const char *name)
{
    escdf_geometry_t *geometry;

    geometry = (escdf_geometry_t *) malloc(sizeof(escdf_geometry_t));
    //FULFILL_OR_RETURN(geometry != NULL, ESCDF_ENOMEM)

    /* check if "geometries" group exists; if not, create it */
    if (!utils_hdf5_check_present(handle->group_id, "geometries")) {
        geometry->group_id = H5Gcreate(handle->group_id, "geometries", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    } else {
        geometry->group_id = H5Gopen(handle->group_id, "geometries", H5P_DEFAULT);
    }

    /* check if specific geometry group exists and open it; if not, create it */
    if (!utils_hdf5_check_present(geometry->group_id, name)) {
        geometry->group_id = H5Gcreate(geometry->group_id, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    else {
        geometry->group_id = H5Gopen(geometry->group_id, name, H5P_DEFAULT);
    }

    return geometry;
}

escdf_errno_t escdf_geometry_close(escdf_geometry_t * geometry)
{
    herr_t herr_status;

    /* close the group */
    herr_status = H5Gclose(geometry->group_id);
    FULFILL_OR_RETURN(herr_status >= 0, herr_status);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_geometry_read_metadata(escdf_geometry_t *geometry)
{
    escdf_errno_t err;
    int number_of_physical_dimensions_range[2] = {3, 3};
    int dimension_types_range[2] = {0, 2};
    int number_of_species_range[2] = {1, 1000};
    int number_of_sites_range[2] = {1, 1000};
    int absolute_or_reduced_coordinates_range[2] = {1, 2};
    int number_of_symmetry_operations_range[2] = {1, 1000};
    hsize_t oneDims[1];

    /* read attributes of the group: */

    /* --number_of_physical_dimensions */
    if ((err = utils_hdf5_read_int(geometry->group_id, "number_of_physical_dimensions",
                                   &geometry->number_of_physical_dimensions,
                                   number_of_physical_dimensions_range)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --dimension_types */
    oneDims[0] = geometry->number_of_physical_dimensions.value;
    if ((err = utils_hdf5_read_int_array(geometry->group_id, "dimension_types",
                                         &geometry->dimension_types,
                                         oneDims, 1, dimension_types_range)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --embedded_system */
    if ((err = utils_hdf5_read_bool(geometry->group_id, "embedded_system",
                                    &geometry->embedded_system)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --number_of_species */
    if ((err = utils_hdf5_read_int(geometry->group_id, "number_of_species",
                                   &geometry->number_of_species,
                                   number_of_species_range)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --number_of_sites */
    if ((err = utils_hdf5_read_int(geometry->group_id, "number_of_sites",
                                   &geometry->number_of_sites,
                                   number_of_sites_range)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --number_of_symmetry_operations */
    if ((err = utils_hdf5_read_int(geometry->group_id, "number_of_symmetry_operations",
                                   &geometry->number_of_symmetry_operations,
                                   number_of_symmetry_operations_range)) != ESCDF_SUCCESS) {
        return err;
    }

    /* read datasets of the group: */

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_geometry_write_metadata(const escdf_geometry_t *geometry)
{
    escdf_errno_t err;
    hsize_t dims[3];
    int value;

    FULFILL_OR_RETURN(geometry, ESCDF_EOBJECT);

    /* check mandatory attributes */
    FULFILL_OR_RETURN(geometry->number_of_physical_dimensions.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(geometry->dimension_types, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(geometry->embedded_system.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(geometry->number_of_species.is_set, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(geometry->number_of_sites.is_set, ESCDF_EUNINIT);

    /* write attributes of the group: */

    /* --number_of_physical_dimensions */
    dims[0] = 1;
    if ((err = utils_hdf5_write_attr
         (geometry->group_id, "number_of_physical_dimensions", H5T_STD_U32LE, dims, 1, H5T_NATIVE_INT,
          &geometry->number_of_physical_dimensions.value)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --dimension_types */
    dims[0] = geometry->number_of_physical_dimensions.value;
    if ((err = utils_hdf5_write_attr
         (geometry->group_id, "dimension_types", H5T_STD_U32LE, dims, 1, H5T_NATIVE_INT,
          geometry->dimension_types)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --embedded_system */
    dims[0] = 1;
    value = (int)geometry->embedded_system.value;
    if ((err = utils_hdf5_write_attr
         (geometry->group_id, "embedded_system", H5T_STD_U32LE, dims, 1, H5T_NATIVE_INT,
          &value)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --number_of_species */
    dims[0] = 1;
    if ((err = utils_hdf5_write_attr
         (geometry->group_id, "number_of_species", H5T_STD_U32LE, dims, 1, H5T_NATIVE_INT,
          &geometry->number_of_species.value)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --number_of_sites */
    dims[0] = 1;
    if ((err = utils_hdf5_write_attr
         (geometry->group_id, "number_of_sites", H5T_STD_U32LE, dims, 1, H5T_NATIVE_INT,
          &geometry->number_of_sites.value)) != ESCDF_SUCCESS) {
        return err;
    }

    /* --number_of_symmetry_operations */
    dims[0] = 1;
    if ((err = utils_hdf5_write_attr
         (geometry->group_id, "number_of_symmetry_operations", H5T_STD_U32LE, dims, 1, H5T_NATIVE_INT,
          &geometry->number_of_symmetry_operations.value)) != ESCDF_SUCCESS) {
        return err;
    }

    /* write datasets of the group: */

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_geometry_set_number_of_physical_dimensions(
        escdf_geometry_t *geometry, const int number_of_physical_dimensions)
{
    /* checks on the value */
    FULFILL_OR_RETURN(number_of_physical_dimensions == 3, ESCDF_EVALUE);

    /* set the value and status */
    geometry->number_of_physical_dimensions = _int_set(number_of_physical_dimensions);

    return ESCDF_SUCCESS;
}

int escdf_geometry_get_number_of_physical_dimensions(
        const escdf_geometry_t *geometry)
{
    /* check if the value is set */
    FULFILL_OR_RETURN_VAL(geometry->number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING, 0);

    return geometry->number_of_physical_dimensions.value;
}

bool escdf_geometry_is_set_number_of_physical_dimensions(
        const escdf_geometry_t *geometry)
{
    return geometry->number_of_physical_dimensions.is_set;
}

escdf_errno_t escdf_geometry_set_dimension_types(
        escdf_geometry_t *geometry, const int *dimension_types, const size_t len)
{
    unsigned int i, c;

    /* checks on the value */
    FULFILL_OR_RETURN(geometry->number_of_physical_dimensions.is_set, ESCDF_ESIZE_MISSING);
    FULFILL_OR_RETURN(len == geometry->number_of_physical_dimensions.value, ESCDF_ESIZE);
    c = 0;
    for (i = 0; i < len; i++) {
        FULFILL_OR_RETURN(dimension_types[i] >= 0 && dimension_types[i] < 3, ESCDF_ERANGE);
        if (dimension_types[i] == 2) c++;
    }
    FULFILL_OR_RETURN(c <= 1, ESCDF_ERANGE);

    /* allocate the array and set the value */
    free(geometry->dimension_types);
    geometry->dimension_types = malloc(sizeof(int) * len);
    memcpy(geometry->dimension_types, dimension_types, sizeof(int) * len);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_geometry_get_dimension_types(
        const escdf_geometry_t *geometry, int *dimension_types, const size_t len)
{
    FULFILL_OR_RETURN(geometry->dimension_types, ESCDF_EUNINIT);
    FULFILL_OR_RETURN(len == geometry->number_of_physical_dimensions.value, ESCDF_ESIZE);

    memcpy(dimension_types, geometry->dimension_types, sizeof(int) * len);

    return ESCDF_SUCCESS;
}

const int * escdf_geometry_ptr_dimension_types(
        const escdf_geometry_t *geometry)
{
    FULFILL_OR_RETURN_VAL(geometry, ESCDF_EOBJECT, NULL);

    return geometry->dimension_types;
}

escdf_errno_t escdf_geometry_set_embedded_system(escdf_geometry_t *geometry,
                                                 const bool embedded_system)
{
    geometry->embedded_system = _bool_set(embedded_system);

    return ESCDF_SUCCESS;
}

bool escdf_geometry_get_embedded_system(const escdf_geometry_t *geometry)
{
    FULFILL_OR_RETURN_VAL(geometry->embedded_system.is_set, ESCDF_EUNINIT, true);
    
    return geometry->embedded_system.value;
}

bool escdf_geometry_is_set_embedded_system(
        const escdf_geometry_t *geometry)
{
    return geometry->embedded_system.is_set;
}

escdf_errno_t escdf_geometry_set_number_of_species(
        escdf_geometry_t *geometry, const int number_of_species)
{
    /* set the value and status */
    geometry->number_of_species = _int_set(number_of_species);

    return ESCDF_SUCCESS;
}

int escdf_geometry_get_number_of_species(
        const escdf_geometry_t *geometry)
{
    /* check if the value is set */
    FULFILL_OR_RETURN_VAL(geometry->number_of_species.is_set, ESCDF_ESIZE_MISSING, 0);

    return geometry->number_of_species.value;
}

bool escdf_geometry_is_set_number_of_species(
        const escdf_geometry_t *geometry)
{
    return geometry->number_of_species.is_set;
}

escdf_errno_t escdf_geometry_set_number_of_sites(
        escdf_geometry_t *geometry, const int number_of_sites)
{
    /* set the value and status */
    geometry->number_of_sites = _int_set(number_of_sites);

    return ESCDF_SUCCESS;
}

int escdf_geometry_get_number_of_sites(
        const escdf_geometry_t *geometry)
{
    /* check if the value is set */
    FULFILL_OR_RETURN_VAL(geometry->number_of_sites.is_set, ESCDF_ESIZE_MISSING, 0);

    return geometry->number_of_sites.value;
}

bool escdf_geometry_is_set_number_of_sites(
        const escdf_geometry_t *geometry)
{
    return geometry->number_of_sites.is_set;
}

escdf_errno_t escdf_geometry_set_number_of_symmetry_operations(
        escdf_geometry_t *geometry, const int number_of_symmetry_operations)
{
    /* set the value and status */
    geometry->number_of_symmetry_operations = _int_set(number_of_symmetry_operations);

    return ESCDF_SUCCESS;
}

int escdf_geometry_get_number_of_symmetry_operations(
        const escdf_geometry_t *geometry)
{
    /* check if the value is set */
    FULFILL_OR_RETURN_VAL(geometry->number_of_symmetry_operations.is_set, ESCDF_ESIZE_MISSING, 0);

    return geometry->number_of_symmetry_operations.value;
}

bool escdf_geometry_is_set_number_of_symmetry_operations(
        const escdf_geometry_t *geometry)
{
    return geometry->number_of_symmetry_operations.is_set;
}