/* vim:set et sts=4: */
/* ibus-input-pad - Input pad for IBus
 * Copyright (C) 2010-2014 Takao Fujiwara <takao.fujiwara1@gmail.com>
 * Copyright (C) 2010-2014 Red Hat, Inc.
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
#include <stdlib.h> /* exit */

#include <input-pad.h>
#include <input-pad-group.h>
#include "engine.h"
#include "i18n.h"
#include "iconfig-gtk.h"

typedef struct _IBusInputPadEngine IBusInputPadEngine;
typedef struct _IBusInputPadEngineClass IBusInputPadEngineClass;

struct _IBusInputPadEngine {
    IBusEngine      parent;

    /* members */
    IBusPropList   *prop_list;
    GSList         *str_list;
};

struct _IBusInputPadEngineClass {
    IBusEngineClass parent;
};

static void     on_window_activated         (gpointer          window_data,
                                             gpointer          data);
static void     set_keyboard_only_kbdui     (void             *window);
static void     set_default_kbdui           (void             *window);
static void     show_keyboard_only_table    (void             *window);
static void     show_default_table          (void             *window);
static GObject* ibus_input_pad_engine_constructor
                                            (GType                   type,
                                             guint                   n_construct_params,
                                             GObjectConstructParam  *construct_params);
static void ibus_input_pad_engine_destroy   (IBusObject       *object);
static void ibus_input_pad_engine_commit_str
                                            (IBusEngine	       *engine,
                                             const gchar       *str);
static gboolean ibus_input_pad_engine_process_key_event
                                             (IBusEngine       *engine,
                                              guint             keyval,
                                              guint             keycode,
                                              guint             modifiers);
static void ibus_input_pad_engine_enable    (IBusEngine        *engine);
static void ibus_input_pad_engine_disable   (IBusEngine        *engine);
static void ibus_input_pad_engine_focus_in  (IBusEngine        *engine);
static void ibus_input_pad_engine_focus_out (IBusEngine        *engine);
static void ibus_input_pad_engine_property_activate
                                            (IBusEngine        *engine,
                                             const char        *prop_name,
                                             guint              prop_state);


static IBusEngineClass *parent_class = NULL;
static void *input_pad_window;
static IBusInputPadConfig *config = NULL;

static unsigned int
on_window_button_pressed (gpointer window_data,
                          gchar   *str,
                          guint    type,
                          guint    keysym,
                          guint    keycode,
                          guint    state,
                          gpointer data)
{
    IBusInputPadEngine *engine = (IBusInputPadEngine *) data;

    if (str == NULL ||
        ((InputPadTableType) type != INPUT_PAD_TABLE_TYPE_CHARS &&
         (InputPadTableType) type != INPUT_PAD_TABLE_TYPE_STRINGS &&
         (InputPadTableType) type != INPUT_PAD_TABLE_TYPE_COMMANDS)) {
        return FALSE;
    }
    if (keysym > 0) {
        return FALSE;
    }

    ibus_input_pad_engine_commit_str (IBUS_ENGINE (engine), str);
    return TRUE;
}

static void
on_window_destroy (gpointer window_data, gpointer data)
{
    IBusInputPadEngine *engine = (IBusInputPadEngine *) data;
    void *gtk_window = input_pad_window_get_window (input_pad_window);

    if (gtk_window != NULL) {
        g_signal_handlers_disconnect_by_func (G_OBJECT (gtk_window),
                                              G_CALLBACK (on_window_button_pressed),
                                              (gpointer) engine);
        g_signal_handlers_disconnect_by_func (G_OBJECT (gtk_window),
                                              G_CALLBACK (on_window_destroy),
                                              (gpointer) engine);
    }

    input_pad_window_activate (input_pad_window);
}

static void
on_window_activated (gpointer window_data, gpointer data)
{
    IBusInputPadEngine *engine = IBUS_INPUT_PAD_ENGINE (data);
    void *gtk_window = input_pad_window_get_window (input_pad_window);

    g_signal_connect (G_OBJECT (gtk_window),
                      "destroy",
                      G_CALLBACK (on_window_destroy),
                      (gpointer) engine);
    g_signal_connect (G_OBJECT (gtk_window),
                      "button-pressed",
                      G_CALLBACK (on_window_button_pressed),
                      (gpointer) engine);
}

