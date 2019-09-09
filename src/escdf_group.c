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
#ifdef HAVE_CHECK_H
#include <check.h>
#endif

#include "escdf_private_group.h"
#include "escdf_group.h"

/* #include "escdf_groups_specs.h" */

#include "utils_hdf5.h"

#define MAX_KNOWN_SPECS 100


static unsigned int n_known_specs = 0;
static escdf_group_specs_t const * known_group_specs[MAX_KNOWN_SPECS];

escdf_errno_t escdf_group_specs_register(const escdf_group_specs_t *specs) {

    int i;

    FULFILL_OR_RETURN(specs != NULL, ESCDF_EVALUE)

    FULFILL_OR_RETURN(n_known_specs < MAX_KNOWN_SPECS, ESCDF_ESIZE)

    known_group_specs[n_known_specs] = specs;
    n_known_specs++;

#ifdef DEBUG
    printf("Registering group %d, %s: %d %d %d\n", specs->group_id, specs->name, specs->nattributes, specs->ndatasets, specs->nsubgroups);
    
    for(i=0; i<specs->nattributes; i++) printf("   attribute %d: %s, %d %d %d \n",
        i, specs->attr_specs[i]->name, specs->attr_specs[i]->datatype,
        specs->attr_specs[i]->stringlength, specs->attr_specs[i]->ndims);
    
    for(i=0; i<specs->ndatasets; i++) printf("   dataset %d: %s, %d %d %d %s \n",
        i, specs->data_specs[i]->name, specs->data_specs[i]->datatype, 
        specs->data_specs[i]->stringlength, specs->data_specs[i]->ndims, 
        specs->data_specs[i]->disordered_storage_allowed?"yes":"no");

    for(i=0; i<specs->nsubgroups; i++) printf("   subgroup %d: %s \n",
        i, specs->subgroup_specs[i]->name);

    fflush(stdout);
#endif
    
    return ESCDF_SUCCESS;
}

void escdf_group_specs_cleanup(){

    unsigned int i;

    for(i=0; i<n_known_specs; i++) known_group_specs[i] = NULL;
    n_known_specs = 0;
}


/************************************************************
 * Helper routines for groups                               *
 ************************************************************/

escdf_group_id_t _escdf_get_group_id(const char* name)
{
    int i;
    escdf_group_id_t group_id;

    group_id = ESCDF_UNDEFINED_ID;

    for(i=0; i<n_known_specs; i++) {
        /* printf("searching for %s: %d %s %d \n", name, i, known_group_specs[i]->root, known_group_specs[i]->group_id ); */
        if(strcmp(known_group_specs[i]->name, name) == 0) group_id = known_group_specs[i]->group_id;
    }
    /* printf("found %d.\n",group_id); */

    return group_id;
}

int _escdf_group_get_attribute_index(const escdf_group_t *group, escdf_attribute_id_t attribute_id)
{
    bool found;
    unsigned int i, index;

    index = -999;

    assert(group!=NULL);

    for (found = false, i = 0; (!found) && (i < group->specs->nattributes); i++) {
        if ( group->specs->attr_specs[i]->id == attribute_id) {index = i; found=true;}
    }

    if(!found) REGISTER_ERROR(ESCDF_ERROR);

    return index;
}


int _escdf_group_get_dataset_index(const escdf_group_t *group, escdf_dataset_id_t dataset_id)
{
    bool found;
    unsigned int i, index;

    index = -999;

    assert(group!=NULL);

    for (found = false, i = 0; (!found) && (i < group->specs->ndatasets); i++) {
        if ( group->specs->data_specs[i]->id == dataset_id) {index = i; found=true;}
    }

    if(!found) REGISTER_ERROR(ESCDF_ERROR);

    return index;
}



const escdf_attribute_specs_t * escdf_group_get_attribute_specs(escdf_group_t *group, const char *name)
{
    int i, result;
    bool found;

    assert(group != NULL);

    for (found = false, i = 0; (!found) && (i < group->specs->nattributes); i++) {
        if ( strcmp(group->specs->attr_specs[i]->name, name) == 0 ) {result = i; found=true;}
    }

    if(found)
        return group->specs->attr_specs[result];
    else    
        return NULL;
}

const escdf_dataset_specs_t * escdf_group_get_dataset_specs(escdf_group_t *group, const char *name)
{
    int i, result;
    bool found;

    assert(group != NULL);

    for (found = false, i = 0; (!found) && (i < group->specs->ndatasets); i++) {
        if ( strcmp(group->specs->data_specs[i]->name, name) == 0 ) {result = i; found=true;}
    }

    if(found)
        return group->specs->data_specs[result];
    else    
        return NULL;
}



