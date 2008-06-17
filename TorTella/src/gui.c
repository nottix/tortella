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

/*
 * Provoca la semplice chiusura di una finestra
 */
gint gui_close_window_event(GtkWidget *widget, gpointer gdata) {
	gtk_widget_destroy(widget);
	return(FALSE);
}

/*
 * Consente l'uscita corretta dal programma, invia un messaggio di BYE e effettua
 * il leave da ogni chat a cui si è connessi, infine chiama la controller_exit e 
 * termina l'interfaccia grafica.
 */
void gui_close_event (GtkWidget *widget, gpointer gdata)
{
	controller_leave_all_chat();
	controller_send_bye();
	controller_exit();
	gtk_main_quit();
}

/*
 * Invocata quando viene chiusa la chat, effettua la chiamata al controller per 
 * fargli gestire l'invio dei pacchetti di tipo LEAVE
 */
gint gui_leave_chat_event(GtkWidget *widget, gpointer gdata)
{
	char *str = (char*)gdata;
	u_int8 val = atoll((char*)gdata);
	controller_leave_flooding(val);
	return(FALSE);
}

/*
 * Chiude la finestra relativa ad una conversazione privata e la rimuove dalla
 * hashtable.
 */
gint gui_leave_pm_event(GtkWidget *widget, gpointer gdata)
{
	u_int8 val = atoll((char*)gdata);
	pm_data *pm;
	if((pm = g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(val)))!=NULL) {
		gtk_widget_destroy(GTK_WIDGET(pm->window)); 
		g_hash_table_remove(pm_data_hashtable, (gconstpointer)to_string(val));
	}
	return (FALSE);
}

/*
 * Aggiunge una chat alla lista delle chat in seguito ad una ricerca che ha prodotto
 * risultati validi
 */
gint gui_add_chat(u_int8 chat_id, char *chat_name)
{
	GtkTreeIter iter;
	gboolean valid = TRUE;
	gchar *id;
	//scorre la lista delle chat visualizzate
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
	
	//prepara i campi da inserire nel nuovo record
	gchar *msg = g_strdup_printf (to_string(chat_id));
	gchar *msg1 = g_strdup_printf (chat_name);
	//inserimento del record
	gtk_list_store_append (chat_model, &chat_iter);
	gtk_list_store_set (GTK_LIST_STORE(chat_model), &chat_iter, 0, msg, 1, msg1, -1);
	g_free (msg);
	g_free (msg1);
	return 0;
}

/*
 * Aggiunge una utente alla chat, effettuando un controllo su eventuali duplicati 
 * in modo da poterli scartare.
 */
gint gui_add_user_to_chat(u_int8 chat_id, u_int8 id, char *user, u_int1 status)
{
       logger(INFO, "[gui_add_user_to_chat] Adding user %lld\n", id);
       gchar *msg = g_strdup_printf(user);
       if(tree_model_hashtable==NULL) {
               logger(INFO, "[add_user_to_chat_list]User list not ready\n");
               return -1;
       }
       tree_model *mod = (tree_model*)g_hash_table_lookup(tree_model_hashtable,(gconstpointer)to_string(chat_id));

       if(mod!=NULL) {
               GtkTreeIter iter, iter2;
               gchar *id_tmp;
               gboolean valid = TRUE;
               gboolean valid2 = TRUE;
		   	   //controlla che sia presente almeno un utente
               if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mod->user_model), &iter)==TRUE) {                      
                       gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mod->user_model), &iter2);
                       valid2 = gtk_tree_model_iter_next(GTK_TREE_MODEL(mod->user_model), &iter2);
                       //ciclo su tutti i record della lista
				   		while(valid) {
                               gtk_tree_model_get(GTK_TREE_MODEL(mod->user_model), &iter, 1, &id_tmp, -1);
                               logger(INFO, "[gui_add_user_to_chat]ID: %s\n", id_tmp);
                               if(atoll(id_tmp) == id) {
                                       logger(INFO, "[gui_add_user_to_chat] existing user\n");
                                       break;
                               }
							   //controllo che l'id non sia già presente
                               if(atoll(id_tmp)!= id && valid2 == FALSE ) {
                                       
								   	   logger(INFO, "[add_user_to_chat_list]User list OK\n");
                                       //inserimento del nuovo record
								   	   gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
                                       gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 0, msg, -1);
                                       g_free(msg);

                                       msg = g_strdup_printf(to_string(id));
                                       gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 1, msg, -1);
                                       g_free(msg);

                                       if(status == ONLINE_ID)
                                               msg = g_strdup_printf(ONLINE);
                                       else if(status == BUSY_ID)
                                               msg = g_strdup_printf(BUSY);
                                       else if(status == AWAY_ID)
                                               msg = g_strdup_printf(AWAY);
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
				   	   //nel caso non siano presenti altri utenti l'iteratore non è valido, quindi si inserisce il primo record
                       logger(INFO, "inserting first user\n");
                       gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
                                       gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 0, msg, -1);
                                       g_free(msg);

                                       msg = g_strdup_printf(to_string(id));
                                       gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 1, msg, -1);
                                       g_free(msg);
                                       if(status == ONLINE_ID)
                                               msg = g_strdup_printf(ONLINE);
                                       else if(status == BUSY_ID)
                                               msg = g_strdup_printf(BUSY);
                                       else if(status == AWAY_ID)
                                               msg = g_strdup_printf(AWAY);
                                       gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 2, msg, -1);
                                       g_free(msg);
               }
               
       }
       else
               return -2;


       return 0;
}

