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

#ifndef LIBESCDF_GEOMETRY_H
#define LIBESCDF_GEOMETRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>

#include "escdf_error.h"
#include "escdf_handle.h"
#include "utils.h"



/******************************************************************************
 * Data structures                                                            *
 ******************************************************************************/

typedef struct escdf_geometry escdf_geometry_t;

/******************************************************************************
 * Low-level creators and destructors                                         *
 ******************************************************************************/

/**
 * This function takes care of creating an instance of escdf_geometry_t by
 * allocating the corresponding memory. It also initializes all its contents to
 * the default values.
 *
 * @return instance of the geometry data type.
 */
escdf_geometry_t * escdf_geometry_new();

/**
 * Free all memory associated with the geometry.
 *
 * @param[in,out] geometry: the geometry.
 */
void escdf_geometry_free(escdf_geometry_t * geometry);

/**
 * Open the group associated with the geometry. If a path is given,
 * the group path will be 'geometries/path', otherwise, if path is NULL, it
 * is 'geometries'. If the group does not exist, an error is returned.
 *
 * @param[in,out] geometry: the geometry.
 * @param[in] handle: the file handle.
 * @param[in] path: path to the geometry group.
 * @return Error code.
 */
escdf_errno_t escdf_geometry_open_group(escdf_geometry_t *geometry, escdf_handle_t *handle, const char *path);

/**
 * Create a group to be associated with the geometry. If a path is given,
 * the group path will be 'geometries/path', otherwise, if path is NULL, it
 * is 'geometries'. If the group already exist, all previous contents of the group are
 * deleted.
 *
 * @param[in,out] geometry: the geometry.
 * @param[in] handle: the file handle.
 * @param[in] path: path to the geometry group.
 * @return Error code.
 */
escdf_errno_t escdf_geometry_create_group(escdf_geometry_t *geometry, escdf_handle_t *handle, const char *path);

/**
 * Closes the group associated with the geometry.
 *
 * @param[in,out] geometry: the geometry.
 * @return Error code.
 */
escdf_errno_t escdf_geometry_close_group(escdf_geometry_t *geometry);


/******************************************************************************
 * High-level creators and destructors                                        *
 ******************************************************************************/

/**
 * This function takes care of creating an instance of escdf_geometry_t by
 * allocating the memory. It also initializes all its contents to the default
 * values.
 *
 * @param[in] handle: the file/group handle defining the root where to open
 * the "/geometries" group.
 * @param[in] name: If NULL, assume that the geometry is stored in the
 * "/geometries" group, otherwise "/geometries/name" is used.
 * @return instance of the geometry data type.
 */
escdf_geometry_t * escdf_geometry_open(const escdf_handle_t *handle,
        const char *name);

/**
 * Free all memory associated with the geometry group and close the group.
 *
 * @param[in,out] geometry: the geometry.
 * @return error code.
 */
escdf_errno_t escdf_geometry_close(escdf_geometry_t * geometry);


/******************************************************************************
 * Metadata functions                                                         *
 ******************************************************************************/

/**
 * Given a path to a ESCDF geometry group, this routines opens the group, reads
 * all the metadata stored in the group, and stores the information in the
 * geometry data type.
 *
 * @param[out] geometry: pointer to instance of the geometry group.
 * @param[in] path: the path to the ESCDF geometry group.
 * @return error code.
 */
escdf_errno_t escdf_geometry_read_metadata(escdf_geometry_t *geometry);

