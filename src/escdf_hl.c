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



#ifdef __cplusplus
extern "C" {
#endif


#include "escdf_hl.h"

#include "escdf_private_group.h"
#include "utils_hdf5.h"


/************************************************************
 * High level routines for accessing attributes in a group  *
 ************************************************************/


escdf_errno_t escdf_hl_attribute_write(escdf_group_t* group, escdf_attribute_id_t attribute_ID, const void* buf)
{
    int i;
    bool attr_found = false;
    escdf_errno_t error;
  
    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(group->specs != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(buf != NULL, ESCDF_EVALUE);

    
    FULFILL_OR_RETURN(group->specs->attr_specs[attribute_ID] != NULL, ESCDF_EVALUE);

    if (group->attr[attribute_ID] == NULL) {
        SUCCEED_OR_RETURN(_escdf_group_attribute_new(group, attribute_ID));
    }

    SUCCEED_OR_RETURN(escdf_attribute_set(group->attr[attribute_ID], buf));
  
    /* If the attribute has successfully been set in memory, we can write to disk */

    SUCCEED_OR_RETURN(escdf_attribute_write(group->attr[attribute_ID], group->loc_id));
  
    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_hl_attribute_read(escdf_group_t* group, escdf_attribute_id_t attribute_ID, void* buf)
{
    int i;
    bool attr_found;
    escdf_errno_t error;

    FULFILL_OR_RETURN(group != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(group->specs != NULL, ESCDF_EVALUE);
    FULFILL_OR_RETURN(buf != NULL, ESCDF_EVALUE);


    FULFILL_OR_RETURN(group->specs->attr_specs[attribute_ID] != NULL, ESCDF_EVALUE);

    if (group->attr[attribute_ID] == NULL) {
        SUCCEED_OR_RETURN(_escdf_group_attribute_new(group, attribute_ID));
    }

    /* check whether the attribute already as a value in memory */

    if (!escdf_attribute_is_set(group->attr[attribute_ID])) {
        SUCCEED_OR_RETURN(escdf_attribute_read(group->attr[attribute_ID], group->loc_id));
    }

    SUCCEED_OR_RETURN(escdf_attribute_get(group->attr[attribute_ID], buf));
  
    return ESCDF_SUCCESS;
  
}



/************************************************************
 * High level routines for accessing datasets in a group    *
 ************************************************************/


escdf_dataset_t *escdf_hl_dataset_create(escdf_group_t *group, escdf_dataset_id_t dataset_ID)
{
    escdf_dataset_t *dataset;
    escdf_errno_t err;

    const char * dataset_name;

    FULFILL_OR_RETURN_VAL(group!=NULL, ESCDF_ERROR, NULL);
   
    if(dataset_ID == ESCDF_UNDEFINED_ID)  return NULL;
    

    /* create dataset structure */

    err = _escdf_group_dataset_new(group, dataset_ID); 
    FULFILL_OR_RETURN_VAL( err == ESCDF_SUCCESS, ESCDF_ERROR, NULL);

#ifdef DEBUG
    printf("_escdf_group_dataset_new resulted %d\n",err); fflush(stdout); 
#endif

    dataset = group->datasets[dataset_ID];

    FULFILL_OR_RETURN_VAL(escdf_dataset_get_id(dataset)==dataset_ID, ESCDF_ERROR, NULL);


    err = escdf_dataset_create(dataset, group->loc_id); 

#ifdef DEBUG
    printf("escdf_dataset_create resulted %d\n",err); fflush(stdout); 
#endif

    if( err != ESCDF_SUCCESS ) {
        escdf_dataset_close(dataset);
        FULFILL_OR_RETURN_VAL(false, ESCDF_ERROR, NULL);
    }

    return dataset;   
}

escdf_dataset_t *escdf_hl_dataset_open(escdf_group_t *group, escdf_dataset_id_t dataset_ID)
{
    escdf_dataset_t *dataset;
    escdf_errno_t err;

    const char * dataset_name;


    FULFILL_OR_RETURN_VAL(group!=NULL, ESCDF_ERROR, NULL);


    if(dataset_ID == ESCDF_UNDEFINED_ID)  return NULL;

    /* printf("escdf_group_dataset_open: name = %s, dataset_number = %d\n",name, dataset_number); */

    err = _escdf_group_dataset_new(group, dataset_ID);

    /* printf("escdf_group_dataset_open: name = %s, new() resulted in %d\n",name, err); */
    
    FULFILL_OR_RETURN_VAL(err == ESCDF_SUCCESS,  ESCDF_ERROR, NULL);

    dataset = group->datasets[dataset_ID];

    FULFILL_OR_RETURN_VAL(escdf_dataset_get_id(dataset)==dataset_ID, ESCDF_ERROR, NULL);

#ifdef DEBUG
    dataset_name = escdf_dataset_get_name(dataset);
    printf("escdf_group_dataset_open: name = %s, dataset->specs->id = %d \n", dataset_name,dataset_ID); 
#endif

    /* open dataset in the file */

    if( escdf_dataset_open(dataset, group->loc_id) != ESCDF_SUCCESS) {
        /* printf("escdf_group_dataset_open: name = %s, failed to open dataset\n", name); */
    
        escdf_dataset_free(dataset);
        FULFILL_OR_RETURN_VAL(false, ESCDF_ERROR, NULL);    
    }

    group->datasets_present[dataset_ID] = true;

    return dataset;   
}

 
escdf_errno_t escdf_hl_dataset_close(escdf_group_t *group, escdf_dataset_id_t dataset_ID)
{
    escdf_errno_t err;

    const char * name_check;

    FULFILL_OR_RETURN(group!=NULL, ESCDF_ERROR);

    name_check = escdf_dataset_get_name(group->datasets[dataset_ID]);

    /* printf("escdf_group_dataset_close: attempting to close dataset %d: %s %s\n", dataset_number, name, name_check); */

    /* if(dataset_number == ESCDF_UNDEFINED_ID)  return ESCDF_SUCCESS; */ /* QUESTION: Shall wi throw an error ? */

    err = escdf_dataset_close(group->datasets[dataset_ID]);

    /* printf("escdf_group_dataset_close: after closing dataset %d: %s %d\n", dataset_number, name, err); */


    FULFILL_OR_RETURN( err == ESCDF_SUCCESS, ESCDF_ERROR);

    escdf_dataset_free(group->datasets[dataset_ID]);

    group->datasets[dataset_ID] = NULL;

    /* printf("escdf_group_dataset_close: after freeing dataset %d: %s %d\n", dataset_number, name, err); */

    return ESCDF_SUCCESS;

}




escdf_errno_t escdf_hl_dataset_write_simple(escdf_group_t *group, escdf_dataset_id_t dataset_ID, const void* buf)
{
    escdf_dataset_t *data;

    if( utils_hdf5_check_present(group->loc_id, group->specs->data_specs[dataset_ID]->name) ) {
        return ESCDF_ERROR;
    }
    else {
        data = escdf_hl_dataset_create(group, dataset_ID);
    }


    FULFILL_OR_RETURN(data != NULL, ESCDF_ERROR);

#ifdef DEBUG
    printf("escdf_group_dataset_write_simple: %s \n", escdf_dataset_get_name(data)); fflush(stdout); 
#endif

    FULFILL_OR_RETURN( escdf_dataset_write_simple(data, buf) == ESCDF_SUCCESS, ESCDF_ERROR );
    FULFILL_OR_RETURN( escdf_dataset_close(data) == ESCDF_SUCCESS, ESCDF_ERROR );

    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_hl_dataset_read_simple(escdf_group_t *group, escdf_dataset_id_t dataset_ID, void *buf)
{
    const escdf_dataset_t *data;

    data = escdf_hl_dataset_open(group, dataset_ID);

    FULFILL_OR_RETURN(data != NULL, ESCDF_ERROR);
    FULFILL_OR_RETURN(buf != NULL, ESCDF_ERROR);

    FULFILL_OR_RETURN( escdf_dataset_read_simple(data, buf) == ESCDF_SUCCESS, ESCDF_ERROR);

    return ESCDF_SUCCESS;

}


escdf_errno_t escdf_hl_dataset_write_at(escdf_group_t *group, escdf_dataset_id_t dataset_ID, 
                                        unsigned int *start, unsigned int *count, unsigned int * stride, void* buf);


escdf_errno_t escdf_hl_dataset_read_at(escdf_group_t *group, escdf_dataset_id_t dataset_ID, 
                                            unsigned int *start, unsigned int *count, unsigned int * stride, void *buf);


#ifdef __cplusplus
}
#endif

