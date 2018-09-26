/* Copyright (C) 2016-2017 Micael Oliveira <micael.oliveira@mpsd.mpg.de>
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

#include <check.h>
#include <zconf.h>
#include <assert.h>

#include "escdf_group.h"

#include "escdf_attributes_ID.h"
#include "escdf_attributes_specs.h"
#include "escdf_groups_ID.h"
#include "escdf_groups_specs.h"


#define FILE_EMPTY "check_group_test_file_empty.h5"
#define FILE_ROOT  "check_group_test_file_root.h5"
#define FILE_PATH  "check_group_test_file_path.h5"
#define GROUP_NAME "fancy_group"

#define DUMMY_INT 1

#define GROUP_TEST 1


const escdf_attribute_specs_t attribute_dummy_int = {
  DUMMY_INT, "dummy_integer", ESCDF_DT_INT, 0, 0, NULL
};

const escdf_attribute_specs_t *group_attributes[] = {
    &attribute_dummy_int
};

const escdf_group_specs_t group_specs = {
    GROUP_TEST, "test", 1, group_attributes
};

static escdf_handle_t *escdf_handle;
static escdf_handle_t *handle_e = NULL, *handle_r = NULL, *handle_p = NULL;
static escdf_group_t *group = NULL;
static escdf_group_t *group_system = NULL;
static escdf_group_t *group_density = NULL;

    



static double alat[3][3] = {{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}};
static char*  name="system_name";

/******************************************************************************
 * Setup and teardown                                                         *
 ******************************************************************************/

