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

#include "escdf_handle.h"

START_TEST(test_handle_open)
{
    ck_assert(escdf_open("new_file.h5") != NULL);
}
END_TEST

Suite * make_handle_suite(void)
{
    Suite *s;
    TCase *tc_handle;

    s = suite_create("Handle");

    tc_handle = tcase_create("File opening and closing");
    tcase_add_test(tc_handle, test_handle_open);
    suite_add_tcase(s, tc_handle);

    return s;
}
