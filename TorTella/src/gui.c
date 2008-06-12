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

#include "gui.h"

gint gui_close_window_event(GtkWidget *widget, gpointer gdata) {
        g_print("Closing window\n");
        gtk_widget_destroy(widget);
        return(FALSE);
}

/*-- This function allows the program to exit properly when the window is closed --*/
gint gui_close_app_window_event (GtkWidget *widget, gpointer gdata) {
        g_print ("Quitting...\n");
        gtk_main_quit();
        return (FALSE);
}

/*-- This function allows the program to exit properly when the window is closed --*/
void gui_close_event (GtkWidget *widget, gpointer gdata)
{
	g_print ("Quitting...\n");
	controller_leave_all_chat();
	controller_send_bye();
	//sleep(1);
	controller_exit();
	gtk_main_quit();
	//exit(0);
}

gint gui_leave_chat_event(GtkWidget *widget, gpointer gdata)
{
	char *str = (char*)gdata;
	u_int8 val = atoll((char*)gdata);
	g_print("Closing chat %lld, str: %s\n", val, str);
	//u_int8 chat_id = atoll((char*)(*tmp));
	//g_print("Closing chat %lld\n", chat_id);
	controller_leave_chat(val);
	//gtk_widget_destroy(widget);
	//Aggiungere il leave dalla chat
	return(FALSE);
}

gint gui_leave_pm_event(GtkWidget *widget, gpointer gdata)
{
	u_int8 val = atoll((char*)gdata);
	g_print("Closing pm %lld\n", val);
	pm_data *pm;
	if((pm = g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(val)))!=NULL) {
		gtk_widget_destroy(GTK_WIDGET(pm->window)); //TODO: DA CONTROLLARE
		g_hash_table_remove(pm_data_hashtable, (gconstpointer)to_string(val));
	}
	return (FALSE);
}

gint gui_add_chat(u_int8 chat_id, char *chat_name)
{
	GtkTreeIter iter;
	gboolean valid = TRUE;
	gchar *id;
	logger(INFO, "[add_chat_to_list]Init\n");
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(chat_model), &iter)==TRUE) {
		while(valid) {

			gtk_tree_model_get(GTK_TREE_MODEL(chat_model), &iter, 0, &id, -1);
			logger(INFO, "[add_chat_to_list]ID to add: %s\n", id);

			if(atoll(id)==chat_id) {
				logger(INFO, "[add_chat_to_list]Exiting: %s\n", id);
				return -1;
			}

			valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(chat_model), &iter); 
		}
	}

	gchar *msg = g_strdup_printf (to_string(chat_id));
	gchar *msg1 = g_strdup_printf (chat_name);
	printf("msg: %s\n", msg1);
	gtk_list_store_append (chat_model, /*temp*/&chat_iter);
	gtk_list_store_set (GTK_LIST_STORE(chat_model), /*temp*/&chat_iter, 0, msg, 1, msg1, -1);
	printf("msg: %s\n", msg);
	g_free (msg);
	g_free (msg1);
	return 0;
}

gint gui_add_user_to_chat(u_int8 chat_id, u_int8 id, char *user, u_int1 status)
{
	//TODO: CONTROLLO DUPLICATI,DOVREBBE ESSERE CORRETTO COSI'
	logger(INFO, "[gui_add_user_to_chat] Adding user %lld\n", id);
	gchar *msg = g_strdup_printf(user);
	if(tree_model_hashtable==NULL) {
		logger(INFO, "[add_user_to_chat_list]Hashtable NULL\n");
		return -1;
	}
	tree_model *mod = (tree_model*)g_hash_table_lookup(tree_model_hashtable,(gconstpointer)to_string(chat_id));

	if(mod!=NULL) {
		GtkTreeIter iter, iter2;
		gchar *id_tmp;
		gboolean valid = TRUE;
		gboolean valid2 = TRUE;
		if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mod->user_model), &iter)==TRUE) {			
			gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mod->user_model), &iter2);
			valid2 = gtk_tree_model_iter_next(GTK_TREE_MODEL(mod->user_model), &iter2);
			while(valid) {
				logger(INFO, "[gui_add_user_to_chat] entering while\n");
				gtk_tree_model_get(GTK_TREE_MODEL(mod->user_model), &iter, 1, &id_tmp, -1);
				logger(INFO, "[gui_add_user_to_chat]ID: %s\n", id_tmp);
				if(atoll(id_tmp) == id) {
					logger(INFO, "[gui_add_user_to_chat] existing user\n");	
					break;
				}
				if(atoll(id_tmp)!= id && valid2 == FALSE ) {
					logger(INFO, "[add_user_to_chat_list]Tree model non NULL\n");
					gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
					gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 0, msg, -1);
					g_free(msg);

					msg = g_strdup_printf(to_string(id));
					//gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
					gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 1, msg, -1);
					g_free(msg);

					//msg = g_strdup_printf(to_string((u_int8)status));
					if(status == ONLINE_ID)
						msg = g_strdup_printf(ONLINE);
					else if(status == BUSY_ID)
						msg = g_strdup_printf(BUSY);
					else if(status == AWAY_ID)
						msg = g_strdup_printf(AWAY);
					//gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
					gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 2, msg, -1);
					g_free(msg);
				}

				valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(mod->user_model), &iter); 
				if(valid2!= FALSE);
					valid2 = gtk_tree_model_iter_next(GTK_TREE_MODEL(mod->user_model), &iter2);
			}
		}
		else
		{
			logger(INFO, "inserting first user\n");
			gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
					gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 0, msg, -1);
					g_free(msg);

					msg = g_strdup_printf(to_string(id));
					//gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
					gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 1, msg, -1);
					g_free(msg);

					//msg = g_strdup_printf(to_string((u_int8)status));
					if(status == ONLINE_ID)
						msg = g_strdup_printf(ONLINE);
					else if(status == BUSY_ID)
						msg = g_strdup_printf(BUSY);
					else if(status == AWAY_ID)
						msg = g_strdup_printf(AWAY);
					//gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
					gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 2, msg, -1);
					g_free(msg);
		}
		/*logger(INFO, "[add_user_to_chat_list]Tree model non NULL\n"); COMMENTATO PER PROVA
		gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
		gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 0, msg, -1);
		g_free(msg);

		msg = g_strdup_printf(to_string(id));
		//gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
		gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 1, msg, -1);
		g_free(msg);

		//msg = g_strdup_printf(to_string((u_int8)status));
		if(status == ONLINE_ID)
			msg = g_strdup_printf(ONLINE);
		else if(status == BUSY_ID)
			msg = g_strdup_printf(BUSY);
		else if(status == AWAY_ID)
			msg = g_strdup_printf(AWAY);
		//gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
		gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 2, msg, -1);
		g_free(msg); */
	}
	else
		return -2;

	return 0;
}

