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
	printf("[servent_create_server]Creating listen tcp socket on: %s:%d\n", src_ip, src_port);
	u_int4 sock = create_listen_tcp_socket(src_ip, src_port);
	return sock;
}

//Crea un client socket
u_int4 servent_create_client(char *dst_ip, u_int4 dst_port) {
	printf("[servent_create_client]Creating tcp socket on: %s:%d\n", dst_ip, dst_port);
	u_int4 sock = create_tcp_socket(dst_ip, dst_port);
	return sock;
}

u_int4 servent_start(char *ip, u_int4 port, u_int1 is_supernode) {
	servent_init(ip, port, ONLINE_ID, is_supernode);
	//Fase iniziale di reperimento dell'ID della chat e degli utenti che ne fanno parte
	//Si registra tale lista in memoria
	//Esempio di dati
	u_int8 chat_id = 21101959;
	u_int8 id = 11111111;
	u_int8 id_dest = 22222222;
	char *ip_dest = "127.0.0.1";
	u_int4 port_dest = 2110;
	
	printf("Avviare il server? ");
	if(getchar()=='s') {
		//Avvia il server
		int serfd = servent_create_server(ip, port); //Locale
		if(serfd<0) {
			printf("[servent_start]Errore nella creazione del server\n");
			return -1;
		}
	
		pthread_t *serthread = (pthread_t*)malloc(sizeof(pthread_t));
		pthread_create(serthread, NULL, servent_listen, (void*)serfd);
		
		server_fd = g_slist_prepend(server_fd, (gpointer)serfd);
		server_thread = g_slist_prepend(server_thread, (gpointer)serthread);
	}
	
	printf("Avviare il client? ");
	if(getchar()=='s') {
		//Avvia il client
		
		pthread_t *clithread = (pthread_t*)malloc(sizeof(pthread_t));
		u_int8 cliid;
		printf("Inserisci l'ID del peer: ");
		scanf("%lld", &cliid);
		servent_data *servent = (servent_data*)malloc(sizeof(servent_data));
		servent->id = cliid;
		servent->ip = ip;
		servent->port = port;
		servent->chat_id_req = 111;
		servent->status = ONLINE_ID;
		
		//local_servent->id_dest = to_string(servent->id);
		//local_servent->id_dest_len = strlen(servent->id);
		servent->msg_len = 5;
		servent->msg = "Hello";
		
		servent->title_len =4;
		servent->title = "Test";
		
		servent->post_type=JOIN_ID;
		g_hash_table_insert(servent_hashtable, (gpointer)to_string(cliid), (gpointer)servent);
		pthread_create(clithread, NULL, servent_connect, (void*)cliid);
		client_thread = g_slist_prepend(client_thread, (gpointer)(*clithread));
		int ch = 'j';
		sleep(1);
		do {
			if(ch=='j')
				servent->post_type=JOIN_ID;
			else if(ch=='p')
				servent->post_type=PING_ID;
			else if(ch=='o')
				servent->post_type=PONG_ID;
			else if(ch=='m')
				servent->post_type=MESSAGE_ID;
			else if(ch=='s') {
				timer_thread = (pthread_t*)malloc(sizeof(pthread_t));
				pthread_create(timer_thread, NULL, servent_timer, NULL);
			}
			else if(ch=='c')
				servent->post_type=CREATE_ID;
			
			pthread_cond_signal(&servent->cond);
			printf("[servent_start]Premere f per bloccare l'invio\n");
			
		}while((ch=getchar())!='f');
	}

	printf("\nPremere q per uscire\n");
	while(getchar()!='q');
	kill_all_thread(1);
	return 0;
}

void servent_close_all(void) {
	
	int i;
	for(i=0; i<g_slist_length(server_fd); i++) {
		printf("[servent_close_all]Closing server %d\n", (int)g_slist_nth_data(server_fd, i));
		close((int)g_slist_nth_data(server_fd, i));
	}
	for(i=0; i<g_slist_length(client_fd); i++) {
		printf("[servent_close_all]Closing client %d\n", (int)g_slist_nth_data(client_fd, i));
		close((int)g_slist_nth_data(client_fd, i));
	}
	for(i=0; i<g_slist_length(server_connection_fd); i++) {
		printf("[servent_close_all]Closing server_connection %d\n", (int)g_slist_nth_data(server_connection_fd, i));
		close((int)g_slist_nth_data(server_connection_fd, i));
	}
	
}

