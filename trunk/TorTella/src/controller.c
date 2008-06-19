/**
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

#include "controller.h"

/** 
 * cambia lo status di un peer tramite l'invio di un PING a tutti gli utenti
 * e controlla che il pacchetto sia stato ricevuto correttamente dagli altri peer
 */
int controller_change_status(u_int1 status) 
{
	servent_data *peer, *sd;
	char *ret;
	if(servent_get_local () == NULL) {
		logger(CTRL_INFO,"[controller_change_status] local_servent not present\n");
	    return -1;
	}
	WLOCK(servent_get_local()->id);
	servent_get_local()->status = status; 	//! Cambio dello status dell'utente
	UNLOCK(servent_get_local()->id);	  
	logger(CTRL_INFO,"[controller_change_status] sending packet\n");
	GList *users = servent_get_values();
	int i=0;
	//!preparazione e invio del pacchetto con il nuovo status a tutti gli utenti 
	for(; i < g_list_length(users); i++) {
		peer = g_list_nth_data(users, i);
		peer->post_type = PING_ID;
		peer->packet_id = generate_id();
		servent_send_packet(peer);
	}
	//!fase di attesa della ricezione del messaggio di OK (o di TIMEOUT). 
	for(i=0; i<g_list_length(users); i++) {
		chatclient *client = (chatclient*)g_list_nth_data(users, i);
		if(client!=NULL) {
			peer = servent_get(client->id);
			logger(CTRL_INFO, "[controller_leave_chat]pop response %lld\n", client->id);
			if(peer!=NULL && peer->id!=servent_get_local()->id) {
				COPY_SERVENT(peer, sd);
				ret = servent_pop_response(peer);
				if(strcmp(ret, TIMEOUT)==0)
					return peer->id;						
			}
		}
	}			
	return 0;
}

/** 
 * notifica il cambiamento di status di un peer remoto tramite una chiamata alla gui
 */
int controller_manipulating_status(u_int8 user_id, u_int1 status) 
{
	char *status_tmp;
	if(user_id <= 0)
		return -1;
	if(status == ONLINE_ID)
		status_tmp = ONLINE;
	else if(status == BUSY_ID)
		status_tmp = BUSY;
	else if(status == AWAY_ID)
		status_tmp = AWAY;
	//!aggiornamento della gui con il nuovo status dell'utente
	gui_change_status(user_id, status_tmp);
	return 0;
}

/** 
 * invia un messaggio a tutti gli utenti di una chat e controlla che il pacchetto
 * sia stato ricevuto correttamente dagli altri peer.
 */
int controller_send_chat_users(u_int8 chat_id, u_int4 msg_len, char *msg) {
	if(chat_id != 0) {
		servent_data *data, *tmp, *sd;
		chatclient *user;
		chat *chat_elem = data_get_chat(chat_id);
		char *ret;
		if(chat_elem==NULL) {
			logger(CTRL_INFO, "[controller_send_chat_users]Chat %lld not present\n", chat_id);
			return -2;
		}
		GList *users = g_hash_table_get_values(chat_elem->users);
		if(users==NULL) {
			logger(CTRL_INFO, "[controller_send_chat_users]Users list is empty\n");
			return -2;
		}
		logger(CTRL_INFO, "[controller_send_chat_users]Users size: %d\n", g_list_length(users));

		int i=0;
		//!Preparazione del pacchetto di tipo MESSAGE e invio a tutti gli utenti della chat
		for(; i<g_list_length(users); i++) {

			user = g_list_nth_data(users, i);
			data = servent_get(user->id);
			if(data!=NULL && data->id!=servent_get_local()->id ) {
				RLOCK(data->id);
				COPY_SERVENT(data, tmp);
				UNLOCK(data->id);
				
				tmp->msg = strdup(msg);
				tmp->msg_len = msg_len;
				tmp->chat_id_req = chat_id;
				tmp->post_type = MESSAGE_ID;
	
				servent_send_packet(tmp);
				logger(CTRL_INFO, "[controller_send_chat_users]Sent msg\n");
			}
			else
				logger(CTRL_INFO, "[controller_send_chat_users]Servent not present\n");
		}
		//! Attesa di ricezione dei pacchetti di OK (o di TIMEOUT) inviati da tutti gli utenti. 
		for(i=0; i<g_list_length(users); i++) {
			chatclient *client = (chatclient*)g_list_nth_data(users, i);
			if(client!=NULL) {
				data = servent_get(client->id);
				logger(CTRL_INFO, "[controller_leave_chat]pop response %lld\n", client->id);
				if(data!=NULL && data->id!=servent_get_local()->id) {
					ret = servent_pop_response(data);
					if(strcmp(ret, TIMEOUT)==0)
						return data->id;
				}
			}
		}		
		return 0;
	}
	return -1;
}

