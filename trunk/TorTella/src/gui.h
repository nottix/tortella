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
#include "utils.h"
#include "controller.h"
#include "supernodedata.h"
#include "tortellaprotocol.h"

#define TOP		0x01
#define BOTTOM	0x02
#define CHAT 	0x03
#define PM 		0x04

#define ONLINE "Online"
#define BUSY   "Busy"
#define AWAY   "Away"

static GtkListStore *chat_model;
static GtkTreeIter   chat_iter;
static GHashTable *tree_model_hashtable = NULL;
static GHashTable *pm_data_hashtable = NULL;

//static int searching = 0; 

GtkWidget *bar_textfield;

struct tree_model
{
	GtkListStore *user_model;
	GtkTreeIter user_iter;
	GtkTextView *text_area;
	GtkTreeView *tree_view;
};
typedef struct tree_model tree_model;

struct pm_data {
	GtkWidget *window;
	GtkTextView *text_area;
};
typedef struct pm_data pm_data;


//FUNZIONI PER LA CREAZIONE DELLE GUI

gint gui_close_window_event(GtkWidget *widget, gpointer gdata);

gint gui_close_app_window_event (GtkWidget *widget, gpointer gdata);

void gui_close_event (GtkWidget *widget, gpointer gdata);

void gui_open_chat_event (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer userdata);

gint gui_leave_chat_event(GtkWidget *widget, gpointer gdata);

gint gui_leave_pm_event(GtkWidget *widget, gpointer gdata);

gint gui_add_chat(u_int8 chat_id, char *chat_name);

gint gui_clear_chat_list();

gint gui_clear_buffer(GtkTextView *widget);

gint gui_add_message(GtkTextView *widget, gchar *msg);

gint gui_add_user_to_chat(u_int8 chat_id, u_int8 id, char *user, u_int1 status);

gint gui_del_user_from_chat(u_int8 chat_id, u_int8 user_id);

gint gui_change_status(u_int8 user_id, char *status); 

gint gui_open_chat_button_event(GtkWidget *widget, GdkEventButton *event, gpointer func_data);

gint gui_open_conversation_event(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer userdata);

gint gui_open_about_event(GtkWidget *widget, gpointer gdata);

gint gui_set_to_online_event(GtkWidget *widget, gpointer gdata);

gint gui_set_to_busy_event(GtkWidget *widget, gpointer gdata);

gint gui_set_to_away_event(GtkWidget *widget, gpointer gdata);

gint gui_search_chat_event(GtkWidget *widget, gpointer gdata);

gint gui_send_text_message_event(GtkWidget *widget, GdkEventKey *event, gpointer gdata);

gint gui_send_pm_message_event(GtkWidget *widget, GdkEventKey *event, gpointer gdata);

GtkWidget *gui_create_users_list(u_int8 index);

GtkWidget *gui_create_chat_list(u_int8 index);

GtkWidget *gui_create_menu(void);

GtkWidget *gui_create_text(u_int8 chat_id, int type, int msg_type);

GtkWidget *gui_create_searchbar(void);

gint gui_create_chat_button(GtkWidget *widget, gpointer gdata);

int gui_open_chatroom(u_int8);

int gui_open_pm(u_int8 user_id, gchar *nickname);

tree_model *gui_get_tree_model(u_int8 chat_id);

int gui_add_msg_to_chat(u_int8 chat_id, char *msg);

int gui_add_msg_pm(u_int8 sender_id, char *msg);

pm_data *gui_pm_data_get(u_int8 id);

#endif //GUI_H
