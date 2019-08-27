/* Copyright (C) 2018 Micael Oliveira <micael.oliveira@mpsd.mpg.de>
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

#include <check.h>
#include <unistd.h>
#include <stdbool.h>

#include "escdf_attributes.h"
#include "escdf_datasets.h"
#include "utils_hdf5.h"
#include "escdf_handle.h"


#define FILE_R "check_dataset_test_file_r.h5"
#define FILE_W "check_dataset_test_file_w.h5"


#define NONE            0
#define DIM0            1
#define DIM1            2
#define DIM2            3
#define ARRAY1_UINT     4
#define ARRAY1_INT      5
#define ARRAY1_DOUBLE   6
#define ARRAY1_STRING   7
#define ARRAY2_UINT     8
#define ARRAY2_INT      9
#define ARRAY2_DOUBLE  10
#define ARRAY2_STRING  11


static const escdf_attribute_specs_t specs_dim0 = {
    DIM0, "dim0", ESCDF_DT_UINT, 0, 0, NULL
};

static const escdf_attribute_specs_t *array1_dims[] = {&specs_dim0};

static const escdf_attribute_specs_t specs_dim1 = {
    DIM1, "dim1", ESCDF_DT_UINT, 0, 0, NULL
};

static const escdf_attribute_specs_t specs_dim2 = {
    DIM2, "dim2", ESCDF_DT_UINT, 0, 0, NULL
};

static const escdf_attribute_specs_t *array2_dims[] = {&specs_dim1, &specs_dim2};

static const escdf_dataset_specs_t specs_none = {
    NONE, "none", ESCDF_DT_NONE, 0, 0, false, false, NULL
};

static const escdf_dataset_specs_t specs_array1_uint = {
    ARRAY1_UINT, "array1_uint", ESCDF_DT_UINT, 0, 1, true, false, array1_dims
};

static const escdf_dataset_specs_t specs_array1_int = {
    ARRAY1_INT, "array1_int", ESCDF_DT_INT, 0, 1, true, false, array1_dims
};

static const escdf_dataset_specs_t specs_array1_double = {
    ARRAY1_DOUBLE, "array1_double", ESCDF_DT_DOUBLE, 0, 1, true, false, array1_dims
};

static const escdf_dataset_specs_t specs_array1_string = {
    ARRAY1_STRING, "array1_string", ESCDF_DT_STRING, 30, 1, true, false, array1_dims
};

static const escdf_dataset_specs_t specs_array2_uint = {
    ARRAY2_UINT, "array2_uint", ESCDF_DT_UINT, 0, 2, false, true, array2_dims
};

static const escdf_dataset_specs_t specs_array2_int = {
    ARRAY2_INT, "array2_int", ESCDF_DT_INT, 0, 2, false, true, array2_dims
};

static const escdf_dataset_specs_t specs_array2_double = {
    ARRAY2_DOUBLE, "array2_double", ESCDF_DT_DOUBLE, 0, 2, false, true, array2_dims
};

static const escdf_dataset_specs_t specs_array2_string = {
    ARRAY2_STRING, "array2_string", ESCDF_DT_STRING, 30, 2, false, true, array2_dims
};

static hid_t string_len_30;
static size_t dims0[] = {4};
static unsigned int array1_uint[4] = {0, 1, 2, 3};
static int array1_int[4] = {-1, 1,  2, -3};
static double array1_double[4] = {0.00, 0.00, 0.25, 0.50};
static char array1_string[4][30] = {"aa1", "aa2", "aa3", "aa4"};

static size_t dims1[] = {2, 3};
static unsigned int array2_uint[2][3] = {{1, 2, 3},
                                        {4, 5, 6}};
static int array2_int[2][3] = {{ 1,  2, -3},
                              {-4, -5,  6}};
static double array2_double[2][3] = {{0.00, 0.00, 0.00},
                                    {0.25, 0.25, 0.25}};
static char array2_string[2][3][30] = {{"element1", "element2", "element3"},
                                      {"string", "another string", "yet another string"}};

static escdf_handle_t *handle_r = NULL, *handle_w = NULL;
static escdf_attribute_t *dtset1_dims[1] = {NULL}, *dtset2_dims[2] = {NULL, NULL};
static escdf_dataset_t *dtset = NULL;


/******************************************************************************
 * Setup and teardown                                                         *
 ******************************************************************************/


