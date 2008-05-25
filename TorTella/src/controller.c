#include "controller.h"

int controller_send_chat_users(u_int8 chat_id, u_int4 msg_len, char *msg) {
	if(chat_id != 0) {
		chat *chat_elem = (chat*)g_hash_table_lookup(chat_hashtable,(gconstpointer)to_string(chat_id));
		GList *users = g_hash_table_get_values(chat_elem->users);

		int i=0;
		for(; i<g_list_length(users); i++) {

			chatclient *user = g_list_nth_data(users, i);

			servent_data *data = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(user->id));

			WLOCK(data->id);

			data->msg = strdup(msg);
			data->msg_len = msg_len;
			data->chat_id_req = chat_id;
			data->post_type = MESSAGE_ID;

			UNLOCK(data->id);

			pthread_cond_signal(&data->cond);
		}
		return 0;
	}
	return -1;
}

int controller_send_subset_users(u_int8 chat_id, u_int4 msg_len, char *msg, GList *users) { 
	if(chat_id != 0) {
		int i= 0;
		for(; i<g_list_length(users); i++) {

			chatclient *user = g_list_nth_data(users, i);

			servent_data *data = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(user->id));

			WLOCK(data->id);

			data->msg = strdup(msg);
			data->msg_len = msg_len;
			data->chat_id_req = chat_id;
			data->post_type = MESSAGE_ID;

			UNLOCK(data->id);

			pthread_cond_signal(&data->cond);
		}
		return 0;
	}
	return -1;
}

int controller_send_pm(u_int4 msg_len, char *msg, u_int8 recv_id) {
	servent_data *data = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(recv_id));

	WLOCK(data->id);

	data->msg = strdup(msg);
	data->msg_len = msg_len;
	data->chat_id_req = 0;
	data->post_type = MESSAGE_ID;

	UNLOCK(data->id);

	pthread_cond_signal(&data->cond);
	return 0;
}

int controller_join_chat(u_int8 chat_id) {
	if(chat_id>0) {
		chat *chat_elem = (chat*)g_hash_table_lookup(chat_hashtable, (gconstpointer)to_string(chat_id));
		if(chat_elem!=NULL) {
			GList *clients = g_hash_table_get_values(chat_elem->users);
			chatclient *client;
			servent_data *peer;
			int i;
			for(i=0; i<g_list_length(clients); i++) {
				client = (chatclient*)g_list_nth_data(clients, i);
				if(client!=NULL) {
					peer = servent_get(client->id);
					if(peer!=NULL) {
						WLOCK(peer->id);
						peer->chat_id_req = chat_id;
						peer->post_type = JOIN_ID;
						UNLOCK(peer->id);
						pthread_cond_signal(&peer->cond);
					}
				}
			}
			return 0;
		}
	}
	return -1;
}

int controller_leave_chat(u_int8 chat_id) {
	if(chat_id>0) {
		chat *chat_elem = (chat*)g_hash_table_lookup(chat_hashtable, (gconstpointer)to_string(chat_id));
		if(chat_elem!=NULL) {
			GList *clients = g_hash_table_get_values(chat_elem->users);
			chatclient *client;
			servent_data *peer;
			int i;
			for(i=0; i<g_list_length(clients); i++) {
				client = (chatclient*)g_list_nth_data(clients, i);
				if(client!=NULL) {
					peer = servent_get(client->id);
					if(peer!=NULL) {
						WLOCK(peer->id);
						peer->chat_id_req = chat_id;
						peer->post_type = LEAVE_ID;
						UNLOCK(peer->id);
						pthread_cond_signal(&peer->cond);
					}
				}
			}
			return 0;
		}
	}
	return -1;
}

int controller_connect_users(GList *users) {
	if(users!=NULL) {
		int i;
		chatclient *client;
		for(i=0; i<g_list_length(users); i++) {
			client = (chatclient*)g_list_nth_data(users, i);
			servent_start_client(client->ip, client->port);
		}
		return 0;
	}
	
	return -1;
}

int controller_init(const char *filename, const char *cache) {
	
	logger_init(8, "tortella");
	
	GList *init_list = NULL;
	logger(INFO, "[controller_init]Init\n");
	init_list = init_read_file(cache);
	
	conf_read(filename);

	servent_start(init_list);

	servent_start_timer();
	
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
	list = create_chat_list(0);
	/*-- Create the handlebox --*/
	handlebox = gtk_handle_box_new();

	/*-- Create the menu bar --*/
	menu = create_menu();

	/*-- Connect the window to the destroyapp function  --*/
	gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(destroyapp), NULL);



	/*-- Add the menubar to the handlebox --*/
	gtk_container_add(GTK_CONTAINER(handlebox), menu);

	/*-- Pack the handlebox into the top of the vbox.  
	 *-- You must use gtk_box_pack_start here instead of gtk_container_add
	 *-- or the menu will get larger as the window is enlarged
	 */

	searchbar = create_searchbar();

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
	gtk_widget_show(handlebox);
	gtk_widget_show(vbox);
	//gtk_widget_show(menuitem);
	gtk_widget_show(searchbar);   
	gtk_widget_show(menu);   
	gtk_widget_show(window);
	gtk_widget_show(list);

	/*-- Start the GTK event loop --*/
	
	pthread_create(&gtk_main_thread, NULL, controller_start_main_thread, NULL);
	//gtk_main();
	return 1;
}

void *controller_start_main_thread(void *unused) {
	gtk_main();
	printf("ok");
	return;
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
		else if(ch=='o')
			servent->post_type=PONG_ID;
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
			add_chat(chat_id, title, &chat_hashtable);

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

int controller_search(const char *query) {
	if(query==NULL || strcmp(query, "")==0) {
		logger(CTRL_INFO, "[controller_init]Stringa di query inaccettabile\n");
		return -1;
	}
	
	GList *servents = servent_get_values();
	logger(CTRL_INFO, "[controller_init]Get values\n");
	if(servents==NULL)
		return -2;
	servent_data *servent;
	int i=0;
	for(; i<g_list_length(servents); i++) {
		servent = g_list_nth_data(servents, i);
		WLOCK(servent->id);
		servent->post_type=SEARCH_ID;
		servent->title = strdup(query);
		servent->title_len = strlen(query);
		pthread_cond_signal(&servent->cond);
		UNLOCK(servent->id);
	}
	
	return 0;
}
