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

#include "escdf_error.h"
#include "escdf_handle.h"


/******************************************************************************
 * Global functions                                                           *
 ***************************************************P**************************/

escdf_handle_t * escdf_create(const char *filename, const char *path) {
    escdf_handle_t *handle = (escdf_handle_t *) malloc(sizeof(escdf_handle_t));
    FULFILL_OR_RETURN_VAL(handle != NULL, ESCDF_ENOMEM, NULL)

    handle->file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    if (path != NULL) {
        if ((H5Lexists(handle->file_id, path, H5P_DEFAULT))) {
            handle->group_id = H5Gopen(handle->file_id, path, H5P_DEFAULT);
        } else {
            handle->group_id = H5Gcreate(handle->file_id, path, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        }
    } else {
        handle->group_id = H5Gopen(handle->file_id, "/", H5P_DEFAULT);
    }
    return handle;
}

escdf_errno_t escdf_close(escdf_handle_t *handle) {
    FULFILL_OR_RETURN(!H5Gclose(handle->group_id), ESCDF_EIO)
    FULFILL_OR_RETURN(!H5Fclose(handle->file_id), ESCDF_EIO)
    return ESCDF_SUCCESS;
}
