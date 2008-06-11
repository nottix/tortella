#include "controller.h"

int controller_change_status(u_int1 status) 
{
	servent_data *peer, *sd;
	char *ret;
	if(servent_get_local () == NULL) {
		logger(CTRL_INFO,"[controller_change_status] local_servent NULL\n");
	    return -1;
	}
	WLOCK(servent_get_local()->id);
	servent_get_local()->status = status;
	UNLOCK(servent_get_local()->id);	  
	logger(CTRL_INFO,"[controller_change_status] sending packet\n");
	GList *users = servent_get_values();
	int i=0;
	for(; i < g_list_length(users); i++) {
		peer = g_list_nth_data(users, i);
		peer->post_type = PING_ID;
		peer->packet_id = generate_id();
		servent_send_packet(peer);
	}
	//////////////////////////////////////////////
	for(i=0; i<g_list_length(users); i++) {
		chatclient *client = (chatclient*)g_list_nth_data(users, i);
		printf("eeee\n");
		if(client!=NULL) {
			peer = servent_get(client->id);
			logger(CTRL_INFO, "[controller_leave_chat]pop response %lld\n", client->id);
			if(peer!=NULL && peer->id!=servent_get_local()->id) {
				COPY_SERVENT(peer, sd);
						//WLOCK(peer->id);

						//UNLOCK(peer->id);
				ret = servent_pop_response(peer);
				if(strcmp(ret, TIMEOUT)==0)
					return peer->id;
				printf("RECEIVED %s\n", ret);
						
			}
		}
	}		
	
	
	return 0;
}

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
	gui_change_status(user_id, status_tmp);
	return 0;
}


int controller_send_chat_users(u_int8 chat_id, u_int4 msg_len, char *msg) {
	if(chat_id != 0) {
		servent_data *data, *tmp, *sd;
		chatclient *user;
		chat *chat_elem = data_get_chat(chat_id);
		char *ret;
		if(chat_elem==NULL) {
			logger(CTRL_INFO, "[controller_send_chat_users]Chat %lld non presente\n", chat_id);
			return -2;
		}
		GList *users = g_hash_table_get_values(chat_elem->users);
		if(users==NULL) {
			logger(CTRL_INFO, "[controller_send_chat_users]Users NULL\n");
			return -2;
		}
		logger(CTRL_INFO, "[controller_send_chat_users]Users size: %d\n", g_list_length(users));

		int i=0;
		for(; i<g_list_length(users); i++) {

			user = g_list_nth_data(users, i);
			data = servent_get(user->id);
			if(data!=NULL && data->id!=servent_get_local()->id ) {
				//RLOCK(data->id);
				COPY_SERVENT(data, tmp);
				//UNLOCK(data->id);
				
				tmp->msg = strdup(msg);
				tmp->msg_len = msg_len;
				tmp->chat_id_req = chat_id;
				tmp->post_type = MESSAGE_ID;
	
				servent_send_packet(tmp);
				logger(CTRL_INFO, "[controller_send_chat_users]Sent msg\n");
			}
			else
				logger(CTRL_INFO, "[controller_send_chat_users]Servent NULL\n");
		}
		///////////////////////////////////////////
		for(i=0; i<g_list_length(users); i++) {
			chatclient *client = (chatclient*)g_list_nth_data(users, i);
			printf("eeee\n");
			if(client!=NULL) {
				data = servent_get(client->id);
				logger(CTRL_INFO, "[controller_leave_chat]pop response %lld\n", client->id);
				if(data!=NULL && data->id!=servent_get_local()->id) {
					COPY_SERVENT(data, sd); //?????
						//WLOCK(peer->id);

						//UNLOCK(peer->id);
					ret = servent_pop_response(data);
					if(strcmp(ret, TIMEOUT)==0)
						return data->id;
				printf("RECEIVED %s\n", ret);
						
				}
			}
		}		
		return 0;
	}
	return -1;
}

