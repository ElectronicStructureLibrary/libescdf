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

#ifndef ESCDF_ESCDF_DATASETS_H
#define ESCDF_ESCDF_DATASETS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>

#include "escdf_handle.h"
#include "escdf_attributes.h"

struct escdf_dataset_specs {
    int id;
    char * name;
    int datatype;

    unsigned int stringlength;
    unsigned int ndims;

    const struct escdf_attribute_specs **dims_specs;
};

typedef struct escdf_dataset_specs escdf_dataset_specs_t;

size_t escdf_dataset_specs_sizeof(const escdf_dataset_specs_t *specs);

hid_t escdf_dataset_specs_hdf5_mem_type(const escdf_dataset_specs_t *specs);

hid_t escdf_dataset_specs_hdf5_disk_type(const escdf_dataset_specs_t *specs);

bool escdf_dataset_specs_is_present(const escdf_dataset_specs_t *specs, hid_t loc_id);


typedef struct escdf_dataset escdf_dataset_t;

escdf_dataset_t * escdf_dataset_new(const escdf_dataset_specs_t *specs, escdf_attribute_t **attr_dims);

void escdf_dataset_free(escdf_dataset_t *attr);

/* 
   size_t escdf_attribute_sizeof(const escdf_attribute_t *attr);
   escdf_errno_t escdf_attribute_set(escdf_attribute_t *attr, void *buf);
   escdf_errno_t escdf_attribute_get(escdf_attribute_t *attr, void *buf);

   escdf_errno_t escdf_attribute_read(escdf_attribute_t *attr, hid_t loc_id);
   escdf_errno_t escdf_attribute_write(escdf_attribute_t *attr, hid_t loc_id);

   bool escdf_attribute_is_set(const escdf_attribute_t *attr);
*/

#ifdef __cplusplus
}
#endif

#endif
