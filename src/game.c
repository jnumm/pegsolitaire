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

#include "game.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "support.h"

extern GdkPixbuf *peg_pixbuf;
extern GdkPixbuf *hole_pixbuf;
#define DEFAULT_GAME_BOARD_SIZE 7
#define DEFAULT_GAME_BOARD_TYPE BOARD_ENGLISH
gchar **game_board;
// 7x7, 1 means there's a peg, 0 means no peg.

gchar **game_board_mask;
// 7x7, 1 means it's part of the cross, 0 means not.

gchar **game_board_dirty;
// 7x7, 1 means the cell needs to be redrawn, 0 means not.

gint game_moves;
gint game_board_size = DEFAULT_GAME_BOARD_SIZE;
gint game_board_type = 0;       // 0 = english, 1 = european

static int
create_game_board_mask ()
{
  int i, j;
  int n = game_board_size;
  int m = n / 2;                // length of one side of the cross
  assert (n % 2 == 1);
  assert (m % 2 == 1);
  // for those keeping score, this means n=3,7,11,15,19... is valid.

  // allocate the game board mask
  game_board_mask = realloc (game_board_mask,
                             game_board_size * sizeof (gchar *));
  if (game_board_mask == NULL)
    return -1;
  memset (game_board_mask, 0, game_board_size * sizeof (gchar *));
  for (i = 0; i < game_board_size; i++) {
    game_board_mask[i] = realloc (game_board_mask[i],
                                  game_board_size * sizeof (gchar));
    if (game_board_mask[i] == NULL)
      return -1;
    memset (game_board_mask[i], 0, game_board_size * sizeof (gchar));
  }

  if (game_board_type == BOARD_ENGLISH) {
    // fill the centre row of the cross
    for (i = n / 2 - m / 2; i < n - (n / 2 - m / 2); i++) {
      for (j = 0; j < game_board_size; j++) {
        game_board_mask[i][j] = 1;
      }
    }

    // fill the centre column of the cross
    for (i = 0; i < game_board_size; i++) {
      for (j = n / 2 - m / 2; j < n - (n / 2 - m / 2); j++) {
        game_board_mask[i][j] = 1;
      }
    }
  } else if (game_board_type == BOARD_EUROPEAN) {
    game_board_type = BOARD_ENGLISH;
    create_game_board_mask ();
    game_board_type = BOARD_EUROPEAN;
    for (i = n / 2 - m / 1.666; i < n - (n / 2 - m / 1.666); i++) {
      for (j = n / 2 - m / 1.666; j < n - (n / 2 - m / 1.666); j++) {
        game_board_mask[i][j] = 1;
      }
    }
  }

  return 0;
}

static int
game_init ()
{
  int i;
  // setup the shape of the game board
  create_game_board_mask ();

  // allocate the game board dirty map
  game_board_dirty = realloc (game_board_dirty,
                              game_board_size * sizeof (gchar *));
  if (game_board_dirty == NULL)
    return -1;
  memset (game_board_dirty, 0, game_board_size * sizeof (gchar *));
  for (i = 0; i < game_board_size; i++) {
    game_board_dirty[i] = realloc (game_board_dirty[i],
                                   game_board_size * sizeof (gchar));
    if (game_board_dirty[i] == NULL)
      return -1;
    memset (game_board_dirty[i], 0, game_board_size * sizeof (gchar));
  }

  // allocate the game board
  game_board = realloc (game_board, game_board_size * sizeof (gchar *));
  if (game_board == NULL)
    return -1;
  memset (game_board, 0, game_board_size * sizeof (gchar *));
  for (i = 0; i < game_board_size; i++) {
    game_board[i] = realloc (game_board[i], game_board_size * sizeof (gchar));
    if (game_board[i] == NULL)
      return -1;
    memset (game_board[i], 0, game_board_size * sizeof (gchar));
  }
  return 0;
}

