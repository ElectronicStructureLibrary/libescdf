#
# Copyright (C) 2015-2017 Damien Caliste <dcaliste@free.fr>
#                         Fabiano Corsetti <fabiano.corsetti@gmail.com>
#                         Micael Oliveira <micael.oliveira@mpsd.mpg.de>
#                         Yann Pouillon <devops@materialsevolution.es>
#
# This file is part of ESCDF.
#
# ESCDF is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, version 2.1 of the License, or (at your option) any
# later version.
#
# ESCDF is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with ESCDF.  If not, see <http://www.gnu.org/licenses/> or write to the
# Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301  USA.

#
# Makefile for the source of Libescdf
#


                    # ------------------------------------ #

#
# Source files
#

# C source - keep this in alphabetical order
escdf_core_srcs = \
  escdf.c \
  escdf_attributes.c \
  escdf_datatransfer.c \
  escdf_datasets.c \
  escdf_lookuptable.c \
  escdf_error.c \
  escdf_group.c \
  escdf_handle.c \
  escdf_hl.c \
  escdf_info.c \
  utils.c \
  utils_hdf5.c

#  escdf_grid_scalarfields.c 
#  escdf_system.c 

# Exported C headers - keep this in alphabetical order
escdf_core_hdrs = \
  escdf.h \
  escdf_attributes.h \
  escdf_attributes_ID.h \
  escdf_attributes_specs.h \
  escdf_common.h \
  escdf_datasets_ID.h \
  escdf_datasets_specs.h \
  escdf_datatransfer.h \
  escdf_error.h \
  escdf_group.h \
  escdf_groups_ID.h \
  escdf_groups_specs.h \
  escdf_handle.h \
  escdf_hl.h \
  escdf_info.h 

#  escdf_grid_scalarfields.h 
#  escdf_system.h

# Internal C headers - keep this in alphabetical order
escdf_hidden_hdrs = \
  utils.h \
  utils_hdf5.h \
  escdf_lookuptable.h \
  escdf_private_group.h

                    # ------------------------------------ #

#
# Build targets
#

# Essential build parameters
AM_CPPFLAGS = @escdf_core_incs@

# Main libraries
lib_LTLIBRARIES = libescdf.la

libescdf_la_LDFLAGS = -version-info 0:0:0
libescdf_la_SOURCES = $(escdf_core_srcs)

# Headers
include_HEADERS = $(escdf_core_hdrs)
noinst_HEADERS = $(escdf_hidden_hdrs)

escdf_spec_headers = \
  escdf_attributes_ID.h \
  escdf_attributes_specs.h \
  escdf_datasets_ID.h \
  escdf_datasets_specs.h \
  escdf_groups_ID.h \
  escdf_groups_specs.h

BUILT_SOURCES = $(escdf_spec_headers)

$(escdf_spec_headers): intermediate.json ;

.INTERMEDIATE: intermediate.json
intermediate.json: generate_attributes_from_JSON.py attributes_def.json
	python $(srcdir)/generate_attributes_from_JSON.py $(srcdir)/attributes_def.json

                    # ------------------------------------ #

#
# Test programs
#

escdf_basic_tests = \
  check_escdf
escdf_gcov_pre_tests = \
  gcov_clean
escdf_gcov_post_tests = \
  gcov_report
escdf_memprof_tests = \
  test_memory

if HAVE_CHECK
check_PROGRAMS = $(escdf_basic_tests)
endif
nodist_check_SCRIPTS = \
  $(escdf_gcov_pre_tests) \
  $(escdf_gcov_post_tests) \
  $(escdf_memprof_tests)

check_escdf_SOURCES = \
  check_escdf.h \
  check_escdf.c \
  check_escdf_attributes.c \
  check_escdf_datasets.c \
  check_escdf_error.c \
  check_escdf_group.c \
  check_escdf_handle.c \
  check_escdf_info.c \
  check_utils.c \
  check_utils_hdf5.c

#  check_escdf_grid_scalarfields.c 
#  check_escdf_system.c


check_escdf_CPPFLAGS = -I$(top_srcdir)/src @escdf_check_incs@
check_escdf_CFLAGS = @escdf_check_cflags@
check_escdf_LDFLAGS = @escdf_check_ldflags@
check_escdf_LDADD = -lescdf $(LIBS_COVERAGE) @escdf_check_libs@
check_escdf_DEPENDENCIES = libescdf.la

if HAVE_CHECK
TESTS = \
  $(escdf_gcov_pre_tests) \
  $(escdf_basic_tests) \
  $(escdf_gcov_post_tests) \
  $(escdf_memprof_tests)
endif

                    # ------------------------------------ #

#
# Explicit dependencies
#

-include gcov-tests-deps.mf

                    # ------------------------------------ #
#
# Clean targets
#

# Temporary test outputs
MOSTLYCLEANFILES = *.gcda *.gcno *.gcov test_*.tmp

# Binary files generated during the tests
CLEANFILES = \
  $(escdf_spec_headers) \
  tmp_grid_scalarfield_read.h5 \
  tmp_grid_scalarfield_test_file.h5 \
  tmp_grid_scalarfield_write.h5
