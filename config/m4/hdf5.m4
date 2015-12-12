# -*- Autoconf -*-
#
# Copyright (C) 2015 Yann Pouillon
#
# This file is part of the Libescdf software package. For license information,
# please see the COPYING file in the top-level directory of the Libescdf source
# distribution.
#



# VDW_HDF5_DETECT()
# ------------------
#
# Check whether the HDF5 library is working.
#
AC_DEFUN([VDW_HDF5_DETECT],[
  dnl Init
  escdf_hdf5_ok="unknown"
  escdf_hdf5_has_hdrs="unknown"
  escdf_hdf5_has_libs="unknown"

  dnl Backup environment
  escdf_saved_CPPFLAGS="${CPPFLAGS}"
  escdf_saved_LIBS="${LIBS}"

  dnl Prepare build parameters
  CPPFLAGS="${CPPFLAGS} ${escdf_hdf5_incs}"
  LIBS="${escdf_hdf5_libs} ${LIBS}"

  dnl Look for C includes
  AC_LANG_PUSH([C])
  AC_CHECK_HEADERS([hdf5.h],
    [escdf_hdf5_has_hdrs="yes"], [escdf_hdf5_has_hdrs="no"])
  AC_LANG_POP([C])

  dnl Look for C libraries and routines
  if test "${escdf_hdf5_has_hdrs}" = "yes"; then
    AC_LANG_PUSH([C])
    AC_MSG_CHECKING([whether the serial HDF5 libraries work])
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
      [[
#include <hdf5.h>
      ]],
      [[
        Hdf5_Init();
      ]])], [escdf_hdf5_has_libs="yes"], [escdf_hdf5_has_libs="no"])
    AC_MSG_RESULT([${escdf_hdf5_has_libs}])
    AC_LANG_POP([C])
  fi

  dnl Take final decision
  AC_MSG_CHECKING([whether we have a full HDF5 support])
  if test "${escdf_hdf5_has_hdrs}" = "yes" -a \
          "${escdf_hdf5_has_libs}" = "yes"; then
    escdf_hdf5_ok="yes"
  else
    escdf_hdf5_ok="no"
  fi
  AC_MSG_RESULT([${escdf_hdf5_ok}])

  dnl Restore environment
  CPPFLAGS="${escdf_saved_CPPFLAGS}"
  LIBS="${escdf_saved_LIBS}"
]) # VDW_HDF5_DETECT