int controller_send_subset_users(u_int8 chat_id, u_int4 msg_len, char *msg, GList *users) { 
	char *ret;
	if(chat_id != 0) {
		int i= 0;
		logger(CTRL_INFO, "[controller_send_subset_users] chat id != 0\n");
		logger(CTRL_INFO, "[controller_send_subset_users] list length %d\n", g_list_length(users));
		for(; i<g_list_length(users); i++) {

			chatclient *user = g_list_nth_data(users, i);
			logger(CTRL_INFO, "[controller_send_subset_users] user id %lld\n",user->id);
			
			//servent_data *data = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(user->id));
			servent_data *data = servent_get(user->id);
			WLOCK(data->id);

			data->msg = strdup(msg);
			data->msg_len = msg_len;
			data->chat_id_req = chat_id;
			data->post_type = MESSAGE_ID;

			UNLOCK(data->id);
			logger(CTRL_INFO, "[controller_send_subset_users] sending packet\n");
			servent_send_packet(data); //
			//pthread_cond_signal(&data->cond);
		}
		//////////////////////////////////////////
		servent_data *sd, *data;
		for(i=0; i<g_list_length(users); i++) {
			chatclient *client = (chatclient*)g_list_nth_data(users, i);
			printf("eeee\n");
			if(client!=NULL) {
				data = servent_get(client->id);
				logger(CTRL_INFO, "[controller_leave_chat]pop response %lld\n", client->id);
				if(data!=NULL && data->id!=servent_get_local()->id) {
					COPY_SERVENT(data, sd);
						//WLOCK(peer->id);

						//UNLOCK(peer->id);
					ret = servent_pop_response(data);
					if(strcmp(ret, TIMEOUT)==0)
						return data->id;
				printf("RECEIVED %s\n", ret);
						
				}
			}
		}		
		return 0;
	}
	return -1;
}

int controller_send_pm(u_int4 msg_len, char *msg, u_int8 recv_id) {
	logger(CTRL_INFO, "[controller_send_pm] id destinatario %d\n", recv_id);
	char *ret;
	servent_data *sd;
	servent_data *data = (servent_data*)servent_get(recv_id);
	
	WLOCK(data->id);

	data->msg = strdup(msg);
	data->msg_len = msg_len;
	data->chat_id_req = 0;
	data->post_type = MESSAGE_ID;

	UNLOCK(data->id);
    servent_send_packet(data); 
	////////////////////////////////
	printf("eeee\n");
	servent_data *peer;
	peer = servent_get(recv_id);
	logger(CTRL_INFO, "[controller_leave_chat]pop response %lld\n", recv_id);
	if(peer!=NULL && peer->id!=servent_get_local()->id) {
			COPY_SERVENT(peer, sd);
			//WLOCK(peer->id);
			//UNLOCK(peer->id);
			ret = servent_pop_response(peer);
			if(strcmp(ret, TIMEOUT)==0)
				return peer->id;
			printf("RECEIVED %s\n", ret);
						
			
	}
		
	return 0;
}

