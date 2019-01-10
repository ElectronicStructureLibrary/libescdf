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

#ifndef LIBESCDF_SYSTEM_H
#define LIBESCDF_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>

#include "escdf_error.h"
#include "escdf_handle.h"



/******************************************************************************
 * Data structures                                                            *
 ******************************************************************************/

typedef struct escdf_system escdf_system_t;

/******************************************************************************
 * Low-level creators and destructors                                         *
 ******************************************************************************/

/**
 * This function takes care of creating an instance of escdf_system_t by
 * allocating the corresponding memory. It also initializes all its contents to
 * the default values.
 *
 * @return instance of the system data type.
 */
escdf_system_t * escdf_system_new();

/**
 * Free all memory associated with the system.
 *
 * @param[in,out] system: the system.
 */
void escdf_system_free(escdf_system_t *system);

/**
 * Open the group associated with the system. If a name is given,
 * the group path will be 'system/name', otherwise, if name is NULL, it
 * is 'system'. If the group does not exist, an error is returned.
 *
 * @param[in,out] system: the system.
 * @param[in] handle: the file handle.
 * @param[in] name: name to the system group.
 * @return Error code.
 */
escdf_errno_t escdf_system_open_group(escdf_system_t *system, const escdf_handle_t *handle, const char *name);

/**
 * Create a group to be associated with the system. If a name is given,
 * the group path will be 'system/name', otherwise, if name is NULL, it
 * is 'system'. If the group already exist, all previous contents of the group are
 * deleted.
 *
 * @param[in,out] system: the system.
 * @param[in] handle: the file handle.
 * @param[in] name: name to the system group.
 * @return Error code.
 */
escdf_errno_t escdf_system_create_group(escdf_system_t *system, const escdf_handle_t *handle, const char *name);

/**
 * Closes the group associated with the system.
 *
 * @param[in,out] system: the system.
 * @return Error code.
 */
escdf_errno_t escdf_system_close_group(escdf_system_t *system);


/******************************************************************************
 * High-level creators and destructors                                        *
 ******************************************************************************/

/**
 * This function takes care of the following tasks:
 * - call escdf_system_new to create an instance of the structure.
 * - call escdf_system_open_group. Note that this function will return an error if the group does not exist.
 * - call escdf_system_read_metadata to read all the metadata from the file and store it in memory.
 *
 * @param[in] handle: the file/group handle defining the root where to open
 * the "/system" group.
 * @param[in] name: If NULL, assume that the system is stored in the
 * "/system" group, otherwise "/system/name" is used.
 * @return instance of the system data type.
 */
escdf_system_t * escdf_system_open(const escdf_handle_t *handle, const char *name);

/**
 * This function performs the following tasks:
 * - call escdf_system_new to create an instance of the structure.
 * - call escdf_system_create_group.
 *
 * @param[in] handle: the file/group handle defining the root where to open
 * the "/system" group.
 * @param[in] name: If NULL, assume that the system is stored in the
 * "/system" group, otherwise "/system/name" is used.
 * @return instance of the system data type.
 *
 */
escdf_system_t * escdf_system_create(const escdf_handle_t *handle, const char *name);

/**
 * This function performs the following tasks:
 * - call escdf_system_close_group to close the group.
 * - call escdf_system_free to free all memory.
 *
 * @param[in,out] system: the system.
 * @return error code.
 */
escdf_errno_t escdf_system_close(escdf_system_t *system);


/******************************************************************************
 * Metadata functions                                                         *
 ******************************************************************************/

/**
 * This routine reads ll the metadata stored in the group, and stores
 * the information in the system data type.
 *
 * @param[out] system: pointer to instance of the system group.
 * @return error code.
 */
escdf_errno_t escdf_system_read_metadata(escdf_system_t *system);

