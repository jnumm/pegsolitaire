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

#include <locale.h>
#include <stdio.h>
#include <stdlib.h> // for exit()

#include <gtk/gtk.h>

#include "callbacks.h"
#include "config.h"
#include "game.h"
#include "i18n.h"
#include "share.h"

// Exposed through share.h
GtkWidget *pegSolitaireWindow = NULL;
GtkAboutDialog *pegSolitaireAboutDialog = NULL;
GtkWidget *boardDrawingArea = NULL;
GtkLabel *statusMessageLabel = NULL;
// End of globals exposed through share.h

static GtkLabel *statusMovesLabel = NULL;
static gint session_xpos = 0;
static gint session_ypos = 0;

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

  game_load_resources ();

  GtkBuilder *builder = gtk_builder_new_from_file (PKGDATADIR "/pegsolitaire.glade");

  gtk_builder_connect_signals (builder, NULL);

  pegSolitaireWindow = GTK_WIDGET (gtk_builder_get_object (builder, "pegSolitaireWindow"));

  pegSolitaireAboutDialog = GTK_ABOUT_DIALOG (gtk_builder_get_object (builder, "pegSolitaireAboutDialog"));
  gtk_about_dialog_set_version (pegSolitaireAboutDialog, VERSION);

  statusMessageLabel = GTK_LABEL (gtk_builder_get_object (builder, "statusMessageLabel"));
  statusMovesLabel = GTK_LABEL (gtk_builder_get_object (builder, "statusMovesLabel"));
  update_statusbar (0);

  boardDrawingArea = GTK_WIDGET (gtk_builder_get_object (builder, "boardDrawingArea"));

  init_cursors ();

  game_new ();

  if (session_xpos > 0 && session_ypos > 0)
    gtk_window_move (GTK_WINDOW (pegSolitaireWindow),
                     session_xpos, session_ypos);

  gtk_widget_show_all (pegSolitaireWindow);

  /* main loop */
  gtk_main ();

  /* cleanup */
  game_unload_resources ();

  return 0;
}
