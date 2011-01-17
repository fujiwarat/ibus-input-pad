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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ibus.h>
#include <glib.h>

#include "iconfig-gtk2.h"

#define IBUS_INPUT_PAD_CONFIG_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), IBUS_TYPE_INPUT_PAD_CONFIG, IBusInputPadConfigPrivate))

typedef struct _SavedTable SavedTable;

typedef enum {
    CONFIG_VALUE_TYPE_NONE = 0,
    CONFIG_VALUE_TYPE_INT,
    CONFIG_VALUE_TYPE_STRING,
} ConfigValueType;

struct _SavedTable {
    gchar                      *path;
    ConfigValueType             type;
    union {
        int                     i;
        gchar                  *str;
    } value;
};

struct _IBusInputPadConfigPrivate {
    IBusConfig                 *config;
    GHashTable                 *table;
    GObject                    *apply_button;
};

G_DEFINE_TYPE (IBusInputPadConfig, ibus_input_pad_config,
               G_TYPE_OBJECT)

static void
destroy_saved_table (SavedTable *saved_table)
{
    if (saved_table->type == CONFIG_VALUE_TYPE_STRING) {
        g_free (saved_table->value.str);
        saved_table->value.str = NULL;
    }
    g_free (saved_table);
}

static void
commit_path (gpointer key, gpointer value, gpointer user_data)
{
    IBusInputPadConfig *config;
    const gchar *path = (const gchar *) key;
    SavedTable *saved_table = (SavedTable *) value;
    gchar *p;
    gchar *section;
    gchar *name;

    g_return_if_fail (key != NULL && value != NULL);
    g_return_if_fail (IBUS_IS_INPUT_PAD_CONFIG (user_data));

    config = IBUS_INPUT_PAD_CONFIG (user_data);
    p = g_strrstr (path, "/");
    g_return_if_fail (p != NULL && p > path);
    section = g_strndup (path, p - path);
    name = g_strdup (p + 1);
    if (saved_table->type == CONFIG_VALUE_TYPE_STRING) {
        ibus_input_pad_config_commit_str (config,
                                          section,
                                          name,
                                          saved_table->value.str);
    } else {
        ibus_input_pad_config_commit_int (config,
                                          section,
                                          name,
                                          saved_table->value.i);
    }
}

static GObject *
ibus_input_pad_config_get_apply_button_real (IBusInputPadConfig      *config)
{
    g_return_val_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config), NULL);
    g_return_val_if_fail (config->priv != NULL, NULL);

    return config->priv->apply_button;
}

static void
ibus_input_pad_config_set_apply_button_real (IBusInputPadConfig      *config,
                                             GObject                 *button)
{
    g_return_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config));
    g_return_if_fail (config->priv != NULL);

    config->priv->apply_button = button;
}

static void
ibus_input_pad_config_init (IBusInputPadConfig *config)
{
    IBusInputPadConfigPrivate *priv = IBUS_INPUT_PAD_CONFIG_GET_PRIVATE (config);
    priv->table = g_hash_table_new_full (g_str_hash,
                                         g_str_equal,
                                         (GDestroyNotify) g_free,
                                         (GDestroyNotify) destroy_saved_table);
    config->priv = priv;
}

static void
ibus_input_pad_config_class_init (IBusInputPadConfigClass *klass)
{
#if 0
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
#endif

    g_type_class_add_private (klass, sizeof (IBusInputPadConfigPrivate));

    klass->get_apply_button = ibus_input_pad_config_get_apply_button_real;
    klass->set_apply_button = ibus_input_pad_config_set_apply_button_real;
}

/* currently this is a static function due to ibus_bus_get_config */
static IBusInputPadConfig *
ibus_input_pad_config_new (GDBusConnection *connection)
{
    GObject *obj;

    obj = g_object_new (IBUS_TYPE_INPUT_PAD_CONFIG,
                        NULL);

    return IBUS_INPUT_PAD_CONFIG (obj);
}

IBusInputPadConfig *
ibus_bus_get_input_pad_config (IBusBus *bus)
{
    IBusConfig *ibus_config;
    IBusInputPadConfig *config;

    ibus_config = ibus_bus_get_config (bus);
    if (ibus_config == NULL) {
        g_warning ("IBus connection failed. Maybe no ibus-daemon.");
        return NULL;
    }
    /* No idea to set parent from ibus_bus_get_config () */
    config = ibus_input_pad_config_new (NULL);
    g_return_val_if_fail (config->priv != NULL, NULL);
    config->priv->config = ibus_config;

    return config;
}

