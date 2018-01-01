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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h> // for exit()

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "game.h"
#include "gridframe.h"
#include "preimage.h"

void update_statusbar (int moves);

extern GtkWidget *pegSolitaireWindow;
extern GtkWidget *boardDrawingArea;
extern GtkWidget *messagewidget;
extern gint game_moves;
extern gint game_board_type;
extern gint game_board_size;
extern GdkPixmap *board_pixmap;
extern gint tile_size, prior_tile_size;
extern gint width, height;
extern guint redraw_all_idle_id;
extern guint resize_all_idle_id;
extern gboolean clear_game;
extern gboolean clear_buffer;
extern GtkWidget *gameframe;
extern gint piece_x;
extern gint piece_y;
extern gint button_down;
extern GamesPreimage *peg_preimage;
extern GamesPreimage *hole_preimage;
extern GdkPixbuf *peg_pixbuf;
extern GdkPixbuf *hole_pixbuf;

/* These have been lifted straight from eog. */

#define hand_closed_data_width 20
#define hand_closed_data_height 20
static char hand_closed_data_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x80, 0x3f, 0x00,
  0x80, 0xff, 0x00, 0x80, 0xff, 0x00, 0xb0, 0xff, 0x00, 0xf0, 0xff, 0x00,
  0xe0, 0xff, 0x00, 0xe0, 0x7f, 0x00, 0xc0, 0x7f, 0x00, 0x80, 0x3f, 0x00,
  0x00, 0x3f, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define hand_closed_mask_width 20
#define hand_closed_mask_height 20
static char hand_closed_mask_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x80, 0x3f, 0x00, 0xc0, 0xff, 0x00,
  0xc0, 0xff, 0x01, 0xf0, 0xff, 0x01, 0xf8, 0xff, 0x01, 0xf8, 0xff, 0x01,
  0xf0, 0xff, 0x01, 0xf0, 0xff, 0x00, 0xe0, 0xff, 0x00, 0xc0, 0x7f, 0x00,
  0x80, 0x7f, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define hand_open_data_width 20
#define hand_open_data_height 20
static char hand_open_data_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,
  0x60, 0x36, 0x00, 0x60, 0x36, 0x00, 0xc0, 0x36, 0x01, 0xc0, 0xb6, 0x01,
  0x80, 0xbf, 0x01, 0x98, 0xff, 0x01, 0xb8, 0xff, 0x00, 0xf0, 0xff, 0x00,
  0xe0, 0xff, 0x00, 0xe0, 0x7f, 0x00, 0xc0, 0x7f, 0x00, 0x80, 0x3f, 0x00,
  0x00, 0x3f, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define hand_open_mask_width 20
#define hand_open_mask_height 20
static char hand_open_mask_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x3f, 0x00,
  0xf0, 0x7f, 0x00, 0xf0, 0x7f, 0x01, 0xe0, 0xff, 0x03, 0xe0, 0xff, 0x03,
  0xd8, 0xff, 0x03, 0xfc, 0xff, 0x03, 0xfc, 0xff, 0x01, 0xf8, 0xff, 0x01,
  0xf0, 0xff, 0x01, 0xf0, 0xff, 0x00, 0xe0, 0xff, 0x00, 0xc0, 0x7f, 0x00,
  0x80, 0x7f, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
GdkCursor *hand_closed_cursor = NULL;
GdkCursor *hand_open_cursor = NULL;
GdkCursor *default_cursor = NULL;
enum {
  CURSOR_NONE = 0,
  CURSOR_OPEN,
  CURSOR_CLOSED
};

