/* Copyright (C) 2016-2017 Micael Oliveira <micael.oliveira@mpsd.mpg.de>
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

#ifndef LIBESCDF_ESCDF_INFO_H
#define LIBESCDF_ESCDF_INFO_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the version number of the library.
 * @param[out] major major version number, -1 if not found
 * @param[out] minor minor version number, -1 if not found
 * @param[out] micro micro version number, -1 if not found
 */
void escdf_info_version(int *major, int *minor, int *micro);

/**
 * Provide the package string of the library.
 * @param[out] info: package name and version.
 */
void escdf_info_string(char *info);


#ifdef __cplusplus
}
#endif

#endif
