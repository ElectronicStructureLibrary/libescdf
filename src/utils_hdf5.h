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

#ifndef LIBESCDF_utils_hdf5_H
#define LIBESCDF_utils_hdf5_H

#include <hdf5.h>

#include "escdf_error.h"
#include "utils.h"


/******************************************************************************
 * check methods                                                              *
 ******************************************************************************/

/**
 * Checks if some location has a given object attached to it.
 *
 * @param[in] loc_id: location identifier to which the object is attached.
 * @param[in] name: object name.
 * @return true if object is present, false otherwise.
 */
bool utils_hdf5_check_present(hid_t loc_id, const char *name);

/**
 * Checks if some location has a given object attached to it. Existance of any
 * intermediate groups specified in the path will also be checked.
 *
 * @param[in] loc_id: location identifier to which the object is attached.
 * @param[in] name: object name.
 * @return true if object is present, false otherwise.
 */
bool utils_hdf5_check_present_recursive(hid_t loc_id, const char *name);

/**
 * Checks if some location has a given attribute attached to it.
 *
 * @param[in] loc_id: location identifier to which the attribute is attached.
 * @param[in] name: attribute name.
 * @return true if attribute is present, false otherwise.
 */
bool utils_hdf5_check_present_attr(hid_t loc_id, const char *name);

/**
 * Checks if the dimensions of a dataspace match some given values.
 *
 * @param[in] dtspace_id: object identifier of dataspace to be checked.
 * @param[in] dims: pointer to array storing the expected size of each dimension.
 * @param[in] ndims: number of expected dimensions in the dataspace.
 * @return error code.
 */
escdf_errno_t utils_hdf5_check_shape(hid_t dtspace_id, const hsize_t *dims, unsigned int ndims);

/**
 * Checks if the dimensions of an attribute match some given values.
 *
 * @param[in] loc_id: object identifier to which the attribute is attached.
 * @param[in] name: attribute name.
 * @param[in] dims: pointer to array storing the expected size of each dimension.
 * @param[in] ndims: number of expected dimensions of the attribute.
 * @param[out] attr_pt: If NULL the access to attribute is terminated on exit; otherwise returns a pointer to the
 *                      attribute object identifier.
 * @return error code.
 */
escdf_errno_t utils_hdf5_check_attr(hid_t loc_id, const char *name, const hsize_t *dims, unsigned int ndims, hid_t *attr_pt);

/**
 * Checks if the dimensions of a dataset match some given values.
 *
 * @param[in] loc_id: object identifier to which the dataset is attached.
 * @param[in] name: dataset name.
 * @param[in] dims: pointer to array storing the expected size of each dimension.
 * @param[in] ndims: number of expected dimensions of the dataset.
 * @param[out] dtset_pt: If NULL the access to dataset is terminated on exit; otherwise returns a pointer to the dataset
 *                       object identifier.
 * @return error code.
 */
escdf_errno_t utils_hdf5_check_dataset(hid_t loc_id, const char *name, const hsize_t *dims, unsigned int ndims, hid_t *dtset_pt);


/******************************************************************************
 * read methods                                                               *
 ******************************************************************************/

/**
 * Read the value of an attribute.
 *
 * @param[in] loc_id: object identifier to which the attribute is attached.
 * @param[in] name: attribute name.
 * @param[in] mem_type_id: Identifier of the attribute datatype (in memory).
 * @param[in] dims: pointer to array storing the size of each dimension.
 * @param[in] ndims: number of dimensions of the attribute.
 * @param[out] buf: buffer for data to be read.
 * @return error code.
 */
escdf_errno_t utils_hdf5_read_attr(hid_t loc_id, const char *name, hid_t mem_type_id, const hsize_t *dims, unsigned int ndims, void *buf);

/**
 * Read the value of a scalar attribute of boolean type and mark it as set.
 *
 * @param[in] loc_id: object identifier to which the attribute is attached.
 * @param[in] name: attribute name.
 * @param[out] scalar: boolean value read.
 * @return error code.
 */
escdf_errno_t utils_hdf5_read_bool(hid_t loc_id, const char *name, _bool_set_t *scalar);

/**
 * Read the value of a scalar attribute of unsigned integer type and mark it as set.
 *
 * @param[in] loc_id: object identifier to which the attribute is attached.
 * @param[in] name: attribute name.
 * @param[out] scalar: unsigned integer value read.
 * @param[in] range: range of allowed values for the attribute. The function will return an error if the value read is
 *                   not between range[0] and range[1].
 * @return error code.
 */
escdf_errno_t utils_hdf5_read_uint(hid_t loc_id, const char *name, _uint_set_t *scalar, unsigned int range[2]);

/**
 * Read the value of a scalar attribute of integer type and mark it as set.
 *
 * @param[in] loc_id: object identifier to which the attribute is attached.
 * @param[in] name: attribute name.
 * @param[out] scalar: integer value read.
 * @param[in] range: range of allowed values for the attribute. The function will return an error if the value read is
 *                   not between range[0] and range[1].
 * @return error code.
 */