void group_file_setup(const char *file, const char *path)
{
    hid_t file_id, file_root_id, group_root_id, group_path_id;


    /* create file */
    file_id = H5Fcreate(file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    file_root_id = H5Gopen(file_id, ".", H5P_DEFAULT);
    group_root_id = H5Gcreate(file_root_id, group_specs.root, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    if (path == NULL) {
        group_path_id = H5Gopen(group_root_id, ".", H5P_DEFAULT);
    } else {
        group_path_id = H5Gcreate(group_root_id, path, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }



    H5Gclose(group_path_id);
    H5Gclose(group_root_id);
    H5Gclose(file_root_id);
    H5Fclose(file_id);
}

void group_handle_setup(void)
{
    group_file_setup(FILE_ROOT, NULL);
    group_file_setup(FILE_PATH, GROUP_NAME);
    handle_r = escdf_open(FILE_ROOT, NULL);
    handle_p = escdf_open(FILE_PATH, NULL);
    handle_e = escdf_create(FILE_EMPTY, NULL);
    escdf_group_specs_register(&group_specs);
}

void group_handle_teardown(void)
{
    escdf_close(handle_r);
    escdf_close(handle_p);
    escdf_close(handle_e);
    unlink(FILE_ROOT);
    unlink(FILE_PATH);
    unlink(FILE_EMPTY);
}

void group_setup(void)
{
    group_handle_setup();
    escdf_group_free(group);
    group = escdf_group_new(group_specs.group_id);
}

void group_teardown(void)
{
    escdf_group_free(group);
    group = NULL;
    group_handle_teardown();
}

void setters_setup(void)
{
    group_file_setup(FILE_ROOT, NULL);
    group_file_setup(FILE_PATH, GROUP_NAME);

    handle_r = escdf_open(FILE_ROOT, NULL);
    handle_p = escdf_open(FILE_PATH, NULL);
    handle_e = escdf_create(FILE_EMPTY, NULL);

    ck_assert( handle_r != NULL );

    escdf_register_all_group_specs();

    ck_assert( handle_r != NULL );

    /*
    escdf_group_free(group_system);
    escdf_group_free(group_density);
    */



    /* obsolete API
    group_system  = escdf_group_create(system_specs.group_id, handle_r, "system");
    group_density = escdf_group_create(density_specs.group_id, handle_r, "density");
    */

    group_system  = escdf_group_create(handle_r, "system", NULL);
    group_density = escdf_group_create(handle_r, "density", NULL);

    ck_assert( group_system != NULL );
    ck_assert( group_density != NULL );
}

void setters_teardown(void)
{
    escdf_group_free(group_system);
    escdf_group_free(group_density);
    group = NULL;

    escdf_close(handle_r);
    escdf_close(handle_p);
    escdf_close(handle_e);
    //unlink(FILE_ROOT);
    //unlink(FILE_PATH);
    //unlink(FILE_EMPTY);
}

void dataset_setup(void) 
{

    escdf_handle = escdf_create("test.h5", NULL);
    ck_assert(escdf_handle != NULL);

    escdf_register_all_group_specs();


    group_system = escdf_group_create(escdf_handle, "system", NULL);
    ck_assert(group_system != NULL);



}

void dataset_teardown(void)
{

    ck_assert( escdf_group_close(group_system) == ESCDF_SUCCESS );

    ck_assert( escdf_close(escdf_handle) == ESCDF_SUCCESS );
}
/******************************************************************************
 * Tests                                                                      *
 ******************************************************************************/

START_TEST(test_group_new)
{
    ck_assert( (group = escdf_group_new(GROUP_TEST)) != NULL);
}
END_TEST

START_TEST(test_group_open_location)
{
    ck_assert(escdf_group_open_location(group, handle_r, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_group_open_location_name)
{
    ck_assert(escdf_group_open_location(group, handle_p, GROUP_NAME) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_group_create_location)
{
    ck_assert(escdf_group_create_location(group, handle_e, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_group_create_location_name)
{
    ck_assert(escdf_group_create_location(group, handle_e, GROUP_NAME) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_group_close_location)
{
    escdf_group_open_location(group, handle_r, NULL);
    ck_assert(escdf_group_close_location(group) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_group_open)
{
    /* currently incompatible with the implementation!!
    ck_assert( (group = escdf_group_open(GROUP_TEST, handle_r, NULL)) != NULL);
    */
}
END_TEST

START_TEST(test_group_open_name)
{
    /* currently incompatible with the implementation!!
    ck_assert( (group = escdf_group_open(GROUP_TEST, handle_p, GROUP_NAME)) != NULL);
    */
}
END_TEST

START_TEST(test_group_create)
{
    /* currently incompatible with the implementation!!
    ck_assert( (group = escdf_group_create(GROUP_TEST, handle_e, NULL)) != NULL);
    */
}
END_TEST

START_TEST(test_group_create_name)
{
    /* currently incompatible with the implementation!!
    ck_assert( (group = escdf_group_create(GROUP_TEST, handle_e, GROUP_NAME)) != NULL);
    */
}
END_TEST

START_TEST(test_group_close)
{
    ck_assert( escdf_group_close(group) == ESCDF_SUCCESS);
    group = NULL;
}
END_TEST


START_TEST(test_group_attribute_system)
{
  escdf_errno_t error;
  char  name[80];
  char  value_string[80];

  unsigned int dim = 3, i, j;
  double values[3][3] = {{99.0, 99.0, 99.0},{99.0, 99.0, 99.0},{99.0, 99.0, 99.0}};

  strcpy(name, "test-string");

  printf("************************************************************\n");
  printf("start set test:  \n"); fflush(stdout);

  ck_assert( group_system != NULL );
  ck_assert( group_density != NULL );

  escdf_group_print_info(group_system);

  ck_assert( escdf_group_attribute_set(group_system, "number_of_physical_dimensions", (void*) &dim) == ESCDF_SUCCESS);
  dim = 0;
  ck_assert( escdf_group_attribute_get(group_system, "number_of_physical_dimensions", (void*) &dim) == ESCDF_SUCCESS);
  ck_assert( dim == 3 );

  ck_assert( escdf_group_attribute_set(group_system, "lattice_vectors", (void*) alat) == ESCDF_SUCCESS );
  ck_assert( escdf_group_attribute_get(group_system, "lattice_vectors", (void*) values) == ESCDF_SUCCESS ); 

  for(i = 0; i<dim; i++) {
    for (j = 0; j<dim; j++) {
      ck_assert( alat[i][j] == values[i][j] );
    }
  }

  ck_assert( escdf_group_attribute_set(group_system, "system_name", (void*) name) == ESCDF_SUCCESS);
  ck_assert( escdf_group_attribute_get(group_system, "system_name", (void*) value_string) == ESCDF_SUCCESS);
  ck_assert( strcmp( name, value_string ) == 0 );

  strcpy(value_string, "    ");


}
END_TEST 


START_TEST(test_group_datasets_system)
{
    escdf_errno_t error;
    char  name[80];
    char  value_string[80];

    unsigned int dim = 3, i, j, num_species = 5;
    unsigned int num;

    double values[3][3] = {{99.0, 99.0, 99.0},{99.0, 99.0, 99.0},{99.0, 99.0, 99.0}};

    char names[5][80];
    char read_names[5][80];

    strcpy(names[0], "Copper");
    strcpy(names[1], "Oxygen");
    strcpy(names[2], "Oxygen 2");
    strcpy(names[3], "Nickel");
    strcpy(names[4], "Empty");



    strcpy(name, "test-string");

    printf("************************************************************\n");
    printf("************************************************************\n");

    printf("start dataset test:  \n"); fflush(stdout);

    if( group_system == NULL ) printf("group_system is invalid!! \n"); fflush(stdout);
    ck_assert( group_system != NULL );

    printf("going to print group info:\n"); fflush(stdout);

    escdf_group_print_info(group_system);

    ck_assert( escdf_group_attribute_set(group_system, "number_of_species", (void*) &num_species) == ESCDF_SUCCESS);
    num = 0;
    ck_assert( escdf_group_attribute_get(group_system, "number_of_species", (void*) &num) == ESCDF_SUCCESS);
    ck_assert( num == num_species );


    escdf_dataset_t* dataset_species_names = escdf_group_dataset_create(group_system, "species_names");
    printf("finished creating dataset species_names.\n"); fflush(stdout);
    
    ck_assert( dataset_species_names != NULL);
    

    ck_assert( escdf_group_dataset_write_simple(dataset_species_names, (void*) &names ) == ESCDF_SUCCESS);
    printf("finished writing species names.\n"); fflush(stdout);

    ck_assert( escdf_group_dataset_read_simple(dataset_species_names, (void*) &read_names ) == ESCDF_SUCCESS);
    printf("finished reading species names.\n"); fflush(stdout);

}
END_TEST 



Suite * make_group_suite(void)
{
    Suite *s;
    TCase *tc_group_new, *tc_group_location, *tc_group_high_level, *tc_group_setters, *tc_group_datasets;

    s = suite_create("Groups");

    tc_group_new = tcase_create("New");
    tcase_add_checked_fixture(tc_group_new, group_handle_setup, group_teardown);
    tcase_add_test(tc_group_new, test_group_new);
    suite_add_tcase(s, tc_group_new);

    tc_group_location = tcase_create("Location");
    tcase_add_checked_fixture(tc_group_location, group_setup, group_teardown);
    tcase_add_test(tc_group_location, test_group_open_location);
    tcase_add_test(tc_group_location, test_group_open_location_name);
    tcase_add_test(tc_group_location, test_group_create_location);
    tcase_add_test(tc_group_location, test_group_create_location_name);
    tcase_add_test(tc_group_location, test_group_close_location);
    suite_add_tcase(s, tc_group_location);

    tc_group_high_level = tcase_create("Group high-level creators/destructors");
    tcase_add_checked_fixture(tc_group_high_level, group_handle_setup, group_teardown);
    tcase_add_test(tc_group_high_level, test_group_open);
    tcase_add_test(tc_group_high_level, test_group_open_name);
    tcase_add_test(tc_group_high_level, test_group_create);
    tcase_add_test(tc_group_high_level, test_group_create_name);
    tcase_add_test(tc_group_high_level, test_group_close);
    suite_add_tcase(s, tc_group_high_level);

    tc_group_setters = tcase_create("Group attribute setters/getters");
    tcase_add_checked_fixture(tc_group_setters, setters_setup, setters_teardown);
    tcase_add_test(tc_group_setters, test_group_attribute_system);
    suite_add_tcase(s, tc_group_setters);

    tc_group_datasets = tcase_create("Group attributes write/read");
    tcase_add_checked_fixture(tc_group_datasets, dataset_setup, dataset_teardown);
    tcase_add_test(tc_group_datasets, test_group_datasets_system);
    suite_add_tcase(s, tc_group_datasets);

    return s;
}
