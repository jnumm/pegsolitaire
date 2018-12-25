/*  render.h: functions for rendering a peg solitaire game
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

#ifndef PEGSOLITAIRE_RENDER_H
#define PEGSOLITAIRE_RENDER_H

#include <gtk/gtk.h>

void init_cursors(void);
void game_load_resources(void);
void game_unload_resources(void);

// The callback functions are explicitly marked for export due to combining
// -fvisibility=hidden with -Wl,--export-dynamic.
#pragma GCC visibility push(default)
gboolean drawarea_draw(GtkWidget *, cairo_t *, gpointer);
gboolean drawarea_motion(GtkWidget *, GdkEventMotion *, gpointer);
gboolean drawarea_button_press(GtkWidget *, GdkEventButton *, gpointer);
gboolean drawarea_button_release(GtkWidget *, GdkEventButton *, gpointer);
#pragma GCC visibility pop

#endif // PEGSOLITAIRE_GAME_H
