/* vim:set et sts=4: */
/* ibus-input-pad - Input pad for IBus
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <input-pad.h>
#include <string.h>       /* strlen */

#include "i18n.h"
#include "iconfig-gtk2.h"
#include "setup-gtk2.h"

#define ENGINE_INPUT_PAD_SECTION "engine/input-pad"

typedef struct _ButtonClickData ButtonClickData;

enum {
    THEME_ID_COL = 0,
    THEME_NAME_COL,
    THEME_DESC_COL,
    THEME_VISIBLE_COL,
    THEME_N_COLS
};

struct _ButtonClickData {
    IBusInputPadConfig *config;
    GtkWidget          *msg_dialog;
};

static gboolean do_cancel_dialog (GtkWidget *dialog, ButtonClickData *button_data);
static void disable_apply_button (IBusInputPadConfig *config);
static void enable_apply_button (IBusInputPadConfig *config);
static const gchar * get_widget_name (GtkWidget *widget);
static gchar * get_combobox_appearance_name (GtkWidget *combobox);
static gchar * get_combobox_appearance_section (GtkWidget *combobox);

#if 0
static void
on_value_changed (IBusInputPadConfig *config,
                  const gchar *section,
                  const gchar *name,
                  GValue value,
                  gpointer data)
{
    g_return_if_fail (section != NULL);

    if (g_ascii_strncasecmp (section, ENGINE_INPUT_PAD_SECTION,
                             strlen (ENGINE_INPUT_PAD_SECTION)) != 0) {
        return;
    }
}
#endif

static void
on_combobox_appearance_changed (GtkComboBox *combobox, gpointer data)
{
    IBusInputPadConfig *config;
    gchar *section;
    gchar *name;
    gchar *kbdui_name = NULL;
    GtkTreeModel *model;
    GtkTreeIter iter;
    int id;
    gboolean res = FALSE;

    g_return_if_fail (GTK_IS_COMBO_BOX (combobox));
    g_return_if_fail (IBUS_IS_INPUT_PAD_CONFIG (data));

    if (!gtk_combo_box_get_active_iter (combobox, &iter)) {
        return;
    }

    config = IBUS_INPUT_PAD_CONFIG (data);
    section = get_combobox_appearance_section (GTK_WIDGET (combobox));
    name = get_combobox_appearance_name (GTK_WIDGET (combobox));
    g_return_if_fail (name != NULL && section != NULL);

    model = gtk_combo_box_get_model (combobox);
    gtk_tree_model_get (model, &iter,
                        THEME_ID_COL, &id,
                        THEME_NAME_COL, &kbdui_name, -1);
    if (!g_strcmp0 (name, "keyboard_theme")) {
        res = ibus_input_pad_config_set_str (config, section, name, kbdui_name);
    } else {
        res = ibus_input_pad_config_set_int (config, section, name, id);
    }
    g_free (kbdui_name);
    g_free (section);
    g_free (name);
    if (res) {
        enable_apply_button (config);
    }
}

static void
on_button_apply_clicked (GtkButton *button, gpointer data)
{
    IBusInputPadConfig *config;

    g_return_if_fail (GTK_IS_BUTTON (button));
    g_return_if_fail (IBUS_IS_INPUT_PAD_CONFIG (data));

    config = IBUS_INPUT_PAD_CONFIG (data);
    ibus_input_pad_config_commit_all (config);
    disable_apply_button (config);
}

static void
on_button_ok_clicked (GtkButton *button, gpointer data)
{
    IBusInputPadConfig *config;
    GtkWidget *dialog;
    GtkWidget *msg_dialog;
    ButtonClickData *button_data = (ButtonClickData *) data;

    g_return_if_fail (GTK_IS_BUTTON (button));
    g_return_if_fail (data != NULL);
    g_return_if_fail (IBUS_IS_INPUT_PAD_CONFIG (button_data->config));
    g_return_if_fail (GTK_IS_MESSAGE_DIALOG (button_data->msg_dialog));

    config = IBUS_INPUT_PAD_CONFIG (button_data->config);
    msg_dialog = GTK_WIDGET (button_data->msg_dialog);
    dialog = gtk_widget_get_toplevel (GTK_WIDGET (button));
    g_return_if_fail (GTK_IS_DIALOG (dialog));

    if (ibus_input_pad_config_get_changed_size (config) == 0) {
        gtk_dialog_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
    } else if (gtk_dialog_run (GTK_DIALOG (msg_dialog)) == GTK_RESPONSE_OK) {
        ibus_input_pad_config_commit_all (config);
        gtk_dialog_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
    } else {
        gtk_widget_hide (msg_dialog);
    }
}