void kill_all_thread(int sig) {
	printf("[killall_thread]Killing thread\n");
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

void servent_init(char *ip, u_int4 port, u_int1 status, u_int1 is_supernode) {
	
	servent_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	
	local_servent = (servent_data*)malloc(sizeof(servent_data));
	u_int8 id = local_servent->id = generate_id();
	printf("[servent_init]ID: %lld\n", local_servent->id);
	local_servent->ip = ip;
	local_servent->port = port;
	local_servent->status = status;
	local_servent->nick = "simone";
	local_servent->is_supernode = is_supernode;
	
	pthread_mutex_init(&local_servent->mutex, NULL);
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
	read_all(chat_hashtable, chatclient_hashtable);
}

void servent_close_supernode() {
	write_all(&chat_hashtable, MODE_TRUNC);
}

//---------THREAD---------------

//Thread che riceve le richieste di connessione e avvia nuovi thread
/* Ogni nuovo peer (client) che richiede di connettersi al server locale viene
 * assegnato ad un nuovo Thread che si occupera' di rispondere alle richieste del client.
 */
void *servent_listen(void *parm) {
	int connFd;
	pthread_t *thread;
	pthread_mutex_t *mutex;
	pthread_mutex_t *mutex_data;
	pthread_cond_t *cond;
	
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
	char *buffer = (char*)malloc(2000);
	http_packet *h_packet;
	int len;
	int fd = (int)parm;
	u_int4 status = 0;
	
	while(1) {
		sleep(1);
		memset(buffer, 0, 2000);
		len = switch_http_packet(fd, buffer, LP_READ);
		printf("[servent_responde]Data received, len: %d\n", len);
		
		if(len>0) {
			h_packet = http_char_to_bin(buffer);
			if(h_packet!=NULL) {
				printf("[servent_responde]h_packet not NULL, type=%d\n", h_packet->type);
				if(h_packet->type==HTTP_REQ_POST) {
					
					printf("[servent_responde]POST ricevuto\n");
									
					//Aggiunge le info in base al tipo di pacchetto
					if(h_packet->data==NULL || h_packet->data->header==NULL) {
						printf("[servent_responde]NULL\n");
						
						status = HTTP_STATUS_CERROR;
					}
					else if(h_packet->data->header->recv_id!=local_servent->id) {
						status = HTTP_STATUS_CERROR;
					}
					else if(h_packet->data->header->desc_id==JOIN_ID) {
						printf("[servent_responde]JOIN ricevuto\n");
						u_int8 id = h_packet->data->header->sender_id;
						servent_data *conn_servent = (servent_data*)malloc(sizeof(servent_data));
						conn_servent->ip = get_dest_ip(fd);
						conn_servent->port = get_dest_port(fd);
						conn_servent->timestamp = h_packet->data->header->timestamp;
						
						//TODO: Aggiungere l'ID alla lista degli utenti della chat
						u_int8 chat_id = GET_JOIN(h_packet->data)->chat_id;
						conn_servent->chat_list = g_slist_prepend(conn_servent->chat_list, (gpointer)chat_id);
						conn_servent->status = GET_JOIN(h_packet->data)->status;
						conn_servent->nick = h_packet->data->data;
						if(local_servent->is_supernode) {
							add_user(chat_id, conn_servent->id, conn_servent->nick, conn_servent->ip, conn_servent->port, chat_hashtable, &chatclient_hashtable);
						}
						
						//Si inizializzano il mutex e il cond
						pthread_mutex_init(&conn_servent->mutex, NULL);
						pthread_mutex_init(&conn_servent->mutex_data, NULL);
						pthread_cond_init(&conn_servent->cond, NULL);
						
						if(g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id))==NULL) {
							printf("[servent_responde]connection added to hashtable\n");
							g_hash_table_insert(servent_hashtable, (gpointer)to_string(id), (gpointer)conn_servent);
						}
						
						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==PING_ID) {
						printf("[servent_responde]PING ricevuto\n");
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						LOCK(h_packet->data->header->sender_id);
						conn_servent->status = GET_PING(h_packet->data)->status;
						conn_servent->timestamp = h_packet->data->header->timestamp;
						UNLOCK(h_packet->data->header->sender_id);
						
						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==PONG_ID) {
						printf("[servent_responde]PONG ricevuto\n");
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						LOCK(h_packet->data->header->sender_id);
						conn_servent->status = GET_PONG(h_packet->data)->status;
						conn_servent->timestamp = h_packet->data->header->timestamp;
						UNLOCK(h_packet->data->header->sender_id);
						
						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==LEAVE_ID) {
						printf("[servent_responde]LEAVE ricevuto\n");
						u_int8 chat_id = GET_LEAVE(h_packet->data)->chat_id;
						
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						LOCK(h_packet->data->header->sender_id);
						conn_servent->chat_list = g_slist_remove(conn_servent->chat_list, (gconstpointer)chat_id);
						conn_servent->timestamp = h_packet->data->header->timestamp;
						UNLOCK(h_packet->data->header->sender_id);
						
						status = HTTP_STATUS_OK;
						//Sconnetti dalla chat
					}
					else if(h_packet->data->header->desc_id==MESSAGE_ID) {
						printf("[servent_responde]MESSAGE ricevuto\n");
						
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));

						LOCK(h_packet->data->header->sender_id);
						conn_servent->msg = h_packet->data->data;
						conn_servent->msg_len = h_packet->data->header->data_len;
						conn_servent->timestamp = h_packet->data->header->timestamp;
						printf("[servent_responde]msg: %s, msg_len: %d\n", dump_data(conn_servent->msg, conn_servent->msg_len), conn_servent->msg_len);
						UNLOCK(h_packet->data->header->sender_id);

						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==SEARCH_ID) {
						printf("[servent_responde]SEARCH ricevuto\n");
						
						LOCK(local_servent->id);
						int res=0;
						//TODO: ricerca nella lista delle chat che possiede
						if(res>=0) {
							status = HTTP_STATUS_OK;
							send_post_response_packet(fd, status, 4, "test");
						}
						
						//TODO: Rinvia il pacchetto agli altri peer se il ttl non è 0
						
						status = -1;
					}
					else if(h_packet->data->header->desc_id==CREATE_ID) {
						printf("[servent_responde]CREATE ricevuto\n");
						u_int8 chat_id = GET_CREATE(h_packet->data)->chat_id;
						u_int4 title_len = h_packet->data->header->data_len;
						char *title = h_packet->data->data;
						
						LOCK(local_servent->id);
						if(local_servent->is_supernode) {
							add_chat(chat_id, title, &chat_hashtable);
							//TODO: eventualmente aggiungere un servent_data ????
							
							chat *test = (chat*)g_hash_table_lookup(chat_hashtable, (gconstpointer)to_string(chat_id));
	
							printf("[servent_responde]chat: %lld\n", test->id);
						}
						
						UNLOCK(local_servent->id);
						
						status = HTTP_STATUS_OK;
						//Crea chat
					}
					
					//Invio la conferma di ricezione
					if(status>=0) {
						printf("[servent_responde]sending\n");
						send_post_response_packet(fd, status, 0, NULL);
					}
						
				}
				else if(h_packet->type==HTTP_REQ_GET) {
					printf("[servent_responde]GET ricevuto\n");
				}
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
	
	u_int8 id_dest = (int)parm;
	//Si prendono l'ip e la porta dalla lista degli id
	servent_data *servent_peer = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id_dest));
	char *ip_dest = servent_peer->ip;
	u_int4 port_dest = servent_peer->port;
	
	int fd = servent_create_client(ip_dest, port_dest);
	
	pthread_mutex_t *mutex = &servent_peer->mutex;
	pthread_cond_t *cond = &servent_peer->cond;	
	
	client_fd = g_slist_prepend(client_fd, (gpointer)fd);
	
	printf("[servent_connect]Init\n");
	
	u_int4 post_type;
	u_int8 chat_id_req;
	u_int4 msg_len;
	char *msg;
	u_int4 title_len;
	char *title;
	
	u_int1 status;
	char *nick;
	
	//Ora si entra nel ciclio infinito che serve per inviare tutte le richieste
	while(1) {
		pthread_cond_wait(cond, mutex); //In attesa di un segnale che indichi la necessita di comunicare con il peer
		printf("[servent_connect]socket %d, Signal received\n", fd);
		
		LOCK(id_dest);
		post_type = servent_peer->post_type;
		chat_id_req = servent_peer->chat_id_req;
		msg_len = servent_peer->msg_len;
		msg = servent_peer->msg;
		title = servent_peer->title;
		title_len = servent_peer->title_len;
		printf("[servent_connect]id_dest: %lld, post_type=%d\n", servent_peer->id, servent_peer->post_type);
		UNLOCK(id_dest);
		
		LOCK(local_servent->id);
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
				send_ping_packet(fd, local_servent->id, id_dest, status);
			}
			else if(post_type==PONG_ID) {
				send_pong_packet(fd, local_servent->id, id_dest, status);
			}
			else if(post_type==LEAVE_ID) {
				send_ping_packet(fd, local_servent->id, id_dest, chat_id_req);
			}
			else if(post_type==MESSAGE_ID) {
				send_message_packet(fd, local_servent->id, id_dest, chat_id_req, msg_len, msg);
			}
			else if(post_type==CREATE_ID) {
				send_create_packet(fd, local_servent->id, id_dest, chat_id_req, title_len, title);
			}
		
			memset(buffer, 0, 2000);
			printf("[servent_connect]listening\n");
			len = switch_http_packet(fd, buffer, LP_READ);
			printf("[sevente_connect]received\n");
			if(len>0) {
				h_packet = http_char_to_bin(buffer);
				if(h_packet!=NULL && h_packet->type==HTTP_RES_POST) {
					if(strcmp(h_packet->header_response->response, HTTP_OK)==0) {
						printf("[servent_connect/%d]OK POST received\n", fd);
					}
					else {
						printf("[servent_connect/%d]Error\n", fd);
					}
				}
			}
		}
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
				LOCK(data->id);
				printf("[servent_timer]Locked\n");
			
				data->post_type = PING_ID;
				
				printf("[servent_timer]Signaling %lld\n", data->id);
				pthread_cond_signal(&data->cond);
				UNLOCK(data->id);
				printf("[servent_timer]Unlocked\n");
			}
		}
		
		printf("[servent_timer]Sleeping\n");
		sleep(timer_interval);
	}
}
