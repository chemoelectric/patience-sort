#
# Copyright © 2022 Barry Schwartz
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License, as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received copies of the GNU General Public License
# along with this program. If not, see
# <https://www.gnu.org/licenses/>.
#

.DELETE_ON_ERROR:

ACLOCAL_AMFLAGS = -I m4 --install
AUTOMAKE_OPTIONS =
AUTOMAKE_OPTIONS += foreign
AUTOMAKE_OPTIONS += subdir-objects
#AUTOMAKE_OPTIONS += -Wno-portability

include $(top_srcdir)/aminclude_static.am
@GNU@ include $(top_srcdir)/silent-rules.mk

EXTRA_DIST =
MOSTLYCLEANFILES =
CLEANFILES =
DISTCLEANFILES =
BUILT_SOURCES =
AM_CPPFLAGS =
AM_CFLAGS =
AM_LDFLAGS =
LDADD =
bin_PROGRAMS =
lib_LTLIBRARIES =
EXTRA_PROGRAMS =
nobase_nodist_pkginclude_HEADERS =
nodist_pkgconfig_DATA =

EXTRA_DIST += $(srcdir)/silent-rules.mk
EXTRA_DIST += COPYING
EXTRA_DIST += INSTALL
EXTRA_DIST += README
EXTRA_DIST += AUTHORS

DISTCLEANFILES += Makefile
DISTCLEANFILES += GNUmakefile

AM_CPPFLAGS += -I$(builddir)
AM_CPPFLAGS += -I$(srcdir)

LDADD += $(CODE_COVERAGE_LIBS)

CODE_COVERAGE_IGNORE_PATTERN =
CODE_COVERAGE_IGNORE_PATTERN += /usr/include/*
CODE_COVERAGE_IGNORE_PATTERN += /usr/include/*/*

clean-local: code-coverage-clean tests-clean
distclean-local: code-coverage-dist-clean ats-includes-distclean

#---------------------------------------------------------------------
#
# Some GNU Make aids you might want to use.
#

comma := ,
empty :=
space := $(empty) $(empty)

# Escape things for sed expressions, etc.
escape = $(subst $$,\$$,$(subst ^,\^,$(subst ],\],$(subst [,\[,$(subst .,\.,$(subst \,\\,$(1)))))))

#--------------------------------------------------------------------------
#
# The C library.
#

lib_LTLIBRARIES += libpatience-sort.la
libpatience_sort_la_SOURCES =
libpatience_sort_la_SOURCES += patience-sort.c
libpatience_sort_la_SOURCES += patience-sort-r.c
libpatience_sort_la_SOURCES += patience-sort.h
dist_libpatience_sort_la_DEPENDENCIES =
dist_libpatience_sort_la_DEPENDENCIES += patience-sort.include.c

# PATIENCE_SORT_SATS = patience-sort/SATS/patience-sort.sats
# PATIENCE_SORT_SATS_C = patience-sort/SATS/patience-sort_sats.c
# CLEANFILES += $(PATIENCE_SORT_SATS_C)

# PATIENCE_SORT_DATS = patience-sort/DATS/patience-sort.dats
# PATIENCE_SORT_DATS_C = patience-sort/DATS/patience-sort_dats.c
# CLEANFILES += $(PATIENCE_SORT_DATS_C)

# PATIENCE_SORT_HATS = patience-sort/HATS/patience-sort.hats

# nobase_dist_pkginclude_HEADERS =
# nobase_dist_pkginclude_HEADERS += $(PATIENCE_SORT_SATS)
# nobase_dist_pkginclude_HEADERS += $(PATIENCE_SORT_DATS)
# nobase_dist_pkginclude_HEADERS += $(PATIENCE_SORT_HATS)

# nodist_libats2_patience_sort_la_SOURCES =
# nodist_libats2_patience_sort_la_SOURCES += $(PATIENCE_SORT_SATS_C)
# nodist_libats2_patience_sort_la_SOURCES += $(PATIENCE_SORT_DATS_C)

#--------------------------------------------------------------------------

# TESTS =
# TESTS += tests/try-int-sort

# EXTRA_PROGRAMS += tests/try-int-sort
# CLEANFILES += tests/try-int-sort
# CLEANFILES += tests/try-int-sort_tests.c
# tests_try_int_sort_SOURCES =
# tests_try_int_sort_SOURCES += tests/try-int-sort_tests.c
# tests_try_int_sort_DEPENDENCIES =
# tests_try_int_sort_DEPENDENCIES += tests/try-int-sort.dats
# tests_try_int_sort_DEPENDENCIES += libats2-patience-sort.la
# tests_try_int_sort_DEPENDENCIES += $(PATIENCE_SORT_HATS)
# tests_try_int_sort_CPPFLAGS =
# tests_try_int_sort_CPPFLAGS += $(AM_CPPFLAGS)
# tests_try_int_sort_CPPFLAGS += -DATS_MEMALLOC_LIBC
# tests_try_int_sort_LDADD =
# tests_try_int_sort_LDADD += libats2-patience-sort.la
# EXTRA_DIST += tests/try-int-sort.dats
# tests/try-int-sort_tests.c: tests/try-int-sort.dats

# tests-clean:
# 	-rm -f tests/*.$(OBJEXT)
# 	-rm -f tests/*.sh

#--------------------------------------------------------------------------

#nodist_pkgconfig_DATA += patience-sort.pc

#--------------------------------------------------------------------------
# local variables:
# coding: utf-8
# end:
