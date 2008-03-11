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
	server_fd = list_add_int(server_fd, sock);
	return sock;
}

//Crea un client socket
u_int4 servent_create_client(char *dst_ip, u_int4 dst_port) {
	printf("[servent_create_client]Creating tcp socket on: %s:%d\n", dst_ip, dst_port);
	u_int4 sock = create_tcp_socket(dst_ip, dst_port);
	client_fd = list_add_int(client_fd, sock);
	return sock;
}

u_int4 servent_start(char *ip, u_int4 port) {
	//Fase iniziale di reperimento dell'ID della chat e degli utenti che ne fanno parte
	/*
	 * char_id
	 * 21101959
	 *
	 * ip:port-id
	 * 127.0.0.1:2110-11111111
	 * ...
	 */
	//Si registra tale lista in memoria
	
	//Esempio di dati
	u_int8 chat_id = 21101959;
	u_int8 id = 11111111;
	
	u_int8 id_dest = 22222222;
	char *ip_dest = "127.0.0.1";
	u_int4 port_dest = 2110;
	
	//Avvia il server
	int serfd = servent_create_server(ip, port); //Locale
	if(serfd<0) {
		printf("[servent_start]Errore nella creazione del server\n");
		return -1;
	}
	pthread_t *serthread;
	pthread_create(serthread, NULL, servent_listen, (void*)serfd);
	list_add_thread(server_thread, serthread);
	
	//Avvia il client
	clifd = servent_create_client(ip_dest, port_dest);
	if(clifd<0) {
		printf("[servent_start]Errore nella creazione del client\n");
		return -1;
	}
	
	//Invio pacchetto di JOIN
	send_join_packet(clifd, id, id_dest, ONLINE_ID, chat_id);
	last_request_type = HTTP_REQ_POST;
	
	//signal(SIGKILL, kill_all_thread);
	sleep(6);
	kill_all_thread(1);
	return 0;
}

void servent_close_all(void) {
	int server_len = list_size(server_fd);
	int server_connection_len = list_size(server_connection_fd);
	int client_len = list_size(client_fd);
	
	int i;
	for(i=0; i<server_len; i++)
		close(LIST_GET_INT(server_fd, i));
	
	for(i=0; i<server_connection_len; i++)
		close(LIST_GET_INT(server_connection_fd, i));
	
	for(i=0; i<client_len; i++)
		close(LIST_GET_INT(client_fd, i));
}

void kill_all_thread(int sig) {
	printf("[killall_thread]Killing thread\n");
	servent_close_all();
	pthread_kill(server_thread[0], SIGKILL);
	pthread_kill(server_connection_thread[0], SIGKILL);
}

/*u_int4 servent__to(u_int8 id_dest) {
	//Si prendono l'ip e la porta dalla lista degli id
	char *ip_dest = "127.0.0.1";
	u_int4 port_dest = 2110;
	
	int fd = servent_create_client(ip_dest, port_dest);
}*/

//---------THREAD---------------

//Thread che riceve le richieste di connessione e avvia nuovi thread
void *servent_listen(void *parm) {
	int connFd;
	
	while(1) {
		connFd = listen_http_packet((int)parm);
		printf("[servent_listen]Connessione ricevuta, socket: %d\n", connFd);
		if(connFd!=0) {
			server_connection_fd[0] = connFd;
			pthread_create(&server_connection_thread[server_connection_num++], NULL, servent_responde, (void*)connFd);
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
	
	while(1) {
		printf("[servent_responde]Connection listening\n");
		len = switch_http_packet((int)parm, buffer, LP_READ);
		printf("[servent_responde]Connection thread started, len: %d\n", len);
		if(len>0) {
			h_packet = http_char_to_bin(buffer);
			if(h_packet!=NULL) {
				if(h_packet->type==HTTP_REQ_POST) {
					printf("POST\n");
				}
				else if(h_packet->type==HTTP_RES_POST && last_request_type==HTTP_REQ_POST) {
					printf("POST OK\n");
				}
			}
		}
	}
	pthread_exit(NULL);
}