/** 
 * invia un messaggio ad un sottoinsieme di utenti di una chat e controlla che 
 * il pacchetto sia stato ricevuto correttamente dagli utenti interessati.
 */
int controller_send_subset_users(u_int8 chat_id, u_int4 msg_len, char *msg, GList *users) { 
	char *ret;
	if(chat_id != 0) {
		int i= 0;
		logger(CTRL_INFO, "[controller_send_subset_users] chat id != 0\n");
		logger(CTRL_INFO, "[controller_send_subset_users] list length %d\n", g_list_length(users));
		//!Preparazione del pacchetto di tipo MESSAGE e invio a tutti gli utenti della lista
		for(; i<g_list_length(users); i++) {

			chatclient *user = g_list_nth_data(users, i);
			logger(CTRL_INFO, "[controller_send_subset_users] user id %lld\n",user->id);
			
			servent_data *data = servent_get(user->id);
			WLOCK(data->id);

			data->msg = strdup(msg);
			data->msg_len = msg_len;
			data->chat_id_req = chat_id;
			data->post_type = MESSAGE_ID;

			UNLOCK(data->id);
			logger(CTRL_INFO, "[controller_send_subset_users] sending packet\n");
			servent_send_packet(data); 
		}
		//! Attesa di ricezione dei pacchetti di OK (o di TIMEOUT) inviati dagli utenti della lista.
		servent_data *sd, *data;
		for(i=0; i<g_list_length(users); i++) {
			chatclient *client = (chatclient*)g_list_nth_data(users, i);
			if(client!=NULL) {
				data = servent_get(client->id);
				logger(CTRL_INFO, "[controller_leave_chat]pop response %lld\n", client->id);
				if(data!=NULL && data->id!=servent_get_local()->id) {
					ret = servent_pop_response(data);
					if(strcmp(ret, TIMEOUT)==0)
						return data->id;						
				}
			}
		}		
		return 0;
	}
	return -1;
}

/** 
 * invia un messaggio privato ad un utente e si accerta che questo sia stato
 * ricevuto correttamente.
 */
int controller_send_pm(u_int4 msg_len, char *msg, u_int8 recv_id) {
	logger(CTRL_INFO, "[controller_send_pm] receiver id %d\n", recv_id);
	char *ret;
	servent_data *sd;
	
	//!Preparazione e invio del messaggio privato all'utente con id pari a recv_id;
	servent_data *data = (servent_data*)servent_get(recv_id);
	
	WLOCK(data->id);

	data->msg = strdup(msg);
	data->msg_len = msg_len;
	data->chat_id_req = 0;
	data->post_type = MESSAGE_ID;

	UNLOCK(data->id);
    servent_send_packet(data); 
	//! Attesa di ricezione del pacchetto di OK (o di TIMEOUT)
	servent_data *peer;
	peer = servent_get(recv_id);
	logger(CTRL_INFO, "[controller_leave_chat]pop response %lld\n", recv_id);
	if(peer!=NULL && peer->id!=servent_get_local()->id) {
			ret = servent_pop_response(peer);
			if(strcmp(ret, TIMEOUT)==0)
				return peer->id;
									
	}
		
	return 0;
}