/*
 * Scorre la lista degli utenti connessi fino a quando non viene trovato l'id 
 * dell'utente da eliminare, dopo di che questo viene eliminato
 */
gint gui_del_user_from_chat(u_int8 chat_id, u_int8 user_id)
{
	tree_model *mod = (tree_model*)g_hash_table_lookup(tree_model_hashtable,(gconstpointer)to_string(chat_id));
	if(mod==NULL) {
		logger(INFO, "[gui_del_user_from_chat]Not connected to chad %lld\n", chat_id);
		return -1;
	}
	GtkTreeIter iter;
	gboolean valid = TRUE;
	gchar *id;
	//posizionamento dell'iteratore al primo record e successivo scorrimento della lista
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mod->user_model), &iter)==TRUE) {
		while(valid) {

			logger(INFO, "[remove_user_from_chat_list]start\n");
			gtk_tree_model_get(GTK_TREE_MODEL(mod->user_model), &iter, 1, &id, -1);
			logger(INFO, "[remove_user_from_chat_list]ID to remove: %s\n", id);
			
			//se l'utente è presente lo si rimuove dalla lista
			if(atoll(id)==user_id) {
				logger(INFO, "[remove_user_from_chat_list]Removing: %s\n", id);
				gtk_list_store_remove(mod->user_model, &iter);
				logger(INFO, "[remove_user_from_list]Removed\n");
				return TRUE;
			}

			valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(mod->user_model), &iter);
			logger(INFO, "[remove_user_from_chat_list]next user\n");

		}
	}
	return (FALSE);
}

/*
 * Scorre la lista degli utenti connessi fino a quando non viene trovato il record
 * da aggiornare, dopo di che viene aggiornato il campo status.
 */
gint gui_change_status(u_int8 user_id, char *status) 
{
	if(tree_model_hashtable == NULL) {
		logger(INFO, "[gui_change_status] user list not ready\n");
		return (FALSE);
	}
	GList *chat_id_list = g_hash_table_get_keys(tree_model_hashtable);
	int i=0;
	char *id;
	for(; i < g_list_length(chat_id_list); i++) {
		u_int8 chat_id = atoll(g_list_nth_data(chat_id_list,i));
		logger(INFO, "[gui_change_status] retrieving chat_id %lld\n",chat_id);

		tree_model *chat_model_tmp = (tree_model*)g_hash_table_lookup(tree_model_hashtable, (gconstpointer)to_string(chat_id));
		GtkTreeIter iter;
		gboolean valid = TRUE;
		
		//posizionamento dell'iteratore al primo record e successivo scorrimento della lista
		if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(chat_model_tmp->user_model), &iter)==TRUE) {			

			while(valid) {
				gtk_tree_model_get(GTK_TREE_MODEL(chat_model_tmp->user_model), &iter, 1, &id, -1);
				logger(INFO, "[gui_change_status]ID: %s\n", id);
				
				//trovato l'utente si aggiorna il campo status
				if(atoll(id)==user_id) {
					gtk_list_store_set(GTK_LIST_STORE(chat_model_tmp->user_model), &iter, 2, status, -1);
					logger(INFO, "[gui_change_status] changed status %s\n", status);
					
				}

				valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(chat_model_tmp->user_model), &iter); 
			}
		}		
	}
	return (FALSE);
}

