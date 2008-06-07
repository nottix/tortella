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
 
#include "servent.h"

//Crea un server socket
int servent_create_server(char *src_ip, u_int4 src_port) {
	logger(SOCK_INFO, "[servent_create_server]Creating listen tcp socket on: %s:%d\n", src_ip, src_port);
	return create_listen_tcp_socket(src_ip, src_port);
}

//Crea un client socket
int servent_create_client(char *dst_ip, u_int4 dst_port) {
	logger(SOCK_INFO, "[servent_create_client]Creating tcp socket on: %s:%d\n", dst_ip, dst_port);
	return create_tcp_socket(dst_ip, dst_port);
}

int servent_start_server(char *local_ip, u_int4 local_port) {
	int serfd = servent_create_server(local_ip, local_port); //Locale
	if(serfd<0) {
		logger(ALARM_INFO, "[servent_start]Errore nella creazione del server\n");
		return -1;
	}
	pthread_t *serthread = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(serthread, NULL, servent_listen, (void*)serfd);
	
	server_fd = g_slist_prepend(server_fd, (gpointer)serfd);
	server_thread = g_slist_prepend(server_thread, (gpointer)serthread);
	
	return 0;
}

int servent_start_client(char *dest_ip, u_int4 dest_port) {
	pthread_t *clithread = (pthread_t*)calloc(1, sizeof(pthread_t));
	u_int8 cliid = new_connection_counter++;
	
	servent_data *servent = (servent_data*)calloc(1, sizeof(servent_data));
	servent->id = cliid;
	logger(SYS_INFO, "[servent_start_client]cliid: %lld\n", cliid);
	servent->ip = dest_ip;
	servent->port = dest_port;
	
	pthread_mutex_init(&servent->mutex, NULL);
	pthread_rwlock_init(&servent->rwlock_data, NULL);
	pthread_cond_init(&servent->cond, NULL);

	servent->post_type=PING_ID;
	g_hash_table_insert(servent_hashtable, (gpointer)to_string(cliid), (gpointer)servent);
	pthread_create(clithread, NULL, servent_connect, (void*)&cliid);
	client_thread = g_slist_prepend(client_thread, (gpointer)(*clithread));
	
	return 0;
}

int servent_start(GList *init_servent) {
	//Inizializzazione servent locale
	if(servent_init(conf_get_local_ip(), conf_get_local_port(), ONLINE_ID)<0) {
		logger(SYS_INFO, "[servent_start]Errore inizializzazione dati\n");
		return -1;
	}
	
	//Avvio server di ascolto richieste
	if(servent_start_server(conf_get_local_ip(), conf_get_local_port())<0) {
		logger(SYS_INFO, "[servent_start]Errore nell'avvio del server\n");
		return -2;
	}
	
	//Fase iniziale di reperimento degli utenti iniziali
	if(init_servent!=NULL) {
		if(servent_init_connection(init_servent)<0) {
			logger(SYS_INFO, "[servent_start]Errore nella connessione iniziale\n");
			return -3;
		}
	}

	return 0;
}

int servent_start_timer(void) {
	timer_thread = (pthread_t*)calloc(1, sizeof(pthread_t));
	pthread_create(timer_thread, NULL, servent_timer, NULL);
	return 0;
}

int servent_init_connection(GList *init_servent) {
	int i;
	init_data *peer;
	for(i=0; i<g_list_length(init_servent); i++) {
		peer = (init_data*)g_list_nth_data(init_servent, i);
		logger(SOCK_INFO, "[servent_init_connection]ip: %s, port: %d\n", peer->ip, peer->port);
		if(servent_start_client(peer->ip, peer->port)<0) {
			logger(SYS_INFO, "[servent_init_connection]Errore connessione a peer conosciuti\n");
			return -1;
		}
	}
	
	return 0;
}