/** permette all'utente di abbandonare tutte le chat a cui era connesso */
int controller_leave_all_chat()
{
	chat *tmp;
	servent_data *sd = servent_get_local();
	GList *iter;
	if(sd == NULL) {
		logger(CTRL_INFO, "[controller_leave_all_chat] local servent not present\n");
		return -1;
	}
	GList *chat_list =  sd->chat_list;
	int i=0;
	logger(CTRL_INFO, "[controller_leave_all_chat] list length %d\n", g_list_length(chat_list));
	//!Chiamata alla funzione controller_leave_flooding per ogni chat a cui si e' connessi
	while((iter=g_list_last(chat_list)) != NULL) {
		tmp = (chat*)iter->data;
		controller_leave_flooding(tmp->id);  
		chat_list = sd->chat_list;
	}
	return 0;
}

/** 
 * stabilisce una connessione tra un peer e una lista di utenti di una chat tramite
 * l'avvio del client.
 */
int controller_connect_users(GList *users) {
	int result = 0;

	if(users!=NULL) {
		int i, counter = 3;
		chatclient *client;
		GList *users_orig = users;
		servent_data *peer, *sd;
		GList *response = NULL;
		GList *timeout = NULL;
		char *ret;
		while(counter--) {
			
			if(timeout!=NULL) {
				users = timeout;
				logger(CTRL_INFO, "[controller_connect_users]Retrying\n");
			}
			
			for(i=0; i<g_list_length(users); i++) {
				client = (chatclient*)g_list_nth_data(users, i);
				logger(CTRL_INFO, "[controller_connect_users]Connecting to client: %s\n", client->nick);
				logger(CTRL_INFO, "[controller_connect_users]Get local status: %s\n", to_string(servent_get_local()->status));
				if(servent_get(client->id)==NULL) {
					peer = servent_start_client(client->ip, client->port, 0);
					response = g_list_append(response, (gpointer)peer);
					
					ret = servent_pop_response(peer);

				}
				else
					logger(CTRL_INFO, "[controller_connect_users]Already connected\n");
					result = -2;
			}

			timeout = NULL;
			for(i=0; i<g_list_length(response); i++) {
				peer = (servent_data*)g_list_nth_data(response, i);
				logger(CTRL_INFO, "[controller_connect_users]pop response %lld\n", peer->id);
				if(peer!=NULL && peer->id!=servent_get_local()->id) {

					ret = servent_pop_response(peer);
					logger(CTRL_INFO, "[controller_connect_users]ret: %s\n", ret);
					if(strcmp(ret, TIMEOUT)==0) {
						logger(CTRL_INFO, "[controller_connect_users]TIMEOUT\n");
						//!Aggiunge alla lista dei non connessi (per ritentare)
						timeout = g_list_append(timeout, (gpointer)client);
					}
					else {
						logger(CTRL_INFO, "[controller_connect_users]RECEIVED OK %s\n", ret);
					}
				}
			}
			response = NULL;
			
			if(timeout==NULL) {
				counter=-1; //!connessioni avvenute con successo
				logger(CTRL_INFO, "[controller_connect_users]Checking users connections\n");
				controller_check_users_con(users_orig);
				break;
			}
		}
		if(counter==-1)
			return result;
	}
	
	return result;
}

/** 
 * funzione d'appoggio per la controller_connect_users(), verifica che il peer
 * con cui si sta cercando di stabilire una connessione sia effettivamente pronto.??????????
 */
int controller_check_users_con(GList *users) {
	if(users==NULL)
		return -1;
	
	servent_data *client;
	chatclient *user;
	int counter=3, i;
	while(counter--) {
		for(i=0; i<g_list_length(users); i++) {
			user = g_list_nth_data(users, i);
			logger(CTRL_INFO, "[controller_check_users_con]User nick: %s\n", user->nick);
			if(user!=NULL) {
				client = servent_get(user->id);
				logger(CTRL_INFO, "[controller_check_users_con]client nick: %s\n", client->nick);
				if((client!=NULL) && (!client->is_online)) {
					logger(CTRL_INFO, "[controller_check_users_con]Servent %lld not ready\n", client->id);
					usleep(200000);
					continue;
				}
			}
			counter=0;
		}
	}
	
	return 0;
}

