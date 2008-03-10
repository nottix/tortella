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
	return create_listen_tcp_socket(src_ip, src_port);
}

//Crea un client socket
u_int4 servent_create_client(char *dst_ip, u_int4 dst_port) {
	return create_tcp_socket(dst_ip, dst_port);
}

u_int4 servent_start() {
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
	
	char *id_dest = "22222222";
	char *ip_dest = "127.0.0.1";
	u_int4 port_dest = 2110;
	
	//Avvia il server
	pthread_t *server_thread;
	int server_fd = servent_create_server("127.0.0.1", 2110); //Locale
	pthread_create(server_thread, NULL, servent_listen_and_responde, (void*)server_fd);
	
	//Avvia il client
	pthread_t *client_thread;
	int client_fd = servent_create_client(ip_dest, port_dest);
	
	//Invio pacchetto di JOIN
	send_join_packet(client_fd, id, id_dest, ONLINE_ID, chat_id);
	last_request_type = HTTP_REQ_POST;
}

/*u_int4 servent__to(u_int8 id_dest) {
	//Si prendono l'ip e la porta dalla lista degli id
	char *ip_dest = "127.0.0.1";
	u_int4 port_dest = 2110;
	
	int fd = servent_create_client(ip_dest, port_dest);
}*/

//---------THREAD---------------

//Thread che riceve i pacchetti e risponde adeguatamente
u_int4 servent_listen_and_responde(void *parm) {
	
	char *buffer = (char*)malloc(RECV_MAX_LEN);
	char *h_packet_char;
	http_packet *h_packet;
	while(1) {
		h_packet_char = listen_http_packet((int)parm, buffer, LP_READ);
		if(h_packet_char!=NULL) {
			h_packet = http_char_to_bin(h_packet_char);
			if(h_packet!=NULL) {
				if(h_packet->type==HTTP_REQ_POST) {
					printf("POST\n");
				}
				else if(h_packet->type==HTTP_RES_POST && last_request_type==HTTP_REQ_POST) {
					printf("PORT OK\n");
				}
			}
		}
	}
	
}