int controller_join_chat(u_int8 chat_id) {
	if(chat_id>0) {
		printf("eeee2\n");
		chat *chat_elem = data_get_chat(chat_id);
		char *ret;
		if(chat_elem!=NULL) {
			printf("eeee1\n");
			GList *clients = g_hash_table_get_values(chat_elem->users);
			chatclient *client;
			servent_data *peer, *sd;
			int i;
			for(i=0; i<g_list_length(clients); i++) {
				client = (chatclient*)g_list_nth_data(clients, i);
				printf("eeee\n");
				if(client!=NULL) {
					peer = servent_get(client->id); 
					logger(CTRL_INFO, "[controller_join_chat]Sending join to %lld\n", client->id);
					if(peer!=NULL) {
						logger(CTRL_INFO, "[controller_join_chat] peer not NULL\n");
						COPY_SERVENT(peer, sd);
						//WLOCK(peer->id);
						sd->chat_id_req = chat_id;
						sd->post_type = JOIN_ID;
						
						logger(CTRL_INFO, "[controller_join_chat] chat %s\n", chat_elem->title);
						servent_get_local()->chat_list = g_list_append(sd->chat_list, (gpointer)chat_elem); //OK???
						//sd->chat_list = g_list_append(sd->chat_list, (gpointer)chat_elem); //
						//servent_add_to_chat_list(sd, chat_elem); //Prova
						//UNLOCK(peer->id);
						servent_send_packet(sd);
						//pthread_cond_signal(&peer->cond);
					}
				}
			}
			
			for(i=0; i<g_list_length(clients); i++) {
				client = (chatclient*)g_list_nth_data(clients, i);
				printf("eeee\n");
				if(client!=NULL) {
					peer = servent_get(client->id);
					logger(CTRL_INFO, "[controller_join_chat]Sending join to %lld\n", client->id);
					if(peer!=NULL) {
						COPY_SERVENT(peer, sd);
						//WLOCK(peer->id);

						//UNLOCK(peer->id);
						ret = servent_pop_response(sd);
						if(ret==NULL) {
							logger(CTRL_INFO, "[controller_join_chat]Ret NULL\n");
							return -1;
						}
						if(strcmp(ret, TIMEOUT)==0)
							return peer->id;
						printf("RECEIVED %s\n", ret);
						data_add_user_to_chat(chat_elem->id, client->id, client->nick, client->ip, client->port);
	
						gui_add_user_to_chat(chat_elem->id, client->id, client->nick, peer->status);
					}
				}
			}
			gui_add_user_to_chat(chat_elem->id, servent_get_local()->id, servent_get_local()->nick, servent_get_local()->status);
			data_add_user_to_chat(chat_elem->id, servent_get_local()->id, servent_get_local()->nick, servent_get_local()->ip, servent_get_local()->port);
			return 0;
		}
	}
	return -1;
}

int controller_leave_chat(u_int8 chat_id) {
	char *ret;
	logger(CTRL_INFO,"[controller_leave_chat] chat_id %lld\n", chat_id); 
	if(chat_id>0) {
		logger(CTRL_INFO,"[controller_leave_chat] chat_id>0\n");
		chat *chat_elem = data_get_chat(chat_id);
		if(chat_elem!=NULL) {
			logger(CTRL_INFO,"[controller_leave_chat] chat_elem !=NULL\n");
			GList *clients = g_hash_table_get_values(chat_elem->users);
			chatclient *client;
			servent_data *peer, *sd;
			int i;
			for(i=0; i<g_list_length(clients); i++) {
				client = (chatclient*)g_list_nth_data(clients, i);
				if(client!=NULL) {
					logger(CTRL_INFO,"[controller_leave_chat] client !=NULL\n");

					peer = servent_get(client->id);
					if(peer!=NULL && peer->id!=servent_get_local()->id) {
						logger(CTRL_INFO,"[controller_leave_chat] peer !=NULL\n");
						
						WLOCK(peer->id);
						peer->chat_id_req = chat_id;
						peer->post_type = LEAVE_ID;
						peer->chat_list = g_list_remove(peer->chat_list, (gconstpointer)chat_elem); //PROVA
						UNLOCK(peer->id);
						//pthread_cond_signal(&peer->cond);
						servent_send_packet(peer); //PROVA
					}
				}
			}
			for(i=0; i<g_list_length(clients); i++) {
				client = (chatclient*)g_list_nth_data(clients, i);
				printf("eeee\n");
				if(client!=NULL) {
					peer = servent_get(client->id);
					logger(CTRL_INFO, "[controller_leave_chat]pop response %lld\n", client->id);
					if(peer!=NULL && peer->id!=servent_get_local()->id) {
						COPY_SERVENT(peer, sd);
						//WLOCK(peer->id);

						//UNLOCK(peer->id);
						ret = servent_pop_response(peer);
						if(strcmp(ret, TIMEOUT)==0)
							return peer->id;
						printf("RECEIVED %s\n", ret);
						//data_del_user_from_chat(chat_id, client->id);
						//remove_user_from_chat_list(chat_id, client->id);
						//data_add_user_to_chat(chat_elem->id, client->id, client->nick, client->ip, client->port);
						//add_user_to_chat_list(chat_elem->id, client->id, client->nick, peer->status);
					}
				}
			}	
			return 0;
		}
	}
	return -1;
}

