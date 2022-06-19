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
distclean-local: code-coverage-dist-clean

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
libpatience_sort_la_DEPENDENCIES =
libpatience_sort_la_DEPENDENCIES += patience-sort.include.c
EXTRA_DIST += patience-sort.include.c

include_HEADERS =
include_HEADERS += patience-sort.h

#--------------------------------------------------------------------------

TESTS =
TESTS += tests/try-int-sort
TESTS += tests/try-stable-sort

EXTRA_PROGRAMS += tests/try-int-sort
CLEANFILES += tests/try-int-sort
tests_try_int_sort_SOURCES =
tests_try_int_sort_SOURCES += tests/try-int-sort.c
tests_try_int_sort_DEPENDENCIES =
tests_try_int_sort_DEPENDENCIES += libpatience-sort.la
tests_try_int_sort_CPPFLAGS =
tests_try_int_sort_CPPFLAGS += $(AM_CPPFLAGS)
tests_try_int_sort_LDADD =
tests_try_int_sort_LDADD += libpatience-sort.la

EXTRA_PROGRAMS += tests/try-stable-sort
CLEANFILES += tests/try-stable-sort
tests_try_stable_sort_SOURCES =
tests_try_stable_sort_SOURCES += tests/try-stable-sort.c
tests_try_stable_sort_DEPENDENCIES =
tests_try_stable_sort_DEPENDENCIES += libpatience-sort.la
tests_try_stable_sort_CPPFLAGS =
tests_try_stable_sort_CPPFLAGS += $(AM_CPPFLAGS)
tests_try_stable_sort_LDADD =
tests_try_stable_sort_LDADD += libpatience-sort.la

tests-clean:
	-rm -f tests/*.$(OBJEXT)
	-rm -f tests/*.sh

#--------------------------------------------------------------------------

nodist_pkgconfig_DATA += patience-sort.pc

#--------------------------------------------------------------------------
# local variables:
# coding: utf-8
# end:
