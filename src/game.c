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

#include "game.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <librsvg/rsvg.h>

#include "i18n.h"
#include "share.h"

#define DEFAULT_GAME_BOARD_SIZE BOARD_SIZE_BEGINNER
#define DEFAULT_GAME_BOARD_TYPE BOARD_ENGLISH

static bool game_board[BOARD_SIZE_ADVANCED][BOARD_SIZE_ADVANCED] = {false};
// true means there's a peg, false means no peg.

static bool game_board_mask[BOARD_SIZE_ADVANCED][BOARD_SIZE_ADVANCED] = {false};
// true means it's part of the cross, false means not.

static cairo_pattern_t *peg_pattern = NULL;
static cairo_pattern_t *hole_pattern = NULL;

// Globals that are exposed through game.h
int game_moves = 0;
int game_board_size = DEFAULT_GAME_BOARD_SIZE;
game_board_enum game_board_type = DEFAULT_GAME_BOARD_TYPE;
double offset_x = 0, offset_y = 0;
double tile_size = 0;
// End of globals that are exposed through game.h

static bool valid_index(int i) { return i >= 0 && i < game_board_size; }

// The geometric center between two points.
static int middle(int src, int dst) { return src + ((dst - src) / 2); }

static void create_game_board_mask(void) {
    int n = game_board_size;
    int m = n / 2; // length of one side of the cross
    assert(n % 2 == 1);
    assert(m % 2 == 1);
    // for those keeping score, this means n=3,7,11,15,19... is valid.

    // initialize game_board_mask
    memset(game_board_mask, 0, sizeof game_board_mask);

    // fill the centre row of the cross
    for (int i = n / 2 - m / 2; i < n - (n / 2 - m / 2); i++) {
        for (int j = 0; j < game_board_size; j++) {
            game_board_mask[i][j] = 1;
        }
    }

    // fill the centre column of the cross
    for (int i = 0; i < game_board_size; i++) {
        for (int j = n / 2 - m / 2; j < n - (n / 2 - m / 2); j++) {
            game_board_mask[i][j] = 1;
        }
    }

    if (game_board_type == BOARD_EUROPEAN) {
        for (int i = n / 2 - m / 1.666; i < n - (n / 2 - m / 1.666); i++) {
            for (int j = n / 2 - m / 1.666; j < n - (n / 2 - m / 1.666); j++) {
                game_board_mask[i][j] = 1;
            }
        }
    }
}

static void game_init(void) {
    // setup the shape of the game board
    create_game_board_mask();
    // clear the board of all pegs
    memset(game_board, 0, sizeof game_board);
}

/* Clears the number of moves, and places a peg in all holes of the board
 * except for the middle one. */
static void game_clear(void) {
    memcpy(game_board, game_board_mask, sizeof game_board);
    game_board[game_board_size / 2][game_board_size / 2] = false;
    game_moves = 0;
}

static int game_count_pegs_on_board(void) {
    int pegs_left = 0;
    // find the number of pegs on the board
    for (int i = 0; i < game_board_size; i++) {
        for (int j = 0; j < game_board_size; j++) {
            pegs_left += game_board[i][j];
        }
    }
    return pegs_left;
}

void game_new(void) {
    game_init();
    game_clear();
}

bool is_game_end(void) {
    // are there any two pegs adjacent in a row?
    for (int i = 0; i < game_board_size; i++) {
        for (int j = 0; j < game_board_size - 1; j++) {
            if (game_board[i][j] && game_board[i][j + 1]) {
                // great, but is the entire row filled with pegs?
                for (int k = 0; k < game_board_size; k++) {
                    if (game_board_mask[i][k] && !game_board[i][k]) {
                        return false; // nope, the game is still on.
                    }
                }
            }
        }
    }

    // are there any two pegs adjacent in a column?
    for (int i = 0; i < game_board_size - 1; i++) {
        for (int j = 0; j < game_board_size; j++) {
            if (game_board[i][j] && game_board[i + 1][j]) {
                // great, but is the entire row filled with pegs?
                for (int k = 0; k < game_board_size; k++) {
                    if (game_board_mask[k][j] && !game_board[k][j]) {
                        return false; // nope, the game is still on.
                    }
                }
            }
        }
    }

    // no pegs adjacent.  no more moves means it's game over.
    return true;
}

void game_toggle_cell(int x, int y) {
    if (!valid_index(x) || !valid_index(y))
        return;

    if (game_board_mask[y][x]) {
        game_board[y][x] = !game_board[y][x];
    }
}

bool game_is_peg_at(int x, int y) {
    return valid_index(x) && valid_index(y) && game_board_mask[y][x] &&
           game_board[y][x];
}

static bool game_is_valid_move(int src_x, int src_y, int dst_x, int dst_y) {
    // is it 2 away with a peg in the middle with a peg in it?
    bool correct_distance = (dst_x == src_x && abs(dst_y - src_y) == 2) ||
                            (dst_y == src_y && abs(dst_x - src_x) == 2);

    return !game_is_peg_at(src_x, src_y) && !game_is_peg_at(dst_x, dst_y) &&
           game_is_peg_at(middle(src_x, dst_x), middle(src_y, dst_y)) &&
           correct_distance;
}

// move peg from src to dst, taking intermediate peg out.
// presumes that peg from src is already removed
bool game_move(int src_x, int src_y, int dst_x, int dst_y) {
    if (game_is_valid_move(src_x, src_y, dst_x, dst_y)) {
        // okay, the peg has been taken out of src_x,src_y and is in the air.
        // it is placed on dst_x,dst_y, and it's a valid move.
        // this means that the dst_x,dst_y doesn't have a peg in it.
        // it also means the one tile we jumped over does have a peg in it.

        // take the jumped peg out.
        game_toggle_cell(middle(src_x, dst_x), middle(src_y, dst_y));

        // put the source peg into the destination spot.
        game_toggle_cell(dst_x, dst_y);
        game_moves++;
        return true;
    }
    return false;
}

static RsvgHandle *load_svg(char *filename) {
    char *full_filename = g_build_filename(PKGDATADIR, filename, NULL);

    GError *err = NULL;
    RsvgHandle *svg = rsvg_handle_new_from_file(full_filename, &err);

    if (err) {
        GtkDialog *dialog = GTK_DIALOG(gtk_message_dialog_new(
            GTK_WINDOW(pegSolitaireWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            _("Could not load image file %s:\n"
              "%s\n\n"
              "Please check that Peg Solitaire is installed correctly."),
            full_filename, err->message));
        gtk_dialog_run(dialog);
        g_error_free(err);
        exit(1);
    }

    g_free(full_filename);
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
    peg_pattern = rsvg_to_pattern(load_svg("peg.svg"));
    hole_pattern = rsvg_to_pattern(load_svg("hole.svg"));
}

void game_unload_resources(void) {
    cairo_pattern_destroy(peg_pattern);
    cairo_pattern_destroy(hole_pattern);
}

void game_draw(cairo_t *cr, int width, int height) {
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
}

const char *game_cheese(void) {
    static const char *cheese[] = {
        N_("GENIUS!"),
        N_("OUTSTANDING!"),
        N_("Sensational!"),
        N_("Excellent!"),
        N_("Very Good!"),
        N_("Not Bad!"),
        N_("Better Luck Next Time!"),
    };

    int pegs_left = game_count_pegs_on_board();
    int cheese_index = MIN(pegs_left, 6);

    if (pegs_left == 1 && game_board[game_board_size / 2][game_board_size / 2])
        cheese_index = 0;

    return gettext(cheese[cheese_index]);
}