static void
set_cursor (int cursor)
{
	static int prev_cursor = -1;
  if ((cursor == CURSOR_NONE) && (prev_cursor != CURSOR_NONE))
    gdk_window_set_cursor (gtk_widget_get_window (pegSolitaireWindow), default_cursor);
  if ((cursor == CURSOR_OPEN) && (prev_cursor != CURSOR_OPEN))
    gdk_window_set_cursor (gtk_widget_get_window (pegSolitaireWindow), hand_open_cursor);
  if ((cursor == CURSOR_CLOSED) && (prev_cursor != CURSOR_CLOSED))
    gdk_window_set_cursor (gtk_widget_get_window (pegSolitaireWindow), hand_closed_cursor);
	prev_cursor = cursor;
}
static GdkCursor *
make_cursor (char *data, char *mask_data)
{
  GdkColor fg = { 0, 65535, 65535, 65535 };
  GdkColor bg = { 0, 0, 0, 0 };
  GdkPixmap *source;
  GdkPixmap *mask;
  GdkCursor *cursor;

  /* Yeah, hard-coded sizes are bad. */
  source = gdk_bitmap_create_from_data (NULL, data, 20, 20);
  mask = gdk_bitmap_create_from_data (NULL, mask_data, 20, 20);

  cursor = gdk_cursor_new_from_pixmap (source, mask, &fg, &bg, 10, 10);

  g_object_unref (source);
  g_object_unref (mask);

  return cursor;
}

static void
board_draw ()
{	
	GtkWidget *w;
	static GdkGC *backgc = NULL;
	GdkColor *bg_color;
	GtkStyle *style;
  GtkAllocation allocation;
	
	w = boardDrawingArea;
	
  if (board_pixmap)
    g_object_unref (board_pixmap);

  gtk_widget_get_allocation(w, &allocation);
  board_pixmap = gdk_pixmap_new (gtk_widget_get_window (w), allocation.width,
                                 allocation.height, -1);
	if (!backgc)
    backgc = gdk_gc_new (gtk_widget_get_window (w));
	style = gtk_widget_get_style (w);
	bg_color = gdk_color_copy (&style->bg[GTK_STATE_NORMAL]);
  gdk_gc_set_foreground (backgc, bg_color);
  gdk_gc_set_fill (backgc, GDK_SOLID);
  gdk_color_free (bg_color);
	
  gdk_draw_rectangle (board_pixmap, backgc, TRUE, 0, 0,
                      allocation.width, allocation.height);
	
	clear_buffer = clear_game = 0;
	gtk_widget_queue_draw (w);
}

//redraw the board and pegs
static gboolean
redraw_all (void)
{
	board_draw ();
	game_draw (pegSolitaireWindow, board_pixmap, height, width, tile_size, 1);
	return 0;
}

// redraw everything in a thread
gboolean
resize_all (void)
{
  if (tile_size != prior_tile_size)
	  {
		  if (peg_pixbuf != NULL)
        g_object_unref (peg_pixbuf);
      peg_pixbuf = NULL;
		  if (hole_pixbuf != NULL)
        g_object_unref (hole_pixbuf);
      hole_pixbuf = NULL;

      if (peg_preimage)
			  {
          peg_pixbuf = games_preimage_render (peg_preimage,
				                                	    tile_size/1.666,
					                                    tile_size/1.666, NULL);
        }

			if (hole_preimage)
			  {
          hole_pixbuf = games_preimage_render (hole_preimage,
				                                	     tile_size/1.666,
					                                     tile_size/1.666, NULL);
        }

      if (peg_pixbuf == NULL || hole_pixbuf == NULL) 
			  {
          GtkWidget *dialog;
          dialog = gtk_message_dialog_new (GTK_WINDOW (pegSolitaireWindow), 
					                                 GTK_DIALOG_MODAL,
				                                   GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
				                                   _("The theme for this game failed "
				                                     "to render.\n\nPlease check that "
				                                     "Peg Solitaire is installed "
				                                     "correctly."));
          gtk_dialog_run (GTK_DIALOG (dialog));
          exit (1);
        }
			prior_tile_size = tile_size;
	  }

	if (redraw_all_idle_id)
    g_source_remove (redraw_all_idle_id);

  redraw_all_idle_id = g_idle_add_full (G_PRIORITY_DEFAULT_IDLE + 1,
	                              				(GSourceFunc) redraw_all, NULL, NULL);

	resize_all_idle_id = 0;
	return 0;
}

static void 
update_tile_size ()
{
	if (width < height)
	  tile_size = width / game_board_size;
	else
	  tile_size = height / game_board_size;
}

// the size has changed, show the resize in a thread.
static void
recalculate_size (void)
{
	update_tile_size ();
	
	if (clear_buffer || clear_game || tile_size != prior_tile_size)
		{
      if (resize_all_idle_id)
        g_source_remove (resize_all_idle_id);

      resize_all_idle_id = g_idle_add ((GSourceFunc) resize_all, NULL);
			clear_buffer = 1;
    }
}

