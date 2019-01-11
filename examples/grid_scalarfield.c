/*  -*- c-basic-offset: 4 -*- */

/* Copyright (C) 2016 Damien Caliste <dcaliste@free.fr>
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

#include <time.h>
#include <stdio.h>

#include <math.h>
#include <mpi.h>

#define HAVE_MPI 1
#include <escdf_group.h>
#include <escdf_datasets.h>
#include <escdf_groups_specs.h>

unsigned int _get_start(int iproc, int nproc, unsigned int len)
{
    unsigned long long int *proclens;
    unsigned long long int len_;
    unsigned int my_offset;
    hsize_t nValues, nGridPoints;
    int i;

    proclens = malloc(sizeof(unsigned long long int) * nproc);
    len_ = (unsigned long long int)len;
    if (nproc == 1) {
        proclens[0] = len_;
    } else {
        MPI_Allgather(&len_, 1, MPI_UNSIGNED_LONG_LONG,
                      proclens, 1, MPI_UNSIGNED_LONG_LONG, MPI_COMM_WORLD);
    }

    my_offset = 0;
    for (i = 0; i < iproc; i++) {
        my_offset += proclens[i];
    }
    free(proclens);

    return my_offset;
}

int main(int argc, char **argv)
{
    escdf_handle_t *file_id;
    escdf_errno_t err;
    escdf_group_t *scalarfield;
    escdf_dataset_t *scalarfield_dens, *scalarfield_ord;
    const unsigned int NDIMS = 3;
    int dirarr[3];
    unsigned int uarr[3], uval, *xyz2zyx, nvals;
    double lattice[3 * 3];
    char *filename;

    int iproc, nproc;
    hsize_t slice, i, j, x, y, z, x0, nx;
    double *dens, rx, ry, rz, fac, wallt;
#define ESCDF_DIRECTION_FREE 0
    const unsigned int NCOMP = 2;
    const unsigned int ESCDF_REAL = 1;
#define NSIZE_X 2
#define NSIZE_Y ((NDIMS > 1) ? 1 : 1)
#define NSIZE_Z ((NDIMS > 2) ? 3 : 1)
#define NDATA NSIZE_X * NSIZE_Y * NSIZE_Z

#define SINGLE_FILE true
#define NRETRY 1

    struct timespec begin, end;

    /* Initialise MPI. */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &iproc);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    escdf_register_all_group_specs();

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

    /**************************************/
    /* Create a ESCDF scalarfield object. */
    /**************************************/
    scalarfield = escdf_group_create(file_id, "density", NULL);
    if (scalarfield == NULL) {
        escdf_error_show(escdf_error_get_last(__func__), __FILE__, __LINE__, __func__);
        return escdf_error_get_last(__func__);
    }

    escdf_group_attribute_set(scalarfield, "number_of_physical_dimensions", &NDIMS);
    dirarr[0] = ESCDF_DIRECTION_FREE;
    dirarr[1] = ESCDF_DIRECTION_FREE;
    dirarr[2] = ESCDF_DIRECTION_FREE;
    escdf_group_attribute_set(scalarfield, "dimension_types", dirarr);
    escdf_group_attribute_set(scalarfield, "lattice_vectors", lattice);
    uarr[0] = (SINGLE_FILE) ? NSIZE_X : nx;
    uarr[1] = NSIZE_Y;
    uarr[2] = NSIZE_Z;
    escdf_group_attribute_set(scalarfield, "number_of_grid_points", uarr);
    escdf_group_attribute_set(scalarfield, "number_of_components", &NCOMP);
    escdf_group_attribute_set(scalarfield, "real_or_complex", &ESCDF_REAL);
    scalarfield_dens = escdf_group_dataset_create_disordered(scalarfield, "values_on_grid");
    if (scalarfield_dens == NULL) {
        escdf_error_show(escdf_error_get_last(__func__), __FILE__, __LINE__, __func__);
        return escdf_error_get_last(__func__);
    }
    scalarfield_ord = escdf_dataset_get_ordering(scalarfield_dens);
    if (scalarfield_ord == NULL) {
        escdf_error_show(escdf_error_get_last(__func__), __FILE__, __LINE__, __func__);
        return escdf_error_get_last(__func__);
    }

    wallt = 0.;
    for (i = 0; i < NRETRY; i++) {
      size_t start[3];
      size_t count[3];
      /**************************************************************/
      /* Write the density slices, providing only slices of data and
         slice sizes, additionally, the density is stored z last so a
         lookup table to convert to default x last ordering is
         provided. */
      /**************************************************************/
      MPI_Barrier(MPI_COMM_WORLD);
      clock_gettime(CLOCK_REALTIME, &begin);
      start[0] = 0;
      start[1] = _get_start(iproc, nproc, nx * NSIZE_Y * NSIZE_Z);
      start[2] = 0;
      count[0] = NCOMP;
      count[1] = nx * NSIZE_Y * NSIZE_Z;
      count[2] = 1;
      err = escdf_dataset_write(scalarfield_dens, start, count, NULL, dens);
      if (err != ESCDF_SUCCESS) {
        escdf_error_show(err, __FILE__, __LINE__, __func__);
        return err;
      }
      err = escdf_dataset_write(scalarfield_ord, start + 1, count + 1, NULL, xyz2zyx);
      if (err != ESCDF_SUCCESS) {
        escdf_error_show(err, __FILE__, __LINE__, __func__);
        return err;
      }

      MPI_Barrier(MPI_COMM_WORLD);
      clock_gettime(CLOCK_REALTIME, &end);

      wallt += (double)(end.tv_sec - begin.tv_sec) +
        1e-9 * (end.tv_nsec - begin.tv_nsec);
    }

    /***********/
    /* Cleanup */
    /***********/
    free(xyz2zyx);
    escdf_dataset_close(scalarfield_dens);
    escdf_group_close(scalarfield);
    escdf_close(file_id);
    
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

      /* Read its meta data. */
      scalarfield = escdf_group_open(file_id, "density", NULL);
      if (!scalarfield) {
        escdf_error_show(err, __FILE__, __LINE__, __func__);
        return err;
      }

      /* read the density. */
      dens = malloc(sizeof(double) * nx * NSIZE_Y * NSIZE_Z * NCOMP);
      MPI_Barrier(MPI_COMM_WORLD);
      clock_gettime(CLOCK_REALTIME, &begin);
      err = escdf_group_dataset_read_at
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

    MPI_Finalize();

    return 0;
}
