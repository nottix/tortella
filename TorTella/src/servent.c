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
	//server_fd = list_add_int(server_fd, sock);
	return sock;
}

//Crea un client socket
u_int4 servent_create_client(char *dst_ip, u_int4 dst_port) {
	printf("[servent_create_client]Creating tcp socket on: %s:%d\n", dst_ip, dst_port);
	u_int4 sock = create_tcp_socket(dst_ip, dst_port);
	//client_fd = list_add_int(client_fd, sock);
	return sock;
}

u_int4 servent_start(char *ip, u_int4 port) {
	servent_init(ip, port, ONLINE_ID);
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
		/*//Avvia il client
		int clifd = servent_create_client(ip, port);
		if(clifd<0) {
			printf("[servent_start]Errore nella creazione del client\n");
			return -1;
		}
		//client_fd = list_add_int(client_fd, clifd);
		client_fd = g_slist_prepend(client_fd, (gpointer)clifd);*/
		
		
		pthread_t *clithread = (pthread_t*)malloc(sizeof(pthread_t));
		u_int8 cliid;
		printf("Inserisci l'ID del peer: ");
		scanf("%lld", &cliid);
		servent_data *servent = (servent_data*)malloc(sizeof(servent_data));
		servent->id = cliid;
		servent->ip = ip;
		servent->port = port;
		servent->chat_id_req = 111;
		servent->post_type=JOIN_ID;
		g_hash_table_insert(servent_hashtable, (gpointer)to_string(cliid), (gpointer)servent);
		pthread_create(clithread, NULL, servent_connect, (void*)cliid);
		client_thread = g_slist_prepend(client_thread, (gpointer)(*clithread));
		sleep(1);
		do {
			pthread_cond_signal(&servent->cond);
			printf("[servent_start]Premere f per bloccare l'invio\n");
			
		}while(getchar()!='f');
	}

	printf("\nPremere q per uscire\n");
	while(getchar()!='q');
	kill_all_thread(1);
	return 0;
}

void servent_close_all(void) {
	/*int server_len = list_size(server_fd);
	int server_connection_len = list_size(server_connection_fd);
	int client_len = list_size(client_fd);
	
	int i;
	for(i=0; i<server_len; i++)
		close(LIST_GET_INT(server_fd, i));
	
	for(i=0; i<server_connection_len; i++)
		close(LIST_GET_INT(server_connection_fd, i));
	
	for(i=0; i<client_len; i++)
		close(LIST_GET_INT(client_fd, i));*/
	
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
	
	/*int server_len = list_size(server_thread);
	int server_connection_len = list_size(server_connection_thread);
	int client_len = list_size(client_thread);
	int i;
	for(i=0; i<server_len; i++)
		pthread_kill(*LIST_GET_THREAD(server_thread, i), SIGKILL);
	
	for(i=0; i<server_connection_len; i++)
		pthread_kill(*LIST_GET_THREAD(server_connection_thread, i), SIGKILL);

	for(i=0; i<client_len; i++)
		pthread_kill(*LIST_GET_THREAD(client_thread, i), SIGKILL);*/
	
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
	
	/*int len = list_size(connection_list);
	int i;
	for(i=0; i<len; i++) {
		printf("[kill_all_thread]closing %d\n", (LIST_GET_CONN(connection_list, i))->fd);
		close((LIST_GET_CONN(connection_list, i))->fd);
		if(((LIST_GET_CONN(connection_list, i))->thread)==NULL)
		   printf("NULL\n");
		pthread_kill(*((LIST_GET_CONN(connection_list, i))->thread), SIGKILL);
	}*/
}

void servent_init(char *ip, u_int4 port, u_int1 status) {
	//pthread_mutex_init(wait_response_mutex, NULL);
	
	servent_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	//servent_hashtable = g_hash_table_new(g_direct_hash, g_direct_equal);
	
	local_servent = (servent_data*)malloc(sizeof(servent_data));
	u_int8 id = local_servent->id = generate_id();
	printf("[servent_init]ID: %lld\n", local_servent->id);
	local_servent->ip = ip;
	local_servent->port = port;
	local_servent->status = status;
	
	pthread_mutex_init(&local_servent->mutex, NULL);
	pthread_cond_init(&local_servent->cond, NULL);
	
	g_hash_table_insert(servent_hashtable, (gpointer)to_string(id), (gpointer)local_servent);
	//printf("[servent_init]Retrieving\n");
	//servent_data *ret = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(local_servent->id));
	//printf("[servent_init]Init with ID: %lld\n", ret->id);
	
	server_fd = NULL;
	client_fd = NULL;
	server_connection_fd = NULL;
	
	server_thread = NULL;
	client_thread = NULL;
	server_connection_thread = NULL;
}
/*
u_int4 servent_add_chat(servent_data *s_data, u_int8 id, u_int8 chat_id) {
	//servent_data *s_data = g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id));
	s_data->chat_list = g_slist_prepend(s_data->chat_list, (gpointer)chat_id);
	return 1;
}*/

//---------THREAD---------------

//Thread che riceve le richieste di connessione e avvia nuovi thread
/* Ogni nuovo peer (client) che richiede di connettersi al server locale viene
 * assegnato ad un nuovo Thread che si occupera' di rispondere alle richieste del client.
 */