static void file_setup(void)
{
    hid_t file_id, root_id, dtset_id;

    /* create file */
    file_id = H5Fcreate(FILE_R, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    root_id = H5Gopen(file_id, ".", H5P_DEFAULT);

    /* write attributes */
    utils_hdf5_write_attr(root_id, specs_dim0.name, H5T_NATIVE_HSIZE, NULL, 0, H5T_NATIVE_UINT, &dims0[0]);
    utils_hdf5_write_attr(root_id, specs_dim1.name, H5T_NATIVE_HSIZE, NULL, 0, H5T_NATIVE_UINT, &dims1[0]);
    utils_hdf5_write_attr(root_id, specs_dim2.name, H5T_NATIVE_HSIZE, NULL, 0, H5T_NATIVE_UINT, &dims1[1]);

    /* write datasets */
    string_len_30 = H5Tcopy(H5T_C_S1);
    H5Tset_size(string_len_30, 30);
    H5Tset_strpad(string_len_30, H5T_STR_NULLTERM);

    utils_hdf5_create_dataset(root_id, specs_array1_uint.name, H5T_NATIVE_UINT, dims0, 1, &dtset_id);
    utils_hdf5_write_dataset(dtset_id, H5P_DEFAULT, &array1_uint, H5T_NATIVE_UINT, NULL, NULL, NULL);
    utils_hdf5_create_dataset(root_id, specs_array1_int.name, H5T_NATIVE_INT, dims0, 1, &dtset_id);
    utils_hdf5_write_dataset(dtset_id, H5P_DEFAULT, &array1_int, H5T_NATIVE_INT, NULL, NULL, NULL);
    utils_hdf5_create_dataset(root_id, specs_array1_double.name, H5T_NATIVE_DOUBLE, dims0, 1, &dtset_id);
    utils_hdf5_write_dataset(dtset_id, H5P_DEFAULT, &array1_double, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);
    utils_hdf5_create_dataset(root_id, specs_array1_string.name, string_len_30, dims0, 1, &dtset_id);
    utils_hdf5_write_dataset(dtset_id, H5P_DEFAULT, &array1_string, string_len_30, NULL, NULL, NULL);

    utils_hdf5_create_dataset(root_id, specs_array2_uint.name, H5T_NATIVE_UINT, dims1, 2, &dtset_id);
    utils_hdf5_write_dataset(dtset_id, H5P_DEFAULT, &array2_uint, H5T_NATIVE_UINT, NULL, NULL, NULL);
    utils_hdf5_create_dataset(root_id, specs_array2_int.name, H5T_NATIVE_INT, dims1, 2, &dtset_id);
    utils_hdf5_write_dataset(dtset_id, H5P_DEFAULT, &array2_int, H5T_NATIVE_INT, NULL, NULL, NULL);
    utils_hdf5_create_dataset(root_id, specs_array2_double.name, H5T_NATIVE_DOUBLE, dims1, 2, &dtset_id);
    utils_hdf5_write_dataset(dtset_id, H5P_DEFAULT, &array2_double, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);
    utils_hdf5_create_dataset(root_id, specs_array2_string.name, string_len_30, dims1, 2, &dtset_id);
    utils_hdf5_write_dataset(dtset_id, H5P_DEFAULT, &array2_string, string_len_30, NULL, NULL, NULL);


    /* close file */
    H5Gclose(root_id);
    H5Fclose(file_id);

    /* create handles */
    handle_r = escdf_open(FILE_R, NULL);
    handle_w = escdf_create(FILE_W, NULL);
}

static void file_teardown(void)
{
    H5Tclose(string_len_30);
    escdf_close(handle_r);
    escdf_close(handle_w);
    unlink(FILE_R);
    unlink(FILE_W);
}

void array1_setup(void)
{
    file_setup();
    dtset1_dims[0] = escdf_attribute_new(&specs_dim0, NULL);
    escdf_attribute_set(dtset1_dims[0], &dims0[0]);
}

void array1_teardown(void)
{
    file_teardown();
    escdf_attribute_free(dtset1_dims[0]);

    dtset = NULL;
    dtset1_dims[0] = NULL;
}

void array2_setup(void)
{
    file_setup();
    dtset2_dims[0] = escdf_attribute_new(&specs_dim1, NULL);
    dtset2_dims[1] = escdf_attribute_new(&specs_dim2, NULL);
    escdf_attribute_set(dtset2_dims[0], &dims1[0]);
    escdf_attribute_set(dtset2_dims[1], &dims1[1]);
}

void array2_teardown(void)
{
    file_teardown();
    escdf_attribute_free(dtset2_dims[0]);
    escdf_attribute_free(dtset2_dims[1]);

    dtset = NULL;
    dtset2_dims[0] = NULL;
    dtset2_dims[1] = NULL;
}


/******************************************************************************
 * Tests                                                                      *
 ******************************************************************************/

START_TEST(test_dataset_specs_sizeof_none)
{
    ck_assert(escdf_dataset_specs_sizeof(&specs_none) == 0);
}
END_TEST

START_TEST(test_dataset_specs_sizeof_uint)
{
    ck_assert(escdf_dataset_specs_sizeof(&specs_array1_uint) == sizeof(unsigned int));
}
END_TEST

START_TEST(test_dataset_specs_sizeof_int)
{
    ck_assert(escdf_dataset_specs_sizeof(&specs_array1_int) == sizeof(int));
}
END_TEST

START_TEST(test_dataset_specs_sizeof_double)
{
    ck_assert(escdf_dataset_specs_sizeof(&specs_array1_double) == sizeof(double));
}
END_TEST

START_TEST(test_dataset_specs_sizeof_string)
{
    ck_assert(escdf_dataset_specs_sizeof(&specs_array1_string) == specs_array1_string.stringlength * sizeof(char));
}
END_TEST


START_TEST(test_dataset_specs_is_present_true)
{
    ck_assert(escdf_dataset_specs_is_present(&specs_array1_uint, handle_r->group_id) == true);
}
END_TEST

START_TEST(test_dataset_specs_is_present_false)
{
    ck_assert(escdf_dataset_specs_is_present(&specs_none, handle_r->group_id) == false);
}
END_TEST


START_TEST(test_dataset_specs_is_compact_true)
{
    ck_assert(escdf_dataset_specs_is_compact(&specs_array2_uint) == true);
}
END_TEST

START_TEST(test_dataset_specs_is_compact_false)
{
    ck_assert(escdf_dataset_specs_is_compact(&specs_array1_uint) == false);
}
END_TEST


START_TEST(test_dataset_new_uint_array1)
{
    ck_assert( (dtset = escdf_dataset_new(&specs_array1_uint, dtset1_dims)) != NULL);
}
END_TEST

START_TEST(test_dataset_new_int_array1)
{
    ck_assert( (dtset = escdf_dataset_new(&specs_array1_int, dtset1_dims)) != NULL);
}
END_TEST

START_TEST(test_dataset_new_double_array1)
{
    ck_assert( (dtset = escdf_dataset_new(&specs_array1_double, dtset1_dims)) != NULL);
}
END_TEST

START_TEST(test_dataset_new_string_array1)
{
    ck_assert( (dtset = escdf_dataset_new(&specs_array1_string, dtset1_dims)) != NULL);
}
END_TEST


START_TEST(test_dataset_new_uint_array2)
{
    ck_assert( (dtset = escdf_dataset_new(&specs_array2_uint, dtset2_dims)) != NULL);
}
END_TEST

START_TEST(test_dataset_new_int_array2)
{
    ck_assert( (dtset = escdf_dataset_new(&specs_array2_int, dtset2_dims)) != NULL);
}
END_TEST

START_TEST(test_dataset_new_double_array2)
{
    ck_assert( (dtset = escdf_dataset_new(&specs_array2_double, dtset2_dims)) != NULL);
}
END_TEST

START_TEST(test_dataset_new_string_array2)
{
    ck_assert( (dtset = escdf_dataset_new(&specs_array2_string, dtset2_dims)) != NULL);
}
END_TEST


Suite * make_datasets_suite(void)
{
    Suite *s;
    TCase *tc_dataset_specs_sizeof, *tc_dataset_specs_is_present, *tc_dataset_specs_disordered_storage_allowed,
	  *tc_dataset_specs_is_compact, *tc_dataset_new_1d, *tc_dataset_new_2d;
    
    s = suite_create("Datasets");

    tc_dataset_specs_sizeof = tcase_create("Dataset size of");
    tcase_add_checked_fixture(tc_dataset_specs_sizeof, NULL, NULL);
    tcase_add_test(tc_dataset_specs_sizeof, test_dataset_specs_sizeof_none);
    tcase_add_test(tc_dataset_specs_sizeof, test_dataset_specs_sizeof_uint);
    tcase_add_test(tc_dataset_specs_sizeof, test_dataset_specs_sizeof_int);
    tcase_add_test(tc_dataset_specs_sizeof, test_dataset_specs_sizeof_double);
    tcase_add_test(tc_dataset_specs_sizeof, test_dataset_specs_sizeof_string);
    suite_add_tcase(s, tc_dataset_specs_sizeof);

    tc_dataset_specs_is_present = tcase_create("Dataset is present");
    tcase_add_checked_fixture(tc_dataset_specs_is_present, file_setup, file_teardown);
    tcase_add_test(tc_dataset_specs_is_present, test_dataset_specs_is_present_true);
    tcase_add_test(tc_dataset_specs_is_present, test_dataset_specs_is_present_false);
    suite_add_tcase(s, tc_dataset_specs_is_present);

    tc_dataset_specs_is_compact = tcase_create("Dataset is compact");
    tcase_add_checked_fixture(tc_dataset_specs_is_compact, NULL, NULL);
    tcase_add_test(tc_dataset_specs_is_compact, test_dataset_specs_is_compact_true);
    tcase_add_test(tc_dataset_specs_is_compact, test_dataset_specs_is_compact_false);
    suite_add_tcase(s, tc_dataset_specs_is_compact);

    tc_dataset_new_1d = tcase_create("Dataset new 1D array");
    tcase_add_checked_fixture(tc_dataset_new_1d, array1_setup, array1_teardown);
    tcase_add_test(tc_dataset_new_1d, test_dataset_new_uint_array1);
    tcase_add_test(tc_dataset_new_1d, test_dataset_new_int_array1);
    tcase_add_test(tc_dataset_new_1d, test_dataset_new_double_array1);
    tcase_add_test(tc_dataset_new_1d, test_dataset_new_string_array1);
    suite_add_tcase(s, tc_dataset_new_1d);

    tc_dataset_new_2d = tcase_create("Dataset new 2D array");
    tcase_add_checked_fixture(tc_dataset_new_2d, array2_setup, array2_teardown);
    tcase_add_test(tc_dataset_new_2d, test_dataset_new_uint_array2);
    tcase_add_test(tc_dataset_new_2d, test_dataset_new_int_array2);
    tcase_add_test(tc_dataset_new_2d, test_dataset_new_double_array2);
    tcase_add_test(tc_dataset_new_2d, test_dataset_new_string_array2);
    suite_add_tcase(s, tc_dataset_new_2d);

    return s;
}