/**
 * permette l'invio di un pacchetto di tipo BYE che notifica a tutti gli utenti
 * conosciuti che si sta abbandonando l'applicazione e controlla che tutti abbiano 
 *ricevuto il messaggio correttamente.
 */
int controller_send_bye() 
{
	chatclient *client;
	servent_data *tmp, *peer, *sd;
	char *ret;
	if(servent_get_local() == NULL) {
		logger(CTRL_INFO,"[controller_send_bye] local_servent not present\n");
	    return -1;
	}
	  
	logger(CTRL_INFO,"[controller_send_bye] sending packet\n");
	
	GList *users = servent_get_values();
	int i=0;
	//!preparazione e invio del pacchetto di tipo BYE a tutti gli utenti conosciuti
	for(; i < g_list_length(users); i++) {
		peer = g_list_nth_data(users, i);
		logger(CTRL_INFO,"[controller_send_bye] sending packet to %lld from %lld\n", peer->id, servent_get_local()->id);
		if(peer!=NULL && peer->id!=servent_get_local()->id && (peer->id >= conf_get_gen_start())) {
			RLOCK(peer->id);
			COPY_SERVENT(peer, tmp);
			UNLOCK(peer->id);
			tmp->post_type = BYE_ID;
			logger(CTRL_INFO, "[controller_send_bye] checking post type %d\n", tmp->post_type);
			tmp->packet_id = generate_id();
			servent_send_packet(tmp);
			logger(CTRL_INFO, "[controller_send_bye] sent\n");
			
		}
	}
	//! Attesa di ricezione dei pacchetti di OK (o di TIMEOUT) inviati da tutti gli utenti
	for(i=0; i<g_list_length(users); i++) {
		client = (chatclient*)g_list_nth_data(users, i);
		if(client!=NULL) {
			peer = servent_get(client->id);
			logger(CTRL_INFO, "[controller_send_bye]pop response %lld\n", client->id);
			if(peer!=NULL && peer->id!=servent_get_local()->id && (peer->id >= conf_get_gen_start())) {
				RLOCK(peer->id);
				COPY_SERVENT(peer, sd);
				UNLOCK(peer->id);
				
				ret = servent_pop_response(sd);
				logger(CTRL_INFO, "[controller_send_bye]ret: %s\n", ret);
				if(strcmp(ret, TIMEOUT)==0) {
					logger(CTRL_INFO, "[controller_send_bye]TIMEOUT\n");
					return sd->id;
				}
				logger(CTRL_INFO, "[controller_send_bye]RECEIVED OK %s\n", ret);
				
			}
		}
	}
	
	return 0;
}

/**
 * Alla ricezione di una BYE da un utente remoto chiude tutte le conversazioni
 * private aperte (se esistenti) con quello specifico utente.
 */
int controller_receive_bye(u_int8 id)
{
   pm_data *pm;
   logger(CTRL_INFO, "[controller_receive_bye] user id %lld\n", id);
   if((pm = gui_pm_data_get(id)) != NULL) {
   	gui_leave_pm_event(pm->window, (gpointer)to_string(id)); 
   }
   return 0;
}

/**
 * Legge il file di configurazione, avvia il logger, legge il file init_data che
 * contiene ip e porta dei peer vicini, avvia il servent e infine avvia il timer
 * necessario per il meccanismo di failure detection.
 */
int controller_init(const char *filename, const char *cache) {
	
	//!lettura del file di configurazione
	conf_read(filename);
	
	//!avvio del logger
	logger_init(conf_get_verbose());
	
	//!inserimento dei vicini presenti nel file init_data nella lista 
	GList *init_list = NULL;
	init_list = init_read_file(cache);
	
	//!avvio del servente
	servent_start(init_list);

	//!avvio del timer per il meccanismo di failure detection
	servent_start_timer();
	
	return 0;
} 