/*
 * Effettua un clear della lista delle chat
 */
gint gui_clear_chat_list()
{
	gtk_list_store_clear(chat_model);
	return (FALSE);
}

/*
 * Effettua un clear della text view in cui vengono scritti i messaggi da inviare.
 * Viene invocata ogni volta che viene inviato un messaggio.
 */
gint gui_clear_buffer(GtkTextView *widget)
{
	GtkTextBuffer *text;
	text = gtk_text_buffer_new(NULL);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(widget),text);
	return (FALSE);
}

/*
 * Aggiunge alla chat il messaggio appena inviato e aggiorna lo scrolling della
 * finestra in modo che l'ultimo messaggio sia ben visibile.
 */
gint gui_add_message(GtkTextView *widget, gchar *msg)
{
	GtkTextBuffer *text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
	GtkTextIter iter;
	gtk_text_buffer_get_end_iter(text,&iter);
	char *first = strstr(msg, "\n");
	int len = first-msg;
	char *tmp = calloc(len+1, 1);
	strncpy(tmp, msg, len);  
	/*inserisce data e nick dell'utente nel buffer, viene colorato di blue per differenziarlo
	 *dal messaggio vero e proprio.
	 */
	gtk_text_buffer_insert_with_tags_by_name(text, &iter, tmp, -1, "blue_fg", NULL);
	msg+=len;
	gtk_text_buffer_get_end_iter(text,&iter);
	//inserimento del messaggio
	gtk_text_buffer_insert(text,&iter,msg,-1); 

	// Scrolling della finestra
	GtkTextIter new_iter;
	text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
	gtk_text_buffer_get_end_iter(text, &new_iter);
	GtkTextMark *mark = gtk_text_mark_new(NULL,FALSE);
	gtk_text_buffer_add_mark(text,mark,&new_iter);
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(widget), mark, 0.0, FALSE, 0 ,0);
	return (FALSE);
}

/*
 * Funzione chiamata alla selezione di una chat, chiama il controller per gestire
 * la connessione agli utenti della chat e l'invio della JOIN, inoltre lancia la 
 * finestra relativa alla chat.
 */
void gui_open_chat_event (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer userdata) {
	GtkTreeModel *model;
	GtkTreeIter   iter;


	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gchar *name;

		gtk_tree_model_get(model, &iter, 0, &name, -1);
		chat *elem = data_get_chat(atoll(name));
		if(elem!=NULL) {

			controller_connect_users(g_hash_table_get_values(elem->users));
			
			gui_open_chatroom(elem->id);

			controller_join_flooding(elem->id);
		}
		g_free(name);
	}
}

/*
 * Evento che si scatena quando si effettua un doppio click su un utente della chat. 
 * Permette di aprire una nuova finestra per una conversazione privata.
 */
gint gui_open_conversation_event(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer userdata) {

	GtkTreeModel *model;
	GtkTreeIter   iter;

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gchar *user_id;
		gchar *name; 
		gtk_tree_model_get(model, &iter, 0, &name, -1);
		gtk_tree_model_get(model, &iter, 1, &user_id, -1);

		if(user_id > 0 ) {
			logger(INFO, "[open_conversation] nick length %d\n", strlen(name));
			if(g_hash_table_lookup(pm_data_hashtable, (gconstpointer)user_id) != NULL) {
				logger(INFO, "[open_conversation] conversation already open\n");
				return (FALSE);
			}	
			gui_open_pm(atoll(user_id),name);
			
		}
	}
	return FALSE;
}

/*
 * Evento che permette di aprire la finestra relativa all'about.
 */
gint gui_open_about_event(GtkWidget *widget, gpointer gdata)
{
	GtkWidget *window;
	GtkWidget *label1;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect (window, "delete_event",
			G_CALLBACK (gui_close_window_event), NULL);

	gtk_container_set_border_width (GTK_CONTAINER (window), 25);

	label1 = gtk_label_new("\tTorTella Chat\nCreated by TorTella Team\n");
	gtk_container_add (GTK_CONTAINER (window), label1);
	gtk_widget_show(label1); 
	gtk_widget_show(window);
	gtk_main();
	return 0;
}

