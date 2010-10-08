# vim:set noet ts=4:
#
# ibus-input-pad - Input pad for IBus
#
# Copyright (c) 2010 Takao Fujiwara <takao.fujiwara1@gmail.com>
# Copyright (c) 2010 Red Hat, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details. 
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

noinst_DIST = \
	$(NULL)

setupdir=$(pkgdatadir)/setup
setup_DATA=setup-gtk2.ui

INCLUDES = \
	-I$(top_srcdir)                                                 \
	-I$(top_srcdir)/engine                                          \
	-DDATAROOTDIR=\""$(datarootdir)"\"                              \
	-DIBUS_INPUT_PAD_SETUP_UI_FILE=\""$(setupdir)/$(setup_DATA)"\"  \
	$(NULL)

noinst_LTLIBRARIES = \
	libiconfig.la                                                   \
	$(NULL)

libiconfig_la_SOURCES = \
	iconfig-gtk2.c                                                  \
	iconfig-gtk2.h                                                  \
	$(NULL)

libiconfig_la_CFLAGS = \
	$(GLIB2_CFLAGS)                                                 \
	$(IBUS_CFLAGS)                                                  \
	$(NULL)

libiconfig_la_LIBADD = \
	$(GLIB2_LIBS)                                                   \
	$(IBUS_LIBS)                                                    \
	$(NULL)

libiconfig_la_LDFLAGS = \
	-export-dynamic                                                 \
	$(NULL)

libexec_PROGRAMS = ibus-setup-input-pad

ibus_setup_input_pad_SOURCES = \
	main.c                                                          \
	setup-gtk2.c                                                    \
	setup-gtk2.h                                                    \
	$(NULL)

ibus_setup_input_pad_CFLAGS = \
	$(GTK2_CFLAGS)                                                  \
	$(IBUS_CFLAGS)                                                  \
	$(INPUT_PAD_CFLAGS)                                             \
	$(NULL)

ibus_setup_input_pad_LDFLAGS = \
	$(NULL)

ibus_setup_input_pad_LDADD = \
	$(builddir)/libiconfig.la                                       \
	$(GTK2_LIBS)                                                    \
	$(IBUS_LIBS)                                                    \
	$(INPUT_PAD_LIBS)                                               \
	$(NULL)


EXTRA_DIST = \
	setup-gtk2.ui                                                   \
	$(NULL)

CLEANFILES = \
	$(NULL)
