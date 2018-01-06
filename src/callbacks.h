#ifndef PEGSOLITAIRE_CALLBACKS_H
#define PEGSOLITAIRE_CALLBACKS_H

#include <gtk/gtk.h>

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
on_pegSolitaireWindow_configure_event (GtkWidget * widget,
                                       GdkEventConfigure * event,
                                       gpointer user_data);

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