/*
 * Setta lo stato dell'utente su Online e invoca il controller_change_status
 * in modo che quest'ultimo possa inviare un pacchetto di PING con il nuovo status
 */
gint gui_set_to_online_event(GtkWidget *widget, gpointer gdata)
{
	controller_change_status (ONLINE_ID);
	gui_change_status(servent_get_local()->id, ONLINE);
	return(FALSE);
}

/*
 * Setta lo stato dell'utente su Busy e invoca il controller_change_status
 * in modo che quest'ultimo possa inviare un pacchetto di PING con il nuovo status
 */
gint gui_set_to_busy_event(GtkWidget *widget, gpointer gdata)
{
	controller_change_status(BUSY_ID);
	gui_change_status(servent_get_local()->id, BUSY);
	return (FALSE);
}

/*
 * Setta lo stato dell'utente su Away e invoca il controller_change_status
 * in modo che quest'ultimo possa inviare un pacchetto di PING con il nuovo status
 */
gint gui_set_to_away_event(GtkWidget *widget, gpointer gdata)
{
	controller_change_status(AWAY_ID);
	gui_change_status(servent_get_local()->id, AWAY);	
	return (FALSE);
}

/*
 * Evento che si scatena alla pressione del bottone search, effettua un clear della
 * lista delle chat e invoca il controller_search in modo che quest'ultimo possa 
 * inviare il pacchetto di SEARCH. 
 */
gint gui_search_chat_event(GtkWidget *widget, gpointer gdata)
{

	gui_clear_chat_list();
	controller_search(gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
	return(FALSE);
}

/*
 * Evento che si scatena alla pressione del bottone create, fà una semplice 
 * chiamata alla controller_create.
 */
gint gui_create_chat_button(GtkWidget *widget, gpointer gdata) {
	return controller_create(gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
}

/*
 * Prepara il corpo del messaggio da inviare a tutta la chat o a un sottoinsieme di
 * questa, dopo di che invoca il controller affinchè questo gestisca l'invio del 
 * messaggio e infine aggiorna la text view locale col nuovo messaggio.
 */
gint gui_send_text_message_event(GtkWidget *widget, GdkEventKey *event, gpointer gdata)
{
	//se è stato premuto il tasto invio, si sta inviando un messaggio
	if(event->type == GDK_KEY_PRESS && event->keyval == GDK_Return) {
		GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
		GtkTextIter start ;
		gtk_text_buffer_get_start_iter(buf, &start);
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(buf, &end);
		//testo del messaggio contenuto nella text view
		char *msg = gtk_text_buffer_get_text(buf, &start, &end, TRUE);		
		GtkTreeSelection *selection;

		u_int8 chat_id = atoll((char*)gdata);
		logger(INFO, "[send_text_message] chat id = %lld\n",chat_id);
		/* Invio del messaggio ad un sottoinsieme di utenti*/
		tree_model *chat_list_tmp = gui_get_tree_model(chat_id);
		
		//elenco degli utenti selezionati
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(chat_list_tmp->tree_view));
		if(gtk_tree_selection_count_selected_rows(selection) > 0) {
			GList *subset = NULL;		
			logger(INFO, "[send_text_message] send to subset\n");
			GtkTreeIter tmp_iter;
			gboolean valid = TRUE;
			char *id;
			//si setta l'iteratore al primo record della lista
			if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(chat_list_tmp->user_model), &tmp_iter)==TRUE) {
				//si scorre la lista degli utenti della chat e si confronta con gli utenti selezionati
				while(valid) {
					if(gtk_tree_selection_iter_is_selected(selection, &tmp_iter)) {
						//Aggiungi alla lista di utenti a cui è rivolto il messaggio
						gtk_tree_model_get(GTK_TREE_MODEL(chat_list_tmp->user_model), &tmp_iter, 1, &id, -1);
						logger(INFO, "[send_text_message] user id: %lld\n", atoll(id));
						subset = g_list_prepend(subset, (gpointer)(servent_get(atoll(id))));
					}
					valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(chat_list_tmp->user_model), &tmp_iter); 
				}
				logger(INFO, "[send_text_message] sending message to subset; list length %d\n", g_list_length(subset)); 
				//invio del messaggio tramite il controller
				if(controller_send_subset_users (chat_id,strlen(msg),msg, subset) >= 0) {
					//clear del buffer e preparazione del messaggio da aggiungere sulla gui
					gui_clear_buffer(GTK_TEXT_VIEW(widget));
					time_t actual_time = time(NULL);
					char *send_msg = prepare_msg(actual_time, servent_get_local()->nick, msg, strlen(msg));
					gui_add_msg_to_chat(chat_id, send_msg);
					gtk_tree_selection_unselect_all(selection);
				}
				return TRUE;
			}
		}
		/* Fine dell'invio ad un sottoinsieme di utenti */
		else { 
			//invio del messaggio tramite il controller
			if(controller_send_chat_users(chat_id, strlen(msg), msg)>=0) {
				//clear del buffer e preparazione del messaggio da aggiungere sulla gui
				gui_clear_buffer(GTK_TEXT_VIEW(widget));
				time_t actual_time = time(NULL);
				char *send_msg = prepare_msg(actual_time, servent_get_local()->nick, msg, strlen(msg));
				gui_add_msg_to_chat(chat_id, send_msg);
			}
			return TRUE;
		}
	}
	return (FALSE);
}

