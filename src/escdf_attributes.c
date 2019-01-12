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

#include <assert.h>

#include "escdf_attributes.h"
#include "utils_hdf5.h"

struct escdf_attribute {
    const escdf_attribute_specs_t *specs;
    bool is_set;
    size_t *dims;
    void *buf;
};


size_t escdf_attribute_specs_sizeof(const escdf_attribute_specs_t *specs)
{
    assert(specs != NULL);

    switch (specs->datatype) {
    case ESCDF_DT_BOOL:
        return sizeof(bool);
    case ESCDF_DT_UINT:
        return sizeof(unsigned int);
    case ESCDF_DT_INT:
        return sizeof(int);
    case ESCDF_DT_DOUBLE:
        return sizeof(double);
    case ESCDF_DT_STRING:
        return specs->stringlength * sizeof(char);
    default:
        return 0;
    }
}

hid_t escdf_attribute_specs_hdf5_disk_type(const escdf_attribute_specs_t *specs)
{
    assert(specs != NULL);

    switch (specs->datatype) {
    case ESCDF_DT_BOOL:
        return H5T_C_S1;
    case ESCDF_DT_UINT:
        return H5T_NATIVE_UINT;
    case ESCDF_DT_INT:
        return H5T_NATIVE_INT;
    case ESCDF_DT_DOUBLE:
        return H5T_NATIVE_DOUBLE;
    case ESCDF_DT_STRING:
        return H5T_C_S1;
    default:
        return 0;
    }
}

hid_t escdf_attribute_specs_hdf5_mem_type(const escdf_attribute_specs_t *specs)
{
    assert(specs != NULL);

    switch (specs->datatype) {
    case ESCDF_DT_BOOL:
        return H5T_C_S1;
    case ESCDF_DT_UINT:
        return H5T_NATIVE_UINT;
    case ESCDF_DT_INT:
        return H5T_NATIVE_INT;
    case ESCDF_DT_DOUBLE:
        return H5T_NATIVE_DOUBLE;
    case ESCDF_DT_STRING:
        return H5T_C_S1;
    default:
        return 0;
    }
}

bool escdf_attribute_specs_is_present(const escdf_attribute_specs_t *specs, hid_t loc_id)
{
    assert(specs != NULL);

    return utils_hdf5_check_present_attr(loc_id, specs->name);
}

int escdf_attribute_specs_get_id(const escdf_attribute_specs_t *specs)
{
    assert( specs != NULL );

    return specs->id;
}


escdf_attribute_t * escdf_attribute_new(const escdf_attribute_specs_t *specs, escdf_attribute_t **attr_dims)
{
    escdf_attribute_t *attr = NULL;
    unsigned int ii;
    size_t *dim;
    size_t len;

    /* Check input */
    assert(specs != NULL);
    if (specs->ndims > 0) {
        assert(attr_dims != NULL);
        for (ii = 0; ii < specs->ndims; ii++) {
            assert(attr_dims[ii]->is_set);
            assert(attr_dims[ii]->specs->datatype == ESCDF_DT_UINT);
            assert(attr_dims[ii]->specs->id == specs->dims_specs[ii]->id);
        }
    }

    /* Allocate memory and set default values */
    attr = (escdf_attribute_t *) malloc(sizeof(escdf_attribute_t));
    if (attr == NULL)
        return attr;
    attr->specs = specs;
    attr->is_set = false;
    attr->buf = NULL;
    attr->dims = NULL;

    /* Get the size of the attribute and allocate buffer memory */
    if (attr->specs->ndims > 0) {
        attr->dims = (size_t *) malloc(attr->specs->ndims * sizeof(size_t));
        if (attr->dims == NULL) {
            free(attr);
            return NULL;
        }
    }
    len = 1;
    for (ii = 0; ii < attr->specs->ndims; ii++) {
        dim = (size_t *) (attr_dims[ii]->buf);
        len *= *dim;
        attr->dims[ii] = *dim;
    }
    attr->buf = malloc(len * escdf_attribute_specs_sizeof(attr->specs));
    if (attr->buf == NULL) {
        free(attr->dims);
        free(attr);
        attr = NULL;
    }

    return attr;
}

void escdf_attribute_free(escdf_attribute_t *attr)
{
    if (attr != NULL) {
        free(attr->dims);
        free(attr->buf);
    }
    free(attr);
}


