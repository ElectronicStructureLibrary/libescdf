# -*- Autoconf -*-
#
# M4 macros for Libescdf
#
# Copyright (C) 2011-2015 Yann Pouillon
#
# This file is part of the Libescdf software package. For license information,
# please see the COPYING file in the top-level directory of the source
# distribution.
#

#
# Support for the Check unit test framework
#



# ESCDF_SEARCH_CHECK()
# ------------------
#
# Look for the Check libraries.
#
AC_DEFUN([ESCDF_SEARCH_CHECK],[
  dnl Init
  escdf_check_has_hdrs="unknown"
  escdf_check_has_libs="unknown"
  escdf_check_ok="unknown"
  escdf_check_cflags="${ESCDF_CHECK_CFLAGS}"
  escdf_check_incs="${ESCDF_CHECK_INCS}"
  escdf_check_ldflags="${ESCDF_CHECK_LDFLAGS}"
  escdf_check_libs="${ESCDF_CHECK_LIBS}"
  escdf_pthread_ok="unknown"
  escdf_rt_ok="unknown"

  dnl Define missing environment variables
  AC_ARG_VAR([ESCDF_CHECK_INCS],
    [Include flags for the Check unit test framework])
  AC_ARG_VAR([ESCDF_CHECK_LDFLAGS],
    [C flags for the Check unit test framework])

  dnl Backup environment
  escdf_saved_CPPFLAGS="${CPPFLAGS}"
  escdf_saved_CFLAGS="${CFLAGS}"
  escdf_saved_LDFLAGS="${LDFLAGS}"
  escdf_saved_LIBS="${LIBS}"

  dnl Init build parameters
  CPPFLAGS="${CPPFLAGS} ${escdf_check_incs}"
  CFLAGS="${CFLAGS} ${escdf_check_cflags}"
  LDFLAGS="${LDFLAGS} ${escdf_check_ldflags}"
  LIBS="${escdf_check_libs} ${LIBS}"

  dnl Search first with pkg-config, then manually
  if test "${escdf_check_cflags}" = "" -a \
          "${escdf_check_incs}" = "" -a \
          "${escdf_check_ldflags}" = "" -a \
          "${escdf_check_libs}" = ""; then
    PKG_CHECK_MODULES([ESCDF_CHECK], [check >= 0.9.4],
      [escdf_pkg_check="yes"; escdf_check_has_hdrs="yes"; escdf_check_has_libs="yes"],
      [escdf_pkg_check="no"])
    if test "${escdf_pkg_check}" = "yes"; then
      escdf_check_cflags="${ESCDF_CHECK_CFLAGS}"
      escdf_check_libs="${ESCDF_CHECK_LIBS}"
    else
      AX_PTHREAD([escdf_pthread_ok="yes"], [escdf_pthread_ok="no"])
      if test "${escdf_pthread_ok}" = "yes"; then
        escdf_check_cflags="${escdf_check_cflags} ${PTHREAD_CFLAGS}"
        escdf_check_libs="${PTHREAD_LIBS} ${escdf_check_libs}"
        CFLAGS="${CFLAGS} ${escdf_check_cflags}"
        LIBS="${escdf_check_libs} ${LIBS}"
      fi
      AC_SEARCH_LIBS([shm_open], [rt], [escdf_rt_ok="yes"], [escdf_rt_ok="no"])
      if test "${escdf_rt_ok}" = "yes"; then
        if test "${ac_cv_search_shm_open}" != "none required"; then
          escdf_check_libs="${ac_cv_search_shm_open} ${escdf_check_libs}"
        fi
      else
        AC_MSG_WARN([no real-time extensions available])
      fi
      AC_CHECK_HEADERS([check.h],
        [escdf_check_has_hdrs="yes"], [escdf_check_has_hdrs="no"])
      if test "${escdf_check_has_hdrs}" = "yes"; then
        AC_SEARCH_LIBS([srunner_create], [check_pic check],
          [escdf_check_has_libs="yes"], [escdf_check_has_libs="no"])
        if test "${escdf_check_has_libs}" = "yes"; then
          if test "${ac_cv_search_srunner_create}" != "none required"; then
            escdf_check_libs="${ac_cv_search_srunner_create} ${escdf_check_libs}"
          fi
        fi
      fi
    fi
  fi

  dnl Look for headers if still needed
  if test "${escdf_check_has_hdrs}" != "yes"; then
    AC_CHECK_HEADERS([check.h],
      [escdf_check_has_hdrs="yes"], [escdf_check_has_hdrs="no"])
  fi

  dnl Check that the specified libraries work
  AC_LANG_PUSH([C])
  AC_LINK_IFELSE([
    AC_LANG_PROGRAM(
      [[
#include "check.h"
      ]],
      [[
        SRunner *sr;
        srunner_run_all(sr, CK_VERBOSE);
      ]])],
    [escdf_check_ok="yes"], [escdf_check_ok="no"])
  AC_LANG_POP([C])

  dnl Display Check configuration
  AC_MSG_CHECKING([for Check C flags])
  AC_MSG_RESULT([${escdf_check_cflags}])
  AC_MSG_CHECKING([for Check include flags])
  AC_MSG_RESULT([${escdf_check_incs}])
  AC_MSG_CHECKING([for Check link flags])
  AC_MSG_RESULT([${escdf_check_ldflags}])
  AC_MSG_CHECKING([for Check libraries])
  AC_MSG_RESULT([${escdf_check_libs}])

  dnl Restore environment
  CPPFLAGS="${escdf_saved_CPPFLAGS}"
  CFLAGS="${escdf_saved_CFLAGS}"
  LDFLAGS="${escdf_saved_LDFLAGS}"
  LIBS="${escdf_saved_LIBS}"

  dnl Substitute variables
  AC_SUBST(escdf_check_cflags)
  AC_SUBST(escdf_check_incs)
  AC_SUBST(escdf_check_ldflags)
  AC_SUBST(escdf_check_libs)
]) # ESCDF_SEARCH_CHECK
