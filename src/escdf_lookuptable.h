/* Copyright (C) 2016-2017 Damien Caliste <dcaliste@free.fr>
 *                         Micael Oliveira <micael.oliveira@mpsd.mpg.de>
 *                         Yann Pouillon <devops@materialsevolution.es>
 *                         Martin Lueders <martin.lueders@stfc.ac.uk>
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

#ifndef LIBESCDF_ESCDF_LOOKUPTABLE_H
#define LIBESCDF_ESCDF_LOOKUPTABLE_H

/* #include "escdf.h" */
#include <hdf5.h>

#include "escdf_error.h"



#ifdef __cplusplus
extern "C" {
#endif



typedef struct escdf_lookuptable escdf_lookuptable_t;

escdf_lookuptable_t *escdf_lookuptable_new();

escdf_errno_t escdf_lookuptable_init(escdf_lookuptable_t *this);
escdf_errno_t escdf_lookuptable_grow(escdf_lookuptable_t *this);
escdf_errno_t escdf_lookuptable_shrink(escdf_lookuptable_t *this);
escdf_errno_t escdf_lookuptable_delete(escdf_lookuptable_t *this);

escdf_errno_t escdf_lookuptable_add(escdf_lookuptable_t *this, hid_t ID, void* ptr);


void* escdf_lookuptable_get_pointer(escdf_lookuptable_t *this, hid_t ID);
hid_t escdf_lookuptable_get_id(escdf_lookuptable_t *this, void* ptr);

bool escdf_lookuptable_check_exist(escdf_lookuptable_t *this, hid_t ID);



#ifdef __cplusplus
}
#endif

#endif