static void
on_button_cancel_clicked (GtkButton *button, gpointer data)
{
    GtkWidget *dialog;
    ButtonClickData *button_data = (ButtonClickData *) data;

    g_return_if_fail (GTK_IS_BUTTON (button));

    dialog = gtk_widget_get_toplevel (GTK_WIDGET (button));

    if (do_cancel_dialog (dialog, button_data)) {
        gtk_dialog_response (GTK_DIALOG (dialog), GTK_RESPONSE_CANCEL);
    }
}

static gboolean
on_dialog_close (GtkWidget *dialog, GdkEventAny *event, gpointer data)
{
    ButtonClickData *button_data = (ButtonClickData *) data;
    return do_cancel_dialog (dialog, button_data);
}

static gboolean
do_cancel_dialog (GtkWidget *dialog, ButtonClickData *button_data)
{
    IBusInputPadConfig *config;
    GtkWidget *msg_dialog;

    g_return_val_if_fail (button_data != NULL, FALSE);
    g_return_val_if_fail (IBUS_IS_INPUT_PAD_CONFIG (button_data->config), FALSE);
    g_return_val_if_fail (GTK_IS_MESSAGE_DIALOG (button_data->msg_dialog), FALSE);
    g_return_val_if_fail (GTK_IS_DIALOG (dialog), FALSE);

    config = IBUS_INPUT_PAD_CONFIG (button_data->config);
    msg_dialog = GTK_WIDGET (button_data->msg_dialog);

    if (ibus_input_pad_config_get_changed_size (config) == 0) {
        return TRUE;
    } else if (gtk_dialog_run (GTK_DIALOG (msg_dialog)) == GTK_RESPONSE_OK) {
        return TRUE;
    }
    gtk_widget_hide (msg_dialog);
    return FALSE;
}

static void
disable_apply_button (IBusInputPadConfig *config)
{
    GObject *o;
    GtkWidget *apply_button;

    g_return_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config));

    o = IBUS_INPUT_PAD_CONFIG_GET_CLASS (config)->get_apply_button (config);
    g_return_if_fail (o != NULL && GTK_IS_WIDGET (o));

    apply_button = GTK_WIDGET (o);

    gtk_widget_set_sensitive (apply_button, FALSE);
}

static void
enable_apply_button (IBusInputPadConfig *config)
{
    GObject *o;
    GtkWidget *apply_button;

    g_return_if_fail (IBUS_IS_INPUT_PAD_CONFIG (config));

    o = IBUS_INPUT_PAD_CONFIG_GET_CLASS (config)->get_apply_button (config);
    g_return_if_fail (o != NULL && GTK_IS_WIDGET (o));

    apply_button = GTK_WIDGET (o);

    if (!gtk_widget_get_sensitive (apply_button)) {
        gtk_widget_set_sensitive (apply_button, TRUE);
    }
}

static const gchar *
get_widget_name (GtkWidget *widget)
{
    const gchar *widget_name = NULL;

    if (GTK_IS_BUILDABLE (widget)) {
        widget_name = gtk_buildable_get_name (GTK_BUILDABLE (widget));
    }
    if (widget_name == NULL) {
        widget_name = (const gchar *) g_object_get_data (G_OBJECT (widget),
                                                         "gtk-builder-name");
    }

    return widget_name;
}

static gchar *
get_combobox_appearance_name (GtkWidget *combobox)
{
    const gchar *widget_name;

    g_return_val_if_fail (combobox != NULL && GTK_IS_WIDGET (combobox), NULL);

    widget_name = get_widget_name (combobox);
    if (widget_name == NULL) {
        return NULL;
    }
    return g_strdup (widget_name);
}

