/* Copyright (C) 2016-2017 Damien Caliste <dcaliste@free.fr>
 *                         Yann Pouillon <devops@materialsevolution.es>
 *
 * This file is part of ESCDF.
 *
 * ESCDF is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, version 2.1 of the License, or (at your option) any
 * later version.
 *
 * ESCDF is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ESCDF.  If not, see <http://www.gnu.org/licenses/> or write to
 * the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA.
 */

/**
 * @file check_escdf_grid_scalarfields.c
 * @brief checks escdf_grid_scalarfields.c and escdf_grid_scalarfields.h
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <check.h>
#include <unistd.h>
#include <hdf5.h>

#include "escdf_grid_scalarfields.h"
#include "utils_hdf5.h"

#if defined HAVE_CONFIG_H
#include "config.h"
#endif


#define CHKFILE "tmp_grid_scalarfield_test_file.h5"

static hid_t file_id, root_id, group_id, subgroup_id;
static hid_t array_id, dataset_id;


void grid_scalarfield_setup(void)
{
    /* Dimensions: d3_1 = 1D length 3, d3_2 = 2D, length (3,3) */
    hsize_t d1_1[1] = {1};
    hsize_t d3_1[1] = {3};
    hsize_t d3_2[2] = {3, 3};

    /* Attributes */
    hsize_t dt[3] = {0, 1, 0};
    double lv[3][3] = {{5.0, 0.0, 0.0}, {0.0, 10.0, 0.0}, {0.0, 0.0, 15.0}};
    hsize_t nc = 2;
    hsize_t ng[3] = {2, 3, 9};
    hsize_t np = 3;
    hsize_t rc = 1;

    /* Dataset */
    hsize_t adims[3] = {2, 54, 1};
    double array[2][54][1];

    /* Internal variables */
    int ierr, i, j;


    /* Fill-in dataset */
    for (i=0; i<2; i++) {
        for (j=0; j<54; j++) {
            array[i][j][0] = (double)(i * 16 + j);
        }
    }

    /* File structure */
    file_id = H5Fcreate(CHKFILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    root_id = H5Gopen(file_id, ".", H5P_DEFAULT);
    ierr = utils_hdf5_create_group(root_id, "densities", &group_id);
    ierr = utils_hdf5_create_group(group_id, "pseudo_density", &subgroup_id);

    /* Dimension types */
    ierr = utils_hdf5_write_attr(subgroup_id, "dimension_types", H5T_NATIVE_HSIZE, d3_1, 1, H5T_NATIVE_HSIZE, dt);

    /* Lattice vectors */
    ierr = utils_hdf5_write_attr(subgroup_id, "lattice_vectors", H5T_NATIVE_DOUBLE, d3_2, 2, H5T_NATIVE_DOUBLE, lv);

    /* Number of components */
    ierr = utils_hdf5_write_attr(subgroup_id, "number_of_components", H5T_NATIVE_HSIZE, d1_1, 1, H5T_NATIVE_HSIZE, &nc);

    /* Number of grid points */
    ierr = utils_hdf5_write_attr(subgroup_id, "number_of_grid_points", H5T_NATIVE_HSIZE, d3_1, 1, H5T_NATIVE_HSIZE, ng);

    /* Number of physical dimensions */
    ierr = utils_hdf5_write_attr(subgroup_id, "number_of_physical_dimensions", H5T_NATIVE_HSIZE, d1_1, 1, H5T_NATIVE_HSIZE, &np);

    /* Real or complex */
    ierr = utils_hdf5_write_attr(subgroup_id, "real_or_complex", H5T_NATIVE_HSIZE, d1_1, 1, H5T_NATIVE_HSIZE, &rc);

    /* Use default ordering */
    ierr = utils_hdf5_write_bool_old(subgroup_id, "use_default_ordering", true);

    /* Values on grid */
    ierr = utils_hdf5_create_dataset(subgroup_id, "values_on_grid", H5T_NATIVE_DOUBLE, adims, 3, &array_id);
    ierr = utils_hdf5_write_dataset(array_id, H5P_DEFAULT, &array, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);

    /* Close everything, since the purpose is to read the file */
    H5Gclose(subgroup_id);
    H5Gclose(group_id);
    H5Gclose(root_id);
    H5Fclose(file_id);
}

void grid_scalarfield_teardown(void)
{
    unlink(CHKFILE);
}