void *servent_listen(void *parm) {
	int connFd;
	pthread_t *thread;
	pthread_mutex_t *mutex;
	pthread_cond_t *cond;
	
	while(1) {
		connFd = listen_http_packet((int)parm);
		printf("[servent_listen]Connessione ricevuta, socket: %d\n", connFd);
		if(connFd!=0) {
			thread = (pthread_t*)malloc(sizeof(pthread_t));
			//list_add_int(server_connection_fd, connFd);
			server_connection_fd = g_slist_prepend(server_connection_fd, (gpointer)connFd);
			pthread_create(thread, NULL, servent_responde, (void*)connFd);
			//list_add_thread(server_connection_thread, thread);
			server_connection_thread = g_slist_prepend(server_connection_thread, (gpointer)(*thread));
			
		/*	thread = (pthread_t*)malloc(sizeof(pthread_t));
			pthread_create(thread, NULL, servent_responde, (void*)connFd);
			mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
			cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
			pthread_mutex_init(mutex, NULL);
			pthread_cond_init(cond, NULL);
			printf("[servent_listen]here\n");
			conn_data *conn;
			conn = (conn_data*)malloc(sizeof(conn_data));
			conn->fd = connFd;
			conn->thread = thread;
			conn->mutex = mutex;
			conn->cond = cond;
			connection_list = list_add_conn(connection_list, conn);*/
		}
	}
	
	pthread_exit(NULL);
}

//Riceve i pacchetti da un peer e li gestisce
void *servent_responde(void *parm) {
	char *buffer = (char*)malloc(2000);
	//char *h_packet_char;
	http_packet *h_packet;
	int len;
	int fd = (int)parm;
	
	while(1) {
		sleep(1);
		memset(buffer, 0, 2000);
		//buffer = (char*)malloc(2000); //TODO: temp
		len = switch_http_packet(fd, buffer, LP_READ); //TODO: Ciclio infinito!!!
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
					}
					if(h_packet->data->header->desc_id==JOIN_ID) {
						u_int8 id = h_packet->data->header->sender_id;
						servent_data *conn_servent = (servent_data*)malloc(sizeof(servent_data));
						conn_servent->ip = get_dest_ip(fd);
						conn_servent->port = get_dest_port(fd);
						
						//TODO: Aggiungere l'ID alla lista degli utenti della chat
						u_int8 chat_id = GET_JOIN(h_packet->data)->chat_id;
						conn_servent->chat_list = g_slist_prepend(conn_servent->chat_list, (gpointer)chat_id);
						conn_servent->status = GET_JOIN(h_packet->data)->status;
						
						//Si inizializzano il mutex e il cond
						pthread_mutex_init(&conn_servent->mutex, NULL);
						pthread_cond_init(&conn_servent->cond, NULL);
						
						if(g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id))==NULL) {
							printf("[servent_responde]connection added to hashtable\n");
							g_hash_table_insert(servent_hashtable, (gpointer)to_string(id), (gpointer)conn_servent);
						}
					}
					else if(h_packet->data->header->desc_id==PING_ID) {
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						conn_servent->status = GET_PING(h_packet->data)->status;
					}
					else if(h_packet->data->header->desc_id==PONG_ID) {
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						conn_servent->status = GET_PONG(h_packet->data)->status;
					}
					else if(h_packet->data->header->desc_id==LEAVE_ID) {
						u_int8 chat_id = GET_LEAVE(h_packet->data)->chat_id;
						
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						conn_servent->chat_list = g_slist_remove(conn_servent->chat_list, (gconstpointer)chat_id);
						//Sconnetti dalla chat
					}
					else if(h_packet->data->header->desc_id==MESSAGE_ID) {
						//Prende il messaggio
						//conn_servent->status = GET_MESSAGE(h_packet->data);
					}
					
					//Invio la conferma di ricezione
					printf("[servent_responde]sending\n");
					send_post_response_packet(fd, HTTP_STATUS_OK);
				}
				else if(h_packet->type==HTTP_REQ_GET) {
					printf("GET REQ\n");
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
	
	//Ora si entra nel ciclio infinito che serve per inviare tutte le richieste
	while(1) {
		pthread_cond_wait(cond, mutex); //In attesa di un segnale che indichi la necessita di comunicare con il peer
		printf("[servent_connect]socket %d, Signal received\n", fd);
		//TODO: ogni volta che si accede a local_servent bisogna prendere un lock
		printf("[servent_connect]id_dest: %lld, post_type=%d\n", servent_peer->id, servent_peer->post_type);
		
		
		if(servent_peer->post_type==HTTP_REQ_GET) {
			//send_get_request_packet(fd, char *filename, u_int4 range_start, u_int4 range_end);
		}
		else {
			
			if(servent_peer->post_type==JOIN_ID) {
				send_join_packet(fd, local_servent->id, id_dest, local_servent->status, local_servent->chat_id_req);
			}
			else if(servent_peer->post_type==PING_ID) {
				send_ping_packet(fd, local_servent->id, id_dest, local_servent->status);
			}
			else if(servent_peer->post_type==PONG_ID) {
				send_pong_packet(fd, local_servent->id, id_dest, local_servent->status);
			}
			else if(servent_peer->post_type==LEAVE_ID) {
				send_ping_packet(fd, local_servent->id, id_dest, local_servent->chat_id_req);
			}
		
			memset(buffer, 0, 2000);
			//buffer = (char*)malloc(2000); //TODO: temp
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