gint gui_del_user_from_chat(u_int8 chat_id, u_int8 user_id)
{
	tree_model *mod = (tree_model*)g_hash_table_lookup(tree_model_hashtable,(gconstpointer)to_string(chat_id));

	GtkTreeIter iter;
	gboolean valid = TRUE;
	gchar *id;
	logger(INFO, "[remove_user_from_chat_list]Init\n");
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mod->user_model), &iter)==TRUE) {
		while(valid) {

			logger(INFO, "[remove_user_from_chat_list]start\n");
			gtk_tree_model_get(GTK_TREE_MODEL(mod->user_model), &iter, 1, &id, -1);
			logger(INFO, "[remove_user_from_chat_list]ID to remove: %s\n", id);

			if(atoll(id)==user_id) {
				logger(INFO, "[remove_user_from_chat_list]Removing: %s\n", id);
				gtk_list_store_remove(mod->user_model, &iter);
				logger(INFO, "[remove_user_from_list]Removed\n");
				return TRUE;
			}

			valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(mod->user_model), &iter);
			logger(INFO, "[remove_user_from_chat_list]next\n");

		}
	}
	return (FALSE);
}

gint gui_change_status(u_int8 user_id, char *status) 
{
	logger(INFO, "[manipulating_status] Enter function\n");
	GList *chat_id_list = g_hash_table_get_keys(tree_model_hashtable);
	int i=0;
	char *id;
	for(; i < g_list_length(chat_id_list); i++) {
		logger(INFO, "[manipulating_status] Entering for\n");
		u_int8 chat_id = atoll(g_list_nth_data(chat_id_list,i));
		logger(INFO, "[manipulating_status] after retrieving glist data chat_id %lld\n",chat_id);

		tree_model *chat_model_tmp = (tree_model*)g_hash_table_lookup(tree_model_hashtable, (gconstpointer)to_string(chat_id));
		
		logger(INFO, "[manipulating_status] after retrieving treemodel data\n");
		GtkTreeIter iter;
		gboolean valid = TRUE;
		if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(chat_model_tmp->user_model), &iter)==TRUE) {			
			logger(INFO, "[manipulating_status] after get iter first\n");
			
			while(valid) {
				logger(INFO, "[manipulating status] entering while\n");
				gtk_tree_model_get(GTK_TREE_MODEL(chat_model_tmp->user_model), &iter, 1, &id, -1);
				logger(INFO, "[manipulating_status]ID: %s\n", id);

				if(atoll(id)==user_id) {
					gtk_list_store_set(GTK_LIST_STORE(chat_model_tmp->user_model), &iter, 2, status, -1);
					logger(INFO, "[manipulating_status] changed status %s\n", status);
					//	logger(INFO, "[gui_add_chat]Exiting: %s\n", id);
					//	return -1;
				}

				valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(chat_model_tmp->user_model), &iter); 
			}
		}		
	}
	return (FALSE);
}

gint gui_clear_chat_list()
{
	gtk_list_store_clear(chat_model);
	return (FALSE);
}

gint gui_clear_buffer(GtkTextView *widget)
{
	GtkTextBuffer *text;
	text = gtk_text_buffer_new(NULL);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(widget),text);
	return (FALSE);
}

