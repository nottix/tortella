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
u_int4 servent_create_server(char *src_ip, u_int4 src_port) {
	logger(SOCK_INFO, "[servent_create_server]Creating listen tcp socket on: %s:%d\n", src_ip, src_port);
	u_int4 sock = create_listen_tcp_socket(src_ip, src_port);
	return sock;
}

//Crea un client socket
u_int4 servent_create_client(char *dst_ip, u_int4 dst_port) {
	logger(SOCK_INFO, "[servent_create_client]Creating tcp socket on: %s:%d\n", dst_ip, dst_port);
	u_int4 sock = create_tcp_socket(dst_ip, dst_port);
	return sock;
}

u_int4 servent_start_server(char *local_ip, u_int4 local_port) {
	int serfd = servent_create_server(local_ip, local_port); //Locale
	if(serfd<0) {
		logger(ALARM_INFO, "[servent_start]Errore nella creazione del server\n");
		return -1;
	}
	pthread_t *serthread = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(serthread, NULL, servent_listen, (void*)serfd);
	
	server_fd = g_slist_prepend(server_fd, (gpointer)serfd);
	server_thread = g_slist_prepend(server_thread, (gpointer)serthread);
	
	return 1;
}

u_int4 servent_start_client(char *dest_ip, u_int4 dest_port) {
	pthread_t *clithread = (pthread_t*)malloc(sizeof(pthread_t));
	u_int8 cliid = new_connection_counter++;
	
	servent_data *servent = (servent_data*)malloc(sizeof(servent_data));
	servent->id = cliid;
	logger(SOCK_INFO, "[servent_start_client]cliid: %lld\n", cliid);
	servent->ip = dest_ip;
	servent->port = dest_port;
	
	pthread_mutex_init(&servent->mutex, NULL);
	pthread_rwlock_init(&servent->rwlock_data, NULL);
	pthread_cond_init(&servent->cond, NULL);

	servent->post_type=PING_ID;

	g_hash_table_insert(servent_hashtable, (gpointer)to_string(cliid), (gpointer)servent);
	pthread_create(clithread, NULL, servent_connect, (void*)&cliid);
	client_thread = g_slist_prepend(client_thread, (gpointer)(*clithread));
	
	return 1;
}

u_int4 servent_start(GList *init_servent) {
	//Inizializzazione servent locale
	servent_init(conf_get_local_ip(), conf_get_local_port(), ONLINE_ID);
	
	//Avvio server di ascolto richieste
	servent_start_server(conf_get_local_ip(), conf_get_local_port());
	
	//Fase iniziale di reperimento degli utenti iniziali
	if(init_servent!=NULL)
		servent_init_connection(init_servent);

	return 0;
}

int servent_start_timer(void) {
	timer_thread = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(timer_thread, NULL, servent_timer, NULL);
	return 0;
}

int servent_init_connection(GList *init_servent) {
	int i;
	init_data *peer;
	for(i=0; i<g_list_length(init_servent); i++) {
		peer = (init_data*)g_list_nth_data(init_servent, i);
		logger(SOCK_INFO, "[servent_init_connection]ip: %s, port: %d\n", peer->ip, peer->port);
		servent_start_client(peer->ip, peer->port);
	}
	
	return 0;
}

void servent_close_all(void) {
	
	int i;
	for(i=0; i<g_slist_length(server_fd); i++) {
		logger(SYS_INFO, "[servent_close_all]Closing server %d\n", (int)g_slist_nth_data(server_fd, i));
		close((int)g_slist_nth_data(server_fd, i));
	}
	for(i=0; i<g_slist_length(client_fd); i++) {
		logger(SYS_INFO, "[servent_close_all]Closing client %d\n", (int)g_slist_nth_data(client_fd, i));
		close((int)g_slist_nth_data(client_fd, i));
	}
	for(i=0; i<g_slist_length(server_connection_fd); i++) {
		logger(SYS_INFO, "[servent_close_all]Closing server_connection %d\n", (int)g_slist_nth_data(server_connection_fd, i));
		close((int)g_slist_nth_data(server_connection_fd, i));
	}
	
}

