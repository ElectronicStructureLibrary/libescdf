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
    unsigned int uarr[NDIMS], uval;
    double darr[NDIMS * NDIMS];
    char *filename;

    int iproc, nproc;
    hsize_t slice, i, j, x, y, z, z0, nz;
    double *dens, rx, ry, rz, fac, wallt;
#define NCOMP 2
#define NSIZE_X 600
#define NSIZE_Y 300
#define NSIZE_Z 200
#define NDATA NSIZE_X * NSIZE_Y * NSIZE_Z

#define SINGLE_FILE false
#define NRETRY 10

    struct timespec begin, end;

    /* Initialise MPI. */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &iproc);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    /*****************************************************/
    /* Create a gaussian density per slice of xy planes. */
    /*****************************************************/
    nz = NSIZE_Z / nproc;
    if (iproc < (NSIZE_Z - nz * nproc))
      nz += 1;
    if (iproc <= (NSIZE_Z - nz * nproc))
      z0 = (NSIZE_Z / nproc + 1) * iproc;
    else
      z0 = NSIZE_Z / nproc * iproc + NSIZE_Z - nz * nproc;
    /* fprintf(stderr, "%d: %lld / %lld\n", iproc, z0, nz); */
    dens = malloc(sizeof(double) * nz * NSIZE_Y * NSIZE_X * NCOMP);
    for (i = 0; i < NCOMP; i++) {
      j = 0;
      fac = (i % NCOMP) ? +1. : -1.;
      for (slice = 0; slice < nz; slice++) {
        z = z0 + slice;
        rz = ((double)z / (double)NSIZE_Z - 0.5) * darr[8];
        rz *= rz;
        for (y = 0; y < NSIZE_Y; y++) {
          ry = ((double)y / (double)NSIZE_Y - 0.5) * darr[4];
          ry *= ry;
          for (x = 0; x < NSIZE_X; x++) {
            rx = ((double)x / (double)NSIZE_X - 0.5) * darr[0];
            rx *= rx;
            /* Centred gaussian. */
            dens[i * nz * NSIZE_Y * NSIZE_X + j++] = fac * exp(-(rx + ry + rz));
          }
        }
      }
    }
    if (iproc == 0) {
    fprintf(stderr, "Data size: %g MB\n", 8. * nz * NSIZE_Y * NSIZE_X * NCOMP / 1024. / 1024.);
    }
    
    /**************************************/
    /* Create a ESCDF scalarfield object. */
    /**************************************/
    scalarfield = escdf_grid_scalarfield_new(NULL);

    escdf_grid_scalarfield_set_number_of_physical_dimensions(scalarfield, NDIMS);
    uarr[0] = ESCDF_DIRECTION_FREE;
    uarr[1] = ESCDF_DIRECTION_FREE;
    uarr[2] = ESCDF_DIRECTION_FREE;
    escdf_grid_scalarfield_set_dimension_types(scalarfield, uarr, NDIMS);
    darr[0] = 5.;
    darr[1] = 0.;
    darr[2] = 0.;
    darr[3] = 0.;
    darr[4] = 5.;
    darr[5] = 0.;
    darr[6] = 0.;
    darr[7] = 0.;
    darr[8] = 5.;
    escdf_grid_scalarfield_set_lattice_vectors(scalarfield, darr, NDIMS * NDIMS);
    uarr[0] = NSIZE_X;
    uarr[1] = NSIZE_Y;
    uarr[2] = (SINGLE_FILE) ? NSIZE_Z : nz;
    escdf_grid_scalarfield_set_number_of_grid_points(scalarfield, uarr, NDIMS);
    escdf_grid_scalarfield_set_number_of_components(scalarfield, NCOMP);
    escdf_grid_scalarfield_set_real_or_complex(scalarfield, 1);
    escdf_grid_scalarfield_set_use_default_ordering(scalarfield, true);

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
         slice sizes */
      /**************************************************************/
      MPI_Barrier(MPI_COMM_WORLD);
      clock_gettime(CLOCK_REALTIME, &begin);
      err = escdf_grid_scalarfield_write_values_on_grid_sliced(scalarfield, file_id, dens, NULL, nz * NSIZE_Y * NSIZE_X);
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
    escdf_grid_scalarfield_free(scalarfield);
    
    if (iproc == 0) {
      fprintf(stderr, "Wall time: %g s\n", wallt / NRETRY);
      fprintf(stderr, "Write spd: %g MB/s\n",
              (8. * nz * NSIZE_Y * NSIZE_X * NCOMP / 1024. / 1024.) /
              (wallt / NRETRY));
    }

    MPI_Finalize();

    return 0;
}
