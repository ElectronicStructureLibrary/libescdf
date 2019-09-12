/* Copyright (C) 2017 Micael Oliveira <micael.oliveira@mpsd.mpg.de>
 *                    Yann Pouillon <devops@materialsevolution.es>
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

#include "utils_hdf5.h"

#define CHKFILE "check_utils_hdf5_test_file.h5"
#define GROUP "mygroup"
#define SUBGROUP "mysubgroup"
#define ATTRIBUTE_BOOL_S   "myboolscalarattribute"
#define ATTRIBUTE_UINT_S   "myuintscalarattribute"
#define ATTRIBUTE_INT_S    "myintscalarattribute"
#define ATTRIBUTE_DBL_S    "mydblscalarattribute"
#define ATTRIBUTE_STRING_S "mystringscalarattribute"
#define ATTRIBUTE_BOOL_A   "myboolarrayattribute"
#define ATTRIBUTE_UINT_A "myuintarrayattribute"
#define ATTRIBUTE_INT_A  "myintarrayattribute"
#define ATTRIBUTE_DBL_A  "mydblarrayattribute"
#define ATTRIBUTE_STRING_A  "mystringarrayattribute"
#define DATASET "mydataset"

static hid_t file_id, root_id;
static hid_t group_id, subgroup_id;
static hid_t space_scalar_id, space_array_id, space_null_id;
static hid_t string_len_4, string_len_20;
static hid_t attribute_bool_scalar_id, attribute_uint_scalar_id, attribute_int_scalar_id, attribute_dbl_scalar_id, attribute_string_scalar_id;
static hid_t attribute_bool_array_id, attribute_uint_array_id, attribute_int_array_id, attribute_dbl_array_id, attribute_string_array_id;
static hid_t dataset_id;

static bool bool_scalar = 0;
static unsigned int uint_scalar = 1;
static int int_scalar = -1;
static double dbl_scalar = 10.0;
static char string_scalar[] = "this is a string";

static size_t dims[2] = {3, 2};
static bool bool_array[3][2] = {{false, true},
                                {true, false},
                                {false, false}};
static unsigned int uint_array[3][2] = {{1, 2},
                                        {3, 4},
                                        {5, 6}};
static int int_array[3][2] = {{-1, 2},
                              {-3, 4},
                              {-5, 6}};
static double dbl_array[3][2] = {{1.0, 2.0},
                                 {3.0, 4.0},
                                 {5.0, 6.0}};
static char string_array[3][2][20] = {{"foo",    "bar"},
                                      {"foobar", "barfoo"},
                                      {"foofoo", "barbar"}};

void utils_hdf5_setup(void)
{
    static char bool_str_scalar[4];
    char bool_str_array[3][2][4];
    int i, j;
    hsize_t dims_[2] = {3, 2};

    if (bool_scalar)
        strcpy(bool_str_scalar, "yes");
    else
        strcpy(bool_str_scalar, "no");

    for (i=0; i<3; i++) {
        for (j=0; j<2; j++) {
            if (bool_array[i][j])
                strcpy(bool_str_array[i][j], "yes");
            else
                strcpy(bool_str_array[i][j], "no");
        }
    }

    file_id = H5Fcreate(CHKFILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    root_id = H5Gopen(file_id, ".", H5P_DEFAULT);    
    group_id = H5Gcreate(root_id, GROUP, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);    
    subgroup_id = H5Gcreate(group_id, SUBGROUP, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    space_scalar_id = H5Screate(H5S_SCALAR);
    space_array_id = H5Screate_simple(2, dims_, NULL);
    space_null_id = H5Screate(H5S_NULL);

    string_len_4 = H5Tcopy(H5T_C_S1);
    H5Tset_size(string_len_4, 4);
    H5Tset_strpad(string_len_4, H5T_STR_NULLTERM);

    string_len_20 = H5Tcopy(H5T_C_S1);
    H5Tset_size(string_len_20, 20);
    H5Tset_strpad(string_len_20, H5T_STR_NULLTERM);

    attribute_bool_scalar_id = H5Acreate(group_id, ATTRIBUTE_BOOL_S, string_len_4, space_scalar_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_bool_scalar_id, string_len_4, &bool_str_scalar);
    attribute_uint_scalar_id = H5Acreate(group_id, ATTRIBUTE_UINT_S, H5T_NATIVE_UINT, space_scalar_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_uint_scalar_id, H5T_NATIVE_UINT, &uint_scalar);
    attribute_int_scalar_id = H5Acreate(group_id, ATTRIBUTE_INT_S, H5T_NATIVE_INT, space_scalar_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_int_scalar_id, H5T_NATIVE_INT, &int_scalar);
    attribute_dbl_scalar_id = H5Acreate(group_id, ATTRIBUTE_DBL_S, H5T_NATIVE_DOUBLE, space_scalar_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_dbl_scalar_id, H5T_NATIVE_DOUBLE, &dbl_scalar);
    attribute_string_scalar_id = H5Acreate(group_id, ATTRIBUTE_STRING_S, string_len_20, space_scalar_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_string_scalar_id, string_len_20, &string_scalar);

    attribute_bool_array_id = H5Acreate(group_id, ATTRIBUTE_BOOL_A, string_len_4, space_array_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_bool_array_id, string_len_4, &bool_str_array);
    attribute_uint_array_id = H5Acreate(group_id, ATTRIBUTE_UINT_A, H5T_NATIVE_UINT, space_array_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_uint_array_id, H5T_NATIVE_UINT, &uint_array);
    attribute_int_array_id = H5Acreate(group_id, ATTRIBUTE_INT_A, H5T_NATIVE_INT, space_array_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_int_array_id, H5T_NATIVE_INT, &int_array);
    attribute_dbl_array_id = H5Acreate(group_id, ATTRIBUTE_DBL_A, H5T_NATIVE_DOUBLE, space_array_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_dbl_array_id, H5T_NATIVE_DOUBLE, &dbl_array);
    attribute_string_array_id = H5Acreate(group_id, ATTRIBUTE_STRING_A, string_len_20, space_array_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attribute_string_array_id, string_len_20, &string_array);

    dataset_id = H5Dcreate(group_id, DATASET, H5T_NATIVE_DOUBLE, space_array_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, space_array_id, space_array_id, H5P_DEFAULT, &dbl_array);
}

void utils_hdf5_teardown(void)
{
    H5Tclose(string_len_4);
    H5Tclose(string_len_20);
    H5Dclose(dataset_id);
    H5Aclose(attribute_bool_scalar_id);
    H5Aclose(attribute_uint_scalar_id);
    H5Aclose(attribute_int_scalar_id);
    H5Aclose(attribute_dbl_scalar_id);
    H5Aclose(attribute_string_scalar_id);
    H5Aclose(attribute_bool_array_id);
    H5Aclose(attribute_uint_array_id);
    H5Aclose(attribute_int_array_id);
    H5Aclose(attribute_dbl_array_id);
    H5Aclose(attribute_string_array_id);
    H5Sclose(space_scalar_id);
    H5Sclose(space_array_id);
    H5Sclose(space_null_id);
    H5Gclose(group_id);
    H5Gclose(subgroup_id);
    H5Gclose(root_id);
    H5Fclose(file_id);
    unlink(CHKFILE);
}

/* check_present */
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

