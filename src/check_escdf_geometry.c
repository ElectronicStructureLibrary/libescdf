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
static escdf_geometry_t *geo = NULL;


/******************************************************************************
 * Setup and teardown                                                         *
 ******************************************************************************/

void geometry_setup_file(const char *file, const char *geom_path)
{
    hid_t file_id, escdf_root_id, geom_root_id, silicon_geom_id;
    hid_t dataset_id, string_len_3, string_len_80;
    hsize_t dims_1D, dims_2D[2], dims_3D[3];
    herr_t status;

    /* set up the meta-data to write */
    char *system_name = "silicon";
    uint number_of_physical_dimensions = 3;
    int dimension_types[3] = {1, 1, 1};
    char *embedded_system = "no";
    uint number_of_species = 1;
    uint number_of_sites = 2;
    uint number_of_symmetry_operations = 48;
    double lattice_vectors[3][3] = {{5.0964124, 5.0964124, 0.0000000},
                                    {5.0964124, 0.0000000, 5.0964124},
                                    {0.0000000, 5.0964124, 5.0964124}};
    uint spacegroup_3D_number = 227;
    char *symmorphic = "no";
    char *time_reversal_symmetry = "no";
    double bulk_regions_for_semi_infinite_setup[2] = {2.0, 3.0};

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
    double concentration_of_species_at_site[2] = {1,0, 1.0};
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
    string_len_3 = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(string_len_3, 3);


    /* Write metadata */

    /* --system_name */
    dims_1D = 1;
    status = utils_hdf5_write_attr(silicon_geom_id, "system_name", string_len_80, &dims_1D, 1, string_len_80, system_name);

    /* --number_of_physical_dimensions */
    dims_1D = 1;
    status = utils_hdf5_write_attr(silicon_geom_id, "number_of_physical_dimensions", H5T_NATIVE_UINT, &dims_1D, 1, H5T_NATIVE_UINT, &number_of_physical_dimensions);

    /* --dimension_types */
    dims_1D = 3;
    status = utils_hdf5_write_attr(silicon_geom_id, "dimension_types", H5T_NATIVE_INT, &dims_1D, 1, H5T_NATIVE_INT, &dimension_types);

    /* --embedded_system */
    dims_1D = 1;
    status = utils_hdf5_write_attr(silicon_geom_id, "embedded_system", string_len_3, &dims_1D, 1, string_len_3, embedded_system);

    /* --number_of_species */
    dims_1D = 1;
    status = utils_hdf5_write_attr(silicon_geom_id, "number_of_species", H5T_NATIVE_UINT, &dims_1D, 1, H5T_NATIVE_UINT, &number_of_species);

    /* --number_of_sites */
    dims_1D = 1;
    status = utils_hdf5_write_attr(silicon_geom_id, "number_of_sites", H5T_NATIVE_UINT, &dims_1D, 1, H5T_NATIVE_UINT, &number_of_sites);

    /* --number_of_symmetry_operations */
    dims_1D = 1;
    status = utils_hdf5_write_attr(silicon_geom_id, "number_of_symmetry_operations", H5T_NATIVE_UINT, &dims_1D, 1, H5T_NATIVE_UINT, &number_of_symmetry_operations);

    /* --lattice_vectors */
    dims_2D[0] = 3;
    dims_2D[1] = 3;
    status = utils_hdf5_create_dataset(silicon_geom_id, "lattice_vectors", H5T_NATIVE_DOUBLE, dims_2D, 2, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, lattice_vectors, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --spacegroup_3D_number */
    dims_1D = 1;
    status = utils_hdf5_create_dataset(silicon_geom_id, "spacegroup_3D_number", H5T_NATIVE_UINT, &dims_1D, 1, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, &spacegroup_3D_number, H5T_NATIVE_UINT, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --symmorphic */
    dims_1D = 1;
    status = utils_hdf5_create_dataset(silicon_geom_id, "symmorphic", string_len_3, &dims_1D, 1, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, symmorphic, string_len_3, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --time_reversal_symmetry */
    dims_1D = 1;
    status = utils_hdf5_create_dataset(silicon_geom_id, "time_reversal_symmetry", string_len_3, &dims_1D, 1, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, time_reversal_symmetry, string_len_3, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

    /* --bulk_regions_for_semi_infinite_setup */
    dims_1D = 2;
    status = utils_hdf5_create_dataset(silicon_geom_id, "bulk_regions_for_semi_infinite_setup", H5T_NATIVE_DOUBLE, &dims_1D, 1, &dataset_id);
    status = utils_hdf5_write_dataset(dataset_id, H5P_DEFAULT, bulk_regions_for_semi_infinite_setup, H5T_NATIVE_DOUBLE, NULL, NULL, NULL);
    status = H5Dclose(dataset_id);

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
    escdf_geometry_free(geo);
    geo = escdf_geometry_new();
}

void geometry_teardown_geo(void)
{
    escdf_geometry_free(geo);
    geo = NULL;
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


/******************************************************************************
 * Low-level creators and destructors                                         *
 ******************************************************************************/

START_TEST(test_geometry_new)
{
    ck_assert((geo = escdf_geometry_new()) != NULL);
}
END_TEST

START_TEST(test_geometry_open_group)
{
    ck_assert(escdf_geometry_open_group(geo, handle_r, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_geometry_open_group_path)
{
    ck_assert(escdf_geometry_open_group(geo, handle_a, GROUP) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_geometry_create_group)
{
    ck_assert(escdf_geometry_create_group(geo, handle_e, NULL) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_geometry_create_group_path)
{
    ck_assert(escdf_geometry_create_group(geo, handle_e, GROUP) == ESCDF_SUCCESS);
}
END_TEST

START_TEST(test_geometry_close_group)
{
    escdf_geometry_open_group(geo, handle_r, NULL);
    ck_assert(escdf_geometry_close_group(geo) == ESCDF_SUCCESS);
}
END_TEST


Suite * make_geometry_suite(void)
{
    Suite *s;
    TCase *tc_geometry_new, *tc_geometry_open_group, *tc_geometry_create_group, *tc_geometry_close_group;

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

    return s;
}
