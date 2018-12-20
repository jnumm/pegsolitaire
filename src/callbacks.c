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

void update_statusbar(int moves);

static bool button_down = false;
static int dragging_peg_x = 0, dragging_peg_y = 0;

static GdkCursor *hand_closed_cursor = NULL;
static GdkCursor *hand_open_cursor = NULL;
static GdkCursor *default_cursor = NULL;
enum { CURSOR_NONE = 0, CURSOR_OPEN, CURSOR_CLOSED };

void init_cursors(void) {
    GdkDisplay *display = gtk_widget_get_display(pegSolitaireWindow);

    if (!default_cursor)
        default_cursor = gdk_cursor_new_from_name(display, "default");
    if (!default_cursor)
        g_warning(_("The \"%s\" cursor is not available"), "default");

    if (!hand_closed_cursor)
        hand_closed_cursor = gdk_cursor_new_from_name(display, "grabbing");
    if (!hand_closed_cursor)
        hand_closed_cursor = gdk_cursor_new_from_name(display, "closedhand");
    if (!hand_closed_cursor)
        g_warning(_("The \"%s\" or \"%s\" cursor is not available"), "grabbing",
                  "closedhand");

    if (!hand_open_cursor)
        hand_open_cursor = gdk_cursor_new_from_name(display, "grab");
    if (!hand_open_cursor)
        hand_open_cursor = gdk_cursor_new_from_name(display, "openhand");
    if (!hand_open_cursor)
        g_warning(_("The \"%s\" or \"%s\" cursor is not available"), "grab",
                  "openhand");
}

static void set_cursor(int cursor) {
    static int prev_cursor = -1;
    if ((cursor == CURSOR_NONE) && (prev_cursor != CURSOR_NONE))
        gdk_window_set_cursor(gtk_widget_get_window(pegSolitaireWindow),
                              default_cursor);
    if ((cursor == CURSOR_OPEN) && (prev_cursor != CURSOR_OPEN))
        gdk_window_set_cursor(gtk_widget_get_window(pegSolitaireWindow),
                              hand_open_cursor);
    if ((cursor == CURSOR_CLOSED) && (prev_cursor != CURSOR_CLOSED))
        gdk_window_set_cursor(gtk_widget_get_window(pegSolitaireWindow),
                              hand_closed_cursor);
    prev_cursor = cursor;
}

static void initiate_new_game(int board_type, int board_size) {
    game_board_type = board_type;
    game_board_size = board_size;
    game_new();

    gtk_label_set_text(statusMessageLabel, "");
    update_statusbar(game_moves);
    gtk_widget_queue_draw(boardDrawingArea);
}

// Following functions are gtk callbacks and all their parameters are required.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

gboolean drawarea_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    game_draw(cr);
    return FALSE;
}

gboolean drawarea_motion(GtkWidget *widget, GdkEventMotion *event,
                         gpointer user_data) {
    int tile_x = event->x / tile_size;
    int tile_y = event->y / tile_size;
    if (button_down) {

    } else if (game_is_peg_at(tile_x, tile_y)) {
        set_cursor(CURSOR_OPEN);
    } else {
        set_cursor(CURSOR_NONE);
    }
    return FALSE;
}

gboolean drawarea_button_press(GtkWidget *widget, GdkEventButton *event,
                               gpointer user_data) {
    if (event->button == 1 && !button_down /* && !is_game_end()*/) {
        int tile_x = event->x / tile_size;
        int tile_y = event->y / tile_size;

        if (!game_is_peg_at(tile_x, tile_y))
            return FALSE;

        set_cursor(CURSOR_CLOSED);
        dragging_peg_x = tile_x;
        dragging_peg_y = tile_y;

        button_down = true;
        game_toggle_cell(tile_x, tile_y);
        gtk_widget_queue_draw(widget);
    }
    return FALSE;
}

gboolean drawarea_button_release(GtkWidget *widget, GdkEventButton *event,
                                 gpointer user_data) {
    if (event->button == 1 && button_down) {
        button_down = false;
        set_cursor(CURSOR_NONE);
        int dest_x = event->x / tile_size;
        int dest_y = event->y / tile_size;

        // Either execute the move or put the peg back where we started.
        if (game_move(dragging_peg_x, dragging_peg_y, dest_x, dest_y)) {
            update_statusbar(game_moves);
            if (is_game_end()) {
                gtk_label_set_text(statusMessageLabel, game_cheese());
            }
        } else {
            game_toggle_cell(dragging_peg_x, dragging_peg_y);
        }

        gtk_widget_queue_draw(widget);
    }
    return FALSE;
}

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
