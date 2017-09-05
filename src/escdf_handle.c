/* Copyright (C) 2016-2017 Damien Caliste <dcaliste@free.fr>
 *                         Micael Oliveira <micael.oliveira@mpsd.mpg.de>
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

#include <stdlib.h>

#include "escdf_error.h"
#include "escdf_handle.h"
#include "utils_hdf5.h"


/******************************************************************************
 * Global functions                                                           *
 ******************************************************************************/
static escdf_errno_t _create_root(escdf_handle_t *handle, const char *path)
{
    if (path != NULL) {
        utils_hdf5_create_group(handle->file_id, path, &(handle->group_id));
    } else {
        handle->group_id = H5Gopen(handle->file_id, "/", H5P_DEFAULT);
    }
    FULFILL_OR_RETURN(handle->group_id >= 0, ESCDF_EOBJECT);

    return ESCDF_SUCCESS;
}

escdf_handle_t * escdf_create(const char *filename, const char *path)
{
    escdf_handle_t *handle = (escdf_handle_t *) malloc(sizeof(escdf_handle_t));
    FULFILL_OR_RETURN_VAL(handle != NULL, ESCDF_ENOMEM, NULL);

    handle->mpi_rank = 0;
    handle->mpi_size = 1;
    handle->transfer_mode = H5P_DEFAULT;
    handle->file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    FULFILL_OR_RETURN_VAL(handle->file_id >= 0, ESCDF_EFILE_CORRUPT, NULL)

    if (path != NULL) {
        utils_hdf5_create_group(handle->file_id, path, &(handle->group_id));
    } else {
        handle->group_id = H5Gopen(handle->file_id, "/", H5P_DEFAULT);
    }
    FULFILL_OR_RETURN_VAL(handle->group_id >= 0, ESCDF_EOBJECT, NULL);

    return handle;
}

escdf_handle_t * escdf_open(const char *filename, const char *path) {
    escdf_handle_t *handle = (escdf_handle_t *) malloc(sizeof(escdf_handle_t));
    FULFILL_OR_RETURN_VAL(handle != NULL, ESCDF_ENOMEM, NULL)

    handle->mpi_rank = 0;
    handle->mpi_size = 1;
    handle->transfer_mode = H5P_DEFAULT;
    handle->file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
    FULFILL_OR_RETURN_VAL(handle->file_id >= 0, ESCDF_EFILE_CORRUPT, NULL)

    if (path != NULL) {
        FULFILL_OR_RETURN_VAL(utils_hdf5_check_present_recursive(handle->file_id, path), ESCDF_EFILE_CORRUPT, NULL)
        handle->group_id = H5Gopen(handle->file_id, path, H5P_DEFAULT);
    } else {
        handle->group_id = H5Gopen(handle->file_id, "/", H5P_DEFAULT);
    }

    FULFILL_OR_RETURN_VAL(handle->group_id >= 0, ESCDF_EFILE_CORRUPT, NULL)

    return handle;
}

#ifdef HAVE_MPI
escdf_handle_t * escdf_create_mpi(const char *filename, const char *path,
    MPI_Comm comm)
{
    hid_t fapl_id;
    herr_t err;
    escdf_handle_t *handle = (escdf_handle_t *) malloc(sizeof(escdf_handle_t));
    FULFILL_OR_RETURN_VAL(handle != NULL, ESCDF_ENOMEM, NULL);

    handle->comm = comm;
    MPI_Comm_size(handle->comm, &(handle->mpi_size));
    MPI_Comm_rank(handle->comm, &(handle->mpi_rank));

    handle->transfer_mode = H5Pcreate(H5P_DATASET_XFER);
    H5Pset_dxpl_mpio(handle->transfer_mode, H5FD_MPIO_COLLECTIVE);

    if ((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0) {
        H5Pclose(handle->transfer_mode);
        free(handle);
        DEFER_FUNC_ERROR(fapl_id);
        return NULL;
    }
    if ((err = H5Pset_fapl_mpio(fapl_id, comm, MPI_INFO_NULL)) < 0) {
        H5Pclose(handle->transfer_mode);
        H5Pclose(fapl_id);
        free(handle);
        DEFER_FUNC_ERROR(fapl_id);
        return NULL;
    }

    if ((handle->file_id = H5Fcreate(filename, H5F_ACC_TRUNC,
                                     H5P_DEFAULT, fapl_id)) < 0) {
        H5Pclose(handle->transfer_mode);
        H5Pclose(fapl_id);
        free(handle);
        DEFER_FUNC_ERROR(handle->file_id);
    };
    H5Pclose(fapl_id);

    if (_create_root(handle, path) != ESCDF_SUCCESS) {
        escdf_close(handle);
        return NULL;
    } else {
        return handle;
    }
}

escdf_handle_t * escdf_open_mpi(const char *filename, const char *path,
    MPI_Comm comm)
{
    hid_t fapl_id;
    herr_t err;
    escdf_handle_t *handle = (escdf_handle_t *) malloc(sizeof(escdf_handle_t));
    FULFILL_OR_RETURN_VAL(handle != NULL, ESCDF_ENOMEM, NULL);

    handle->comm = comm;
    MPI_Comm_size(handle->comm, &(handle->mpi_size));
    MPI_Comm_rank(handle->comm, &(handle->mpi_rank));

    handle->transfer_mode = H5Pcreate(H5P_DATASET_XFER);
    H5Pset_dxpl_mpio(handle->transfer_mode, H5FD_MPIO_COLLECTIVE);

    if ((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0) {
        H5Pclose(handle->transfer_mode);
        free(handle);
        DEFER_FUNC_ERROR(fapl_id);
        return NULL;
    }
    if ((err = H5Pset_fapl_mpio(fapl_id, comm, MPI_INFO_NULL)) < 0) {
        H5Pclose(handle->transfer_mode);
        H5Pclose(fapl_id);
        free(handle);
        DEFER_FUNC_ERROR(fapl_id);
        return NULL;
    }

    if ((handle->file_id = H5Fopen(filename, H5F_ACC_RDONLY, fapl_id)) < 0) {
        H5Pclose(handle->transfer_mode);
        H5Pclose(fapl_id);
        free(handle);
        DEFER_FUNC_ERROR(handle->file_id);
    };
    H5Pclose(fapl_id);

    if (_create_root(handle, path) != ESCDF_SUCCESS) {
        escdf_close(handle);
        return NULL;
    } else {
        return handle;
    }
}
#endif

escdf_errno_t escdf_close(escdf_handle_t *handle) {
    herr_t err;

    err = 0;
    if (handle->transfer_mode != H5P_DEFAULT) {
        DEFER_TEST_ERROR((err = H5Pclose(handle->transfer_mode)) < 0, err);
    }
    DEFER_TEST_ERROR((err = H5Gclose(handle->group_id)) < 0, err);
    DEFER_TEST_ERROR((err = H5Fclose(handle->file_id)) < 0, err);
    free(handle);
    return (err < 0) ? ESCDF_EIO : ESCDF_SUCCESS;
}
