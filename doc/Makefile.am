#
# Makefile for the documentation of Libescdf
#

# Markdown sources
escdf_mddocs = \
  configuring-libescdf.md \
  hacking-the-build-system.md

# HTML documents
escdf_htmldocs = \
  configuring-libescdf.html \
  hacking-the-build-system.html

                    ########################################

# User-defined rules
MARKDOWN = $(top_srcdir)/config/scripts/markdown
MDFLAGS  = 

SUFFIXES = .md .html
.md.html:
	sed -e "s/@TITLE@/$$(head -n 1 $<)/" \
	  $(srcdir)/data/escdf_header.html >$@
	$(MARKDOWN) $(MDFLAGS) $< >>$@
	cat $(srcdir)/data/escdf_footer.html >>$@

# Targets
htmldocdir = $(docdir)/html
htmldoc_DATA = $(escdf_htmldocs)

# Inform Automake
CLEANFILES = $(escdf_htmldocs)
EXTRA_DIST = data/escdf_header.html data/escdf_footer.html $(escdf_mddocs)
