# vim:set et ts=4:
#
# input-pad - The input pad 
#
# Copyright (c) 2010 Takao Fujiwara <takao.fujiwara1@gmail.com>
# Copyright (c) 2010 Red Hat, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

dnl - if not 1, append datestamp to the version number.
m4_define([ibus_released], [1])
m4_define([ibus_major_version], [0])
m4_define([ibus_minor_version], [1])
m4_define([ibus_micro_version], [2])
m4_define(ibus_maybe_datestamp,
    m4_esyscmd([if test x]ibus_released[ != x1; then date +.%Y%m%d | tr -d '\n\r'; fi]))

m4_define([ibus_version],
    ibus_major_version.ibus_minor_version.ibus_micro_version[]ibus_maybe_datestamp)

AC_INIT([ibus-input-pad], [ibus_version], [http://code.google.com/p/ibus/issues/entry],[ibus-input-pad])
AM_INIT_AUTOMAKE([1.10])
AC_GNU_SOURCE

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

dnl - VERSION
libinput_pad_VERSION="ibus_major_version.ibus_minor_version"
AC_SUBST(libinput_pad_VERSION)
AC_MSG_NOTICE([$PACKAGE sub version $libinput_pad_VERSION])

dnl - define PACKAGE_VERSION_* variables
AM_SANITY_CHECK
AM_MAINTAINER_MODE
AM_DISABLE_STATIC
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_LIBTOOL
IT_PROG_INTLTOOL([0.35.0])

dnl - For dislpay Date
m4_define(ibus_datedisplay,
    m4_esyscmd(date '+%a %b %d %Y' | tr -d '\n\r'))
DATE_DISPLAY="ibus_datedisplay"
AC_SUBST(DATE_DISPLAY)

dnl - pkgconfig
AM_PATH_GLIB_2_0
PKG_CHECK_MODULES(GLIB2, [
    glib-2.0 >= 2.8
])

PKG_CHECK_MODULES(GTK2, [
    gtk+-2.0
])

m4_define([ibus_pc_name], [ibus-1.0])
PKG_CHECK_MODULES(IBUS,
   [ibus_pc_name >= 1.2.0.20100111],
   [AC_MSG_NOTICE([IBUS version is ok])],
   [PKG_CHECK_MODULES(IBUS, ibus_pc_name)
    AC_MSG_NOTICE([IBUS version is old])
    AC_DEFINE(IBUS_DEPRECATED_LANGUAGE_MENU_ITEM, 1, [Define if old ibus])]
)

PKG_CHECK_MODULES(INPUT_PAD, [
    input-pad
])

dnl - define GETTEXT_* variables
GETTEXT_PACKAGE=ibus-input-pad
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package])

AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR(IBUS_LOCALEDIR)

dnl - OUTPUT files
AC_CONFIG_FILES([ po/Makefile.in
Makefile
ibus-input-pad.spec
engine/Makefile
setup/Makefile
])

AC_OUTPUT
