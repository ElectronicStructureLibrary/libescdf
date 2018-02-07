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

#include <assert.h>

#include "escdf_group.h"
#include "utils_hdf5.h"

#define MAX_KNOWN_SPECS 100


static unsigned int n_known_specs = 0;
static escdf_group_specs_t const * known_group_specs[MAX_KNOWN_SPECS];


escdf_errno_t escdf_group_specs_register(const escdf_group_specs_t *specs) {
    FULFILL_OR_RETURN(specs != NULL, ESCDF_EVALUE)

    FULFILL_OR_RETURN(n_known_specs < MAX_KNOWN_SPECS, ESCDF_ESIZE)

    known_group_specs[n_known_specs] = specs;
    n_known_specs++;

    return ESCDF_SUCCESS;
}


struct escdf_group {
    const escdf_group_specs_t *specs;

    hid_t loc_id; /**< Handle for HDF5 group */

    escdf_attribute_t **attr;
};




const escdf_attribute_specs_t * _get_attribute_specs(escdf_group_specs_t *group_specs, char *name)
{
    unsigned int ii;

    for (ii=0; ii<group_specs->nattributes; ii++) {
        if (strcmp(group_specs->attr_specs[ii]->name, name) == 0)
            return group_specs->attr_specs[ii];
    }

    return NULL;
}




escdf_group_t * escdf_group_new(escdf_group_id group_id)
{
    escdf_group_t *group = NULL;
    unsigned int ii;

    group = (escdf_group_t *) malloc(sizeof(escdf_group_t));
    if (group == NULL)
        return group;

    group->specs = NULL;
    for (ii = 0; ii < n_known_specs; ii++) {
        if (known_group_specs[ii]->group_id == group_id) {
            group->specs = known_group_specs[ii];
            break;
        }
    }
    if (group->specs == NULL) {
        free(group);
        return NULL;
    }

    group->loc_id = -1;

    group->attr = (escdf_attribute_t **) malloc(group->specs->nattributes * sizeof(escdf_attribute_t *));
    if (group->attr == NULL) {
        free(group);
    } else {
        for (ii=0; ii<group->specs->nattributes; ii++)
            group->attr[ii] = NULL;
    }

    return group;
}

void escdf_group_free(escdf_group_t *group)
{
    unsigned int ii;

    if (group != NULL) {
        for (ii=0; ii<group->specs->nattributes; ii++)
            escdf_attribute_free(group->attr[ii]);
        free(group->attr);
    }
    free(group);
}

escdf_errno_t escdf_group_open_location(escdf_group_t *group, const escdf_handle_t *handle, const char *name)
{
    char location_path[ESCDF_STRLEN_GROUP];

    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE)
    FULFILL_OR_RETURN(handle != NULL, ESCDF_EVALUE)

    if (name == NULL)
        sprintf(location_path, "%s", group->specs->root);
    else
        sprintf(location_path, "%s/%s", group->specs->root, name);

    group->loc_id = H5Gopen(handle->group_id, location_path, H5P_DEFAULT);

    FULFILL_OR_RETURN(group->loc_id >= 0, group->loc_id);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_group_create_location(escdf_group_t *group, const escdf_handle_t *handle, const char *name)
{
    char location_path[ESCDF_STRLEN_GROUP];

    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE)
    FULFILL_OR_RETURN(handle != NULL, ESCDF_EVALUE)

    if (name == NULL)
        sprintf(location_path, "%s", group->specs->root);
    else
        sprintf(location_path, "%s/%s", group->specs->root, name);

    utils_hdf5_create_group(handle->file_id, location_path, &(group->loc_id));

    FULFILL_OR_RETURN(group->loc_id >= 0, group->loc_id);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_group_close_location(escdf_group_t *group)
{
    herr_t herr_status;

    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE)

    /* close the group */
    if (group->loc_id >= 0) {
        herr_status = H5Gclose(group->loc_id);
        FULFILL_OR_RETURN(herr_status >= 0, herr_status);
    }

    return ESCDF_SUCCESS;
}

escdf_group_t * escdf_group_open(escdf_group_id group_id, const escdf_handle_t *handle, const char *name)
{
    escdf_group_t *group;

    if ((group = escdf_group_new(group_id)) == NULL)
        return NULL;

    if (escdf_group_open_location(group, handle, name) != ESCDF_SUCCESS)
        goto cleanup;

    if (escdf_group_read_attributes(group) != ESCDF_SUCCESS) {
        escdf_group_close_location(group);
        goto cleanup;
    }

    return group;

    cleanup:
    escdf_group_free(group);
    return NULL;
}

escdf_group_t * escdf_group_create(escdf_group_id group_id, const escdf_handle_t *handle, const char *name)
{
    escdf_group_t *group;

    if ((group = escdf_group_new(group_id)) == NULL)
        return NULL;

    if (escdf_group_create_location(group, handle, name) != ESCDF_SUCCESS) {
        escdf_group_free(group);
        return NULL;
    }

    return group;
}

escdf_errno_t escdf_group_close(escdf_group_t *group)
{
    escdf_errno_t err;

    if (group != NULL) {
        if ((err = escdf_group_close_location(group)) != ESCDF_SUCCESS)
            return err;

        escdf_group_free(group);
    }

    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_group_read_attributes(escdf_group_t *group)
{
    unsigned int iattr, ndims, idim, ii;
    escdf_attribute_t *dims = NULL;


    for (iattr = 0; iattr < group->specs->nattributes; iattr++) {
        if (group->attr[iattr] != NULL) /* We have already read this attribute */
            continue;

        ndims = group->specs->attr_specs[iattr]->ndims;
        if (ndims > 0) {
            dims = (escdf_attribute_t *) malloc(sizeof(escdf_attribute_t *) * ndims);
            if (dims == NULL) {
                /* Deal with error */
            }

            for (idim = 0; idim < ndims; idim++) {
                for (ii = 0; ii < group->specs->nattributes; iattr++) {

                }
            }


            free(dims);
        }

        group->attr[iattr] = escdf_attribute_new(group->specs->attr_specs[iattr], &dims);



    }


    return ESCDF_SUCCESS;
}


