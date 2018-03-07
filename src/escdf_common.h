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

#ifndef LIBESCDF_ESCDF_COMMON_H
#define LIBESCDF_ESCDF_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* IMPORTANT: this file must contain defines only, in order to be readable from Fortran. */
#include <stdbool.h>

/* Strings */
#define ESCDF_STRLEN_ERROR 1024
#define ESCDF_STRLEN_GROUP 256


/**
 * Data types
 */
#define ESCDF_DT_NONE 0
#define ESCDF_DT_BOOL 1
#define ESCDF_DT_UINT 2
#define ESCDF_DT_INT 3
#define ESCDF_DT_DOUBLE 4
#define ESCDF_DT_STRING 5

/**
 * undefined ID
 */

#define ESCDF_UNDEFINED_ID -999

/**
 * Error codes
 *
 * Note: except for SUCCESS and ERROR, please keep alphabetical order.
 */
#define ESCDF_SUCCESS  0
#define ESCDF_ERROR   -1
#define ESCDF_EFILE_CORRUPT 1
#define ESCDF_EFILE_FORMAT 2
#define ESCDF_EIO 3
#define ESCDF_ENOFILE 4
#define ESCDF_ENOMEM 5
#define ESCDF_ENOSUPPORT 6
#define ESCDF_ETYPE 7
#define ESCDF_EVALUE 8

#define ESCDF_EOBJECT 9
#define ESCDF_ERANGE 10
#define ESCDF_ESIZE 11
#define ESCDF_ESIZE_MISSING 12
#define ESCDF_EUNINIT 13
#define ESCDF_ESTART 14
#define ESCDF_ECOUNT 15
#define ESCDF_ESTRIDE 16
#define ESCDF_ERROR_ARGS -10000
#define ESCDF_ERROR_DIM -1000

#ifdef __cplusplus
}
#endif

#endif
