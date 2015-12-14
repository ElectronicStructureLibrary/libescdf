# -*- Autoconf -*-
#
# Copyright (C) 2015 Yann Pouillon
#
# This file is part of the Libescdfxc software package. For license information,
# please see the COPYING file in the top-level directory of the Libescdfxc source
# distribution.
#



# ESCDF_MPI_DETECT()
# ------------------
#
# Chechks whether the configured MPI implementation is working.
#
AC_DEFUN([ESCDF_MPI_DETECT], [
  dnl Init
  escdf_mpi_ok="unknown"

  dnl Display current MPI status
  AC_MSG_CHECKING([how MPI parameters have been set])
  AC_MSG_RESULT([${escdf_mpi_type}])
  AC_MSG_CHECKING([whether the MPI C compiler is set])
  AC_MSG_RESULT([${escdf_mpi_cc_set}])
  AC_MSG_CHECKING([whether the MPI C compiler is wrapped])
  AC_MSG_RESULT([${escdf_mpi_cc_wrap}])
  AC_MSG_CHECKING([whether the MPI Fortran compiler is set])
  AC_MSG_RESULT([${escdf_mpi_fc_set}])
  AC_MSG_CHECKING([whether the MPI Fortran compiler is wrapped])
  AC_MSG_RESULT([${escdf_mpi_fc_wrap}])

  dnl Warn if serial component of wrapped compilers supports MPI
  if test "${escdf_mpi_cc_wrap}" = "yes"; then
    AC_MSG_NOTICE([validating that '${escdf_sercc}' is indeed serial])
    AC_MSG_NOTICE([please ignore possible warnings about mpi.h not found])
    _ESCDF_MPI_CHECK_CC([${escdf_sercc}])
    if test "${escdf_mpi_cc_ok}" = "yes"; then
      AC_MSG_WARN([the serial C compiler is MPI-aware
                    Your current configuration is probably ill-defined.
                    The build will likely fail.])
      sleep 5
    fi
  fi
  if test "${escdf_mpi_fc_wrap}" = "yes"; then
    AC_MSG_NOTICE([validating that '${escdf_serfc}' is indeed serial])
    _ESCDF_MPI_CHECK_FC([${escdf_serfc}])
    if test "${escdf_mpi_fc_ok}" = "yes"; then
      AC_MSG_WARN([the serial Fortran compiler is MPI-aware
                    Your current configuration is probably ill-defined.
                    The build will likely fail.])
      sleep 5
    fi
  fi

  dnl Test MPI compilers
  _ESCDF_MPI_CHECK_CC([${CC}])
  if test "${escdf_mpi_cc_ok}" = "yes"; then
    _ESCDF_MPI_CHECK_FC([${FC}])
  fi

  dnl Take final decision
  AC_MSG_CHECKING([whether we have a full MPI support])
  if test "${escdf_mpi_cc_ok}" = "yes" -a \
          "${escdf_mpi_fc_ok}" = "yes"; then
    escdf_mpi_ok="yes"
  else
    escdf_mpi_ok="no"
  fi
  AC_MSG_RESULT([${escdf_mpi_ok}])
]) # ESCDF_MPI_DETECT



# ESCDF_MPI_INIT()
# ----------------
#
# Initializes MPI parameters.
#
AC_DEFUN([ESCDF_MPI_INIT], [
  if test "${escdf_mpi_enable}" != "no"; then
    AC_MSG_CHECKING([how MPI parameters have been set])
    AC_MSG_RESULT([${escdf_mpi_type}])
    if test "${escdf_mpi_type}" = "env"; then
      _AC_SRCDIRS(["."])
    fi
    _ESCDF_MPI_INIT_CC
    _ESCDF_MPI_INIT_FC
  fi
]) # ESCDF_MPI_INIT



                    ########################################



