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

#ifndef LIBESCDF_ESCDF_COMMON_H
#define LIBESCDF_ESCDF_COMMON_H

/* IMPORTANT: this file must contain defines only, in order to be readable from Fortran. */


/**
 * Error codes
 *
 * Note: except for SUCCESS and ERROR, please keep alphabetical order.
 */
#define PSPIO_SUCCESS  0
#define PSPIO_ERROR   -1
#define PSPIO_EFILE_CORRUPT 1
#define PSPIO_EFILE_FORMAT 2
#define PSPIO_EIO 3
#define PSPIO_ENOFILE 4
#define PSPIO_ENOMEM 5
#define PSPIO_ENOSUPPORT 6
#define PSPIO_ETYPE 7
#define PSPIO_EVALUE 8

#endif
