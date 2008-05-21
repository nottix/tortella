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

//FUNZIONI PER LA CREAZIONE DELLE GUI
int open_chatroom_gui(void);
int open_pm_gui(void);

gint destroyapp (GtkWidget *widget, gpointer gdata);

gint ClosingAppWindow (GtkWidget *widget, gpointer gdata);

gint open_chat(GtkWidget *widget, GdkEventButton *event, gpointer func_data);

gint open_conversation(GtkWidget *widget, GdkEventButton *event, gpointer func_data);

GtkWidget *create_list(int index );

GtkWidget *create_menu(void);

GtkWidget *create_text(int i );

GtkWidget *create_searchbar(void);

int open_chatroom_gui(void);

int open_pm_gui(void);

#endif //GUI_H
