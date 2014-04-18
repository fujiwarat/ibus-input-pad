/* vim:set et sts=4: */
/* ibus-input-pad - Input pad for IBus
 * Copyright (C) 2010-2011 Takao Fujiwara <takao.fujiwara1@gmail.com>
 * Copyright (C) 2010-2011 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __IBUS_INPUT_PAD_SETUP_GTK2_H__
#define __IBUS_INPUT_PAD_SETUP_GTK2_H__

#include <ibus.h>
#include <gtk/gtk.h>

#include "iconfig-gtk.h"

void            ibus_input_pad_setup_gtk_init (int *argc, char ***argv);
GtkWidget *     ibus_input_pad_setup_gtk_dialog_new (IBusInputPadConfig *config);
gboolean        ibus_input_pad_setup_gtk_dialog_run (GtkWidget *dialog);
void            ibus_input_pad_setup_gtk_dialog_destroy (GtkWidget *dialog);

#endif
