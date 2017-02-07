/*
 Copyright (C) 2017 M. Oliveira

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <check.h>
#include <unistd.h>

#include "utils_hdf5.h"

#define FILE "check_utils_hdf5_test_file.h5"
#define GROUP "mygroup"
#define ATTRIBUTE_S "myscalarattribute"
#define ATTRIBUTE_A "myarrayattribute"
#define DATASET "mydataset"

static hid_t file_id, root_id, group_id;
static hid_t space_scalar_id, space_array_id, space_null_id;
static hid_t attribute_scalar_id, attribute_array_id;
static hid_t dataset_id;

void utils_hdf5_setup(void)
{
    double scalar = 10.0;
    hsize_t dims[2] = {3, 2};
    double array[3][2] = {{1.0, 2.0},
                          {3.0, 4.0},
                          {5.0, 6.0}};
    file_id = H5Fcreate(FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    root_id = H5Gopen(file_id, ".", H5P_DEFAULT);
    group_id = H5Gcreate(root_id, GROUP, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    space_scalar_id = H5Screate(H5S_SCALAR);
    space_array_id = H5Screate_simple(2, dims, NULL);
    space_null_id = H5Screate(H5S_NULL);

    attribute_scalar_id = H5Acreate(group_id, ATTRIBUTE_S, H5T_NATIVE_DOUBLE, space_scalar_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_scalar_id, H5T_NATIVE_DOUBLE, &scalar);
    attribute_array_id = H5Acreate(group_id, ATTRIBUTE_A, H5T_NATIVE_DOUBLE, space_array_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_array_id, H5T_NATIVE_DOUBLE, &array);

    dataset_id = H5Dcreate(group_id, DATASET, H5T_NATIVE_DOUBLE, space_array_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, space_array_id, space_array_id, H5P_DEFAULT, &array);
}

void utils_hdf5_teardown(void)
{
    H5Dclose(dataset_id);
    H5Aclose(attribute_scalar_id);
    H5Aclose(attribute_array_id);
    H5Sclose(space_scalar_id);
    H5Sclose(space_array_id);
    H5Sclose(space_null_id);
    H5Gclose(group_id);
    H5Gclose(root_id);
    H5Fclose(file_id);
    unlink(FILE);
}

/* check_present */
START_TEST(test_utils_hdf5_check_present_true)
{
    ck_assert(utils_hdf5_check_present(root_id, GROUP));
}
END_TEST

START_TEST(test_utils_hdf5_check_present_false)
{
    ck_assert(!utils_hdf5_check_present(root_id, "not_present"));
}
END_TEST

