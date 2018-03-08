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

escdf_errno_t escdf_group_attribute_new(escdf_group_t *group, unsigned int iattr); /* unsused */

escdf_errno_t escdf_group_specs_register(const escdf_group_specs_t *specs) {
    FULFILL_OR_RETURN(specs != NULL, ESCDF_EVALUE)

    FULFILL_OR_RETURN(n_known_specs < MAX_KNOWN_SPECS, ESCDF_ESIZE)

    known_group_specs[n_known_specs] = specs;
    n_known_specs++;

    printf("Registering group %d, %s\n", specs->group_id, specs->root);

    return ESCDF_SUCCESS;
}

escdf_group_id _escdf_get_group_id(const char* name)
{
    int i;
    escdf_group_id group_id;

    group_id = ESCDF_UNDEFINED_ID;

    for(i=0; i<n_known_specs; i++) {
        printf("searching for %s: %d %s %d \n", name, i, known_group_specs[i]->root, known_group_specs[i]->group_id );
        if(strcmp(known_group_specs[i]->root, name) == 0) group_id = known_group_specs[i]->group_id;
    }
    printf("found %d.\n",group_id);

    return group_id;
}


struct escdf_group {
    const escdf_handle_t *escdf_handle;

    const escdf_group_specs_t *specs;

    char * name;

    hid_t loc_id; /**< Handle for HDF5 group */

    escdf_attribute_t **attr;
    escdf_dataset_t   **datasets;

    bool *datasets_present;
};

/************************************************************
 * Helper routines for groups                               *
 ************************************************************/

escdf_attribute_t * _escdf_group_get_arribute_from_name(escdf_group_t *group, const char *name)
{
    int i, result;
    bool found;

    assert(group != NULL);

    for (found = false, i = 0; i < group->specs->nattributes && found == false; i++) {
        if ( strcmp(group->specs->attr_specs[i]->name, name) == 0 ) {result = i; found=true;}
    }

    if(found)
        return group->attr[result];
    else    
        return NULL;

}

escdf_dataset_t * _escdf_group_get_dataset_from_name(escdf_group_t *group, const char *name)
{
    int i, result;
    bool found;

    assert(group != NULL);

    for (found = false, i = 0; i < group->specs->ndatasets && found == false; i++) {
        if ( strcmp(group->specs->data_specs[i]->name, name) == 0 ) {result = i; found=true;}
    }

    if(found)
        return group->datasets[result];
    else    
        return NULL;

}

escdf_dataset_t * _escdf_group_get_dataset_form_id(escdf_group_t *group, hid_t dtset_id)
{
    int i, result;
    bool found;

    assert(group != NULL);

    for (found = false, i = 0; i < group->specs->ndatasets && found == false; i++) {
        if ( escdf_dataset_get_id(group->datasets[i])  == dtset_id ) {result = i; found=true;}
    }

    if(found)
        return group->datasets[result];
    else    
        return NULL;

}


int _escdf_group_get_dataset_number_from_name(escdf_group_t *group, const char *name)
{
    int i, result;
    bool found;

    assert(group != NULL);

    for (found = false, i = 0; i < group->specs->ndatasets && found == false; i++) {
        if ( strcmp(group->specs->data_specs[i]->name, name) == 0 ) {result = i; found=true;}
    }

    if(found)
        return result;
    else    
        return ESCDF_UNDEFINED_ID;

}


const escdf_attribute_specs_t * _get_attribute_specs(escdf_group_specs_t *group_specs, char *name)
{
    unsigned int ii;

    for (ii=0; ii<group_specs->nattributes; ii++) {
        if (strcmp(group_specs->attr_specs[ii]->name, name) == 0)
            return group_specs->attr_specs[ii];
    }

    return NULL;
}

const escdf_dataset_specs_t * _get_dataset_specs(escdf_group_specs_t *group_specs, char *name)
{
    unsigned int ii;

    for (ii=0; ii<group_specs->ndatasets; ii++) {
        if (strcmp(group_specs->data_specs[ii]->name, name) == 0)
            return group_specs->data_specs[ii];
    }

    return NULL;
}


/******************************************************************************
 * Low-level creators and destructors                                         *
 ******************************************************************************/

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

    group->loc_id = ESCDF_UNDEFINED_ID; /* was -1 */

    group->attr = (escdf_attribute_t **) malloc(group->specs->nattributes * sizeof(escdf_attribute_t *));
    if (group->attr == NULL) {
        free(group);
    } else {
        for (ii=0; ii<group->specs->nattributes; ii++)
            group->attr[ii] = NULL;
    }

    group->datasets = (escdf_dataset_t **) malloc(group->specs->ndatasets * sizeof(escdf_dataset_t *));
    if (group->datasets == NULL) {
        free(group);
    } else {
        for (ii=0; ii<group->specs->ndatasets; ii++)
            group->datasets[ii] = NULL;
    }


    group->datasets_present = (bool *) malloc(group->specs->ndatasets * sizeof(bool));

    return group;
}

