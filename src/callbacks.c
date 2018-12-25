/*
 *  Copyright (C) 2007-2008 Ben Asselstine
 *  Copyright (C) 2017-2018 Juhani Numminen
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "callbacks.h"

#include <gtk/gtk.h>
#include <stdbool.h>

#include "config.h"
#include "game.h"
#include "i18n.h"
#include "share.h"

void update_statusbar(void) {
    // TRANSLATORS: This is the number of moves the player has made.
    gchar *str = g_strdup_printf(_("Moves: %d"), game_moves);
    gtk_label_set_text(statusMovesLabel, str);
    g_free(str);
}

static void initiate_new_game(int board_type, int board_size) {
    game_board_type = board_type;
    game_board_size = board_size;
    game_new();

    gtk_label_set_text(statusMessageLabel, "");
    update_statusbar();
    gtk_widget_queue_draw(boardDrawingArea);
}

// Following functions are gtk callbacks and all their parameters are required.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void menu_restart(GtkMenuItem *menuitem, gpointer user_data) {
    initiate_new_game(game_board_type, game_board_size);
}

void menu_eng_beginner(GtkMenuItem *menuitem, gpointer user_data) {
    initiate_new_game(BOARD_ENGLISH, BOARD_SIZE_BEGINNER);
}

void menu_eng_intermediate(GtkMenuItem *menuitem, gpointer user_data) {
    initiate_new_game(BOARD_ENGLISH, BOARD_SIZE_INTERMEDIATE);
}

void menu_eng_advanced(GtkMenuItem *menuitem, gpointer user_data) {
    initiate_new_game(BOARD_ENGLISH, BOARD_SIZE_ADVANCED);
}

void menu_eur_beginner(GtkMenuItem *menuitem, gpointer user_data) {
    initiate_new_game(BOARD_EUROPEAN, BOARD_SIZE_BEGINNER);
}

void menu_eur_intermediate(GtkMenuItem *menuitem, gpointer user_data) {
    initiate_new_game(BOARD_EUROPEAN, BOARD_SIZE_INTERMEDIATE);
}

void menu_eur_advanced(GtkMenuItem *menuitem, gpointer user_data) {
    initiate_new_game(BOARD_EUROPEAN, BOARD_SIZE_ADVANCED);
}

void menu_help(GtkMenuItem *menuitem, gpointer user_data) {
    GError *err = NULL;
    gtk_show_uri_on_window(GTK_WINDOW(pegSolitaireWindow), "ghelp:pegsolitaire",
                           gtk_get_current_event_time(), &err);
    if (err) {
        g_warning(_("Cannot show help: %s"), err->message);
        g_error_free(err);
    }
}

void menu_about(GtkMenuItem *menuitem, gpointer user_data) {
    gtk_dialog_run(GTK_DIALOG(pegSolitaireAboutDialog));
    gtk_widget_hide(GTK_WIDGET(pegSolitaireAboutDialog));
}

void window_destroy(GObject *object, gpointer user_data) { gtk_main_quit(); }

void menu_quit(GtkMenuItem *menuitem, gpointer user_data) { gtk_main_quit(); }

#pragma GCC diagnostic pop
