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

#ifndef PEGSOLITAIRE_GAME_H
#define PEGSOLITAIRE_GAME_H

#include <cairo.h>
#include <stdbool.h>

typedef enum { BOARD_ENGLISH, BOARD_EUROPEAN } game_board_enum;

#define BOARD_SIZE_BEGINNER 7
#define BOARD_SIZE_INTERMEDIATE 11
#define BOARD_SIZE_ADVANCED 15

extern int game_moves;
extern game_board_enum game_board_type;
extern int game_board_size;
extern double offset_x, offset_y;
extern double tile_size;

void game_load_resources(void);
void game_unload_resources(void);
void game_new(void);
bool is_game_end(void);
void game_draw(cairo_t *cr, int width, int height);
void game_toggle_cell(int i, int j);
bool game_is_peg_at(int i, int j);
bool game_move(int src_x, int src_y, int dst_x, int dst_y);
const char *game_cheese(void);

#endif // PEGSOLITAIRE_GAME_H