static gchar *
get_combobox_appearance_section (GtkWidget *combobox)
{
    GtkWidget *parent;
    const gchar *parent_widget_name;
    gchar *section = NULL;

    g_return_val_if_fail (combobox != NULL && GTK_IS_WIDGET (combobox), NULL);

    parent = gtk_widget_get_parent (combobox);
    g_return_val_if_fail (parent != NULL, NULL);
    parent_widget_name = get_widget_name (parent);
    g_return_val_if_fail (parent_widget_name != NULL, NULL);

    section = g_build_path ("/", ENGINE_INPUT_PAD_SECTION,
                            parent_widget_name, NULL);

    return section;
}

static void
set_combobox_appearance_str (GtkWidget   *combobox,
                             const gchar *str)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    int id;
    gchar *kbdui_name;

    g_return_if_fail (GTK_IS_COMBO_BOX (combobox));
    g_return_if_fail (str != NULL);

    model = gtk_combo_box_get_model (GTK_COMBO_BOX (combobox));
    g_return_if_fail (model != NULL);
    gtk_tree_model_get_iter_first (model, &iter);
    do {
        gtk_tree_model_get (model, &iter,
                            THEME_ID_COL, &id,
                            THEME_NAME_COL, &kbdui_name, -1);
        if (!g_strcmp0 (str, kbdui_name)) {
            gtk_combo_box_set_active (GTK_COMBO_BOX (combobox), id);
            g_free (kbdui_name);
            break;
        }
        g_free (kbdui_name);
    } while (gtk_tree_model_iter_next (model, &iter));
}

static void
set_combobox_appearance_default_value (GtkWidget          *combobox,
                                       IBusInputPadConfig *config)
{
    gchar *section;
    gchar *name;
    gchar *value_str = NULL;
    int value_int = 0;

    g_return_if_fail (GTK_IS_COMBO_BOX (combobox));

    name = get_combobox_appearance_name (combobox);
    section = get_combobox_appearance_section (combobox);
    g_return_if_fail (name != NULL && section != NULL);

    if (!g_strcmp0 (name, "keyboard_theme")) {
        ibus_input_pad_config_get_str (config, section, name, &value_str);
        set_combobox_appearance_str (combobox,
                                     (const gchar *) value_str);
        g_free (value_str);
    } else {
        ibus_input_pad_config_get_int (config, section, name, &value_int);
        gtk_combo_box_set_active (GTK_COMBO_BOX (combobox),
                                  value_int);
    }
    g_free (section);
    g_free (name);
}

static GtkTreeModel *
keyboard_theme_new (InputPadWindowKbduiName *list)
{
    GtkTreeStore *store;
    GtkTreeIter iter;
    int i;

    g_return_val_if_fail (list != NULL, NULL);

    store = gtk_tree_store_new (THEME_N_COLS,
                                G_TYPE_INT,
                                G_TYPE_STRING, G_TYPE_STRING,
                                G_TYPE_BOOLEAN);

    gtk_tree_store_append (store, &iter, NULL);
    gtk_tree_store_set (store, &iter,
                        THEME_ID_COL, 0,
                        THEME_NAME_COL, "default",
                        THEME_DESC_COL, _("Default"),
                        THEME_VISIBLE_COL, TRUE, -1);

    for (i = 0; list[i].name; i++) {
        if (list[i].type != INPUT_PAD_WINDOW_TYPE_GTK) {
            continue;
        }
        gtk_tree_store_append (store, &iter, NULL);
        gtk_tree_store_set (store, &iter,
                            THEME_ID_COL, i + 1,
                            THEME_NAME_COL, list[i].name,
                            THEME_DESC_COL, list[i].description,
                            THEME_VISIBLE_COL, TRUE, -1);
    }
    return GTK_TREE_MODEL (store);
}

