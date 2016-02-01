/*
 Copyright (C) 2015 M. Oliveira

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

/**
 * @file check_pspsio_info.c
 * @brief checks escdf_info.c and escdf_info.h
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <check.h>

#include "escdf_info.h"

#if defined HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE_STRING ""
#define PACKAGE_VERSION ""
#endif

START_TEST(test_info_version)
{
    int major, minor, micro;
    char *version_string;
    size_t s = strlen(PACKAGE_VERSION);

    version_string = (char *) malloc (s+1);
    ck_assert(version_string != NULL);

    escdf_info_version(&major, &minor, &micro);
    sprintf(version_string,"%d.%d.%d",major,minor,micro);

    ck_assert_str_eq(version_string, PACKAGE_VERSION);
    free(version_string);
}
END_TEST

START_TEST(test_info_string)
{
    char *info_string;
    size_t s = strlen(PACKAGE_STRING);

    info_string = (char *) malloc (s+1);
    ck_assert(info_string != NULL);

    escdf_info_string(info_string);
    ck_assert_str_eq(info_string, PACKAGE_STRING);
    free(info_string);
}
END_TEST


Suite * make_info_suite(void)
{
    Suite *s;
    TCase *tc_info;

    s = suite_create("Info");

    tc_info = tcase_create("Version");
    tcase_add_test(tc_info, test_info_version);
    tcase_add_test(tc_info, test_info_string);
    suite_add_tcase(s, tc_info);

    return s;
}
