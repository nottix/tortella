/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
 
#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdkkeysyms.h>

GtkListStore *chat_model;
GtkTreeIter   chat_iter;

struct tree_model
{
  GtkListStore *user_model;
  GtkTreeIter   user_iter;
};
typedef struct tree_model tree_model;


//FUNZIONI PER LA CREAZIONE DELLE GUI
int open_chatroom_gui(void);
int open_pm_gui(void);

void destroyapp (GtkWidget *widget, gpointer gdata);

gint destroywindow(GtkWidget *widget, gpointer gdata);

gint ClosingAppWindow (GtkWidget *widget, gpointer gdata);

gint add_to_list(char *chat_name);

gint clear_chat_list();

gint open_chat(GtkWidget *widget, GdkEventButton *event, gpointer func_data);

gint open_conversation(GtkWidget *widget, GdkEventButton *event, gpointer func_data);

gint open_about(GtkWidget *widget, gpointer gdata);

gint set_to_online(GtkWidget *widget, gpointer gdata);

gint set_to_busy(GtkWidget *widget, gpointer gdata);

gint set_to_away(GtkWidget *widget, gpointer gdata);

gint search_chat_button(GtkWidget *widget, gpointer gdata);

gint send_text_message(GtkWidget *widget, GdkEventKey *event, gpointer gdata);

GtkWidget *create_users_list(int index);

GtkWidget *create_chat_list(int index);

GtkWidget *create_menu(void);

GtkWidget *create_text(int i );

GtkWidget *create_searchbar(void);

int open_chatroom_gui(void);

int open_pm_gui(void);

#endif //GUI_H
