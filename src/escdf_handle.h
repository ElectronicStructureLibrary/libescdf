/* Copyright (C) 2016-2017 Damien Caliste <dcaliste@free.fr>
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

#ifndef LIBESCDF_HANDLE_H
#define LIBESCDF_HANDLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hdf5.h>

#include "escdf_error.h"

#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_MPI_H
#include <mpi.h>
#endif



/******************************************************************************
 * Data structures                                                            *
 ******************************************************************************/

/**
 * This handle is an abstract reference to an ESCDF file and all access to a file
 * through Libescd is done through it.
 */
typedef struct {
    hid_t file_id;  /**< HDF5 file identifier */

    hid_t group_id; /**< HDF5 group identifier that is to be considered as root */

    int mpi_size, mpi_rank;
    hid_t transfer_mode;

#ifdef HAVE_MPI
    MPI_Comm comm;
#endif
} escdf_handle_t;


/******************************************************************************
 * Global functions                                                           *
 ******************************************************************************/

/**
 * Create a file an returns a handle to it. Optionally, the root group is set to
 * 'path' if path is not NULL.
 *
 * @param[in] filename: the name of the file to be created.
 * @param[in] path: path for the root group inside the file.
 * @return instance of the handle.
 */
escdf_handle_t * escdf_create(const char *filename, const char *path);

/**
 * Opens a file an returns a handle to it. Optionally, consider the root group
 * to be given by 'path' if path is not NULL.
 *
 * @param[in] filename: the name of the file to be created.
 * @param[in] path: path for the root group inside the file.
 * @return instance of the handle.
 */
escdf_handle_t * escdf_open(const char *filename, const char *path);

/**
 * Close the file and free the memory.
 *
 * @return error code.
 */
escdf_errno_t escdf_close(escdf_handle_t *handle);


#ifdef HAVE_MPI
/**
 * Creates a file an returns a handle to it. ptionally, the root group is set to
 * 'path' if path is not NULL.
 *
 * @param[in] filename: the name of the file to be created.
 * @param[in] path: path for the root group inside the file.
 * @return instance of the handle.
 */
escdf_handle_t * escdf_create_mpi(const char *filename, const char *path, MPI_Comm comm);

/**
 * Opens a file an returns a handle to it. Optionally, consider the root group
 * to be given by 'path' if path is not NULL.
 *
 * @param[in] filename: the name of the file to be created.
 * @param[in] path: path for the root group inside the file.
 * @return instance of the handle.
 */
escdf_handle_t * escdf_open_mpi(const char *filename, const char *path, MPI_Comm comm);
#endif

#ifdef __cplusplus
}
#endif

#endif