int controller_leave_all_chat()
{
	servent_data *sd = servent_get_local();
	if(sd == NULL) {
		logger(CTRL_INFO, "[controller_leave_all_chat] servent NULL\n");
		return -1;
	}
	GList *chat_list =  sd->chat_list;
	int i=0;
	logger(CTRL_INFO, "[controller_leave_all_chat] lunghezza lista %d\n", g_list_length(chat_list));
	for(; i < g_list_length(chat_list); i++) {
		chat *tmp =  (chat*)g_list_nth_data(chat_list, i);
		controller_leave_chat(tmp->id);
	}
	return 0;
}

int controller_connect_users(GList *users) {
	if(users!=NULL) {
		int i;
		chatclient *client;
		servent_data *peer, *sd;
		char *ret;
		for(i=0; i<g_list_length(users); i++) {
			client = (chatclient*)g_list_nth_data(users, i);
			logger(CTRL_INFO, "[controller_connect_users]Connecting to client: %s\n", client->nick);
			logger(CTRL_INFO, "[controller_connect_users]Get local status: %s\n", to_string(servent_get_local()->status));
			if(servent_get(client->id)==NULL)
				servent_start_client(client->ip, client->port);
			else
				logger(CTRL_INFO, "[controller_connect_users]Già connesso\n");
		}
		
		for(i=0; i<g_list_length(users); i++) {
			client = (chatclient*)g_list_nth_data(users, i);
			if(client!=NULL) {
				peer = servent_get(client->id);
				logger(CTRL_INFO, "[controller_connect_users]pop response %lld\n", client->id);
				if(peer!=NULL && peer->id!=servent_get_local()->id) {
					RLOCK(peer->id);
					COPY_SERVENT(peer, sd);
					UNLOCK(peer->id);

					ret = servent_pop_response(sd);
					logger(CTRL_INFO, "[controller_connect_users]ret: %s\n", ret);
					if(strcmp(ret, TIMEOUT)==0) {
						logger(CTRL_INFO, "[controller_connect_users]TIMEOUT\n");
						return sd->id;
					}
					logger(CTRL_INFO, "[controller_connect_users]RECEIVED OK %s\n", ret);
				}
			}
		}
		
		return 0;
	}
	
	return -1;
}

int controller_send_bye() 
{
	chatclient *client;
	servent_data *tmp, *peer, *sd;
	char *ret;
	if(servent_get_local() == NULL) {
		logger(CTRL_INFO,"[controller_send_bye] local_servent NULL\n");
	    return -1;
	}
	  
	logger(CTRL_INFO,"[controller_send_bye] sending packet\n");
	
	GList *users = servent_get_values();
	int i=0;
	for(; i < g_list_length(users); i++) {
		peer = g_list_nth_data(users, i);
		logger(CTRL_INFO,"[controller_send_bye] sending packet to %lld from %lld\n", peer->id, servent_get_local()->id);
		if(peer!=NULL && peer->id!=servent_get_local()->id) {
			COPY_SERVENT(peer, tmp);
			tmp->post_type = BYE_ID;
			logger(CTRL_INFO, "[controller_send_bye] checking post type %d\n", tmp->post_type);
			tmp->packet_id = generate_id();
			servent_send_packet(tmp);
			logger(CTRL_INFO, "[controller_send_bye] sent\n");
			
		}
	}
	
	for(i=0; i<g_list_length(users); i++) {
		client = (chatclient*)g_list_nth_data(users, i);
		if(client!=NULL) {
			peer = servent_get(client->id);
			logger(CTRL_INFO, "[controller_send_bye]pop response %lld\n", client->id);
			if(peer!=NULL && peer->id!=servent_get_local()->id) {
				RLOCK(peer->id);
				COPY_SERVENT(peer, sd);
				UNLOCK(peer->id);
				
				//while(strcmp(ret,TIMEOUT)!=0) {
				ret = servent_pop_response(sd);
				logger(CTRL_INFO, "[controller_send_bye]ret: %s\n", ret);
				if(strcmp(ret, TIMEOUT)==0) {
					logger(CTRL_INFO, "[controller_send_bye]TIMEOUT\n");
					return sd->id;
				}
				//}
				logger(CTRL_INFO, "[controller_send_bye]RECEIVED OK %s\n", ret);
				//data_del_user_from_chat(chat_id, client->id);
				//remove_user_from_chat_list(chat_id, client->id);
				//data_add_user_to_chat(chat_elem->id, client->id, client->nick, client->ip, client->port);
				//add_user_to_chat_list(chat_elem->id, client->id, client->nick, peer->status);
			}
		}
	}
	
	return 0;
}