/*
 * Come la send_text_message_event, ma rivolta all'invio di messaggi privati
 */
gint gui_send_pm_message_event(GtkWidget *widget, GdkEventKey *event, gpointer gdata) 
{
	//se è stato premuto il tasto invio, si sta inviando un messaggio
	if(event->type == GDK_KEY_PRESS && event->keyval == GDK_Return) {

		u_int8 user_id = atoll((char*)gdata);
		GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
		GtkTextIter start ;
		gtk_text_buffer_get_start_iter(buf, &start);
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(buf, &end);
		//testo del messaggio contenuto nella text view
		char *msg = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
		logger(INFO, "[send_pm_message]Msg: %s to %lld\n", msg, user_id);
		//invio del messaggio tramite il controller
		if(controller_send_pm(strlen(msg), msg, user_id)>=0) { 
			//clear del buffer e preparazione del messaggio da aggiungere sulla gui
			gui_clear_buffer(GTK_TEXT_VIEW(widget));
			time_t actual_time = time(NULL);
			char *send_msg = prepare_msg(actual_time, servent_get_local()->nick, msg, strlen(msg));
			gui_add_msg_pm(user_id, send_msg); 
		}
		return TRUE;
	}
	return (FALSE);
}

/*
 * Aggiunge alla text view della chat il nuovo messaggio.
 */
int gui_add_msg_to_chat(u_int8 chat_id, char *msg) {
	tree_model *model_str = gui_get_tree_model(chat_id);
	if(model_str==NULL)
		return -1;
	gui_add_message(GTK_TEXT_VIEW(model_str->text_area), msg);

	return 0;
}

/*
 * Aggiunge alla text view della conversazione privata il nuovo messaggio.
 */
