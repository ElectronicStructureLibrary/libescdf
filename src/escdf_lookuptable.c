/* Copyright (C) 2016-2017 Damien Caliste <dcaliste@free.fr>
 *                         Micael Oliveira <micael.oliveira@mpsd.mpg.de>
 *                         Yann Pouillon <devops@materialsevolution.es>
 *                         Martin Lueders <martin.lueders@stfc.ac.uk>
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

#define LOOKUPTABLE_GROW_SIZE 1024

#include "escdf_lookuptable.h"
#include <assert.h>

struct escdf_lookuptable {

    hsize_t size;
    hsize_t num_elements;

    hid_t *IDs;
    void  **pointers;
};

escdf_lookuptable_t *escdf_lookuptable_new()
{
    return (escdf_lookuptable_t*) malloc(sizeof(escdf_lookuptable_t));

}



escdf_errno_t escdf_lookuptable_init(escdf_lookuptable_t *this)
{
    this->size = LOOKUPTABLE_GROW_SIZE;

    this->IDs = malloc(this->size * sizeof(hid_t));
    assert(this->IDs != NULL);

    this->pointers = malloc(this->size * sizeof(void*));
    assert(this->pointers != NULL);

    this->num_elements = 0;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_lookuptable_grow(escdf_lookuptable_t *this)
{
    hid_t *new_IDs;
    void **new_pointers;
    int ii, old_size;
    
    assert(this->IDs != NULL);
    assert(this->pointers != NULL);

    old_size = this->size;

    this->size = this->size + LOOKUPTABLE_GROW_SIZE;

    new_IDs = malloc(this->size * sizeof(hid_t));
    assert(this->IDs != NULL);

    new_pointers = malloc(this->size * sizeof(void*));
    assert(this->pointers != NULL);

    for(ii = 0; ii<old_size; ii++) {
        new_IDs[ii] = this->IDs[ii];
        new_pointers[ii] = this->pointers[ii];
    }

    free(this->IDs);
    free(this->pointers);

    this->IDs = new_IDs;
    this->pointers = new_pointers;

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_lookuptable_shrink(escdf_lookuptable_t *this)
{
    hid_t *new_IDs;
    void **new_pointers;
    int ii, old_size, new_size;
    
    assert(this->IDs != NULL);
    assert(this->pointers != NULL);

    old_size = this->size;

    new_size = this->size - LOOKUPTABLE_GROW_SIZE;

    if (new_size < 1) return ESCDF_SUCCESS;

    new_IDs = malloc(this->size * sizeof(hid_t));
    assert(this->IDs != NULL);

    new_pointers = malloc(this->size * sizeof(void*));
    assert(this->pointers != NULL);

    for(ii = 0; ii<old_size; ii++) {
        new_IDs[ii] = this->IDs[ii];
        new_pointers[ii] = this->pointers[ii];
    }

    free(this->IDs);
    free(this->pointers);

    this->IDs = new_IDs;
    this->pointers = new_pointers;

    return ESCDF_SUCCESS;

}

escdf_errno_t escdf_lookuptable_delete(escdf_lookuptable_t *this)
{      
    assert(this->IDs != NULL);
    assert(this->pointers != NULL);

    free(this->IDs);
    free(this->pointers);

    return ESCDF_SUCCESS;
}

escdf_errno_t escdf_lookuptable_add(escdf_lookuptable_t *this, hid_t ID, void* ptr)
{
    assert(this->IDs != NULL);
    assert(this->pointers != NULL);

    if(this->num_elements + 1 > this->size) escdf_lookuptable_grow(this);

    this->IDs[this->num_elements] = ID;
    this->pointers[this->num_elements] = ptr;

    this->num_elements += 1;

    return ESCDF_SUCCESS;
}

bool escdf_lookuptable_check_exist(escdf_lookuptable_t *this, hid_t ID)
{
    int ii;
    bool result;

    assert(this->IDs != NULL);
    assert(this->pointers != NULL);

    result = false;

    for(ii = 0; ii < this->num_elements && ID != this->IDs[ii]; ii++);

    if(ii<this->num_elements) result = true;

    return result;       
}

void* escdf_lookuptable_get_pointer(escdf_lookuptable_t *this, hid_t ID)
{
    int ii;
    void* result;

    assert(this->IDs != NULL);
    assert(this->pointers != NULL);

    result = NULL;

    for(ii = 0; ii < this->num_elements && ID != this->IDs[ii]; ii++);

    if(ii<this->num_elements) result = this->pointers[ii];

    return result;       
}

hid_t escdf_lookuptable_get_id(escdf_lookuptable_t *this, void* ptr)
{
    int ii;
    hid_t result;

    assert(this->IDs != NULL);
    assert(this->pointers != NULL);

    result = ESCDF_UNDEFINED_ID;

    for(ii = 0; ii < this->num_elements && ptr != this->pointers[ii]; ii++);

    if(ii<this->num_elements) result = this->IDs[ii];

    return result;       
}


