/* Copyright (C) 2016-2017 Fabiano Corsetti <fabiano.corsetti@gmail.com>
 *                         Micael Oliveira <micael.oliveira@mpsd.mpg.de>
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

#include <check.h>
#include <unistd.h>

#include "escdf_handle.h"
#include "escdf_geometry.h"
#include "utils_hdf5.h"

#define FILE_R "check_escdf_geometry_test_file_R.h5"
#define FILE_A "check_escdf_geometry_test_file_A.h5"
#define FILE_E "check_escdf_geometry_test_file_E.h5"
#define GROUP "Silicon"

static escdf_handle_t *handle_r = NULL, *handle_a = NULL, *handle_e = NULL;
static escdf_geometry_t *geo_r = NULL, *geo_a = NULL, *geo_e = NULL;

/* Data to be writen to the files */
static char *system_name = "silicon";
static unsigned int number_of_physical_dimensions = 3;
static int dimension_types[3] = {1, 1, 1};
static bool embedded_system = false;
static unsigned int number_of_species = 1;
static unsigned int number_of_sites = 2;
static unsigned int number_of_symmetry_operations = 48;
static double lattice_vectors[3][3] = {{5.0964124, 5.0964124, 0.0000000},
                                       {5.0964124, 0.0000000, 5.0964124},
                                       {0.0000000, 5.0964124, 5.0964124}};
static unsigned int spacegroup_3D_number = 227;
static bool symmorphic = false;
static bool time_reversal_symmetry = false;
static double bulk_regions_for_semi_infinite_dimension[2] = {2.0, 3.0};

/******************************************************************************
 * Setup and teardown                                                         *
 ******************************************************************************/

