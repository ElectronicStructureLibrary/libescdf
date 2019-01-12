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

#ifndef ESCDF_PRIVATE_GROUP_H
#define ESCDF_PRIVATE_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "escdf_handle.h"
#include "escdf_attributes.h"
#include "escdf_datasets.h"
#include "escdf_group.h"
#include "escdf_groups_ID.h"
// #include "escdf_groups_specs.h"


/**
 * @brief escdf_group data structure
 * 
 * This struct contains the private data of a group
 * 
 */
struct escdf_group {
    const escdf_handle_t *escdf_handle;

    const escdf_group_specs_t *specs;  /**< Pointer to the group specification */

    char * name;                       /**< Group name */

    hid_t loc_id;                      /**< Handle for HDF5 group */

    escdf_attribute_t **attr;          /**< List of attributes */
    escdf_dataset_t   **datasets;      /**< List of datasets */

    bool *datasets_present;            /**< Flag whether datasets are present */
};

escdf_errno_t escdf_group_attribute_new(escdf_group_t *, escdf_attribute_id_t ); 
escdf_errno_t _escdf_group_dataset_new(escdf_group_t *, escdf_dataset_id_t );

#ifdef __cplusplus
}
#endif

#endif