# _ESCDF_MPI_CHECK_CC(CC)
# -----------------------
#
# Check whether the MPI C compiler is working.
#
AC_DEFUN([_ESCDF_MPI_CHECK_CC], [
  dnl Init
  escdf_mpi_cc_ok="unknown"
  escdf_mpi_cc_has_funs="unknown"
  escdf_mpi_cc_has_hdrs="unknown"

  dnl Prepare environment
  escdf_saved_CC="${CC}"
  CC="$1"
  tmp_mpi_header=mpi.h
  tmp_mpi_cache=AS_TR_SH([ac_cv_header_${tmp_mpi_header}])
  ${as_unset} ${tmp_mpi_cache}

  dnl Look for C includes
  AC_LANG_PUSH([C])
  AC_CHECK_HEADERS([mpi.h],
    [escdf_mpi_cc_has_hdrs="yes"], [escdf_mpi_cc_has_hdrs="no"])
  AC_LANG_POP([C])

  dnl Look for C functions
  if test "${escdf_mpi_cc_has_hdrs}" = "yes"; then
    AC_CHECK_FUNC([MPI_Init], [escdf_mpi_cc_has_funs="yes"],
      [escdf_mpi_cc_has_funs="no"])
  fi

  dnl Validate C support
  AC_MSG_CHECKING([whether the MPI C compiler works])
  if test "${escdf_mpi_cc_has_funs}" = "yes" -a \
          "${escdf_mpi_cc_has_hdrs}" = "yes"; then
    escdf_mpi_cc_ok="yes"
  else
    escdf_mpi_cc_ok="no"
  fi
  AC_MSG_RESULT([${escdf_mpi_cc_ok}])

  dnl Restore environment
  CC="${escdf_saved_CC}"
  unset tmp_mpi_cache
  unset tmp_mpi_header
]) # _ESCDF_MPI_CHECK_CC



# _ESCDF_MPI_CHECK_FC(FC)
# -----------------------
#
# Check whether the MPI Fortran compiler is working.
#
AC_DEFUN([_ESCDF_MPI_CHECK_FC], [
  dnl Init
  escdf_mpi_fc_ok="unknown"
  escdf_mpi_fc_has_funs="unknown"
  escdf_mpi_fc_has_mods="unknown"

  dnl Prepare environment
  escdf_saved_FC="${FC}"
  FC="$1"

  dnl Look for Fortran modules
  AC_LANG_PUSH([Fortran])
  AC_MSG_CHECKING([for a Fortran MPI module])
  AC_LINK_IFELSE([AC_LANG_PROGRAM([],
    [[
      use mpi
    ]])], [escdf_mpi_fc_has_mods="yes"], [escdf_mpi_fc_has_mods="no"])
  AC_MSG_RESULT([${escdf_mpi_fc_has_mods}])
  AC_LANG_POP([Fortran])

  dnl Look for Fortran functions
  if test "${escdf_mpi_fc_has_mods}" = "yes"; then
    AC_LANG_PUSH([Fortran])
    AC_MSG_CHECKING([for a Fortran MPI_Init])
    AC_LINK_IFELSE([AC_LANG_PROGRAM([],
      [[
        use mpi
        integer :: ierr
        call mpi_init(ierr)
      ]])], [escdf_mpi_fc_has_funs="yes"], [escdf_mpi_fc_has_funs="no"])
    AC_MSG_RESULT([${escdf_mpi_fc_has_funs}])
    AC_LANG_POP([Fortran])
  fi

  dnl Validate Fortran support
  AC_MSG_CHECKING([whether the MPI Fortran compiler works])
  if test "${escdf_mpi_fc_has_funs}" = "yes" -a \
          "${escdf_mpi_fc_has_mods}" = "yes"; then
    escdf_mpi_fc_ok="yes"
  else
    escdf_mpi_fc_ok="no"
  fi
  AC_MSG_RESULT([${escdf_mpi_fc_ok}])

  dnl Restore environment
  FC="${escdf_saved_FC}"
]) # _ESCDF_MPI_CHECK_FC



# _ESCDF_MPI_INIT_CC()
# --------------------
#
# Initializes MPI parameters related to the C compiler.
#
AC_DEFUN([_ESCDF_MPI_INIT_CC], [
  dnl Init
  escdf_sercc="${CC}"
  escdf_mpicc=""
  escdf_mpi_cc_set="no"
  escdf_mpi_cc_wrap="unknown"

  dnl Look for a MPI C compiler
  case "${escdf_mpi_type}" in

    def)
      escdf_mpi_cc_wrap="no"
      ;;

    dir)
      escdf_mpicc="${with_mpi}/bin/mpicc"
      if test -x "${escdf_mpicc}"; then
        AC_MSG_CHECKING([for an executable MPI C compiler])
        AC_MSG_RESULT([${escdf_mpicc}])
        if test "${escdf_sercc}" = ""; then
          AC_MSG_NOTICE([setting CC to '${escdf_mpicc}'])
          CC="${escdf_mpicc}"
          escdf_mpi_cc_set="yes"
          escdf_mpi_cc_wrap="no"
        else
          escdf_mpi_cc_wrap="yes"
        fi
      else
        AC_MSG_ERROR([MPI C compiler not found in ${with_mpi}/bin])
      fi
      ;;

    env|yon)
      if test -n "${MPICC}"; then
        escdf_mpicc="${MPICC}"
      else
        AC_CHECK_PROGS([escdf_mpicc], [mpicc])
      fi
      if test -n "${escdf_sercc}" -a -n "${escdf_mpicc}"; then
        escdf_mpi_cc_wrap="yes"
      elif test -n "${escdf_mpicc}"; then
        AC_MSG_NOTICE([setting CC to '${escdf_mpicc}'])
        CC="${escdf_mpicc}"
        escdf_mpi_cc_set="yes"
        escdf_mpi_cc_wrap="no"
      fi
      ;;

  esac

  if test "${escdf_mpi_cc_wrap}" = "yes"; then
    _ESCDF_MPI_CREATE_WRAPPER([CC], [${escdf_sercc}], [${escdf_mpicc}])
    escdf_mpi_cc_set="yes"
  fi
]) # _ESCDF_MPI_INIT_CC