/**
 * Sets system_name in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] system_name: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_system_name(
    escdf_system_t *system, const char *system_name);

/**
 * Get the value of system_name stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] system_name: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_system_name(
    const escdf_system_t *system, char *system_name);

/**
 * Sets number_of_physical_dimensions in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] number_of_physical_dimensions: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_number_of_physical_dimensions(
    escdf_system_t *system, unsigned int number_of_physical_dimensions);

/**
 * Get the value of number_of_physical_dimensions stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] number_of_physical_dimensions: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_number_of_physical_dimensions(
    const escdf_system_t *system, unsigned int *number_of_physical_dimensions);

/**
 * Sets dimension_types in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] dimension_types: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_dimension_types(
    escdf_system_t *system, const int *dimension_types);

/**
 * Get the value of dimension_types stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] dimension_types: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_dimension_types(
    const escdf_system_t *system, int *dimension_types);


/**
 * Sets embedded_system in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] embedded_system: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_embedded_system(
    escdf_system_t *system, bool embedded_system);

/**
 * Get the value of embedded_system stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] embedded_system: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_embedded_system(
    const escdf_system_t *system, bool *embedded_system);

/**
 * Sets number_of_species in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] number_of_species: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_number_of_species(
    escdf_system_t *system, unsigned int number_of_species);

/**
 * Get the value of number_of_species stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] number_of_species: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_number_of_species(
    const escdf_system_t *system, unsigned int *number_of_species);

/**
 * Sets number_of_sites in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] number_of_sites: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_number_of_sites(
    escdf_system_t *system, unsigned int number_of_sites);

/**
 * Get the value of number_of_sites stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] number_of_sites: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_number_of_sites(
    const escdf_system_t *system, unsigned int *number_of_sites);

/**
 * Sets number_of_symmetry_operations in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] number_of_symmetry_operations: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_number_of_symmetry_operations(
    escdf_system_t *system, unsigned int number_of_symmetry_operations);

/**
 * Get the value of number_of_symmetry_operations stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] number_of_symmetry_operations: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_number_of_symmetry_operations(
    const escdf_system_t *system, unsigned int *number_of_symmetry_operations);

/**
 * Sets lattice_vectors in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] lattice_vectors: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_lattice_vectors(
    escdf_system_t *system, const double *lattice_vectors);

/**
 * Get the value of lattice_vectors stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] lattice_vectors: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_lattice_vectors(
    const escdf_system_t *system, double *lattice_vectors);

/**
 * Sets spacegroup_3D_number in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] spacegroup_3D_number: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_spacegroup_3D_number(
    escdf_system_t *system, unsigned int spacegroup_3D_number);

/**
 * Get the value of spacegroup_3D_number stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] spacegroup_3D_number: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_spacegroup_3D_number(
    const escdf_system_t *system, unsigned int *spacegroup_3D_number);

/**
 * Sets symmorphic in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] symmorphic: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_symmorphic(
    escdf_system_t *system, bool symmorphic);

/**
 * Get the value of symmorphic stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] symmorphic: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_symmorphic(
    const escdf_system_t *system, bool *symmorphic);

/**
 * Sets time_reversal_symmetry in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] time_reversal_symmetry: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_time_reversal_symmetry(
    escdf_system_t *system, bool time_reversal_symmetry);

/**
 * Get the value of time_reversal_symmetry stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] time_reversal_symmetry: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_time_reversal_symmetry(
    const escdf_system_t *system, bool *time_reversal_symmetry);

/**
 * Sets bulk_regions_for_semi_infinite_dimension in the system data type.
 *
 * @param[in,out] system: instance of the system group.
 * @param[in] bulk_regions_for_semi_infinite_dimension: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_system_set_bulk_regions_for_semi_infinite_dimension(
    escdf_system_t *system, const double *bulk_regions_for_semi_infinite_dimension);

/**
 * Get the value of bulk_regions_for_semi_infinite_dimension stored in the system data
 * type.
 *
 * @param[in] system: instance of the system group.
 * @param[out] bulk_regions_for_semi_infinite_dimension: the value of the variable
 * @return error code.
 */
escdf_errno_t escdf_system_get_bulk_regions_for_semi_infinite_dimension(
    const escdf_system_t *system, double *bulk_regions_for_semi_infinite_dimension);



/******************************************************************************
 * Data functions                                                             *
 ******************************************************************************/

/**
 * Writes the magnetic_moment_directions variable to the system group.
 *
 * @param[in] system: instance of the system group.
 * @param[in] buffer: the buffer where the data is stored in memory.
 * @param[in] start:
 * @param[in] count:
 * @param[in] map:
 * @return error code.
 */
escdf_errno_t escdf_system_write_magnetic_moment_directions(
    const escdf_system_t *system, const double *buffer,
    const unsigned int *start, const unsigned int *count,
    const unsigned int *map);
/**
 * Reads the magnetic_moment_directions variable from the system group.
 *
 * @param[in] system: instance of the system group.
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
escdf_errno_t escdf_system_read_magnetic_moment_directions(
    const escdf_system_t *system, double *buffer,
    const unsigned int *start, const unsigned int *count,
    const unsigned int *map);


#ifdef __cplusplus
}
#endif

#endif
