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

/*
 * Developed by Notargiacomo Simone
 * TorTella socket interface
 */
#include "socketmanager.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Crea un socket di connessione remota ovvero
 * si connette ad un server remoto.
 */
int create_tcp_socket(const char *dst_ip, int dst_port)
{
	struct sockaddr_in sAddr;
	int sockfd;

	// creazione socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		logger(SOCK_INFO, "[create_tcp_socket]Errore nella creazione del socket tcp");
		return -1;
	}
	memset((char *) &sAddr, 0, sizeof(sAddr));
	sAddr.sin_family = AF_INET;
	if (dst_port > 0)
		sAddr.sin_port = htons(dst_port);
	else {
		logger(SOCK_INFO, "[create_tcp_socket]Errore nella porta: [%d]\n", dst_port);
		return -1;
	}

	if (inet_pton(AF_INET, dst_ip, &sAddr.sin_addr) <= 0) {
		logger(SOCK_INFO, "[create_tcp_socket]Errore nella conversione dell'indirizzo IP: [%s]\n", dst_ip);
		return -1;
	}
	if (connect(sockfd, (struct sockaddr *) &sAddr, sizeof(sAddr)) < 0) {
		logger(SOCK_INFO, "[create_tcp_socket]Unable to connect to: %s:%d\n", dst_ip, dst_port);
		return -1;
	}

	int reuse = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
		logger(SOCK_INFO, "[create_tcp_socket]Error in setsockopt SO_REUSEADDR\n");
		return -1;
	}
	if(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &reuse, sizeof(int)) < 0) {
		logger(SOCK_INFO, "[create_tcp_socket]Error in setsockopt SO_KEEPALIVE\n");
		return -1;
	}

	return sockfd;
}

/*
 * Crea un socket d'ascolto ovver un server in attesa di connessioni.
 */
int create_listen_tcp_socket(const char *src_ip, int src_port)
{
	int listenfd = 0;
	struct sockaddr_in sAddr;
	// creazione socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		logger(SOCK_INFO, "[create_listen_tcp_socket]Errore nella creazione del socket tcp");
		return -1;
	}

	memset((char *) &sAddr, 0, sizeof(sAddr));
	sAddr.sin_family = AF_INET;
	if (src_port > 0)
		sAddr.sin_port = htons(src_port);
	else {
		logger(SOCK_INFO, "[create_listen_tcp_socket]Errore nella porta: [%d]\n", src_port);
		return -1;
	}

	if (inet_pton(AF_INET, src_ip, &sAddr.sin_addr) <= 0) {
		logger(SOCK_INFO, "[create_listen_tcp_socket]Errore nella conversione dell'indirizzo IP: [%s]\n", src_ip);
		return -1;
	}

	int reuse = 1;
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
		logger(SOCK_INFO, "[create_listen_tcp_socket]Error in setsockopt SO_REUSEADDR\n");
		return -1;
	}
	if(setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &reuse, sizeof(int)) < 0) {
		logger(SOCK_INFO, "[create_listen_tcp_socket]Error in setsockopt SO_KEEPALIVE\n");
		return -1;
	}

	// Binding
	if ((bind(listenfd, (struct sockaddr *) &sAddr, sizeof(sAddr))) < 0) {
		logger(SOCK_INFO, "[create_listen_tcp_socket]Bind error\n");
		return -1;
	}
	// Listen
	if (listen(listenfd, conf_get_qlen()) < 0) {
		logger(SOCK_INFO, "[create_listen_tcp_socket]Listen error\n");
		return -1;
	}

	return listenfd;
}

/*
 * Elimina un socket ovvero chiude una connessione.
 */
int delete_socket(int sock_descriptor)
{
	if (sock_descriptor < 0) {
		logger(SOCK_INFO, "[delete_socket]SocketDescriptor error: [%d]", sock_descriptor);
		return -1;
	}

	// if( shutdown(sock_descriptor, SHUT_RDWR) < 0)
	if (close(sock_descriptor) < 0) {
		logger(SOCK_INFO, "[delete_socket]Socket shutdown error");
		return -1;
	}
	return 0;
}

