#
# Makefile for the ESCDF test data
#
# Purpose: make tests possible when the source tree is read-only
#

# List of data files used by tests
escdf_data_files = \
  grid_scalarfield_read.h5

# Data files must be explicitly copied to build dirs to avoid errors
# with 'make distcheck'
all-local:
	for h5f in $(escdf_data_files); do \
	  if test ! -e $$h5f; then \
	    cp -f $(srcdir)/$$h5f .; \
	    chmod 644 $$h5f; \
	    echo "$$h5f" >>escdf_cleanfiles.tmp; \
	  fi; \
	done

# Copied files must be removed to have 'make distcleancheck' succeed
clean-local:
	if test -s escdf_cleanfiles.tmp; then \
	  rm `cat escdf_cleanfiles.tmp`; \
	  rm escdf_cleanfiles.tmp; \
	fi

# Append data files not used by tests to this list
EXTRA_DIST = $(escdf_data_files)
