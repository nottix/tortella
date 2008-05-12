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
	
	return 0;
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
