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


typedef struct escdf_attribute_specs escdf_attribute_specs_t;


struct escdf_attribute_specs {
    int id;
    char * name;
    int datatype;

    unsigned int stringlength;
    unsigned int ndims;

    const escdf_attribute_specs_t **dims_specs;
};


/**
 * @brief get attribute size according to the specifications
 * 
 * @param[in]  escdf_attribute_specs_t *
 * @return const char* 
 */
size_t escdf_attribute_specs_sizeof(const escdf_attribute_specs_t *specs);

/**
 * @brief check whether the attribute specifications are present in a given location
 * 
 * @param[in]  escdf_attribute_specs_t *, hid_t
 * @return bool 
 */
bool escdf_attribute_specs_is_present(const escdf_attribute_specs_t *specs, hid_t loc_id);

/* the next two functions should become obsolete */

hid_t escdf_attribute_specs_hdf5_mem_type(const escdf_attribute_specs_t *specs);

hid_t escdf_attribute_specs_hdf5_disk_type(const escdf_attribute_specs_t *specs);





typedef struct escdf_attribute escdf_attribute_t;

/**
 * @brief create a new attribute according to the specifications with given dimensions
 * 
 * @param[in]  escdf_attribute_specs_t *specs :  specifications
 * @param[in]  escdf_attribute_t **attr_dims :   given dimensions
 * @return escdf_attribute_t * : pointer to new attribute
 */
escdf_attribute_t * escdf_attribute_new(const escdf_attribute_specs_t *specs, escdf_attribute_t **attr_dims);

/**
 * @brief release an attribute
 * 
 * @param[in]  escdf_attribute_t *attr :  pointer to attribute to be released
 */
void escdf_attribute_free(escdf_attribute_t *attr);


/**
 * @brief obtain the size (in memory) of the attribute attr
 * 
 * @param[in] const escdf_attribute_t *attr : pointer to the attribute
 * @return size_t : size in Bytes
 */
size_t escdf_attribute_sizeof(const escdf_attribute_t *attr);

/**
 * @brief obtain actual dimensions of a attribute
 * 
 * @param[in] const escdf_attribute_t *attr :  pointer to attribute
 * @return *unsigned int : array of dimensions (rank defined in specs)
 */
const unsigned int *escdf_attribute_get_dimensions(const escdf_attribute_t *attr);

/**
 * @brief set the attribute to a value given in the buffer
 * 
 * @param[in] escdf_attribute_t *attr : pointer to attribute
 * @param[in] const void *buf :  buffer
 * @return escdf_errno_t : error code
 */
escdf_errno_t escdf_attribute_set(escdf_attribute_t *attr, const void *buf);

/**
 * @brief get value from attribute
 * 
 * @param[in] escdf_attribute_t *attr : pointer to attribute
 * @param[in] void *buf :  buffer
 * @return escdf_errno_t : error code
 */
escdf_errno_t escdf_attribute_get(const escdf_attribute_t *attr, void *buf);

/**
 * @brief read attribute from disk and store in memory
 * 
 * @param[in] escdf_attribute_t *attr :  pointer to attribute
 * @param[in] hid_t loc_id :  location in file
 * @return escdf_errno_t : error code
 */
escdf_errno_t escdf_attribute_read(escdf_attribute_t *attr, hid_t loc_id);

/**
 * @brief write attribute from memory to disk
 * 
 * @param[in] escdf_attribute_t *attr :  pointer to attribute
 * @param[in] hid_t loc_id :  location in file
 * @return escdf_errno_t : error code
 */
escdf_errno_t escdf_attribute_write(escdf_attribute_t *attr, hid_t loc_id);

/**
 * @brief print contents of an attribute for debugging
 * 
 * @param[in] escdf_attribute_t *attr :  pointer to attribute
 * @return escdf_errno_t : error code
 */
escdf_errno_t escdf_attribute_print(escdf_attribute_t *attr);

/**
 * @brief check whether an attribute is set
 * 
 * @param[in] escdf_attribute_t *attr :  pointer to attribute
 * @return bool 
 */
bool escdf_attribute_is_set(const escdf_attribute_t *attr);

/**
 * @brief get the ID of the specifications of an attribute
 * 
 * @param[in] const escdf_attribute_t *attr :  pointer to attribute
 * @return int
 */
int escdf_attribute_get_specs_id(const escdf_attribute_t *attr);


#ifdef __cplusplus
}
#endif

#endif