gint gui_add_message(GtkTextView *widget, gchar *msg)
{
	GtkTextBuffer *text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
	GtkTextIter iter;
	gtk_text_buffer_get_end_iter(text,&iter);
	char *first = strstr(msg, "\n");
	int len = first-msg;
	char *tmp = calloc(len+1, 1);
	strncpy(tmp, msg, len);  
	gtk_text_buffer_insert_with_tags_by_name(text, &iter, tmp, -1, "blue_fg", NULL);
	//gtk_text_buffer_insert(text,&iter,tmp,-1);
	msg+=len;
	gtk_text_buffer_get_end_iter(text,&iter),
	gtk_text_buffer_insert(text,&iter,msg,-1); 
	
	// Scrolling
	GtkTextIter new_iter;
	text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
	gtk_text_buffer_get_end_iter(text, &new_iter);
	GtkTextMark *mark = gtk_text_mark_new(NULL,FALSE);
	gtk_text_buffer_add_mark(text,mark,&new_iter);
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(widget), mark, 0.0, FALSE, 0 ,0);
	return (FALSE);
}


//EVENTO CHE SI SCATENA ALLA SELEZIONE DELLA CHAT.
gint gui_open_chat_button_event(GtkWidget *widget, GdkEventButton *event, gpointer func_data) {
	if (event->type==GDK_2BUTTON_PRESS ||
			event->type==GDK_3BUTTON_PRESS) {
		printf("I feel %s clicked with button %d\n",
				event->type==GDK_2BUTTON_PRESS ? "double" : "triple",
						event->button);

		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
		GList *lis = gtk_tree_selection_get_selected_rows(selection, &chat_model); //warning
		int i=0;
		for(; i<g_list_length(lis); i++) {
			printf("selected row: %s\n", (gchar*)g_list_nth_data(lis, i));
			//chat_id = atol(
		}
		//printf("size: %d\n", g_list_length(lis));
		//gpointer data = gtk_tree_selection_get_user_data(selection);
		//printf("data: %s\n", (char*)data);

		//if(data_search_chat_local())
		/* prende l'ID della chat e richiede la connessione al controller */
		gui_open_chatroom(1); 
	}

	return FALSE;
}

void gui_open_chat_event (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer userdata) {
	GtkTreeModel *model;
	GtkTreeIter   iter;

	g_print ("A row has been double-clicked!\n");

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gchar *name;

		gtk_tree_model_get(model, &iter, 0, &name, -1);

		g_print ("Double-clicked row contains name %s\n", name);

		chat *elem = data_get_chat(atoll(name));
		if(elem!=NULL) {

			
			//controller_request_list(elem->id); COMMENTATO PER PROVA
			usleep(200000);
			controller_connect_users(g_hash_table_get_values(elem->users));
			
			//controller_request_list(elem->id);  //VA IN DEADLOCK
			//TODO: Attendere che i risultati siano arrivati
			//sleep(2);
			gui_open_chatroom(elem->id);
			controller_join_chat(elem->id);
		}
		g_free(name);
	}
}

gint gui_open_conversation_event(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer userdata) {
	/*if (event->type==GDK_2BUTTON_PRESS ||
			event->type==GDK_3BUTTON_PRESS) {
		printf("I feel %s clicked with button %d\n",
				event->type==GDK_2BUTTON_PRESS ? "double" : "triple",
						event->button);
		gui_open_pm();  
	}*/
	GtkTreeModel *model;
	GtkTreeIter   iter;

	g_print ("A row has been double-clicked!\n");

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gchar *user_id;
		gchar *name; 
		gtk_tree_model_get(model, &iter, 0, &name, -1);
		gtk_tree_model_get(model, &iter, 1, &user_id, -1);

		g_print ("Double-clicked row contains name %s\n", user_id);

		//chat *elem = data_get_chat(atoll(name));
		if(user_id > 0 ) {
			//g_hash_table_insert(tree_model_hashtable, (gpointer)user_id, (gpointer)model); //PROVA NELLA STESSA HASHTABLE
			logger(INFO, "[open_conversation] nick length %d\n", strlen(name));
			if(g_hash_table_lookup(pm_data_hashtable, (gconstpointer)user_id) != NULL) {
				logger(INFO, "[open_conversation] conversazione già attiva\n");
				return (FALSE);
			}	
			gui_open_pm(atoll(user_id),name);
			// gui_open_chatroom(elem->id);
			// usleep(200);
			// controller_connect_users(g_hash_table_get_values(elem->users));
			// sleep(1);//FIXIT
			// controller_join_chat(elem->id);
		}
		// g_free(name);
	}
	return FALSE;
}

gint gui_open_about_event(GtkWidget *widget, gpointer gdata)
{
	GtkWidget *window;
	GtkWidget *label1;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect (window, "delete_event",
			G_CALLBACK (gui_close_window_event), NULL);

	gtk_container_set_border_width (GTK_CONTAINER (window), 25);

	label1 = gtk_label_new("\tTorTella Chat\nCreated by TorTella Team\nAll rights reserved 2007/2008");
	gtk_container_add (GTK_CONTAINER (window), label1);
	gtk_widget_show(label1); 
	gtk_widget_show(window);
	gtk_main();
	return 0;
}

gint gui_set_to_online_event(GtkWidget *widget, gpointer gdata)
{
	g_print("Online...\n");
	controller_change_status (ONLINE_ID);
	gui_change_status(servent_get_local()->id, ONLINE);
	return(FALSE);
}