void
on_helpAboutMenuItem_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GdkPixbuf *pixbuf;
  GtkWidget *pegSolitaireAboutDialog;
  pegSolitaireAboutDialog = create_pegSolitaireAboutDialog ();
	pixbuf = create_pixbuf ("pegsolitaire-icon.png");
  if (pixbuf)
    {
      gtk_window_set_icon (GTK_WINDOW (pegSolitaireAboutDialog), pixbuf);
      g_object_unref (pixbuf);
    }

  gtk_widget_show_all (pegSolitaireAboutDialog);
  gtk_dialog_run (GTK_DIALOG(pegSolitaireAboutDialog));
  gtk_widget_hide (pegSolitaireAboutDialog);
}

void
on_helpContentsMenuItem_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GError *err = NULL;
  /* TODO: gtk3: change to gtk_show_uri_on_window */
  gtk_show_uri (gtk_widget_get_screen (pegSolitaireWindow), "ghelp:pegsolitaire",
      gtk_get_current_event_time (), &err);
  if (err != NULL) {
    g_warning (_("Cannot show help: %s"), err->message);
    g_error_free (err);
  }
}

static void
initiate_new_game (int board_type, int board_size)
{
	game_board_type = board_type;
	game_board_size = board_size;
	update_tile_size ();
	clear_game = 1;
  game_new ();

	gtk_label_set_text (GTK_LABEL (messagewidget), "");
	update_statusbar (game_moves);
	
	games_grid_frame_set (GAMES_GRID_FRAME (gameframe), board_size, board_size);
  recalculate_size ();
}

void
on_gameRestartMenuItem_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	initiate_new_game (game_board_type, game_board_size);	
}

void
on_pegSolitaireWindow_activate_default (GtkWindow       *window,
                                        gpointer         user_data)
{
}

void
on_pegSolitaireWindow_destroy          (GObject         *object,
                                        gpointer         user_data)
{
	gtk_main_quit();
}

void
on_gameQuitMenuItem_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


gboolean
on_pegSolitaireWindow_configure_event  (GtkWidget       *widget,
                                        GdkEventConfigure *ev,
                                        gpointer         user_data)
{
  return FALSE;
}

gboolean
on_boardDrawingArea_motion_notify_event (GtkWidget       *widget,
                                         GdkEventMotion  *event,
                                         gpointer         user_data)
{
	int i = event->x / tile_size;
	int j = event->y / tile_size;
	int icon_size = tile_size / 1.666;
	if (button_down == 1)
	  {
			//before we draw the peg, let's expose the board again.
			gdk_draw_pixmap (gtk_widget_get_window (widget),
			                 gtk_widget_get_style (widget)->fg_gc[gtk_widget_get_state (widget)],
											 board_pixmap, 0, 0, 
											 0, 0,
			                 tile_size * game_board_size, tile_size * game_board_size);
			
			gdk_draw_pixbuf (gtk_widget_get_window (widget),
			                 gtk_widget_get_style (widget)->fg_gc[gtk_widget_get_state (widget)],
			                 peg_pixbuf, 0, 0, 
			                 event->x - (icon_size / 2), event->y - (icon_size/2), icon_size, icon_size, 
			                 GDK_RGB_DITHER_NORMAL, 0, 0);
	  }
	else
		{
			//switch up the pointer when we're over a peg and what-not.
			if (game_is_peg_at (i, j) == TRUE)
			  {
					if ((int) event->x % tile_size < (tile_size / 2) + (icon_size / 2) && 
						  (int) event->x % tile_size > (tile_size / 2) - (icon_size / 2) &&
					    (int) event->y % tile_size < (tile_size / 2) + (icon_size / 2) && 
						  (int) event->y % tile_size > (tile_size / 2) - (icon_size / 2))
  				  set_cursor (CURSOR_OPEN);
					else
  					set_cursor (CURSOR_NONE);
				}
			else
				set_cursor (CURSOR_NONE);
					
		}
  return FALSE;
}