/* check_shape */
START_TEST(test_utils_hdf5_check_shape_scalar_correct)
{
    ck_assert(utils_hdf5_check_shape(space_scalar_id, NULL, 0) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_check_shape_array_correct)
{
    hsize_t dims[2] = {3, 2};
    ck_assert(utils_hdf5_check_shape(space_array_id, dims, 2) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_check_shape_scalar_wrong_args)
{
    hsize_t dims[1] = {2};
    ck_assert(utils_hdf5_check_shape(space_scalar_id, dims, 1) == ESCDF_ERROR);
}
END_TEST

START_TEST(test_utils_hdf5_check_shape_array_wrong_args)
{
    hsize_t dims[1] = {2};
    ck_assert(utils_hdf5_check_shape(space_array_id, dims, 1) == ESCDF_ERROR_DIM);

}
END_TEST

START_TEST(test_utils_hdf5_check_shape_no_class)
{
    ck_assert(utils_hdf5_check_shape(group_id, NULL, 0) == ESCDF_ERROR_ARGS);
}
END_TEST

START_TEST(test_utils_hdf5_check_shape_wrong_class)
{
    ck_assert(utils_hdf5_check_shape(space_null_id, NULL, 0) == ESCDF_ERROR_DIM);
}
END_TEST

/* check_attr */
START_TEST(test_utils_hdf5_check_attr_scalar)
{
    ck_assert(utils_hdf5_check_attr(group_id, ATTRIBUTE_S, NULL, 0, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_check_attr_array)
{
    hsize_t dims[2] = {3, 2};
    ck_assert(utils_hdf5_check_attr(group_id, ATTRIBUTE_A, dims, 2, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_check_attr_array_wrong)
{
    hsize_t dims[2] = {1, 3};
    ck_assert(utils_hdf5_check_attr(group_id, ATTRIBUTE_A, dims, 2, NULL) == ESCDF_ERROR);
}
END_TEST

START_TEST(test_utils_hdf5_check_attr_ptr)
{
    hid_t attr_id = 0;
    ck_assert(utils_hdf5_check_attr(group_id, ATTRIBUTE_S, NULL, 0, &attr_id) == ESCDF_SUCCESS);
    ck_assert(attr_id != 0);
    H5Aclose(attr_id);
}
END_TEST

/* check_dataset */
START_TEST(test_utils_hdf5_check_dataset)
{
    hsize_t dims[2] = {3, 2};
    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_check_dataset_wrong)
{
    hsize_t dims[2] = {1, 3};
    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, NULL) == ESCDF_ERROR);
}
END_TEST

START_TEST(test_utils_hdf5_check_dataset_ptr)
{
    hid_t dtset_id = 0;
    hsize_t dims[2] = {3, 2};
    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(dtset_id != 0);
    H5Dclose(dtset_id);
}
END_TEST

/* read_attr */
START_TEST(test_utils_hdf5_read_attr_scalar)
{
    double value;
    ck_assert(utils_hdf5_read_attr(group_id, ATTRIBUTE_S, H5T_NATIVE_DOUBLE, NULL, 0, &value) == ESCDF_SUCCESS);
    ck_assert(value == 10.0);
}
END_TEST

START_TEST(test_utils_hdf5_read_attr_array)
{
    hsize_t dims[2] = {3, 2};
    double values[3][2];
    ck_assert(utils_hdf5_read_attr(group_id, ATTRIBUTE_A, H5T_NATIVE_DOUBLE, dims, 2, &values) == ESCDF_SUCCESS);
    ck_assert(values[0][0] == 1.0);
    ck_assert(values[0][1] == 2.0);
    ck_assert(values[1][0] == 3.0);
    ck_assert(values[1][1] == 4.0);
    ck_assert(values[2][0] == 5.0);
    ck_assert(values[2][1] == 6.0);
}
END_TEST

/* read_dataset */
START_TEST(test_utils_hdf5_read_dataset)
{
    hid_t dtset_id = 0;
    hsize_t dims[2] = {3, 2};
    double values[3][2];
    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_dataset(dtset_id, H5P_DEFAULT, &values, H5T_NATIVE_DOUBLE, NULL, NULL, NULL) == ESCDF_SUCCESS);
    ck_assert(values[0][0] == 1.0);
    ck_assert(values[0][1] == 2.0);
    ck_assert(values[1][0] == 3.0);
    ck_assert(values[1][1] == 4.0);
    ck_assert(values[2][0] == 5.0);
    ck_assert(values[2][1] == 6.0);
    H5Dclose(dtset_id);
}
END_TEST

START_TEST(test_utils_hdf5_read_dataset_sliced)
{
    hid_t dtset_id = 0;
    hsize_t dims[2] = {3, 2};
    double values[3];
    hsize_t start[2] = {0, 0};
    hsize_t count[2] = {3, 1};
    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_dataset(dtset_id, H5P_DEFAULT, &values, H5T_NATIVE_DOUBLE, start, count, NULL) == ESCDF_SUCCESS);
    ck_assert(values[0] == 1.0);
    ck_assert(values[1] == 3.0);
    ck_assert(values[2] == 5.0);
    H5Dclose(dtset_id);
}
END_TEST

/* read_dataset_at */
START_TEST(test_utils_hdf5_read_dataset_at)
{
    hid_t dtset_id = 0;
    hsize_t dims[2] = {3, 2};
    hsize_t number_of_points = 6;
    hsize_t coordinates[6][2] = {{0, 0},
                                 {1, 0},
                                 {2, 0},
                                 {0, 1},
                                 {1, 1},
                                 {2, 1}};
    double values[6];
    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_dataset_at(dtset_id, H5P_DEFAULT, &values, H5T_NATIVE_DOUBLE, number_of_points, coordinates) == ESCDF_SUCCESS);
    ck_assert(values[0] == 1.0);
    ck_assert(values[1] == 3.0);
    ck_assert(values[2] == 5.0);
    ck_assert(values[3] == 2.0);
    ck_assert(values[4] == 4.0);
    ck_assert(values[5] == 6.0);
    H5Dclose(dtset_id);
}
END_TEST

START_TEST(test_utils_hdf5_read_dataset_at_empty)
{
    hid_t dtset_id = 0;
    hsize_t dims[2] = {3, 2};
    hsize_t number_of_points = 0;
    double *values = NULL, *coordinates = NULL;
    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_dataset_at(dtset_id, H5P_DEFAULT, &values, H5T_NATIVE_DOUBLE, number_of_points, coordinates) == ESCDF_SUCCESS);
    ck_assert_ptr_eq(values, NULL);
    H5Dclose(dtset_id);
}
END_TEST