gint gui_set_to_busy_event(GtkWidget *widget, gpointer gdata)
{
	g_print("Busy...\n");
	controller_change_status(BUSY_ID);
	gui_change_status(servent_get_local()->id, BUSY);
	return (FALSE);
}

gint gui_set_to_away_event(GtkWidget *widget, gpointer gdata)
{
	g_print("Away...\n");
	controller_change_status(AWAY_ID);
	gui_change_status(servent_get_local()->id, AWAY);	
	return (FALSE);
}

gint gui_search_chat_event(GtkWidget *widget, gpointer gdata)
{

	g_print("Search Chat: %s\n", gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
	//set_searching();
	gui_clear_chat_list();
	controller_search(gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
	//	GList *chats;
	//	int i=0, counter=6;
	//	chat *chat_val;
	//	while(counter--) { //TODO: trovare metodo migliore di stampa risultati
	//		chats = data_search_all_local_chat(gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
	//		for(; i<g_list_length(chats); i++) {
	//			chat_val = (chat*)g_list_nth_data(chats, i);
	//			gui_add_chat(chat_val->id, chat_val->title);
	//			
	//		}
	//		usleep(200);
	//	}
	return(FALSE);
}

gint gui_create_chat_button(GtkWidget *widget, gpointer gdata) {
	g_print("Create Chat: %s\n", gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
	return controller_create(gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
}

gint gui_send_text_message_event(GtkWidget *widget, GdkEventKey *event, gpointer gdata)
{
	if(event->type == GDK_KEY_PRESS && event->keyval == GDK_Return) {
	 	GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
		g_print("Enter pressed...\n");
		GtkTextIter start ;
		gtk_text_buffer_get_start_iter(buf, &start);
		g_print("Enter pressed...\n");
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(buf, &end);
		g_print("Enter pressed...\n");
		char *msg = gtk_text_buffer_get_text(buf, &start, &end, TRUE);		
		g_print("Enter pressed...\n");
		GtkTreeSelection *selection;
		
		u_int8 chat_id = atoll((char*)gdata);
		logger(INFO, "[send_text_message] chat id = %lld\n",chat_id);
		/* 					SUBSET						*/
		tree_model *chat_list_tmp = gui_get_tree_model(chat_id);
		
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(chat_list_tmp->tree_view));
		if(gtk_tree_selection_count_selected_rows(selection) > 0) {
			GList *subset = NULL;		
			logger(INFO, "[send_text_message] send to subset\n");
			GtkTreeIter tmp_iter;
			gboolean valid = TRUE;
			char *id;
			logger(INFO, "[add_user_tolist_of_subset]Init\n");
		
			if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(chat_list_tmp->user_model), &tmp_iter)==TRUE) {
				while(valid) {
					if(gtk_tree_selection_iter_is_selected(selection, &tmp_iter)) {
						//Aggiungi alla lista di utenti a cui è rivolto il messaggio
						gtk_tree_model_get(GTK_TREE_MODEL(chat_list_tmp->user_model), &tmp_iter, 1, &id, -1);
						logger(INFO, "[send_text_message] id utente: %lld\n", atoll(id));
						subset = g_list_prepend(subset, (gpointer)(servent_get(atoll(id))));
						g_print("Utente selezionato: %s\n", (servent_get(atoll(id)))->nick);
					}
				valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(chat_list_tmp->user_model), &tmp_iter); 
				}
				logger(INFO, "[send_text_message] sending message to subset; list length %d\n", g_list_length(subset)); 
				if(controller_send_subset_users (chat_id,strlen(msg),msg, subset) >= 0) {
					gui_clear_buffer(GTK_TEXT_VIEW(widget));
					time_t actual_time = time(NULL);
					char *send_msg = prepare_msg(actual_time, servent_get_local()->nick, msg, strlen(msg));
					gui_add_msg_to_chat(chat_id, send_msg);
					gtk_tree_selection_unselect_all(selection);
				}
				return TRUE;
			}
		}
		/***			FINE SUBSET			***/
		else { 
		/*	GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
			g_print("Enter pressed...\n");
			GtkTextIter start ;
			gtk_text_buffer_get_start_iter(buf, &start);
			g_print("Enter pressed...\n");
			GtkTextIter end;
			gtk_text_buffer_get_end_iter(buf, &end);
			g_print("Enter pressed...\n");
			char *msg = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
			logger(INFO, "[gui_send_text_message_event]Msg: %s to %lld\n", msg, chat_id); */
			if(controller_send_chat_users(chat_id, strlen(msg), msg)>=0) {
				gui_clear_buffer(GTK_TEXT_VIEW(widget));
				time_t actual_time = time(NULL);
				char *send_msg = prepare_msg(actual_time, servent_get_local()->nick, msg, strlen(msg));
				gui_add_msg_to_chat(chat_id, send_msg);
						
				//tree_model *model_str = gui_get_tree_model(chat_id);
				//gui_add_message(GTK_TEXT_VIEW(model_str->text_area), msg);
			}
			return TRUE;
		}
	}
	return (FALSE);
}

