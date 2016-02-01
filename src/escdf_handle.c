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

#include <stdlib.h>

#include "escdf_handle.h"

/*****************************************************************************
 * Data structures                                                           *
 *****************************************************************************/

struct escdf_handle {
    hid_t file_id;  /**< HDF5 file identifier */
};


/*****************************************************************************
 * Global functions                                                          *
 *****************************************************************************/

escdf_handle_t * escdf_open(char *filename) {
    escdf_handle_t *handle = (escdf_handle_t *) malloc(sizeof(escdf_handle_t));
    if (handle != NULL)
        handle->file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    return handle;
}

void escdf_close(escdf_handle_t *handle) {
    int status = H5Fclose(handle->file_id);
}