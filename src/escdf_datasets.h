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

typedef struct escdf_dataset_specs escdf_dataset_specs_t;


struct escdf_dataset_specs {
    int id;
    char * name;
    int datatype;

    unsigned int stringlength;
    unsigned int ndims;

    bool disordered_storage_allowed;

    const escdf_attribute_specs_t **dims_specs;

    /** 
     * the reordering table specs are not necessary in the dataset_specs.
     * 
     *  const escdf_dataset_specs_t *reordering_table_specs;
     */

    
};


bool escdf_dataset_specs_is_present(const escdf_dataset_specs_t *specs, hid_t loc_id);

size_t escdf_dataset_specs_sizeof(const escdf_dataset_specs_t *specs);


/**
 *  The next two functions will be superceeded by 
 *  utils_hdf5_mem_type and utils_hdf5_disk_type. 
 **/

hid_t escdf_dataset_specs_hdf5_mem_type(const escdf_dataset_specs_t *specs);

hid_t escdf_dataset_specs_hdf5_disk_type(const escdf_dataset_specs_t *specs);





typedef struct escdf_dataset escdf_dataset_t;

/**
 * Create a new dataset:
 * 
 * This routine only creates the structure in memory and sets the dimensions. It does not create the dataset in the file
 */

escdf_dataset_t * escdf_dataset_new(const escdf_dataset_specs_t *specs, escdf_attribute_t **attr_dims);


/**
 * Access dimensions of a dataset
 */

unsigned int escdf_dataset_get_number_of_dimensions(const escdf_dataset_t *data);

hsize_t * escdf_dataset_get_dimensions(const escdf_dataset_t *data);


bool escdf_dataset_is_disordered_storage_allowed(const escdf_dataset_t *data);
bool escdf_dataset_is_ordered(const escdf_dataset_t *data);

escdf_errno_t escdf_dataset_set_ordered(escdf_dataset_t *data, bool ordered);

/**
 * Get pointer to the dataset holding the reordering table.
 * 
 * If the table is not set, this will return a NULL pointer.
 */

int * escdf_dataset_get_reordering_table(const escdf_dataset_t *data);

/**
 * Set pointer to the dataset holding the reordering table.
 * 
 * This will also set is_ordered to false.
 */

escdf_errno_t escdf_dataset_set_reordering_table(escdf_dataset_t *data, int *table);

hid_t escdf_dataset_get_id(escdf_dataset_t *data);
hid_t escdf_dataset_get_dtset_id(escdf_dataset_t *data);

const char * escdf_dataset_get_name(const escdf_dataset_t *data);

void escdf_dataset_free(escdf_dataset_t *attr);


/**
 * Create a dataset in the file:
 * 
 * This routine creates the dataset in the file and also writes the reordering table if necessary.
 */

escdf_errno_t escdf_dataset_create(escdf_dataset_t *data, hid_t loc_id);

escdf_errno_t escdf_dataset_open(escdf_dataset_t *data, hid_t loc_id);

escdf_errno_t escdf_dataset_close(escdf_dataset_t *data);




escdf_errno_t escdf_dataset_read_simple(const escdf_dataset_t *data, void *buf);
escdf_errno_t escdf_dataset_write_simple(escdf_dataset_t *data, void *buf);


#ifdef __cplusplus
}
#endif

#endif