# _ESCDF_MPI_INIT_FC()
# --------------------
#
# Initializes MPI parameters related to the Fortran compiler.
#
AC_DEFUN([_ESCDF_MPI_INIT_FC], [
  dnl Init
  escdf_serfc="${FC}"
  escdf_mpifc=""
  escdf_mpi_fc_set="no"
  escdf_mpi_fc_wrap="unknown"

  dnl Look for a MPI Fortran compiler
  case "${escdf_mpi_type}" in

    def)
      escdf_mpi_fc_wrap="no"
      ;;

    dir)
      escdf_mpifc="${with_mpi}/bin/mpif90"
      if test -x "${escdf_mpifc}"; then
        AC_MSG_CHECKING([for an executable MPI Fortran compiler])
        AC_MSG_RESULT([${escdf_mpifc}])
        if test "${escdf_serfc}" = ""; then
          AC_MSG_NOTICE([setting FC to '${escdf_mpifc}'])
          FC="${escdf_mpifc}"
          escdf_mpi_fc_set="yes"
          escdf_mpi_fc_wrap="no"
        else
          escdf_mpi_fc_wrap="yes"
        fi
      else
        AC_MSG_ERROR([MPI Fortran compiler not found in ${with_mpi}/bin])
      fi
      ;;

    env|yon)
      if test -n "${MPIFC}"; then
        escdf_mpifc="${MPIFC}"
      else
        AC_CHECK_PROGS([escdf_mpifc], [mpif90 mpif95])
      fi
      if test -n "${escdf_serfc}" -a -n "${escdf_mpifc}"; then
        escdf_mpi_fc_wrap="yes"
      elif test -n "${escdf_mpifc}"; then
        AC_MSG_NOTICE([setting FC to '${escdf_mpifc}'])
        FC="${escdf_mpifc}"
        escdf_mpi_fc_set="yes"
        escdf_mpi_fc_wrap="no"
      fi
      ;;

  esac

  if test "${escdf_mpi_fc_wrap}" = "yes"; then
    _ESCDF_MPI_CREATE_WRAPPER([FC], [${escdf_serfc}], [${escdf_mpifc}])
    escdf_mpi_fc_set="yes"
  fi
]) # _ESCDF_MPI_INIT_FC



# _ESCDF_MPI_CREATE_WRAPPER(COMPILER_TYPE, SERIAL_COMPILER, MPI_COMPILER)
# -----------------------------------------------------------------------
#
# Creates a wrapper for MPI compilers when they can be configured to
# accept different serial compilers.
#
# Note: it is impossible to set two compiler levels with the Autotools,
#       because Automake requires CC, CXX, and FC to be set to
#       the actual compilers.
#
AC_DEFUN([_ESCDF_MPI_CREATE_WRAPPER], [
  dnl Init
  tmp_comp_name=`echo "$1" | sed -e 's/.*/\L&/'`
  ${MKDIR_P} config/wrappers

  dnl Create file
  cat >config/wrappers/wrap-mpi${tmp_comp_name} <<EOF
#!/bin/sh

$1="$2"
export $1

$3 \[$]{*}
EOF

  dnl Fix permissions
  chmod u+x config/wrappers/wrap-mpi${tmp_comp_name}

  dnl Overwrite compiler setting
  eval tmp_wrapper_path="${ac_abs_top_builddir}/config/wrappers/wrap-mpi${tmp_comp_name}"
  tmp_wrapper_name=`basename "${tmp_wrapper_path}"`
  AC_MSG_NOTICE([wrapping serial and MPI compilers into ${tmp_wrapper_name}])
  $1="${tmp_wrapper_path}"

  dnl Clean-up
  unset tmp_comp_name
  unset tmp_wrapper_name
  unset tmp_wrapper_path
]) # _ESCDF_MPI_CREATE_WRAPPER
