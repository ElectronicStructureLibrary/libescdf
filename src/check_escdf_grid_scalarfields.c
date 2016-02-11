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
    escdf_handle_t *file_id;
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    
    file_id = escdf_open(ESCDF_CHK_DATADIR "/grid_scalarfield_read.h5", NULL);
    ck_assert(file_id != NULL);

    scalarfield = escdf_grid_scalarfield_new("densities/pseudo_density");
    err = escdf_grid_scalarfield_read_metadata(scalarfield, file_id);
    ck_assert(err == ESCDF_SUCCESS);

    escdf_grid_scalarfield_free(scalarfield);

    escdf_close(file_id);
}
END_TEST

START_TEST(test_write_metadata)
{
    escdf_handle_t *file_id;
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    unsigned int uarr[2], uval;
    double darr[4];
    
    scalarfield = escdf_grid_scalarfield_new(NULL);

    escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    uarr[0] = 0;
    uarr[1] = 2;
    escdf_grid_scalarfield_set_dimension_types(scalarfield, uarr, 2);
    darr[0] = 1.;
    darr[1] = 2.;
    darr[2] = 3.;
    darr[3] = 4.;
    escdf_grid_scalarfield_set_lattice_vectors(scalarfield, darr, 4);
    uarr[0] = 6;
    uarr[1] = 4;
    escdf_grid_scalarfield_set_number_of_grid_points(scalarfield, uarr, 2);
    escdf_grid_scalarfield_set_number_of_components(scalarfield, 2);
    escdf_grid_scalarfield_set_real_or_complex(scalarfield, 1);
    escdf_grid_scalarfield_set_use_default_ordering(scalarfield, false);
    
    /* Create a new file using default properties. */
    file_id = escdf_create("tmp_grid_scalarfield_write.h5", NULL);
    ck_assert(file_id != NULL);

    err = escdf_grid_scalarfield_write_metadata(scalarfield, file_id);
    ck_assert(err == ESCDF_SUCCESS);

    escdf_grid_scalarfield_free(scalarfield);

    escdf_close(file_id);

    /* Read the file to be sure ! */
    file_id = escdf_open("tmp_grid_scalarfield_write.h5", NULL);
    ck_assert(file_id != NULL);

    scalarfield = escdf_grid_scalarfield_new("density");
    err = escdf_grid_scalarfield_read_metadata(scalarfield, file_id);
    ck_assert(err == ESCDF_SUCCESS);

    uval = escdf_grid_scalarfield_get_number_of_physical_dimensions(scalarfield);
    ck_assert(uval == 2);
    
    err = escdf_grid_scalarfield_get_dimension_types(scalarfield, uarr, 2);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(uarr[0] == 0 && uarr[1] == 2);

    err = escdf_grid_scalarfield_get_lattice_vectors(scalarfield, darr, 4);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(darr[0] == 1. && darr[1] == 2. && darr[2] == 3. && darr[3] == 4.);

    err = escdf_grid_scalarfield_get_number_of_grid_points(scalarfield, uarr, 2);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(uarr[0] == 6 && uarr[1] == 4);

    uval = escdf_grid_scalarfield_get_number_of_components(scalarfield);
    ck_assert(uval == 2);
    
    uval = escdf_grid_scalarfield_get_real_or_complex(scalarfield);
    ck_assert(uval == 1);
    
    uval = escdf_grid_scalarfield_get_use_default_ordering(scalarfield);
    ck_assert(uval == 0);

    escdf_grid_scalarfield_free(scalarfield);

    escdf_close(file_id);
}
END_TEST