int controller_receive_bye(u_int8 id)
{
   pm_data *pm;
   logger(CTRL_INFO, "[controller_receive_bye] id utente %lld\n", id);
   if((pm = gui_pm_data_get(id)) != NULL) {
   	gui_leave_pm_event(pm->window, (gpointer)to_string(id)); //QUI BISOGNA PASSARE I PARAMETRI IN MODO CORRETTO?
   }
   return 0;
}

int controller_request_list(u_int8 chat_id) //PROVA
{
	GList *servents = servent_get_values();
	logger(CTRL_INFO, "[controller_request_list]LIST\n");
	if(servents==NULL) {
		logger(CTRL_INFO, "[controller_request_list]Servents null\n");
		return 0;
	}
	servent_data *servent, *tmp;
	int i=0;
	for(; i<g_list_length(servents); i++) {
		servent = g_list_nth_data(servents, i);
		logger(CTRL_INFO, "[controller_request_list]Servent ID: %lld\n", servent->id);
		if(servent->id!=servent_get_local()->id && servent->id >= conf_get_gen_start ()) {
			
			if(servent->queue==NULL) {
				logger(CTRL_INFO, "[controller_request_list]Coda Servent NULL\n");
				continue;
			}
			RLOCK(servent->id);
			logger(INFO, "[controller_request_list]Copy servent\n");
			COPY_SERVENT(servent, tmp);
			UNLOCK(servent->id);
			if(tmp->queue==NULL || tmp->res_queue==NULL) {
				logger(CTRL_INFO, "[controller_request_list]Coda NULL\n");
				continue;
			}
			tmp->post_type = LIST_ID;
			tmp->chat_id_req = chat_id;
			tmp->ttl = 3;
			tmp->hops = 0;
			tmp->packet_id = generate_id(); //Se non ci fosse verrebbe riutilizzato l'ID di uno degli eventuali pacchetti SEARCH ritrasmessi
			logger(INFO, "[controller_request_list]Send\n");
			servent_send_packet(tmp);
			logger(INFO, "[controller_request_list]Sent\n");
		}
		else
			logger(INFO, "[controller_request_list]Local ID\n");
		
	}
	//Da gestire i timeout
	
	char *ret;
	for(i=0; i<g_list_length(servents); i++) {
		servent = g_list_nth_data(servents, i);
		if(servent->id!=servent_get_local()->id) {
			ret = servent_pop_response(servent);
			if(strcmp(ret, TIMEOUT)==0) {
				logger(CTRL_INFO, "[controller_request_list]TIMEOUT\n");
				return servent->id;
			}
			printf("RECEIVED %s\n", ret);
		}
		else
			logger(INFO, "[controller_request_list]Local response");
		
	}
	logger(INFO, "[controller_request_list]End");
	
	return 0;
}