static int
game_clear ()
{
  int i, j;
  int size = game_board_size;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      if (game_board_mask[i][j] == 0)
        continue;
      if (game_board[i][j] == 0) {
        game_board[i][j] = 1;
        game_board_dirty[i][j] = 1;
      }
    }
  }
  if (game_board[size / 2][size / 2] == 1) {
    game_board[size / 2][size / 2] = 0;
    game_board_dirty[size / 2][size / 2] = 1;
  }
  game_moves = 0;
  return 0;
}

int
game_count_pegs_on_board ()
{
  int i, j, pegs_left = 0;
  // find the number of pegs on the board
  for (i = 0; i < game_board_size; i++) {
    for (j = 0; j < game_board_size; j++) {
      if (game_board[i][j])
        pegs_left++;
    }
  }
  return pegs_left;
}

int
game_new ()
{
  int err;
  err = game_init ();
  if (!err)
    err = game_clear ();
  return err;
}

int
is_game_end ()
{
  int i, j, k;

  // are there any two pegs adjacent in a row?
  for (i = 0; i < game_board_size; i++) {
    for (j = 0; j < game_board_size - 1; j++) {
      if (game_board[i][j] && game_board[i][j + 1]) {
        // great, but is the entire row filled with pegs?
        for (k = 0; k < game_board_size; k++) {
          if (game_board_mask[i][k] == 1)
            if (game_board[i][k] == 0)
              return 0; // nope, the game is still on.
        }
      }
    }
  }

  // are there any two pegs adjacent in a column?
  for (i = 0; i < game_board_size - 1; i++) {
    for (j = 0; j < game_board_size; j++) {
      if (game_board[i][j] && game_board[i + 1][j]) {
        // great, but is the entire row filled with pegs?
        for (k = 0; k < game_board_size; k++) {
          if (game_board_mask[k][j] == 1)
            if (game_board[k][j] == 0)
              return 0; // nope, the game is still on.
        }
      }
    }
  }

  // no pegs adjacent.  no more moves means it's game over.
  return 1;
}

void
game_toggle_cell (int i, int j)
{
  if (i < 0 || j < 0 || i >= game_board_size || j >= game_board_size)
    return;

  if (game_board_mask[i][j]) {
    game_board[i][j] = !game_board[i][j];
    game_board_dirty[i][j] = 1;
  }
}

gboolean
game_is_peg_at (int i, int j)
{
  if (i < 0 || j < 0 || i >= game_board_size || j >= game_board_size)
    return FALSE;
  if (game_board_mask[i][j] == 0)
    return FALSE;
  return game_board[i][j];
}

gboolean
game_is_valid_move (int src_x, int src_y, int dst_x, int dst_y)
{
  int delta_x, delta_y;
  if (src_x < 0 || src_y < 0 || dst_x < 0 || dst_y < 0)
    return FALSE;
  if (src_x >= game_board_size || src_y >= game_board_size)
    return FALSE;
  if (dst_x >= game_board_size || dst_y >= game_board_size)
    return FALSE;
  if (game_board_mask[src_x][src_y] == 0
      || game_board_mask[dst_x][dst_y] == 0)
    return FALSE;

  // well the peg has to be out of the source pos'n already
  if (game_is_peg_at (src_x, src_y) == TRUE)
    return FALSE;
  // and there can't be a peg at the destination either.
  if (game_is_peg_at (dst_x, dst_y) == TRUE)
    return FALSE;

  //is it 2 away with a peg in the middle with a peg in it?
  delta_x = src_x - dst_x;
  delta_y = src_y - dst_y;
  if (delta_x == 0 && (delta_y == -2 || delta_y == 2))
    return game_is_peg_at (dst_x, dst_y + (delta_y / 2));
  else if (delta_y == 0 && (delta_x == -2 || delta_x == 2))
    return game_is_peg_at (dst_x + (delta_x / 2), dst_y);
  return FALSE;
}

