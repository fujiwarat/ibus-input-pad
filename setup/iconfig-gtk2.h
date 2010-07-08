/* vim:set et sts=4: */
/* ibus-input-pad - The input pad for IBus
 * Copyright (C) 2010 Takao Fujiwara <takao.fujiwara1@gmail.com>
 * Copyright (C) 2010 Red Hat, Inc.
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

#ifndef __IBUS_INPUT_PAD_ICONFIG_H__
#define __IBUS_INPUT_PAD_ICONFIG_H__

#include <ibus.h>
#include <glib.h>

G_BEGIN_DECLS

#define IBUS_TYPE_INPUT_PAD_CONFIG             (ibus_input_pad_config_get_type ())
#define IBUS_INPUT_PAD_CONFIG(o)               (G_TYPE_CHECK_INSTANCE_CAST ((o), IBUS_TYPE_INPUT_PAD_CONFIG, IBusInputPadConfig))
#define IBUS_INPUT_PAD_CONFIG_CLASS(k)         (G_TYPE_CHECK_CLASS_CAST ((k), IBUS_TYPE_INPUT_PAD_CONFIG, IBusInputPadConfigClass))
#define IBUS_IS_INPUT_PAD_CONFIG(o)            (G_TYPE_CHECK_INSTANCE_TYPE ((o), IBUS_TYPE_INPUT_PAD_CONFIG))
#define IBUS_IS_INPUT_PAD_CONFIG_CLASS(k)      (G_TYPE_CHECK_CLASS_TYPE ((k), IBUS_TYPE_INPUT_PAD_CONFIG))
#define IBUS_INPUT_PAD_CONFIG_GET_CLASS(o)     (G_TYPE_INSTANCE_GET_CLASS ((o), IBUS_TYPE_INPUT_PAD_CONFIG, IBusInputPadConfigClass))

#define CHAR_TABLE_COMBO_BOX_DEFAULT            1
#define LAYOUT_TABLE_COMBO_BOX_DEFAULT          1
#define KEYBOARD_THEME_DEFAULT                 "default"

typedef struct _IBusInputPadConfig IBusInputPadConfig;
typedef struct _IBusInputPadConfigPrivate IBusInputPadConfigPrivate;
typedef struct _IBusInputPadConfigClass IBusInputPadConfigClass;

struct _IBusInputPadConfig {
    GObject                     parent;
    IBusInputPadConfigPrivate  *priv;
};

struct _IBusInputPadConfigClass {
    GObjectClass                parent_class;

    GObject *  (* get_apply_button)
                       (IBusInputPadConfig    *config);

    void       (* set_apply_button)
                       (IBusInputPadConfig    *config,
                        GObject               *button);

    /* Padding for future expansion */
    void (*_iconfig_reserved1) (void);
    void (*_iconfig_reserved2) (void);
    void (*_iconfig_reserved3) (void);
    void (*_iconfig_reserved4) (void);
};

IBusInputPadConfig *    ibus_bus_get_input_pad_config (IBusBus *bus);
GType                   ibus_input_pad_config_get_type (void);
gboolean                ibus_input_pad_config_get_int
                                (IBusInputPadConfig    *config,
                                 const gchar           *section,
                                 const gchar           *name,
                                 int                   *value_intp);
gboolean                ibus_input_pad_config_get_str
                                (IBusInputPadConfig    *config,
                                 const gchar           *section,
                                 const gchar           *name,
                                 gchar                **value_strp);
gboolean                ibus_input_pad_config_set_int
                                (IBusInputPadConfig    *config,
                                 const gchar           *section,
                                 const gchar           *name,
                                 int                    value_int);
gboolean                ibus_input_pad_config_set_str
                                (IBusInputPadConfig    *config,
                                 const gchar           *section,
                                 const gchar           *name,
                                 const gchar           *value_str);
gboolean                ibus_input_pad_config_commit_int
                                (IBusInputPadConfig    *config,
                                 const gchar           *section,
                                 const gchar           *name,
                                 int                    value_int);
gboolean                ibus_input_pad_config_commit_str
                                (IBusInputPadConfig    *config,
                                 const gchar           *section,
                                 const gchar           *name,
                                 const gchar           *value_str);
gboolean                ibus_input_pad_config_commit_all
                                (IBusInputPadConfig    *config);
int                     ibus_input_pad_config_get_changed_size
                                (IBusInputPadConfig      *config);

G_END_DECLS
#endif
