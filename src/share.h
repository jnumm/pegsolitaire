/* share.h: Global variables of pegsolitaire.
 * Copyright (C) 2018 Juhani Numminen
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PEGSOLITAIRE_SHARE_H
#define PEGSOLITAIRE_SHARE_H

#include <gtk/gtk.h>

#include "preimage.h"

// defined in main.c
extern GtkWidget *pegSolitaireWindow;
extern GtkWidget *gameframe;
extern GtkWidget *boardDrawingArea;
extern GtkWidget *messagewidget;
extern GdkPixmap *board_pixmap;
extern gint tile_size, prior_tile_size;
extern gint width, height;
extern guint redraw_all_idle_id;
extern guint resize_all_idle_id;
extern gboolean clear_game;
extern gboolean clear_buffer;
extern gint piece_x;
extern gint piece_y;
extern gint button_down;
extern GamesPreimage *peg_preimage;
extern GamesPreimage *hole_preimage;
extern GdkPixbuf *peg_pixbuf;
extern GdkPixbuf *hole_pixbuf;

// defined in game.c
extern gint game_moves;
extern gint game_board_type;
extern gint game_board_size;

#endif // PEGSOLITAIRE_SHARE_H