escdf_attribute_t * _escdf_group_get_arribute_from_name(escdf_group_t *group, const char *name)
{
    int i, result;
    bool found;

    assert(group != NULL);

    for (found = false, i = 0; (!found) && (i < group->specs->nattributes); i++) {
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

    for (found = false, i = 0; (!found) && (i < group->specs->ndatasets); i++) {
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

    for (found = false, i = 0; (!found) && (i < group->specs->ndatasets); i++) {
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

    /* printf("searching for dataset_number of %s. ndatasets = %d \n", name, group->specs->ndatasets); fflush(stdout); */

    

    for(found = false,i = 0; (!found) && (i < group->specs->ndatasets); i++) {
        /* printf("searching for %s: %d %s\n", name, i, group->specs->data_specs[i]->name); fflush(stdout); */
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

escdf_group_t * escdf_group_new(escdf_group_id_t group_id)
{
    escdf_group_t *group = NULL;
    unsigned int ii;

#ifdef DEBUG
    printf("%s (%s, %d): group_id = %d\n", __func__, __FILE__, __LINE__, group_id); fflush(stdout);
#endif

    group = (escdf_group_t *) malloc(sizeof(escdf_group_t));
    if (group == NULL)
        return NULL;

#ifdef DEBUG
    printf("%s (%s, %d): memory allocated.\n", __func__, __FILE__, __LINE__); fflush(stdout);
#endif

    group->specs = NULL;
    for (bool found=false, ii = 0; (!found) && (ii < n_known_specs); ii++) {
        if (known_group_specs[ii]->group_id == group_id) {
            group->specs = known_group_specs[ii];
            found = true;
        }
    }
    if (group->specs == NULL) {
        printf("escdf_group_new: No specifications found! \n"); fflush(stdout);
        free(group);
        return NULL;
    }

#ifdef DEBUG
    printf("%s (%s, %d): specs associated.\n", __func__, __FILE__, __LINE__); fflush(stdout);
#endif

    group->loc_id = ESCDF_UNDEFINED_ID; /* was -1 */

    if(group->specs->nattributes>0) {
        group->attr = (escdf_attribute_t **) malloc(group->specs->nattributes * sizeof(escdf_attribute_t *));
        if (group->attr == NULL) {
            free(group);
            group = NULL;
        } else {
            for (ii=0; ii<group->specs->nattributes; ii++)
                group->attr[ii] = NULL;
        }
    }
#ifdef DEBUG
    printf("%s (%s, %d): attributes allocated.\n", __func__, __FILE__, __LINE__); fflush(stdout);
#endif

    if(group->specs->ndatasets>0) {
        group->datasets = (escdf_dataset_t **) malloc(group->specs->ndatasets * sizeof(escdf_dataset_t *));
        if (group->datasets == NULL) {
            free(group);
            group = NULL;
        } else {
            for (ii=0; ii<group->specs->ndatasets; ii++) group->datasets[ii] = NULL;
        }
    }
#ifdef DEBUG
    printf("%s (%s, %d): datasets allocated.\n", __func__, __FILE__, __LINE__); fflush(stdout);
#endif


    group->root = (char*) malloc(strlen(group->specs->name));
    assert(group->root!=NULL);
    strcpy(group->root, group->specs->name);

#ifdef DEBUG
    printf("%s (%s, %d): group_name = %s\n", __func__, __FILE__, __LINE__, group->root); fflush(stdout);
#endif


    group->datasets_present = (bool *) malloc(group->specs->ndatasets * sizeof(bool));
    for (ii=0; ii<group->specs->ndatasets; ii++)group->datasets_present[ii] = false;

    /*
    printf("escdf_group_new: created new escdf_group_t %s with %d attributes and %d datasets\n", 
        group->specs->root, group->specs->nattributes, group->specs->ndatasets);
    fflush(stdout);
    */
#ifdef DEBUG
    printf("%s (%s, %d): end.\n", __func__, __FILE__, __LINE__); fflush(stdout);
#endif

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
        free(group->root);
    }
    free(group);
}

escdf_errno_t escdf_group_open_location(escdf_group_t *group, const escdf_handle_t *handle, const char *name)
{
    char location_path[ESCDF_STRLEN_GROUP];

    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE)
    FULFILL_OR_RETURN(handle != NULL, ESCDF_EVALUE)

    if (name == NULL)
        sprintf(location_path, "%s", group->root);
    else
        sprintf(location_path, "%s/%s", group->root, name);

        /*    group->loc_id = H5Gopen2(handle->group_id, location_path, H5P_DEFAULT); */

#ifdef DEBUG
    printf("%s (%s, %d): opening location for %s. \n",  __func__, __FILE__, __LINE__, location_path); fflush(stdout);
#endif

    FULFILL_OR_RETURN( utils_hdf5_open_group(handle->group_id, location_path, &group->loc_id) == ESCDF_SUCCESS, group->loc_id);

    /*  FULFILL_OR_RETURN(group->loc_id >= 0, group->loc_id); */

#ifdef DEBUG
    printf("%s (%s, %d): opened location for %s resulting in loc_id =  %d. \n",  __func__, __FILE__, __LINE__, location_path, group->loc_id); fflush(stdout);
#endif

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_group_create_location(escdf_group_t *group, const escdf_handle_t *handle, const char *name)
{
    char location_path[ESCDF_STRLEN_GROUP];

#ifdef DEBUG
    printf("%s (%s, %d): before initial checks. \n",  __func__, __FILE__, __LINE__); fflush(stdout);
#endif

    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE)
    FULFILL_OR_RETURN(handle != NULL, ESCDF_EVALUE)

#ifdef DEBUG
    printf("%s (%s, %d): after initial checks. \n",  __func__, __FILE__, __LINE__); fflush(stdout);
#endif

    if (name == NULL)
        sprintf(location_path, "%s", group->specs->name);
    else
        sprintf(location_path, "%s/%s", group->specs->name, name);

//    utils_hdf5_create_group(handle->file_id, location_path, &(group->loc_id));
    utils_hdf5_create_group(handle->group_id, location_path, &(group->loc_id));

#ifdef DEBUG
    printf("%s (%s, %d): after utils_hdf5_create_group. \n",  __func__, __FILE__, __LINE__); fflush(stdout);
#endif


    FULFILL_OR_RETURN(group->loc_id >= 0, group->loc_id);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_group_close_location(escdf_group_t *group)
{
    herr_t herr_status;

    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE)

#ifdef DEBUG
    printf("%s (%s, %d): closing location for %s. \n",  __func__, __FILE__, __LINE__, group->specs->name); fflush(stdout);
#endif

    /* close the HDF5 group in the file */
    if (group->loc_id >= 0) {
        herr_status = H5Gclose(group->loc_id);
        FULFILL_OR_RETURN(herr_status >= 0, herr_status);
    }

    return ESCDF_SUCCESS;
}


/******************************************************************************
 * High-level creators and destructors                                        *
 ******************************************************************************/

escdf_group_t * escdf_group_open(const escdf_handle_t *handle, escdf_group_id_t group_id, const char *instance_name)
{
    escdf_group_t *group;

    int i;

#ifdef DEBUG
    printf("%s (%s, %d): opening group %d \n", __func__, __FILE__, __LINE__, group_id); 
#endif

    if ((group = escdf_group_new(group_id)) == NULL) {
#ifdef DEBUG        
        printf("%S (%S, %d): problem creating the group %s! \n", __func__, __FILE__, __LINE__, known_group_specs[group_id]->name);
#endif
        REGISTER_ERROR(ESCDF_ERROR);
        return NULL;
    }

    /* we need to check first whether the group exist in the file */


    if (escdf_group_open_location(group, handle, instance_name) != ESCDF_SUCCESS) {
        printf("escdf_group_open: opening group %s failed. \n", group->specs->name);
        goto cleanup;
    }

#ifdef DEBUG
    printf("%s (%s, %d): after escdf_open_group_location.\n", __func__, __FILE__, __LINE__);
#endif

    if (escdf_group_read_attributes(group) != ESCDF_SUCCESS) {
#ifdef DEBUG
        printf("%s (%s, %d): reading attributes for group %s failed.\n",  __func__, __FILE__, __LINE__, group->specs->name);
#endif
        escdf_group_close_location(group);
        goto cleanup;
    }

#ifdef DEBUG
    printf("%s (%s, %d): after reading attributes for group %s. \n",  __func__, __FILE__, __LINE__, group->specs->name);
#endif

    if (escdf_group_query_datasets(group) != ESCDF_SUCCESS) {
        escdf_group_close_location(group);
        goto cleanup;
    }

#ifdef DEBUG
    printf("escdf_group_open: queried datasets for group %s. \n", group->specs->name); 
#endif

    for(i=0; i< group->specs->ndatasets; i++) {
        printf("escdf_group_open: dataset '%s' is %s. \n", group->specs->data_specs[i]->name, group->datasets_present[i]?"present":"NOT present");
    }

    return group;

    cleanup:
    escdf_group_free(group);
    return NULL;
}

escdf_group_t * escdf_group_create(const escdf_handle_t *handle, escdf_group_id_t group_id, const char* instance_name)
{
    escdf_group_t *group;
    
#ifdef DEBUG
    printf("%s (%s, %d): creating group %d : %s \n", __func__, __FILE__, __LINE__, group_id, known_group_specs[group_id]->name); fflush(stdout);
#endif


    /* create new escdf_group instance */

    if ((group = escdf_group_new(group_id)) == NULL) {
        printf("escdf_group_create: escdf_group_new failed!\n"); fflush(stdout);
        return NULL;
    }
    /* create group in the file */

    if (escdf_group_create_location(group, handle, instance_name) != ESCDF_SUCCESS) {
        printf("escdf_group_create: escdf_group_create_location failed!\n"); fflush(stdout);
        escdf_group_free(group);
        return NULL;
    }

    if(instance_name != NULL) {
        group->root = (char*) malloc( strlen(instance_name)  + 1);
        strcpy(group->root, instance_name);
    } else {
        group->root = (char*) malloc(   + 1);
        strcpy(group->root, "");
    }
    
    group->escdf_handle = handle;

    return group;
}

escdf_errno_t escdf_group_close(escdf_group_t *group)
{
    escdf_errno_t err;

#ifdef DEBUG
    printf("%s (%s, %d): closing group: %s \n", __func__, __FILE__, __LINE__, group->specs->name); fflush(stdout);
#endif

    if (group != NULL) {
        if ((err = escdf_group_close_location(group)) != ESCDF_SUCCESS)
            return err;

        escdf_group_free(group);
    }

    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_group_read_attributes(escdf_group_t *group)
{
    unsigned int iattr;
    escdf_errno_t error;

    FULFILL_OR_RETURN(group->specs != NULL, ESCDF_EVALUE);

#ifdef DEBUG
    printf("%s (%s, %d): after initial checks. \n",  __func__, __FILE__, __LINE__);
#endif

    for (iattr = 0; iattr < group->specs->nattributes; iattr++) {

        if (group->attr[iattr] == NULL) {
            error = _escdf_group_attribute_new(group, group->specs->attr_specs[iattr]->id);
#ifdef DEBUG
            printf("%s (%s, %d): _escdf_group_attribute_new() resulted in %d. \n",  __func__, __FILE__, __LINE__, error);
#endif
	        SUCCEED_OR_RETURN(error);
        }

        if(escdf_attribute_specs_is_present(group->specs->attr_specs[iattr], group->loc_id)) {       
	
	        if (!escdf_attribute_is_set(group->attr[iattr])) {
                error = escdf_attribute_read(group->attr[iattr], group->loc_id);
#ifdef DEBUG
              printf("%s (%s, %d): _escdf_attribute_read() resulted in %d. \n",  __func__, __FILE__, __LINE__, error);
#endif
	            SUCCEED_OR_RETURN(error);
	        }   
        }
        else {
	        /* We might want to throw an error here */
	        printf("WARNING: Attribute '%s' not found in file. \n", group->specs->attr_specs[iattr]->name);
        }
    }
    return ESCDF_SUCCESS;
}




escdf_errno_t escdf_group_attribute_set(escdf_group_t *group, escdf_attribute_id_t attribute_id, const void* buf)
{
    unsigned int iattr, i;
    bool attr_found = false;
    escdf_errno_t error;
  
    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(group->specs != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(buf != NULL, ESCDF_EVALUE);

    for (attr_found = false, i = 0; i < group->specs->nattributes; i++) {
        assert(group->specs->attr_specs[i] != NULL);
#ifdef DEBUG__
        printf("%s (%s, %d): (%s,%u): probing attribute %u(%u) : ID = %u \n", __func__, __FILE__, __LINE__, group->name, attribute_id, i, group->specs->nattributes, group->specs->attr_specs[i]->id);    
#endif        
        if ( group->specs->attr_specs[i]->id == attribute_id ) {iattr = i; attr_found=true;}
    }
    FULFILL_OR_RETURN(attr_found == true, ESCDF_ERROR);

#ifdef DEBUG
    printf("%s (%s, %d): (%s,%u): found attribute %u(%u) \n", __func__, __FILE__, __LINE__, group->specs->name, attribute_id, iattr, group->specs->nattributes);    
#endif

    FULFILL_OR_RETURN(group->specs->attr_specs[iattr] != NULL, ESCDF_EVALUE);

    if (group->attr[iattr] == NULL) {
        error = _escdf_group_attribute_new(group, attribute_id);
#ifdef DEBUG
        printf("%s (%s, %d): (%s,%u): created attribute %u(%u). Error = %d \n", __func__, __FILE__, __LINE__, group->specs->name, attribute_id, iattr, group->specs->nattributes, error);    
#endif
        SUCCEED_OR_RETURN(error = ESCDF_SUCCESS);
    }
    assert(group->attr[iattr]!=NULL);

    SUCCEED_OR_RETURN(escdf_attribute_set(group->attr[iattr], buf));
  
    /* If the attribute has successfully been set in memory, we can write to disk */

    SUCCEED_OR_RETURN(escdf_attribute_write(group->attr[iattr], group->loc_id));
  
    return ESCDF_SUCCESS;
}



escdf_errno_t escdf_group_attribute_get(escdf_group_t *group, escdf_attribute_id_t attribute_id, void *buf)
{
    unsigned int iattr, i;
    bool attr_found;

    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(group->specs != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(buf != NULL, ESCDF_EVALUE);

    for (attr_found = false, i = 0; i < group->specs->nattributes; i++) {
        if ( group->specs->attr_specs[i]->id == attribute_id) {iattr = i; attr_found=true;}
    }
    FULFILL_OR_RETURN(attr_found == true, ESCDF_ERROR);
    FULFILL_OR_RETURN(group->specs->attr_specs[iattr] != NULL, ESCDF_EVALUE);

    if (group->attr[iattr] == NULL) {
        SUCCEED_OR_RETURN(_escdf_group_attribute_new(group, iattr));
    }

    /* check whether the attribute already as a value in memory */

    if (!escdf_attribute_is_set(group->attr[iattr])) {
        SUCCEED_OR_RETURN(escdf_attribute_read(group->attr[iattr], group->loc_id));
    }

    SUCCEED_OR_RETURN(escdf_attribute_get(group->attr[iattr], buf));
  
    return ESCDF_SUCCESS;
  
}

escdf_errno_t _escdf_group_attribute_new(escdf_group_t *group, escdf_attribute_id_t attribute_id)
{ 

    unsigned int i, ii, idim, ndims, dim_ID, index;
    bool attr_found;

    escdf_attribute_t **dims = NULL;

    /* need to create the attribute */ 

    assert(group!=NULL);
    
    FULFILL_OR_EXIT(group->specs != NULL, ESCDF_EVALUE);

    for (attr_found = false, i = 0; i < group->specs->nattributes; i++) {
        if ( group->specs->attr_specs[i]->id == attribute_id) {index = i; attr_found=true;}
    }
    FULFILL_OR_RETURN(attr_found == true, ESCDF_ERROR);

    /* determine the dimensions from linked dimension attributes */

    ndims = group->specs->attr_specs[index]->ndims;
#ifdef DEBUG
    printf("%s(%d): %s: name = %s, ndims = %u\n", __FILE__, __LINE__, __func__,  group->specs->attr_specs[index]->name, ndims);
/*    printf("_escdf_group_attribute_new: name = %s, ndims = %u\n", group->specs->attr_specs[index]->name, ndims); */
#endif

    if( ndims > 0 ) {
      
        dims = (escdf_attribute_t**) malloc(ndims * sizeof(escdf_attribute_t*));
      
        FULFILL_OR_RETURN(dims != NULL, ESCDF_ERROR);
      
        for(i = 0; i<ndims; i++) {
	
	        FULFILL_OR_RETURN_CLEAN( group->specs->attr_specs[index] != NULL, ESCDF_EVALUE, dims );
	        FULFILL_OR_RETURN_CLEAN( group->specs->attr_specs[index]->dims_specs[i] != NULL, ESCDF_EVALUE, dims );
	

	        idim = group->specs->attr_specs[index]->dims_specs[i]->id;
		  
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


    group->attr[index] = escdf_attribute_new(group->specs->attr_specs[index], dims);
#ifdef DEBUG
    printf("_escdf_group_attribute_new( %s ): set group->attr[%d] \n", group->specs->name, index);
#endif


    assert(group->attr[index]!=NULL);

    if(dims != NULL) free(dims);

    return ESCDF_SUCCESS;

}


escdf_errno_t _escdf_group_dataset_new(escdf_group_t *group, escdf_dataset_id_t dataset_id) {

    unsigned int i, ii;        
    unsigned int idim, idata;  /* array indices for dimension attributes and dataset */
    unsigned int ndims;

    bool found;

    escdf_attribute_id_t dim_ID;
    escdf_attribute_t **dims = NULL;
    escdf_dataset_t *data;

    /* initial checks */

    assert(group!=NULL);

    if(group->datasets == NULL) printf("_escdf_group_dataset_new: group->datasets==NULL!!\n");
    FULFILL_OR_EXIT(group->datasets != NULL, ESCDF_EVALUE);

    /* determine storage index of the dataset from the dataset_id */

    idata = _escdf_group_get_dataset_index(group, dataset_id);

    assert(idata >= 0);

    /* determine the dimensions from linked dimension attributes */

    ndims = group->specs->data_specs[idata]->ndims;

#ifdef DEBUG
    printf("%s (%s, %d): for \"%s\", ndims = %d\n", __func__, __FILE__, __LINE__, group->specs->data_specs[idata]->name, ndims); fflush(stdout); 
#endif

    if( ndims > 0 ) {
        
        dims = (escdf_attribute_t**) malloc(ndims * sizeof(escdf_attribute_t*));
        if(dims == NULL) printf("%s (%s, %d): malloc error (dims==NULL) !!\n", __func__, __FILE__, __LINE__);
        FULFILL_OR_RETURN(dims != NULL, ESCDF_ERROR);
      
        for(i = 0; i<ndims; i++) {

#ifdef DEBUG	
            if(group->specs->data_specs[idata] == NULL) printf("%s (%s, %d): group->specs->data_specs[%d]==NULL!!\n", __func__, __FILE__, __LINE__, idata);
            if(group->specs->data_specs[idata]->dims_specs[i] == NULL) printf("%s (%s, %d): group->specs->data_specs[%d]->dims_specs[%d]==NULL!!\n", __func__, __FILE__, __LINE__, idata, i);
#endif
	        FULFILL_OR_RETURN_CLEAN( group->specs->data_specs[idata] != NULL, ESCDF_EVALUE, dims );
	        FULFILL_OR_RETURN_CLEAN( group->specs->data_specs[idata]->dims_specs[i] != NULL, ESCDF_EVALUE, dims );
	

	        dim_ID = group->specs->data_specs[idata]->dims_specs[i]->id;

/*
#ifdef DEBUG            
            printf("%s (%s, %d): for %s, dim_ID = %d\n", __func__, __FILE__, __LINE__, group->specs->data_specs[idata]->name, dim_ID); fflush(stdout); 
#endif            
*/

	        for (found=false, ii = 0; ii < group->specs->nattributes; ii++) {
	            if (group->specs->attr_specs[ii]->id == dim_ID) {idim = ii; found=true;}
	        }
#ifdef DEBUG
            if(!found) printf("%s (%s, %d): dim_ID = %i not found!\n", __func__, __FILE__, __LINE__, dim_ID);
#endif            
            FULFILL_OR_RETURN_CLEAN( found == true, ESCDF_ERROR, dims );

#ifdef DEBUG        
            printf("%s (%s, %d): for \"%s\", found \"%s\" (dim_ID = %d, index = %d) with %d dimensions.\n", __func__, __FILE__, __LINE__,
                group->specs->data_specs[idata]->name, 
                group->specs->attr_specs[idim]->name, dim_ID, idata, 
                group->specs->attr_specs[idim]->ndims); 
            fflush(stdout);
#endif            


	        if(group->attr[idim] == NULL) {
                printf("%s (%s, %d) for %s: group->attrib[%d] = NULL!\n", __func__, __FILE__, __LINE__, group->specs->data_specs[idata]->name, idim );
	            if (dims != NULL) free(dims);
	            return ESCDF_ERROR_DIM;
	        }
	        else {
	            FULFILL_OR_RETURN_CLEAN( group->attr[idim] !=  NULL, ESCDF_EVALUE, dims );
	            dims[i] = group->attr[idim];
	        }
        }
      
    }
    else { /* ndims == 0 case */
        dims = NULL;
#ifdef DEBUG
        printf("%s (%s, %d): setting dims=NULL!!\n", __func__, __FILE__, __LINE__);
#endif
    }

#ifdef DEBUG_
    for(i=0; i<ndims; i++) {
        void *d = malloc( escdf_attribute_sizeof(dims[i]));
        escdf_errno_t error = escdf_attribute_get(dims[i], d);
  
        assert( error == ESCDF_SUCCESS);

        printf("%s (%s, %d):  dims[%d]->name = %s.\n", __func__, __FILE__, __LINE__, i, escdf_attribute_get_specs(dims[i])->name); 
        free(d);
    }
#endif



    data = escdf_dataset_new(group->specs->data_specs[idata], dims);
#ifdef DEBUG
    printf("%s (%s, %d): dataset created\n", __func__, __FILE__, __LINE__); 
#endif

    group->datasets[idata] = data;



#ifdef DEBUG
    if(!group->specs->data_specs[idata]->compact) {
        size_t *dims_check = escdf_dataset_get_dimensions(group->datasets[idata]);
        for(i=0; i<ndims; i++) {
            printf("%s (%s, %d): Dims_check[%d] = %lu\n", __func__, __FILE__, __LINE__, i,dims_check[i]);
        }
        if(dims_check != NULL) free(dims_check);
    } 
#endif

    if(dims != NULL) free(dims);

    /*
    printf("_escdf_group_dataset_new: done. \n");
    printf( "type_id = %lld; from specs: %lld\n\n", escdf_dataset_get_type_id(group->datasets[idata]), \
        utils_hdf5_disk_type(group->specs->data_specs[idata]->datatype) ); 
    fflush(stdout);
    */

#ifdef DEBUG
    printf("%s (%s, %d): end.\n", __func__, __FILE__, __LINE__);
#endif   

    return ESCDF_SUCCESS;

};


escdf_errno_t _escdf_group_subgroup_new(escdf_group_t *group, escdf_group_id_t subgroup_id)
{ 

    unsigned int i, ii, index;
    bool subgroup_found;

    escdf_attribute_t **dims = NULL;

    /* need to create the attribute */ 

    assert(group!=NULL);
    
    FULFILL_OR_EXIT(group->specs != NULL, ESCDF_EVALUE);

    for (subgroup_found = false, i = 0; i < group->specs->nsubgroups; i++) {
        if ( group->specs->subgroup_specs[i]->group_id == subgroup_id) {index = i; subgroup_found=true;}
    }
    FULFILL_OR_RETURN(subgroup_found == true, ESCDF_ERROR);


    group->subgroups[index] = escdf_group_new(group->specs->subgroup_specs[index]->group_id);
#ifdef DEBUG
    printf("_escdf_group_subgroup_new( %s ): set group->subgroup[%d] \n", group->specs->name, index);
#endif


    assert(group->subgroups[index]!=NULL);


    return ESCDF_SUCCESS;
}



escdf_errno_t escdf_group_query_datasets(const escdf_group_t *group)
{
    int i;

    assert(group != NULL);

    for(i=0; i<group->specs->ndatasets; i++) {
        group->datasets_present[i] =  utils_hdf5_check_present(group->loc_id, group->specs->data_specs[i]->name);
    }

    return ESCDF_SUCCESS;
}


/************************************************************
 * High level routines for accessing datasets in a group    *
 ************************************************************/



escdf_dataset_t *escdf_group_dataset_create(escdf_group_t *group, escdf_dataset_id_t dataset_id)
{
    unsigned int idata, i;
    escdf_dataset_t *dataset;
    escdf_errno_t err;
    bool found;

    assert(group!=NULL);

    /* get dataset index */

    for (found = false, i = 0; i < group->specs->ndatasets; i++) {
        if ( group->specs->data_specs[i]->id == dataset_id) {idata = i; found=true;}
    }
    FULFILL_OR_RETURN(found == true, ESCDF_ERROR);

#ifdef DEBUG   
    printf("%s (%s, %d): dataset_id = %d, index = %d. \n",__func__, __FILE__, __LINE__, dataset_id, idata); 
#endif

    if(idata == ESCDF_UNDEFINED_ID)  return NULL;
    

    /* create dataset structure */

    err = _escdf_group_dataset_new(group, dataset_id);
#ifdef DEBUG   
    printf("%s (%s, %d): _escdf_group_dataset_new(group, %d) returned %d. \n",__func__, __FILE__, __LINE__, dataset_id, err); 
#endif

    FULFILL_OR_RETURN_VAL(err == ESCDF_SUCCESS, ESCDF_ERROR, NULL);

    dataset = group->datasets[idata];

    /* print for checks */

    /* id = escdf_dataset_get_id(dataset); */
    /* dataset_name = escdf_dataset_get_name(dataset); */

    /*
    printf("escdf_group_dataset_create: name = %s, dataset->specs->id = %d, %s, loc_id = %lld\n", name, id, dataset_name, group->loc_id);
    fflush(stdout);
    */

    /* create dataset in file */

    /*
    unsigned int *dims_check;

    dims_check = escdf_dataset_get_dimensions(dataset);

    printf("escdf_group_dataset_create: Number of Dimensions = %lld.\n", escdf_dataset_get_number_of_dimensions(dataset) );
    for(int i=0; i<escdf_dataset_get_number_of_dimensions(dataset); i++) {
        printf("escdf_group_dataset_create: Dims check dims[%d] = %lld.\n", i, dims_check[i]);
    }
    */

#ifdef DEBUG
    printf("%s (%s, %d): calling escdf_dataset_create.\n",__func__, __FILE__, __LINE__); fflush(stdout); 
#endif

    err = escdf_dataset_create(dataset, group->loc_id); 

#ifdef DEBUG
    printf("%s (%s, %d): escdf_dataset_create returned %i\n",__func__, __FILE__, __LINE__, err); fflush(stdout); 
#endif

    if( err != ESCDF_SUCCESS ) {
        escdf_dataset_close(dataset);
        FULFILL_OR_RETURN_VAL(false, ESCDF_ERROR, NULL);
    }

    return dataset;   
}


escdf_dataset_t *escdf_group_dataset_open(escdf_group_t *group, escdf_dataset_id_t dataset_id)
{
    escdf_dataset_t *dataset;
    escdf_errno_t err;

    unsigned int idata, i;
    bool found;

    assert(group!=NULL);

    for (found = false, i = 0; i < group->specs->ndatasets; i++) {
        if ( group->specs->data_specs[i]->id == dataset_id) {idata = i; found=true;}
    }
    FULFILL_OR_RETURN(found == true, ESCDF_ERROR);



    if(idata == ESCDF_UNDEFINED_ID)  return NULL;

    /* printf("escdf_group_dataset_open: name = %s, dataset_number = %d\n",name, dataset_number); */

    err = _escdf_group_dataset_new(group, idata);

    /* printf("escdf_group_dataset_open: name = %s, new() resulted in %d\n",name, err); */
    
    FULFILL_OR_RETURN_VAL(err == ESCDF_SUCCESS,  ESCDF_ERROR, NULL);

    dataset = group->datasets[idata];


    /* id = escdf_dataset_get_id(dataset); */
    /* dataset_name = escdf_dataset_get_name(dataset); */


    /* printf("escdf_group_dataset_open: name = %s, dataset->specs->id = %d, %s\n", name, id, dataset_name); */

    /* open dataset in the file */

    if( escdf_dataset_open(dataset, group->loc_id) != ESCDF_SUCCESS) {
        /* printf("escdf_group_dataset_open: name = %s, failed to open dataset\n", name); */
    
        escdf_dataset_free(dataset);
        FULFILL_OR_RETURN_VAL(false, ESCDF_ERROR, NULL);    
    }

    group->datasets_present[idata] = true;

    return dataset;   
}

escdf_errno_t escdf_group_dataset_close(escdf_group_t *group, escdf_dataset_id_t dataset_id)
{
    unsigned int idata, i;
    escdf_errno_t err;
    bool found;

    assert(group != NULL);


    for (found = false, i = 0; i < group->specs->ndatasets; i++) {
        if ( group->specs->data_specs[i]->id == dataset_id) {idata = i; found=true;}
    }
    FULFILL_OR_RETURN(found == true, ESCDF_ERROR);


    /* name_check = escdf_dataset_get_name(group->datasets[dataset_number]); */

    /* printf("escdf_group_dataset_close: attempting to close dataset %d: %s %s\n", dataset_number, name, name_check); */

    /* if(dataset_number == ESCDF_UNDEFINED_ID)  return ESCDF_SUCCESS; */ /* QUESTION: Shall wi throw an error ? */

    err = escdf_dataset_close(group->datasets[idata]);

    /* printf("escdf_group_dataset_close: after closing dataset %d: %s %d\n", dataset_number, name, err); */


    FULFILL_OR_RETURN( err == ESCDF_SUCCESS, ESCDF_ERROR);

    escdf_dataset_free(group->datasets[idata]);

    group->datasets[idata] = NULL;

    /* printf("escdf_group_dataset_close: after freeing dataset %d: %s %d\n", dataset_number, name, err); */

    return ESCDF_SUCCESS;

}


escdf_errno_t escdf_group_dataset_write_at(const escdf_dataset_t *data, 
                                            const size_t *start, const size_t *count, const size_t *stride, void* buf)
{
    escdf_errno_t err;
    assert(data != NULL);

    

    err = escdf_dataset_write(data, start, count, stride, buf);
    return err;
    


}



escdf_errno_t escdf_group_dataset_read_at(const escdf_dataset_t *data, 
                                            const size_t *start, const size_t *count, const size_t *stride, void *buf)
{
    escdf_errno_t err;
    assert(data != NULL);

    

    err = escdf_dataset_read(data, start, count, stride, buf);
    return err;
    

}

void escdf_group_print_info(const escdf_group_t* group) 
{

    assert(group!=NULL);
    assert(group->specs!=NULL);
    assert(group->specs->name!=NULL);
    assert(group->escdf_handle!=NULL);


    printf("Group Information for group %s:\n\n", group->specs->name);
    printf(" Information from the specifications:\n\n");
    printf("  group ID   = %d\n", group->specs->group_id);
    printf("  group name = %s\n", group->specs->name);
    printf("  Number of attributes = %d\n", group->specs->nattributes);
    printf("  Number of datasets   = %d\n", group->specs->ndatasets);
    printf("\n");
    printf(" Information from the group:\n\n");
    printf("  Group loc_id = %ld \n", group->loc_id);
    printf("  root name = %s\n", group->root);
    printf("  ESCDF handle:\n");
    printf("    file_id  = %ld \n", group->escdf_handle->file_id);
    printf("    group_id = %ld \n", group->escdf_handle->group_id);
    printf("    transfer_mode = %ld \n", group->escdf_handle->transfer_mode);
    printf("\n");
    printf(" Registered attributes: \n");
    for(int i=0; i<group->specs->nattributes; i++) {
        printf("     %s\n", group->specs->attr_specs[i]->name);
    }
    printf("\n");
    printf(" Registered datasets: \n");
    for(int i=0; i<group->specs->ndatasets; i++) {
        printf("     %s\n", group->specs->data_specs[i]->name);
    }
    printf("\n\n");

};


escdf_dataset_t * _escdf_group_get_dataset(const escdf_group_t *group, escdf_dataset_id_t dataset_id)
{
    escdf_dataset_t *data = NULL;
    assert(group!=NULL);

    int index = _escdf_group_get_dataset_index(group, dataset_id);
    if(index>=0) data = group->datasets[index];

    return data;
}
