/* Copyright (C) 2016-2017 Fabiano Corsetti <fabiano.corsetti@gmail.com>
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

#ifndef ESCDF_ESCDF_GROUP_H
#define ESCDF_ESCDF_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "escdf_handle.h"
#include "escdf_attributes.h"
#include "escdf_datasets.h"
#include "escdf_groups_ID.h"


/******************************************************************************
 * Data structures                                                            *
 ******************************************************************************/

/**
 * @brief Group specifications:
 * 
 * This struct contains the public information about the group.
 * 
 */
struct escdf_group_specs {
    escdf_group_id_t group_id;                    /**< ESCDF group ID */
    const char * root;                             /**< Root location of the group */

    unsigned int nattributes;                      /**< Number of attributes in the group */
    const escdf_attribute_specs_t **attr_specs;    /**< List of attribute specifications */

    unsigned int ndatasets;                        /**< Number of datasets in the group */
    const escdf_dataset_specs_t **data_specs;      /**< List of dataset specifications */
};

typedef struct escdf_group_specs escdf_group_specs_t;

typedef struct escdf_group escdf_group_t;


/**
 * @brief register the group specifications
 * 
 * @param[in] specs 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_group_specs_register(const escdf_group_specs_t *specs);


/* void escdf_register_all_group_specs(); */



/******************************************************************************
 * Low-level creators and destructors                                         *
 ******************************************************************************/

/**
 * @brief Create new group (low level routine)
 * 
 * This function takes care of creating an instance of escdf_group_t by
 * allocating the corresponding memory. It also initializes all its contents to
 * the default values.
 *
 * @param[in] group_id
 * @return instance of the group.
 */
escdf_group_t * escdf_group_new(escdf_group_id_t group_id);

/**
 * @brief Free all memory associated with the group (low level routine)
 *
 * @param[in,out] group: the group (low level routine)
 */
void escdf_group_free(escdf_group_t *group);

/**
 * @brief Open group location (low level routine)
 * 
 * Opens a location where to access the group data. If a name is given, the
 * location path will be 'group_root/name', where 'group_root' is defined in
 * the group specifications. If name is NULL, the location path is simply
 * 'group_root'. If the location does not exist, an error is returned.
 *
 * @param[in,out] group: the group.
 * @param[in] handle: the file handle.
 * @param[in] name: path to the location to open.
 * @return Error code.
 */
escdf_errno_t escdf_group_open_location(escdf_group_t *group, const escdf_handle_t *handle, const char *name);

/**
 * @brief Create new location (low level routine)
 * 
 * Creates a new location to store the group data. If a name is given, the
 * location path will be 'group_root/name', where 'group_root' is defined in
 * the group specifications. If name is NULL, the location path is simply
 * 'group_root'. If the location already exist, all previous contents of are
 * deleted.
 *
 * @param[in,out] group: the group.
 * @param[in] handle: the file handle.
 * @param[in] name: path to the location to create.
 * @return Error code.
 */
escdf_errno_t escdf_group_create_location(escdf_group_t *group, const escdf_handle_t *handle, const char *name);

/**
 * @brief Closes the location associated with the group.
 *
 * @param[in,out] group: the group.
 * @return Error code.
 */
escdf_errno_t escdf_group_close_location(escdf_group_t *group);


/******************************************************************************
 * High-level creators and destructors                                        *
 ******************************************************************************/

/**
 * @brief Open a group (high level routine)
 * 
 * This function takes care of the following tasks:
 * - call escdf_group_new to create an instance of the structure.
 * - call escdf_group_open_location. Note that this function will return an
 *   error if the location does not exist.
 * - call escdf_group_read_metadata to read all the metadata from the file and store it in memory.
 *
 * @param[in] handle: the file/group handle defining the root where to open
 * the "/group" group.
 * @param[in] group_name: Group name, as defined in the specifications.
 * @param[in] instance_name: If NULL, assume that the group is stored in the
 * "/group" group, otherwise "/group/name" is used.
 * @return instance of the group data type.
 */
escdf_group_t * escdf_group_open(const escdf_handle_t *handle, const char *group_name, const char *instance_name);

/**
 * @brief Create a group (high level routine)
 * 
 * This function performs the following tasks:
 * - call escdf_group_new to create an instance of the structure.
 * - call escdf_group_create_group.
 *
 * @param[in] handle: the file/group handle defining the root where to open
 * the "/group" group.
 * @param[in] group_name: Group name, as defined in the specifications.
 * @param[in] instance_name: If NULL, assume that the group is stored in the
 * "/group" group, otherwise "/group/name" is used.
 * @return instance of the group data type.
 *
 */
escdf_group_t * escdf_group_create(const escdf_handle_t *handle, const char *group_name, const char *instance_name);

/**
 * @brief Close a group
 * 
 * This function performs the following tasks:
 * - call escdf_group_close_group to close the group.
 * - call escdf_group_free to free all memory.
 *
 * @param[in,out] group: the group.
 * @return error code.
 */