static void
ibus_input_pad_engine_commit_str (IBusEngine *engine, const gchar *str)
{
    IBusText *text;

    text = ibus_text_new_from_string (str);
    ibus_engine_commit_text (engine, text);
}

static void
ibus_input_pad_engine_class_init (IBusInputPadEngineClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
    IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);

    parent_class = (IBusEngineClass *) g_type_class_peek_parent (klass);
    object_class->constructor = ibus_input_pad_engine_constructor;
    ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_input_pad_engine_destroy;
    engine_class->process_key_event = ibus_input_pad_engine_process_key_event;
    engine_class->enable = ibus_input_pad_engine_enable;
    engine_class->disable = ibus_input_pad_engine_disable;
    engine_class->focus_in = ibus_input_pad_engine_focus_in;
    engine_class->focus_out = ibus_input_pad_engine_focus_out;
    engine_class->property_activate = ibus_input_pad_engine_property_activate;
}

static void
ibus_input_pad_engine_init (IBusInputPadEngine *engine)
{
    IBusText *label;
    IBusText *tooltip;
    IBusProperty *input_pad_prop;
    IBusProperty *prop;
    IBusPropList *prop_list;
    void *gtk_window;

    engine->prop_list = ibus_prop_list_new ();
    g_object_ref_sink (engine->prop_list);

    label = ibus_text_new_from_string (_("Launch Input Pad"));
    tooltip = ibus_text_new_from_string (_("Launch Input Pad"));
    input_pad_prop = ibus_property_new ("ibus-shared-menu",
                                        PROP_TYPE_MENU,
                                        label,
                                        "ibus-setup",
                                        tooltip,
                                        TRUE, TRUE, PROP_STATE_UNCHECKED, NULL);
    ibus_prop_list_append (engine->prop_list, input_pad_prop);

    prop_list = ibus_prop_list_new ();

    label = ibus_text_new_from_string (_("Show Input Pad"));
    tooltip = ibus_text_new_from_string (_("Show Input Pad"));
    prop = ibus_property_new ("show-input-pad",
                              PROP_TYPE_NORMAL,
                              label,
                              DATAROOTDIR "/pixmaps/input-pad.png",
                              tooltip,
                              TRUE, TRUE, PROP_STATE_UNCHECKED, NULL);
    ibus_prop_list_append (prop_list, prop);

    label = ibus_text_new_from_string (_("Show Input Pad (Keyboard Only)"));
    tooltip = ibus_text_new_from_string (_("Show Input Pad (Keyboard Only)"));
    prop = ibus_property_new ("show-input-pad-layout-only",
                              PROP_TYPE_NORMAL,
                              label,
                              DATAROOTDIR "/pixmaps/input-pad.png",
                              tooltip,
                              TRUE, TRUE, PROP_STATE_UNCHECKED, NULL);
    ibus_prop_list_append (prop_list, prop);

    ibus_property_set_sub_props (input_pad_prop, prop_list);

    label = ibus_text_new_from_string (_("Setup Input Pad"));
    tooltip = ibus_text_new_from_string (_("Configure Input Pad"));
    prop = ibus_property_new ("setup-input-pad",
                              PROP_TYPE_NORMAL,
                              label,
                              "preferences-desktop",
                              tooltip,
                              TRUE, TRUE, PROP_STATE_UNCHECKED, NULL);
    ibus_prop_list_append (engine->prop_list, prop);

    gtk_window = input_pad_window_get_window (input_pad_window);

    if (gtk_window != NULL) {
        g_signal_connect (G_OBJECT (gtk_window),
                          "destroy",
                          G_CALLBACK (on_window_destroy),
                          (gpointer) engine);
        g_signal_connect (G_OBJECT (gtk_window),
                          "button-pressed",
                          G_CALLBACK (on_window_button_pressed),
                          (gpointer) engine);
    }

    g_signal_connect (G_OBJECT (input_pad_window),
                      "activated",
                      G_CALLBACK (on_window_activated),
                      (gpointer) engine);
}

