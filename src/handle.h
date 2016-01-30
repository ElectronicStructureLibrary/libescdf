
#ifndef LIBESCDF_HANDLE_H
#define LIBESCDF_HANDLE_H

#include <hdf5.h>

/**********************************************************************
 * Data structures                                                    *
 **********************************************************************/

/**
*
*/
typedef struct {
    hid_t hdf_id;  /**< HDF5 file/group identifier */
} _handle_t;

#endif