int gui_add_msg_pm(u_int8 sender_id, char *msg) {
	if(msg==NULL) {
		return -1;
	}

	pm_data *pm = (pm_data*)g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(sender_id));
	if(pm==NULL) {
		logger(INFO, "[add_msg_pm]PM window not ready\n");
		logger(INFO, "[add_msg_pm] strlen nick %d\n",strlen(servent_get(sender_id)->nick));
		//apertura della nuova conversazione privata nel caso questa non sia già presente
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

/*
 * Controlla nell'hashtable la presenza o meno del tree_model corrispondente alla 
 * chat.
 */
tree_model *gui_get_tree_model(u_int8 chat_id) {
	if(tree_model_hashtable == NULL) {	//PROVA
		return NULL;
	}
	return (tree_model*)g_hash_table_lookup(tree_model_hashtable, to_string(chat_id));
}

/*
 * Crea il componente grafico che contiene la lista degli utenti connessi ad una chat.
 */
GtkWidget *gui_create_users_list(u_int8 index )
{

	GtkWidget *scrolled_window;
	GtkWidget *tree_view;
	GtkListStore *model = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	GtkCellRenderer *cell;
	tree_model *model_str = calloc(1, sizeof(tree_model));
	model_str->user_model = model;

	// crea una nuova scrolled window con lo scrolling abilitato solo se necessario 
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC, 
			GTK_POLICY_AUTOMATIC);
	
	// Definizione della tree view e dei sotto componenti
	tree_view = gtk_tree_view_new ();
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), 
			tree_view);
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
	gtk_widget_show (tree_view);
	
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "Nickname", cell, "text", 0, NULL);
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "ID", cell, "text", 1, NULL);
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "Status", cell, "text", 2, NULL);

	//modalità di selezione dei record della lista
	GtkTreeSelection *select;
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_MULTIPLE);
	
	//evento che si scatena al doppio click su un record della lista
	g_signal_connect(G_OBJECT(tree_view),
			"row-activated",
			G_CALLBACK(gui_open_conversation_event),
			to_string(index));  
	if(tree_model_hashtable == NULL) {
		logger(INFO, "[create_users_list] created user list\n");
		tree_model_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	}

	logger(INFO, "[create_users_list]chat ID: %lld\n", index);
	model_str->tree_view = GTK_TREE_VIEW(tree_view); 
	g_hash_table_insert(tree_model_hashtable, (gpointer)to_string(index), (gpointer)model_str);

	return scrolled_window;
}

/*
 * Crea il componente grafico che contiene la lista delle chat trovate.
 */ 
GtkWidget *gui_create_chat_list(u_int8 index )
{

	GtkWidget *scrolled_window;
	GtkWidget *tree_view;
	GtkCellRenderer *cell;
	
	// crea una nuova scrolled window con lo scrolling abilitato solo se necessario 
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC, 
			GTK_POLICY_AUTOMATIC);

	// Definizione della tree view e dei sotto componenti
	chat_model = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
	tree_view = gtk_tree_view_new ();
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), 
			tree_view);
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (chat_model));
	gtk_widget_show (tree_view);

	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "ID", cell, "text", 0, NULL);
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "Title", cell, "text", 1, NULL);

	//evento che si scatena al doppio click su un record della lista
	g_signal_connect(tree_view, "row-activated", (GCallback) gui_open_chat_event, NULL);

	pm_data_hashtable = g_hash_table_new(g_str_hash, g_str_equal);

	return scrolled_window;
}

/*
 * Crea la barra dei menu situata su ogni finestra. (File -- Status -- Help).
 */
GtkWidget *gui_create_menu() {
	GtkWidget *menubar;
	GtkWidget *menuFile;
	GtkWidget *menuEdit;
	GtkWidget *menuHelp;
	GtkWidget *menuitem;
	GtkWidget *menu;
	/*-- Crea la menu bar --*/
	menubar = gtk_menu_bar_new();
	/*---------------- Crea File menu items ------------------*/

	menuFile = gtk_menu_item_new_with_label ("File");
	gtk_menu_bar_append (GTK_MENU_BAR(menubar), menuFile);
	gtk_widget_show(menuFile);

	/*-- Crea File submenu  --*/
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuFile), menu);

	/*-- Crea un NEW menu item da collocare nel File submenu --*/
	menuitem = gtk_menu_item_new_with_label ("New");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_widget_show (menuitem);

	/*-- Crea un OPEN menu item da collocare nel File submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Open");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_widget_show (menuitem);

	/*-- Crea un Exit menu item da collocare nel File submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Exit");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gui_close_event), NULL);
	gtk_widget_show (menuitem);
	/*---------------- Fine dichiarazione File menu ----------------*/

	/*---------------- Crea Edit menu items --------------------*/

	menuEdit = gtk_menu_item_new_with_label ("Stato");
	gtk_menu_bar_append (GTK_MENU_BAR(menubar), menuEdit);
	gtk_widget_show(menuEdit);

	/*-- Crea submenu --*/
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuEdit), menu);

	/*-- Crea Online menu item da collocare in Stato submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Online");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gui_set_to_online_event), NULL);
	gtk_widget_show (menuitem);

	/*-- Crea Busy menu item da collocare in Stato submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Busy");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gui_set_to_busy_event), NULL);
	gtk_widget_show (menuitem);

	/*-- Crea Away menu item da collocare in Stato submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Away");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gui_set_to_away_event), NULL);
	gtk_widget_show (menuitem);
	/*---------------- Fine dichiarazione Edit menu ----------------*/

	/*---------------- Start Help menu  ----------------*/
	menuHelp = gtk_menu_item_new_with_label ("Help");
	gtk_menu_bar_append (GTK_MENU_BAR(menubar), menuHelp);
	gtk_widget_show(menuHelp);

	/*-- Crea Help submenu --*/
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuHelp), menu);

	/*-- Crea About menu item da collocare in Help submenu --*/
	menuitem = gtk_menu_item_new_with_label ("About");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gui_open_about_event), NULL);
	gtk_widget_show (menuitem);
	/*---------------- Fine Help menu ----------------*/
	return menubar;
}