START_TEST(test_getters)
{
    escdf_handle_t *file_id;
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    unsigned int uval, uarr[3];
    const unsigned int *upt;
    double darr[9];
    const double *dpt;
    
    /* Create a new file using default properties. */
    file_id = escdf_open(ESCDF_CHK_DATADIR "/grid_scalarfield_read.h5", NULL);
    ck_assert(file_id != NULL);

    scalarfield = escdf_grid_scalarfield_new("densities/pseudo_density");
    err = escdf_grid_scalarfield_read_metadata(scalarfield, file_id);
    ck_assert(err == ESCDF_SUCCESS);

    escdf_close(file_id);

    uval = escdf_grid_scalarfield_get_number_of_physical_dimensions(scalarfield);
    ck_assert(uval == 3);
    
    err = escdf_grid_scalarfield_get_dimension_types(scalarfield, uarr, 3);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(uarr[0] == 0 && uarr[1] == 1 && uarr[2] == 0);
    upt = escdf_grid_scalarfield_ptr_dimension_types(scalarfield);
    ck_assert(upt != NULL);
    ck_assert(upt[0] == 0 && upt[1] == 1 && upt[2] == 0);

    err = escdf_grid_scalarfield_get_lattice_vectors(scalarfield, darr, 9);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(darr[0] == 5. && darr[1] == 0. && darr[2] == 0. &&
              darr[3] == 0. && darr[4] == 10. && darr[5] == 0. &&
              darr[6] == 0. && darr[7] == 0. && darr[8] == 15.);
    dpt = escdf_grid_scalarfield_ptr_lattice_vectors(scalarfield);
    ck_assert(dpt != NULL);
    ck_assert(dpt[0] == 5. && dpt[1] == 0. && dpt[2] == 0. &&
              dpt[3] == 0. && dpt[4] == 10. && dpt[5] == 0. &&
              dpt[6] == 0. && dpt[7] == 0. && dpt[8] == 15.);

    err = escdf_grid_scalarfield_get_number_of_grid_points(scalarfield, uarr, 3);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(uarr[0] == 2 && uarr[1] == 3 && uarr[2] == 9);
    upt = escdf_grid_scalarfield_ptr_number_of_grid_points(scalarfield);
    ck_assert(upt != NULL);
    ck_assert(upt[0] == 2 && upt[1] == 3 && upt[2] == 9);

    uval = escdf_grid_scalarfield_get_number_of_components(scalarfield);
    ck_assert(uval == 2);
    
    uval = escdf_grid_scalarfield_get_real_or_complex(scalarfield);
    ck_assert(uval == 1);
    
    uval = escdf_grid_scalarfield_get_use_default_ordering(scalarfield);
    ck_assert(uval == 1);
    
    escdf_grid_scalarfield_free(scalarfield);
}
END_TEST

START_TEST(test_set_number_of_physical_dimensions)
{
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    unsigned int uval;

    scalarfield = escdf_grid_scalarfield_new("density");

    err = escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    ck_assert(err == ESCDF_SUCCESS);
    uval = escdf_grid_scalarfield_get_number_of_physical_dimensions(scalarfield);
    ck_assert(uval == 2);
    
    escdf_grid_scalarfield_free(scalarfield);
}
END_TEST

START_TEST(test_set_dimension_types)
{
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    unsigned int uarr[3];
    const unsigned int *upt;

    scalarfield = escdf_grid_scalarfield_new("density");
    err = escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    ck_assert(err == ESCDF_SUCCESS);
    
    uarr[0] = 2;
    uarr[1] = 0;
    err = escdf_grid_scalarfield_set_dimension_types(scalarfield, uarr, 2);
    ck_assert(err == ESCDF_SUCCESS);
    err = escdf_grid_scalarfield_get_dimension_types(scalarfield, uarr, 2);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(uarr[0] == 2 && uarr[1] == 0);
    upt = escdf_grid_scalarfield_ptr_dimension_types(scalarfield);
    ck_assert(upt != NULL);
    ck_assert(upt[0] == 2 && upt[1] == 0);
    
    escdf_grid_scalarfield_free(scalarfield);
}
END_TEST

START_TEST(test_set_lattice_vectors)
{
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    double darr[4];
    const double *dpt;

    scalarfield = escdf_grid_scalarfield_new("density");
    err = escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    ck_assert(err == ESCDF_SUCCESS);
    
    darr[0] = 1.;
    darr[1] = 2.;
    darr[2] = 3.;
    darr[3] = 4.;
    err = escdf_grid_scalarfield_set_lattice_vectors(scalarfield, darr, 4);
    ck_assert(err == ESCDF_SUCCESS);
    err = escdf_grid_scalarfield_get_lattice_vectors(scalarfield, darr, 4);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(darr[0] == 1. && darr[1] == 2. && darr[2] == 3. && darr[3] == 4.);
    dpt = escdf_grid_scalarfield_ptr_lattice_vectors(scalarfield);
    ck_assert(dpt != NULL);
    ck_assert(dpt[0] == 1. && dpt[1] == 2. && dpt[2] == 3. && dpt[3] == 4.);
    
    escdf_grid_scalarfield_free(scalarfield);
}
END_TEST

