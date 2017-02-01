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

void utils_hdf5_setup(void)
{
    file_id = H5Fcreate(FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    root_id = H5Gopen(file_id, ".", H5P_DEFAULT);
    group_id = H5Gcreate(root_id, GROUP, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

}

void utils_hdf5_teardown(void)
{
    herr_t status;

    status = H5Gclose(group_id);
    status = H5Gclose(root_id);
    status = H5Fclose(file_id);
    unlink(FILE);
}

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

Suite * make_utils_hdf5_suite(void)
{
    Suite *s;
    TCase *tc_utils_hdf5_check_present;

    s = suite_create("HDF5 utilities");

    tc_utils_hdf5_check_present = tcase_create("Check present");
    tcase_add_checked_fixture(tc_utils_hdf5_check_present, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_present, test_utils_hdf5_check_present_true);
    tcase_add_test(tc_utils_hdf5_check_present, test_utils_hdf5_check_present_false);
    suite_add_tcase(s, tc_utils_hdf5_check_present);

    return s;
}