int controller_init(const char *filename, const char *cache) {
	
	conf_read(filename);
	
	logger_init(8);
	
	GList *init_list = NULL;
	logger(INFO, "[controller_init]Init\n");
	init_list = init_read_file(cache);
	
	servent_start(init_list);

	//servent_start_timer();
	
	return 0;
}

int controller_exit() {
	kill_all_thread(0);
	
	logger_close();
	return 0;
}

int controller_init_gui(void) {

	/*-- Declare the GTK Widgets used in the program --*/
	GtkWidget *window;
	GtkWidget *menu;
	GtkWidget *vbox;
	GtkWidget *handlebox;  
	GtkWidget *list;
	GtkWidget *searchbar;

	/*-- Create the new window --*/
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	/*-- Create the vbox --*/
	vbox = gtk_vbox_new(FALSE, 5);
	list = gui_create_chat_list(0);
	/*-- Create the handlebox --*/
	handlebox = gtk_handle_box_new();

	/*-- Create the menu bar --*/
	menu = gui_create_menu();

	/*-- Connect the window to the gui_close_event function  --*/
	gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(gui_close_event), NULL);

	/*-- Add the menubar to the handlebox --*/
	gtk_container_add(GTK_CONTAINER(handlebox), menu);

	/*-- Pack the handlebox into the top of the vbox.  
	 *-- You must use gtk_box_pack_start here instead of gtk_container_add
	 *-- or the menu will get larger as the window is enlarged
	 */

	searchbar = gui_create_searchbar();

	gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), searchbar, FALSE, TRUE, 5);

	gtk_container_add(GTK_CONTAINER(vbox),list);

	//gtk_box_pack_start(GTK_BOX(vbox), searchbar, FALSE, TRUE, 0);  

	/*-- Add the vbox to the main window --*/
	gtk_container_add(GTK_CONTAINER(window), vbox);


	/*-- Set window border to zero so that text area takes up the whole window --*/
	gtk_container_border_width (GTK_CONTAINER (window), 0);

	/*-- Set the window to be 640 x 480 pixels --*/
	gtk_window_set_default_size (GTK_WINDOW(window), 300, 400);

	/*-- Set the window title --*/
	gtk_window_set_title(GTK_WINDOW (window), "Lista Chat");


	/*-- Display the widgets --*/
//	gtk_widget_show(handlebox);
//	gtk_widget_show(vbox);
//	//gtk_widget_show(menuitem);
//	gtk_widget_show(searchbar);   
//	gtk_widget_show(menu);
	gtk_widget_show_all(window);
	//gtk_widget_show(list);

	/*-- Start the GTK event loop --*/
	
	//pthread_create(&gtk_main_thread, NULL, controller_start_main_thread, NULL);
	gtk_main();
	return 1;
}

void *controller_start_main_thread(void *unused) {
	gtk_main();
	printf("ok");
	return NULL;
}