// move peg from src to dst, taking intermediate peg out.
// presumes that peg from src is already removed
gboolean
game_move (int src_x, int src_y, int dst_x, int dst_y)
{
  if (game_is_valid_move (src_x, src_y, dst_x, dst_y) == TRUE) {
    int delta_x = src_x - dst_x;
    int delta_y = src_y - dst_y;
    // okay, the peg has been taken out of src_x,src_y and is in the air.
    // it is placed on dst_x,dst_y, and it's a valid move.
    // this means that the dst_x,dst_y doesn't have a peg in it.
    // it also means the one tile we jumped over does have a peg in it.

    // take the jumped peg out.
    if (delta_x == 0)
      game_toggle_cell (dst_x, dst_y + (delta_y / 2));
    else
      game_toggle_cell (dst_x + (delta_x / 2), dst_y);

    // put the source peg into the destination spot.
    game_toggle_cell (dst_x, dst_y);
    game_moves++;
    return TRUE;
  }
  return FALSE;
}

int
game_draw (GtkWidget * widget, GdkPixmap * pixmap, gint height, gint width,
           gint tile_size, int force)
{
  int i, j;
  for (i = 0; i < game_board_size; i++) {
    for (j = 0; j < game_board_size; j++) {
      if ((game_board_dirty[i][j] == 1) || (force)) {
        game_draw_cell (widget, pixmap, height, width, tile_size, i, j);
        game_board_dirty[i][j] = 0;
      }
    }
  }

  gtk_widget_queue_draw (widget);
  return 0;
}

int
game_draw_cell (GtkWidget * widget, GdkPixmap * pixmap, gint height,
                gint width, gint tile_size, gint x, gint y)
{
  GdkPixbuf *p;
  static GdkGC *backgc = NULL;
  GdkColor *bg_color;
  GtkStyle *style;
  if (x < 0 || y < 0)
    return -1;
  if (x >= game_board_size || y >= game_board_size)
    return -1;
  if (game_board_mask[x][y] == 0)
    return 1;
  if (game_board[x][y])
    p = peg_pixbuf;
  else
    p = hole_pixbuf;

  if (!backgc)
    backgc = gdk_gc_new (gtk_widget_get_window (widget));
  style = gtk_widget_get_style (widget);
  bg_color = gdk_color_copy (&style->bg[GTK_STATE_NORMAL]);
  gdk_gc_set_foreground (backgc, bg_color);
  gdk_gc_set_fill (backgc, GDK_SOLID);
  gdk_color_free (bg_color);

  gdk_draw_rectangle (pixmap, backgc, TRUE,
                      (x * tile_size), (y * tile_size), tile_size, tile_size);
  int icon_size = tile_size / 1.666;
  gdk_draw_pixbuf (pixmap, backgc, p, 0, 0,
                   (x * tile_size) + (tile_size / 2) - (icon_size / 2),
                   (y * tile_size) + (tile_size / 2) - (icon_size / 2),
                   icon_size, icon_size, GDK_RGB_DITHER_NORMAL, 0, 0);

  GdkRectangle update;
  update.x = x * tile_size;
  update.y = y * tile_size;
  update.width = tile_size;
  update.height = tile_size;
  gtk_widget_draw (widget, &update);

  return 0;
}

gchar *
game_cheese ()
{
  int i = 6;
  int pegs_left = game_count_pegs_on_board ();
  static gchar *cheese[] = {
    N_("GENIUS"),
    N_("OUTSTANDING"),
    N_("Sensational"),
    N_("Excellent"),
    N_("Very Good"),
    N_("Not Bad"),
    N_("Better Luck Next Time"),
  };
  if (pegs_left > 6)
    i = 6;
  else if (pegs_left > 1)
    i = pegs_left;
  else if (pegs_left == 1) {
    if (game_board[game_board_size / 2][game_board_size / 2] == 1)
      i = 1;
    else
      i = 0;
  }
  return cheese[i];
}