gint gui_send_pm_message_event(GtkWidget *widget, GdkEventKey *event, gpointer gdata) 
{

	if(event->type == GDK_KEY_PRESS && event->keyval == GDK_Return) {
		g_print("Enter pressed...\n");
		
		u_int8 user_id = atoll((char*)gdata);
		GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
		g_print("Enter pressed...\n");
		GtkTextIter start ;
		gtk_text_buffer_get_start_iter(buf, &start);
		g_print("Enter pressed...\n");
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(buf, &end);
		g_print("Enter pressed...\n");
		char *msg = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
		logger(INFO, "[send_pm_message]Msg: %s to %lld\n", msg, user_id);
		if(controller_send_pm(strlen(msg), msg, user_id)>=0) { 
			gui_clear_buffer(GTK_TEXT_VIEW(widget));
			time_t actual_time = time(NULL);
			char *send_msg = prepare_msg(actual_time, servent_get_local()->nick, msg, strlen(msg));
			gui_add_msg_pm(user_id, send_msg); 
		}
		return TRUE;
	}
	return (FALSE);
}

int gui_add_msg_to_chat(u_int8 chat_id, char *msg) {
	tree_model *model_str = gui_get_tree_model(chat_id);
	if(model_str==NULL)
		return -1;
	gui_add_message(GTK_TEXT_VIEW(model_str->text_area), msg);
	
	return 0;
}

int gui_add_msg_pm(u_int8 sender_id, char *msg) {
	if(msg==NULL) {
		return -1;
	}
	
	pm_data *pm = (pm_data*)g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(sender_id));
	if(pm==NULL) {
		logger(INFO, "[add_msg_pm]PM NULL\n");
		logger(INFO, "[add_msg_pm] strlen nick %d\n",strlen(servent_get(sender_id)->nick));
		gui_open_pm(sender_id, servent_get(sender_id)->nick);
		pm = (pm_data*)g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(sender_id));
		if(pm==NULL) {
			logger(INFO, "[add_msg_pm]PM error\n");
			return -1;
		}
	}
	logger(INFO, "[add_msg_pm]Adding msg\n");
	gui_add_message(pm->text_area, msg);
	logger(INFO, "[add_msg_pm]Added msg\n");
	
	return 0;
}

tree_model *gui_get_tree_model(u_int8 chat_id) {
	return (tree_model*)g_hash_table_lookup(tree_model_hashtable, to_string(chat_id));
}

GtkWidget *gui_create_users_list(u_int8 index )
{

	GtkWidget *scrolled_window;
	GtkWidget *tree_view;
	GtkListStore *model = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	// GtkTreeIter iter;
	GtkCellRenderer *cell;
	//GtkTreeViewColumn *column[3];
	tree_model *model_str = calloc(1, sizeof(tree_model));
	model_str->user_model = model;

	/* Create a new scrolled window, with scrollbars only if needed */
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC, 
			GTK_POLICY_AUTOMATIC);

	// model = gtk_list_store_new (1, G_TYPE_STRING);
	tree_view = gtk_tree_view_new ();
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), 
			tree_view);
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
	gtk_widget_show (tree_view);

	/* Add some messages to the window */
	/*for (i = 0; i < 10; i++) {

        gchar *msg = g_strdup_printf ("User #%d", i);
        gtk_list_store_append (GTK_LIST_STORE (model_str->user_model), &(model_str->user_iter));
        gtk_list_store_set (GTK_LIST_STORE (model_str->user_model), 
	                    &(model_str->user_iter),
                            0, msg,
	                    -1);
	g_free (msg);
    }*/

	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "Nickname", cell, "text", 0, NULL);
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "ID", cell, "text", 1, NULL);
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "Status", cell, "text", 2, NULL);

	GtkTreeSelection *select;
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_MULTIPLE);

	// gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column[0]));
	//gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column[1]));
	//gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column[2]));

	g_signal_connect(G_OBJECT(tree_view),
			"row-activated",
			G_CALLBACK(gui_open_conversation_event),
			to_string(index));  //EVENTO CHE AL DOPPIO CLICK SU UNA CHAT DOVREBBE APRIRE LA GUI DELLA CHAT SELEZIONATA
	if(tree_model_hashtable == NULL) {
		logger(INFO, "[create_users_list] Hashtable created\n");
		tree_model_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	}

	logger(INFO, "[create_users_list]chat ID: %lld\n", index);
	model_str->tree_view = GTK_TREE_VIEW(tree_view); //PROVA
	g_hash_table_insert(tree_model_hashtable, (gpointer)to_string(index), (gpointer)model_str);

	return scrolled_window;
}