static void
table_add_kbdui_list (GtkWidget                *table,
                      InputPadWindowKbduiName  *list,
                      IBusInputPadConfig       *config)
{
    guint nrows, ncols;
    GtkTreeModel *model;
    GtkWidget *label;
    GtkWidget *combobox;
    GtkCellRenderer *renderer;

    g_return_if_fail (GTK_IS_TABLE (table));

    gtk_table_get_size (GTK_TABLE (table), &nrows, &ncols);
    g_return_if_fail (nrows > 0 && ncols >= 2);

    if ((model = keyboard_theme_new (list)) == NULL) {
        return;
    }

    label = gtk_label_new_with_mnemonic (_("Keyboard _Theme:"));
    gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
    gtk_widget_set_tooltip_text (label,
                                 _("Which theme you choose in kbdui modules"));
    gtk_table_attach_defaults (GTK_TABLE (table),
                               label,
                               0, 1,
                               nrows - 1, nrows);
    gtk_widget_show (label);

    combobox = gtk_combo_box_new ();
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), combobox);
    gtk_combo_box_set_model (GTK_COMBO_BOX (combobox), model);
    g_object_unref (G_OBJECT (model));
    if (GTK_IS_BUILDABLE (combobox)) {
        gtk_buildable_set_name (GTK_BUILDABLE (combobox), "keyboard_theme");
    }

    renderer = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox), renderer, FALSE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer,
                                    "text", THEME_DESC_COL,
                                    "visible", THEME_VISIBLE_COL,
                                    NULL);

    gtk_table_attach_defaults (GTK_TABLE (table),
                               combobox,
                               1, 2,
                               nrows - 1, nrows);
    set_combobox_appearance_default_value (combobox, config);

    gtk_widget_show (combobox);
    g_signal_connect (G_OBJECT (combobox), "changed",
                      G_CALLBACK (on_combobox_appearance_changed),
                      (gpointer) config);
}

static void
destroy_kbdui_list (InputPadWindowKbduiName *list)
{
    int i;
    for (i = 0; list[i].name; i++) {
        g_free (list[i].name);
        list[i].name = NULL;
        g_free (list[i].description);
        list[i].description= NULL;
    }
    g_free (list);
}

static void
create_about_vbox (GtkBuilder *builder)
{
    GtkWidget *about_dialog;
    GtkWidget *about_vbox;
    GtkWidget *content_area;
    GtkWidget *vbox;
    GtkWidget *widget;
    GtkWidget *old_parent;
    GList *list;

    about_dialog = GTK_WIDGET (gtk_builder_get_object (builder, "about_dialog"));
    about_vbox = GTK_WIDGET (gtk_builder_get_object (builder, "about_vbox"));

    g_return_if_fail (GTK_IS_DIALOG (about_dialog));
    g_return_if_fail (GTK_IS_VBOX (about_vbox));

    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (about_dialog), VERSION);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG (about_dialog));
    list = gtk_container_get_children (GTK_CONTAINER (content_area));
    g_return_if_fail (GTK_IS_BOX (list->data));
    vbox = GTK_WIDGET (list->data);
    list = gtk_container_get_children (GTK_CONTAINER (vbox));

    while (list) {
        /* Copied the implementation of gtk_widget_reparent() 
         * but gtk_box_pack_start() also includes set parent. */
        widget = GTK_WIDGET (list->data);
        old_parent = gtk_widget_get_parent (widget);
        g_object_ref (G_OBJECT (widget));
        gtk_container_remove (GTK_CONTAINER (old_parent), widget);
        gtk_box_pack_start (GTK_BOX (about_vbox), widget,
                            FALSE, FALSE, 0);
        list = list->next;
    }
}