void kill_all_thread(int sig) {
	logger(SYS_INFO, "[kill_all_thread]Killing thread\n");
	servent_close_supernode();
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
	
	logger(SYS_INFO, "[kill_all_thread]Closing supernode\n");

}

void servent_init(char *ip, u_int4 port, u_int1 status) {
	
	//Inizializza la lista delle chat conosciute leggendo da un file predefinito
	servent_init_supernode();
	logger(SYS_INFO, "[servent_init]Supernode initialized on %s:%d\n", conf_get_local_ip(), conf_get_local_port());
	
	servent_hashtable = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);
	route_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	
	local_servent = (servent_data*)malloc(sizeof(servent_data));
	u_int8 id = local_servent->id = generate_id();
	logger(SYS_INFO, "[servent_init]ID: %lld\n", local_servent->id);
	local_servent->ip = ip;
	local_servent->port = port;
	local_servent->status = status;
	local_servent->nick = "simone";
	
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
}

void servent_init_supernode() {
	read_all(&chat_hashtable, &chatclient_hashtable);
	//read_from_file("data/111", &chat_hashtable, &chatclient_hashtable);
}

void servent_close_supernode() {
	logger(SYS_INFO, "[servent_close_supernode]Init\n");
	write_all(chat_hashtable, MODE_TRUNC);
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
			thread = (pthread_t*)malloc(sizeof(pthread_t));
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
	char *buffer/* = (char*)malloc(2000)*/;
	http_packet *h_packet;
	int len;
	int fd = (int)parm;
	u_int4 status = 0;
	
	while(1) {
		//sleep(1);
		//memset(buffer, 0, 2000);
		len = switch_http_packet(fd, &buffer, LP_READ);
		printf("[servent_responde]Data received, buffer: %s, len: %d\n", buffer, len);
		
		if(len>0) {
			printf("[servent_responde]Converting\n");
			h_packet = http_char_to_bin((const char*)buffer);
			printf("[servent_responde]char converted\n");
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
						
						WLOCK(id);
						servent_data *conn_servent = g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id));

						conn_servent->timestamp = h_packet->data->header->timestamp;
						
						add_exist_user_to_chat(GET_JOIN(h_packet->data)->chat_id, id, chat_hashtable, &chatclient_hashtable);

						UNLOCK(id);
						
						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==PING_ID) {
						printf("[servent_responde]PING ricevuto da %lld a %lld\n", h_packet->data->header->sender_id, h_packet->data->header->recv_id);
						
						servent_data *conn_servent;
						u_int8 id = h_packet->data->header->sender_id;
						if((conn_servent=g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id)))!=NULL) {
							WLOCK(id);
							conn_servent->status = GET_PING(h_packet->data)->status;
							conn_servent->timestamp = h_packet->data->header->timestamp;
							conn_servent->nick = h_packet->data->data;
							UNLOCK(id);
							printf("[servent_responde]PING old\n");
							
							status = HTTP_STATUS_OK;
						}
						else {
							printf("[servent_responde]New ping\n");
							
							if(h_packet->data->header->recv_id < conf_get_gen_start()) {
								status = HTTP_STATUS_OK;
								char *new_id = to_string(local_servent->id);
								printf("[servent_responde]sending new ID: %s with len %d\n", new_id, strlen(new_id));
								send_post_response_packet(fd, status, strlen(new_id), new_id);
								status = 0;
							}
							else {
								status = HTTP_STATUS_OK;
							}
							
							conn_servent = (servent_data*)calloc(1, sizeof(servent_data));
							conn_servent->ip = get_dest_ip(fd);
							conn_servent->port = GET_PING(h_packet->data)->port;
							conn_servent->timestamp = h_packet->data->header->timestamp;
							
							conn_servent->status = GET_PING(h_packet->data)->status;
							conn_servent->timestamp = h_packet->data->header->timestamp;
							conn_servent->nick = h_packet->data->data;
							conn_servent->id = h_packet->data->header->sender_id;
							
							add_user(conn_servent->id, conn_servent->nick, conn_servent->ip, conn_servent->port, &chatclient_hashtable);
							
							//Si inizializzano il mutex e il cond
							pthread_mutex_init(&conn_servent->mutex, NULL);
							pthread_rwlock_init(&conn_servent->rwlock_data, NULL);
							pthread_cond_init(&conn_servent->cond, NULL);
							
							printf("[servent_responde]Lookup ID: %s\n", to_string(id));
							if(g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id))==NULL) {
								printf("[servent_responde]connection %s added to hashtable\n", to_string(id));
								g_hash_table_insert(servent_hashtable, (gpointer)to_string(id), (gpointer)conn_servent);
							}
							
							conn_servent->post_type = PING_ID;
							WLOCK(local_servent->id);
							conn_servent->status = local_servent->status;
							UNLOCK(local_servent->id);
							
							pthread_t *cli_thread = (pthread_t*)malloc(sizeof(pthread_t));
							pthread_create(cli_thread, NULL, servent_connect, (void*)&id);
							client_thread = g_slist_prepend(client_thread, (gpointer)(*cli_thread));
							//sleep(1);
							printf("[servent_responde]Invio a %lld\n", conn_servent->id);
							
							//pthread_cond_signal(&conn_servent->cond); //FIXIT: probabile race condition
						}
					}
					else if(h_packet->data->header->desc_id==PONG_ID) {
						printf("[servent_responde]PONG ricevuto\n");
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						WLOCK(h_packet->data->header->sender_id);
						conn_servent->status = GET_PONG(h_packet->data)->status;
						conn_servent->timestamp = h_packet->data->header->timestamp;
						UNLOCK(h_packet->data->header->sender_id);
						
						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==LEAVE_ID) {
						printf("[servent_responde]LEAVE ricevuto\n");
						u_int8 chat_id = GET_LEAVE(h_packet->data)->chat_id;
						
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						WLOCK(h_packet->data->header->sender_id);
						conn_servent->chat_list = g_list_remove(conn_servent->chat_list, (gconstpointer)&chat_id);
						conn_servent->timestamp = h_packet->data->header->timestamp;
						UNLOCK(h_packet->data->header->sender_id);
						
						status = HTTP_STATUS_OK;
						//Sconnetti dalla chat
					}
					else if(h_packet->data->header->desc_id==MESSAGE_ID) {
						printf("[servent_responde]MESSAGE ricevuto\n");
						
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));

						WLOCK(h_packet->data->header->sender_id);
						conn_servent->msg = h_packet->data->data;
						conn_servent->msg_len = h_packet->data->header->data_len;
						conn_servent->timestamp = h_packet->data->header->timestamp;
						printf("[servent_responde]msg: %s, msg_len: %d\n", conn_servent->msg, conn_servent->msg_len);
						UNLOCK(h_packet->data->header->sender_id);

						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==SEARCH_ID) {
						printf("[servent_responde]SEARCH ricevuto\n");
						
						GList *res;
						GList *servent_list;
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						if(conn_servent==NULL) {
							printf("[servent_responde]conn_servent entry doesn't found\n");
							continue;
						}
						printf("[servent_responde]conn_servent entry found\n");
						//int len;
						WLOCK(conn_servent->id);
						printf("[servent_respond]Locked\n");
						
						if(h_packet->data_len>0) {
							printf("[servent_responde]Searching %s\n", tortella_get_data(h_packet->data_string));
							res = search_all_chat(tortella_get_data(h_packet->data_string), chat_hashtable);
							printf("[servent_responde]Results number %d\n", g_list_length(res));
							//if(g_list_length(res)>0) {
								conn_servent->chat_res = res;
							//}
							//else {
							//	conn_servent->chat_res = NULL;
							//}
							conn_servent->packet_id = h_packet->data->header->id;
							conn_servent->post_type = SEARCHHITS_ID;
						}
				
						//local_servent->title = tortella_get_data(h_packet->data_string);
						//local_servent->title_len = h_packet->data_len;
						UNLOCK(conn_servent->id);
						pthread_cond_signal(&conn_servent->cond);
						printf("[servent_responde]Sending SEARCHHITS packet to searching peer\n");
						
						if(GET_SEARCH(h_packet->data)->ttl>0) {
							printf("[servent_responde]TTL > 0\n");
							int i;
							servent_list = g_hash_table_get_values(servent_hashtable);
							for(i=0; i<g_list_length(servent_list); i++) {
							
								conn_servent = (servent_data*)g_list_nth_data(servent_list, i);
								if(conn_servent->id!=h_packet->data->header->sender_id) {
									WLOCK(conn_servent->id);
									conn_servent->ttl = GET_SEARCH(h_packet->data)->ttl-1;
									conn_servent->hops = GET_SEARCH(h_packet->data)->hops+1;
									conn_servent->title = tortella_get_data(h_packet->data_string);
									conn_servent->title_len = h_packet->data->header->data_len;
									conn_servent->packet_id = h_packet->data->header->id;
									conn_servent->post_type = SEARCH_ID;
									
									//Aggiunta regola di routing alla tabella
									add_route_entry(h_packet->data->header->id, h_packet->data->header->sender_id, conn_servent->id, route_hashtable);
									UNLOCK(conn_servent->id);
									pthread_cond_signal(&conn_servent->cond);
									printf("[servent_responde]Retrasmitting SEARCH packet to other peers\n");
								}
							}		   
						}
						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==SEARCHHITS_ID) {
						printf("[servent_responde]SEARCHHITS ricevuto\n");
						
						GList *chat_list = char_to_chatlist(tortella_get_data(h_packet->data_string), h_packet->data->header->data_len);
						int i;
						chat *chat_elem;
						for(i=0; i<g_list_length(chat_list); i++) {
							chat_elem = (chat*)g_list_nth_data(chat_list, i);
							add_chat(chat_elem->id, chat_elem->title, &chat_hashtable);
							printf("[servent_responde]Chat %lld added to list\n", chat_elem->id);
						}
						
						route_entry *entry = get_route_entry(h_packet->data->header->id, route_hashtable);
						if(entry!=NULL) {
							WLOCK(entry->sender_id);
							
							servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(entry->sender_id));
							conn_servent->packet_id = h_packet->data->header->id;
							printf("[servent_responde]list size: %d, users: %d\n", g_list_length(chat_list), g_hash_table_size(((chat*)g_list_nth_data(chat_list, 0))->users));
							conn_servent->chat_res = chat_list;
							conn_servent->post_type = SEARCHHITS_ID;
							
							UNLOCK(entry->sender_id);
							pthread_cond_signal(&conn_servent->cond);
							printf("[servent_responde]Routing packet from %lld to %lld\n", h_packet->data->header->sender_id, entry->sender_id);
							del_route_entry(h_packet->data->header->id, route_hashtable);
							printf("[servent_responde]Route entry %lld deleted\n", h_packet->data->header->id);
						}
						
						status = HTTP_STATUS_OK;
						
					}
