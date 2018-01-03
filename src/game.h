/*
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef PEGSOLITAIRE_GAME_H
#define PEGSOLITAIRE_GAME_H

#include <gtk/gtk.h>

enum
{
  BOARD_ENGLISH,
  BOARD_EUROPEAN
} game_board_enum;


int game_new ();
int is_game_end ();
int game_draw (GtkWidget * widget, GdkPixmap * pixmap, gint height,
               gint width, gint tile_size, int force);
int game_draw_cell (GtkWidget * widget, GdkPixmap * pixmap, gint height,
                    gint width, gint tile_size, gint x, gint y);
void game_toggle_cell (int i, int j);
gboolean game_is_peg_at (int i, int j);
gboolean game_move (int src_x, int src_y, int dst_x, int dst_y);
int game_count_pegs_on_board ();
gchar *game_cheese ();

#endif // PEGSOLITAIRE_GAME_H