/**
 * Uccide tutti i thread e chiude il file di logger.
 */
int controller_exit() {
	servent_kill_all_thread(0);
	
	logger_close();
	return 0;
}

/**
 * Costruisce la prima finestra dell'interfaccia grafica, relativa alla creazione
 * e alla ricerca delle chat e avvia l'apposito thread per la gestione degli eventi
 */
int controller_init_gui(void) {

	/**--Dichiarazione dei widget della finestra --*/
	GtkWidget *window;
	GtkWidget *menu;
	GtkWidget *vbox;
	GtkWidget *handlebox;  
	GtkWidget *list;
	GtkWidget *searchbar;

	/**-- Creazione della finestra --*/
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	/**-- Creazione del vbox e della chat_list--*/
	vbox = gtk_vbox_new(FALSE, 5);
	list = gui_create_chat_list(0);
	/**-- Creazione dell'handlebox --*/
	handlebox = gtk_handle_box_new();

	/**-- creazione della menubar --*/
	menu = gui_create_menu();

	/**-- connette la finestra all'evento gui_close_event --*/
	gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(gui_close_event), NULL);

	/**-- Aggiunge il menubar all'handlebox --*/
	gtk_container_add(GTK_CONTAINER(handlebox), menu);

	/**-- creazione dell'area della finestra relativa alla ricerca e alla creazione della chat --*/
	searchbar = gui_create_searchbar();

	/**-- aggiunta dei componenti handlebox, searchbar e list alla vbox --*/
	gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), searchbar, FALSE, TRUE, 5);

	gtk_container_add(GTK_CONTAINER(vbox),list);

	/**-- Aggiunta del vbox alla finestra principale --*/
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/**-- setting delle dimensioni e del titolo della window --*/
	gtk_container_border_width (GTK_CONTAINER (window), 0);
	gtk_window_set_default_size (GTK_WINDOW(window), 300, 400);
	gtk_window_set_title(GTK_WINDOW (window), "Lista Chat");


	/**-- Mostra i widget --*/
	gtk_widget_show_all(window);
	
	/**-- Start the GTK event loop --*/
	gtk_main();
	return 1;
}

/**
 * Controlla che la query inserita sia accettabile, dopo di che invia un pacchetto
 * di tipo SEARCH a tutti gli utenti conosciuti, evitando l'invio ai fake id e a 
 * se' stesso.
 */
u_int8 controller_search(const char *query) {
	if(query==NULL || strcmp(query, "")==0) {
		logger(CTRL_INFO, "[controller_search]Query string not valid\n");
		return 0;
	}
	
	GList *servents = servent_get_values();
	logger(CTRL_INFO, "[controller_search]Query: %s\n", query);
	if(servents==NULL) {
		logger(CTRL_INFO, "[controller_search]Servents list is empty\n");
		return 0;
	}
	servent_data *servent, *tmp;
	int i=0;
	/** Preparazione e invio del pacchetto di SEARCH a tutti gli utenti conosciuti */
	for(; i<g_list_length(servents); i++) {
		servent = g_list_nth_data(servents, i);
		logger(CTRL_INFO, "[controller_search]Servent ID: %lld\n", servent->id);
		
		//!controllo per evitare l'auto-invio del pacchetto e l'invio ai fake id
		if(servent->id!=servent_get_local()->id && servent->id >= conf_get_gen_start ()) {
			
			if(servent->queue==NULL) {
				continue;
			}
			RLOCK(servent->id);
			logger(INFO, "[controller_search]Copy servent\n");
			COPY_SERVENT(servent, tmp);
			UNLOCK(servent->id);
			if(tmp->queue==NULL || tmp->res_queue==NULL) {
				continue;
			}
			logger(INFO, "[controller_search]Copy\n");
			tmp->post_type = SEARCH_ID;
			tmp->title = strdup(query);
			tmp->title_len = strlen(query);
			tmp->ttl = 3;
			tmp->hops = 0;
			tmp->packet_id = generate_id(); //!Se non ci fosse verrebbe riutilizzato l'ID di uno degli eventuali pacchetti SEARCH ritrasmessi
			logger(INFO, "[controller_search]Send\n");
			servent_send_packet(tmp);
			logger(INFO, "[controller_search]Sent\n");
		}
		else
			logger(INFO, "[controller_search]Local ID\n");
		
	}

	logger(INFO, "[controller_search]End\n");
	
	return 0;
}

