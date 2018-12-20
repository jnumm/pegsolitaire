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

#ifndef PEGSOLITAIRE_CALLBACKS_H
#define PEGSOLITAIRE_CALLBACKS_H

#include <gtk/gtk.h>

void init_cursors(void);

// The callback functions are explicitly marked for export due to combining
// -fvisibility=hidden with -Wl,--export-dynamic.
#pragma GCC visibility push(default)
gboolean drawarea_draw(GtkWidget *, cairo_t *, gpointer);
gboolean drawarea_motion(GtkWidget *, GdkEventMotion *, gpointer);
gboolean drawarea_button_press(GtkWidget *, GdkEventButton *, gpointer);
gboolean drawarea_button_release(GtkWidget *, GdkEventButton *, gpointer);

void menu_restart(GtkMenuItem *, gpointer);

void menu_eng_beginner(GtkMenuItem *, gpointer);
void menu_eng_intermediate(GtkMenuItem *, gpointer);
void menu_eng_advanced(GtkMenuItem *, gpointer);
void menu_eur_beginner(GtkMenuItem *, gpointer);
void menu_eur_intermediate(GtkMenuItem *, gpointer);
void menu_eur_advanced(GtkMenuItem *, gpointer);

void menu_help(GtkMenuItem *, gpointer);
void menu_about(GtkMenuItem *, gpointer);

void window_destroy(GObject *, gpointer);
void menu_quit(GtkMenuItem *, gpointer);
#pragma GCC visibility pop

#endif // PEGSOLITAIRE_CALLBACKS_H
