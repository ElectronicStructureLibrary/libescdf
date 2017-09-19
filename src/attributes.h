/* Copyright (C) 2017 Micael Oliveira <micael.oliveira@mpsd.mpg.de>
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

#ifndef ESCDF_ESCDF_ATTRIBUTES_H
#define ESCDF_ESCDF_ATTRIBUTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>

#include "escdf_handle.h"


struct _attribute_specs {
    int id;
    char * name;
    int datatype;
    unsigned int ndims;

    const struct _attribute_specs **dims_specs;
};

typedef struct _attribute_specs _attribute_specs_t;

size_t _attribute_specs_sizeof(const _attribute_specs_t *specs);

hid_t _attribute_specs_hdf5_mem_type(const _attribute_specs_t *specs);

hid_t _attribute_specs_hdf5_disk_type(const _attribute_specs_t *specs);

bool _attribute_specs_is_present(const _attribute_specs_t *specs, hid_t loc_id);



typedef struct {
    const _attribute_specs_t *specs;
    bool is_set;
    hsize_t *dims;
    void *buf;
} _attribute_t;

_attribute_t * _attribute_new(const _attribute_specs_t *specs, _attribute_t **attr_dims);

void _attribute_free(_attribute_t *attr);

size_t _attribute_sizeof(const _attribute_t *attr);

escdf_errno_t _attribute_set(_attribute_t *attr, void *buf);

escdf_errno_t _attribute_get(_attribute_t *attr, void *buf);

escdf_errno_t _attribute_read(_attribute_t *attr, hid_t loc_id);

escdf_errno_t _attribute_write(_attribute_t *attr, hid_t loc_id);


#ifdef __cplusplus
}
#endif

#endif