/* check_present_recursive */
START_TEST(test_utils_hdf5_check_present_recursive_true)
{
   ck_assert(utils_hdf5_check_present_recursive(root_id, GROUP"/"SUBGROUP));
}
END_TEST

START_TEST(test_utils_hdf5_check_present_recursive_false)
{
    ck_assert(!utils_hdf5_check_present_recursive(root_id, GROUP"/not_present"));
}
END_TEST

/* check_present_attr */
START_TEST(test_utils_hdf5_check_present_attr_true)
{
    ck_assert(utils_hdf5_check_present_attr(group_id, ATTRIBUTE_DBL_S));
}
END_TEST

START_TEST(test_utils_hdf5_check_present_attr_false)
{
    ck_assert(!utils_hdf5_check_present_attr(group_id, "not_present"));
}
END_TEST

/* check_shape */
START_TEST(test_utils_hdf5_check_shape_scalar_correct)
{
    ck_assert(utils_hdf5_check_shape(space_scalar_id, NULL, 0) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_check_shape_array_correct)
{
    size_t dims[2] = {3, 2};

    ck_assert(utils_hdf5_check_shape(space_array_id, dims, 2) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_check_shape_scalar_wrong_args)
{
    size_t dims[1] = {2};

    ck_assert(utils_hdf5_check_shape(space_scalar_id, dims, 1) == ESCDF_ERROR);
}
END_TEST

START_TEST(test_utils_hdf5_check_shape_array_wrong_args)
{
    size_t dims[1] = {2};

    ck_assert(utils_hdf5_check_shape(space_array_id, dims, 1) == ESCDF_ERROR_DIM);

}
END_TEST

START_TEST(test_utils_hdf5_check_shape_no_class)
{
    ck_assert(utils_hdf5_check_shape(group_id, NULL, 0) == ESCDF_ERROR_ARGS);
}
END_TEST

START_TEST(test_utils_hdf5_check_shape_wrong_class)
{
    ck_assert(utils_hdf5_check_shape(space_null_id, NULL, 0) == ESCDF_ERROR_DIM);
}
END_TEST

/* check_attr */
START_TEST(test_utils_hdf5_check_attr_scalar)
{
    ck_assert(utils_hdf5_check_attr(group_id, ATTRIBUTE_DBL_S, NULL, 0, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_check_attr_array)
{
    ck_assert(utils_hdf5_check_attr(group_id, ATTRIBUTE_DBL_A, dims, 2, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_check_attr_array_wrong)
{
    size_t wrong_dims[2] = {1, 3};

    ck_assert(utils_hdf5_check_attr(group_id, ATTRIBUTE_DBL_A, wrong_dims, 2, NULL) == ESCDF_ERROR);
}
END_TEST

START_TEST(test_utils_hdf5_check_attr_ptr)
{
    hid_t attr_id = 0;

    ck_assert(utils_hdf5_check_attr(group_id, ATTRIBUTE_DBL_S, NULL, 0, &attr_id) == ESCDF_SUCCESS);
    ck_assert(attr_id != 0);
    H5Aclose(attr_id);
}
END_TEST

/* check_dataset */
START_TEST(test_utils_hdf5_check_dataset)
{
    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_check_dataset_wrong)
{
    size_t wrong_dims[2] = {1, 3};

    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, wrong_dims, 2, NULL) == ESCDF_ERROR);
}
END_TEST

START_TEST(test_utils_hdf5_check_dataset_ptr)
{
    hid_t dtset_id = 0;

    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(dtset_id != 0);
    H5Dclose(dtset_id);
}
END_TEST

/* read_attr */
START_TEST(test_utils_hdf5_read_attr_scalar)
{
    double value = 0.0;

    ck_assert(utils_hdf5_read_attr(group_id, ATTRIBUTE_DBL_S, H5T_NATIVE_DOUBLE, NULL, 0, &value) == ESCDF_SUCCESS);
    ck_assert(value == dbl_scalar);
}
END_TEST

START_TEST(test_utils_hdf5_read_attr_string_scalar)
{
    char string[20] = "";

    ck_assert(utils_hdf5_read_attr_string(group_id, ATTRIBUTE_STRING_S, 20, NULL, 0, string) == ESCDF_SUCCESS);
    ck_assert_str_eq(string, string_scalar);
}
END_TEST

START_TEST(test_utils_hdf5_read_attr_bool_scalar)
{
    bool value = !bool_scalar;

    ck_assert(utils_hdf5_read_attr_bool(group_id, ATTRIBUTE_BOOL_S, NULL, 0, &value) == ESCDF_SUCCESS);
    ck_assert(value == bool_scalar);
}
END_TEST

START_TEST(test_utils_hdf5_read_uint)
{
    _uint_set_t var;
    unsigned int range[] = {0,10};

    ck_assert(utils_hdf5_read_uint(group_id, ATTRIBUTE_UINT_S, &var, range) == ESCDF_SUCCESS);
    ck_assert(var.is_set);
    ck_assert(var.value == uint_scalar);
}
END_TEST

START_TEST(test_utils_hdf5_read_int)
{
    _int_set_t var;
    int range[] = {-10,10};

    ck_assert(utils_hdf5_read_int(group_id, ATTRIBUTE_INT_S, &var, range) == ESCDF_SUCCESS);
    ck_assert(var.is_set);
    ck_assert(var.value == int_scalar);
}
END_TEST

START_TEST(test_utils_hdf5_read_attr_array)
{
    double values[3][2] = {{0.0, 0.0},
                           {0.0, 0.0},
                           {0.0, 0.0}};

    ck_assert(utils_hdf5_read_attr(group_id, ATTRIBUTE_DBL_A, H5T_NATIVE_DOUBLE, dims, 2, &values) == ESCDF_SUCCESS);
    ck_assert(values[0][0] == dbl_array[0][0]);
    ck_assert(values[0][1] == dbl_array[0][1]);
    ck_assert(values[1][0] == dbl_array[1][0]);
    ck_assert(values[1][1] == dbl_array[1][1]);
    ck_assert(values[2][0] == dbl_array[2][0]);
    ck_assert(values[2][1] == dbl_array[2][1]);
}
END_TEST

START_TEST(test_utils_hdf5_read_attr_string_array)
{
    char values[3][2][20] = {{"", ""},
                             {"", ""},
                             {"", ""}};

    ck_assert(utils_hdf5_read_attr_string(group_id, ATTRIBUTE_STRING_A, 20, dims, 2, &values) == ESCDF_SUCCESS);
    ck_assert_str_eq(values[0][0], string_array[0][0]);
    ck_assert_str_eq(values[0][1], string_array[0][1]);
    ck_assert_str_eq(values[1][0], string_array[1][0]);
    ck_assert_str_eq(values[1][1], string_array[1][1]);
    ck_assert_str_eq(values[2][0], string_array[2][0]);
    ck_assert_str_eq(values[2][1], string_array[2][1]);
}
END_TEST

START_TEST(test_utils_hdf5_read_attr_bool_array)
{
    bool values[3][2]= {{true,  false},
                        {false, true},
                        {true,  true}};

    ck_assert(utils_hdf5_read_attr_bool(group_id, ATTRIBUTE_BOOL_A, dims, 2, &values) == ESCDF_SUCCESS);
    ck_assert(values[0][0] == bool_array[0][0]);
    ck_assert(values[0][1] == bool_array[0][1]);
    ck_assert(values[1][0] == bool_array[1][0]);
    ck_assert(values[1][1] == bool_array[1][1]);
    ck_assert(values[2][0] == bool_array[2][0]);
    ck_assert(values[2][1] == bool_array[2][1]);
}
END_TEST

START_TEST(test_utils_hdf5_read_uint_array)
{
    unsigned int *values;
    unsigned int range[] = {0, 6};

    ck_assert(utils_hdf5_read_uint_array(group_id, ATTRIBUTE_UINT_A, &values, dims, 2, range) == ESCDF_SUCCESS);
    ck_assert(values[0] == 1);
    ck_assert(values[1] == 2);
    ck_assert(values[2] == 3);
    ck_assert(values[3] == 4);
    ck_assert(values[4] == 5);
    ck_assert(values[5] == 6);
}
END_TEST

START_TEST(test_utils_hdf5_read_int_array)
{
    int *values;
    int range[] = {-6, 6};

    ck_assert(utils_hdf5_read_int_array(group_id, ATTRIBUTE_INT_A, &values, dims, 2, range) == ESCDF_SUCCESS);
    ck_assert(values[0] == -1);
    ck_assert(values[1] ==  2);
    ck_assert(values[2] == -3);
    ck_assert(values[3] ==  4);
    ck_assert(values[4] == -5);
    ck_assert(values[5] ==  6);
}
END_TEST

START_TEST(test_utils_hdf5_read_dbl_array)
{
    double *values;
    double range[] = {0.0, 7.0};

    ck_assert(utils_hdf5_read_dbl_array(group_id, ATTRIBUTE_DBL_A, &values, dims, 2, range) == ESCDF_SUCCESS);
    ck_assert(values[0] == 1.0);
    ck_assert(values[1] == 2.0);
    ck_assert(values[2] == 3.0);
    ck_assert(values[3] == 4.0);
    ck_assert(values[4] == 5.0);
    ck_assert(values[5] == 6.0);
}
END_TEST


/* read_dataset */
START_TEST(test_utils_hdf5_read_dataset)
{
    hid_t dtset_id = 0;
    double values[3][2];

    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_dataset(dtset_id, H5P_DEFAULT, &values, H5T_NATIVE_DOUBLE, NULL, NULL, NULL) == ESCDF_SUCCESS);
    ck_assert(values[0][0] == 1.0);
    ck_assert(values[0][1] == 2.0);
    ck_assert(values[1][0] == 3.0);
    ck_assert(values[1][1] == 4.0);
    ck_assert(values[2][0] == 5.0);
    ck_assert(values[2][1] == 6.0);
    H5Dclose(dtset_id);
}
END_TEST

START_TEST(test_utils_hdf5_read_dataset_sliced)
{
    hid_t dtset_id = 0;
    double values[3];
    size_t start[2] = {0, 0};
    size_t count[2] = {3, 1};

    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_dataset(dtset_id, H5P_DEFAULT, &values, H5T_NATIVE_DOUBLE, start, count, NULL) == ESCDF_SUCCESS);
    ck_assert(values[0] == 1.0);
    ck_assert(values[1] == 3.0);
    ck_assert(values[2] == 5.0);
    H5Dclose(dtset_id);
}
END_TEST

/* read_dataset_at */
START_TEST(test_utils_hdf5_read_dataset_at)
{

    escdf_errno_t error;
    hid_t dtset_id = 0;
    unsigned int number_of_points = 6;
    size_t coordinates[12] = {0, 0,
				    1, 0,
				    2, 0,
				    0, 1,
				    1, 1,
				    2, 1};
    double values[6];

    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_dataset_at(dtset_id, H5P_DEFAULT, &values, H5T_NATIVE_DOUBLE, number_of_points, coordinates) == ESCDF_SUCCESS);
    
    ck_assert(values[0] == 1.0);
    ck_assert(values[1] == 3.0);
    ck_assert(values[2] == 5.0);
    ck_assert(values[3] == 2.0);
    ck_assert(values[4] == 4.0);
    ck_assert(values[5] == 6.0);
    
    H5Dclose(dtset_id);
}
END_TEST

START_TEST(test_utils_hdf5_read_dataset_at_empty)
{
    hid_t dtset_id = 0;
    unsigned int number_of_points = 0;
    double *values = NULL;
    size_t *coordinates = NULL;

    ck_assert(utils_hdf5_check_dataset(group_id, DATASET, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_dataset_at(dtset_id, H5P_DEFAULT, &values, H5T_NATIVE_DOUBLE, number_of_points, coordinates) == ESCDF_SUCCESS);
    ck_assert_ptr_eq(values, NULL);
    H5Dclose(dtset_id);
}
END_TEST

/* create_group */
START_TEST(test_utils_hdf5_create_group)
{
    ck_assert(utils_hdf5_create_group(file_id, "somegroup", NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_create_group_ptr)
{
    hid_t tmp_id = 0;

    ck_assert(utils_hdf5_create_group(file_id, "somegroup", &tmp_id) == ESCDF_SUCCESS);
    ck_assert(tmp_id != 0);
    H5Gclose(tmp_id);
}END_TEST

START_TEST(test_utils_hdf5_create_group_overwrite)
{
    ck_assert(utils_hdf5_create_group(file_id, GROUP, NULL) == ESCDF_ERROR_ARGS);
}
END_TEST

START_TEST(test_utils_hdf5_create_group_existing_path)
{
    ck_assert(utils_hdf5_create_group(file_id, "mygroup/somegroup", NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_create_group_non_existing_path)
{
    ck_assert(utils_hdf5_create_group(file_id, "somegroup1/somegroup2", NULL) == ESCDF_SUCCESS);
}
END_TEST

/* create_attribute */
START_TEST(test_utils_hdf5_create_attribute_scalar)
{
    ck_assert(utils_hdf5_create_attr(group_id, "someattribute", H5T_NATIVE_DOUBLE, NULL, 0, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_create_attribute_array)
{
    ck_assert(utils_hdf5_create_attr(group_id, "someattribute", H5T_NATIVE_DOUBLE, dims, 2, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_create_attribute_ptr)
{
    hid_t attr_id = 0;

    ck_assert(utils_hdf5_create_attr(group_id, "someattribute", H5T_NATIVE_DOUBLE, NULL, 0, &attr_id) == ESCDF_SUCCESS);
    ck_assert(attr_id != 0);
    H5Aclose(attr_id);
}
END_TEST

START_TEST(test_utils_hdf5_create_attribute_existing)
{
    ck_assert(utils_hdf5_create_attr(group_id, ATTRIBUTE_DBL_S, H5T_NATIVE_DOUBLE, NULL, 0, NULL) == ESCDF_ERROR);
}
END_TEST

/* create_dataset */
START_TEST(test_utils_hdf5_create_dataset)
{
    ck_assert(utils_hdf5_create_dataset(group_id, "somedataset", H5T_NATIVE_DOUBLE, dims, 2, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_utils_hdf5_create_dataset_ptr)
{
    hid_t dtset_id = 0;

    ck_assert(utils_hdf5_create_dataset(group_id, "somedataset", H5T_NATIVE_DOUBLE, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(dtset_id != 0);
    H5Dclose(dtset_id);
}
END_TEST

START_TEST(test_utils_hdf5_create_dataset_existing)
{
    ck_assert(utils_hdf5_create_dataset(group_id, DATASET, H5T_NATIVE_DOUBLE, dims, 2, NULL) == ESCDF_ERROR);
}
END_TEST

/* write_attribute */
START_TEST(test_utils_hdf5_write_attr_scalar)
{
    double value = 0.;

    ck_assert(utils_hdf5_write_attr(group_id, "someattribute", H5T_NATIVE_DOUBLE, NULL, 0, H5T_NATIVE_DOUBLE, &dbl_scalar) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_attr(group_id, "someattribute", H5T_NATIVE_DOUBLE, NULL, 0, &value) == ESCDF_SUCCESS);
    ck_assert(dbl_scalar == value);
}
END_TEST

START_TEST(test_utils_hdf5_write_attr_bool_scalar)
{
    bool value = !bool_scalar;

    ck_assert(utils_hdf5_write_attr_bool(group_id, "someattribute", NULL, 0, &bool_scalar) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_attr_bool(group_id, "someattribute", NULL, 0, &value) == ESCDF_SUCCESS);
    ck_assert(value == bool_scalar);
}
END_TEST

START_TEST(test_utils_hdf5_write_attr_string_scalar)
{
    char string[20] = "";

    ck_assert(utils_hdf5_write_attr_string(group_id, "someattribute", 20, NULL, 0, string_scalar) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_attr_string(group_id, "someattribute", 20, NULL, 0, string) == ESCDF_SUCCESS);
    ck_assert_str_eq(string, string_scalar);
}
END_TEST

START_TEST(test_utils_hdf5_write_attr_array)
{
    double values[3][2] = {{0.0, 0.0},
                           {0.0, 0.0},
                           {0.0, 0.0}};

    ck_assert(utils_hdf5_write_attr(group_id, "someattribute", H5T_NATIVE_DOUBLE, dims, 2, H5T_NATIVE_DOUBLE, &dbl_array) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_attr(group_id, "someattribute", H5T_NATIVE_DOUBLE, dims, 2, &values) == ESCDF_SUCCESS);
    ck_assert(dbl_array[0][0] == values[0][0]);
    ck_assert(dbl_array[0][1] == values[0][1]);
    ck_assert(dbl_array[1][0] == values[1][0]);
    ck_assert(dbl_array[1][1] == values[1][1]);
    ck_assert(dbl_array[2][0] == values[2][0]);
    ck_assert(dbl_array[2][1] == values[2][1]);
}
END_TEST


START_TEST(test_utils_hdf5_write_attr_string_array)
{
    char values[3][2][20] = {{"", ""},
                             {"", ""},
                             {"", ""}};

    ck_assert(utils_hdf5_write_attr_string(group_id, "someattribute", 20, dims, 2, &string_array) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_attr_string(group_id, "someattribute", 20, dims, 2, &values) == ESCDF_SUCCESS);
    ck_assert_str_eq(string_array[0][0], values[0][0]);
    ck_assert_str_eq(string_array[0][1], values[0][1]);
    ck_assert_str_eq(string_array[1][0], values[1][0]);
    ck_assert_str_eq(string_array[1][1], values[1][1]);
    ck_assert_str_eq(string_array[2][0], values[2][0]);
    ck_assert_str_eq(string_array[2][1], values[2][1]);
}
END_TEST

START_TEST(test_utils_hdf5_write_attr_bool_array)
{
    bool values[3][2]= {{true,  false},
                        {false, true},
                        {true,  true}};

    ck_assert(utils_hdf5_write_attr_bool(group_id, "someattribute", dims, 2, &bool_array) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_attr_bool(group_id, "someattribute", dims, 2, &values) == ESCDF_SUCCESS);
    ck_assert(bool_array[0][0] == values[0][0]);
    ck_assert(bool_array[0][1] == values[0][1]);
    ck_assert(bool_array[1][0] == values[1][0]);
    ck_assert(bool_array[1][1] == values[1][1]);
    ck_assert(bool_array[2][0] == values[2][0]);
    ck_assert(bool_array[2][1] == values[2][1]);
    }
END_TEST

/* write_dataset */
START_TEST(test_utils_hdf5_write_dataset)
{
    hid_t dtset_id;
    double array[3][2] = {{1.0, 2.0},
                          {3.0, 4.0},
                          {5.0, 6.0}};
    double values[3][2];

    ck_assert(utils_hdf5_create_dataset(group_id, "somedataset", H5T_NATIVE_DOUBLE, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_write_dataset(dtset_id, H5P_DEFAULT, &array, H5T_NATIVE_DOUBLE, NULL, NULL, NULL) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_dataset(dtset_id, H5P_DEFAULT, &values, H5T_NATIVE_DOUBLE, NULL, NULL, NULL) == ESCDF_SUCCESS);
    ck_assert(array[0][0] == values[0][0]);
    ck_assert(array[0][1] == values[0][1]);
    ck_assert(array[1][0] == values[1][0]);
    ck_assert(array[1][1] == values[1][1]);
    ck_assert(array[2][0] == values[2][0]);
    ck_assert(array[2][1] == values[2][1]);
}
END_TEST

START_TEST(test_utils_hdf5_write_dataset_slice)
{
    hid_t dtset_id;
    double array[3] = {1.0, 2.0, 3.0};
    double values[3];
    size_t start[2] = {0, 0};
    size_t count[2] = {3, 1};

    ck_assert(utils_hdf5_create_dataset(group_id, "somedataset", H5T_NATIVE_DOUBLE, dims, 2, &dtset_id) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_write_dataset(dtset_id, H5P_DEFAULT, &array, H5T_NATIVE_DOUBLE, start, count, NULL) == ESCDF_SUCCESS);
    ck_assert(utils_hdf5_read_dataset(dtset_id, H5P_DEFAULT, &values, H5T_NATIVE_DOUBLE, start, count, NULL) == ESCDF_SUCCESS);
    ck_assert(values[0] == array[0]);
    ck_assert(values[1] == array[1]);
    ck_assert(values[2] == array[2]);
}
END_TEST


Suite * make_utils_hdf5_suite(void)
{
    Suite *s;
    TCase *tc_utils_hdf5_check_present, *tc_utils_hdf5_check_present_recursive, *tc_utils_hdf5_check_present_attr, *tc_utils_hdf5_check_shape, *tc_utils_hdf5_check_attr, *tc_utils_hdf5_check_dataset;
    TCase *tc_utils_hdf5_read_attr, *tc_utils_hdf5_read_dataset;
    TCase *tc_utils_hdf5_create_group, *tc_utils_hdf5_create_attribute, *tc_utils_hdf5_create_dataset;
    TCase *tc_utils_hdf5_write_attribute, *tc_utils_hdf5_write_dataset;

    s = suite_create("HDF5 utilities");

    tc_utils_hdf5_check_present = tcase_create("Check present");
    tcase_add_checked_fixture(tc_utils_hdf5_check_present, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_present, test_utils_hdf5_check_present_true);
    tcase_add_test(tc_utils_hdf5_check_present, test_utils_hdf5_check_present_false);
    suite_add_tcase(s, tc_utils_hdf5_check_present);

    tc_utils_hdf5_check_present_recursive = tcase_create("Check present recursive");
    tcase_add_checked_fixture(tc_utils_hdf5_check_present_recursive, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_present_recursive, test_utils_hdf5_check_present_recursive_true);
    tcase_add_test(tc_utils_hdf5_check_present_recursive, test_utils_hdf5_check_present_recursive_false);
    suite_add_tcase(s, tc_utils_hdf5_check_present_recursive);

    tc_utils_hdf5_check_present_attr = tcase_create("Check present attribute");
    tcase_add_checked_fixture(tc_utils_hdf5_check_present_attr, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_present_attr, test_utils_hdf5_check_present_attr_true);
    tcase_add_test(tc_utils_hdf5_check_present_attr, test_utils_hdf5_check_present_attr_false);
    suite_add_tcase(s, tc_utils_hdf5_check_present_attr);

    tc_utils_hdf5_check_shape = tcase_create("Check shape");
    tcase_add_checked_fixture(tc_utils_hdf5_check_shape, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_scalar_correct);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_array_correct);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_scalar_wrong_args);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_array_wrong_args);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_no_class);
    tcase_add_test(tc_utils_hdf5_check_shape, test_utils_hdf5_check_shape_wrong_class);
    suite_add_tcase(s, tc_utils_hdf5_check_shape);

    tc_utils_hdf5_check_attr = tcase_create("Check attribute");
    tcase_add_checked_fixture(tc_utils_hdf5_check_attr, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_attr, test_utils_hdf5_check_attr_scalar);
    tcase_add_test(tc_utils_hdf5_check_attr, test_utils_hdf5_check_attr_array);
    tcase_add_test(tc_utils_hdf5_check_attr, test_utils_hdf5_check_attr_array_wrong);
    tcase_add_test(tc_utils_hdf5_check_attr, test_utils_hdf5_check_attr_ptr);
    suite_add_tcase(s, tc_utils_hdf5_check_attr);

    tc_utils_hdf5_check_dataset = tcase_create("Check dataset");
    tcase_add_checked_fixture(tc_utils_hdf5_check_dataset, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_check_dataset, test_utils_hdf5_check_dataset);
    tcase_add_test(tc_utils_hdf5_check_dataset, test_utils_hdf5_check_dataset_wrong);
    tcase_add_test(tc_utils_hdf5_check_dataset, test_utils_hdf5_check_dataset_ptr);
    suite_add_tcase(s, tc_utils_hdf5_check_dataset);

    tc_utils_hdf5_read_attr = tcase_create("Read attribute");
    tcase_add_checked_fixture(tc_utils_hdf5_read_attr, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_attr_scalar);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_attr_array);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_attr_string_scalar);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_attr_bool_scalar);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_uint);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_int);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_attr_string_array);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_attr_bool_array);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_uint_array);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_int_array);
    tcase_add_test(tc_utils_hdf5_read_attr, test_utils_hdf5_read_dbl_array);
    suite_add_tcase(s, tc_utils_hdf5_read_attr);

    tc_utils_hdf5_read_dataset = tcase_create("Read dataset");
    tcase_add_checked_fixture(tc_utils_hdf5_read_dataset, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_read_dataset, test_utils_hdf5_read_dataset);
    tcase_add_test(tc_utils_hdf5_read_dataset, test_utils_hdf5_read_dataset_sliced);
    tcase_add_test(tc_utils_hdf5_read_dataset, test_utils_hdf5_read_dataset_at);
    tcase_add_test(tc_utils_hdf5_read_dataset, test_utils_hdf5_read_dataset_at_empty);
    suite_add_tcase(s, tc_utils_hdf5_read_dataset);

    tc_utils_hdf5_create_group = tcase_create("Create group");
    tcase_add_checked_fixture(tc_utils_hdf5_create_group, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_create_group, test_utils_hdf5_create_group);
    tcase_add_test(tc_utils_hdf5_create_group, test_utils_hdf5_create_group_ptr);
    tcase_add_test(tc_utils_hdf5_create_group, test_utils_hdf5_create_group_overwrite);
    tcase_add_test(tc_utils_hdf5_create_group, test_utils_hdf5_create_group_existing_path);
    tcase_add_test(tc_utils_hdf5_create_group, test_utils_hdf5_create_group_non_existing_path);
    suite_add_tcase(s, tc_utils_hdf5_create_group);

    tc_utils_hdf5_create_attribute = tcase_create("Create attribute");
    tcase_add_checked_fixture(tc_utils_hdf5_create_attribute, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_create_attribute, test_utils_hdf5_create_attribute_scalar);
    tcase_add_test(tc_utils_hdf5_create_attribute, test_utils_hdf5_create_attribute_array);
    tcase_add_test(tc_utils_hdf5_create_attribute, test_utils_hdf5_create_attribute_ptr);
    tcase_add_test(tc_utils_hdf5_create_attribute, test_utils_hdf5_create_attribute_existing);
    suite_add_tcase(s, tc_utils_hdf5_create_attribute);
    
    tc_utils_hdf5_create_dataset = tcase_create("Create dataset");
    tcase_add_checked_fixture(tc_utils_hdf5_create_dataset, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_create_dataset, test_utils_hdf5_create_dataset);
    tcase_add_test(tc_utils_hdf5_create_dataset, test_utils_hdf5_create_dataset_ptr);
    tcase_add_test(tc_utils_hdf5_create_dataset, test_utils_hdf5_create_dataset_existing);
    suite_add_tcase(s, tc_utils_hdf5_create_dataset);

    tc_utils_hdf5_write_attribute = tcase_create("Write attribute");
    tcase_add_checked_fixture(tc_utils_hdf5_write_attribute, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_write_attribute, test_utils_hdf5_write_attr_scalar);
    tcase_add_test(tc_utils_hdf5_write_attribute, test_utils_hdf5_write_attr_bool_scalar);
    tcase_add_test(tc_utils_hdf5_write_attribute, test_utils_hdf5_write_attr_string_scalar);
    tcase_add_test(tc_utils_hdf5_write_attribute, test_utils_hdf5_write_attr_array);
    tcase_add_test(tc_utils_hdf5_write_attribute, test_utils_hdf5_write_attr_bool_array);
    tcase_add_test(tc_utils_hdf5_write_attribute, test_utils_hdf5_write_attr_string_array);
    suite_add_tcase(s, tc_utils_hdf5_write_attribute);

    tc_utils_hdf5_write_dataset = tcase_create("Write dataset");
    tcase_add_checked_fixture(tc_utils_hdf5_write_dataset, utils_hdf5_setup, utils_hdf5_teardown);
    tcase_add_test(tc_utils_hdf5_write_dataset, test_utils_hdf5_write_dataset);
    tcase_add_test(tc_utils_hdf5_write_dataset, test_utils_hdf5_write_dataset_slice);
    suite_add_tcase(s, tc_utils_hdf5_write_dataset);

    return s;
}