int controller_menu() {
	servent_data *servent;
	GList *clients;
	GList *keys;
	
	while(getchar()!='e') {
		logger(INFO, "[controller_menu]Sblocked\n");
		
		clients = servent_get_values();
		keys = servent_get_keys();
		
		int j;
		logger(INFO, "[controller_menu]clients size: %d\n", g_list_length(clients));
		for(j=0; j<g_list_length(clients); j++) {
			logger(INFO, "[controller_menu]servent->id\n");
			servent = (servent_data*)g_list_nth_data(clients, j);
			if(servent->id!=servent_get_local()->id) {
				break;
			}
		}
	
		logger(INFO, "[controller_menu]after\n");
	}
	
	servent->chat_id_req = 123;

	servent->msg_len = 5;
	servent->msg = "Hello";

	servent->title_len =5;
	servent->title = "linux";

	servent->ttl=3;
	servent->hops=0;

	int ch = 'b';

	while((ch=getchar())!='q') {
		if(ch=='\n')
			continue;
		if(ch=='j')
			servent->post_type=JOIN_ID;
		else if(ch=='p')
			servent->post_type=PING_ID;
		else if(ch=='m') {
			char *text = calloc(100000, 1);
			scanf("%s", text);
			logger(INFO, "[servent_start]msg sent: %s\n", text);
			servent->msg = text;
			servent->msg_len = strlen(text);
			servent->post_type=MESSAGE_ID;
		}
		else if(ch=='s') {
			timer_thread = (pthread_t*)malloc(sizeof(pthread_t));
			pthread_create(timer_thread, NULL, servent_timer, NULL);
		}
		else if(ch=='c') {
			//				servent->post_type=CREATE_ID;
			u_int8 chat_id;
			char *title = calloc(20, 1);
			printf("Inserire ID chat da creare: ");
			scanf("%lld", &chat_id);
			printf("Inserire titolo chat da creare: ");
			scanf("%s", title);
			data_add_chat(chat_id, title);

			chat *test = (chat*)g_hash_table_lookup(chat_hashtable, (gconstpointer)to_string(chat_id));
			printf("chat created with ID: %lld\n", test->id);
		}
		else if(ch=='r') {
			char *title = calloc(20, 1);
			printf("Inserire titolo chat da ricercare: ");
			scanf("%s", title);
			servent->title = title;
			servent->title_len = strlen(title);
			servent->post_type=SEARCH_ID;
		}
		else if(ch=='e') {
			GList *chats = g_hash_table_get_values(chat_hashtable);
			int len = g_list_length(chats);
			int i;
			for(i=0; i<len; i++) {
				chat *elem = (chat*)g_list_nth_data(chats, i);
				printf("[servent_start]ID: %lld, title: %s\n", elem->id, elem->title);
			}
			continue;
		}
		else if(ch=='l') {
			int i;
			clients = servent_get_values();
			keys = servent_get_keys();
			for(i=0; i<g_list_length(clients); i++) {
				servent_data *ser = (servent_data*)g_list_nth_data(clients, i);
				printf("[servent_start]local_servent->id: %lld\n", servent_get_local()->id);
				if(ser->id!=servent_get_local()->id) {
					printf("[servent_start]client[%s]: %lld, ip: %s:%d\n", (char*)g_list_nth_data(keys, i), ser->id, ser->ip, ser->port);
				}
			}
			continue;
		}

		printf("[servent_start]signal\n");
		pthread_cond_signal(&servent->cond);
		printf("[servent_start]Premere f per bloccare l'invio\n");

	}
	
	return 0;
}

u_int8 controller_search(const char *query) {
	if(query==NULL || strcmp(query, "")==0) {
		logger(CTRL_INFO, "[controller_search]Stringa di query inaccettabile\n");
		return 0;
	}
	
	GList *servents = servent_get_values();
	logger(CTRL_INFO, "[controller_search]Query: %s\n", query);
	if(servents==NULL) {
		logger(CTRL_INFO, "[controller_search]Servents null\n");
		return 0;
	}
	servent_data *servent, *tmp;
	int i=0;
	for(; i<g_list_length(servents); i++) {
		servent = g_list_nth_data(servents, i);
		logger(CTRL_INFO, "[controller_search]Servent ID: %lld\n", servent->id);
		if(servent->id!=servent_get_local()->id && servent->id >= conf_get_gen_start ()) {
			
			if(servent->queue==NULL) {
				logger(CTRL_INFO, "[controller_search]Coda Servent NULL\n");
				continue;
			}
			RLOCK(servent->id);
			logger(INFO, "[controller_search]Copy servent\n");
			COPY_SERVENT(servent, tmp);
			UNLOCK(servent->id);
			if(tmp->queue==NULL || tmp->res_queue==NULL) {
				logger(CTRL_INFO, "[controller_search]Coda NULL\n");
				continue;
			}
			logger(INFO, "[controller_search]Copy\n");
			tmp->post_type = SEARCH_ID;
			tmp->title = strdup(query);
			tmp->title_len = strlen(query);
			tmp->ttl = 3;
			tmp->hops = 0;
			tmp->packet_id = generate_id(); //Se non ci fosse verrebbe riutilizzato l'ID di uno degli eventuali pacchetti SEARCH ritrasmessi
			logger(INFO, "[controller_search]Send\n");
			servent_send_packet(tmp);
			logger(INFO, "[controller_search]Sent\n");
		}
		else
			logger(INFO, "[controller_search]Local ID\n");
		
	}
	//Da gestire i timeout
	
	char *ret;
	for(i=0; i<g_list_length(servents); i++) {
		servent = g_list_nth_data(servents, i);
		if(servent->id!=servent_get_local()->id) {
			ret = servent_pop_response(servent);
			if(strcmp(ret, TIMEOUT)==0) {
				logger(CTRL_INFO, "[controller_search]TIMEOUT\n");
				return servent->id;
			}
			printf("RECEIVED %s\n", ret);
		}
		else
			logger(INFO, "[controller_search]Local response");
		
	}
	logger(INFO, "[controller_search]End");
	
	return 0;
}