START_TEST(test_set_number_of_grid_points)
{
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    unsigned int uarr[3];
    const unsigned int *upt;

    scalarfield = escdf_grid_scalarfield_new("density");
    err = escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    ck_assert(err == ESCDF_SUCCESS);
    
    uarr[0] = 6;
    uarr[1] = 3;
    err = escdf_grid_scalarfield_set_number_of_grid_points(scalarfield, uarr, 2);
    ck_assert(err == ESCDF_SUCCESS);
    err = escdf_grid_scalarfield_get_number_of_grid_points(scalarfield, uarr, 2);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(uarr[0] == 6 && uarr[1] == 3);
    upt = escdf_grid_scalarfield_ptr_number_of_grid_points(scalarfield);
    ck_assert(upt != NULL);
    ck_assert(upt[0] == 6 && upt[1] == 3);
    
    escdf_grid_scalarfield_free(scalarfield);
}
END_TEST

START_TEST(test_set_number_of_components)
{
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    unsigned int uval;

    scalarfield = escdf_grid_scalarfield_new("density");

    err = escdf_grid_scalarfield_set_number_of_components(scalarfield, 4);
    ck_assert(err == ESCDF_SUCCESS);
    uval = escdf_grid_scalarfield_get_number_of_components(scalarfield);
    ck_assert(uval == 4);
    
    
    escdf_grid_scalarfield_free(scalarfield);
}
END_TEST

START_TEST(test_set_real_or_complex)
{
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    unsigned int uval;

    scalarfield = escdf_grid_scalarfield_new("density");

    err = escdf_grid_scalarfield_set_real_or_complex(scalarfield, 1);
    ck_assert(err == ESCDF_SUCCESS);
    uval = escdf_grid_scalarfield_get_real_or_complex(scalarfield);
    ck_assert(uval == 1);
    
    escdf_grid_scalarfield_free(scalarfield);
}
END_TEST

START_TEST(test_set_use_default_ordering)
{
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    unsigned int uval;

    scalarfield = escdf_grid_scalarfield_new("density");

    err = escdf_grid_scalarfield_set_use_default_ordering(scalarfield, 0);
    ck_assert(err == ESCDF_SUCCESS);
    uval = escdf_grid_scalarfield_get_use_default_ordering(scalarfield);
    ck_assert(uval == 0);
    
    escdf_grid_scalarfield_free(scalarfield);
}
END_TEST

START_TEST(test_read_values_on_grid)
{
    escdf_handle_t *file_id;
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    double dens[108];
    hsize_t start[3] = {0, 2, 0};
    hsize_t count[3] = {2, 2, 1};
    unsigned int i, j;
    
    file_id = escdf_open(ESCDF_CHK_DATADIR "/grid_scalarfield_read.h5", NULL);
    ck_assert(file_id != NULL);

    scalarfield = escdf_grid_scalarfield_new("densities/pseudo_density");
    err = escdf_grid_scalarfield_read_metadata(scalarfield, file_id);
    ck_assert(err == ESCDF_SUCCESS);

    /* total reading. */
    err = escdf_grid_scalarfield_read_values_on_grid(scalarfield, file_id, dens, NULL, NULL, NULL);
    ck_assert(err == ESCDF_SUCCESS);
    for (j = 0; j < 2; j++) {
        for (i = 0; i < 54; i++) {
            if (i < 9)
                ck_assert(dens[i + j * 54] == i + j * 9);
            else if (i >= 12 && i < 21)
                ck_assert(dens[i + j * 54] == i - 12 + 1 + j * 9);
            else if (i >= 24 && i < 33)
                ck_assert(dens[i + j * 54] == i - 24 + 2 + j * 9);
            else
                ck_assert(dens[i + j * 54] == 0.);
        }
    }
    
    /* Slice reading. */
    err = escdf_grid_scalarfield_read_values_on_grid(scalarfield, file_id, dens, start, count, NULL);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(dens[0] == 2.);
    ck_assert(dens[1] == 3.);
    ck_assert(dens[2] == 11.);
    ck_assert(dens[3] == 12.);

    escdf_grid_scalarfield_free(scalarfield);

    escdf_close(file_id);
}
END_TEST

