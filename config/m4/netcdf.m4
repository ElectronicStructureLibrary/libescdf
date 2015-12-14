# -*- Autoconf -*-
#
# Copyright (C) 2015 Yann Pouillon
#
# This file is part of the Libescdf software package. For license information,
# please see the COPYING file in the top-level directory of the Libescdf source
# distribution.
#



# ESCDF_NETCDF_DETECT()
# ---------------------
#
# Check whether the NETCDF library is working.
#
AC_DEFUN([ESCDF_NETCDF_DETECT],[
  dnl Init
  escdf_netcdf_ok="unknown"
  escdf_netcdf_has_hdrs="unknown"
  escdf_netcdf_has_libs="unknown"

  dnl Backup environment
  escdf_saved_CPPFLAGS="${CPPFLAGS}"
  escdf_saved_LIBS="${LIBS}"

  dnl Prepare build parameters
  CPPFLAGS="${CPPFLAGS} ${escdf_netcdf_incs}"
  LIBS="${escdf_netcdf_libs} ${LIBS}"

  dnl Look for C includes
  AC_LANG_PUSH([C])
  AC_CHECK_HEADERS([netcdf.h],
    [escdf_netcdf_has_hdrs="yes"], [escdf_netcdf_has_hdrs="no"])
  AC_LANG_POP([C])

  dnl Look for C libraries and routines
  if test "${escdf_netcdf_has_hdrs}" = "yes"; then
    AC_LANG_PUSH([C])
    AC_MSG_CHECKING([whether the serial NETCDF libraries work])
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
      [[
#include <netcdf.h>
      ]],
      [[
        int ncid;
        nc_open("conftest.nc", NC_NOWRITE, &ncid);
      ]])], [escdf_netcdf_has_libs="yes"], [escdf_netcdf_has_libs="no"])
    AC_MSG_RESULT([${escdf_netcdf_has_libs}])
    AC_LANG_POP([C])
  fi

  dnl Take final decision
  AC_MSG_CHECKING([whether we have a full NETCDF support])
  if test "${escdf_netcdf_has_hdrs}" = "yes" -a \
          "${escdf_netcdf_has_libs}" = "yes"; then
    escdf_netcdf_ok="yes"
  else
    escdf_netcdf_ok="no"
  fi
  AC_MSG_RESULT([${escdf_netcdf_ok}])

  dnl Restore environment
  CPPFLAGS="${escdf_saved_CPPFLAGS}"
  LIBS="${escdf_saved_LIBS}"
]) # ESCDF_NETCDF_DETECT