int controller_create(const char *title) {
	if(title==NULL || strcmp(title, "")==0) {
		logger(CTRL_INFO, "[controller_create]Titolo non valido\n");
		return -1;
	}
	
	u_int8 chat_id = generate_id();
	data_add_chat(chat_id, title);
	
	servent_data *local = servent_get_local();
	data_add_user_to_chat(chat_id, local->id, local->nick, local->ip, local->port);
	chat *test = data_get_chat(chat_id);
	local->chat_list = g_list_append(local->chat_list, (gpointer)test);
	printf("chat created with ID: %lld\n", test->id);
	
	gui_open_chatroom(chat_id);
	
	data_add_existing_user_to_chat(chat_id, servent_get_local()->id);
	gui_add_user_to_chat(chat_id, local->id, local->nick, local->status);
	return 0;
}

int controller_add_user_to_chat(u_int8 chat_id, u_int8 id) {
	
	servent_data *servent = servent_get(id);
	logger(CTRL_INFO, "[controller_add_user_to_chat]Addingi user: %s, id: %lld, status: %c\n", servent->nick, servent->id, servent->status);
	if(servent->status == ONLINE_ID)
		logger(CTRL_INFO,"[controller_add_user_to_chat] ONLINE\n");
	else if(servent->status == AWAY_ID)
		logger(CTRL_INFO, "[controller_add_user_to_chat] AWAY\n");
	else if(servent->status == BUSY_ID)
		logger(CTRL_INFO, "[controller_add_user_to_chat] BUSY\n");
	gui_add_user_to_chat(chat_id, servent->id, servent->nick, servent->status);
	
	return 0;
}

int controller_rem_user_from_chat(u_int8 chat_id, u_int8 id) {
	gui_del_user_from_chat(chat_id, id);
	data_del_user_from_chat(chat_id, id);
	return 0;
}

int controller_add_msg_to_chat(u_int8 chat_id, char *msg) {
	
	if(msg==NULL) {
		logger(CTRL_INFO, "[controller_add_msg_to_chat]Messaggio non valido\n");
		return -1;
	}
	
	if(chat_id>0) {
		if(gui_add_msg_to_chat(chat_id, msg)<0) {
			logger(CTRL_INFO, "[controller_add_msg_to_chat]Msg error\n");
			return -2;
		}
		return 0;
	}

	return -3;
}

int controller_add_msg(u_int8 sender_id, char *msg) {
	
	if(msg==NULL) {
		logger(CTRL_INFO, "[controller_add_msg]Messaggio non valido\n");
		return -1;
	}
	
	if(sender_id>0) {
		if(gui_add_msg_pm(sender_id, msg)<0) {
			logger(CTRL_INFO, "[controller_add_msg]Msg pm error\n");
			return -2;
		}
		return 0;
	}
	return -3;
}