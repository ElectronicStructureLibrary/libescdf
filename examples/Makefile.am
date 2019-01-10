EXTRA_PROGRAMS = grid groups_example groups_read_example

grid_SOURCES = grid_scalarfield.c
grid_LDADD = $(top_builddir)/src/libescdf.la

groups_example_SOURCES = groups_example.c
groups_example_LDADD = $(top_builddir)/src/libescdf.la

groups_read_example_SOURCES = groups_read_example.c
groups_read_example_LDADD = $(top_builddir)/src/libescdf.la

AM_CPPFLAGS = -I$(top_srcdir)/src

CLEANFILES = $(EXTRA_PROGRAMS) grid_scalarfield.*h5
