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
#include "escdf_datasets_ID.h"

typedef struct escdf_dataset_specs escdf_dataset_specs_t;

/**
 * @brief definition of the escdf dataset specifications
 */
struct escdf_dataset_specs {
    escdf_dataset_id_t id;
    char * name;
    int datatype;

    unsigned int stringlength;

    /**
     * @brief Number of dimensions
     * 
     * ndims refers to the logical number of dimensions, and defines the necessary number
     * of dims_specs attributes (see below).
     * 
     * Note that the number of dimensions of the physical dataset in the file might differ,
     * e.g. in case of compact storage or for sparse datasets.
     */
    unsigned int ndims;

    /**
     * @brief Flag whether compact storage is used:
     * 
     * e.g. species_at_site, we will store it as an effective one-dimensional array.
     *
     * This is currently allowed only for irregular 2D arrays.
     * 
     */
    bool compact;


    const escdf_attribute_specs_t **dims_specs;

    /** 
     * the reordering table specs are not necessary in the dataset_specs.
     */ 
    const escdf_dataset_specs_t *ordering_specs;
};

size_t escdf_dataset_specs_sizeof(const escdf_dataset_specs_t *specs);


/**
 * @brief Check whether specifications are present
 * 
 * @param[in] specs 
 * @param[in] loc_id 
 * @return true 
 * @return false 
 */
bool escdf_dataset_specs_is_present(const escdf_dataset_specs_t *specs, hid_t loc_id);

/**
 * @brief return whether compact storage is used.
 * 
 * @param[in] specs
 * @return true 
 * @return false 
 */
bool escdf_dataset_specs_is_compact(const escdf_dataset_specs_t *specs);




typedef struct escdf_dataset escdf_dataset_t;

/**
 * @brief Create a new dataset, based on specification specs and dimensions defind in the attributes attr_dims
 * 
 * @param specs [in]: Specifications for the dataset 
 * @param attr_dims [in]: Attributes defining the dimensions
 * @return escdf_dataset_t* : Pointer to newly created dataset
 */
escdf_dataset_t * escdf_dataset_new(const escdf_dataset_specs_t *specs, escdf_attribute_t **attr_dims);

/**
 * @brief Free the memory of the dataset
 * 
 * @param data 
 */
void escdf_dataset_free(escdf_dataset_t *data);

    
/**
 * @brief get dimensions (array)
 * 
 * @param[in] data 
 * @return const hsize_t* 
 */
const size_t * escdf_dataset_get_dimensions(const escdf_dataset_t *data);

/**
 * @brief Get pointer to the dataset holding the reordering table.
 * 
 * @param[in] data
 * 
 * If the table is not set, this will return a NULL pointer.
 */
int * escdf_dataset_get_reordering_table(const escdf_dataset_t *data);

/**
 * @brief Set pointer to the dataset holding the reordering table.
 * 
 * @param[inout] data
 * @param[in] table: pointer to the reordering table
 * This will also set is_ordered to false.
 */
escdf_errno_t escdf_dataset_set_reordering_table(escdf_dataset_t *data, int *table);

/**
 * @brief get the dataset ID 
 * 
 * @param[in] data 
 * @return hid_t 
 */
hid_t escdf_dataset_get_id(const escdf_dataset_t *data);

/**
 * @brief get the HDF5 dtset_id connected to the dataset
 * 
 * @param[in] data 
 * @return hid_t 
 */
hid_t escdf_dataset_get_dtset_id(const escdf_dataset_t *data);

/**
 * brief get HF5 type ID connected to that dataset
 * 
 * @param[in] data
 * @return hid_t
 */
hid_t escdf_dataset_get_type_id(const escdf_dataset_t *data);

/**
 * @brief get the dataset name
 * 
 * @param[in] data 
 * @return const char* 
 */
const char * escdf_dataset_get_name(const escdf_dataset_t *data);

/**
 * Create a dataset in the file:
 * 
 * This routine creates the dataset in the file and also writes the reordering table if necessary.
 */

/**
 * @brief create a dataset
 * 
 * @param data 
 * @param loc_id 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_dataset_create(escdf_dataset_t *data, hid_t loc_id);

/**
 * @brief ensure a dataset exists
 *
 * Creates the dataset if it does not yet exists.
 * 
 * @param data 
 * @param loc_id 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_dataset_ensure(escdf_dataset_t *data, hid_t loc_id);

/**
 * @brief open a dataset
 * 
 * @param data 
 * @param loc_id 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_dataset_open(escdf_dataset_t *data, hid_t loc_id);

/**
 * @brief close a dataset
 * 
 * @param data 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_dataset_close(escdf_dataset_t *data);

/**
 * @brief get the ordering dataset associated, if any
 * 
 * @param data 
 * @return escdf_dataset_t 
 */
escdf_dataset_t * escdf_dataset_get_ordering(const escdf_dataset_t * data);

escdf_errno_t escdf_dataset_read_simple(const escdf_dataset_t *data, void *buf);

escdf_errno_t escdf_dataset_write_simple(escdf_dataset_t *data, const void *buf);

/**
 * @brief read from dataset *data
 * 
 * @param data 
 * @param start 
 * @param count 
 * @param stride 
 * @param buf 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_dataset_read(const escdf_dataset_t *data, const size_t *start, const size_t *count, const size_t *stride, void *buf);

/**
 * @brief write to dataset *data
 * 
 * @param data 
 * @param start 
 * @param count 
 * @param stride 
 * @param buf 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_dataset_write(const escdf_dataset_t *data, const size_t *start, const size_t *count, const size_t *stride, const void *buf);

/**
 * @brief dump basic data to screen 
 * 
 * @param[in] data 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_dataset_print(const escdf_dataset_t *data);


#ifdef __cplusplus
}
#endif

#endif
