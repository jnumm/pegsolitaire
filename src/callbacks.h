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

void
init_cursors (void);

gboolean
resize_all (void);

void
on_helpAboutMenuItem_activate (GtkMenuItem * menuitem, gpointer user_data);

void
on_helpContentsMenuItem_activate (GtkMenuItem * menuitem, gpointer user_data);

void
on_gameRestartMenuItem_activate (GtkMenuItem * menuitem, gpointer user_data);

void on_pegSolitaireWindow_destroy (GObject * object, gpointer user_data);

void
on_gameQuitMenuItem_activate (GtkMenuItem * menuitem, gpointer user_data);

gboolean
  on_boardDrawingArea_motion_notify_event
  (GtkWidget * widget, GdkEventMotion * event, gpointer user_data);

gboolean
on_boardDrawingArea_button_press_event (GtkWidget * widget,
                                        GdkEventButton * event,
                                        gpointer user_data);

gboolean
  on_boardDrawingArea_button_release_event
  (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

gboolean
on_boardDrawingArea_expose_event (GtkWidget * widget,
                                  GdkEventExpose * event, gpointer user_data);

void
  on_gameEnglishBeginnerMenuItem_activate
  (GtkMenuItem * menuitem, gpointer user_data);

void
  on_gameEnglishIntermediateMenuItem_activate
  (GtkMenuItem * menuitem, gpointer user_data);

void
  on_gameEnglishAdvancedMenuItem_activate
  (GtkMenuItem * menuitem, gpointer user_data);
void
  on_gameEuropeanBeginnerMenuItem_activate
  (GtkMenuItem * menuitem, gpointer user_data);

void
  on_gameEuropeanIntermediateMenuItem_activate
  (GtkMenuItem * menuitem, gpointer user_data);

void
  on_gameEuropeanAdvancedMenuItem_activate
  (GtkMenuItem * menuitem, gpointer user_data);

gboolean
on_boardDrawingArea_configure_event (GtkWidget * widget,
                                     GdkEventConfigure * event,
                                     gpointer user_data);

void
on_helpAboutMenuItem_activate (GtkMenuItem * menuitem, gpointer user_data);

#endif // PEGSOLITAIRE_CALLBACKS_H
