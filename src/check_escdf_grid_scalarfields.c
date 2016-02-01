/*
 Copyright (C) 2016 D. Caliste

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
 * @file check_escdf_grid_scalarfields.c
 * @brief checks escdf_grid_scalarfields.c and escdf_grid_scalarfields.h
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <check.h>

#include "escdf_grid_scalarfields.h"

#if defined HAVE_CONFIG_H
#include "config.h"
#else
#define ESCDF_CHK_DATADIR "."
#endif

START_TEST(test_read_metadata)
{
    hid_t file_id;
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    
    /* Create a new file using default properties. */
    file_id = H5Fopen(ESCDF_CHK_DATADIR "/grid_scalarfield_read.h5",
                      H5F_ACC_RDONLY, H5P_DEFAULT);
    ck_assert(file_id >= 0);

    err = escdf_grid_scalarfield_read_metadata(&scalarfield,
                                               file_id, "/densities/pseudo_density");
    ck_assert(err == ESCDF_SUCCESS);

    escdf_grid_scalarfield_free(scalarfield);

    H5Fclose(file_id);
}
END_TEST


Suite * make_grid_scalarfield_suite(void)
{
    Suite *s;
    TCase *tc_info;

    s = suite_create("Grid scalarfields");

    tc_info = tcase_create("Read");
    tcase_add_test(tc_info, test_read_metadata);
    suite_add_tcase(s, tc_info);

    return s;
}