escdf_errno_t utils_hdf5_read_int(hid_t loc_id, const char *name, _int_set_t *scalar, int range[2]);

/**
 * Read the value of a scalar attribute of string type.
 *
 * @param[in] loc_id: object identifier to which the attribute is attached.
 * @param[in] name: attribute name.
 * @param[out] scalar: string value read.
 * @param[in] len: length of the string.
 * @return error code.
 */
escdf_errno_t utils_hdf5_read_string(hid_t loc_id, const char *name, char **string, hsize_t len);

/**
 * Read the values of an array attribute of unsigned integer type.
 *
 * @param[in] loc_id: object identifier to which the attribute is attached.
 * @param[in] name: attribute name.
 * @param[out] array: pointer to array of unsigned integer values read.
 * @param[in] dims: pointer to array storing the size of each dimension.
 * @param[in] ndims: number of dimensions of the attribute.
 * @param[in] range: range of allowed values for the attribute. The function will return an error if the values read are
 *                   not between range[0] and range[1].
 * @return error code.
 */
escdf_errno_t utils_hdf5_read_uint_array(hid_t loc_id, const char *name, unsigned int **array, const hsize_t *dims, unsigned int ndims, unsigned int range[2]);

/**
 * Read the values of an array attribute of integer type.
 *
 * @param[in] loc_id: object identifier to which the attribute is attached.
 * @param[in] name: attribute name.
 * @param[out] array: pointer to array of integer values read.
 * @param[in] dims: pointer to array storing the size of each dimension.
 * @param[in] ndims: number of dimensions of the attribute.
 * @param[in] range: range of allowed values for the attribute. The function will return an error if the values read are
 *                   not between range[0] and range[1].
 * @return error code.
 */
escdf_errno_t utils_hdf5_read_int_array(hid_t loc_id, const char *name, int **array, const hsize_t *dims, unsigned int ndims, int range[2]);

/**
 * Read the values of an array attribute of double type.
 *
 * @param[in] loc_id: object identifier to which the attribute is attached.
 * @param[in] name: attribute name.
 * @param[out] array: pointer to array of double values read.
 * @param[in] dims: pointer to array storing the size of each dimension.
 * @param[in] ndims: number of dimensions of the attribute.
 * @param[in] range: range of allowed values for the attribute. The function will return an error if the values read are
 *                   not between range[0] and range[1].
 * @return error code.
 */
escdf_errno_t utils_hdf5_read_dbl_array(hid_t loc_id, const char *name, double **array, const hsize_t *dims, unsigned int ndims, double range[2]);

/**
 * Reads raw data from a dataset into a buffer. Optionally, data is read from an hyperslice of the dataset defined by
 * setting start, count, and stride.
 *
 * @param[in] dtset_id: identifier of the dataset to read from.
 * @param[in] xfer_id: identifier of a transfer property list for this I/O operation.
 * @param[out] buf: buffer for data to be read.
 * @param[in] mem_type_id: identifier of the memory datatype.
 * @param[in] start: offset of start of hyperslab.
 * @param[in] count: number of blocks included in hyperslab.
 * @param[in] stride: hyperslab stride.
 * @return error code.
 */
escdf_errno_t utils_hdf5_read_dataset(hid_t dtset_id, hid_t xfer_id, void *buf, hid_t mem_type_id, const hsize_t *start, const hsize_t *count, const hsize_t *stride);

/**
 * Reads selected array elements from a dataset into a buffer.
 *
 * @param[in] dtset_id: identifier of the dataset to read from.
 * @param[in] xfer_id: identifier of a transfer property list for this I/O operation.
 * @param[out] buf: buffer for data to be read.
 * @param[in] mem_type_id: identifier of the memory datatype.
 * @param[in] num_points: number of elements to be selected.
 * @param[in] coord: a pointer to a buffer containing a serialized copy of a 2-dimensional array of zero-based values
 *                   specifying the coordinates of the elements in the point selection.
 * @return error code.
 */
escdf_errno_t utils_hdf5_read_dataset_at(hid_t dtset_id, hid_t xfer_id, void *buf, hid_t mem_type_id, hsize_t num_points, const hsize_t *coord);


/******************************************************************************
 * create methods                                                             *
 ******************************************************************************/

/**
 * Create a new group at a specified location. It creates intermediate groups that may be missing in the path.
 *
 * @param[in] loc_id: file or group identifier.
 * @param[in] path: relative path from loc_id.
 * @param[out] group_pt: if NULL the access to group is terminated on exit; otherwise returns a pointer to the group
 *                       object identifier.
 * @return error code.
 */
escdf_errno_t utils_hdf5_create_group(hid_t loc_id, const char *path, hid_t *group_pt);