GtkWidget *gui_create_chat_list(u_int8 index )
{

	GtkWidget *scrolled_window;
	GtkWidget *tree_view;
	GtkCellRenderer *cell;
	// GtkTreeViewColumn *column[2];

	/* Create a new scrolled window, with scrollbars only if needed */
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC, 
			GTK_POLICY_AUTOMATIC);

	chat_model = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
	tree_view = gtk_tree_view_new ();
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), 
			tree_view);
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (chat_model));
	gtk_widget_show (tree_view);

	/* Add some messages to the window */
	/*for (i = 0; i < 10; i++) {

        gchar *msg = g_strdup_printf ("Chat #%d", i);
        gtk_list_store_append (GTK_LIST_STORE (chat_model), &chat_iter);
        gtk_list_store_set (GTK_LIST_STORE (chat_model), 
	                    &chat_iter,
                            0, msg,
	                    -1);
	g_free (msg);
    }*/

	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "ID", cell, "text", 0, NULL);
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "Title", cell, "text", 1, NULL);
	// GtkTreeSelection *select;
	//   select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	//  gtk_tree_selection_set_mode(select, GTK_SELECTION_MULTIPLE);

	//gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column[0]));
	//gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column[1]));
	//    g_signal_connect(G_OBJECT(tree_view),
	//                     "button_press_event",
	//                     G_CALLBACK(gui_open_chat_button_event),
	//                     NULL);  //EVENTO CHE AL DOPPIO CLICK SU UNA CHAT DOVREBBE APRIRE LA GUI DELLA CHAT SELEZIONATA
	//
	g_signal_connect(tree_view, "row-activated", (GCallback) gui_open_chat_event, NULL);
	
	pm_data_hashtable = g_hash_table_new(g_str_hash, g_str_equal);

	return scrolled_window;
}

GtkWidget *gui_create_menu() {
	GtkWidget *menubar;
	GtkWidget *menuFile;
	GtkWidget *menuEdit;
	GtkWidget *menuHelp;
	GtkWidget *menuitem;
	GtkWidget *menu;
	/*-- Create the menu bar --*/
	menubar = gtk_menu_bar_new();
	/*---------------- Create File menu items ------------------*/

	menuFile = gtk_menu_item_new_with_label ("File");
	gtk_menu_bar_append (GTK_MENU_BAR(menubar), menuFile);
	gtk_widget_show(menuFile);

	/*-- Create File submenu  --*/
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuFile), menu);

	/*-- Create New menu item under File submenu --*/
	menuitem = gtk_menu_item_new_with_label ("New");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_widget_show (menuitem);

	/*-- Create Open menu item under File submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Open");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_widget_show (menuitem);

	/*-- Create Exit menu item under File submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Exit");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gui_close_app_window_event), NULL);
	gtk_widget_show (menuitem);
	/*---------------- End File menu declarations ----------------*/

	/*---------------- Create Edit menu items --------------------*/

	menuEdit = gtk_menu_item_new_with_label ("Stato");
	gtk_menu_bar_append (GTK_MENU_BAR(menubar), menuEdit);
	gtk_widget_show(menuEdit);

	/*-- Create File submenu --*/
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuEdit), menu);

	/*-- Create Undo menu item under Edit submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Online");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gui_set_to_online_event), NULL);
	gtk_widget_show (menuitem);

	/*-- Create Copy menu item under File submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Busy");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gui_set_to_busy_event), NULL);
	gtk_widget_show (menuitem);

	/*-- Create Cut menu item under File submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Away");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gui_set_to_away_event), NULL);
	gtk_widget_show (menuitem);
	/*---------------- End Edit menu declarations ----------------*/

	/*---------------- Start Help menu declarations ----------------*/
	menuHelp = gtk_menu_item_new_with_label ("Help");
	gtk_menu_bar_append (GTK_MENU_BAR(menubar), menuHelp);
	gtk_widget_show(menuHelp);

	/*-- Create Help submenu --*/
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuHelp), menu);

	/*-- Create About menu item under Help submenu --*/
	menuitem = gtk_menu_item_new_with_label ("About");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gui_open_about_event), NULL);
	gtk_widget_show (menuitem);
	/*---------------- End Help menu declarations ----------------*/
	return menubar;
}

