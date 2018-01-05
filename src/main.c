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

#include <locale.h>
#include <stdio.h>
#include <stdlib.h> // for exit()

#include <gtk/gtk.h>

#include "callbacks.h"
#include "config.h"
#include "game.h"
#include "gridframe.h"
#include "i18n.h"
#include "preimage.h"
#include "share.h"

// Exposed through share.h
GtkWidget *pegSolitaireWindow;
GtkWidget *gameframe;
GtkWidget *boardDrawingArea;
GtkLabel *statusMessageLabel;
GdkPixmap *board_pixmap;
gint tile_size, prior_tile_size;
gint width, height;
guint redraw_all_idle_id = 0;
guint resize_all_idle_id = 0;
gboolean clear_game = 1;
gboolean clear_buffer = 1;
gint piece_x;
gint piece_y;
gint button_down;
GamesPreimage *peg_preimage;
GamesPreimage *hole_preimage;
GdkPixbuf *peg_pixbuf;
GdkPixbuf *hole_pixbuf;
// End of globals exposed through share.h

GtkLabel *statusMovesLabel;
gint session_xpos = 0;
gint session_ypos = 0;

gboolean resize_all (void);

static void
create_boardDrawingArea ()
{
  gameframe = games_grid_frame_new (7, 7);
  games_grid_frame_set_padding (GAMES_GRID_FRAME (gameframe), 10, 10);
  gtk_widget_set_size_request (GTK_WIDGET (gameframe), 250, 250);

  boardDrawingArea = gtk_drawing_area_new ();
  gtk_widget_set_name (GTK_WIDGET (boardDrawingArea), "boardDrawingArea");

  gtk_widget_set_double_buffered (boardDrawingArea, 0);
  gtk_container_add (GTK_CONTAINER (gameframe), boardDrawingArea);
  gtk_widget_set_events (boardDrawingArea, GDK_EXPOSURE_MASK |
                         GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK |
                         GDK_BUTTON_RELEASE_MASK);

  g_signal_connect (G_OBJECT (boardDrawingArea), "expose_event",
                    G_CALLBACK (on_boardDrawingArea_expose_event), NULL);
  g_signal_connect (G_OBJECT (boardDrawingArea), "configure_event",
                    G_CALLBACK (on_boardDrawingArea_configure_event), NULL);
  g_signal_connect (G_OBJECT (boardDrawingArea), "button_press_event",
                    G_CALLBACK (on_boardDrawingArea_button_press_event),
                    NULL);
  g_signal_connect (G_OBJECT (boardDrawingArea), "button_release_event",
                    G_CALLBACK (on_boardDrawingArea_button_release_event),
                    NULL);
  g_signal_connect (G_OBJECT (boardDrawingArea), "motion_notify_event",
                    G_CALLBACK (on_boardDrawingArea_motion_notify_event),
                    NULL);
}

void
update_statusbar (int moves)
{
  // TRANSLATORS: This is the number of moves the player has made.
  gchar *str = g_strdup_printf (_("Moves: %d"), moves);
  gtk_label_set_text (statusMovesLabel, str);
  g_free (str);
}

/* Session Options */

static const GOptionEntry options[] = {
  {"x", 'x', 0, G_OPTION_ARG_INT, &session_xpos, N_("X location of window"),
   N_("X")},
  {"y", 'y', 0, G_OPTION_ARG_INT, &session_ypos, N_("Y location of window"),
   N_("Y")},
  {NULL}
};

GamesPreimage *
load_image (char *filename)
{
  char *fname;
  GamesPreimage *preimage;
  fname = g_build_filename (PKGDATADIR, filename, NULL);
  if (g_file_test (fname, G_FILE_TEST_EXISTS)) {
    preimage = games_preimage_new_from_file (fname, NULL);
  } else {
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                     _("Could not find the image:\n"
                                       "%s\n\n"
                                       "Please check that Peg Solitaire is installed correctly."),
                                     fname);
    gtk_dialog_run (GTK_DIALOG (dialog));
    exit (1);
  }
  g_free (fname);
  return preimage;
}

int
main (int argc, char *argv[])
{
  GOptionContext *context;
  GError *error = NULL;

  setlocale (LC_ALL, "");
#ifdef ENABLE_NLS
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

  context = g_option_context_new ("");
  g_option_context_add_main_entries (context, options, PACKAGE);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));

  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    fputs (error->message, stderr);
    exit (1);
  }

  gtk_init (&argc, &argv);

  // TODO: gtk3: change to gtk_builder_new_from_file (filename)
  GtkBuilder *builder = gtk_builder_new ();
  if (!gtk_builder_add_from_file (builder, PKGDATADIR "/pegsolitaire.glade", &error)) {
    fputs (error->message, stderr);
    exit (1);
  }

  gtk_builder_connect_signals (builder, NULL);

  pegSolitaireWindow = GTK_WIDGET (gtk_builder_get_object (builder, "pegSolitaireWindow"));
  statusMessageLabel = GTK_LABEL (gtk_builder_get_object (builder, "statusMessageLabel"));
  statusMovesLabel = GTK_LABEL (gtk_builder_get_object (builder, "statusMovesLabel"));
  update_statusbar (0);

  peg_preimage = load_image ("peg.svg");
  hole_preimage = load_image ("hole.svg");

  create_boardDrawingArea ();
  GtkBox *vbox = GTK_BOX (gtk_builder_get_object(builder, "pegSolitaireVBox"));
  gtk_box_pack_start (vbox, gameframe, TRUE, TRUE, 0);

  game_new ();

  if (session_xpos > 0 && session_ypos > 0)
    gtk_window_move (GTK_WINDOW (pegSolitaireWindow),
                     session_xpos, session_ypos);

  gtk_widget_show_all (pegSolitaireWindow);

  if (resize_all_idle_id)
    g_source_remove (resize_all_idle_id);

  resize_all_idle_id = g_idle_add ((GSourceFunc) resize_all, NULL);

  gtk_main ();
  return 0;
}