static GObject*
ibus_input_pad_engine_constructor (GType                   type,
                                   guint                   n_construct_params,
                                   GObjectConstructParam  *construct_params)
{
    IBusInputPadEngine *engine;

    engine = (IBusInputPadEngine *) G_OBJECT_CLASS (parent_class)->constructor (type,
                                                    n_construct_params,
                                                    construct_params);

    return (GObject *) engine;
}

static void
free_str_list (GSList *str_list)
{
    GSList *head = str_list;
    gchar *str;

    g_return_if_fail (str_list != NULL);

    while (str_list) {
        str = (gchar *) str_list->data;
        g_free (str);
        str_list->data = NULL;
        str_list = str_list->next;
    }
    g_slist_free (head);
}

static void
ibus_input_pad_engine_destroy (IBusObject *object)
{
    IBusInputPadEngine *engine = (IBusInputPadEngine *) object;

    if (engine->prop_list) {
        g_object_unref (engine->prop_list);
        engine->prop_list = NULL;
    }
    if (engine->str_list) {
        free_str_list (engine->str_list);
        engine->str_list = NULL;
    }
    IBUS_OBJECT_CLASS (parent_class)->destroy (object);
}

static gboolean
ibus_input_pad_engine_process_key_event (IBusEngine    *engine,
                                         guint          keyval,
                                         guint          keycode,
                                         guint          modifiers)
{
    return parent_class->process_key_event (engine, keyval, keycode, modifiers);
}

static void
ibus_input_pad_engine_enable (IBusEngine *engine)
{
    parent_class->enable (engine);
    input_pad_window_set_char_button_sensitive (input_pad_window, TRUE);
}

static void
ibus_input_pad_engine_disable (IBusEngine *engine)
{
    parent_class->disable (engine);
    input_pad_window_set_char_button_sensitive (input_pad_window, FALSE);
}

static void
ibus_input_pad_engine_focus_in (IBusEngine *engine)
{
    IBusInputPadEngine *input_pad = (IBusInputPadEngine *) engine;

    ibus_engine_register_properties (engine, input_pad->prop_list);

    parent_class->focus_in (engine);

    input_pad_window_reorder_button_pressed (input_pad_window);
}

static void
ibus_input_pad_engine_focus_out (IBusEngine *engine)
{
    parent_class->focus_out (engine);
}

#if 0
static void
update_show_input_pad_label (IBusInputPadEngine *engine,
                             gboolean            is_shown)
{
    IBusProperty *input_pad_prop = ibus_prop_list_get (engine->prop_list, 0);
    IBusProperty *prop;
    IBusPropList *props;
    IBusText *label;

    g_return_if_fail (input_pad_prop != NULL);

    props = input_pad_prop->sub_props;
    g_return_if_fail (props != NULL);

    prop = ibus_prop_list_get (props, 0);
    g_return_if_fail (prop && prop->label && prop->label->text);
    if (is_shown) {
        label = ibus_text_new_from_string (_("Hide Input Pad"));
    } else {
        label = ibus_text_new_from_string (_("Show Input Pad"));
    }
    ibus_property_set_label (prop, label);
    ibus_property_set_tooltip (prop, label);
    ibus_engine_update_property ((IBusEngine*) engine, prop);
}
#endif

static void
set_keyboard_only_kbdui (void *window)
{
    const gchar *section = "engine/input-pad/keyboard_only_table";
    const gchar *name = NULL;
    gchar *str = NULL;

    name = "keyboard_theme";
    ibus_input_pad_config_get_str (config, section, name, &str);

    if (str && g_strcmp0 (str, "default") != 0) {
        input_pad_window_set_kbdui_name (window, str);
    } else {
        input_pad_window_set_kbdui_name (window, NULL);
    }
    g_free (str);
}

static void
set_default_kbdui (void *window)
{
    const gchar *section = "engine/input-pad/default_table";
    const gchar *name = NULL;
    gchar *str = NULL;

    name = "keyboard_theme";
    ibus_input_pad_config_get_str (config, section, name, &str);

    if (str && g_strcmp0 (str, "default") != 0) {
        input_pad_window_set_kbdui_name (window, str);
    } else {
        input_pad_window_set_kbdui_name (window, NULL);
    }
    g_free (str);
}

