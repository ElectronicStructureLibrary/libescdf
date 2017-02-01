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
#define ATTRIBUTE "myattribute"
#define DATASET "mydataset"

static hid_t file_id, root_id, group_id, attr_id, dtset_id;
static hid_t space_scalar_id, space_array_id, space_null_id;

void utils_hdf5_setup(void)
{
    hsize_t dims[2] = {3, 2};

    file_id = H5Fcreate(FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    root_id = H5Gopen(file_id, ".", H5P_DEFAULT);
    group_id = H5Gcreate(root_id, GROUP, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    space_scalar_id = H5Screate(H5S_SCALAR);
    space_array_id = H5Screate_simple(2, dims, NULL);
    space_null_id = H5Screate(H5S_NULL);
}

void utils_hdf5_teardown(void)
{
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


Suite * make_utils_hdf5_suite(void)
{
    Suite *s;
    TCase *tc_utils_hdf5_check_present, *tc_utils_hdf5_check_shape;

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
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_no_class);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_wrong_class);
    suite_add_tcase(s, tc_utils_hdf5_check_shape);

    return s;
}