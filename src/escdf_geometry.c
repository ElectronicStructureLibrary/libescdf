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
    _int_set_t number_of_physical_dimensions;
    int *dimension_types;
    _bool_set_t embedded_system;

    /* Information about which data is present in the group */
    bool magnetic_moment_directions_is_present;
};


/******************************************************************************
 * Global functions                                                           *
 ******************************************************************************/

escdf_geometry_t * escdf_geometry_new(const escdf_handle_t *handle,
        const char *name)
{
    escdf_geometry_t *geometry;

    geometry = (escdf_geometry_t *) malloc(sizeof(escdf_geometry_t));
    FULFILL_OR_RETURN(geometry != NULL, ESCDF_ENOMEM)

    /* check if "geometries" group exists; if not, create it */
    if (!utils_hdf5_check_present(handle->group_id, "geometries")) {
        geometry->group_id = H5Gcreate(handle->group_id, "geometries",
                                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    } else {
        geometry->group_id = H5Gopen(handle->group_id, "geometries",
                                     H5P_DEFAULT);
    }

    /* check if specific geometry group exists and open it; if not, create it */
    if (!utils_hdf5_check_present(geometry->group_id, name)) {
        geometry->group_id = H5Gcreate(geometry->group_id, name, H5P_DEFAULT,
                                       H5P_DEFAULT, H5P_DEFAULT);
    }
    else {
        geometry->group_id = H5Gopen(geometry->group_id, name, H5P_DEFAULT);
    }

    /* no metadata set at the moment */
    geometry->number_of_physical_dimensions.is_set = false;
    geometry->dimension_types = NULL;
    geometry->embedded_system.is_set = false;

    /* don't yet know which data is present, so set all to false */
    geometry->magnetic_moment_directions_is_present = false;

    return geometry;
}

escdf_errno_t escdf_geometry_free(escdf_geometry_t * geometry)
{
    herr_t herr_status;

    /* close the group */
    herr_status = H5Gclose(geometry->group_id);
    FULFILL_OR_RETURN(herr_status >= 0, herr_status);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_geometry_read_metadata(escdf_geometry_t *geometry)
{
    /* read attributes of the group: */

    /* --number_of_physical_dimensions */
    int range[2] = {3, 3};
    utils_hdf5_read_int(geometry->group_id, "number_of_physical_dimensions",
                        geometry->number_of_physical_dimensions, range);

    /* read datasets of the group: */

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_geometry_write_metadata(const escdf_geometry_t *geometry)
{
    hid_t group_id, dataspace_id, attribute_id;
    hsize_t dims;
    herr_t herr_status;

    /* write attributes of the group: */

    /* --number_of_physical_dimensions */
    if (geometry->number_of_physical_dimensions.is_set) {
        dims = 1;
        dataspace_id = H5Screate_simple(1, &dims, NULL);
        attribute_id = H5Acreate2(geometry->group_id,
                "number_of_physical_dimensions", H5T_NATIVE_INT, dataspace_id,
                H5P_DEFAULT, H5P_DEFAULT);
        herr_status = H5Awrite(attribute_id, H5T_NATIVE_INT,
                &geometry->number_of_physical_dimensions.value);
        FULFILL_OR_RETURN(herr_status >= 0, herr_status);
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