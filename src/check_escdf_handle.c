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

#include "escdf_common.h"
#include "escdf_handle.h"

#if defined HAVE_CONFIG_H
#include "config.h"
#else
#define ESCDF_CHK_DATADIR "."
#endif

START_TEST(test_handle_open_and_close)
{
    escdf_handle_t *handle;

    handle = escdf_create(ESCDF_CHK_DATADIR "/empty.h5", NULL);
    ck_assert(handle != NULL);
    ck_assert(escdf_close(handle) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_handle_open_and_close_group)
{
    escdf_handle_t *handle;

    handle = escdf_create(ESCDF_CHK_DATADIR "/empty.h5", "densities");
    ck_assert(handle != NULL);
    ck_assert(escdf_close(handle) == ESCDF_SUCCESS);
}
END_TEST



Suite * make_handle_suite(void)
{
    Suite *s;
    TCase *tc_handle;

    s = suite_create("Handle");

    tc_handle = tcase_create("File opening and closing");
    tcase_add_test(tc_handle, test_handle_open_and_close);
    tcase_add_test(tc_handle, test_handle_open_and_close_group);
    suite_add_tcase(s, tc_handle);

    return s;
}
