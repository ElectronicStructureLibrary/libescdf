/* Copyright (C) 2018 Micael Oliveira <micael.oliveira@mpsd.mpg.de>
 *                    Martin Lueders <martin.lueders@stfc.ac.uk>
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

#include "escdf_datatransfer.h"

struct escdf_datatransfer {

    hid_t id;

};

hid_t escdf_datatransfer_get_id(escdf_datatransfer_t *trans)
{
    return trans->id;
}