//					else if(h_packet->data->header->desc_id==CREATE_ID) {
//						printf("[servent_responde]CREATE ricevuto\n");
//						u_int8 chat_id = GET_CREATE(h_packet->data)->chat_id;
//						u_int4 title_len = h_packet->data->header->data_len;
//						char *title = h_packet->data->data;
//						
//						WLOCK(local_servent->id);
//						//if(local_servent->is_supernode) {
//							add_chat(chat_id, title, &chat_hashtable);
//							//TODO: eventualmente aggiungere un servent_data ????
//							
//							chat *test = (chat*)g_hash_table_lookup(chat_hashtable, (gconstpointer)to_string(chat_id));
//	
//							printf("[servent_responde]chat created with ID: %lld\n", test->id);
//						//}
//						
//						UNLOCK(local_servent->id);
//						
//						status = HTTP_STATUS_OK;
//						//Crea chat
//					}
					
					//Invio la conferma di ricezione
					if(status>0) {
						printf("[servent_responde]sending post response\n");
						send_post_response_packet(fd, status, 0, NULL);
					}
						
				}
				else if(h_packet->type==HTTP_REQ_GET) {
					printf("[servent_responde]GET ricevuto\n");
					
					//char *filename = h_packet->header_request->request;
				}
				
				/*if(h_packet->header_request!=NULL)
					free(h_packet->header_request);
				if(h_packet->header_response!=NULL)
					free(h_packet->header_response);
				if(h_packet->data!=NULL)
					free(h_packet->data);
				if(h_packet->data_string!=NULL)
					free(h_packet->data_string);*/
				if(h_packet->header_request!=NULL) {
					printf("[servent_responde]free\n");
					free(h_packet->header_request);
				}
				free(h_packet);
			}
		}
		else {
			printf("[servent_responde]Client disconnected\n");
			close(fd);
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
	//Si prendono l'ip e la porta dalla lista degli id
	servent_data *servent_peer = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id_dest));
	char *ip_dest = servent_peer->ip;
	u_int4 port_dest = servent_peer->port;
	
	int fd = servent_create_client(ip_dest, port_dest);
	
	pthread_mutex_t *mutex = &servent_peer->mutex;
	pthread_cond_t *cond = &servent_peer->cond;
	
	client_fd = g_slist_prepend(client_fd, (gpointer)fd);
	
	printf("[servent_connect]Init %lld\n", id_dest);
	
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
	

	//Ora si entra nel ciclio infinito che serve per inviare tutte le richieste
	while(1) {
		printf("[servent_connect]connect while\n");
		WLOCK(id_dest);
		post_type = servent_peer->post_type;
		chat_id_req = servent_peer->chat_id_req;
		msg_len = servent_peer->msg_len;
		msg = servent_peer->msg;
//		msg = calloc(2000, 1);
//		memset(msg, 55, 2000);
//		msg_len = 2000;
		title = servent_peer->title;
		title_len = servent_peer->title_len;
		ttl = servent_peer->ttl;
		hops = servent_peer->hops;
		packet_id = servent_peer->packet_id;
		printf("[servent_connect]Sending to id_dest %lld the packet type %d\n", servent_peer->id, servent_peer->post_type);
//		UNLOCK(id_dest);
		
		RLOCK(local_servent->id);
		status = local_servent->status;
		nick = local_servent->nick;
		UNLOCK(local_servent->id);
		
		if(post_type==HTTP_REQ_GET) {
			//send_get_request_packet(fd, char *filename, u_int4 range_start, u_int4 range_end);
		}
		else {
			
			if(post_type==JOIN_ID) {
				send_join_packet(fd, local_servent->id, id_dest, status, chat_id_req, nick); //TODO: Dubbio su chat_id_req Local o peer???
			}
			else if(post_type==PING_ID) {
				send_ping_packet(fd, local_servent->id, id_dest, nick, local_servent->port, status);
			}
			else if(post_type==PONG_ID) {
//				send_pong_packet(fd, local_servent->id, id_dest, status);
			}
			else if(post_type==LEAVE_ID) {
				send_leave_packet(fd, local_servent->id, id_dest, chat_id_req);
			}
			else if(post_type==MESSAGE_ID) {
				send_message_packet(fd, local_servent->id, id_dest, chat_id_req, msg_len, msg);
			}
//			else if(post_type==CREATE_ID) {
//				send_create_packet(fd, local_servent->id, id_dest, chat_id_req, title_len, title);
//			}
			else if(post_type==SEARCH_ID) {
				send_search_packet(fd, packet_id, local_servent->id, id_dest, ttl, hops, title_len, title);
			}
			else if(post_type==SEARCHHITS_ID) {
				
				servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id_dest));
				
				RLOCK(local_servent->id);
				//LOCK(id_dest);
				
				printf("[servent_connect]Converting chatlist\n");
				int length;
				char *buf = chatlist_to_char(conn_servent->chat_res, &length);
				if(buf==NULL) {
					length=0;
				}
				else {
					printf("[servent_connect]converted in buffer: %s\n", buf);
				}
				send_searchhits_packet(fd, packet_id, local_servent->id, id_dest, g_list_length(conn_servent->chat_res), length, buf);
				
				//UNLOCK(id_dest);
				UNLOCK(local_servent->id);
				
			}
		
			//memset(buffer, 0, 2000);
			printf("[servent_connect]Listening response\n");
			len = switch_http_packet(fd, &buffer, LP_READ);
			printf("[sevente_connect]Received response\n");
			if(len>0) {
				printf("[servent_connect]buffer recv: %s\n", dump_data(buffer, len));
				h_packet = http_char_to_bin(buffer);
				if(h_packet!=NULL && h_packet->type==HTTP_RES_POST) {
					if(strcmp(h_packet->header_response->response, HTTP_OK)==0) {
						printf("[servent_connect]OK POST received\n");
						if(post_type==PING_ID && id_dest<conf_get_gen_start()) {
							printf("[servent_connect]Responding to PING\n");
							//servent_data *data = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id_dest));
							if(servent_peer!=NULL) {
								printf("[servent_connect]Removing old %s\n", to_string(id_dest));
								g_hash_table_remove(servent_hashtable, (gconstpointer)to_string(id_dest));
								printf("[servent_connect]Removed old ID: %lld\n", servent_peer->id);
								printf("[servent_connect]Converting %s, len: %d\n", h_packet->data_string, h_packet->data_len);
								char *buf = malloc(h_packet->data_len+1);
								sprintf(buf, "%s", strndup(h_packet->data_string, h_packet->data_len));
								printf("[servent_connect]buf: %s.\n", buf);
								id_dest = strtoull(buf, NULL, 10);
								printf("[servent_connect]New ID: %lld.\n", id_dest);
								servent_peer->id = id_dest;
								g_hash_table_insert(servent_hashtable, (gpointer)to_string(id_dest), (gpointer)servent_peer);
							}
							else
								printf("[servent_connect]Data isn't present\n");
						}
					}
					else {
						printf("[servent_connect]Error\n");
					}
				}
			}
		}
		UNLOCK(id_dest);
		
		printf("[servent_connect]Waiting\n");
		pthread_cond_wait(cond, mutex); //In attesa di un segnale che indichi la necessita di comunicare con il peer
		printf("[servent_connect]Signal received in id_dest %lld\n", id_dest);
	}
	pthread_exit(NULL);
}

void *servent_timer(void *parm) {
	GList *list;
	servent_data *data;
	int i;
	
	while(1) {
		
		list=g_hash_table_get_values(servent_hashtable);
		
		for(i=0; i<g_list_length(list); i++) {
			data = (servent_data*)g_list_nth_data(list, i);
			if(data->id!=local_servent->id) {
				WLOCK(data->id);
				printf("[servent_timer]Locked\n");
			
				data->post_type = PING_ID;
				
				printf("[servent_timer]Signaling %lld\n", data->id);
				
				UNLOCK(data->id);
				pthread_cond_signal(&data->cond);
				printf("[servent_timer]Unlocked\n");
			}
		}
		
		printf("[servent_timer]Sleeping\n");
		sleep(timer_interval);
	}
}
