 /*  -*- c-basic-example-groups -*- */

/* Copyright (C) 2019 Sebastian Ohlmann, based on an example from M. Lüders.
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


#include "../src/escdf_group.h"
#include "../src/escdf_groups_specs.h"

/* Comments: The above includes should be moved into the escdf.h header */

#define NX 128
#define NY 128
#define NZ 128
#define N NX*NY*NZ

#define MPI

#ifdef MPI
#include <mpi.h>
#endif

int main(int argc, char **argv) {
  /* set density */
  double * density;
  density = (double *) malloc(N * sizeof(double));
  memset(density, 0, N * sizeof(double));

  escdf_handle_t *escdf_file;
  escdf_group_t *group_density;

  escdf_dataset_t *dataset_values;
  escdf_dataset_t *dataset_number_of_grid_points;

  escdf_errno_t error;

  unsigned int num_dims = 3;
  unsigned int real_or_complex = 1;
  unsigned int number_of_components = 1;
  unsigned int number_of_grid_points[3] = {NX, NY, NZ};

  /* Should this be moved into the escdf_create() call? */
  escdf_register_all_group_specs();

#ifndef MPI
  escdf_file = escdf_create("density.h5", NULL);
#else
  MPI_Init(&argc, &argv);
  escdf_file = escdf_create_mpi("density.h5", NULL, MPI_COMM_WORLD);
#endif

  group_density = escdf_group_create(escdf_file, "density", NULL);
  if(group_density == NULL) printf("Null pointer for group !!\n");

  /* Set attributes needed for density */
  error = escdf_group_attribute_set(group_density, "number_of_physical_dimensions", &num_dims);
  if(error != ESCDF_SUCCESS) printf("setting 'number_of_phycical_dimensions' results in error = %d \n", error);

  error = escdf_group_attribute_set(group_density, "real_or_complex", &real_or_complex);
  if(error != ESCDF_SUCCESS) printf("setting 'real_or_complex' results in error = %d \n", error);

  error = escdf_group_attribute_set(group_density, "number_of_components", &number_of_components);
  if(error != ESCDF_SUCCESS) printf("setting 'number_of_phycical dimensions' results in error = %d \n", error);

  error = escdf_group_attribute_set(group_density, "number_of_grid_points", &number_of_grid_points);
  if(error != ESCDF_SUCCESS) printf("setting 'number_of_grid_points' results in error = %d \n", error);


  /* Set the values of the dataset */
  dataset_values = escdf_group_dataset_create(group_density, "values_on_grid");
  if(dataset_values==NULL) printf("Null pointer for dataset values_on_grid!!\n");

  escdf_dataset_print(dataset_values);

  escdf_group_dataset_write_simple(dataset_values, density);
  printf("Datasets written.\n");

  /* Now close everything */
  escdf_group_dataset_close(group_density, "values_on_grid");
  printf("Datasets closed. \n");
  escdf_group_close(group_density);
  printf("Group closed. \n");

  escdf_close(escdf_file);
  printf("File closed. \n");

#ifdef MPI
  MPI_Finalize();
#endif

  return 0;
}