/**
 * Invia un pacchetto di tipo JOIN a tutti gli utenti conosciuti (ammesso che ne conosca
 * qualcuno, evitando l'invio ai fake id e a se' stesso; successivamente, ogni volta
 * che riceve un messaggio di OK da un utente, aggiunge questi alla chat; infine
 * aggiunge anche se' stesso.
 */
int controller_join_flooding(u_int8 chat_id) {
	if(chat_id > 0) {
		GList *servents = servent_get_values();
		if(servents==NULL) {
			logger(CTRL_INFO, "[controller_join]Servents null\n");
			return 0;
		}
		servent_data *servent, *tmp, *peer;
		chat *chat_elem = data_get_chat(chat_id);
		if(chat_elem != NULL) {
			int i=0;
			int count = 0;
			//! Preparazione e invio del pacchetto di JOIN a tutti gli utenti conosciuti
			for(; i<g_list_length(servents); i++) {
				servent = g_list_nth_data(servents, i);
				logger(CTRL_INFO, "[controller_join]Servent ID: %lld;  nick: %s\n", servent->id, servent->nick);
				
				//!controllo per evitare l'auto-invio del pacchetto e l'invio ai fake id
				if(servent->id!=servent_get_local()->id && servent->id >= conf_get_gen_start ()) {
					
					if(servent->queue==NULL) {
						logger(CTRL_INFO, "[controller_join]Coda Servent NULL\n");
						continue;
					}
					RLOCK(servent->id);
					logger(INFO, "[controller_join]Copy servent\n");
					COPY_SERVENT(servent, tmp);
					UNLOCK(servent->id);
					if(tmp->queue==NULL || tmp->res_queue==NULL) {
						continue;
					}
					logger(INFO, "[controller_join]Copy\n");
					tmp->post_type = JOIN_ID;
					tmp->ttl = 3;
					tmp->hops = 0;
					tmp->nick_req = servent_get_local()->nick;
					tmp->packet_id = generate_id(); //!Se non ci fosse verrebbe riutilizzato l'ID di uno degli eventuali pacchetti SEARCH ritrasmessi
					tmp->chat_id_req = chat_id;
					tmp->user_id_req = servent_get_local()->id;
					tmp->status_req = servent_get_local()->status;
					tmp->ip_req = servent_get_local()->ip;
					tmp->port_req = servent_get_local()->port;	
					if(count == 0) {
						servent_get_local()->chat_list = g_list_append(servent_get_local()->chat_list, (gpointer)chat_elem);
						count = 1;
					}	
					logger(INFO, "[controller_join]Send\n");
					servent_send_packet(tmp);
					logger(INFO, "[controller_join]Sent\n");
				}
				else
					logger(INFO, "[controller_join]Local ID\n");

			}
			GList *users = data_get_chatclient_from_chat(chat_elem->id);
			int j;
			//! Attesa di ricezione dei pacchetti di OK (o di TIMEOUT).
			for(i=0; i<g_list_length(servents); i++) {
				peer = (servent_data*)g_list_nth_data(servents, i);
				chatclient *client = data_get_chatclient(peer->id);
				logger(CTRL_INFO, "[controller_join_chat]Sending join to %lld\n", peer->id);
				if(client!=NULL && peer!=NULL && peer->id!=servent_get_local()->id && peer->id >= conf_get_gen_start ()) {
					char *ret = servent_pop_response(peer);
					if(ret==NULL) {
						return -1;
					}
					if(strcmp(ret, TIMEOUT)==0)
						return peer->id;

					for(j=0; j<g_list_length(users); j++) {
						chatclient *user = (chatclient*)g_list_nth_data(users, j);
						if(user!=NULL) {
							if(user->id == client->id) {
								//!Aggiunta dell'utente remoto alla chat e conseguente aggiornamento della gui
								data_add_user_to_chat(chat_elem->id, client->id, client->nick, client->ip, client->port);
								gui_add_user_to_chat(chat_elem->id, client->id, client->nick, peer->status);
								break;
							}
						}
					}
				}
			}
		}
		//!Aggiunta dell'utente locale alla chat e conseguente aggiornamento della gui
		gui_add_user_to_chat(chat_elem->id, servent_get_local()->id, servent_get_local()->nick, servent_get_local()->status);
		data_add_user_to_chat(chat_elem->id, servent_get_local()->id, servent_get_local()->nick, servent_get_local()->ip, servent_get_local()->port);
		return 0;
	}
	logger(INFO, "[controller_join]End\n");
	
	return 0;
}