escdf_errno_t escdf_group_close(escdf_group_t *group);


/******************************************************************************
 * Group attributes                                                           *
 ******************************************************************************/

/**
 * @brief Read group attributes
 * 
 * This routine reads all the metadata stored in the group, and stores
 * the information in the group data type.
 *
 * @param[out] group: pointer to instance of the group group.
 * @return error code.
 */
escdf_errno_t escdf_group_read_attributes(escdf_group_t *group);

/**
 * @brief Query group attributes
 * 
 * This routine queries whether a named attribute is present in the group.
 *
 * @param[in] group: pointer to the instance of the group group.
 * @param[in] name: name of the attribute to be queried
 * @return yes/no.
 */
bool escdf_group_query_attribute(const escdf_group_t *group, const char *name); /* unused */

/**
 * @brief get pointer to attribute_specs from name
 * 
 * @param group 
 * @param[in] name 
 * @return const escdf_attribute_specs_t* 
 */
const escdf_attribute_specs_t * escdf_group_get_attribute_specs(escdf_group_t *group, const char *name);


/**
 * @brief get pointer to dataset_specs from name
 * 
 * @param group 
 * @param[in] name 
 * @return const escdf_dataset_specs_t* 
 */
const escdf_dataset_specs_t * escdf_group_get_dataset_specs(escdf_group_t *group, const char *name);



/************************************************************
 * High level routines for accessing attributes in a group  *
 ************************************************************/


/**
 * @brief This routine sets the value of an attribute by name.
 * 
 * This routine stores the value in the data structure, and writes it to disk.
 *
 * If the attribute does not exist, it the routine will create it.
 *
 * @param[in] group: pointer to the group in which to look for the attribute
 * @param[in] attribute_name: attribute name
 * @param[in] buf: data to be written
 * @return error code
 */
escdf_errno_t escdf_group_attribute_set(escdf_group_t* group, const char* attribute_name, const void* buf);

/**
 * @brief This routine gets the value of an attribute by name.
 *
 * @param[in] escdf_group_t* group: pointer to the group in which to look for the attribute
 * @param[in] const char* attribute_nam: attribute name
 * @param[in] const void* buf: data to be written
 * @return error code
 */
escdf_errno_t escdf_group_attribute_get(escdf_group_t* group, const char* attribute_name, void* buf);

/************************************************************
 * Low level routines for accessing datasets in a group     *
 ************************************************************/



escdf_errno_t escdf_group_query_datasets(const escdf_group_t *group);


/************************************************************
 * High level routines for accessing datasets in a group    *
 ************************************************************/


/**
 * @brief Create new dataset in a group
 * 
 * @param group 
 * @param[in] name 
 * @return escdf_dataset_t* 
 */
escdf_dataset_t *escdf_group_dataset_create(escdf_group_t *group, const char *name);

/**
 * @brief Open dataset in a group
 * 
 * @param group 
 * @param[in] name 
 * @return escdf_dataset_t* 
 */
escdf_dataset_t *escdf_group_dataset_open(escdf_group_t *group, const char *name);
 
/**
 * @brief Close dataset in a group
 * 
 * @param group 
 * @param[in] name 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_group_dataset_close(escdf_group_t *group, const char *name);



/**
 * @brief Write a section of a dataset
 * 
 * @param data: pointer to dataset being written to 
 * @param[in] start: 
 * @param[in] count:
 * @param[in] stride: 
 * @param[in] buf: 
 * @return escdf_errno_t: return 0 on success. 
 */
escdf_errno_t escdf_group_dataset_write_at(const escdf_dataset_t *data, 
                                            const size_t *start, const size_t *count, const size_t * stride, void* buf);


/**
 * @brief Read a section of a dataset
 * 
 * @param[in] data: pointer to dataset being written to 
 * @param[in] start: 
 * @param[in] count:
 * @param[in] stride: 
 * @param[out] buf: 
 * @return escdf_errno_t: return 0 on success. 
 */
escdf_errno_t escdf_group_dataset_read_at(const escdf_dataset_t *data, 
                                            const size_t *start, const size_t *count, const size_t *stride, void *buf);




/************************************************************
 * Helper routines for groups                               *
 ************************************************************/

void escdf_group_print_info(const escdf_group_t* group);


/**
 * This routine returns the pointer to a group attribute.
 *
 * @param[in] group: pointer to the group.
 * @param[in] attribute_name: name of the attribute.
 * @return pointer to the attribute
 */

/*
escdf_attribute_t *escdf_group_get_arribute_from_name(escdf_group_t *group, const char *name); 

escdf_dataset_t * escdf_group_get_dataset_from_name(escdf_group_t *group, const char *name);

escdf_dataset_t * escdf_group_get_dataset_form_id(escdf_group_t *group, hid_t dtset_id);
*/


#ifdef __cplusplus
}
#endif

#endif
