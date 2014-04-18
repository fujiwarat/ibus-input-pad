# vim:set noet ts=4:
#
# ibus-input-pad - Input pad for IBus
#
# Copyright (c) 2010-2014 Takao Fujiwara <takao.fujiwara1@gmail.com>
# Copyright (c) 2010-2014 Red Hat, Inc.
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

AM_CPPFLAGS = \
	-I$(top_srcdir)                                                 \
	-I$(top_srcdir)/engine                                          \
	-DDATAROOTDIR=\""$(datarootdir)"\"                              \
	$(NULL)

noinst_LTLIBRARIES = \
	libiconfig.la                                                   \
	$(NULL)

libiconfig_la_SOURCES = \
	iconfig-gtk.c                                                   \
	iconfig-gtk.h                                                   \
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
	setup-gtk.c                                                     \
	setup-gtk.h                                                     \
	resources.c                                                     \
	$(NULL)

ibus_setup_input_pad_CFLAGS = \
	$(GTK3_CFLAGS)                                                  \
	$(IBUS_CFLAGS)                                                  \
	$(INPUT_PAD_CFLAGS)                                             \
	$(NULL)

ibus_setup_input_pad_LDFLAGS = \
	$(NULL)

ibus_setup_input_pad_LDADD = \
	$(builddir)/libiconfig.la                                       \
	$(GTK3_LIBS)                                                    \
	$(IBUS_LIBS)                                                    \
	$(INPUT_PAD_LIBS)                                               \
	$(NULL)

resources.c: ibus-setup-input-pad.gresource.xml
	$(GLIB_COMPILE_RESOURCES) $< --target=$@ --generate-source

desktop_in_in_files = ibus-setup-input-pad.desktop.in.in
desktop_in_files = $(desktop_in_in_files:.in.in=.in)

$(desktop_in_files): %.desktop.in: %.desktop.in.in Makefile
	$(AM_V_GEN) sed \
	    -e "s|\@libexecdir\@|$(libexecdir)|" \
	    -e "s|\@datadir\@|$(datadir)|" \
	    $< > $@.tmp && mv $@.tmp $@

desktopdir=$(datadir)/applications
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

MAINTAINERCLEANFILES = resources.c

EXTRA_DIST = \
	$(desktop_in_in_files)                                          \
	ibus-setup-input-pad.gresource.xml                              \
	setup-gtk.ui                                                    \
	$(NULL)

CLEANFILES = \
	$(desktop_DATA)                                                 \
	$(desktop_in_files)                                             \
	$(NULL)
