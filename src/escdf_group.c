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
#include <stdlib.h>
#include <check.h>

#include "escdf_group.h"
#include "utils_hdf5.h"

#define MAX_KNOWN_SPECS 100


static unsigned int n_known_specs = 0;
static escdf_group_specs_t const * known_group_specs[MAX_KNOWN_SPECS];

escdf_errno_t escdf_group_attribute_new(escdf_group_t *group, unsigned int iattr);

escdf_errno_t escdf_group_specs_register(const escdf_group_specs_t *specs) {
    FULFILL_OR_RETURN(specs != NULL, ESCDF_EVALUE)

    FULFILL_OR_RETURN(n_known_specs < MAX_KNOWN_SPECS, ESCDF_ESIZE)

    known_group_specs[n_known_specs] = specs;
    n_known_specs++;

    return ESCDF_SUCCESS;
}


struct escdf_group {
    const escdf_group_specs_t *specs;
    const escdf_dataset_specs_t *data_specs;

    hid_t loc_id; /**< Handle for HDF5 group */

    escdf_attribute_t **attr;
    escdf_dataset_t   **datasets;
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

    FULFILL_OR_RETURN(group->specs != NULL, ESCDF_EVALUE);

    for (iattr = 0; iattr < group->specs->nattributes; iattr++) {

        if (group->attr[iattr] == NULL) {
	        SUCCEED_OR_RETURN(escdf_group_attribute_new(group, iattr));
        }

        if(escdf_attribute_is_present(group->attr[iattr], group->loc_id)) {       
	
	        if (!escdf_attribute_is_set(group->attr[iattr])) {
	            SUCCEED_OR_RETURN(escdf_attribute_read(group->attr[iattr], group->loc_id));
	        }   
        }
        else {
	        /* We might want to throw an error here */
	        printf("WARNING: Attribute %i not found in file. \n", iattr);
        }
    }
    return ESCDF_SUCCESS;
}




