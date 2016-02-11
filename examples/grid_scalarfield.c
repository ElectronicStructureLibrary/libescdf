/*  -*- c-basic-offset: 4 -*- */
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
#include <time.h>
#include <stdio.h>

#include <math.h>
#include <mpi.h>

#define HAVE_MPI 1
#include <escdf_grid_scalarfields.h>

int main(int argc, char **argv)
{
    escdf_handle_t *file_id;
    escdf_errno_t err;
    escdf_grid_scalarfield_t *scalarfield;
#define NDIMS 3
    escdf_direction_type dirarr[3];
    unsigned int uarr[3], uval, *xyz2zyx, nvals;
    double lattice[3 * 3];
    char *filename;

    int iproc, nproc;
    hsize_t slice, i, j, x, y, z, x0, nx;
    double *dens, rx, ry, rz, fac, wallt;
#define NCOMP 2
#define NSIZE_X 200
#define NSIZE_Y ((NDIMS > 1) ? 100 : 1)
#define NSIZE_Z ((NDIMS > 2) ? 300 : 1)
#define NDATA NSIZE_X * NSIZE_Y * NSIZE_Z

#define SINGLE_FILE true
#define NRETRY 10

    struct timespec begin, end;

    /* Initialise MPI. */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &iproc);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    /*****************************************************/
    /* Create a gaussian density per slice of yz planes. */
    /*****************************************************/

    lattice[0] = 0.;
    lattice[1] = 0.;
    lattice[2] = 0.;
    lattice[3] = 0.;
    lattice[4] = 0.;
    lattice[5] = 0.;
    lattice[6] = 0.;
    lattice[7] = 0.;
    lattice[8] = 0.;
    for (i = 0; i < NDIMS; i++) {
      lattice[i * NDIMS + i] = 5.;
    }

    nx = NSIZE_X / nproc;
    if (iproc < (NSIZE_X - nx * nproc))
      nx += 1;
    if (iproc <= (NSIZE_X - nx * nproc))
      x0 = (NSIZE_X / nproc + 1) * iproc;
    else
      x0 = NSIZE_X / nproc * iproc + NSIZE_X - nx * nproc;
    /* fprintf(stderr, "%d: %lld / %lld\n", iproc, x0, nx); */
    dens = malloc(sizeof(double) * nx * NSIZE_Y * NSIZE_Z * NCOMP);
    for (i = 0; i < NCOMP; i++) {
      j = 0;
      fac = (i % NCOMP) ? +1. : -1.;
      for (slice = 0; slice < nx; slice++) {
        x = x0 + slice;
        rx = ((double)x / (double)NSIZE_X - 0.5) * lattice[0];
        rx *= rx;
        for (y = 0; y < NSIZE_Y; y++) {
          ry = ((double)y / (double)NSIZE_Y - 0.5) * lattice[NDIMS + 1];
          ry *= ry;
          for (z = 0; z < NSIZE_Z; z++) {
            rz = ((double)z / (double)NSIZE_Z - 0.5) * lattice[2 * NDIMS + 2];
            rz *= rz;
            /* Centred gaussian. */
            dens[i * nx * NSIZE_Y * NSIZE_Z + j++] = fac * exp(-(rx + ry + rz));
          }
        }
      }
    }
    /* The lookup table that convert local storage to default zyx
       ordering. */
    xyz2zyx = malloc(sizeof(unsigned int) * nx * NSIZE_Y * NSIZE_Z);
    i = 0;
    for (slice = 0; slice < nx; slice++) {
      x = x0 + slice;
      for (y = 0; y < NSIZE_Y; y++) {
        for (z = 0; z < NSIZE_Z; z++) {
          xyz2zyx[i++] = z * NSIZE_Y * NSIZE_X + y * NSIZE_X + x;
        }
      }
    }
    if (iproc == 0) {
      fprintf(stderr, "Data size: %g MB\n", 8. * nx * NSIZE_Y * NSIZE_Z * NCOMP / 1024. / 1024.);
    }
    
    /**************************************/
    /* Create a ESCDF scalarfield object. */
    /**************************************/
    scalarfield = escdf_grid_scalarfield_new(NULL);

    escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, NDIMS);
    dirarr[0] = ESCDF_DIRECTION_FREE;
    dirarr[1] = ESCDF_DIRECTION_FREE;
    dirarr[2] = ESCDF_DIRECTION_FREE;
    escdf_grid_scalarfield_set_dimension_types(scalarfield, dirarr, NDIMS);
    escdf_grid_scalarfield_set_lattice_vectors(scalarfield, lattice, NDIMS * NDIMS);
    uarr[0] = (SINGLE_FILE) ? NSIZE_X : nx;
    uarr[1] = NSIZE_Y;
    uarr[2] = NSIZE_Z;
    escdf_grid_scalarfield_set_number_of_grid_points(scalarfield, uarr, NDIMS);
    escdf_grid_scalarfield_set_number_of_components(scalarfield, NCOMP);
    escdf_grid_scalarfield_set_real_or_complex(scalarfield, ESCDF_REAL);
    escdf_grid_scalarfield_set_use_default_ordering(scalarfield, false);

    wallt = 0.;
    for (i = 0; i < NRETRY; i++) {
      /* Create a new file. */
      if (SINGLE_FILE) {
        file_id = escdf_create_mpi("grid_scalarfield.h5", NULL, MPI_COMM_WORLD);
      } else {
        filename = malloc(sizeof(char) *
                          (snprintf(NULL, 0, "grid_scalarfield.%04d.h5", iproc) + 1));
        sprintf(filename, "grid_scalarfield.%04d.h5", iproc);
        file_id = escdf_create(filename, NULL);
        free(filename);
      }
      if (file_id == NULL) {
        escdf_error_show(escdf_error_get_last(__func__), __FILE__, __LINE__, __func__);
        return escdf_error_get_last(__func__);
      }

      err = escdf_grid_scalarfield_write_metadata(scalarfield, file_id);
      if (err != ESCDF_SUCCESS) {
        escdf_error_show(err, __FILE__, __LINE__, __func__);
        return err;
      }
    
      /**************************************************************/
      /* Write the density slices, providing only slices of data and
         slice sizes, additionally, the density is stored z last so a
         lookup table to convert to default x last ordering is
         provided. */
      /**************************************************************/
      MPI_Barrier(MPI_COMM_WORLD);
      clock_gettime(CLOCK_REALTIME, &begin);
      err = escdf_grid_scalarfield_write_values_on_grid_sliced
        (scalarfield, file_id, dens, xyz2zyx, nx * NSIZE_Y * NSIZE_Z);
      if (err != ESCDF_SUCCESS) {
        escdf_error_show(err, __FILE__, __LINE__, __func__);
        return err;
      }

      escdf_close(file_id);

      MPI_Barrier(MPI_COMM_WORLD);
      clock_gettime(CLOCK_REALTIME, &end);

      wallt += (double)(end.tv_sec - begin.tv_sec) +
        1e-9 * (end.tv_nsec - begin.tv_nsec);
    }

    /***********/
    /* Cleanup */
    /***********/
    free(dens);
    free(xyz2zyx);
    escdf_grid_scalarfield_free(scalarfield);
    
    if (iproc == 0) {
      fprintf(stderr, "Wall time: %g s\n", wallt / NRETRY);
      fprintf(stderr, "Write spd: %g MB/s\n",
              (8. * nx * NSIZE_Y * NSIZE_Z * NCOMP / 1024. / 1024.) /
              (wallt / NRETRY));
    }

    /****************************************/
    /* Read example and speed measurements. */
    /****************************************/
    wallt = 0.;
    for (i = 0; i < NRETRY; i++) {
      /* Open file. */
      if (SINGLE_FILE) {
        file_id = escdf_open_mpi("grid_scalarfield.h5", NULL, MPI_COMM_WORLD);
      } else {
        filename = malloc(sizeof(char) *
                          (snprintf(NULL, 0, "grid_scalarfield.%04d.h5", iproc) + 1));
        sprintf(filename, "grid_scalarfield.%04d.h5", iproc);
        file_id = escdf_open(filename, NULL);
        free(filename);
      }
      if (file_id == NULL) {
        escdf_error_show(escdf_error_get_last(__func__), __FILE__, __LINE__, __func__);
        return escdf_error_get_last(__func__);
      }

      /* Read it meta data. */
      scalarfield = escdf_grid_scalarfield_new(NULL);
      err = escdf_grid_scalarfield_read_metadata(scalarfield, file_id);
      if (err != ESCDF_SUCCESS) {
        escdf_error_show(err, __FILE__, __LINE__, __func__);
        return err;
      }

      /* read the density. */
      dens = malloc(sizeof(double) * nx * NSIZE_Y * NSIZE_Z * NCOMP);
      MPI_Barrier(MPI_COMM_WORLD);
      clock_gettime(CLOCK_REALTIME, &begin);
      err = escdf_grid_scalarfield_read_values_on_grid_sliced
        (scalarfield, file_id, dens, NULL, nx * NSIZE_Y * NSIZE_Z);
      if (err != ESCDF_SUCCESS) {
        escdf_error_show(err, __FILE__, __LINE__, __func__);
        return err;
      }
      MPI_Barrier(MPI_COMM_WORLD);
      clock_gettime(CLOCK_REALTIME, &end);
      wallt += (double)(end.tv_sec - begin.tv_sec) +
        1e-9 * (end.tv_nsec - begin.tv_nsec);

      escdf_close(file_id);
    }

    if (iproc == 0) {
      fprintf(stderr, "Wall time: %g s\n", wallt / NRETRY);
      fprintf(stderr, "Read  spd: %g MB/s\n",
              (8. * nx * NSIZE_Y * NSIZE_Z * NCOMP / 1024. / 1024.) /
              (wallt / NRETRY));
    }

    /***********/
    /* Cleanup */
    /***********/
    free(dens);
    /* free(xyz2zyx); */
    escdf_grid_scalarfield_free(scalarfield);    

    MPI_Finalize();

    return 0;
}