GtkWidget *gui_create_text(u_int8 chat_id, int type, int msg_type)
{
	logger(INFO, "[create_text]chat ID: %lld\n", chat_id);
	GtkWidget *scrolled_window;
	GtkWidget *view = gtk_text_view_new ();
	GtkTextBuffer *buffer;

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	// gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window),gtk_adjustment_new(0,0,1,1,1)); //NON FUNZIONA
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC,
			GTK_POLICY_AUTOMATIC);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), TRUE);
	//gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), view);
	gtk_container_add (GTK_CONTAINER (scrolled_window), view);
	/*GtkAdjustment *vadj;
	vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
	gtk_adjustment_set_value(vadj, vadj->lower);*/
	//insert_text (buffer);
	if(type == BOTTOM) {
		gtk_text_view_set_editable(GTK_TEXT_VIEW(view),TRUE);
		logger(INFO, "[create_text]signal connect\n");
		if(msg_type == PM) {
			g_signal_connect(GTK_OBJECT(view),"key_press_event", G_CALLBACK(gui_send_pm_message_event), (gpointer)to_string(chat_id)); 
		}
		else if(msg_type == CHAT) {	
			g_signal_connect(GTK_OBJECT(view),"key_press_event", G_CALLBACK(gui_send_text_message_event), (gpointer)to_string(chat_id)); 
		}
		gtk_text_view_set_editable(GTK_TEXT_VIEW(view),TRUE);
		logger(INFO, "[create_text]signal connected\n");
	}
	else if (type == TOP){
		if(msg_type == CHAT) {
			gtk_text_view_set_editable(GTK_TEXT_VIEW(view),FALSE);
			gtk_text_buffer_create_tag(buffer, "blue_fg", "foreground", "blue", NULL);
			tree_model *model_str = gui_get_tree_model(chat_id);
			if(model_str!=NULL) {
				logger(INFO, "[create_text]OK\n");
				model_str->text_area = GTK_TEXT_VIEW(view);
			}
			else {
				logger(INFO, "[create_text]NULL\n");
				return NULL;
			}
		}
		else if(msg_type == PM) {
			pm_data *pm = (pm_data*)g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(chat_id));
			logger(INFO, "[create_text]Checking\n");
			if(pm==NULL) {
				pm = calloc(1, sizeof(pm_data));
				pm->text_area = GTK_TEXT_VIEW(view);
				gtk_text_view_set_editable(GTK_TEXT_VIEW(pm->text_area),FALSE);
				gtk_text_buffer_create_tag(buffer, "blue_fg", "foreground", "blue", NULL);
				logger(INFO, "[create_text]Allocating\n");
				g_hash_table_insert(pm_data_hashtable, (gpointer)to_string(chat_id), (gpointer)pm);
				logger(INFO, "[create_text]Allocated\n");
			}
		}
	}	
	gtk_widget_show_all (scrolled_window);

	return scrolled_window;
}

int gui_open_chatroom(u_int8 chat_id) {
	/*-- Declare the GTK Widgets used in the program --*/
	GtkWidget *window;
	GtkWidget *menu;
	GtkWidget *vbox;
	GtkWidget *hbox; //
	GtkWidget *vbox2;
	GtkWidget *handlebox;  
	GtkWidget *text;
	GtkWidget *list;
	GtkWidget *chat;

	/*-- Create the new window --*/
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	menu = gui_create_menu();
	list = gui_create_users_list (chat_id);

	text = gui_create_text(chat_id, TOP, CHAT);
	chat = gui_create_text(chat_id, BOTTOM, CHAT);

	/*-- Create the vbox --*/
	vbox = gtk_vbox_new(FALSE, 5);

	hbox = gtk_hbox_new(FALSE, 5);
	vbox2 = gtk_vbox_new(FALSE, 5);
	/*-- Create a text area --*/

	/*-- Create the handlebox --*/
	handlebox = gtk_handle_box_new();

	logger(INFO,"[open_chatroom_gui] chat_id %lld\n", chat_id);
	/*-- Connect the window to the gui_close_event function  --*/


	/*-- Add the menubar to the handlebox --*/
	gtk_container_add(GTK_CONTAINER(handlebox), menu);

	/*-- Pack the handlebox into the top of the vbox.  
	 *-- You must use gtk_box_pack_start here instead of gtk_container_add
	 *-- or the menu will get larger as the window is enlarged
	 */ 

	gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(vbox),hbox);

	gtk_container_add(GTK_CONTAINER(hbox),vbox2);
	gtk_container_add (GTK_CONTAINER (hbox), list);
	gtk_container_add(GTK_CONTAINER(vbox2),text);
	gtk_box_pack_end(GTK_BOX(vbox2), chat, FALSE, TRUE, 0);
	/*-- Add the vbox to the main window --*/
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/*-- Add some text to the window --*/
	//  gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, buffer, strlen(buffer));

	/*-- Set window border to zero so that text area takes up the whole window --*/
	gtk_container_border_width (GTK_CONTAINER (window), 0);

	/*-- Set the window to be 640 x 480 pixels --*/
	gtk_window_set_default_size (GTK_WINDOW(window), 640, 400);

	/*-- Set the window title --*/
	gtk_window_set_title(GTK_WINDOW (window), "Chat Room");
	logger(INFO, "[open_chatroom_gui]chat_id created: %lld\n", chat_id);

	/*-- Display the widgets --*/
	gtk_widget_show(handlebox);
	gtk_widget_show(hbox);
	gtk_widget_show(vbox);
	gtk_widget_show(vbox2);
	gtk_widget_show(text);
	gtk_widget_show(list);
	gtk_widget_show(chat);
	gtk_widget_show(menu);
	gtk_widget_show(window);

	/*-- Start the GTK event loop --*/

	logger(INFO, "[open gui] to string %s\n", to_string(chat_id));
	g_signal_connect(GTK_OBJECT(window), "destroy", G_CALLBACK(gui_leave_chat_event), (gpointer)to_string(chat_id));
	//gtk_main();
	/*-- Return 0 if exit is successful --*/
	return 0;
}

