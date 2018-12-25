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

#include "i18n.h"
#include "share.h"

#define DEFAULT_GAME_BOARD_SIZE BOARD_SIZE_BEGINNER
#define DEFAULT_GAME_BOARD_TYPE BOARD_ENGLISH

bool game_board[BOARD_SIZE_ADVANCED][BOARD_SIZE_ADVANCED] = {false};
// true means there's a peg, false means no peg.

bool game_board_mask[BOARD_SIZE_ADVANCED][BOARD_SIZE_ADVANCED] = {false};
// true means it's part of the cross, false means not.

// Globals that are exposed through game.h
int game_moves = 0;
int game_board_size = DEFAULT_GAME_BOARD_SIZE;
game_board_enum game_board_type = DEFAULT_GAME_BOARD_TYPE;
// End of globals that are exposed through game.h

static bool valid_index(int i) { return i >= 0 && i < game_board_size; }

static bool valid_cell(GdkPoint cell) {
    return valid_index(cell.x) && valid_index(cell.y);
}

// The geometric center between two points.
static int halfway(int src, int dst) { return src + ((dst - src) / 2); }

static GdkPoint middle(GdkPoint src, GdkPoint dst) {
    return (GdkPoint){halfway(src.x, dst.x), halfway(src.y, dst.y)};
}

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

void game_toggle_cell(GdkPoint cell) {
    if (!valid_cell(cell))
        return;

    if (game_board_mask[cell.y][cell.x]) {
        game_board[cell.y][cell.x] = !game_board[cell.y][cell.x];
    }
}

bool game_is_peg_at(GdkPoint cell) {
    return valid_cell(cell) && game_board_mask[cell.y][cell.x] &&
           game_board[cell.y][cell.x];
}

static bool is_in_mask(GdkPoint cell) {
    return valid_cell(cell) && game_board_mask[cell.y][cell.x];
}

static bool game_is_valid_move(GdkPoint src, GdkPoint dst) {
    // is it 2 away with a peg in the middle with a peg in it?
    bool correct_distance = (dst.x == src.x && abs(dst.y - src.y) == 2) ||
                            (dst.y == src.y && abs(dst.x - src.x) == 2);

    return is_in_mask(src) && is_in_mask(dst) && !game_is_peg_at(src) &&
           !game_is_peg_at(dst) && game_is_peg_at(middle(src, dst)) &&
           correct_distance;
}

// move peg from src to dst, taking intermediate peg out.
// presumes that peg from src is already removed
bool game_move(GdkPoint src, GdkPoint dst) {
    if (game_is_valid_move(src, dst)) {
        // okay, the peg has been taken out of src_x,src_y and is in the air.
        // it is placed on dst_x,dst_y, and it's a valid move.
        // this means that the dst_x,dst_y doesn't have a peg in it.
        // it also means the one tile we jumped over does have a peg in it.

        // take the jumped peg out.
        game_toggle_cell(middle(src, dst));

        // put the source peg into the destination spot.
        game_toggle_cell(dst);
        game_moves++;
        return true;
    }
    return false;
}

const char *game_cheese(void) {
    bool center_peg = game_board[game_board_size / 2][game_board_size / 2];
    switch (game_count_pegs_on_board()) {
    case 1:
        return center_peg ? _("GENIUS!") : _("OUTSTANDING!");
    case 2:
        return _("Sensational!");
    case 3:
        return _("Excellent!");
    case 4:
        return _("Very Good!");
    case 5:
        return _("Not Bad!");
    default:
        return _("Better Luck Next Time!");
    }
}