gboolean
ibus_input_pad_config_get_int (IBusInputPadConfig      *config,
                               const gchar             *section,
                               const gchar             *name,
                               int                     *value_intp)
{
    IBusConfig *ibus_config;
    GVariant *value = NULL;

    g_return_val_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config), FALSE);
    g_return_val_if_fail (config->priv != NULL, FALSE);

    ibus_config = config->priv->config;
    if ((value = ibus_config_get_value (ibus_config, section, name)) != NULL) {
        if (value_intp) {
            *value_intp = g_variant_get_int32 (value);
        }
        g_variant_unref (value);
        return TRUE;
    }
    if (!g_strcmp0 (name, "char_table_combo_box")) {
        if (value_intp) {
            *value_intp = CHAR_TABLE_COMBO_BOX_DEFAULT;
        }
        return TRUE;
    } else if (!g_strcmp0 (name, "layout_table_combo_box")) {
        if (value_intp) {
            *value_intp = LAYOUT_TABLE_COMBO_BOX_DEFAULT;
        }
        return TRUE;
    }
    return FALSE;
}

gboolean
ibus_input_pad_config_get_str (IBusInputPadConfig      *config,
                               const gchar             *section,
                               const gchar             *name,
                               gchar                  **value_strp)
{
    IBusConfig *ibus_config;
    GVariant *value = NULL;

    g_return_val_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config), FALSE);
    g_return_val_if_fail (config->priv != NULL, FALSE);

    ibus_config = config->priv->config;
    if ((value = ibus_config_get_value (ibus_config, section, name)) != NULL) {
        if (value_strp) {
            *value_strp = g_variant_dup_string (value, NULL);
        }
        g_variant_unref (value);
        return TRUE;
    }
    if (!g_strcmp0 (name, "keyboard_theme")) {
        if (value_strp) {
            *value_strp = g_strdup (KEYBOARD_THEME_DEFAULT);
        }
        return TRUE;
    }
    return FALSE;
}

#if 0
gboolean
ibus_input_pad_config_get_value (IBusConfig    *config,
                                 const gchar   *section,
                                 const gchar   *name,
                                 GVariant      *value)
{
    if ((value = ibus_config_get_value (config, section, name)) != NULL) {
        return TRUE;
    }
    if (!g_strcmp0 (name, "keyboard_theme")) {
        value = g_variant_new ("s", KEYBOARD_THEME_DEFAULT);
        return TRUE;
    } else if (!g_strcmp0 (name, "char_table_combo_box")) {
        value = g_variant_new ("i", CHAR_TABLE_COMBO_BOX_DEFAULT);
        return TRUE;
    } else if (!g_strcmp0 (name, "layout_table_combo_box")) {
        value = g_variant_new ("i", LAYOUT_TABLE_COMBO_BOX_DEFAULT);
        return TRUE;
    }
    return FALSE;
}
#endif

gboolean
ibus_input_pad_config_set_int (IBusInputPadConfig      *config,
                               const gchar             *section,
                               const gchar             *name,
                               int                      value_int)
{
    IBusConfig *ibus_config;
    GVariant *value = NULL;
    int orig_i = 0;
    gchar *path;
    SavedTable *saved_table;

    g_return_val_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config), FALSE);
    g_return_val_if_fail (config->priv != NULL, FALSE);
    g_return_val_if_fail (section != NULL && name != NULL, FALSE);

    ibus_config = config->priv->config;
    if ((value = ibus_config_get_value (ibus_config, section, name)) != NULL) {
        orig_i = g_variant_get_int32 (value);
        g_variant_unref (value);
        if (value_int == orig_i) {
            return FALSE;
        }
    } else if (!g_strcmp0 (name, "char_table_combo_box")) {
        if (value_int == CHAR_TABLE_COMBO_BOX_DEFAULT) {
            return FALSE;
        }
    } else if (!g_strcmp0 (name, "layout_table_combo_box")) {
        if (value_int == LAYOUT_TABLE_COMBO_BOX_DEFAULT) {
            return FALSE;
        }
    }

    path = g_build_path ("/", section, name, NULL);
    saved_table = g_new0 (SavedTable, 1);
    saved_table->path = g_strdup (path);
    saved_table->type = CONFIG_VALUE_TYPE_INT;
    saved_table->value.i = value_int;
    if (g_hash_table_lookup (config->priv->table, path)) {
        g_hash_table_replace (config->priv->table,
                              (gpointer) g_strdup (path),
                              (gpointer) saved_table);
    } else {
        g_hash_table_insert (config->priv->table,
                             (gpointer) g_strdup (path),
                             (gpointer) saved_table);
    }
    g_free (path);

    return TRUE;
}