int gui_open_pm(u_int8 user_id, gchar *nickname) {
	/*-- Declare the GTK Widgets used in the program --*/
	GtkWidget *window;
	GtkWidget *menu;
	GtkWidget *vbox;

	GtkWidget *vbox2;
	GtkWidget *handlebox;  
	GtkWidget *text;
	GtkWidget *chat;

	/*-- Create the new window --*/
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	/*-- Create the vbox --*/
	vbox = gtk_vbox_new(FALSE, 5);


	vbox2 = gtk_vbox_new(FALSE, 5);
	//gtk_paned_set_position(GTK_PANED(vbox2), 500);  //TROVARE UN MODO CORRETTO DI SETTARE IL SECONDO PARAMETRO PER POSIZIONARE BENE IL SEPARATORE
	/*-- Create a text area --*/
	text = gui_create_text(user_id, TOP, PM);
	chat = gui_create_text(user_id, BOTTOM, PM); //Naturalmente non si vede nella window xkè l'id della chat non è 1
	logger(INFO, "[open_pm_gui]Here1\n");

	/*-- Create the handlebox --*/
	handlebox = gtk_handle_box_new();
	logger(INFO, "[open_pm_gui]Here2\n");
	/*-- Create the menu bar --*/
	menu = gui_create_menu();
	logger(INFO, "[open_pm_gui]Here3\n");
	/*-- Add the menubar to the handlebox --*/
	gtk_container_add(GTK_CONTAINER(handlebox), menu);
	logger(INFO, "[open_pm_gui]Here4\n");
	/*-- Pack the handlebox into the top of the vbox.  
	 *-- You must use gtk_box_pack_start here instead of gtk_container_add
	 *-- or the menu will get larger as the window is enlarged
	 */

	//gtk_box_pack_start(GTK_BOX(hbox), handlebox, FALSE, TRUE, 0);    

	gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 0);    
	gtk_container_add(GTK_CONTAINER(vbox),vbox2);
	logger(INFO, "[open_pm_gui]Here5\n");
	//gtk_paned_add1(GTK_PANED(vbox2), chat);
	//gtk_paned_add2(GTK_PANED(vbox2), text);
	gtk_container_add(GTK_CONTAINER(vbox2),text);
	gtk_box_pack_end(GTK_BOX(vbox2), chat, FALSE, TRUE, 0);
	logger(INFO, "[open_pm_gui]Here6\n");
	gtk_container_add(GTK_CONTAINER(window), vbox);
	logger(INFO, "[open_pm_gui]Here7\n");
	/*-- Add some text to the window --*/
	//  gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, buffer, strlen(buffer));

	/*-- Set window border to zero so that text area takes up the whole window --*/
	gtk_container_border_width (GTK_CONTAINER (window), 0);
	logger(INFO, "[open_pm_gui]Here8\n");
	/*-- Set the window to be 640 x 480 pixels --*/
	gtk_window_set_default_size (GTK_WINDOW(window), 640, 400);
	logger(INFO, "[open_pm_gui]Here9\n");
	/*-- Set the window title --*/
	gtk_window_set_title(GTK_WINDOW (window), nickname); 
	//logger(INFO, "[gui_open_pm]Here10\n");
	/*-- Display the widgets --*/
	gtk_widget_show(handlebox);
	gtk_widget_show(vbox);
	gtk_widget_show(vbox2);
	gtk_widget_show(text);
	//logger(INFO, "[gui_open_pm]Here30\n");
	gtk_widget_show (chat);
	gtk_widget_show(menu);      
	gtk_widget_show(window);

	logger(INFO, "[open_pm_gui]Here\n");
	/*-- Connect the window to the gui_close_event function  --*/
	g_signal_connect(GTK_OBJECT(window), "destroy", G_CALLBACK(gui_leave_pm_event), (gpointer)to_string(user_id));

	pm_data *pm = (pm_data*)g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(user_id));
	logger(INFO, "[open_pm_gui]Checking\n");
	if(pm!=NULL) {
		pm->window = GTK_WIDGET(window);
	}
	/*-- Start the GTK event loop --*/
	//gtk_main();

	/*-- Return 0 if exit is successful --*/
	return 0; 
}

GtkWidget *gui_create_searchbar(void) {
	GtkWidget *bar_container = gtk_hbox_new(FALSE, 5);

	bar_textfield = gtk_entry_new();
	GtkWidget *bar_button = gtk_button_new();
	GtkWidget *bar_create_button = gtk_button_new();

	gtk_entry_set_width_chars(GTK_ENTRY(bar_textfield), (gint)40);
	gtk_button_set_label(GTK_BUTTON(bar_button), (const gchar*)"Search");
	g_signal_connect (G_OBJECT (bar_button), "clicked",
			G_CALLBACK (gui_search_chat_event), NULL);

	gtk_button_set_label(GTK_BUTTON(bar_create_button), (const gchar*)"Create");
	g_signal_connect(G_OBJECT(bar_create_button), "clicked", G_CALLBACK(gui_create_chat_button), NULL);

	gtk_container_add(GTK_CONTAINER(bar_container), bar_textfield);
	//gtk_box_pack_start(GTK_BOX(bar_container), bar_textfield, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(bar_container), bar_button, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(bar_container), bar_create_button, FALSE, TRUE, 5);

	gtk_widget_show(bar_create_button);
	gtk_widget_show(bar_textfield);
	gtk_widget_show(bar_button);

	return bar_container;
}

pm_data *gui_pm_data_get(u_int8 id) {
	if(pm_data_hashtable!=NULL)
		return g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(id));
	else
		return NULL;
}
