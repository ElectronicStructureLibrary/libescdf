#
# Top Makefile for Libescdf
#

ACLOCAL_AMFLAGS = -I config/m4

# Make sure that 'make distcheck' performs a minimal build
AM_DISTCHECK_CONFIGURE_FLAGS = \
  --disable-debug \
  --without-mpi

SUBDIRS = src doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = config/data/libvdwxc.pc

DISTCLEANFILES = \
  libescdf-*.tar.gz \
  config/data/libescdf.pc

EXTRA_DIST = config/scripts/markdown
