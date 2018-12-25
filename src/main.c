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

#include <gtk/gtk.h>

#include "callbacks.h"
#include "config.h"
#include "game.h"
#include "i18n.h"
#include "render.h"
#include "share.h"

// Exposed through share.h
GtkWidget *pegSolitaireWindow = NULL;
GtkAboutDialog *pegSolitaireAboutDialog = NULL;
GtkWidget *boardDrawingArea = NULL;
GtkLabel *statusMessageLabel = NULL;
GtkLabel *statusMovesLabel = NULL;
// End of globals exposed through share.h

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
#ifdef ENABLE_NLS
    bindtextdomain(PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(PACKAGE, "UTF-8");
    textdomain(PACKAGE);
#endif

    const GOptionEntry app_opts[] = {{NULL}};
    GError *err = NULL;
    if (!gtk_init_with_args(&argc, &argv, "", app_opts, PACKAGE, &err)) {
        fprintf(stderr, _("pegsolitaire: Could not initialize GTK+.\n"));
        if (err)
            fprintf(stderr, "%s\n", err->message);
        return 1;
    }

    game_load_resources();

    GtkBuilder *builder =
        gtk_builder_new_from_file(PKGDATADIR "/pegsolitaire.glade");
    gtk_builder_connect_signals(builder, NULL);

    pegSolitaireWindow =
        GTK_WIDGET(gtk_builder_get_object(builder, "pegSolitaireWindow"));

    pegSolitaireAboutDialog = GTK_ABOUT_DIALOG(
        gtk_builder_get_object(builder, "pegSolitaireAboutDialog"));
    gtk_about_dialog_set_version(pegSolitaireAboutDialog, VERSION);

    statusMessageLabel =
        GTK_LABEL(gtk_builder_get_object(builder, "statusMessageLabel"));
    statusMovesLabel =
        GTK_LABEL(gtk_builder_get_object(builder, "statusMovesLabel"));
    update_statusbar();

    boardDrawingArea =
        GTK_WIDGET(gtk_builder_get_object(builder, "boardDrawingArea"));

    init_cursors();

    game_new();

    gtk_widget_show_all(pegSolitaireWindow);

    /* main loop */
    gtk_main();

    /* cleanup */
    game_unload_resources();

    return 0;
}
