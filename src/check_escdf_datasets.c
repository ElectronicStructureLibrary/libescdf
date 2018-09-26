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

#include "escdf_datasets.h"
#include "utils_hdf5.h"
#include "escdf_handle.h"


#define FILE_R "check_dataset_test_file_r.h5"
#define FILE_W "check_dataset_test_file_w.h5"

static escdf_handle_t *handle_r = NULL, *handle_w = NULL;

/******************************************************************************
 * Setup and teardown                                                         *
 ******************************************************************************/


void file_setup_datasets(void)
{
    hid_t file_id, root_id;

    /* create file */
    file_id = H5Fcreate(FILE_R, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    root_id = H5Gopen(file_id, ".", H5P_DEFAULT);


    H5Gclose(root_id);
    H5Fclose(file_id);

    handle_r = escdf_open(FILE_R, NULL);
    handle_w = escdf_create(FILE_W, NULL);
}

void file_teardown_datasets(void)
{
    escdf_close(handle_r);
    escdf_close(handle_w);
    unlink(FILE_R);
    unlink(FILE_W);
}



/******************************************************************************
 * Tests                                                                      *
 ******************************************************************************/



Suite * make_datasets_suite(void)
{
    Suite *s;
    
    s = suite_create("Datasets");
    
    return s;
}
