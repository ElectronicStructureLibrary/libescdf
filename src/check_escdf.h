/*
 Copyright (C) 2016 M. Oliveira

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

#ifndef LIBESCDF_CHECK_ESCDF_H
#define LIBESCDF_CHECK_ESCDF_H

/**
 * @file check_escdf.h
 * @brief escdf check testsuites
 */

Suite *make_info_suite(void);
Suite *make_error_suite(void);
Suite *make_handle_suite(void);
Suite *make_grid_scalarfield_suite(void);

#endif
