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

#include "escdf_densities.h"

#include "utils.h"

struct _density_t {
    escdf_density_type_t type;
    union {
        _grid_scalarfield_t *grid_scalarfield;
    };
    _density_t *next;
};

struct _escdf_densities_t {
    char *path;
    _uint_set_t number_of_densities;

    struct _density_t *densities;
};

escdf_densities_t* escdf_densities_new(const char *path)
{
    escdf_densities_t *densities;

    densities = malloc(sizeof(escdf_densities_t));
    if (!path || !path[0]) {
        densities->path = strdup("densities");
    } else {
        densities->path = strdup(path);
    }
    densities->number_of_densities.value = 0;
    densities->number_of_densities.value = false;
    densities->densities = NULL;
    
    return densities;
}

void escdf_densities_free(escdf_densities_t *densities)
{
    struct _density_t *iter, *current;

    if (!densities)
        return;

    free(densities->path);
    iter = densities->densities;
    while (iter) {
        switch (iter->type) {
        case (ESCDF_DENSITY_GRID_PSEUDO):
        case (ESCDF_DENSITY_GRID_CORE):
        case (ESCDF_DENSITY_GRID_TOTAL):
            escdf_grid_scalarfield_free(iter->grid_scalarfield);
            break;
        default:
            escdf_return_if_fails(false);
        }
        current = iter;
        iter = iter->next;
        free(current);
    }
}

static escdf_err_t _density_from_group(hid_t gid, const gchar *path,
                                       struct _density_t **density)
{
    htri_t exists;
    escdf_err_t err;

    if ((exists = H5Lexists(gid, "values_on_grid",  H5P_DEFAULT)) < 0)
        return escdf_error(exists);
    if (exists == TRUE) {
        *density = malloc(sizeof(struct _density_t));
        err = escdf_grid_scalarfield_read_metadata(&(*density)->grid_scalarfield, path);
        if (err == ESCDF_NO_ERROR) {
            escdf_grid_scalarfield_get_type((*density)->type = ...;
        }
        return err;
    }

    /* No known density in thhis group. */
    return -1;
}

escdf_err_t escdf_densities_read_metadata(escdf_densities_t **densities, const char *path)
{
    herr_t err;
    H5G_info_t ginfo;
    hsize_t n;
    
    escdf_return_val_if_fails(densities, NULL);

    *densities = escdf_densities_new(path);

    /* We traverse objects of current group to find which types of
       density this group contains, based on main ESCDF variable. */
    n = 0;
    do {
        err = H5Gget_info_by_idx(loc_id, densities->path,
                                 H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, n++,
                                 &ginfo, H5P_DEFAULT);
        if (err >= 0) {
            
        }
    } while (err >= 0);
    if (n > 1) {
        densities->number_of_densities.value = n - 1;
        densities->number_of_densities.is_set = true;
    }

    return ESCDF_NO_ERROR;
}

unsigned int escdf_densities_get_number_of_densities(const escdf_densities_t *densities)
{
    escdf_return_val_if_fails(densities, 0);

    return densities->number_of_densities.value;
}

static const struct _density_t* _get_density_from_i(const escdf_densities_t *densities,
                                               const unsigned int i_density)
{
    unsigned int i;
    struct _density_t *iter;
    
    escdf_return_val_if_fails(densities, NULL);
    escdf_return_val_if_fails(i_density >= densities->number_of_densities, NULL);

    for (iter = densities->densities, i = 0; iter; iter = iter->next, i++) {
        if (i == i_density)
            return iter;
    }
    return NULL;
}
static const struct _density_t* _get_density_from_type(const escdf_densities_t *densities,
                                                       const escdf_density_type_t type)
{
    struct _density_t *iter;
    
    escdf_return_val_if_fails(densities, NULL);

    for (iter = densities->densities; iter; iter = iter->next) {
        if (iter->type == type)
            return iter;
    }
    return NULL;
}

escdf_density_type_t escdf_densities_get_type(const escdf_densities_t *densities,
                                              const unsigned int i_density)
{
    const struct _density_t *density;

    density = _get_density_from_i(densities, i_density);
    escdf_return_val_if_fails(densities, ESCDF_DENSITY_UNKNOWN);
    
    return density->type;
}

escdf_grid_scalarfield_t* escdf_densities_get_grid_scalarfield(const escdf_densities_t *densities,
                                                               const unsigned int i_density)
{
    const struct _density_t *density;

    density = _get_density_from_i(densities, i_density);
    escdf_return_val_if_fails(densities, NULL);
    escdf_return_val_if_fails(density->type == ESCDF_DENSITY_GRID_PSEUDO
                              || density->type == ESCDF_DENSITY_GRID_CORE
                              || density->type == ESCDF_DENSITY_GRID_TOTAL
                              , NULL);

    return density->grid_scalarfield;
}
escdf_grid_scalarfield_t* escdf_densities_get_grid_scalarfield_from_type(const escdf_densities_t *densities,
                                                                         const escdf_density_type_t type)
{
    const struct _density_t *density;

    escdf_return_val_if_fails(type == ESCDF_DENSITY_GRID_PSEUDO
                              || type == ESCDF_DENSITY_GRID_CORE
                              || type == ESCDF_DENSITY_GRID_TOTAL
                              , NULL);
    
    density = _get_density_from_type(densities, type);
    escdf_return_val_if_fails(densities, NULL);

    return density->grid_scalarfield;
}
