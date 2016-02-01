#
# Copyright (C) 2016 M. Oliveira, Y. Pouillon
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# 

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
  escdf_geometry.c \
  escdf_handler.c \
  utils.c

# Exported C headers - keep this in alphabetical order
escdf_core_hdrs = \
  escdf.h \
  escdf_geometry.h \
  escdf_handler.h

# Internal C headers - keep this in alphabetical order
escdf_hidden_hdrs = \
  util.h

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
