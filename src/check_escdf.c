/* Copyright (C) 2016-2017 Damien Caliste <dcaliste@free.fr>
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


#include <stdlib.h>
#include <check.h>

#include "check_escdf.h"

int main(void)
{
    int number_failed;
    SRunner *sr;

    sr = srunner_create(make_info_suite());
    srunner_add_suite(sr, make_error_suite());
    srunner_add_suite(sr, make_utils_suite());
    srunner_add_suite(sr, make_utils_hdf5_suite());
    srunner_add_suite(sr, make_handle_suite());
    srunner_add_suite(sr, make_attributes_suite());
    srunner_add_suite(sr, make_group_suite());
    //  srunner_add_suite(sr, make_system_suite());
    //  srunner_add_suite(sr, make_grid_scalarfield_suite());

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