START_TEST(test_write_values_on_grid)
{
    escdf_handle_t *file_id;
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    unsigned int uarr[2], uval;
    double darr[4];

    double dens[48];
    hsize_t start[3] = {0, 2, 0};
    hsize_t count[3] = {2, 9, 1};
    unsigned int i, j;
    
    scalarfield = escdf_grid_scalarfield_new(NULL);

    escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    uarr[0] = 0;
    uarr[1] = 2;
    escdf_grid_scalarfield_set_dimension_types(scalarfield, uarr, 2);
    darr[0] = 1.;
    darr[1] = 2.;
    darr[2] = 3.;
    darr[3] = 4.;
    escdf_grid_scalarfield_set_lattice_vectors(scalarfield, darr, 4);
    uarr[0] = 6;
    uarr[1] = 4;
    escdf_grid_scalarfield_set_number_of_grid_points(scalarfield, uarr, 2);
    escdf_grid_scalarfield_set_number_of_components(scalarfield, 2);
    escdf_grid_scalarfield_set_real_or_complex(scalarfield, 1);
    escdf_grid_scalarfield_set_use_default_ordering(scalarfield, true);
    
    /* Create a new file using default properties. */
    file_id = escdf_create("tmp_grid_scalarfield_write.h5", NULL);
    ck_assert(file_id != NULL);

    err = escdf_grid_scalarfield_write_metadata(scalarfield, file_id);
    ck_assert(err == ESCDF_SUCCESS);

    for (i = 0; i  < 18; i++) {
        dens[i] = i;
    }
    err = escdf_grid_scalarfield_write_values_on_grid_ordered(scalarfield, file_id, dens, start, count, NULL);
    ck_assert(err == ESCDF_SUCCESS);

    err = escdf_grid_scalarfield_read_values_on_grid(scalarfield, file_id, dens, NULL, NULL, NULL);
    ck_assert(err == ESCDF_SUCCESS);
    for (j = 0; j < 2; j++) {
        for (i = 0; i < 24; i++) {
            if (i > 1 && i < 11)
                ck_assert(dens[i + j * 24] == i - 2 + j * 9);
            else
                ck_assert(dens[i + j * 24] == 0.);
        }
    }

    escdf_grid_scalarfield_free(scalarfield);

    escdf_close(file_id);
}
END_TEST