gboolean
ibus_input_pad_config_set_str (IBusInputPadConfig      *config,
                               const gchar             *section,
                               const gchar             *name,
                               const gchar             *value_str)
{
    IBusConfig *ibus_config;
    GVariant *value = NULL;
    const gchar *orig_str = NULL;
    gchar *path;
    SavedTable *saved_table;

    g_return_val_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config), FALSE);
    g_return_val_if_fail (config->priv != NULL, FALSE);
    g_return_val_if_fail (section != NULL && name != NULL, FALSE);

    ibus_config = config->priv->config;
    if ((value = ibus_config_get_value (ibus_config, section, name)) != NULL) {
        orig_str = g_variant_get_string (value, NULL);
        if (!g_strcmp0 (value_str, orig_str)) {
            g_variant_unref (value);
            return FALSE;
        }
        g_variant_unref (value);
    } else if (!g_strcmp0 (name, "keyboard_theme")) {
        if (value_str == NULL ||
            !g_strcmp0 (value_str, KEYBOARD_THEME_DEFAULT)) {
            return FALSE;
        }
    }

    path = g_build_path ("/", section, name, NULL);
    saved_table = g_new0 (SavedTable, 1);
    saved_table->path = g_strdup (path);
    saved_table->type = CONFIG_VALUE_TYPE_STRING;
    if (value_str) {
        saved_table->value.str = g_strdup (value_str);
    }
    if (g_hash_table_lookup (config->priv->table, path)) {
        g_hash_table_replace (config->priv->table,
                              (gpointer) g_strdup (path),
                              (gpointer) saved_table);
    } else {
        g_hash_table_insert (config->priv->table,
                             (gpointer) g_strdup (path),
                             (gpointer) saved_table);
    }
    g_free (path);

    return TRUE;
}

gboolean
ibus_input_pad_config_commit_int (IBusInputPadConfig      *config,
                                  const gchar             *section,
                                  const gchar             *name,
                                  int                      value_int)
{
    IBusConfig *ibus_config;
    GVariant *value = NULL;

    g_return_val_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config), FALSE);
    g_return_val_if_fail (config->priv != NULL, FALSE);
    g_return_val_if_fail (section != NULL && name != NULL, FALSE);

    ibus_config = config->priv->config;
    value = g_variant_new ("i", value_int);
    if (!ibus_config_set_value (ibus_config, section, name, value)) {
        return FALSE;
    }
    g_variant_unref (value);
    return TRUE;
}

gboolean
ibus_input_pad_config_commit_str (IBusInputPadConfig      *config,
                                  const gchar             *section,
                                  const gchar             *name,
                                  const gchar             *value_str)
{
    IBusConfig *ibus_config;
    GVariant *value = NULL;

    g_return_val_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config), FALSE);
    g_return_val_if_fail (config->priv != NULL, FALSE);
    g_return_val_if_fail (section != NULL && name != NULL, FALSE);

    ibus_config = config->priv->config;
    value = g_variant_new ("s", value_str);
    if (!ibus_config_set_value (ibus_config, section, name, value)) {
        return FALSE;
    }
    g_variant_unref (value);
    return TRUE;
}

gboolean
ibus_input_pad_config_commit_all (IBusInputPadConfig      *config)
{
    g_return_val_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config), FALSE);
    g_return_val_if_fail (config->priv != NULL, FALSE);

    g_hash_table_foreach (config->priv->table, commit_path, config);
    g_hash_table_remove_all (config->priv->table);
    return TRUE;
}

int
ibus_input_pad_config_get_changed_size (IBusInputPadConfig      *config)
{
    g_return_val_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config), 0);
    g_return_val_if_fail (config->priv != NULL, 0);

    return g_hash_table_size (config->priv->table);
}
