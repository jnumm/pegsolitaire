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

static GdkCursor *try_cursor_names(const char *names[]) {
    GdkDisplay *display = gtk_widget_get_display(pegSolitaireWindow);
    GdkCursor *cursor = NULL;
    for (int i = 0; names[i] && !cursor; i++)
        cursor = gdk_cursor_new_from_name(display, names[i]);
    if (!cursor)
        g_warning("The \"%s\" cursor is not available", names[0]);
    return cursor;
}

void init_cursors(void) {
    default_cursor = try_cursor_names((const char *[]){"default", NULL});
    hand_closed_cursor =
        try_cursor_names((const char *[]){"closedhand", "grabbing", NULL});
    hand_open_cursor =
        try_cursor_names((const char *[]){"openhand", "grab", NULL});
}

static void set_cursor(GdkCursor *cursor) {
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(boardDrawingArea)),
                          cursor);
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
    game_draw(cr, gtk_widget_get_allocated_width(widget),
              gtk_widget_get_allocated_height(widget));
    return FALSE;
}

gboolean drawarea_motion(GtkWidget *widget, GdkEventMotion *event,
                         gpointer user_data) {
    int tile_x = (event->x - offset_x) / tile_size;
    int tile_y = (event->y - offset_y) / tile_size;
    if (button_down) {
    } else if (game_is_peg_at(tile_x, tile_y)) {
        set_cursor(hand_open_cursor);
    } else {
        set_cursor(default_cursor);
    }
    return FALSE;
}

gboolean drawarea_button_press(GtkWidget *widget, GdkEventButton *event,
                               gpointer user_data) {
    if (event->button == 1 && !button_down /* && !is_game_end()*/) {
        int tile_x = (event->x - offset_x) / tile_size;
        int tile_y = (event->y - offset_y) / tile_size;

        if (!game_is_peg_at(tile_x, tile_y))
            return FALSE;

        set_cursor(hand_closed_cursor);
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
        int dest_x = (event->x - offset_x) / tile_size;
        int dest_y = (event->y - offset_y) / tile_size;

        // Either execute the move or put the peg back where we started.
        if (game_move(dragging_peg_x, dragging_peg_y, dest_x, dest_y)) {
            set_cursor(hand_open_cursor);
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