void servent_close_all(void) {
	
	int i;
	for(i=0; i<g_slist_length(server_fd); i++) {
		logger(SYS_INFO, "[servent_close_all]Closing server %d\n", (int)g_slist_nth_data(server_fd, i));
		delete_socket((int)g_slist_nth_data(server_fd, i));
	}
	for(i=0; i<g_slist_length(client_fd); i++) {
		logger(SYS_INFO, "[servent_close_all]Closing client %d\n", (int)g_slist_nth_data(client_fd, i));
		delete_socket((int)g_slist_nth_data(client_fd, i));
	}
	for(i=0; i<g_slist_length(server_connection_fd); i++) {
		logger(SYS_INFO, "[servent_close_all]Closing server_connection %d\n", (int)g_slist_nth_data(server_connection_fd, i));
		delete_socket((int)g_slist_nth_data(server_connection_fd, i));
	}
	
}

void kill_all_thread(int sig) {
	logger(SYS_INFO, "[kill_all_thread]Killing thread\n");
	servent_close_supernode();
	logger(SYS_INFO, "[kill_all_thread]Closing supernode\n");
	servent_close_all();

	int i;
	for(i=0; i<g_slist_length(server_thread); i++) {
		pthread_kill((pthread_t)g_slist_nth_data(server_thread, i), SIGKILL);
	}
	for(i=0; i<g_slist_length(client_thread); i++) {
		pthread_kill((pthread_t)g_slist_nth_data(client_thread, i), SIGKILL);
	}
	for(i=0; i<g_slist_length(server_connection_thread); i++) {
		pthread_kill((pthread_t)g_slist_nth_data(server_connection_thread, i), SIGKILL);
	}
	
	if(timer_thread!=NULL)
		pthread_kill(*timer_thread, SIGKILL);
}

int servent_init(char *ip, u_int4 port, u_int1 status) {
	
	//Inizializza la lista delle chat conosciute leggendo da un file predefinito
	servent_init_supernode();
	logger(SYS_INFO, "[servent_init]Supernode initialized on %s:%d\n", conf_get_local_ip(), conf_get_local_port());
	
	servent_hashtable = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);
	route_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	search_packet_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	
	local_servent = (servent_data*)calloc(1, sizeof(servent_data));
	u_int8 id = local_servent->id = generate_id();
	logger(SYS_INFO, "[servent_init]ID: %lld\n", local_servent->id);
	local_servent->ip = ip;
	local_servent->port = port;
	local_servent->queue = g_queue_new();
	local_servent->res_queue = g_queue_new();
	local_servent->status = status;
	local_servent->nick = conf_get_nick();
	
	add_user(local_servent->id, local_servent->nick, local_servent->ip, local_servent->port);
	
	pthread_mutex_init(&local_servent->mutex, NULL);
	pthread_rwlock_init(&local_servent->rwlock_data, NULL);
	pthread_cond_init(&local_servent->cond, NULL);
	
	g_hash_table_insert(servent_hashtable, (gpointer)to_string(id), (gpointer)local_servent);
	
	server_fd = NULL;
	client_fd = NULL;
	server_connection_fd = NULL;
	
	server_thread = NULL;
	client_thread = NULL;
	server_connection_thread = NULL;
	
	timer_thread = NULL;
	
	return 0;
}

void servent_init_supernode() {
	//read_all();
}

void servent_close_supernode() {
	//write_all(MODE_TRUNC);
}

servent_data *servent_get(u_int8 id) {
	return (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id));
}

GList *servent_get_values(void) {
	return g_hash_table_get_values(servent_hashtable);
}

GList *servent_get_keys(void) {
	return g_hash_table_get_keys(servent_hashtable);
}

servent_data *servent_get_local(void) {
	return local_servent;
}

void servent_send_packet(servent_data *sd) {
	g_queue_push_tail(sd->queue, (gpointer)sd);
}

servent_data *servent_pop_queue(servent_data *sd) {
	servent_data *servent;
	if(sd==NULL || sd->queue==NULL) {
		logger(SYS_INFO, "[servent_pop_queue]NULL\n");
		return NULL;
	}
	while((servent = (servent_data*)g_queue_pop_head(sd->queue))==NULL) {
		usleep(10000);
	}
	return servent;
}

void servent_append_response(servent_data *sd, const char *response) {
	g_queue_push_tail(sd->res_queue,(gpointer)strdup(response));
}