GtkWidget *
ibus_input_pad_setup_gtk2_dialog_new (IBusInputPadConfig *config)
{
    const gchar *filename = IBUS_INPUT_PAD_SETUP_UI_FILE;
    GError *error = NULL;
    GtkBuilder *builder = gtk_builder_new ();
    GtkWidget *dialog = NULL;
    GtkWidget *char_table_combobox;
    GtkWidget *layout_table_combobox;
    GtkWidget *default_table;
    GtkWidget *keyboard_table;
    GtkWidget *keyboard_frame;
    GtkWidget *apply_button;
    GtkWidget *ok_button;
    GtkWidget *cancel_button;
    GtkWidget *ok_dialog;
    GtkWidget *cancel_dialog;
    InputPadWindowKbduiName *list;
    static ButtonClickData button_data_ok;
    static ButtonClickData button_data_cancel;

    if (!filename ||
        !g_file_test (filename, G_FILE_TEST_EXISTS)) {
        g_error ("File Not Found: %s\n", filename ? filename : "(null)");
        return NULL;
    }

    gtk_builder_set_translation_domain (builder, GETTEXT_PACKAGE);
    gtk_builder_add_from_file (builder, filename, &error);
    if (error) {
        g_error ("ERROR: %s\n",
                 error ? error->message ? error->message : "" : "");
        g_error_free (error);
        return NULL;
    }

    dialog = GTK_WIDGET (gtk_builder_get_object (builder, "setup_dialog"));
    gtk_window_set_icon_from_file (GTK_WINDOW (dialog),
                                   DATAROOTDIR "/pixmaps/input-pad.png",
                                   &error);
    error = NULL;
    gtk_window_set_default_icon_from_file (DATAROOTDIR "/pixmaps/input-pad.png",
                                           &error);

    create_about_vbox (builder);

    char_table_combobox = GTK_WIDGET (gtk_builder_get_object (builder, "char_table_combo_box"));
    layout_table_combobox = GTK_WIDGET (gtk_builder_get_object (builder, "layout_table_combo_box"));
    default_table = GTK_WIDGET (gtk_builder_get_object (builder, "default_table"));
    keyboard_table = GTK_WIDGET (gtk_builder_get_object (builder, "keyboard_only_table"));
    keyboard_frame = GTK_WIDGET (gtk_builder_get_object (builder, "keyboard_only_frame"));
    apply_button = GTK_WIDGET (gtk_builder_get_object (builder, "button_apply"));
    ok_button = GTK_WIDGET (gtk_builder_get_object (builder, "button_ok"));
    ok_dialog = GTK_WIDGET (gtk_builder_get_object (builder, "dialog_ok"));
    cancel_button = GTK_WIDGET (gtk_builder_get_object (builder, "button_cancel"));
    cancel_dialog = GTK_WIDGET (gtk_builder_get_object (builder, "dialog_cancel"));

#if 0
    if (config) {
        g_signal_connect (G_OBJECT (config), "value-changed",
                          G_CALLBACK (on_value_changed), NULL);
    }
#endif
    set_combobox_appearance_default_value (char_table_combobox, config);
    g_signal_connect (G_OBJECT (char_table_combobox), "changed",
                      G_CALLBACK (on_combobox_appearance_changed),
                      (gpointer) config);
    set_combobox_appearance_default_value (layout_table_combobox, config);
    g_signal_connect (G_OBJECT (layout_table_combobox), "changed",
                      G_CALLBACK (on_combobox_appearance_changed),
                      (gpointer) config);

    list = input_pad_window_get_kbdui_name_list ();
    if (list == NULL) {
        gtk_widget_hide (keyboard_frame);
    } else {
#ifdef IBUS_DEPRECATED_LANGUAGE_MENU_ITEM
        gtk_widget_hide (keyboard_frame);
        table_add_kbdui_list (default_table, list, config);
#else
        table_add_kbdui_list (default_table, list, config);
        table_add_kbdui_list (keyboard_table, list, config);
#endif
        destroy_kbdui_list (list);
    }

    gtk_widget_set_sensitive (apply_button, FALSE);
    IBUS_INPUT_PAD_CONFIG_GET_CLASS (config)->set_apply_button (config,
                                                                G_OBJECT (apply_button));
    g_signal_connect (G_OBJECT (apply_button), "clicked",
                      G_CALLBACK (on_button_apply_clicked),
                      (gpointer) config);
    button_data_ok.config = config;
    button_data_ok.msg_dialog = ok_dialog;
    g_signal_connect (G_OBJECT (ok_button), "clicked",
                      G_CALLBACK (on_button_ok_clicked),
                      (gpointer) &button_data_ok);
    button_data_cancel.config = config;
    button_data_cancel.msg_dialog = cancel_dialog;
    g_signal_connect (G_OBJECT (cancel_button), "clicked",
                      G_CALLBACK (on_button_cancel_clicked),
                      (gpointer) &button_data_cancel);

    g_signal_connect (G_OBJECT (dialog), "delete_event",
                      G_CALLBACK (on_dialog_close),
                      (gpointer) &button_data_cancel);

    gtk_builder_connect_signals (builder, NULL);
    g_object_unref (G_OBJECT (builder));

    return dialog;
}

void
ibus_input_pad_setup_gtk2_init (int *argc, char ***argv)
{
    gtk_init (argc, argv);
}

gboolean
ibus_input_pad_setup_gtk2_dialog_run (GtkWidget *dialog)
{
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
        return TRUE;
    }
    return FALSE;
}

void
ibus_input_pad_setup_gtk2_dialog_destroy (GtkWidget *dialog)
{
    gtk_widget_destroy (dialog);
}