/*
 * Crea le varie text view (di scrittura e lettura) per la chat e per il pm, 
 * impostando i valori in modo opportuno.
 */
GtkWidget *gui_create_text(u_int8 chat_id, int type, int msg_type)
{
	logger(INFO, "[create_text]chat ID: %lld\n", chat_id);
	GtkWidget *scrolled_window;
	GtkWidget *view = gtk_text_view_new ();
	GtkTextBuffer *buffer;

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	//creazione della scrolled window
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC,
			GTK_POLICY_AUTOMATIC);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), TRUE);
	gtk_container_add (GTK_CONTAINER (scrolled_window), view);
	
	/* da qui parte la differenziazione tra le text view relative alla chat e al pm 
	 * con i relativi eventi, che si scatenano alla pressione di un tasto 
	 */
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
				model_str->text_area = GTK_TEXT_VIEW(view);
			}
			else {
				return NULL;
			}
		}
		else if(msg_type == PM) {
			pm_data *pm = (pm_data*)g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(chat_id));
			if(pm==NULL) {
				pm = calloc(1, sizeof(pm_data));
				pm->text_area = GTK_TEXT_VIEW(view);
				gtk_text_view_set_editable(GTK_TEXT_VIEW(pm->text_area),FALSE);
				gtk_text_buffer_create_tag(buffer, "blue_fg", "foreground", "blue", NULL);
				logger(INFO, "[create_text]Allocating pm window\n");
				g_hash_table_insert(pm_data_hashtable, (gpointer)to_string(chat_id), (gpointer)pm);
				logger(INFO, "[create_text]Allocated pm window\n");
			}
		}
	}	
	gtk_widget_show_all (scrolled_window);

	return scrolled_window;
}

/*
 * Crea la finestra relativa ad una singola chat e assegna ai componenti gli 
 * eventi appropriati
 */
int gui_open_chatroom(u_int8 chat_id) {

	GtkWidget *window;
	GtkWidget *menu;
	GtkWidget *vbox;
	GtkWidget *hbox; 
	GtkWidget *vbox2;
	GtkWidget *handlebox;  
	GtkWidget *text;
	GtkWidget *list;
	GtkWidget *chat;

	// Crea i vari componenti
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	menu = gui_create_menu();
	list = gui_create_users_list (chat_id);

	text = gui_create_text(chat_id, TOP, CHAT);
	chat = gui_create_text(chat_id, BOTTOM, CHAT);

	vbox = gtk_vbox_new(FALSE, 5);

	hbox = gtk_hbox_new(FALSE, 5);
	vbox2 = gtk_vbox_new(FALSE, 5);

	handlebox = gtk_handle_box_new();

	logger(INFO,"[open_chatroom_gui] chat_id %lld\n", chat_id);

	//aggiunge i vari componenti ai box
	gtk_container_add(GTK_CONTAINER(handlebox), menu);
	gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(vbox),hbox);
	gtk_container_add(GTK_CONTAINER(hbox),vbox2);
	gtk_container_add (GTK_CONTAINER (hbox), list);
	gtk_container_add(GTK_CONTAINER(vbox2),text);
	gtk_box_pack_end(GTK_BOX(vbox2), chat, FALSE, TRUE, 0);
	//aggiunge il vbox alla finestra principale
	gtk_container_add(GTK_CONTAINER(window), vbox);

	//setta le caratteristiche della finestra	
	gtk_container_border_width (GTK_CONTAINER (window), 0);
	gtk_window_set_default_size (GTK_WINDOW(window), 640, 400);
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


	logger(INFO, "[open gui] to string %s\n", to_string(chat_id));
	//evento che si scatena alla chiusura della finestra
	g_signal_connect(GTK_OBJECT(window), "destroy", G_CALLBACK(gui_leave_chat_event), (gpointer)to_string(chat_id));
	return 0;
}