/**
 * Invia un pacchetto di tipo LEAVE a tutti gli utenti conosciuti per avvertire che
 * si sta abbandonando la chat (rappresentata dal parametro chat_id) e rimuove 
 * questa dall'elenco delle chat a cui si e' connessi, dopo di che attende che tutti 
 * abbiano ricevuto il messaggio correttamente.
 */
int controller_leave_flooding(u_int8 chat_id) {
	char *ret;
	logger(CTRL_INFO,"[controller_leave_chat] chat_id %lld\n", chat_id); 
	if(chat_id>0) {
		logger(CTRL_INFO,"[controller_leave_chat] chat_id>0\n");
		chat *chat_elem = data_get_chat(chat_id);
		if(chat_elem!=NULL) {
			logger(CTRL_INFO,"[controller_leave_chat] chat present\n");
			GList *clients = servent_get_values();
			chatclient *client;
			servent_data *peer, *sd;
			int i;
			
			//! Preparazione e invio del pacchetto di LEAVE a tutti gli utenti conosciuti
			for(i=0; i<g_list_length(clients); i++) {
				client = (chatclient*)g_list_nth_data(clients, i);
				if(client!=NULL) {
					logger(CTRL_INFO,"[controller_leave_chat] client present\n");

					peer = servent_get(client->id);
					if(peer!=NULL && peer->id!=servent_get_local()->id) {
						logger(CTRL_INFO,"[controller_leave_chat] peer present\n");
						
						WLOCK(peer->id);
						COPY_SERVENT(peer, sd);
						sd->chat_id_req = chat_id;
						sd->user_id_req = servent_get_local()->id;
						sd->post_type = LEAVE_ID;
						sd->packet_id = generate_id();
						sd->ttl = 3;
						sd->hops = 0;
							
					    UNLOCK(peer->id);
						servent_send_packet(sd); 
					}
				}
			}
			//! Attesa di ricezione dei pacchetti di OK (o di TIMEOUT).
			for(i=0; i<g_list_length(clients); i++) {
				client = (chatclient*)g_list_nth_data(clients, i);
				if(client!=NULL) {
					peer = (servent_data*)g_list_nth_data(clients, i);
					logger(CTRL_INFO, "[controller_leave_chat]pop response %lld\n",peer->id);
					if(peer!=NULL && peer->id!=servent_get_local()->id) {
						COPY_SERVENT(peer, sd);
					
						ret = servent_pop_response(peer);
						if(strcmp(ret, TIMEOUT)==0)
							return peer->id;						
					}
				}
			}
			servent_get_local()->chat_list = g_list_remove(servent_get_local()->chat_list, (gconstpointer)chat_elem); 
			data_del_user_from_chat (chat_id, servent_get_local()->id);
			return 0;
		}
	}
	return -1;
}

/**
 * Consente ad un utente di creare una chat purche' abbia un nome che non sia nullo o
 * rappresentato da una stringa vuota. Successivamente viene generato un id da associare
 * alla chat e infine viene aperta la gui relativa alla chat con conseguente aggiunta
 * dell'utente alla lista dei peer partecipanti alla chat.
 */