START_TEST(test_read_metadata)
{
    escdf_handle_t *file_id;
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    
    file_id = escdf_open(CHKFILE, NULL);
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
    escdf_direction_type dirarr[2];
    escdf_real_or_complex rc;
    unsigned int uarr[2], uval;
    double darr[4];
    
    scalarfield = escdf_grid_scalarfield_new(NULL);

    escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    dirarr[0] = ESCDF_DIRECTION_FREE;
    dirarr[1] = ESCDF_DIRECTION_SEMI_INFINITE;
    escdf_grid_scalarfield_set_dimension_types(scalarfield, dirarr, 2);
    darr[0] = 1.;
    darr[1] = 2.;
    darr[2] = 3.;
    darr[3] = 4.;
    escdf_grid_scalarfield_set_lattice_vectors(scalarfield, darr, 4);
    uarr[0] = 6;
    uarr[1] = 4;
    escdf_grid_scalarfield_set_number_of_grid_points(scalarfield, uarr, 2);
    escdf_grid_scalarfield_set_number_of_components(scalarfield, 2);
    escdf_grid_scalarfield_set_real_or_complex(scalarfield, ESCDF_REAL);
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
    
    err = escdf_grid_scalarfield_get_dimension_types(scalarfield, dirarr, 2);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(dirarr[0] == ESCDF_DIRECTION_FREE &&
              dirarr[1] == ESCDF_DIRECTION_SEMI_INFINITE);

    err = escdf_grid_scalarfield_get_lattice_vectors(scalarfield, darr, 4);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(darr[0] == 1. && darr[1] == 2. && darr[2] == 3. && darr[3] == 4.);

    err = escdf_grid_scalarfield_get_number_of_grid_points(scalarfield, uarr, 2);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(uarr[0] == 6 && uarr[1] == 4);

    uval = escdf_grid_scalarfield_get_number_of_components(scalarfield);
    ck_assert(uval == 2);
    
    rc = escdf_grid_scalarfield_get_real_or_complex(scalarfield);
    ck_assert(rc == ESCDF_REAL);
    
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
    escdf_direction_type dirarr[2];
    const escdf_direction_type *dirpt;
    escdf_real_or_complex rc;
    unsigned int uval, uarr[3];
    const unsigned int *upt;
    double darr[9];
    const double *dpt;
    bool bval;
    
    /* Create a new file using default properties. */
    file_id = escdf_open(CHKFILE, NULL);
    ck_assert(file_id != NULL);

    scalarfield = escdf_grid_scalarfield_new("densities/pseudo_density");
    err = escdf_grid_scalarfield_read_metadata(scalarfield, file_id);
    ck_assert(err == ESCDF_SUCCESS);

    escdf_close(file_id);

    uval = escdf_grid_scalarfield_get_number_of_physical_dimensions(scalarfield);
    ck_assert(uval == 3);
    
    err = escdf_grid_scalarfield_get_dimension_types(scalarfield, dirarr, 3);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(dirarr[0] == ESCDF_DIRECTION_FREE &&
              dirarr[1] == ESCDF_DIRECTION_PERIODIC &&
              dirarr[2] == ESCDF_DIRECTION_FREE);
    dirpt = escdf_grid_scalarfield_ptr_dimension_types(scalarfield);
    ck_assert(dirpt != NULL);
    ck_assert(dirpt[0] == ESCDF_DIRECTION_FREE &&
              dirpt[1] == ESCDF_DIRECTION_PERIODIC &&
              dirpt[2] == ESCDF_DIRECTION_FREE);

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
    
    rc = escdf_grid_scalarfield_get_real_or_complex(scalarfield);
    ck_assert(rc == ESCDF_REAL);
    
    bval = escdf_grid_scalarfield_get_use_default_ordering(scalarfield);
    ck_assert(bval);
    
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
    escdf_direction_type dirarr[3];
    const escdf_direction_type *dirpt;

    scalarfield = escdf_grid_scalarfield_new("density");
    err = escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    ck_assert(err == ESCDF_SUCCESS);
    
    dirarr[0] = ESCDF_DIRECTION_SEMI_INFINITE;
    dirarr[1] = ESCDF_DIRECTION_FREE;
    err = escdf_grid_scalarfield_set_dimension_types(scalarfield, dirarr, 2);
    ck_assert(err == ESCDF_SUCCESS);
    err = escdf_grid_scalarfield_get_dimension_types(scalarfield, dirarr, 2);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(dirarr[0] == ESCDF_DIRECTION_SEMI_INFINITE &&
              dirarr[1] == ESCDF_DIRECTION_FREE);
    dirpt = escdf_grid_scalarfield_ptr_dimension_types(scalarfield);
    ck_assert(dirpt != NULL);
    ck_assert(dirpt[0] == ESCDF_DIRECTION_SEMI_INFINITE &&
              dirpt[1] == ESCDF_DIRECTION_FREE);
    
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
    escdf_real_or_complex rc;

    scalarfield = escdf_grid_scalarfield_new("density");

    err = escdf_grid_scalarfield_set_real_or_complex(scalarfield, ESCDF_REAL);
    ck_assert(err == ESCDF_SUCCESS);
    rc = escdf_grid_scalarfield_get_real_or_complex(scalarfield);
    ck_assert(rc == ESCDF_REAL);
    
    escdf_grid_scalarfield_free(scalarfield);
}
END_TEST