/*
 * Crea la finestra per una conversazione privata
 */
int gui_open_pm(u_int8 user_id, gchar *nickname) {

	GtkWidget *window;
	GtkWidget *menu;
	GtkWidget *vbox;
	GtkWidget *vbox2;
	GtkWidget *handlebox;  
	GtkWidget *text;
	GtkWidget *chat;

	/*-- Crea la nuova finestra e i vari componenti --*/
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	
	vbox = gtk_vbox_new(FALSE, 5);
	vbox2 = gtk_vbox_new(FALSE, 5);
	text = gui_create_text(user_id, TOP, PM);
	chat = gui_create_text(user_id, BOTTOM, PM); 
	handlebox = gtk_handle_box_new();
	menu = gui_create_menu();

	/*-- Aggiunge i componenti ai box --*/
	gtk_container_add(GTK_CONTAINER(handlebox), menu);

	gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 0);    
	gtk_container_add(GTK_CONTAINER(vbox),vbox2);
	gtk_container_add(GTK_CONTAINER(vbox2),text);
	gtk_box_pack_end(GTK_BOX(vbox2), chat, FALSE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	
	/*-- Setta le caratteristiche della finestra --*/
	gtk_container_border_width (GTK_CONTAINER (window), 0);
	gtk_window_set_default_size (GTK_WINDOW(window), 640, 400);
	gtk_window_set_title(GTK_WINDOW (window), nickname); 

	gtk_widget_show(handlebox);
	gtk_widget_show(vbox);
	gtk_widget_show(vbox2);
	gtk_widget_show(text);
	gtk_widget_show (chat);
	gtk_widget_show(menu);      
	gtk_widget_show(window);

	/*-- evento che si scatena alla chiusura della finestra  --*/
	g_signal_connect(GTK_OBJECT(window), "destroy", G_CALLBACK(gui_leave_pm_event), (gpointer)to_string(user_id));

	pm_data *pm = (pm_data*)g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(user_id));
	if(pm!=NULL) {
		pm->window = GTK_WIDGET(window);
	}
	return 0; 
}

/*
 * Crea il componente grafico presente nella finestra iniziale in cui si può 
 * effettuare la creazione o la ricerca di una chat.
 */
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
	gtk_box_pack_start(GTK_BOX(bar_container), bar_button, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(bar_container), bar_create_button, FALSE, TRUE, 5);

	gtk_widget_show(bar_create_button);
	gtk_widget_show(bar_textfield);
	gtk_widget_show(bar_button);

	return bar_container;
}

/*
 * Controlla nell'hashtable la presenza o meno della struct pm_data relativa 
 * all'id dell'utente.
 */
pm_data *gui_pm_data_get(u_int8 id) {
	if(pm_data_hashtable!=NULL)
		return g_hash_table_lookup(pm_data_hashtable, (gconstpointer)to_string(id));
	else
		return NULL;
}

/*
 * Recupera dal tree_model associato alla gui della chat la lista degli id degli
 * utenti connessi.
 */
GList *gui_get_chat_users(u_int8 chat_id) {
	tree_model *mod = gui_get_tree_model(chat_id);
	GList *id_list = NULL;

	if(mod == NULL) {
		logger(INFO, "[gui_get_chat_users] users list not ready\n");
		return id_list;
	}
	GtkTreeIter iter;
	gchar *id;
	gboolean valid = TRUE;
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mod->user_model), &iter)==TRUE) {			

		while(valid) {
			gtk_tree_model_get(GTK_TREE_MODEL(mod->user_model), &iter, 1, &id, -1);
			logger(INFO, "[gui_get_chat_users]user id: %s\n", id);
			id_list = g_list_append(id_list, (gpointer)id);
			

			valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(mod->user_model), &iter); 
		}
	}
	
	return id_list;
}