int controller_create(const char *title) {
	if(title==NULL || strcmp(title, "")==0) {
		logger(CTRL_INFO, "[controller_create]Chat title is invalid\n");
		return -1;
	}
	
	//!Generazione degll'id e aggiunta della chat all'hashtable delle chat
	u_int8 chat_id = generate_id();
	data_add_chat(chat_id, title);
	
	servent_data *local = servent_get_local();
	//!aggiunta dell'utente ai client connessi alla chat
	data_add_user_to_chat(chat_id, local->id, local->nick, local->ip, local->port);
	chat *test = data_get_chat(chat_id);
	//!aggiunta della chat alla lista locale delle chat a cui si e' connessi
	local->chat_list = g_list_append(local->chat_list, (gpointer)test);
	
	//!apertura della finestra della chat e aggiornamento dei dati
	gui_open_chatroom(chat_id);
	data_add_existing_user_to_chat(chat_id, servent_get_local()->id);
	gui_add_user_to_chat(chat_id, local->id, local->nick, local->status);
	return 0;
}

/**
 * Chiama la funzione gui_add_user_to_chat controllando prima che l'utente che
 * si sta cercando di inserire abbia tutti i campi inizializzati correttamente. Nel
 * caso invece l'utente non sia conosciuto e quindi sia inizialmente NULL viene 
 * aggiunto alla chat un utente in modo provvisorio.
 */
int controller_add_user_to_chat(u_int8 chat_id, u_int8 id) {
	servent_data *servent = servent_get(id);

	if(servent ==NULL) {
		logger(CTRL_INFO, "[controller_add_user_to_chat] servent not present\n");
		chatclient *tmp = data_get_chatclient(id);
		//!Aggiornamento provvisorio della lista degli utenti della chat a livello di GUI
		gui_add_user_to_chat(chat_id, tmp->id, tmp->nick, 0);
		return 0;
	}
	logger(CTRL_INFO, "[controller_add_user_to_chat]Adding user: %s, id: %lld, status: %c\n", servent->nick, servent->id, servent->status);
	//!Aggiornamento della lista degli utenti della chat a livello di GUI
	gui_add_user_to_chat(chat_id, servent->id, servent->nick, servent->status);
	
	return 0;
}

/**
 * Funzione d'appoggio che rimuove l'utente dalla chat a cui era connesso, sia a 
 * livello di gui che a livello di data_manager.
 */
int controller_rem_user_from_chat(u_int8 chat_id, u_int8 id) {
	gui_del_user_from_chat(chat_id, id);
	data_del_user_from_chat(chat_id, id);
	return 0;
}

/**
 * Permette ad un utente, una volta ricevuto un messaggio da un partecipante
 * alla chat, di aggiornare la text view della chat stessa.
 */
int controller_add_msg_to_chat(u_int8 chat_id, char *msg) {
	
	if(msg==NULL) {
		logger(CTRL_INFO, "[controller_add_msg_to_chat]Message invalid\n");
		return -1;
	}
	
	if(chat_id>0) {
		if(gui_add_msg_to_chat(chat_id, msg)<0) {
			logger(CTRL_INFO, "[controller_add_msg_to_chat]Message error\n");
			return -2;
		}
		return 0;
	}

	return -3;
}

/**
 * Come la controller_add_msg_to_chat, ma utilizzata nel caso venga ricevuto 
 * un messaggio privato.
 */
int controller_add_msg(u_int8 sender_id, char *msg) {
	
	if(msg==NULL) {
		logger(CTRL_INFO, "[controller_add_msg]Message invalid\n");
		return -1;
	}
	
	if(sender_id>0) {
		if(gui_add_msg_pm(sender_id, msg)<0) {
			logger(CTRL_INFO, "[controller_add_msg]Adding pm to gui error\n");
			return -2;
		}
		return 0;
	}
	return -3;
}
