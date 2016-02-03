/*
 Copyright (C) 2016 M. Oliveira

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

#include "escdf_common.h"
#include "escdf_handle.h"

#define FILE "test_file.h5"
#define GROUP_A "GroupA"
#define GROUP_B "GroupB"

void handle_setup(void)
{
    hid_t file_id, group_id1, group_id2;

    file_id = H5Fcreate(FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    group_id1 = H5Gcreate(file_id, GROUP_A, H5P_DEFAULT, H5P_DEFAULT,
            H5P_DEFAULT);
    group_id2 = H5Gcreate(group_id1, GROUP_B, H5P_DEFAULT, H5P_DEFAULT,
            H5P_DEFAULT);
    H5Gclose(group_id2);
    H5Gclose(group_id1);
    H5Fclose(file_id);
}

void handle_teardown(void)
{
    unlink(FILE);
}

START_TEST(test_handle_open)
{
    escdf_handle_t *handle;

    ck_assert((handle = escdf_open(FILE, NULL)) != NULL);
    ck_assert(escdf_close(handle) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_handle_create)
{
    escdf_handle_t *handle;

    ck_assert((handle = escdf_create(FILE, NULL)) != NULL);
    ck_assert(escdf_close(handle) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_handle_open_path)
{
    escdf_handle_t *handle;

    ck_assert((handle = escdf_open(FILE, GROUP_A"/"GROUP_B)) != NULL);
    ck_assert(escdf_close(handle) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_handle_create_path)
{
    escdf_handle_t *handle;

    ck_assert((handle = escdf_create(FILE, GROUP_A"/"GROUP_B)) != NULL);
    ck_assert(escdf_close(handle) == ESCDF_SUCCESS);
}
END_TEST


Suite * make_handle_suite(void)
{
    Suite *s;
    TCase *tc_handle_new, *tc_handle_existing;

    s = suite_create("Handle");

    tc_handle_new = tcase_create("New file");
    tcase_add_checked_fixture(tc_handle_new, NULL, handle_teardown);
    tcase_add_test(tc_handle_new, test_handle_create);
    tcase_add_test(tc_handle_new, test_handle_create_path);
    suite_add_tcase(s, tc_handle_new);

    tc_handle_existing = tcase_create("Existing file");
    tcase_add_checked_fixture(tc_handle_existing, handle_setup, handle_teardown);
    tcase_add_test(tc_handle_existing, test_handle_open);
    tcase_add_test(tc_handle_existing, test_handle_open_path);
    suite_add_tcase(s, tc_handle_existing);

    return s;
}
