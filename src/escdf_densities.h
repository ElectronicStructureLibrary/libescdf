/* Copyright (C) 2016 Damien Caliste <dcaliste@free.fr>
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

#ifndef LIBESCDF_DENSITIES_H
#define LIBESCDF_DENSITIES_H

#include "escdf_grid_scalarfields.h"

struct _escdf_densities_t;
typedef struct _escdf_densities_t escdf_densities_t;

/* typedef enum { */
/*     ESCDF_GRID_DENSITY, */
/*     ESCDF_ATOM_CENTERED_DENSITY, */
/*     ESCDF_UNKNOWN_DENSITY */
/* } escdf_density_family_t; */

typedef enum {
    ESCDF_DENSITY_GRID_PSEUDO,
    ESCDF_DENSITY_GRID_CORE,
    ESCDF_DENSITY_GRID_TOTAL,
    ESCDF_DENSITY_ATOM_CORE,
    ESCDF_DENSITY_UNKNOWN
} escdf_density_type_t;



escdf_densities_t* escdf_densities_new(const char *path);

void escdf_densities_free(escdf_densities_t *densities);


escdf_err_t escdf_densities_read_metadata(escdf_densities_t **densities, const char *path);

unsigned int escdf_densities_get_number_of_densities(const escdf_densities_t *densities);

escdf_density_type_t escdf_densities_get_type(const escdf_densities_t *densities,
                                              const unsigned int i_density);

escdf_grid_scalarfield_t* escdf_densities_get_grid_scalarfield(const escdf_densities_t *densities,
                                                               const unsigned int i_density);
escdf_grid_scalarfield_t* escdf_densities_get_grid_scalarfield_from_type(const escdf_densities_t *densities,
                                                                         const escdf_density_type_t type);

#endif