static void
show_keyboard_only_table (void *window)
{
    input_pad_window_set_show_table (window,
                                     INPUT_PAD_WINDOW_SHOW_TABLE_TYPE_NOTHING);
}

static void
show_default_table (void *window)
{
    const gchar *section = "engine/input-pad/default_table";
    const gchar *name = NULL;
    int n;

    name = "char_table_combo_box";
    ibus_input_pad_config_get_int (config, section, name, &n);
    input_pad_window_set_show_table (window, n);

    name = "layout_table_combo_box";
    ibus_input_pad_config_get_int (config, section, name, &n);
    input_pad_window_set_show_layout (window, n);
}

static void
ibus_input_pad_engine_property_activate (IBusEngine *engine,
                                         const char *prop_name,
                                         guint       prop_state)
{
    gboolean is_shown = FALSE;
    GError *error = NULL;
    gchar *argv[2] = { NULL, };
    gchar *path;
    const gchar *libexecdir;

    g_return_if_fail (prop_name != NULL);
    g_return_if_fail (engine != NULL);

    if (!g_strcmp0 (prop_name, "show-input-pad") ||
        !g_strcmp0 (prop_name, "show-input-pad-layout-only")) {
        void *window = input_pad_window;

        if (input_pad_window_get_window (window) == NULL)
            return;

        if (!g_strcmp0 (prop_name, "show-input-pad-layout-only")) {
            set_keyboard_only_kbdui (window);
        } else {
            set_default_kbdui (window);
        }

        ibus_input_pad_engine_focus_in (engine);

        // TODO: Update menu item label?
        /* is_shown = input_pad_window_get_visible (window); */
        if (is_shown) {
            input_pad_window_hide (window);
        } else {
            input_pad_window_show (window);
            if (!g_strcmp0 (prop_name, "show-input-pad-layout-only")) {
                show_keyboard_only_table (window);
            } else {
                show_default_table (window);
            }
        }
#if 0
        update_show_input_pad_label ((IBusInputPadEngine *) engine, is_shown);
#endif
    }
    if (!g_strcmp0 (prop_name, "setup-input-pad")) {
        libexecdir = g_getenv ("LIBEXECDIR");
        if (libexecdir == NULL) {
            libexecdir = LIBEXECDIR;
        }
        g_return_if_fail (libexecdir != NULL);
        path = g_build_filename (libexecdir, "ibus-setup-input-pad", NULL);
        argv[0] = path;
        argv[1] = NULL;
        g_spawn_async (NULL, argv, NULL, 0, NULL, NULL, NULL, &error);
        g_free (path);
    }
}

GType
ibus_input_pad_engine_get_type (void)
{
    static GType type = 0;

    static const GTypeInfo type_info = {
        sizeof (IBusInputPadEngineClass),
        (GBaseInitFunc)     NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc)    ibus_input_pad_engine_class_init,
        NULL,
        NULL,
        sizeof (IBusInputPadEngine),
        0,
        (GInstanceInitFunc) ibus_input_pad_engine_init,
    };

    if (type == 0) {
            type = g_type_register_static (IBUS_TYPE_ENGINE,
                                           "IBusInputPadEngine",
                                           &type_info,
                                           (GTypeFlags) 0);
    }

    return type;
}

void
ibus_input_pad_init (int *argc, char ***argv, IBusBus *bus)
{
    int retval;
    gboolean do_exit = FALSE;

    config = ibus_bus_get_input_pad_config (bus);
    if (config) {
        g_object_ref_sink (config);
    }
    retval = input_pad_window_init (argc, argv, 0, &do_exit);

    if (do_exit) {
        g_object_unref (config);
        config = NULL;
        exit (retval);
    }

    input_pad_window = input_pad_window_new ();
    input_pad_window_hide (input_pad_window);
}

int
ibus_input_pad_main (void)
{
    return input_pad_window_main (input_pad_window);
}

void
ibus_input_pad_finit (void)
{
    g_object_unref (config);
    config = NULL;
}