/* create_group */
START_TEST(test_utils_hdf5_create_group)
{
    ck_assert(utils_hdf5_create_group(file_id, "somegroup", NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_create_group_ptr)
{
    hid_t tmp_id = 0;
    ck_assert(utils_hdf5_create_group(file_id, "somegroup", &tmp_id) == ESCDF_SUCCESS);
    ck_assert(tmp_id != 0);
    H5Gclose(tmp_id);
}END_TEST

START_TEST(test_utils_hdf5_create_group_overwrite)
{
    ck_assert(utils_hdf5_create_group(file_id, GROUP, NULL) == ESCDF_ERROR_ARGS);
}
END_TEST

START_TEST(test_utils_hdf5_create_group_existing_path)
{
    ck_assert(utils_hdf5_create_group(file_id, "mygroup/somegroup", NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_create_group_non_existing_path)
{
    ck_assert(utils_hdf5_create_group(file_id, "somegroup1/somegroup2", NULL) == ESCDF_SUCCESS);
}
END_TEST


Suite * make_utils_hdf5_suite(void)
{
    Suite *s;
    TCase *tc_utils_hdf5_check_present, *tc_utils_hdf5_check_shape, *tc_utils_hdf5_check_attr, *tc_utils_hdf5_check_dataset;
    TCase *tc_utils_hdf5_read_attr, *tc_utils_hdf5_read_dataset;
    TCase *tc_utils_hdf5_create_group;

    s = suite_create("HDF5 utilities");

    tc_utils_hdf5_check_present = tcase_create("Check present");
    tcase_add_checked_fixture(tc_utils_hdf5_check_present, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_present, test_utils_hdf5_check_present_true);
    tcase_add_test(tc_utils_hdf5_check_present, test_utils_hdf5_check_present_false);
    suite_add_tcase(s, tc_utils_hdf5_check_present);

    tc_utils_hdf5_check_shape = tcase_create("Check shape");
    tcase_add_checked_fixture(tc_utils_hdf5_check_shape, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_scalar_correct);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_array_correct);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_scalar_wrong_args);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_array_wrong_args);
//    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_no_class);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_wrong_class);
    suite_add_tcase(s, tc_utils_hdf5_check_shape);

    tc_utils_hdf5_check_attr = tcase_create("Check attribute");
    tcase_add_checked_fixture(tc_utils_hdf5_check_attr, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_attr, test_utils_hdf5_check_attr_scalar);
    tcase_add_test(tc_utils_hdf5_check_attr, test_utils_hdf5_check_attr_array);
    tcase_add_test(tc_utils_hdf5_check_attr, test_utils_hdf5_check_attr_array_wrong);
    tcase_add_test(tc_utils_hdf5_check_attr, test_utils_hdf5_check_attr_ptr);
    suite_add_tcase(s, tc_utils_hdf5_check_attr);

    tc_utils_hdf5_check_dataset = tcase_create("Check dataset");
    tcase_add_checked_fixture(tc_utils_hdf5_check_dataset, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_dataset, test_utils_hdf5_check_dataset);
    tcase_add_test(tc_utils_hdf5_check_dataset, test_utils_hdf5_check_dataset_wrong);
    tcase_add_test(tc_utils_hdf5_check_dataset, test_utils_hdf5_check_dataset_ptr);
    suite_add_tcase(s, tc_utils_hdf5_check_dataset);

    tc_utils_hdf5_read_attr = tcase_create("Read attribute");
    tcase_add_checked_fixture(tc_utils_hdf5_read_attr, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_attr_scalar);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_attr_array);
    suite_add_tcase(s, tc_utils_hdf5_read_attr);

    tc_utils_hdf5_read_dataset = tcase_create("Read dataset");
    tcase_add_checked_fixture(tc_utils_hdf5_read_dataset, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_read_dataset, test_utils_hdf5_read_dataset);
    tcase_add_test(tc_utils_hdf5_read_dataset, test_utils_hdf5_read_dataset_sliced);
    tcase_add_test(tc_utils_hdf5_read_dataset, test_utils_hdf5_read_dataset_at);
    tcase_add_test(tc_utils_hdf5_read_dataset, test_utils_hdf5_read_dataset_at_empty);
    suite_add_tcase(s, tc_utils_hdf5_read_dataset);

    tc_utils_hdf5_create_group = tcase_create("Create group");
    tcase_add_checked_fixture(tc_utils_hdf5_create_group, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_create_group, test_utils_hdf5_create_group);
    tcase_add_test(tc_utils_hdf5_create_group, test_utils_hdf5_create_group_ptr);
    tcase_add_test(tc_utils_hdf5_create_group, test_utils_hdf5_create_group_overwrite);
    tcase_add_test(tc_utils_hdf5_create_group, test_utils_hdf5_create_group_existing_path);
    tcase_add_test(tc_utils_hdf5_create_group, test_utils_hdf5_create_group_non_existing_path);
    suite_add_tcase(s, tc_utils_hdf5_create_group);

    return s;
}