escdf_errno_t escdf_group_attribute_set(escdf_group_t *group, const char* attribute_name, void* buf)
{
    unsigned int iattr, i;
    bool attr_found = false;
    escdf_errno_t error;
  
    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(group->specs != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(buf != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(group->specs->attr_specs[iattr] != NULL, ESCDF_EVALUE);

    for (attr_found = false, i = 0; i < group->specs->nattributes; i++) {
        if ( strcmp(group->specs->attr_specs[i]->name, attribute_name) == 0 ) {iattr = i; attr_found=true;}
    }
    FULFILL_OR_RETURN(attr_found == true, ESCDF_ERROR);

    if (group->attr[iattr] == NULL) {
        SUCCEED_OR_RETURN(escdf_group_attribute_new(group, iattr));
    }

    SUCCEED_OR_RETURN(escdf_attribute_set(group->attr[iattr], buf));
  
    /* If the attribute has successfully been set in memory, we can write to disk */

    SUCCEED_OR_RETURN(escdf_attribute_write(group->attr[iattr], group->loc_id));
  
    return ESCDF_SUCCESS;
}



escdf_errno_t escdf_group_attribute_get(escdf_group_t *group, const char* attribute_name, void *buf)
{
    unsigned int iattr, i;
    bool attr_found;
    escdf_errno_t error;

    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(group->specs != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(buf != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(group->specs->attr_specs[iattr] != NULL, ESCDF_EVALUE);

    for (attr_found = false, i = 0; i < group->specs->nattributes; i++) {
        if ( strcmp(group->specs->attr_specs[i]->name, attribute_name) == 0 ) {iattr = i; attr_found=true;}
    }
    FULFILL_OR_RETURN(attr_found == true, ESCDF_ERROR);

    if (group->attr[iattr] == NULL) {
        SUCCEED_OR_RETURN(escdf_group_attribute_new(group, iattr));
    }

    /* check whether the attribute already as a value in memory */

    if (!escdf_attribute_is_set(group->attr[iattr])) {
        SUCCEED_OR_RETURN(escdf_attribute_read(group->attr[iattr], group->loc_id));
    }

    SUCCEED_OR_RETURN(escdf_attribute_get(group->attr[iattr], buf));
  
    return ESCDF_SUCCESS;
  
}

escdf_errno_t escdf_group_attribute_new(escdf_group_t *group, unsigned int iattr)
{ 

    unsigned int i, ii, idim, ndims, dim_ID;
    bool attr_found;

    escdf_attribute_t **dims = NULL;
    escdf_errno_t error;

    /* need to create the attribute */ 

    /* determine the dimensions from linked dimension attributes */
    
    FULFILL_OR_EXIT(group->specs != NULL, ESCDF_EVALUE);

    ndims = group->specs->attr_specs[iattr]->ndims;

    if( ndims > 0 ) {
      
        dims = (escdf_attribute_t**) malloc(ndims * sizeof(escdf_attribute_t*));
      
        FULFILL_OR_RETURN(dims != NULL, ESCDF_ERROR);
      
        for(i = 0; i<ndims; i++) {
	
	        FULFILL_OR_RETURN_CLEAN( group->specs->attr_specs[iattr] != NULL, ESCDF_EVALUE, dims );
	        FULFILL_OR_RETURN_CLEAN( group->specs->attr_specs[iattr]->dims_specs[i] != NULL, ESCDF_EVALUE, dims );
	

	        idim = group->specs->attr_specs[iattr]->dims_specs[i]->id;
		  
	        for (attr_found=false, ii = 0; ii < group->specs->nattributes; ii++) {
	            if (group->specs->attr_specs[ii]->id == idim) {dim_ID = ii; attr_found=true;}
	        }
            FULFILL_OR_RETURN_CLEAN( attr_found == true, ESCDF_ERROR, dims );

	        if(group->attr[dim_ID] == NULL) {
	            if (dims != NULL) free(dims);
	            return ESCDF_ERROR_DIM;
	        }
	        else {
	            FULFILL_OR_RETURN_CLEAN( group->attr[dim_ID] !=  NULL, ESCDF_EVALUE, dims );
	            dims[i] = group->attr[dim_ID];
	        }
        }
      
    }
    else { 
        dims = NULL;
    }

    group->attr[iattr] = escdf_attribute_new(group->specs->attr_specs[iattr], dims);

    if(dims != NULL) free(dims);

    return ESCDF_SUCCESS;

}


escdf_errno_t escdf_group_dataset_new(escdf_group_t *group, unsigned int idata) {

    unsigned int i, ii, idim, ndims, dim_ID;
    bool found;

    escdf_attribute_t **dims = NULL;
    escdf_errno_t error;

    /* need to create the attribute */ 

    /* determine the dimensions from linked dimension attributes */
    
    FULFILL_OR_EXIT(group->datasets != NULL, ESCDF_EVALUE);

    ndims = group->specs->data_specs[idata]->ndims;

    if( ndims > 0 ) {
        
        dims = (escdf_attribute_t**) malloc(ndims * sizeof(escdf_attribute_t*));
      
        FULFILL_OR_RETURN(dims != NULL, ESCDF_ERROR);
      
        for(i = 0; i<ndims; i++) {
	
	        FULFILL_OR_RETURN_CLEAN( group->specs->data_specs[idata] != NULL, ESCDF_EVALUE, dims );
	        FULFILL_OR_RETURN_CLEAN( group->specs->data_specs[idata]->dims_specs[i] != NULL, ESCDF_EVALUE, dims );
	

	        idim = group->specs->data_specs[idata]->dims_specs[i]->id;
		  
	        for (found=false, ii = 0; ii < group->specs->nattributes; ii++) {
	            if (group->specs->attr_specs[ii]->id == idim) {dim_ID = ii; found=true;}
	        }
            FULFILL_OR_RETURN_CLEAN( found == true, ESCDF_ERROR, dims );

	        if(group->attr[dim_ID] == NULL) {
	            if (dims != NULL) free(dims);
	            return ESCDF_ERROR_DIM;
	        }
	        else {
	            FULFILL_OR_RETURN_CLEAN( group->attr[dim_ID] !=  NULL, ESCDF_EVALUE, dims );
	            dims[i] = group->attr[dim_ID];
	        }
        }
      
    }
    else { 
      dims = NULL;
    }

    group->datasets[idata] = escdf_dataset_new(group->specs->data_specs[idata], dims);

    if(dims != NULL) free(dims);

    return ESCDF_SUCCESS;

};