/**
 * Sets system_name in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] system_name: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_system_name(
    escdf_geometry_t *geometry, const char *system_name);

/**
 * Get the value of system_name stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] system_name: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_system_name(
    const escdf_geometry_t *geometry, char *system_name);

/**
 * Sets number_of_physical_dimensions in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] number_of_physical_dimensions: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_number_of_physical_dimensions(
    escdf_geometry_t *geometry, unsigned int number_of_physical_dimensions);

/**
 * Get the value of number_of_physical_dimensions stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] number_of_physical_dimensions: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_number_of_physical_dimensions(
    const escdf_geometry_t *geometry, unsigned int *number_of_physical_dimensions);

/**
 * Sets dimension_types in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] dimension_types: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_dimension_types(
    escdf_geometry_t *geometry, const int *dimension_types);

/**
 * Get the value of dimension_types stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] dimension_types: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_dimension_types(
    const escdf_geometry_t *geometry, int *dimension_types);


/**
 * Sets embedded_system in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] embedded_system: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_embedded_system(
    escdf_geometry_t *geometry, bool embedded_system);

/**
 * Get the value of embedded_system stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] embedded_system: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_embedded_system(
    const escdf_geometry_t *geometry, bool *embedded_system);

/**
 * Sets number_of_species in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] number_of_species: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_number_of_species(
    escdf_geometry_t *geometry, unsigned int number_of_species);

/**
 * Get the value of number_of_species stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] number_of_species: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_number_of_species(
    const escdf_geometry_t *geometry, unsigned int *number_of_species);

/**
 * Sets number_of_sites in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] number_of_sites: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_number_of_sites(
    escdf_geometry_t *geometry, unsigned int number_of_sites);

/**
 * Get the value of number_of_sites stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] number_of_sites: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_number_of_sites(
    const escdf_geometry_t *geometry, unsigned int *number_of_sites);

/**
 * Sets number_of_symmetry_operations in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] number_of_symmetry_operations: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_number_of_symmetry_operations(
    escdf_geometry_t *geometry, unsigned int number_of_symmetry_operations);

/**
 * Get the value of number_of_symmetry_operations stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] number_of_symmetry_operations: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_number_of_symmetry_operations(
    const escdf_geometry_t *geometry, unsigned int *number_of_symmetry_operations);

/**
 * Sets lattice_vectors in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] lattice_vectors: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_lattice_vectors(
    escdf_geometry_t *geometry, const double *lattice_vectors);

/**
 * Get the value of lattice_vectors stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] lattice_vectors: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_lattice_vectors(
    const escdf_geometry_t *geometry, double *lattice_vectors);

/**
 * Sets spacegroup_3D_number in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] spacegroup_3D_number: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_spacegroup_3D_number(
    escdf_geometry_t *geometry, unsigned int spacegroup_3D_number);

/**
 * Get the value of spacegroup_3D_number stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] spacegroup_3D_number: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_spacegroup_3D_number(
    const escdf_geometry_t *geometry, unsigned int *spacegroup_3D_number);

/**
 * Sets symmorphic in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] symmorphic: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_symmorphic(
    escdf_geometry_t *geometry, bool symmorphic);

/**
 * Get the value of symmorphic stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] symmorphic: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_symmorphic(
    const escdf_geometry_t *geometry, bool *symmorphic);

/**
 * Sets time_reversal_symmetry in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] time_reversal_symmetry: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_time_reversal_symmetry(
    escdf_geometry_t *geometry, bool time_reversal_symmetry);

/**
 * Get the value of time_reversal_symmetry stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] time_reversal_symmetry: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_time_reversal_symmetry(
    const escdf_geometry_t *geometry, bool *time_reversal_symmetry);

/**
 * Sets bulk_regions_for_semi_infinite_dimension in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] bulk_regions_for_semi_infinite_dimension: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_geometry_set_bulk_regions_for_semi_infinite_dimension(
    escdf_geometry_t *geometry, const double bulk_regions_for_semi_infinite_dimension[2]);

/**
 * Get the value of bulk_regions_for_semi_infinite_dimension stored in the geometry data
 * type.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] bulk_regions_for_semi_infinite_dimension: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_geometry_get_bulk_regions_for_semi_infinite_dimension(
    const escdf_geometry_t *geometry, double bulk_regions_for_semi_infinite_dimension[2]);



/******************************************************************************
 * Data functions                                                             *
 ******************************************************************************/

/**
 * Writes the magnetic_moment_directions variable to the geometry group.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[in] buffer: the buffer where the data is stored in memory.
 * @param[in] start:
 * @param[in] count:
 * @param[in] map:
 * @return error code.
 */
escdf_errno_t escdf_geometry_write_magnetic_moment_directions(
        const escdf_geometry_t *geometry, const double *buffer,
        const unsigned int *start, const unsigned int *count,
        const unsigned int *map);
/**
 * Reads the magnetic_moment_directions variable from the geometry group.
 *
 * @param[in] geometry: instance of the geometry group.
 * @param[out] buffer: the buffer where the data is to be stored in memory.
 * @param[in] start: vector of integers specifying the index in the variable
 * where the first of the data values will be read.
 * @param[in] count: vector of integers specifying the edge lengths along each
 * dimension of the block of data values to be read.
 * @param[in] map: vector of integers that specifies the mapping between the
 * dimensions of the variable and the in-memory structure of the internal data
 * array.
 * @return error code.
 */
escdf_errno_t escdf_geometry_read_magnetic_moment_directions(
        const escdf_geometry_t *geometry, double *buffer,
        const unsigned int *start, const unsigned int *count,
        const unsigned int *map);


#ifdef __cplusplus
}
#endif

#endif
