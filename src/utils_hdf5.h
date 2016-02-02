/*
  Copyright (C) 2016 D. Caliste, M. Oliveira

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

#ifndef LIBESCDF_utils_hdf5_H
#define LIBESCDF_utils_hdf5_H

#include "utils.h"

bool utils_hdf5_check_present(hid_t loc_id, const char *name);

escdf_errno_t utils_hdf5_check_shape(hid_t dtspace_id, hsize_t *dims,
                                     unsigned int ndims);

escdf_errno_t utils_hdf5_check_attr(hid_t loc_id, const char *name,
                                    hsize_t *dims, unsigned int ndims,
                                    hid_t *attr_pt);

escdf_errno_t utils_hdf5_check_dtset(hid_t loc_id, const char *name,
                                     hsize_t *dims, unsigned int ndims,
                                     hid_t *dtset_pt);

escdf_errno_t utils_hdf5_read_attr(hid_t loc_id, const char *name,
                                   hid_t mem_type_id, hsize_t *dims,
                                   unsigned int ndims, void *buf);

escdf_errno_t utils_hdf5_read_bool(hid_t loc_id, const char *name,
                                   _bool_set_t *scalar);

escdf_errno_t utils_hdf5_read_uint(hid_t loc_id, const char *name,
                                   _uint_set_t *scalar, unsigned int range[2]);

escdf_errno_t utils_hdf5_read_uint_array(hid_t loc_id, const char *name,
                                         unsigned int **array, hsize_t *dims,
                                         unsigned int ndims, unsigned int
                                         range[2]);

escdf_errno_t utils_hdf5_read_dbl_array(hid_t loc_id, const char *name,
                                        double **array, hsize_t *dims,
                                        unsigned int ndims, double range[2]);

escdf_errno_t utils_hdf5_create_dataset(hid_t loc_id, const char *name,
                                        hid_t type_id, hsize_t *dims, unsigned
                                        int ndims, hid_t *dtset_pt);

escdf_errno_t utils_hdf5_write_attr(hid_t loc_id, const char *name,
                                    hid_t disk_type_id, hsize_t *dims,
                                    unsigned int ndims, hid_t mem_type_id,
                                    const void *buf);

#endif