int listen_http_packet(int listen_socket)
{
	int connFd = 0;

	if ((connFd=accept(listen_socket, (struct sockaddr *) NULL, NULL)) < 0) {
		logger(SOCK_INFO, "[listen_http_packet]Errore nell'accept\n");
		return -1;
	}

	return connFd;
}

int recv_http_packet(int connFd, char **buffer) {
	int len = 0;
	if ((len=recv_packet(connFd, buffer)) < 0) {
		logger(SOCK_INFO, "[recv_http_packet]Errore in lettura!");
		return -1;
	}
	if(*buffer==NULL) {
		logger(SOCK_INFO, "[recv_http_packet]buffer null\n");
		return -2;
	}

	return len;
}

int send_packet(int sock_descriptor, char *buffer, int len)
{
	int char_write = 0;
	if (sock_descriptor < 0) {
		logger(SOCK_INFO, "[send_packet]Socket descriptor not valid, sock_descriptor = %d", sock_descriptor);
		return -1;
	}
	// Questa blocco si potrebbe ritentare per n volte, dove n e' un
	// parametro di configurazione.
	if ((char_write=write(sock_descriptor, buffer, len)) != len) {
		logger(SOCK_INFO, "[send_packet]Perdita dati in trasmissione");
		return -2;
	}

	return char_write;
}

int recv_packet(int sock_descriptor, char **buffer)
{
	return recv_sized_packet(sock_descriptor, buffer, conf_get_buffer_len());
}

int recv_sized_packet(int sock_descriptor, char **buf, int max_len)
{
	u_int4 char_read = 0;
	int flag = 0;
	int len = 0;
	if (max_len < 0)
		return -1;

	char *tmp = calloc(max_len, 1);
	*buf = calloc(max_len, 1);
	char *buffer = *buf;
	char *iter = buffer;
	char *buf2;

	if (sock_descriptor < 0) {
		logger(SOCK_INFO, "[recv_sized_packet]Socket descriptor not valid, sock_descriptor = %d", sock_descriptor);
		return -1;
	}

	while(!flag) {
		char_read = read(sock_descriptor, tmp, max_len);
		if(char_read==0) {
			flag=1;
			continue;
		}
		len += char_read;

		buf2 = calloc(len-char_read, 1);
		memcpy(buf2, buffer, len-char_read);
		//free(buffer);
		buffer = calloc(len, 1);
		memcpy(buffer, buf2, len-char_read);
		//free(buf2);

		memcpy(iter, tmp, char_read);
		iter += char_read;
		memset(tmp, 0, max_len);

		if(char_read < max_len){
			flag = 1;
		}
	}
	
	if (len < 0) {
		logger(SOCK_INFO, "[recv_sized_packet]read error");
		return -1;
	}
	return len;
}

char *get_dest_ip(int socket) {
	struct sockaddr_in peer;
	memset((char *) &peer, 0, sizeof(peer));
	u_int4 peer_len;
	/* We must put the length in a variable.              */
	peer_len = sizeof(peer);
	/* Ask getpeername to fill in peer's socket address.  */
	if (getpeername(socket, (struct sockaddr *)&peer, &peer_len) == -1) {
		logger(SOCK_INFO, "[get_dest_ip]Failed\n");
		return NULL;
	}

	/* Print it. The IP address is often zero because     */
	/* sockets are seldom bound to a specific local       */
	/* interface.                                         */
	logger(SOCK_INFO, "[get_dest_ip]Peer's IP address is: %s\n", inet_ntoa(peer.sin_addr));
	return inet_ntoa(peer.sin_addr);
}

u_int4 get_dest_port(int socket) {
	struct sockaddr_in peer;
	memset((char *) &peer, 0, sizeof(peer));
	u_int4 peer_len;
	/* We must put the length in a variable.              */
	peer_len = sizeof(peer);
	/* Ask getpeername to fill in peer's socket address.  */
	if (getpeername(socket, (struct sockaddr *)&peer, &peer_len) == -1) {
		logger(SOCK_INFO, "[get_dest_port]Failed\n");
		return -1;
	}

	/* Print it. The IP address is often zero because     */
	/* sockets are seldom bound to a specific local       */
	/* interface.                                         */
	logger(SOCK_INFO, "[get_dest_port]Peer's port is: %d\n", (int) ntohs(peer.sin_port));
	return (u_int4) ntohs(peer.sin_port);
}