escdf_errno_t escdf_attribute_set(escdf_attribute_t *attr, const void *buf)
{
    assert(attr != NULL);

    memcpy(attr->buf, buf, escdf_attribute_sizeof(attr));
    attr->is_set = true;

    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_attribute_get(const escdf_attribute_t *attr, void *buf)
{
    assert(buf != NULL);
    assert(attr != NULL);
    assert(attr->is_set);

    memcpy(buf, attr->buf, escdf_attribute_sizeof(attr));

    return ESCDF_SUCCESS;
}


escdf_errno_t escdf_attribute_read(escdf_attribute_t *attr, hid_t loc_id)
{
    escdf_errno_t err;

    assert(attr != NULL);
    assert(escdf_attribute_specs_is_present(attr->specs, loc_id));

    switch (attr->specs->datatype) {
    case ESCDF_DT_BOOL:
        err = utils_hdf5_read_attr_bool(loc_id, attr->specs->name, attr->dims, attr->specs->ndims, attr->buf);
        break;
    case ESCDF_DT_STRING:
        err = utils_hdf5_read_attr_string(loc_id, attr->specs->name, attr->specs->stringlength, attr->dims,
                                          attr->specs->ndims, attr->buf);
        break;
    default:
        err = utils_hdf5_read_attr(loc_id, attr->specs->name, escdf_attribute_specs_hdf5_mem_type(attr->specs), attr->dims,
                                       attr->specs->ndims, attr->buf);
    }
    attr->is_set = err == ESCDF_SUCCESS;

    return err;
}


escdf_errno_t escdf_attribute_write(escdf_attribute_t *attr, hid_t loc_id)
{
    escdf_errno_t err;

    assert(attr != NULL);
    assert(attr->is_set);

    switch (attr->specs->datatype) {
    case ESCDF_DT_BOOL:
        err = utils_hdf5_write_attr_bool(loc_id, attr->specs->name, attr->dims, attr->specs->ndims, attr->buf);
        break;
    case ESCDF_DT_STRING:
        err = utils_hdf5_write_attr_string(loc_id, attr->specs->name, attr->specs->stringlength, attr->dims,
                                           attr->specs->ndims, attr->buf);
        break;
    default:
              err = utils_hdf5_write_attr(loc_id, attr->specs->name, escdf_attribute_specs_hdf5_disk_type(attr->specs),
                                          attr->dims, attr->specs->ndims, escdf_attribute_specs_hdf5_mem_type(attr->specs),
                                          attr->buf);
    }

    return err;
}

size_t escdf_attribute_sizeof(const escdf_attribute_t *attr)
{
    unsigned int ii;
    size_t len;

    assert(attr != NULL);

    len = 1;
    for (ii = 0; ii < attr->specs->ndims; ii++)
        len *= attr->dims[ii];

    return len * escdf_attribute_specs_sizeof(attr->specs);
}

bool escdf_attribute_is_set(const escdf_attribute_t *attr)
{
  assert( attr != NULL );

  return attr->is_set;
}

const size_t * escdf_attribute_get_dimensions(const escdf_attribute_t *attr) {

    assert( attr != NULL );

    return attr->dims;
}

int escdf_attribute_get_specs_id(const escdf_attribute_t *attr)
{
    assert( attr != NULL );

    return attr->specs->id;
}

escdf_errno_t escdf_attribute_print(escdf_attribute_t *attr)
{
    unsigned int i, j;
    char datatype_name[20], isSet[6];

    if( attr == NULL ) {

        printf("Attribute not defined! \n"); fflush(stdout);
        return ESCDF_ERROR;

    } else {

        if( attr->specs == NULL ) {

        printf("Attribute Specs not defined! \n"); fflush(stdout);
        return ESCDF_ERROR;
      
        } else {
            switch(attr->specs->datatype) {
            case ESCDF_DT_NONE:
                strcpy(datatype_name, "ESCDF_DT_NONE");
                break;
            case ESCDF_DT_UINT:
                strcpy(datatype_name, "ESCDF_DT_UINT");
                break;
            case ESCDF_DT_INT:
                strcpy(datatype_name, "ESCDF_DT_INT");
                break;
            case ESCDF_DT_BOOL:
                strcpy(datatype_name, "ESCDF_DT_BOOL");
                break;
            case ESCDF_DT_DOUBLE:
                strcpy(datatype_name, "ESCDF_DT_DOUBLE");
                break;
            case ESCDF_DT_STRING:
                strcpy(datatype_name, "ESCDF_DT_STRING");
                break;
            default :
                strcpy(datatype_name, "UNDEFINED");
                break;
            }

            if (attr->is_set)
                strcpy(isSet,"True");
            else
                strcpy(isSet,"False");

      
            printf("Attribute dump for: %s with ID = %i \n",attr->specs->name, attr->specs->id);
            printf("  Is set %s \n", isSet);
            printf("  Data type = %s (%i) \n", datatype_name, attr->specs->datatype );
            printf("  Number of Dimensions = %i \n", attr->specs->ndims );
            for(i = 0; i < attr->specs->ndims; i++) {
	            printf("  Dimensions (%i) = %i \n", i, (int) attr->dims[i] );
            }

            printf("  Data dump: \n"); fflush(stdout);
      
            assert( attr->buf != NULL );

            switch(attr->specs->datatype) {
            case ESCDF_DT_NONE:
                printf("Datetype NONE. No data.\n");
                break;
            case ESCDF_DT_UINT   :
                switch(attr->specs->ndims) {
                case 0: printf("Value = %i \n", *( (unsigned int*) attr->buf) ); break;
	            case 1:
                    for(i=0; i<attr->dims[0]; i++)
                        printf("%i ", ((unsigned int*) attr->buf)[i] );
                    printf("\n");
                    break;
                case 2:
                    for(i=0; i<attr->dims[0]; i++) {
                        for(j=0; j<attr->dims[1]; j++) {
                            printf("%i ", ((unsigned int*) attr->buf)[i+j*attr->dims[0] ] ); fflush(stdout);
                        }
                        printf("\n");
                    }
                    printf("\n");
                    break;
                }
                break;
            case ESCDF_DT_INT    :
                switch(attr->specs->ndims) {
                case 0:
                    printf("Value = %i \n", *((int*) attr->buf));
                    break;
                case 1:
                    for(i=0; i<attr->dims[0]; i++)
                        printf("%i ", ((int*) attr->buf)[i] );
                    printf("\n");
                    break;
                case 2:
                    for(i=0; i<attr->dims[0]; i++) {
                        for(j=0; j<attr->dims[1]; j++) {
                            printf("%i ", ((int*) attr->buf)[i+j*attr->dims[0] ] ); fflush(stdout);
                        }
                        printf("\n");
                    }
                    printf("\n");
                    break;
                };
                break;

            case ESCDF_DT_BOOL:
                break;
 
            case ESCDF_DT_DOUBLE:
	            switch(attr->specs->ndims) {
                case 0:
                    printf("Value = %f \n", *((double*) attr->buf));
                    break;
	            case 1:
                    for(i=0; i<attr->dims[0]; i++)
                        printf("%f ", ((double*) attr->buf)[i] );
                    printf("\n");
	                break;
                case 2:
	                for(i = 0; i < attr->dims[0]; i++) {
	                    for(j = 0; j < attr->dims[1]; j++) {
	                        printf("%f ", ((double*) attr->buf)[i+j*attr->dims[0] ] ); fflush(stdout);
                        }
	                    printf("\n");
                    }
                    printf("\n");
	                break;
                }
	            break;

            case ESCDF_DT_STRING:
                switch(attr->specs->ndims) {
                case 0: printf("Value = %s \n", ((char*) attr->buf)); break;
                case 1:
                    for(i=0; i<attr->dims[0]; i++)
                        printf("%s ", &((char*) attr->buf)[i] );
                    printf("\n");
                    break;
                case 2:
                    for(i=0; i<attr->dims[0]; i++) {
                        for(j=0; j<attr->dims[1]; j++) {
                            printf("%s ", &((char*) attr->buf)[i+j*attr->dims[0] ] ); fflush(stdout);
                        }
                        printf("\n");
                    }
                    printf("\n");
                    break;
                };
                break;

                default : break;
            }

            /* Get dimentions */

            printf("\n");
            fflush(stdout);
        }
  }
  return ESCDF_SUCCESS;

}
