/*  render.c: functions for rendering a peg solitaire game
 *  Copyright (C) 2018 Juhani Numminen
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

#include "render.h"

#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "callbacks.h"
#include "data.h"
#include "game.h"
#include "i18n.h"
#include "share.h"

#define GAME_NOT_DRAGGING -1
static int game_dragging_at_x = GAME_NOT_DRAGGING;
static int game_dragging_at_y = GAME_NOT_DRAGGING;

static bool button_down = false;
static GdkPoint dragging_peg = {0, 0};

static GdkCursor *hand_closed_cursor = NULL;
static GdkCursor *hand_open_cursor = NULL;
static GdkCursor *default_cursor = NULL;

static cairo_pattern_t *hole_pattern = NULL;
static cairo_pattern_t *peg_pattern = NULL;

static double offset_x = 0, offset_y = 0;
static double tile_size = 0;

static GdkCursor *try_cursor_names(const char *names[]) {
    GdkDisplay *display = gtk_widget_get_display(pegSolitaireWindow);
    GdkCursor *cursor = NULL;
    for (int i = 0; names[i] && !cursor; i++)
        cursor = gdk_cursor_new_from_name(display, names[i]);
    if (!cursor)
        g_warning(_("The \"%s\" cursor is not available"), names[0]);
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

static GdkPoint widget_coords_to_cell(int x, int y) {
    return (GdkPoint){(x - offset_x) / tile_size, (y - offset_y) / tile_size};
}

static RsvgHandle *load_svg(const char *str) {
    GError *err = NULL;
    RsvgHandle *svg =
        rsvg_handle_new_from_data((const guint8 *)str, strlen(str), &err);
    if (err) {
        GtkDialog *dialog = GTK_DIALOG(gtk_message_dialog_new(
            NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            _("Something went wrong.\n"
              "Could not load internal image data:\n"
              "%s"),
            err->message));
        gtk_window_set_title(GTK_WINDOW(dialog), _("Peg Solitaire"));
        gtk_dialog_run(dialog);
        exit(1);
    }
    return svg;
}

static cairo_pattern_t *rsvg_to_pattern(RsvgHandle *svg) {
    RsvgDimensionData svg_dimensions;
    rsvg_handle_get_dimensions(svg, &svg_dimensions);

    cairo_surface_t *surface = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32, svg_dimensions.width, svg_dimensions.height);
    cairo_t *cr = cairo_create(surface);
    rsvg_handle_render_cairo(svg, cr);
    cairo_pattern_t *pattern = cairo_pattern_create_for_surface(surface);
    cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);

    cairo_matrix_t scale;
    cairo_matrix_init_scale(&scale, svg_dimensions.width,
                            svg_dimensions.height);
    cairo_pattern_set_matrix(pattern, &scale);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    g_object_unref(svg);

    return pattern;
}

void game_load_resources(void) {
    hole_pattern = rsvg_to_pattern(load_svg(hole_svg));
    peg_pattern = rsvg_to_pattern(load_svg(peg_svg));
}

void game_unload_resources(void) {
    cairo_pattern_destroy(peg_pattern);
    cairo_pattern_destroy(hole_pattern);
}

// Following functions are gtk callbacks and all their parameters are required.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

gboolean drawarea_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    double shorter_side = fmin(width, height);
    offset_x = 0.5 * (width - shorter_side);
    offset_y = 0.5 * (height - shorter_side);
    tile_size = shorter_side / game_board_size;
    cairo_translate(cr, offset_x, offset_y);
    cairo_scale(cr, tile_size, tile_size);

    cairo_set_source(cr, hole_pattern);
    for (int y = 0; y < game_board_size; y++)
        for (int x = 0; x < game_board_size; x++)
            if (game_board_mask[y][x])
                cairo_rectangle(cr, x, y, 1, 1);
    cairo_fill(cr);

    cairo_set_source(cr, peg_pattern);
    for (int y = 0; y < game_board_size; y++)
        for (int x = 0; x < game_board_size; x++)
            if (game_board[y][x])
                cairo_rectangle(cr, x, y, 1, 1);
    cairo_fill(cr);

    if (game_dragging_at_x != GAME_NOT_DRAGGING) {
        cairo_translate(cr, (game_dragging_at_x - offset_x) / tile_size - 0.5,
                        (game_dragging_at_y - offset_y) / tile_size - 0.5);
        cairo_set_source(cr, peg_pattern);
        cairo_rectangle(cr, 0, 0, 1, 1);
        cairo_fill(cr);
    }

    return FALSE;
}

gboolean drawarea_motion(GtkWidget *widget, GdkEventMotion *event,
                         gpointer user_data) {
    GdkPoint cell = widget_coords_to_cell(event->x, event->y);
    if (button_down) {
        game_dragging_at_x = event->x;
        game_dragging_at_y = event->y;
        gtk_widget_queue_draw(widget);
    } else if (game_is_peg_at(cell)) {
        set_cursor(hand_open_cursor);
    } else {
        set_cursor(default_cursor);
    }
    return FALSE;
}

gboolean drawarea_button_press(GtkWidget *widget, GdkEventButton *event,
                               gpointer user_data) {
    if (event->button == 1 && !button_down /* && !is_game_end()*/) {
        GdkPoint cell = widget_coords_to_cell(event->x, event->y);

        if (!game_is_peg_at(cell))
            return FALSE;

        game_dragging_at_x = event->x;
        game_dragging_at_y = event->y;

        set_cursor(hand_closed_cursor);
        dragging_peg = cell;

        button_down = true;
        game_toggle_cell(cell);
        gtk_widget_queue_draw(widget);
    }
    return FALSE;
}

gboolean drawarea_button_release(GtkWidget *widget, GdkEventButton *event,
                                 gpointer user_data) {
    if (event->button == 1 && button_down) {
        button_down = false;
        game_dragging_at_x = GAME_NOT_DRAGGING;
        GdkPoint dest = widget_coords_to_cell(event->x, event->y);

        // Either execute the move or put the peg back where we started.
        if (game_move(dragging_peg, dest)) {
            set_cursor(hand_open_cursor);
            update_statusbar();
            if (is_game_end()) {
                gtk_label_set_text(statusMessageLabel, game_cheese());
            }
        } else {
            game_toggle_cell(dragging_peg);
        }

        gtk_widget_queue_draw(widget);
    }
    return FALSE;
}

#pragma GCC diagnostic pop