char *servent_pop_response(servent_data *sd) {
	char *buf;
	if(sd==NULL || sd->res_queue==NULL) {
		logger(SYS_INFO, "[servent_append_responde]Response queue NULL\n");
		return NULL;
	}
	
	int counter = 0;
	while((buf = (char*)g_queue_pop_head(sd->res_queue))==NULL) {
		if(counter>10) //Serve per il timeout
			break;
		usleep(200000);
		counter++;
	}
	return buf;
}

char *get_search_packet(u_int8 id) {
	return (char*)g_hash_table_lookup(search_packet_hashtable,(gconstpointer)to_string(id));
}

void new_search_packet(u_int8 id) {
	//if(search_packet_hashtable == NULL) {
	//	search_packet_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	//}
	g_hash_table_insert(search_packet_hashtable,(gpointer)to_string(id),(gpointer)to_string(id));
}

//---------THREAD---------------

//Thread che riceve le richieste di connessione e avvia nuovi thread
/* Ogni nuovo peer (client) che richiede di connettersi al server locale viene
 * assegnato ad un nuovo Thread che si occupera' di rispondere alle richieste del client.
 */
void *servent_listen(void *parm) {
	int connFd;
	pthread_t *thread;
	
	while(1) {
		connFd = listen_http_packet((int)parm);
		printf("[servent_listen]Connessione ricevuta, socket: %d\n", connFd);
		if(connFd!=0) {
			thread = (pthread_t*)calloc(1, sizeof(pthread_t));
			server_connection_fd = g_slist_prepend(server_connection_fd, (gpointer)connFd);
			
			pthread_create(thread, NULL, servent_responde, (void*)connFd);
			server_connection_thread = g_slist_prepend(server_connection_thread, (gpointer)(*thread));
			
		}
	}
	
	pthread_exit(NULL);
}