START_TEST(test_set_use_default_ordering)
{
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    bool bval;

    scalarfield = escdf_grid_scalarfield_new("density");

    err = escdf_grid_scalarfield_set_use_default_ordering(scalarfield, false);
    ck_assert(err == ESCDF_SUCCESS);
    bval = escdf_grid_scalarfield_get_use_default_ordering(scalarfield);
    ck_assert(!bval);
    
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
    
    file_id = escdf_open(CHKFILE, NULL);
    ck_assert(file_id != NULL);

    scalarfield = escdf_grid_scalarfield_new("densities/pseudo_density");
    err = escdf_grid_scalarfield_read_metadata(scalarfield, file_id);
    ck_assert(err == ESCDF_SUCCESS);

    /* total reading. */
    err = escdf_grid_scalarfield_read_values_on_grid(scalarfield, file_id, dens, NULL, NULL, NULL);
    ck_assert(err == ESCDF_SUCCESS);
    for (i=0; i<2; i++) {
        for (j=0; j<54; j++) {
            ck_assert((dens[i*54+j] - (double)(i * 16 + j)) < 1.0e-15);
        }
    }

    /* Slice reading. */
    err = escdf_grid_scalarfield_read_values_on_grid(scalarfield, file_id, dens, start, count, NULL);
    ck_assert(err == ESCDF_SUCCESS);
    ck_assert(dens[0] == 2.0);
    ck_assert(dens[1] == 3.0);
    ck_assert(dens[2] == 18.0);
    ck_assert(dens[3] == 19.0);

    escdf_grid_scalarfield_free(scalarfield);

    escdf_close(file_id);
}
END_TEST

START_TEST(test_write_values_on_grid)
{
    escdf_handle_t *file_id;
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
    escdf_direction_type dirarr[2];
    unsigned int uarr[2], uval;
    double darr[4];

    double dens[48];
    hsize_t start[3] = {0, 2, 0};
    hsize_t count[3] = {2, 9, 1};
    unsigned int i, j;
    
    scalarfield = escdf_grid_scalarfield_new(NULL);

    escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    dirarr[0] = ESCDF_DIRECTION_FREE;
    dirarr[1] = ESCDF_DIRECTION_SEMI_INFINITE;
    escdf_grid_scalarfield_set_dimension_types(scalarfield, dirarr, 2);
    darr[0] = 1.;
    darr[1] = 2.;
    darr[2] = 3.;
    darr[3] = 4.;
    escdf_grid_scalarfield_set_lattice_vectors(scalarfield, darr, 4);
    uarr[0] = 6;
    uarr[1] = 4;
    escdf_grid_scalarfield_set_number_of_grid_points(scalarfield, uarr, 2);
    escdf_grid_scalarfield_set_number_of_components(scalarfield, 2);
    escdf_grid_scalarfield_set_real_or_complex(scalarfield, ESCDF_REAL);
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
    escdf_direction_type dirarr[2];
    unsigned int uarr[2], uval;
    double darr[4];

    double dens[48];
    unsigned int *tbl;
    unsigned int i, j;
    
    /* Generate a density on disk with default ordering. */
    scalarfield = escdf_grid_scalarfield_new(NULL);

    escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, 2);
    dirarr[0] = ESCDF_DIRECTION_FREE;
    dirarr[1] = ESCDF_DIRECTION_SEMI_INFINITE;
    escdf_grid_scalarfield_set_dimension_types(scalarfield, dirarr, 2);
    darr[0] = 1.;
    darr[1] = 2.;
    darr[2] = 3.;
    darr[3] = 4.;
    escdf_grid_scalarfield_set_lattice_vectors(scalarfield, darr, 4);
    uarr[0] = 6;
    uarr[1] = 4;
    escdf_grid_scalarfield_set_number_of_grid_points(scalarfield, uarr, 2);
    escdf_grid_scalarfield_set_number_of_components(scalarfield, 2);
    escdf_grid_scalarfield_set_real_or_complex(scalarfield, ESCDF_REAL);
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
    tcase_add_checked_fixture(tc_info, grid_scalarfield_setup, grid_scalarfield_teardown);
    suite_add_tcase(s, tc_info);

    return s;
}
