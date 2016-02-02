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

#ifndef LIBESCDF_SCALARFIELDS_H
#define LIBESCDF_SCALARFIELDS_H

#include "escdf_error.h"
#include "escdf_handle.h"

/* to be removed later when in utils.h */
#include <string.h>

/******************************************************************************
 * Data structures                                                            *
 ******************************************************************************/

struct _escdf_grid_scalarfield_t;
typedef struct _escdf_grid_scalarfield_t escdf_grid_scalarfield_t;


/******************************************************************************
 * Global functions                                                           *
 ******************************************************************************/

/**
 * Creates a new instance of the scalarfield data type.
 *
 * @param[in] path: the path to the ESCDF scalarfield group.
 * @return instance of the scalarfield data type.
 */
escdf_grid_scalarfield_t* escdf_grid_scalarfield_new(const char *path);

/**
 * Free all memory associated with the scalarfield group.
 *
 * @param[in,out] scalarfield: the scalarfield.
 */
void escdf_grid_scalarfield_free(escdf_grid_scalarfield_t *scalarfield);


/******************************************************************************
 * Metadata functions                                                         *
 ******************************************************************************/

/**
 * Given a path to a ESCDF scalarfield group, this routines opens the group, reads
 * all the metadata stored in the group, and stores the information in the
 * scalarfield data type.
 *
 * @param[out] scalarfield: pointer to instance of the scalarfield group.
 * @param[in] path: the path to the ESCDF scalarfield group.
 * @return error code.
 */
escdf_errno_t escdf_grid_scalarfield_read_metadata(escdf_grid_scalarfield_t **scalarfield,
                                                   hid_t file_id, const char *path);

escdf_errno_t escdf_grid_scalarfield_write_metadata(const escdf_grid_scalarfield_t *scalarfield,
                                                    hid_t file_id);

/**
 * Sets the value of number_of_physical_dimensions in the geometry data type.
 *
 * @param[in,out] geometry: instance of the geometry group.
 * @param[in] number_of_physical_dimensions: the value of the variable to be set.
 * @return error code.
 */
escdf_errno_t escdf_grid_scalarfield_set_number_of_physical_dimensions(escdf_grid_scalarfield_t *scalarfield,
                                                                       const unsigned int number_of_physical_dimensions);
unsigned int escdf_grid_scalarfield_get_number_of_physical_dimensions(const escdf_grid_scalarfield_t *scalarfield);

escdf_errno_t escdf_grid_scalarfield_set_dimension_types(escdf_grid_scalarfield_t *scalarfield,
                                                         const unsigned int *dimension_types,
                                                         const size_t len);
escdf_errno_t escdf_grid_scalarfield_get_dimension_types(const escdf_grid_scalarfield_t *scalarfield,
                                                         unsigned int *dimension_types,
                                                         const size_t len);
const unsigned int* escdf_grid_scalarfield_ptr_dimension_types(const escdf_grid_scalarfield_t *scalarfield);

escdf_errno_t escdf_grid_scalarfield_set_lattice_vectors(escdf_grid_scalarfield_t *scalarfield,
                                                         const double *lattice_vectors,
                                                         const size_t len);
escdf_errno_t escdf_grid_scalarfield_get_lattice_vectors(const escdf_grid_scalarfield_t *scalarfield,
                                                         double *lattice_vectors,
                                                         const size_t len);
const double* escdf_grid_scalarfield_ptr_lattice_vectors(const escdf_grid_scalarfield_t *scalarfield);

escdf_errno_t escdf_grid_scalarfield_set_number_of_grid_points(escdf_grid_scalarfield_t *scalarfield,
                                                               const unsigned int *number_of_grid_points,
                                                               const size_t len);
escdf_errno_t escdf_grid_scalarfield_get_number_of_grid_points(const escdf_grid_scalarfield_t *scalarfield,
                                                               unsigned int *number_of_grid_points,
                                                               const size_t len);
const unsigned int* escdf_grid_scalarfield_ptr_number_of_grid_points(const escdf_grid_scalarfield_t *scalarfield);

escdf_errno_t escdf_grid_scalarfield_set_number_of_components(escdf_grid_scalarfield_t *scalarfield,
                                                              const unsigned int number_of_components);
unsigned int escdf_grid_scalarfield_get_number_of_components(const escdf_grid_scalarfield_t *scalarfield);

escdf_errno_t escdf_grid_scalarfield_set_real_or_complex(escdf_grid_scalarfield_t *scalarfield,
                                                         const unsigned int real_or_complex);
unsigned int escdf_grid_scalarfield_get_real_or_complex(const escdf_grid_scalarfield_t *scalarfield);

escdf_errno_t escdf_grid_scalarfield_set_use_default_ordering(escdf_grid_scalarfield_t *scalarfield,
                                                              const bool use_default_ordering);
bool escdf_grid_scalarfield_get_use_default_ordering(const escdf_grid_scalarfield_t *scalarfield);

escdf_errno_t escdf_grid_scalarfield_serialise(escdf_grid_scalarfield_t *scalarfield, FILE *f);

#endif