gboolean
on_boardDrawingArea_button_press_event (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	if ((event->button == 1) && (button_down == 0))
	  {
	    int i, j;
			if (is_game_end () == 1)
				return FALSE;
			
	    i = event->x / tile_size;
	    j = event->y / tile_size;
	    
			if (game_is_peg_at (i, j) == FALSE)
				return FALSE;
			set_cursor (CURSOR_CLOSED);

			button_down = 1;
			game_toggle_cell (i, j);
			game_draw (pegSolitaireWindow, board_pixmap, height, width, tile_size, 0);
			piece_x = i;
			piece_y = j;
			GdkRectangle update;
			update.x = i *tile_size;
			update.y = j * tile_size;
			update.width = tile_size;
			update.height = tile_size;
			gtk_widget_draw (widget, &update);
	}
	return FALSE;
}


gboolean
on_boardDrawingArea_button_release_event (GtkWidget       *widget,
                                          GdkEventButton  *event,
                                          gpointer         user_data)
{
	if (event->button == 1)
	  {
			if (button_down == 1)
			  {
	        int i, j;
					button_down = 0;
					set_cursor (CURSOR_NONE);
	        i = event->x / tile_size;
	        j = event->y / tile_size;
					if (game_move (piece_x, piece_y, i, j) == FALSE)
					  {
							//put the peg back where we started.
							game_toggle_cell (piece_x, piece_y);
        			game_draw (pegSolitaireWindow, board_pixmap, height, width, 
			                        tile_size, 0);
							return FALSE;
						}
					game_draw (pegSolitaireWindow, board_pixmap, height, width, tile_size, 
					           0);
					update_statusbar (game_moves);
					if (is_game_end ())
					  {
						  gchar *str;
	            str = g_strdup_printf ("%s!",	game_cheese());
              gtk_label_set_text (GTK_LABEL (messagewidget), str);
              g_free (str);
						}
						else
							set_cursor (CURSOR_OPEN);
		    }
		}
  return FALSE;
}

gboolean
on_boardDrawingArea_expose_event       (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	if (clear_game)
		return FALSE;
	
  gdk_draw_pixmap (gtk_widget_get_window (widget),
                   gtk_widget_get_style (widget)->fg_gc[gtk_widget_get_state (widget)],
                   board_pixmap,
                   event->area.x, event->area.y,
                   event->area.x, event->area.y,
                   event->area.width, event->area.height);
  return FALSE;
}

gboolean
on_boardDrawingArea_configure_event    (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
  if (!default_cursor)
    default_cursor = gdk_cursor_new (GDK_LEFT_PTR);

  if (!hand_closed_cursor)
    hand_closed_cursor = make_cursor (hand_closed_data_bits, 
	                                    hand_closed_mask_bits);

  if (!hand_open_cursor)
    hand_open_cursor = make_cursor (hand_open_data_bits, hand_open_mask_bits);
	
  width = event->width;
	height = event->height;
	recalculate_size ();
  return FALSE;
}

void
on_gameEnglishBeginnerMenuItem_activate (GtkMenuItem     *menuitem,
                                         gpointer         user_data)
{
	initiate_new_game (BOARD_ENGLISH, 7);
}

void
on_gameEnglishIntermediateMenuItem_activate (GtkMenuItem     *menuitem,
                                             gpointer         user_data)
{
	initiate_new_game (BOARD_ENGLISH, 11);
}

void
on_gameEnglishAdvancedMenuItem_activate (GtkMenuItem     *menuitem,
                                         gpointer         user_data)
{
	initiate_new_game (BOARD_ENGLISH, 15);
}

void
on_gameEuropeanBeginnerMenuItem_activate (GtkMenuItem     *menuitem,
                                          gpointer         user_data)
{
	initiate_new_game (BOARD_EUROPEAN, 7);
}

void
on_gameEuropeanIntermediateMenuItem_activate (GtkMenuItem     *menuitem,
                                              gpointer         user_data)
{
	initiate_new_game (BOARD_EUROPEAN, 11);
}

void
on_gameEuropeanAdvancedMenuItem_activate (GtkMenuItem     *menuitem,
                                          gpointer         user_data)
{
	initiate_new_game (BOARD_EUROPEAN, 15);
}