/**
 * Creates an attribute attached to a specified object.
 *
 * @param[in] loc_id: object identifier to which the attribute is to be attached to.
 * @param[in] name: attribute name.
 * @param[in] type_id: identifier of datatype for attribute.
 * @param[in] dims: pointer to array storing the size of each dimension.
 * @param[in] ndims: number of dimensions of the attribute.
 * @param[out] attr_pt: if NULL the access to attribute is terminated on exit; otherwise returns a pointer to the
 *                      attribute object identifier.
 * @return error code.
 */
escdf_errno_t utils_hdf5_create_attr(hid_t loc_id, const char *name, hid_t type_id, const hsize_t *dims, const unsigned int ndims, hid_t *attr_pt);

/**
 * Creates a dataset attached to a specified object.
 *
 * @param[in] loc_id: object identifier to which the dataset is to be attached to.
 * @param[in] name: dataset name.
 * @param[in] type_id: identifier of datatype for dataset.
 * @param[in] dims: pointer to array storing the size of each dimension.
 * @param[in] ndims: number of dimensions of the dataset.
 * @param[out] dtset_pt: if NULL the access to dataset is terminated on exit; otherwise returns a pointer to the dataset
 *                       object identifier.
 * @return error code.
 */
escdf_errno_t utils_hdf5_create_dataset(hid_t loc_id, const char *name, hid_t type_id, const hsize_t *dims, unsigned int ndims, hid_t *dtset_pt);


/******************************************************************************
 * write methods                                                              *
 ******************************************************************************/

/**
 * Creates an attribute attached to a specified object and write data to it.
 *
 * @param[in] loc_id: object identifier to which the attribute is to be attached to.
 * @param[in] name: attribute name.
 * @param[in] disk_type_id: identifier of datatype for attribute.
 * @param[in] dims: pointer to array storing the size of each dimension.
 * @param[in] ndims: number of dimensions of the attribute.
 * @param[in] mem_type_id: identifier of the attribute datatype (in memory).
 * @param[in] buf: data to be written.
 * @return error code.
 */
escdf_errno_t utils_hdf5_write_attr(hid_t loc_id, const char *name, hid_t disk_type_id, const hsize_t *dims, unsigned int ndims, hid_t mem_type_id, const void *buf);

/**
 * Writes raw data from a buffer to a dataset. Optionally, data is written to an hyperslice of the dataset defined by
 * setting start, count, and stride.
 *
 * @param[in] loc_id: object identifier to which the dataset is to be attached to.
 * @param[in] xfer_id: identifier of a transfer property list for this I/O operation.
 * @param[in] buf: data to be written.
 * @param[in] mem_type_id: identifier of the memory datatype.
 * @param[in] start: offset of start of hyperslab.
 * @param[in] count: number of blocks included in hyperslab.
 * @param[in] stride: hyperslab stride.
 * @return error code.
 */
escdf_errno_t utils_hdf5_write_dataset(hid_t dtset_id, hid_t xfer_id, const void *buf, hid_t mem_type_id, const hsize_t *start, const hsize_t *count, const hsize_t *stride);

/**
 * Creates a scalar attribute attached to a specified object and writes a boolean to it.
 *
 * @param[in] loc_id: object identifier to which the attribute is to be attached to.
 * @param[in] name: attribute name.
 * @param[in] value: boolean to be written.
 * @return error code.
 */
escdf_errno_t utils_hdf5_write_bool(hid_t loc_id, const char *name, const bool value);

/**
 * Creates a scalar attribute attached to a specified object and writes a string to it.
 *
 * @param[in] loc_id: object identifier to which the attribute is to be attached to.
 * @param[in] name: attribute name.
 * @param[in] string: string to be written.
 * @param[in] len: length of the string.
 * @return error code.
 */
escdf_errno_t utils_hdf5_write_string(hid_t loc_id, const char *name, const char *string, hsize_t len);


/******************************************************************************
 * misc methods                                                               *
 ******************************************************************************/

/**
 * Selects a hyperslab region from a dataset. If either start or count are NULL, then returns the entire dataset
 * dataspace.
 *
 * @param[in] dtset_id: dataset identifier.
 * @param[in] diskspace_id: identifier for a copy of the dataspace on disk.
 * @param[in] memspace_id: identifier of the dataspace in memory.
 * @param[in] start: offset of start of hyperslab.
 * @param[in] count: number of blocks included in hyperslab.
 * @param[in] stride: hyperslab stride.
 * @return error code.
 */
escdf_errno_t utils_hdf5_select_slice(hid_t dtset_id, hid_t *diskspace_id, hid_t *memspace_id, const hsize_t *start, const hsize_t *count, const hsize_t *stride);

#if H5_VERS_MINOR < 8 || H5_VERS_RELEASE < 5
htri_t H5Oexists_by_name(hid_t loc_id, const char *name, hid_t lapl_id);
#endif

#endif