//Riceve i pacchetti da un peer e li gestisce
//parm: socket
void *servent_responde(void *parm) {
	printf("[servent_responde]Server initialized\n");
	char *buffer;
	http_packet *h_packet;
	int len;
	int fd = (int)parm;
	u_int4 status = 0;
	
	while(1) {
		logger(SYS_INFO, "[servent_responde]Waiting\n");
		len = recv_http_packet(fd, &buffer);
		printf("[servent_responde]Data received len: %d, buffer: \nSTART\n%s\nEND\n", len, dump_data(buffer, len));
		
		if(len>0) {
			printf("[servent_responde]Converting\n");
			h_packet = http_char_to_bin((const char*)buffer);
			if(h_packet!=NULL) {
				printf("[servent_responde]http packet received, type=%d\n", h_packet->type);
				if(h_packet->type==HTTP_REQ_POST) {
					
					printf("[servent_responde]POST ricevuto\n");
					
					//Aggiunge le info in base al tipo di pacchetto
					if(h_packet->data==NULL || h_packet->data->header==NULL) {
						printf("[servent_responde]NULL\n");
						
						status = HTTP_STATUS_CERROR;
					}
					else if(h_packet->data->header->recv_id!=local_servent->id && h_packet->data->header->recv_id>=conf_get_gen_start()) {
						status = HTTP_STATUS_CERROR;
					}
					else if(h_packet->data->header->desc_id==JOIN_ID) {
						printf("[servent_responde]JOIN ricevuto\n");
						u_int8 id = h_packet->data->header->sender_id;
						
						servent_data *conn_servent = g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id));
						
						WLOCK(id);
						conn_servent->timestamp = h_packet->data->header->timestamp;
						UNLOCK(id);
						add_exist_user_to_chat(GET_JOIN(h_packet->data)->chat_id, id);
						controller_add_user_to_chat(GET_JOIN(h_packet->data)->chat_id, id);
						
						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==PING_ID) {
						printf("[servent_responde]PING ricevuto da %lld a %lld\n", h_packet->data->header->sender_id, h_packet->data->header->recv_id);
						
						servent_data *conn_servent;
						u_int8 id = h_packet->data->header->sender_id;
						logger(SYS_INFO, "[servent_responde]Searching in hashtable: %lld\n", id);
						if((conn_servent=g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id)))!=NULL) {
							logger(SYS_INFO, "[servent_responde]Found: %lld\n", id);
							WLOCK(id);
							conn_servent->status = GET_PING(h_packet->data)->status;
							conn_servent->timestamp = h_packet->data->header->timestamp;
							conn_servent->nick = h_packet->data->data;
							UNLOCK(id);
							add_user(conn_servent->id, conn_servent->nick, conn_servent->ip, conn_servent->port);
							printf("[servent_responde]Old PING, nick: %s, status: %c\n", conn_servent->nick, conn_servent->status);
							controller_manipulating_status(id, conn_servent->status);
							status = HTTP_STATUS_OK;
						}
						else {
							printf("[servent_responde]New PING\n");
							
							if(h_packet->data->header->recv_id < conf_get_gen_start()) {
								status = HTTP_STATUS_OK;
								
								conn_servent = (servent_data*)calloc(1, sizeof(servent_data));
								conn_servent->ip = get_dest_ip(fd);
								conn_servent->port = GET_PING(h_packet->data)->port;
								conn_servent->timestamp = h_packet->data->header->timestamp;
								conn_servent->queue = g_queue_new();
								conn_servent->res_queue = g_queue_new();
								
								conn_servent->status = GET_PING(h_packet->data)->status;
								logger(SYS_INFO, "[servent_responde]Status recv: %c\n", conn_servent->status);
								conn_servent->nick = h_packet->data->data;
								conn_servent->id = h_packet->data->header->sender_id;
								
								add_user(conn_servent->id, conn_servent->nick, conn_servent->ip, conn_servent->port);
								
								//Si inizializzano il mutex e il cond
								pthread_mutex_init(&conn_servent->mutex, NULL);
								pthread_rwlock_init(&conn_servent->rwlock_data, NULL);
								pthread_cond_init(&conn_servent->cond, NULL);
								
								printf("[servent_responde]Lookup ID: %s\n", to_string(id));
								if(g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id))==NULL) {
									printf("[servent_responde]connection %s added to hashtable\n", to_string(id));
									g_hash_table_insert(servent_hashtable, (gpointer)to_string(id), (gpointer)conn_servent);
								}
								
								//conn_servent->post_type = PING_ID;
								//RLOCK(local_servent->id);
							
								//conn_servent->status = local_servent->status;
								//UNLOCK(local_servent->id);
								
								pthread_t *cli_thread = (pthread_t*)malloc(sizeof(pthread_t));
								pthread_create(cli_thread, NULL, servent_connect, (void*)&id);
								client_thread = g_slist_prepend(client_thread, (gpointer)(*cli_thread));
							}
							else {
								status = HTTP_STATUS_OK;
								GList *users = servent_get_values ();
								int i=0;
								logger(SYS_INFO, "[servent_responde]Changing ID\n");
								for(;i<g_list_length(users);i++) {
									servent_data *tmp = (servent_data*)g_list_nth_data(users,i);
									
									char *nick = tmp->nick;
									char *tmp_ip = tmp->ip;
									u_int4 tmp_port = tmp->port;
									
									logger(SYS_INFO, "[servent_responde]old ID: %lld, new ID: %lld\n", tmp->id, h_packet->data->header->sender_id);
									logger(SYS_INFO, "[servent_responde]nick: %s, ip: %s, port: %d\n", nick, get_dest_ip(fd), GET_PING(h_packet->data)->port);
									if((strcmp(tmp_ip, get_dest_ip(fd))==0) && (tmp_port == GET_PING(h_packet->data)->port)) {
										logger(SYS_INFO, "[servent_responde]Changing old ID: %lld with new ID: %lld\n", tmp->id, h_packet->data->header->sender_id);
										tmp->id = h_packet->data->header->sender_id;
										tmp->status = GET_PING(h_packet->data)->status;
										tmp->timestamp = h_packet->data->header->timestamp;
										tmp->nick = h_packet->data->data;
										add_user(tmp->id, tmp->nick, tmp->ip, tmp->port);
										
										//g_hash_table_remove(servent_hashtable, (gpointer)tmp); TODO: meglio toglierlo per evitare segfault
										g_hash_table_insert(servent_hashtable, (gpointer)to_string(tmp->id),(gpointer)tmp);
									}
								}
							}
							
							
						}
					}
					else if(h_packet->data->header->desc_id==LEAVE_ID) {
						printf("[servent_responde]LEAVE ricevuto\n");
						u_int8 chat_id = GET_LEAVE(h_packet->data)->chat_id;
						
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						WLOCK(h_packet->data->header->sender_id);
						//conn_servent->chat_list = g_list_remove(conn_servent->chat_list, (gconstpointer)&chat_id); //TODO: non ci vorrebbe
						conn_servent->timestamp = h_packet->data->header->timestamp;
						UNLOCK(h_packet->data->header->sender_id);
						
						status = HTTP_STATUS_OK;
						//Sconnetti dalla chat
						logger(SYS_INFO, "[servent_responde]Deleting user\n");
						controller_rem_user_from_chat(chat_id, conn_servent->id);
						logger(SYS_INFO, "[servent_responde]Deleted user: %lld\n", conn_servent->id);
					}
					else if(h_packet->data->header->desc_id==MESSAGE_ID) {
						printf("[servent_responde]MESSAGE ricevuto\n");
						
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));

						WLOCK(h_packet->data->header->sender_id);
						conn_servent->msg = h_packet->data->data;
						conn_servent->msg_len = h_packet->data->header->data_len;
						conn_servent->timestamp = h_packet->data->header->timestamp;
						
						char *send_msg = prepare_msg(conn_servent->timestamp, conn_servent->nick, conn_servent->msg, conn_servent->msg_len);
						UNLOCK(h_packet->data->header->sender_id);
						u_int8 chat_id = GET_MESSAGE(h_packet->data)->chat_id;
						if(chat_id==0) {
							logger(SYS_INFO, "[servent_responde]PM\n");
							controller_add_msg(h_packet->data->header->sender_id, send_msg);
						}
						else {
							logger(SYS_INFO, "[servent_responde]CHAT\n");
							controller_add_msg_to_chat(chat_id, send_msg);
						}
						printf("[servent_responde]msg: %s, msg_len: %d\n", conn_servent->msg, conn_servent->msg_len);
						

						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==SEARCH_ID) {
						printf("[servent_responde]SEARCH ricevuto packet_id: %lld\n", h_packet->data->header->id);
						if(get_search_packet(h_packet->data->header->id) == NULL) {
							new_search_packet(h_packet->data->header->id);
						
							GList *res;
							GList *servent_list;
							servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
							if(conn_servent==NULL) {
								printf("[servent_responde]conn_servent entry %lld doesn't found\n", h_packet->data->header->sender_id);
								continue;
							}
							printf("[servent_responde]conn_servent entry found\n");
						
							if(h_packet->data_len>0) {
								servent_data *sd;
								RLOCK(conn_servent->id);
								COPY_SERVENT(conn_servent, sd);
								UNLOCK(conn_servent->id);
							
								printf("[servent_responde]Searching %s\n", tortella_get_data(h_packet->data_string));
								res = search_all_chat(tortella_get_data(h_packet->data_string));
								printf("[servent_responde]Results number %d\n", g_list_length(res));
				
								logger(SYS_INFO, "[servent_responde]Sending to ID: %lld\n", sd->id);
								sd->chat_res = res;
								sd->packet_id = h_packet->data->header->id;
								sd->post_type = SEARCHHITS_ID;								
								servent_send_packet(sd);
						
							}

						
							printf("[servent_responde]Sending SEARCHHITS packet to searching peer\n");
						
							if(GET_SEARCH(h_packet->data)->ttl>0) {
								printf("[servent_responde]TTL > 0\n");
								int i;
								servent_list = g_hash_table_get_values(servent_hashtable);
								for(i=0; i<g_list_length(servent_list); i++) {
							
									conn_servent = (servent_data*)g_list_nth_data(servent_list, i);
									if(conn_servent->id!=h_packet->data->header->sender_id) {
										RLOCK(conn_servent->id);
										servent_data *sd;
										COPY_SERVENT(conn_servent, sd);
										sd->ttl = GET_SEARCH(h_packet->data)->ttl-1;
										sd->hops = GET_SEARCH(h_packet->data)->hops+1;
										sd->title = tortella_get_data(h_packet->data_string);
										sd->title_len = h_packet->data->header->data_len;
										sd->packet_id = h_packet->data->header->id;
										sd->post_type = SEARCH_ID;
										UNLOCK(conn_servent->id);
										servent_send_packet(sd);
										//Aggiunta regola di routing alla tabella
										add_route_entry(h_packet->data->header->id, h_packet->data->header->sender_id, conn_servent->id, route_hashtable);
										printf("[servent_responde]Retrasmitting SEARCH packet to other peers\n");
									}
								}		   
							}
							status = HTTP_STATUS_OK;
						}
					}
					else if(h_packet->data->header->desc_id==SEARCHHITS_ID) {
						printf("[servent_responde]SEARCHHITS ricevuto\n");
						
						GList *chat_list = char_to_chatlist(tortella_get_data(h_packet->data_string), h_packet->data->header->data_len);
						add_all_to_chat(chat_list); //TODO: deve ricercare anche in locale!!!
						
						route_entry *entry = get_route_entry(h_packet->data->header->id, route_hashtable);
						if(entry!=NULL) {
							RLOCK(entry->sender_id);
							servent_data *sd;
							servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(entry->sender_id));
							
							COPY_SERVENT(conn_servent, sd);
							sd->packet_id = h_packet->data->header->id;
		//COMMENTATA SEG FAULT					printf("[servent_responde]list size: %d, users: %d\n", g_list_length(chat_list), g_hash_table_size(((chat*)g_list_nth_data(chat_list, 0))->users));
							sd->chat_res = chat_list;
							sd->post_type = SEARCHHITS_ID; 
							UNLOCK(entry->sender_id);
							servent_send_packet(sd);
							printf("[servent_responde]Routing packet from %lld to %lld\n", h_packet->data->header->sender_id, entry->sender_id);
							del_route_entry(h_packet->data->header->id, route_hashtable);
							printf("[servent_responde]Route entry %lld deleted\n", h_packet->data->header->id); 
						}
						else {
							int i=0;
							chat *chat_val;
							
							for(; i<g_list_length(chat_list); i++) {
								chat_val = (chat*)g_list_nth_data(chat_list, i);																
								logger(SYS_INFO,"[servent_responde] title chat %s\n", chat_val->title);
								GList *local_chat = search_all_local_chat(chat_val->title);
								int j=0;
								for(; j<g_list_length(local_chat); j++) {
								chat *tmp = (chat*)g_list_nth_data(local_chat, j);
								logger(SYS_INFO, "[servent_responde] title chat %s\n", tmp->title);
								add_chat_to_list(tmp->id, tmp->title);
								}
							}
						}
						
						status = HTTP_STATUS_OK;
						
					}
					
					//Invio la conferma di ricezione
					if(status>0) {
						printf("[servent_responde]Sending post response\n");
						send_post_response_packet(fd, status, 0, NULL);
					}
						
				}
				else if(h_packet->type==HTTP_REQ_GET) {
					printf("[servent_responde]GET ricevuto\n");
				}
				
				/*if(h_packet->header_request!=NULL)
					free(h_packet->header_request);
				if(h_packet->header_response!=NULL)
					free(h_packet->header_response);
				if(h_packet->data!=NULL)
					free(h_packet->data);
				if(h_packet->data_string!=NULL)
					free(h_packet->data_string);*/