void escdf_group_free(escdf_group_t *group)
{
    unsigned int ii;

    if (group != NULL) {
        for (ii=0; ii<group->specs->nattributes; ii++)
            escdf_attribute_free(group->attr[ii]);
        free(group->attr);
        for (ii=0; ii<group->specs->ndatasets; ii++)
            escdf_dataset_free(group->datasets[ii]);

        free(group->datasets);
        free(group->datasets_present);
    }
    free(group);

    group = NULL;
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

//    group->loc_id = H5Gopen2(handle->group_id, location_path, H5P_DEFAULT);

    FULFILL_OR_RETURN( utils_hdf5_open_group(handle->group_id, location_path, &group->loc_id) == ESCDF_SUCCESS, group->loc_id);

//    FULFILL_OR_RETURN(group->loc_id >= 0, group->loc_id);

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


/******************************************************************************
 * High-level creators and destructors                                        *
 ******************************************************************************/

escdf_group_t * escdf_group_open(const escdf_handle_t *handle, const char* group_name, const char *instance_name)
{
    escdf_group_t *group;
    escdf_group_id group_id;

    /* get group_id corresponding to the group name */

    FULFILL_OR_RETURN_VAL( group_id = _escdf_get_group_id(group_name) != ESCDF_UNDEFINED_ID, ESCDF_ERROR, NULL);

    if ((group = escdf_group_new(group_id)) == NULL)
        return NULL;

    if (escdf_group_open_location(group, handle, instance_name) != ESCDF_SUCCESS)
        goto cleanup;

    if (escdf_group_read_attributes(group) != ESCDF_SUCCESS) {
        escdf_group_close_location(group);
        goto cleanup;
    }

    if (escdf_group_query_datasets(group) != ESCDF_SUCCESS) {
        escdf_group_close_location(group);
        goto cleanup;
    }

    return group;

    cleanup:
    escdf_group_free(group);
    return NULL;
}

escdf_group_t * escdf_group_create(const escdf_handle_t *handle, const char *group_name, const char* instance_name)
{
    escdf_group_t *group;
    escdf_group_id group_id;
    
    /* get group_id corresponding to the group name */

    FULFILL_OR_RETURN_VAL( (group_id = _escdf_get_group_id(group_name)) != ESCDF_UNDEFINED_ID, ESCDF_ERROR, NULL);

    printf("Looking for group %s\n", group_name);


    /* create new escdf_group instance */

    if ((group = escdf_group_new(group_id)) == NULL)
        return NULL;

    printf("Found group ID = %d with name %s \n", group_id, group->specs->root);


    /* create group in the file */

    if (escdf_group_create_location(group, handle, instance_name) != ESCDF_SUCCESS) {
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


escdf_errno_t _escdf_group_dataset_new(escdf_group_t *group, unsigned int idata) {

    unsigned int i, ii, idim, ndims, dim_ID;
    bool found;

    escdf_attribute_t **dims = NULL;
    escdf_errno_t error;


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

escdf_errno_t escdf_group_query_datasets(const escdf_group_t *group)
{
    int i;

    assert(group != NULL);

    for(i=0; i<group->specs->ndatasets; i++) {
        group->datasets_present[i] =  utils_hdf5_check_present(group->loc_id, group->specs->attr_specs[i]->name);
    }

    return ESCDF_SUCCESS;
}


/************************************************************
 * High level routines for accessing datasets in a group    *
 ************************************************************/



escdf_dataset_t *escdf_group_dataset_create(escdf_group_t *group, const char *name)
{
    unsigned int dataset_id;
    escdf_dataset_t *dataset;

    assert(group!=NULL);

    /* get dataset ID */

    dataset_id = _escdf_group_get_dataset_number_from_name(group, name);

    if(dataset_id == ESCDF_UNDEFINED_ID)  return NULL;
    
    /* create dataset structure */

    FULFILL_OR_RETURN_VAL(_escdf_group_dataset_new(group, dataset_id), ESCDF_ERROR, NULL);

    dataset = group->datasets[dataset_id];

    /* create dataset in file */

    if( escdf_dataset_create(dataset, group->loc_id) != ESCDF_SUCCESS ) {
        escdf_dataset_close(dataset);
        FULFILL_OR_RETURN_VAL(false, ESCDF_ERROR, NULL);
    }

    return dataset;   
}


escdf_dataset_t *escdf_group_dataset_open(escdf_group_t *group, const char *name)
{
    escdf_dataset_t *dataset;

    unsigned int dataset_id;

    assert(group!=NULL);

    dataset_id = _escdf_group_get_dataset_number_from_name(group, name);

    if(dataset_id == ESCDF_UNDEFINED_ID)  return NULL;

    FULFILL_OR_RETURN_VAL(_escdf_group_dataset_new(group, dataset_id), ESCDF_ERROR, NULL);

    dataset = group->datasets[dataset_id];

    /* open dataset in the file */

    if( escdf_dataset_open(dataset, group->loc_id) != ESCDF_SUCCESS) {
        escdf_dataset_free(dataset);
        FULFILL_OR_RETURN_VAL(false, ESCDF_ERROR, NULL);    
    }

    group->datasets_present[dataset_id] = true;

    return dataset;   
}

escdf_errno_t escdf_group_dataset_close(escdf_group_t *group, const char *name)
{
    unsigned int dataset_id;

    assert(group != NULL);

    dataset_id = _escdf_group_get_dataset_number_from_name(group, name);

    if(dataset_id == ESCDF_UNDEFINED_ID)  return ESCDF_SUCCESS; /* QUESTION: Shall wi throw an error ? */

    FULFILL_OR_RETURN(escdf_dataset_close(group->datasets[dataset_id]) == ESCDF_SUCCESS, ESCDF_ERROR);

    escdf_dataset_free(group->datasets[dataset_id]);


    return ESCDF_SUCCESS;

}


escdf_errno_t escdf_group_dataset_write_simple(escdf_dataset_t *data, void* buf)
{
    assert(data != NULL);

    return ESCDF_SUCCESS;
}


/*
escdf_errno_t escdf_group_dataset_write_at(const escdf_group_t *group, escdf_dataset_t *data, 
                                            const hid_t *start, const hid_t *count, const hid_t * stride, void* buf)
{
    assert(group != NULL);
}

escdf_errno_t escdf_group_dataset_read_at(const escdf_group_t *group, const escdf_dataset_t *data, 
                                            const hid_t *start, const hid_t *count, const hid_t * stride, void *buf)
{

}

*/

