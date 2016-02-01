/*
 Copyright (C) 2016 M. Oliveira

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

escdf_geometry_t * escdf_geometry_new(const hid_t file, const char *path)
{
    escdf_geometry_t *geometry = (escdf_geometry_t *) malloc(sizeof(escdf_geometry_t));
    hid_t lapl_id;
    herr_t herr_status;
    htri_t htri_status;

    /* check if "/geometry" exists; if not, create it */
    lapl_id = H5Pcreate(H5P_LINK_ACCESS);
    htri_status = H5Lexists(file, "/geometry", lapl_id);
    if (!htri_status) {
        geometry->group_id = H5Gcreate(file, "/geometry", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        herr_status = H5Gclose(geometry->group_id);
    }

    /* check if specific geometry group exists and open it; if not, create it */
    htri_status = H5Lexists(file, path, lapl_id);
    if (!htri_status) {
        geometry->group_id = H5Gcreate(file, path, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    else {
        geometry->group_id = H5Gopen(file, path, H5P_DEFAULT);
    }
    herr_status = H5Pclose(lapl_id);

    /* no metadata set at the moment */
    geometry->number_of_physical_dimensions.is_set = false;
    geometry->dimension_types = NULL;
    geometry->embedded_system.is_set = false;

    /* don't yet know which data is present, so set all to false */
    geometry->magnetic_moment_directions_is_present = false;

    return geometry;
}

void escdf_geometry_free(escdf_geometry_t * geometry)
{
    herr_t herr_status;

    /* close the group */
    herr_status = H5Gclose(geometry->group_id);
}

escdf_error_t escdf_geometry_read_metadata(escdf_geometry_t *geometry)
{
    escdf_error_t error;
    hid_t attribute_id;
    herr_t herr_status;
    htri_t htri_status;

    /* read attributes of the group: */

    /* --number_of_physical_dimensions */
    htri_status = H5Aexists(geometry->group_id, "number_of_physical_dimensions");
    if (htri_status) {
      geometry->number_of_physical_dimensions.is_set = true;
      attribute_id = H5Aopen(geometry->group_id, "number_of_physical_dimensions", H5P_DEFAULT);
      herr_status = H5Aread(attribute_id, H5T_NATIVE_INT, &geometry->number_of_physical_dimensions.value);
    }

    /* read datasets of the group */

    return error;
}

escdf_error_t escdf_geometry_write_metadata(const escdf_geometry_t *geometry)
{
    escdf_error_t error;
    hid_t group_id, dataspace_id, attribute_id;
    hsize_t dims;
    herr_t herr_status;

    /* write attributes of the group */

    /* --number_of_physical_dimensions */
    if (geometry->number_of_physical_dimensions.is_set) {
        dims = 1;
        dataspace_id = H5Screate_simple(1, &dims, NULL);
        attribute_id = H5Acreate2(geometry->group_id, "number_of_physical_dimensions", H5T_NATIVE_INT,
                                  dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
        herr_status = H5Awrite(attribute_id, H5T_NATIVE_INT, &geometry->number_of_physical_dimensions.value);
    }

    /* write datasets of the group */

    return error;
}

escdf_error_t escdf_geometry_set_number_of_physical_dimensions(escdf_geometry_t *geometry,
                                                               const int number_of_physical_dimensions)
{
    escdf_error_t error;

    /* checks on the value */
    if (number_of_physical_dimensions != 3) {
        error.id = 1;
        return error;
    }

    /* set the value */
    geometry->number_of_physical_dimensions.value = number_of_physical_dimensions;

    /* set the status */
    geometry->number_of_physical_dimensions.is_set = true;

    return error;
}

int escdf_geometry_get_number_of_physical_dimensions(const escdf_geometry_t *geometry)
{
    escdf_error_t error;

    /* check if the value is set */
    if (!geometry->number_of_physical_dimensions.is_set) {
        error.id = 1;
    }

    return geometry->number_of_physical_dimensions.value;
}

bool escdf_geometry_is_set_number_of_physical_dimensions(const escdf_geometry_t *geometry)
{
    return geometry->number_of_physical_dimensions.is_set;
}