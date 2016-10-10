/*
 Copyright (C) 2016 F. Corsetti, M. Oliveira

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

#include "escdf_geometry.h"

#define FILE "check_escdf_geometry_test_file.h5"

void geometry_setup(void) {

    char *escdf_root_path=NULL;
    char *silicon_geom_path=NULL;
    hid_t file_id, escdf_root_id, geom_root_id, silicon_geom_id;
    hid_t dataspace_id, attribute_id, dataset_id, string_len_3;
    hsize_t dims_1D, dims_2D[2];
    herr_t status;

    /* set some data to write */
    uint number_of_physical_dimensions = 3;
    int dimension_types[3] = {1, 1, 1};
    char *embedded_system = "no";
    uint number_of_species = 1;
    uint number_of_sites = 2;
    int absolute_or_reduced_coordinates = 2;
    double lattice_vectors[3][3] = {{5.0964124, 5.0964124, 0.0000000},
                                    {5.0964124, 0.0000000, 5.0964124},
                                    {0.0000000, 5.0964124, 5.0964124}};
    double site_positions[2][3] = {{0.00, 0.00, 0.00},
                                   {0.25, 0.25, 0.25}};
    uint species_at_sites[2][1] = {{1},
                                   {1}};
    double atomic_numbers[1] = {14.0};

    /* create file with a silicon geometry group */
    file_id = H5Fcreate(FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (escdf_root_path == NULL) {
        escdf_root_id = H5Gopen(file_id, ".", H5P_DEFAULT);
    } else {
        escdf_root_id = H5Gcreate(file_id, escdf_root_path, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    geom_root_id = H5Gcreate(escdf_root_id, "geometries", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (silicon_geom_path == NULL) {
        silicon_geom_id = H5Gopen(geom_root_id, ".", H5P_DEFAULT);
    } else {
        silicon_geom_id = H5Gcreate(geom_root_id, silicon_geom_path, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }

    /* write attributes of the group: */

    /* --number_of_physical_dimensions */
    dims_1D = 1;
    dataspace_id = H5Screate_simple(1, &dims_1D, NULL);
    attribute_id = H5Acreate2(silicon_geom_id, "number_of_physical_dimensions", H5T_NATIVE_UINT,
                              dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Awrite(attribute_id, H5T_NATIVE_UINT, &number_of_physical_dimensions);

    /* --dimension_types */
    dims_1D = 3;
    dataspace_id = H5Screate_simple(1, &dims_1D, NULL);
    attribute_id = H5Acreate2(silicon_geom_id, "dimension_types", H5T_NATIVE_INT,
                              dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Awrite(attribute_id, H5T_NATIVE_INT, &dimension_types);

    /* --embedded_system */
    dims_1D = 1;
    dataspace_id = H5Screate_simple(1, &dims_1D, NULL);
    string_len_3 = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(string_len_3, 3);
    attribute_id = H5Acreate2(silicon_geom_id, "embedded_system", string_len_3,
                              dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Awrite(attribute_id, string_len_3, embedded_system);

    /* --number_of_physical_dimensions */
    dims_1D = 1;
    dataspace_id = H5Screate_simple(1, &dims_1D, NULL);
    attribute_id = H5Acreate2(silicon_geom_id, "number_of_species", H5T_NATIVE_UINT,
                              dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Awrite(attribute_id, H5T_NATIVE_UINT, &number_of_species);

    /* --number_of_sites */
    dims_1D = 1;
    dataspace_id = H5Screate_simple(1, &dims_1D, NULL);
    attribute_id = H5Acreate2(silicon_geom_id, "number_of_sites", H5T_NATIVE_UINT,
                              dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Awrite(attribute_id, H5T_NATIVE_UINT, &number_of_sites);

    /* --absolute_or_reduced_coordinates */
    dims_1D = 1;
    dataspace_id = H5Screate_simple(1, &dims_1D, NULL);
    attribute_id = H5Acreate2(silicon_geom_id, "absolute_or_reduced_coordinates", H5T_NATIVE_INT,
                              dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Awrite(attribute_id, H5T_NATIVE_INT, &absolute_or_reduced_coordinates);

    /* write datasets of the group: */

    /* --lattice_vectors */
    dims_2D[0] = 3;
    dims_2D[1] = 3;
    dataspace_id = H5Screate_simple(2, dims_2D, NULL);
    dataset_id = H5Dcreate2(silicon_geom_id, "lattice_vectors", H5T_NATIVE_DOUBLE,
                            dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &lattice_vectors);

    /* --site_positions */
    dims_2D[0] = 2;
    dims_2D[1] = 3;
    dataspace_id = H5Screate_simple(2, dims_2D, NULL);
    dataset_id = H5Dcreate2(silicon_geom_id, "site_positions", H5T_NATIVE_DOUBLE,
                            dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &site_positions);

    /* --species_at_sites */
    dims_2D[0] = 2;
    dims_2D[1] = 1;
    dataspace_id = H5Screate_simple(2, dims_2D, NULL);
    dataset_id = H5Dcreate2(silicon_geom_id, "species_at_sites", H5T_NATIVE_UINT,
                            dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dataset_id, H5T_NATIVE_UINT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &species_at_sites);

    /* --atomic_numbers */
    dims_1D = 1;
    dataspace_id = H5Screate_simple(1, &dims_1D, NULL);
    dataset_id = H5Dcreate2(silicon_geom_id, "atomic_numbers", H5T_NATIVE_DOUBLE,
                            dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &atomic_numbers);

    /* close everything */
    status = H5Tclose(string_len_3);
    status = H5Dclose(dataset_id);
    status = H5Aclose(attribute_id);
    status = H5Sclose(dataspace_id);
    status = H5Gclose(silicon_geom_id);
    status = H5Gclose(geom_root_id);
    status = H5Gclose(escdf_root_id);
    status = H5Fclose(file_id);
}

void geometry_teardown(void)
{
    unlink(FILE);
}

START_TEST(test_geometry_new)
{
    escdf_handle_t *handle;
    escdf_geometry_t *geometry;

    ck_assert((handle = escdf_create(FILE, NULL)) != NULL);
    ck_assert((geometry = escdf_geometry_new(handle, ".")) != NULL);
    ck_assert(escdf_geometry_free(geometry) == ESCDF_SUCCESS);
}
END_TEST

Suite * make_geometry_suite(void)
{
    Suite *s;
    TCase *tc_geometry;

    s = suite_create("Geometry");

    tc_geometry = tcase_create("New file");
    tcase_add_checked_fixture(tc_geometry, geometry_setup, geometry_teardown);
    tcase_add_test(tc_geometry, test_geometry_new);
    suite_add_tcase(s, tc_geometry);
    return s;
}