START_TEST(test_read_values_on_grid_sliced)
{
    escdf_handle_t *file_id;
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    unsigned int uarr[2], uval;
    double darr[4];

    double dens[48];
    unsigned int *tbl;
    unsigned int i, j;
    
    /* Generate a density on disk with default ordering. */
    scalarfield = escdf_grid_scalarfield_new(NULL);

    escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    uarr[0] = 0;
    uarr[1] = 2;
    escdf_grid_scalarfield_set_dimension_types(scalarfield, uarr, 2);
    darr[0] = 1.;
    darr[1] = 2.;
    darr[2] = 3.;
    darr[3] = 4.;
    escdf_grid_scalarfield_set_lattice_vectors(scalarfield, darr, 4);
    uarr[0] = 6;
    uarr[1] = 4;
    escdf_grid_scalarfield_set_number_of_grid_points(scalarfield, uarr, 2);
    escdf_grid_scalarfield_set_number_of_components(scalarfield, 2);
    escdf_grid_scalarfield_set_real_or_complex(scalarfield, 1);
    escdf_grid_scalarfield_set_use_default_ordering(scalarfield, true);
    
    /* Create a new file using default properties. */
    file_id = escdf_create("tmp_grid_scalarfield_read.h5", NULL);
    ck_assert(file_id != NULL);

    err = escdf_grid_scalarfield_write_metadata(scalarfield, file_id);
    ck_assert(err == ESCDF_SUCCESS);

    for (i = 0; i  < 48; i++) {
      dens[i] = (i < 24) ? (double)i : -((double)i - 24);
    }
    err = escdf_grid_scalarfield_write_values_on_grid_ordered(scalarfield, file_id,
                                                              dens, NULL, NULL, NULL);
    if (err != ESCDF_SUCCESS) {
      escdf_error_show(err, __FILE__, __LINE__, __func__);
    }
    ck_assert(err == ESCDF_SUCCESS);

    for (i = 0; i  < 48; i++) {
      dens[i] = 0.;
    }

    /* Test the ordered reading. */
    err = escdf_grid_scalarfield_read_values_on_grid_sliced(scalarfield, file_id,
                                                            dens, NULL, 24);
    ck_assert(err == ESCDF_SUCCESS);
    
    for (i = 0; i  < 48; i++) {
      ck_assert((i < 24) ? (dens[i] == (double)i) : (dens[i] == -((double)i - 24)));
    }

    /* Test the disordered reading. */
    tbl = malloc(sizeof(unsigned int) * 24);
    for (i = 0; i  < 24; i++) {
      tbl[i] = (i + 12) % 24;
    }
    err = escdf_grid_scalarfield_read_values_on_grid_sliced(scalarfield, file_id,
                                                            dens, tbl, 24);
    free(tbl);
    ck_assert(err == ESCDF_SUCCESS);
    
    for (i = 0; i  < 48; i++) {
      ck_assert((i < 24) ? (dens[i] == (double)((i + 12) % 24)) :
                (dens[i] == -(double)((i + 12) % 24)));
    }

    escdf_close(file_id);

    file_id = escdf_create("tmp_grid_scalarfield_read.h5", NULL);
    ck_assert(file_id != NULL);

    /* Recreate the file disordered. */
    escdf_grid_scalarfield_set_use_default_ordering(scalarfield, false);
    err = escdf_grid_scalarfield_write_metadata(scalarfield, file_id);
    ck_assert(err == ESCDF_SUCCESS);

    tbl = malloc(sizeof(unsigned int) * 24);
    for (i = 0; i  < 24; i++) {
      tbl[i] = (i + 12) % 24;
    }
    for (i = 0; i  < 48; i++) {
      dens[i] = (i < 24) ? (double)tbl[i % 24] : -(double)tbl[i % 24];
    }
    err = escdf_grid_scalarfield_write_values_on_grid_sliced(scalarfield, file_id,
                                                             dens, tbl, 24);
    free(tbl);
    for (i = 0; i  < 48; i++) {
      dens[i] = 0.;
    }

    /* Test the ordered reading. */
    err = escdf_grid_scalarfield_read_values_on_grid_sliced(scalarfield, file_id,
                                                            dens, NULL, 24);
    ck_assert(err == ESCDF_SUCCESS);
    
    for (i = 0; i  < 48; i++) {
      ck_assert((i < 24) ? (dens[i] == (double)i) : (dens[i] == -((double)i - 24)));
    }

    /* Test the disordered reading. */
    tbl = malloc(sizeof(unsigned int) * 24);
    for (i = 0; i  < 24; i++) {
      tbl[i] = (i < 12) ? i * 2 : (i % 12) * 2 + 1;
    }
    err = escdf_grid_scalarfield_read_values_on_grid_sliced(scalarfield, file_id,
                                                            dens, tbl, 24);
    ck_assert(err == ESCDF_SUCCESS);
    
    for (i = 0; i  < 48; i++) {
      ck_assert((i < 24) ? (dens[i] == (double)tbl[i]) : (dens[i] == -(double)tbl[i - 24]));
    }
    free(tbl);

    escdf_close(file_id);

    escdf_grid_scalarfield_free(scalarfield);
}
END_TEST

Suite * make_grid_scalarfield_suite(void)
{
    Suite *s;
    TCase *tc_info;

    s = suite_create("Grid scalarfields");

    tc_info = tcase_create("Read");
    tcase_add_test(tc_info, test_read_metadata);
    tcase_add_test(tc_info, test_getters);
    tcase_add_test(tc_info, test_set_number_of_physical_dimensions);
    tcase_add_test(tc_info, test_set_dimension_types);
    tcase_add_test(tc_info, test_set_lattice_vectors);
    tcase_add_test(tc_info, test_set_number_of_grid_points);
    tcase_add_test(tc_info, test_set_number_of_components);
    tcase_add_test(tc_info, test_set_real_or_complex);
    tcase_add_test(tc_info, test_set_use_default_ordering);
    tcase_add_test(tc_info, test_write_metadata);
    tcase_add_test(tc_info, test_read_values_on_grid);
    tcase_add_test(tc_info, test_write_values_on_grid);
    tcase_add_test(tc_info, test_read_values_on_grid_sliced);
    suite_add_tcase(s, tc_info);

    return s;
}
