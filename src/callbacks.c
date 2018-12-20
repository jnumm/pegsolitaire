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

static gint tile_size = 0, prior_tile_size = 0;
static gint width = 0, height = 0;
static gboolean clear_game = 1;
static gboolean clear_buffer = 1;
static gint piece_x = 0;
static gint piece_y = 0;
static bool button_down = false;

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
    clear_game = 1;
    game_new();

    gtk_label_set_text(statusMessageLabel, "");
    update_statusbar(game_moves);
}

// Following functions are gtk callbacks and all their parameters are required.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void on_helpAboutMenuItem_activate(GtkMenuItem *menuitem, gpointer user_data) {
    gtk_dialog_run(GTK_DIALOG(pegSolitaireAboutDialog));
    gtk_widget_hide(GTK_WIDGET(pegSolitaireAboutDialog));
}

void on_helpContentsMenuItem_activate(GtkMenuItem *menuitem,
                                      gpointer user_data) {
    GError *err = NULL;
    gtk_show_uri_on_window(GTK_WINDOW(pegSolitaireWindow), "ghelp:pegsolitaire",
                           gtk_get_current_event_time(), &err);
    if (err) {
        g_warning(_("Cannot show help: %s"), err->message);
        g_error_free(err);
    }
}

void on_gameRestartMenuItem_activate(GtkMenuItem *menuitem,
                                     gpointer user_data) {
    initiate_new_game(game_board_type, game_board_size);
}

void on_pegSolitaireWindow_destroy(GObject *object, gpointer user_data) {
    gtk_main_quit();
}

void on_gameQuitMenuItem_activate(GtkMenuItem *menuitem, gpointer user_data) {
    gtk_main_quit();
}

gboolean on_boardDrawingArea_motion_notify_event(GtkWidget *widget,
                                                 GdkEventMotion *event,
                                                 gpointer user_data) {
    int i = event->x / tile_size;
    int j = event->y / tile_size;
    int icon_size = tile_size / 1.666;
    if (button_down) {
        // before we draw the peg, let's expose the board again.
        /*gdk_draw_pixmap (gtk_widget_get_window (widget),
                         gtk_widget_get_style
           (widget)->fg_gc[gtk_widget_get_state (widget)], board_pixmap, 0, 0,
           0, 0, tile_size * game_board_size, tile_size * game_board_size);*/

        /*gdk_draw_pixbuf (gtk_widget_get_window (widget),
                         gtk_widget_get_style
           (widget)->fg_gc[gtk_widget_get_state (widget)], peg_pixbuf, 0, 0,
           event->x - (icon_size / 2), event->y - (icon_size / 2), icon_size,
           icon_size, GDK_RGB_DITHER_NORMAL, 0, 0);*/
    } else {
        // switch up the pointer when we're over a peg and what-not.
        if (game_is_peg_at(i, j)) {
            if ((int)event->x % tile_size < (tile_size / 2) + (icon_size / 2) &&
                (int)event->x % tile_size > (tile_size / 2) - (icon_size / 2) &&
                (int)event->y % tile_size < (tile_size / 2) + (icon_size / 2) &&
                (int)event->y % tile_size > (tile_size / 2) - (icon_size / 2))
                set_cursor(CURSOR_OPEN);
            else
                set_cursor(CURSOR_NONE);
        } else
            set_cursor(CURSOR_NONE);
    }
    return FALSE;
}

gboolean on_boardDrawingArea_button_press_event(GtkWidget *widget,
                                                GdkEventButton *event,
                                                gpointer user_data) {
    if ((event->button == 1) && !button_down) {
        int i, j;
        if (is_game_end())
            return FALSE;

        i = event->x / tile_size;
        j = event->y / tile_size;

        if (!game_is_peg_at(i, j))
            return FALSE;
        set_cursor(CURSOR_CLOSED);

        button_down = true;
        game_toggle_cell(i, j);
        // game_draw(pegSolitaireWindow, /*board_pixmap,*/ tile_size, 0);
        piece_x = i;
        piece_y = j;
        GdkRectangle update;
        update.x = i * tile_size;
        update.y = j * tile_size;
        update.width = tile_size;
        update.height = tile_size;
        // gtk_widget_draw(widget, &update);
    }
    return FALSE;
}

gboolean on_boardDrawingArea_button_release_event(GtkWidget *widget,
                                                  GdkEventButton *event,
                                                  gpointer user_data) {
    if (event->button == 1) {
        if (button_down) {
            int i, j;
            button_down = false;
            set_cursor(CURSOR_NONE);
            i = event->x / tile_size;
            j = event->y / tile_size;
            if (!game_move(piece_x, piece_y, i, j)) {
                // put the peg back where we started.
                game_toggle_cell(piece_x, piece_y);
                // game_draw(pegSolitaireWindow, board_pixmap, tile_size, 0);
                return FALSE;
            }
            // game_draw(pegSolitaireWindow, board_pixmap, tile_size, 0);
            update_statusbar(game_moves);
            if (is_game_end()) {
                gtk_label_set_text(statusMessageLabel, game_cheese());
            } else
                set_cursor(CURSOR_OPEN);
        }
    }
    return FALSE;
}

gboolean on_boardDrawingArea_draw(GtkWidget *widget, cairo_t *cr,
                                  gpointer data) {
    game_draw(cr);
    return FALSE;
}

void on_gameEnglishBeginnerMenuItem_activate(GtkMenuItem *menuitem,
                                             gpointer user_data) {
    initiate_new_game(BOARD_ENGLISH, BOARD_SIZE_BEGINNER);
}

void on_gameEnglishIntermediateMenuItem_activate(GtkMenuItem *menuitem,
                                                 gpointer user_data) {
    initiate_new_game(BOARD_ENGLISH, BOARD_SIZE_INTERMEDIATE);
}

void on_gameEnglishAdvancedMenuItem_activate(GtkMenuItem *menuitem,
                                             gpointer user_data) {
    initiate_new_game(BOARD_ENGLISH, BOARD_SIZE_ADVANCED);
}

void on_gameEuropeanBeginnerMenuItem_activate(GtkMenuItem *menuitem,
                                              gpointer user_data) {
    initiate_new_game(BOARD_EUROPEAN, BOARD_SIZE_BEGINNER);
}

void on_gameEuropeanIntermediateMenuItem_activate(GtkMenuItem *menuitem,
                                                  gpointer user_data) {
    initiate_new_game(BOARD_EUROPEAN, BOARD_SIZE_INTERMEDIATE);
}

void on_gameEuropeanAdvancedMenuItem_activate(GtkMenuItem *menuitem,
                                              gpointer user_data) {
    initiate_new_game(BOARD_EUROPEAN, BOARD_SIZE_ADVANCED);
}

#pragma GCC diagnostic pop
