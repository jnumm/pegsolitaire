/*  share.h: Global variables of pegsolitaire.
 *  Copyright (C) 2018 Juhani Numminen
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

#ifndef PEGSOLITAIRE_SHARE_H
#define PEGSOLITAIRE_SHARE_H

#include <gtk/gtk.h>

//#include "preimage.h"

// defined in main.c
extern GtkWidget *pegSolitaireWindow;
extern GtkAboutDialog *pegSolitaireAboutDialog;
extern GtkWidget *boardDrawingArea;
extern GtkLabel *statusMessageLabel;
//extern GdkPixmap *board_pixmap;
extern guint resize_all_idle_id;
//extern GamesPreimage *peg_preimage;
//extern GamesPreimage *hole_preimage;
//extern GdkPixbuf *peg_pixbuf;
//extern GdkPixbuf *hole_pixbuf;

#endif // PEGSOLITAIRE_SHARE_H
