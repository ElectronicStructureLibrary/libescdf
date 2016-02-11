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

#ifndef LIBESCDF_HANDLE_H
#define LIBESCDF_HANDLE_H

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
*
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

escdf_handle_t * escdf_create(const char *filename, const char *path);

escdf_handle_t * escdf_open(const char *filename, const char *path);

escdf_errno_t escdf_close(escdf_handle_t *handle);

#ifdef HAVE_MPI
escdf_handle_t * escdf_create_mpi(const char *filename, const char *path,
    MPI_Comm comm);

escdf_handle_t * escdf_open_mpi(const char *filename, const char *path,
    MPI_Comm comm);
#endif

#endif
