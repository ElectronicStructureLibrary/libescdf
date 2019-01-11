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

#ifndef ESCDF_ESCDF_HL_H
#define ESCDF_ESCDF_HL_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Query group attributes
 * 
 * This routine queries whether a named attribute is present in the group.
 *
 * @param[in] group: pointer to the instance of the group group.
 * @param[in] name: name of the attribute to be queried
 * @return yes/no.
 */
bool escdf_HL_query_attribute(const escdf_group_t *group, const char *name); /* unused */

/**
 * @brief get pointer to attribute_specs from name
 * 
 * @param group 
 * @param[in] name 
 * @return const escdf_attribute_specs_t* 
 *
const escdf_attribute_specs_t * escdf_group_get_attribute_specs(escdf_group_t *group, const char *name);
*/

/**
 * @brief get pointer to dataset_specs from name
 * 
 * @param group 
 * @param[in] name 
 * @return const escdf_dataset_specs_t* 
 *
const escdf_dataset_specs_t * escdf_group_get_dataset_specs(escdf_group_t *group, const char *name);
*/


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
escdf_errno_t escdf_HL_attribute_set(escdf_group_t* group, hid_t attribute_ID, const void* buf);

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
 * @brief write complete dataset
 * 
 * @param data 
 * @param buf 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_group_dataset_write_simple(escdf_dataset_t *data, void* buf);

/**
 * @brief read complete dataset
 * 
 * @param[in] data 
 * @param buf 
 * @return escdf_errno_t 
 */
escdf_errno_t escdf_group_dataset_read_simple(const escdf_dataset_t *data, void *buf);

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
                                            unsigned int *start, unsigned int *count, unsigned int * stride, void* buf);


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
                                            unsigned int *start, unsigned int *count, unsigned int * stride, void *buf);





#ifdef __cplusplus
}
#endif

#endif