void geometry_setup_file(const char *file, const char *geom_path)
{
    hid_t file_id, escdf_root_id, geom_root_id, silicon_geom_id;
    hid_t dataset_id, string_len_3, string_len_80;
    hsize_t dims_1D, dims_2D[2], dims_3D[3];
    herr_t status;

    /* set up the data to write */
    uint species_at_sites[2] = {1, 1};
    double fractional_site_positions[2][3] = {{0.00, 0.00, 0.00},
                                              {0.25, 0.25, 0.25}};
    double cartesian_site_positions[2][3] = {{0.00, 0.00, 0.00},
                                             {2.5482062, 2.5482062, 2.5482062}};
    char *species_names[1] = {"Silicon"};
    char *chemical_symbols[1] = {"Si"};
    double atomic_numbers[1] = {14.0};
    double reduced_symmetry_matrices[48][3][3];
    double reduced_symmetry_translations[48][3];
    uint number_of_species_at_site[2] = {1, 1};
    double concentration_of_species_at_site[2] = {1.0, 1.0};
    double local_rotations[2][3][3] = {{{1.0, 0.0, 0.0},
                                        {0.0, 1.0, 0.0},
                                        {0.0, 0.0, 1.0}},
                                       {{1.0, 0.0, 0.0},
                                        {0.0, 1.0, 0.0},
                                        {0.0, 0.0, 1.0}}};
    double magnetic_moments[2][3] = {{0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0}};
    int site_regions[2];
    int cell_in_host;
    uint site_in_host;
    double forces[2][3];
    double stress_tensor[3][3];


    /* create file with a silicon geometry group */
    file_id = H5Fcreate(file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    escdf_root_id = H5Gopen(file_id, ".", H5P_DEFAULT);
    geom_root_id = H5Gcreate(escdf_root_id, "geometries", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (geom_path == NULL) {
        silicon_geom_id = H5Gopen(geom_root_id, ".", H5P_DEFAULT);
    } else {
        silicon_geom_id = H5Gcreate(geom_root_id, geom_path, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }

    /* Initialize some helper variables */
    string_len_80 = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(string_len_80, 80);
    status = H5Tset_strpad(string_len_80, H5T_STR_NULLTERM);
    string_len_3 = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(string_len_3, 3);
    status = H5Tset_strpad(string_len_3, H5T_STR_NULLTERM);

    /* Write metadata */

    /* --system_name */
    status = utils_hdf5_write_string(silicon_geom_id, "system_name", system_name, 80);

    /* --number_of_physical_dimensions */
    status = utils_hdf5_write_attr(silicon_geom_id, "number_of_physical_dimensions", H5T_NATIVE_UINT, NULL, 0, H5T_NATIVE_UINT, &number_of_physical_dimensions);

    /* --dimension_types */
    dims_1D = 3;
    status = utils_hdf5_write_attr(silicon_geom_id, "dimension_types", H5T_NATIVE_INT, &dims_1D, 1, H5T_NATIVE_INT, &dimension_types);

    /* --embedded_system */
    dims_1D = 1;
    status = utils_hdf5_write_bool(silicon_geom_id, "embedded_system", embedded_system);

    /* --number_of_species */
    status = utils_hdf5_write_attr(silicon_geom_id, "number_of_species", H5T_NATIVE_UINT, NULL, 0, H5T_NATIVE_UINT, &number_of_species);

    /* --number_of_sites */
    status = utils_hdf5_write_attr(silicon_geom_id, "number_of_sites", H5T_NATIVE_UINT, NULL, 0, H5T_NATIVE_UINT, &number_of_sites);

    /* --number_of_symmetry_operations */
    status = utils_hdf5_write_attr(silicon_geom_id, "number_of_symmetry_operations", H5T_NATIVE_UINT, NULL, 0, H5T_NATIVE_UINT, &number_of_symmetry_operations);

    /* --lattice_vectors */
    dims_2D[0] = 3;
    dims_2D[1] = 3;
    status = utils_hdf5_write_attr(silicon_geom_id, "lattice_vectors", H5T_NATIVE_DOUBLE, dims_2D, 2, H5T_NATIVE_DOUBLE, &lattice_vectors);

    /* --spacegroup_3D_number */
    status = utils_hdf5_write_attr(silicon_geom_id, "spacegroup_3D_number", H5T_NATIVE_UINT, NULL, 0, H5T_NATIVE_UINT, &spacegroup_3D_number);

    /* --symmorphic */
    status = utils_hdf5_write_bool(silicon_geom_id, "symmorphic", symmorphic);

    /* --time_reversal_symmetry */
    status = utils_hdf5_write_bool(silicon_geom_id, "time_reversal_symmetry", time_reversal_symmetry);

    /* --bulk_regions_for_semi_infinite_dimension */
    dims_1D = 2;
    status = utils_hdf5_write_attr(silicon_geom_id, "bulk_regions_for_semi_infinite_dimension", H5T_NATIVE_DOUBLE, &dims_1D, 1, H5T_NATIVE_DOUBLE, &bulk_regions_for_semi_infinite_dimension);

    /* Write data */

    /* --species_at_sites */
    dims_1D = 2;
    status = utils_hdf5_create_dataset(silicon_geom_id, "species_at_sites", H5T_NATIVE_UINT, &dims_1D, 1, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, species_at_sites, H5T_NATIVE_UINT, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --cartesian_site_positions */
    dims_2D[0] = 2;
    dims_2D[1] = 3;
    status = utils_hdf5_create_dataset(silicon_geom_id, "cartesian_site_positions", H5T_NATIVE_DOUBLE, dims_2D, 2, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, cartesian_site_positions, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --fractional_site_positions */
    dims_2D[0] = 2;
    dims_2D[1] = 3;
    status = utils_hdf5_create_dataset(silicon_geom_id, "fractional_site_positions", H5T_NATIVE_DOUBLE, dims_2D, 2, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, fractional_site_positions, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --species_names */
    dims_1D = 1;
    status = utils_hdf5_create_dataset(silicon_geom_id, "species_names", string_len_80, &dims_1D, 1, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, *species_names, string_len_80, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --chemical_symbols */
    dims_1D = 1;
    status = utils_hdf5_create_dataset(silicon_geom_id, "chemical_symbols", string_len_3, &dims_1D, 1, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, *chemical_symbols, string_len_3, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --atomic_numbers */
    dims_1D = 1;
    status = utils_hdf5_create_dataset(silicon_geom_id, "atomic_numbers", H5T_NATIVE_DOUBLE, &dims_1D, 1, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, atomic_numbers, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --reduced_symmetry_matrices */

    /* --reduced_symmetry_translations */

    /* --number_of_species_at_site */
    dims_1D = 2;
    status = utils_hdf5_create_dataset(silicon_geom_id, "number_of_species_at_site", H5T_NATIVE_UINT, &dims_1D, 1, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, number_of_species_at_site, H5T_NATIVE_UINT, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --concentration_of_species_at_site */
    dims_1D = 2;
    status = utils_hdf5_create_dataset(silicon_geom_id, "concentration_of_species_at_site", H5T_NATIVE_DOUBLE, &dims_1D, 1, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, concentration_of_species_at_site, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --local_rotations */
    dims_3D[0] = 2;
    dims_3D[1] = 3;
    dims_3D[2] = 3;
    status = utils_hdf5_create_dataset(silicon_geom_id, "local_rotations", H5T_NATIVE_DOUBLE, dims_3D, 3, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, local_rotations, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --magnetic_moments */
    dims_2D[0] = 2;
    dims_2D[1] = 3;
    status = utils_hdf5_create_dataset(silicon_geom_id, "magnetic_moments", H5T_NATIVE_DOUBLE, dims_2D, 2, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, magnetic_moments, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --site_regions */

    /* --cell_in_host */

    /* --site_in_host */

    /* --forces */

    /* --stress */


    /* close everything */
    status = H5Tclose(string_len_3);
    status = H5Tclose(string_len_80);

    status = H5Gclose(silicon_geom_id);
    status = H5Gclose(geom_root_id);
    status = H5Gclose(escdf_root_id);
    status = H5Fclose(file_id);
}

void geometry_setup_geo(void)
{
    escdf_geometry_free(geo_a);
    escdf_geometry_free(geo_r);
    escdf_geometry_free(geo_e);
    geo_a = escdf_geometry_new();
    geo_r = escdf_geometry_new();
    geo_e = escdf_geometry_new();
}

void geometry_teardown_geo(void)
{
    escdf_geometry_free(geo_a);
    escdf_geometry_free(geo_r);
    escdf_geometry_free(geo_e);
    geo_a = NULL;
    geo_r = NULL;
    geo_e = NULL;
}

void geometry_setup(void)
{
    geometry_setup_file(FILE_R, NULL);
    geometry_setup_file(FILE_A, GROUP);
    handle_r = escdf_open(FILE_R, NULL);
    handle_a = escdf_open(FILE_A, NULL);
    handle_e = escdf_create(FILE_E, NULL);
    geometry_setup_geo();
}

void geometry_teardown(void) {
    geometry_teardown_geo();
    escdf_close(handle_r);
    escdf_close(handle_a);
    escdf_close(handle_e);
    unlink(FILE_R);
    unlink(FILE_A);
    unlink(FILE_E);
}

void geometry_group_setup(void)
{
    geometry_setup();
    escdf_geometry_open_group(geo_r, handle_r, NULL);
    escdf_geometry_open_group(geo_a, handle_a, GROUP);
    escdf_geometry_create_group(geo_e, handle_e, NULL);
}

void geometry_group_teardown(void)
{
    escdf_geometry_close_group(geo_r);
    escdf_geometry_close_group(geo_a);
    escdf_geometry_close_group(geo_e);
    geometry_teardown();
}


/******************************************************************************
 * Low-level creators and destructors                                         *
 ******************************************************************************/

START_TEST(test_geometry_new)
{
    ck_assert((geo_e = escdf_geometry_new()) != NULL);
}
END_TEST

START_TEST(test_geometry_open_group)
{
    ck_assert(escdf_geometry_open_group(geo_r, handle_r, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_geometry_open_group_path)
{
    ck_assert(escdf_geometry_open_group(geo_a, handle_a, GROUP) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_geometry_create_group)
{
    ck_assert(escdf_geometry_create_group(geo_e, handle_e, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_geometry_create_group_path)
{
    ck_assert(escdf_geometry_create_group(geo_e, handle_e, GROUP) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_geometry_close_group)
{
    escdf_geometry_open_group(geo_r, handle_r, NULL);
    ck_assert(escdf_geometry_close_group(geo_r) == ESCDF_SUCCESS);
}
END_TEST

/******************************************************************************
 * Metadata methods                                                           *
 ******************************************************************************/

START_TEST(test_geometry_read_metadata)
{
    ck_assert(escdf_geometry_read_metadata(geo_r) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_geometry_read_metadata_empty)
{
    ck_assert(escdf_geometry_read_metadata(geo_e) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_geometry_get_system_name)
{
    char name[80];

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_system_name(geo_r, name) == ESCDF_SUCCESS);
    ck_assert_str_eq(system_name, name);
}
END_TEST

START_TEST(test_geometry_get_system_name_undef)
{
    char name[80];

    ck_assert(escdf_geometry_get_system_name(geo_e, name) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_system_name)
{
    char name[80];

    ck_assert(escdf_geometry_set_system_name(geo_e, "diamond") == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_system_name(geo_e, name) == ESCDF_SUCCESS);
    ck_assert_str_eq("diamond", name);
}
END_TEST

START_TEST(test_geometry_get_number_of_physical_dimensions)
{
    unsigned int value = number_of_physical_dimensions + 1;

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_number_of_physical_dimensions(geo_r, &value) == ESCDF_SUCCESS);
    ck_assert_int_eq(value, number_of_physical_dimensions);
}
END_TEST

START_TEST(test_geometry_get_number_of_physical_dimensions_undef)
{
    unsigned int value;

    ck_assert(escdf_geometry_get_number_of_physical_dimensions(geo_e, &value) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_number_of_physical_dimensions)
{
    unsigned int value_in = 2, value_out = 3;

    ck_assert(escdf_geometry_set_number_of_physical_dimensions(geo_e, value_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_number_of_physical_dimensions(geo_e, &value_out) == ESCDF_SUCCESS);
    ck_assert_int_eq(value_in, value_out);
}
END_TEST

START_TEST(test_geometry_get_dimension_types)
{
    int values[3] = {20, 20, 20};

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_dimension_types(geo_r, values) == ESCDF_SUCCESS);
    ck_assert_int_eq(values[0], dimension_types[0]);
    ck_assert_int_eq(values[1], dimension_types[1]);
    ck_assert_int_eq(values[2], dimension_types[2]);
}
END_TEST

START_TEST(test_geometry_get_dimension_types_undef)
{
    int values[3];

    ck_assert(escdf_geometry_get_dimension_types(geo_e, values) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_dimension_types)
{
    int values_in[3] = {0, 1, 2}, values_out[3] = {3, 4, 5};

    escdf_geometry_set_number_of_physical_dimensions(geo_e, 3);
    ck_assert(escdf_geometry_set_dimension_types(geo_e, values_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_dimension_types(geo_e, values_out) == ESCDF_SUCCESS);
    ck_assert_int_eq(values_in[0], values_out[0]);
    ck_assert_int_eq(values_in[1], values_out[1]);
    ck_assert_int_eq(values_in[2], values_out[2]);
}
END_TEST

START_TEST(test_geometry_get_embedded_system)
{
    bool value = !embedded_system;

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_embedded_system(geo_r, &value) == ESCDF_SUCCESS);
    ck_assert(value == embedded_system);
}
END_TEST

START_TEST(test_geometry_get_embedded_system_undef)
{
    bool value;

    ck_assert(escdf_geometry_get_embedded_system(geo_e, &value) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_embedded_system)
{
    bool value_in = true, value_out = false;

    ck_assert(escdf_geometry_set_embedded_system(geo_e, value_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_embedded_system(geo_e, &value_out) == ESCDF_SUCCESS);
    ck_assert(value_in == value_out);
}
END_TEST

START_TEST(test_geometry_get_number_of_species)
{
    unsigned int value = number_of_species + 1;

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_number_of_species(geo_r, &value) == ESCDF_SUCCESS);
    ck_assert_int_eq(value, number_of_species);
}
END_TEST

START_TEST(test_geometry_get_number_of_species_undef)
{
    unsigned int value;

    ck_assert(escdf_geometry_get_number_of_species(geo_e, &value) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_number_of_species)
{
    unsigned int value_in = 3, value_out = 2;

    ck_assert(escdf_geometry_set_number_of_species(geo_e, value_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_number_of_species(geo_e, &value_out) == ESCDF_SUCCESS);
    ck_assert_int_eq(value_in, value_out);
}
END_TEST

START_TEST(test_geometry_get_number_of_sites)
{
    unsigned int value = number_of_sites + 1;

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_number_of_sites(geo_r, &value) == ESCDF_SUCCESS);
    ck_assert_int_eq(value, number_of_sites);
}
END_TEST

START_TEST(test_geometry_get_number_of_sites_undef)
{
    unsigned int value;

    ck_assert(escdf_geometry_get_number_of_sites(geo_e, &value) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_number_of_sites)
{
    unsigned int value_in = 3, value_out = 2;

    ck_assert(escdf_geometry_set_number_of_sites(geo_e, value_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_number_of_sites(geo_e, &value_out) == ESCDF_SUCCESS);
    ck_assert_int_eq(value_in, value_out);
}
END_TEST

START_TEST(test_geometry_get_number_of_symmetry_operations)
{
    unsigned int value = number_of_symmetry_operations + 1;

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_number_of_symmetry_operations(geo_r, &value) == ESCDF_SUCCESS);
    ck_assert_int_eq(value, number_of_symmetry_operations);
}
END_TEST

START_TEST(test_geometry_get_number_of_symmetry_operations_undef)
{
    unsigned int value;

    ck_assert(escdf_geometry_get_number_of_symmetry_operations(geo_e, &value) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_number_of_symmetry_operations)
{
    unsigned int value_in = 3, value_out = 2;

    ck_assert(escdf_geometry_set_number_of_symmetry_operations(geo_e, value_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_number_of_symmetry_operations(geo_e, &value_out) == ESCDF_SUCCESS);
    ck_assert_int_eq(value_in, value_out);
}
END_TEST

START_TEST(test_geometry_get_lattice_vectors)
{
    double values[9] = {0.0, 0.1, 0.2,
                        0.3, 0.4, 0.5,
                        0.6, 0.7, 0.8};

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_lattice_vectors(geo_r, values) == ESCDF_SUCCESS);
    ck_assert(values[0] == lattice_vectors[0][0]);
    ck_assert(values[1] == lattice_vectors[0][1]);
    ck_assert(values[2] == lattice_vectors[0][2]);
    ck_assert(values[3] == lattice_vectors[1][0]);
    ck_assert(values[4] == lattice_vectors[1][1]);
    ck_assert(values[5] == lattice_vectors[1][2]);
    ck_assert(values[6] == lattice_vectors[2][0]);
    ck_assert(values[7] == lattice_vectors[2][1]);
    ck_assert(values[8] == lattice_vectors[2][2]);
}
END_TEST

START_TEST(test_geometry_get_lattice_vectors_undef)
{
    double values[9];

    ck_assert(escdf_geometry_get_lattice_vectors(geo_e, values) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_lattice_vectors)
{
    double values_in[9] = {0.0, 0.1, 0.2,
                           0.3, 0.4, 0.5,
                           0.6, 0.7, 0.8};
    double values_out[9] = {1.0, 1.1, 1.2,
                            1.3, 1.4, 1.5,
                            1.6, 1.7, 1.8};

    escdf_geometry_set_number_of_physical_dimensions(geo_e, 3);
    ck_assert(escdf_geometry_set_lattice_vectors(geo_e, values_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_lattice_vectors(geo_e, values_out) == ESCDF_SUCCESS);
    ck_assert(values_in[0] == values_out[0]);
    ck_assert(values_in[1] == values_out[1]);
    ck_assert(values_in[2] == values_out[2]);
    ck_assert(values_in[3] == values_out[3]);
    ck_assert(values_in[4] == values_out[4]);
    ck_assert(values_in[5] == values_out[5]);
    ck_assert(values_in[6] == values_out[6]);
    ck_assert(values_in[7] == values_out[7]);
    ck_assert(values_in[8] == values_out[8]);
}
END_TEST

START_TEST(test_geometry_get_spacegroup_3D_number)
{
    unsigned int value = spacegroup_3D_number + 1;

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_spacegroup_3D_number(geo_r, &value) == ESCDF_SUCCESS);
    ck_assert_int_eq(value, spacegroup_3D_number);
}
END_TEST

START_TEST(test_geometry_get_spacegroup_3D_number_undef)
{
    unsigned int value;

    ck_assert(escdf_geometry_get_spacegroup_3D_number(geo_e, &value) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_spacegroup_3D_number)
{
    unsigned int value_in = 3, value_out = 2;

    ck_assert(escdf_geometry_set_spacegroup_3D_number(geo_e, value_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_spacegroup_3D_number(geo_e, &value_out) == ESCDF_SUCCESS);
    ck_assert_int_eq(value_in, value_out);
}
END_TEST

START_TEST(test_geometry_get_symmorphic)
{
    bool value = !symmorphic;

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_symmorphic(geo_r, &value) == ESCDF_SUCCESS);
    ck_assert_int_eq(value, symmorphic);
}
END_TEST

START_TEST(test_geometry_get_symmorphic_undef)
{
    bool value;

    ck_assert(escdf_geometry_get_symmorphic(geo_e, &value) == ESCDF_ESIZE_MISSING); }
END_TEST

START_TEST(test_geometry_set_symmorphic)
{
    bool value_in = false, value_out = true;

    ck_assert(escdf_geometry_set_symmorphic(geo_e, value_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_symmorphic(geo_e, &value_out) == ESCDF_SUCCESS);
    ck_assert_int_eq(value_in, value_out);
}
END_TEST

START_TEST(test_geometry_get_time_reversal_symmetry)
{
    bool value = !time_reversal_symmetry;

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_time_reversal_symmetry(geo_r, &value) == ESCDF_SUCCESS);
    ck_assert_int_eq(value, time_reversal_symmetry);
}
END_TEST

START_TEST(test_geometry_get_time_reversal_symmetry_undef)
{
    bool value;

    ck_assert(escdf_geometry_get_time_reversal_symmetry(geo_e, &value) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_time_reversal_symmetry)
{
    bool value_in = true, value_out = false;

    ck_assert(escdf_geometry_set_time_reversal_symmetry(geo_e, value_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_time_reversal_symmetry(geo_e, &value_out) == ESCDF_SUCCESS);
    ck_assert_int_eq(value_in, value_out);
}
END_TEST

START_TEST(test_geometry_get_bulk_regions_for_semi_infinite_dimension)
{
    double values[2] = {bulk_regions_for_semi_infinite_dimension[0] + 1.0,
                        bulk_regions_for_semi_infinite_dimension[1] + 2.0};

    escdf_geometry_read_metadata(geo_r);
    ck_assert(escdf_geometry_get_bulk_regions_for_semi_infinite_dimension(geo_r, values) == ESCDF_SUCCESS);
    ck_assert(values[0] == bulk_regions_for_semi_infinite_dimension[0]);
    ck_assert(values[1] == bulk_regions_for_semi_infinite_dimension[1]);
}
END_TEST

START_TEST(test_geometry_get_bulk_regions_for_semi_infinite_dimension_undef)
{
    double values[2];

    ck_assert(escdf_geometry_get_bulk_regions_for_semi_infinite_dimension(geo_e, values) == ESCDF_ESIZE_MISSING);
}
END_TEST

START_TEST(test_geometry_set_bulk_regions_for_semi_infinite_dimension)
{
    double values_in[2] = {0.1, 0.2}, values_out[2] = {0.3, 0.4};

    ck_assert(escdf_geometry_set_bulk_regions_for_semi_infinite_dimension(geo_e, values_in) == ESCDF_SUCCESS);
    ck_assert(escdf_geometry_get_bulk_regions_for_semi_infinite_dimension(geo_e, values_out) == ESCDF_SUCCESS);
    ck_assert(values_in[0] == values_out[0]);
    ck_assert(values_in[1] == values_out[1]);
}
END_TEST


Suite * make_geometry_suite(void)
{
    Suite *s;
    TCase *tc_geometry_new, *tc_geometry_open_group, *tc_geometry_create_group, *tc_geometry_close_group;
    TCase *tc_geometry_read_metadata, *tc_geometry_system_name, *tc_geometry_number_of_physical_dimensions,
        *tc_geometry_dimension_types, *tc_geometry_embedded_system, *tc_geometry_number_of_species,
        *tc_geometry_number_of_sites, *tc_geometry_number_of_symmetry_operations, *tc_geometry_lattice_vectors,
        *tc_geometry_spacegroup_3D_number, *tc_geometry_symmorphic, *tc_geometry_time_reversal_symmetry,
        *tc_geometry_bulk_regions_for_semi_infinite_dimension;

    s = suite_create("Geometry");

    /* Low-level creators and destructors */
    tc_geometry_new = tcase_create("New");
    tcase_add_checked_fixture(tc_geometry_new, NULL, geometry_teardown_geo);
    tcase_add_test(tc_geometry_new, test_geometry_new);
    suite_add_tcase(s, tc_geometry_new);

    tc_geometry_open_group = tcase_create("Open group");
    tcase_add_checked_fixture(tc_geometry_open_group, geometry_setup, geometry_teardown);
    tcase_add_test(tc_geometry_open_group, test_geometry_open_group);
    tcase_add_test(tc_geometry_open_group, test_geometry_open_group_path);
    suite_add_tcase(s, tc_geometry_open_group);

    tc_geometry_create_group = tcase_create("Create group");
    tcase_add_checked_fixture(tc_geometry_create_group, geometry_setup, geometry_teardown);
    tcase_add_test(tc_geometry_create_group, test_geometry_create_group);
    tcase_add_test(tc_geometry_create_group, test_geometry_create_group_path);
    suite_add_tcase(s, tc_geometry_create_group);

    tc_geometry_close_group = tcase_create("Close group");
    tcase_add_checked_fixture(tc_geometry_close_group, geometry_setup, geometry_teardown);
    tcase_add_test(tc_geometry_close_group, test_geometry_close_group);
    suite_add_tcase(s, tc_geometry_close_group);

    /* Metadata methods */
    tc_geometry_read_metadata = tcase_create("Read metadata");
    tcase_add_checked_fixture(tc_geometry_read_metadata, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_read_metadata, test_geometry_read_metadata);
    tcase_add_test(tc_geometry_read_metadata, test_geometry_read_metadata_empty);
    suite_add_tcase(s, tc_geometry_read_metadata);

    tc_geometry_system_name = tcase_create("System name");
    tcase_add_checked_fixture(tc_geometry_system_name, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_system_name, test_geometry_get_system_name);
    tcase_add_test(tc_geometry_system_name, test_geometry_get_system_name_undef);
    tcase_add_test(tc_geometry_system_name, test_geometry_set_system_name);
    suite_add_tcase(s, tc_geometry_system_name);

    tc_geometry_number_of_physical_dimensions = tcase_create("Number of physical dimensions");
    tcase_add_checked_fixture(tc_geometry_number_of_physical_dimensions, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_number_of_physical_dimensions, test_geometry_get_number_of_physical_dimensions);
    tcase_add_test(tc_geometry_number_of_physical_dimensions, test_geometry_get_number_of_physical_dimensions_undef);
    tcase_add_test(tc_geometry_number_of_physical_dimensions, test_geometry_set_number_of_physical_dimensions);
    suite_add_tcase(s, tc_geometry_number_of_physical_dimensions);

    tc_geometry_dimension_types = tcase_create("Dimension types");
    tcase_add_checked_fixture(tc_geometry_dimension_types, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_dimension_types, test_geometry_get_dimension_types);
    tcase_add_test(tc_geometry_dimension_types, test_geometry_get_dimension_types_undef);
    tcase_add_test(tc_geometry_dimension_types, test_geometry_set_dimension_types);
    suite_add_tcase(s, tc_geometry_dimension_types);

    tc_geometry_embedded_system = tcase_create("Embedded system");
    tcase_add_checked_fixture(tc_geometry_embedded_system, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_embedded_system, test_geometry_get_embedded_system);
    tcase_add_test(tc_geometry_embedded_system, test_geometry_get_embedded_system_undef);
    tcase_add_test(tc_geometry_embedded_system, test_geometry_set_embedded_system);
    suite_add_tcase(s, tc_geometry_embedded_system);

    tc_geometry_number_of_species = tcase_create("Number of species");
    tcase_add_checked_fixture(tc_geometry_number_of_species, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_number_of_species, test_geometry_get_number_of_species);
    tcase_add_test(tc_geometry_number_of_species, test_geometry_get_number_of_species_undef);
    tcase_add_test(tc_geometry_number_of_species, test_geometry_set_number_of_species);
    suite_add_tcase(s, tc_geometry_number_of_species);

    tc_geometry_number_of_sites = tcase_create("Number of sites");
    tcase_add_checked_fixture(tc_geometry_number_of_sites, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_number_of_sites, test_geometry_get_number_of_sites);
    tcase_add_test(tc_geometry_number_of_sites, test_geometry_get_number_of_sites_undef);
    tcase_add_test(tc_geometry_number_of_sites, test_geometry_set_number_of_sites);
    suite_add_tcase(s, tc_geometry_number_of_sites);

    tc_geometry_number_of_symmetry_operations = tcase_create("Number of symmetry operations");
    tcase_add_checked_fixture(tc_geometry_number_of_symmetry_operations, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_number_of_symmetry_operations, test_geometry_get_number_of_symmetry_operations);
    tcase_add_test(tc_geometry_number_of_symmetry_operations, test_geometry_get_number_of_symmetry_operations_undef);
    tcase_add_test(tc_geometry_number_of_symmetry_operations, test_geometry_set_number_of_symmetry_operations);
    suite_add_tcase(s, tc_geometry_number_of_symmetry_operations);

    tc_geometry_lattice_vectors = tcase_create("Lattice vectors");
    tcase_add_checked_fixture(tc_geometry_lattice_vectors, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_lattice_vectors, test_geometry_get_lattice_vectors);
    tcase_add_test(tc_geometry_lattice_vectors, test_geometry_get_lattice_vectors_undef);
    tcase_add_test(tc_geometry_lattice_vectors, test_geometry_set_lattice_vectors);
    suite_add_tcase(s, tc_geometry_lattice_vectors);

    tc_geometry_spacegroup_3D_number = tcase_create("Spacegroup 3D number");
    tcase_add_checked_fixture(tc_geometry_spacegroup_3D_number, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_spacegroup_3D_number, test_geometry_get_spacegroup_3D_number);
    tcase_add_test(tc_geometry_spacegroup_3D_number, test_geometry_get_spacegroup_3D_number_undef);
    tcase_add_test(tc_geometry_spacegroup_3D_number, test_geometry_set_spacegroup_3D_number);
    suite_add_tcase(s, tc_geometry_spacegroup_3D_number);

    tc_geometry_symmorphic = tcase_create("Symmorphic");
    tcase_add_checked_fixture(tc_geometry_symmorphic, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_symmorphic, test_geometry_get_symmorphic);
    tcase_add_test(tc_geometry_symmorphic, test_geometry_get_symmorphic_undef);
    tcase_add_test(tc_geometry_symmorphic, test_geometry_set_symmorphic);
    suite_add_tcase(s, tc_geometry_symmorphic);

    tc_geometry_time_reversal_symmetry = tcase_create("Time reversal symmetry");
    tcase_add_checked_fixture(tc_geometry_time_reversal_symmetry, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_time_reversal_symmetry, test_geometry_get_time_reversal_symmetry);
    tcase_add_test(tc_geometry_time_reversal_symmetry, test_geometry_get_time_reversal_symmetry_undef);
    tcase_add_test(tc_geometry_time_reversal_symmetry, test_geometry_set_time_reversal_symmetry);
    suite_add_tcase(s, tc_geometry_time_reversal_symmetry);

    tc_geometry_bulk_regions_for_semi_infinite_dimension = tcase_create("Bulk regions for semi infinite dimensions");
    tcase_add_checked_fixture(tc_geometry_bulk_regions_for_semi_infinite_dimension, geometry_group_setup, geometry_group_teardown);
    tcase_add_test(tc_geometry_bulk_regions_for_semi_infinite_dimension, test_geometry_get_bulk_regions_for_semi_infinite_dimension);
    tcase_add_test(tc_geometry_bulk_regions_for_semi_infinite_dimension, test_geometry_get_bulk_regions_for_semi_infinite_dimension_undef);
    tcase_add_test(tc_geometry_bulk_regions_for_semi_infinite_dimension, test_geometry_set_bulk_regions_for_semi_infinite_dimension);
    suite_add_tcase(s, tc_geometry_bulk_regions_for_semi_infinite_dimension);

    return s;
}