//				if(h_packet->header_request!=NULL) {
//					printf("[servent_responde]free\n");
//					free(h_packet->header_request);
//				}
//				free(h_packet);
			}
		}
		else {
			printf("[servent_responde]Client disconnected\n");
			delete_socket(fd);
			server_connection_fd = g_slist_remove(server_connection_fd, (gconstpointer)fd);
			server_connection_thread = g_slist_remove(server_connection_thread, (gconstpointer)(pthread_self()));
			pthread_exit(NULL);
		}
	}
	pthread_exit(NULL);
}

//parm: id del peer
void *servent_connect(void *parm) {
	int len;
	char *buffer = (char*)malloc(2000);
	http_packet *h_packet;
	u_int8 id_dest = *((u_int8*)(parm));
	logger(SYS_INFO, "[servent_connect]ID: %lld\n", id_dest);
	
	//Si prendono l'ip e la porta dalla lista degli id
	servent_data *servent_peer = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id_dest));
	servent_data *servent_queue;
	if(servent_peer == NULL) {
		logger(SYS_INFO, "[servent_connect] servent_peer nullo\n");
		return NULL;
	}
	char *ip_dest = servent_peer->ip;
	u_int4 port_dest = servent_peer->port;
	
	int fd = servent_create_client(ip_dest, port_dest);

	client_fd = g_slist_prepend(client_fd, (gpointer)fd);

	servent_peer->queue = g_queue_new();
	servent_peer->res_queue = g_queue_new();

	//Aggiunta richiesta di PING nella coda del sudddetto servent
	servent_data *tmp;
	COPY_SERVENT(servent_peer, tmp);
	tmp->post_type = PING_ID;
	servent_send_packet(tmp);

	u_int4 post_type;
	u_int8 chat_id_req;
	u_int4 msg_len;
	char *msg;
	u_int4 title_len;
	char *title;
	u_int1 ttl, hops;
	u_int8 packet_id;
	
	u_int1 status;
	char *nick;

	//Ora si entra nel ciclo infinito che serve per inviare tutte le richieste
	while(1) {
		logger(SYS_INFO, "[servent_connect]Waiting\n");
		if(servent_peer==NULL) {
			logger(SYS_INFO, "[servent_connect] Peer NULL\n");
			break;
			//servent_peer = servent_get(id_dest);
		}
		id_dest = servent_peer->id;
		servent_queue = servent_pop_queue(servent_peer);
		if(servent_queue==NULL) {
			logger(SYS_INFO, "[servent_connect] Queue NULL\n");
			continue;
		}
		logger(SYS_INFO, "[servent_connect]Signal received in id_dest %lld\n", id_dest);
		
		RLOCK(local_servent->id); 
		WLOCK(id_dest); 
		logger(SYS_INFO, "[servent_connect] post_type %d\n", servent_queue->post_type);
		servent_peer->post_type = servent_queue->post_type;
		servent_peer->chat_id_req = servent_queue->chat_id_req;
		servent_peer->msg_len = servent_queue->msg_len;
		servent_peer->msg = servent_queue->msg;
		servent_peer->title = servent_queue->title;
		servent_peer->title_len = servent_queue->title_len;
		servent_peer->ttl = servent_queue->ttl;
		servent_peer->hops = servent_queue->hops;
		servent_peer->packet_id = servent_queue->packet_id;
		
		post_type = servent_peer->post_type;
		chat_id_req = servent_peer->chat_id_req;
		msg_len = servent_peer->msg_len;
		msg = servent_peer->msg;
		title = servent_peer->title;
		title_len = servent_peer->title_len;
		ttl = servent_peer->ttl;
		hops = servent_peer->hops;
		packet_id = servent_peer->packet_id;
		
		status = local_servent->status;
		nick = local_servent->nick;
		
		if(post_type==HTTP_REQ_GET) {
			//send_get_request_packet(fd, char *filename, u_int4 range_start, u_int4 range_end);
		}
		else {
			
			if(post_type==JOIN_ID) {
				send_join_packet(fd, local_servent->id, id_dest, status, chat_id_req, nick);
			}
			else if(post_type==PING_ID) {
				send_ping_packet(fd, local_servent->id, id_dest, nick, local_servent->port, status);
			}
			else if(post_type==BYE_ID) {
				send_bye_packet(fd, local_servent->id, id_dest);
			}
			else if(post_type==LEAVE_ID) {
				send_leave_packet(fd, local_servent->id, id_dest, chat_id_req);
			}
			else if(post_type==MESSAGE_ID) {
				send_message_packet(fd, local_servent->id, id_dest, chat_id_req, msg_len, msg);
			}
			else if(post_type==LIST_ID) {
				send_list_packet(fd, local_servent->id, id_dest, chat_id_req);
			}
			else if(post_type==LISTHITS_ID) {
				int length;
				char *buf = userlist_to_char(servent_queue->user_res, &length);
				if(buf==NULL) {
					length=0;
				}
				else {
					logger(SYS_INFO, "[servent_connect]Results converted in buffer: %s\n", buf);
				}
				send_listhits_packet(fd, local_servent->id, id_dest, g_list_length(servent_queue->user_res), length, buf);
			}
			else if(post_type==SEARCH_ID) {
				send_search_packet(fd, packet_id, local_servent->id, id_dest, ttl, hops, title_len, title);
			}
			else if(post_type==SEARCHHITS_ID) {
				int length;
				char *buf = chatlist_to_char(servent_queue->chat_res, &length);
				if(buf==NULL) {
					length=0;
				}
				else {
					logger(SYS_INFO, "[servent_connect]Results converted in buffer: %s\n", buf);
				}
				send_searchhits_packet(fd, packet_id, local_servent->id, id_dest, g_list_length(servent_queue->chat_res), length, buf);
			}
		
			free(buffer);
			logger(SYS_INFO, "[servent_connect]Listening response\n");
			len = recv_http_packet(fd, &buffer);
			logger(SYS_INFO, "[sevente_connect]Received response\n");
			if(len>0) {
				printf("[servent_connect]buffer recv: %s\n", dump_data(buffer, len));
				h_packet = http_char_to_bin(buffer);
				if(h_packet!=NULL && h_packet->type==HTTP_RES_POST) {
					if(strcmp(h_packet->header_response->response, HTTP_OK)==0) {
						printf("[servent_connect]OK POST received\n");
						/*if(post_type==PING_ID && id_dest<conf_get_gen_start()) {
							printf("[servent_connect]Responding to PING\n");
							if(servent_peer!=NULL) {
								printf("[servent_connect]Removing old %s\n", to_string(id_dest));
								servent_data *tmp;
								COPY_SERVENT(servent_peer, tmp);
								g_hash_table_remove(servent_hashtable, (gconstpointer)to_string(id_dest));
								printf("[servent_connect]Removed old ID: %lld\n", servent_peer->id);
								printf("[servent_connect]Converting %s, len: %d\n", h_packet->data_string, h_packet->data_len);
								char *buf = calloc(h_packet->data_len+1, 1);
								sprintf(buf, "%s", strndup(h_packet->data_string, h_packet->data_len));
								printf("[servent_connect]buf: %s.\n", buf);
								id_dest = strtoull(buf, NULL, 10);
								printf("[servent_connect]New ID: %lld.\n", id_dest);
								COPY_SERVENT(tmp, servent_peer);
								servent_peer->id = id_dest;
								g_hash_table_insert(servent_hashtable, (gpointer)to_string(id_dest), (gpointer)servent_peer);
							}
							else
								printf("[servent_connect]Data isn't present\n");
						}*/
						
					}
					else {
						printf("[servent_connect]Error\n");
					}
				}
			}
			if(h_packet != NULL) {
				logger(SYS_INFO, "[servent_connect]Appending response\n");
				if(servent_peer==NULL) {
					logger(SYS_INFO, "[servent_connect] Peer response NULL\n");
					servent_peer = servent_get(id_dest);
				}
				servent_append_response(servent_peer, h_packet->header_response->response);
				logger(SYS_INFO, "[servent_connect]Appended\n");
			}
			else {
				logger(SYS_INFO, "[servent_connect]Appending response TIMEOUT\n");
				servent_append_response(servent_peer, TIMEOUT);
			}
			UNLOCK(id_dest); 
			UNLOCK(local_servent->id); 
		}
	}
	pthread_exit(NULL);
}

void *servent_timer(void *parm) {
	GList *list;
	servent_data *data, *tmp;
	int i;
	
	while(1) {
		
		list=g_hash_table_get_values(servent_hashtable);
		
		for(i=0; i<g_list_length(list); i++) {
			data = (servent_data*)g_list_nth_data(list, i);
			if(data->id!=local_servent->id) {
			
				COPY_SERVENT(data, tmp);
				tmp->post_type = PING_ID;
				servent_send_packet(tmp);
				printf("[servent_timer]Signaling %lld\n", data->id);
			}
		}
		
		printf("[servent_timer]Sleeping\n");
		sleep(timer_interval);
	}
}
