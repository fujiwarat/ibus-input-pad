/* vim:set et sts=4: */
/* ibus-input-pad - Input pad for IBus
 * Copyright (C) 2010-2025 Takao Fujiwara <takao.fujiwara1@gmail.com>
 * Copyright (C) 2010-2025 Red Hat, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ibus.h>
#include <gtk/gtk.h>

#ifdef ENABLE_NLS
#include <libintl.h>
#endif

#include "iconfig-gtk.h"
#include "setup-gtk.h"

static IBusInputPadConfig *
get_config (void)
{
    IBusBus *bus;
    IBusInputPadConfig *config;

    ibus_init ();
    bus = ibus_bus_new ();
    config = ibus_bus_get_input_pad_config (bus);

    return config;
}

int
main (int argc, char *argv[])
{
    IBusInputPadConfig *config;
    GtkWidget *dialog;

#ifdef ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
#endif

    ibus_input_pad_setup_gtk_init (&argc, &argv);

    config = get_config ();
    g_assert (config != NULL);
    dialog = ibus_input_pad_setup_gtk_dialog_new (config);
    ibus_input_pad_setup_gtk_dialog_run (dialog);
    ibus_input_pad_setup_gtk_dialog_destroy (dialog);

    return 0;
}
