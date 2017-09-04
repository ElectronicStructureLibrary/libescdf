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

#define CHKFILE "check_escdf_handle_test_file.h5"
#define GROUP_A "GroupA"
#define GROUP_B "GroupB"

static escdf_handle_t *handle = NULL;

void handle_setup_file(void)
{
    hid_t file_id, group_id1, group_id2;

    file_id = H5Fcreate(CHKFILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    group_id1 = H5Gcreate(file_id, GROUP_A, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    group_id2 = H5Gcreate(group_id1, GROUP_B, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(group_id2);
    H5Gclose(group_id1);
    H5Fclose(file_id);
}

void handle_teardown_file(void)
{
    unlink(CHKFILE);
}

void handle_setup(void)
{
    handle_setup_file();
    handle = escdf_open(CHKFILE, NULL);
}

void handle_teardown(void)
{
    escdf_close(handle);
    handle = NULL;
    handle_teardown_file();
}


START_TEST(test_handle_open)
{
    ck_assert((handle = escdf_open(CHKFILE, NULL)) != NULL);
}
END_TEST

START_TEST(test_handle_open_path)
{
    ck_assert((handle = escdf_open(CHKFILE, GROUP_A"/"GROUP_B)) != NULL);
}
END_TEST

START_TEST(test_handle_create)
{
    ck_assert((handle = escdf_create(CHKFILE, NULL)) != NULL);
}
END_TEST

START_TEST(test_handle_create_path)
{
    ck_assert((handle = escdf_create(CHKFILE, GROUP_A"/"GROUP_B)) != NULL);
}
END_TEST

START_TEST(test_handle_close)
{
    ck_assert(escdf_close(handle) == ESCDF_SUCCESS);
}
END_TEST


Suite * make_handle_suite(void)
{
    Suite *s;
    TCase *tc_handle_open, *tc_handle_create, *tc_handle_overwrite, *tc_handle_close;

    s = suite_create("Handle");

    tc_handle_open = tcase_create("Open file");
    tcase_add_checked_fixture(tc_handle_open, handle_setup_file, handle_teardown);
    tcase_add_test(tc_handle_open, test_handle_open);
    tcase_add_test(tc_handle_open, test_handle_open_path);
    suite_add_tcase(s, tc_handle_open);

    tc_handle_create = tcase_create("Create file");
    tcase_add_checked_fixture(tc_handle_create, NULL, handle_teardown);
    tcase_add_test(tc_handle_create, test_handle_create);
    tcase_add_test(tc_handle_create, test_handle_create_path);
    suite_add_tcase(s, tc_handle_create);

    tc_handle_overwrite = tcase_create("Overwrite file");
    tcase_add_checked_fixture(tc_handle_overwrite, handle_setup_file, handle_teardown);
    tcase_add_test(tc_handle_overwrite, test_handle_create);
    tcase_add_test(tc_handle_overwrite, test_handle_create_path);
    suite_add_tcase(s, tc_handle_overwrite);

    tc_handle_close = tcase_create("Close file");
    tcase_add_checked_fixture(tc_handle_close, handle_setup, handle_teardown_file);
    tcase_add_test(tc_handle_close, test_handle_close);
    suite_add_tcase(s, tc_handle_close);

    return s;
}
