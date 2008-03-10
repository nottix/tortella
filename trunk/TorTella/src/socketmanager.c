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

#define DEBUG

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
		fprintf(stderr, "\nErrore nella creazione del socket tcp");
	return (1);
    }
#ifdef DEBUG
    fprintf(stdout, "\n[create_tcp_socket]socket creato\n");
#endif
    memset((char *) &sAddr, 0, sizeof(sAddr));
    sAddr.sin_family = AF_INET;
    if (dst_port > 0)
		sAddr.sin_port = htons(dst_port);
    else
		fprintf(stderr, "\nerrore nella porta: [%d]\n", dst_port);

    if (inet_pton(AF_INET, dst_ip, &sAddr.sin_addr) <= 0) {
		fprintf(stderr, "\nErrore nella conversione dell'indirizzo IP: [%s]\n", dst_ip);
		return (1);
    }
    if (connect(sockfd, (struct sockaddr *) &sAddr, sizeof(sAddr)) < 0) {
		fprintf(stderr, "\nUnable to connect to: %s:%d\n", dst_ip, dst_port);
		return (1);
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
		fprintf(stderr, "\nErrore nella creazione del socket tcp");
	return (1);
    }
#ifdef DEBUG
    fprintf(stdout, "\n[create_listen_tcp_socket]socket creato\n");
#endif
    memset((char *) &sAddr, 0, sizeof(sAddr));
    sAddr.sin_family = AF_INET;
    if (src_port > 0)
		sAddr.sin_port = htons(src_port);
    else
		fprintf(stderr, "\nerrore nella porta: [%d]\n", src_port);

    if (inet_pton(AF_INET, src_ip, &sAddr.sin_addr) <= 0) {
		fprintf(stderr, "\nErrore nella conversione dell'indirizzo IP: [%s]\n", src_ip);
		return (-1);
    }
    // Binding
    if ((bind(listenfd, (struct sockaddr *) &sAddr, sizeof(sAddr))) < 0) {
		fprintf(stderr, "\nBind error\n");
		return (-1);
    }
    // Listen
    if (listen(listenfd, QLEN) < 0) {
		fprintf(stderr, "\nListen error\n");
		return (-1);
    }

    return listenfd;
}

/*
 * Elimina un socket ovvero chiude una connessione.
 */
int delete_socket(int sock_descriptor)
{
    if (sock_descriptor < 0)
		fprintf(stderr, "\nSocketDescriptor error: [%d]", sock_descriptor);

    // if( shutdown(sock_descriptor, SHUT_RDWR) < 0)
    if (close(sock_descriptor) < 0) {
		fprintf(stderr, "\nSocket shutdown error");
		return (1);
    }
    return 0;
}

int listen_packet(int listen_socket, char *buffer, unsigned int mode)
{
    int connFd = 0;
    // for(;;)
    // {
    if ((connFd=accept(listen_socket, (struct sockaddr *) NULL, NULL)) < 0) {
		fprintf(stderr, "\nerrore nell'accept\n");
		return (-1);
    }
    // }
#ifdef DEBUG
    fprintf(stdout, "\n[listen_packet]Nuova connessione ricevuta, modalita' %d!\n", mode);
#endif
    switch (mode) {
    	case LP_WRITE:
#ifdef DEBUG
			fprintf(stdout, "\n[listen_packet]Invio dati...\n");
#endif
			if (write(connFd, buffer, strlen(buffer)) != strlen(buffer)) {
	    		fprintf(stderr, "\nErrore in scrittura!");
			}
#ifdef DEBUG
			fprintf(stdout, "\n[listen_packet]Dati inviati\n");
#endif
			break;
    	case LP_READ:
			if (recv_packet(connFd, buffer) < 0) {
	    		fprintf(stderr, "\nErrore in lettura!");
			}
#ifdef DEBUG
			fprintf(stdout, "\n[listen_packet]Dati ricevuti\n");
#endif
			break;
    	case LP_NONE:
			break;
    	default:
			fprintf(stderr, "\nno selection\n");
    }
    return connFd;
}

int listen_http_packet(int listen_socket, char *buffer, unsigned int mode)
{
    int connFd = 0;
    // for(;;)
    // {
    if ((connFd=accept(listen_socket, (struct sockaddr *) NULL, NULL)) < 0) {
		fprintf(stderr, "\nerrore nell'accept\n");
		return (-1);
    }
    // }
#ifdef DEBUG
    fprintf(stdout, "\n[listen_packet]Nuova connessione ricevuta, modalita' %d!\n", mode);
#endif
	int len;
    switch (mode) {
    	case LP_WRITE:
#ifdef DEBUG
			fprintf(stdout, "\n[listen_packet]Invio dati...\n");
#endif
			if (write(connFd, buffer, strlen(buffer)) != strlen(buffer)) {
	    		fprintf(stderr, "\nErrore in scrittura!");
			}
#ifdef DEBUG
			fprintf(stdout, "\n[listen_packet]Dati inviati\n");
#endif
			break;
    	case LP_READ:
			if (recv_http_packet(connFd, buffer, &len) < 0) {
	    		fprintf(stderr, "\nErrore in lettura!");
			}
#ifdef DEBUG
			fprintf(stdout, "\n[listen_packet]Dati ricevuti\n");
#endif
			break;
    	case LP_NONE:
			break;
    	default:
			fprintf(stderr, "\nno selection\n");
    }
    return connFd;
}

// Usando la libreria string.h, si potrebbe omettere il parametro len
int send_packet(int sock_descriptor, char *buffer, int len)
{
    int char_write = 0;
    if (sock_descriptor < 0) {
		fprintf(stderr, "[send_packet]Socket descriptor not valid, sock_descriptor = %d", sock_descriptor);
		return -1;
    }
    // Questa blocco si potrebbe ritentare per n volte, dove n e' un
    // parametro di configurazione.
    if ((char_write=write(sock_descriptor, buffer, len)) != len) {
		fprintf(stderr, "\n[send_packet]Perdita dati in trasmissione");
		return -2;
    }
#ifdef DEBUG
    fprintf(stdout, "\n[send_packet]char_write = %d\n", char_write);
#endif
    return char_write;
}

// Buffer e' gia allocato?
int recv_packet(int sock_descriptor, char *buffer)
{
    return recv_sized_packet(sock_descriptor, buffer, BUFFER_LEN);
}

int recv_sized_packet(int sock_descriptor, char *buffer, int max_len)
{
    unsigned int char_read = 0;
    unsigned int counter = 0;
    if (max_len < 0)
		return -1;
    if (buffer == 0)
		return -1;

    memset(buffer, 0, max_len);
#ifdef DEBUG
    fprintf(stdout, "\n[recv_packet]buffer_ptr %x\n", buffer);
#endif
    if (sock_descriptor < 0) {
		fprintf(stderr, "[recv_packet]Socket descriptor not valid, sock_descriptor = %d", sock_descriptor);
		return -1;
    }
	
    while ((char_read = read(sock_descriptor, buffer, max_len)) > 0) {
#ifdef DEBUG
		fprintf(stdout, "\n[recv_packet]char_read = %d\n", char_read);
#endif
		counter += char_read;
		if ((counter >= max_len)) {
	    	// questo vuol dire che sono ancora presenti caratteri sul
	    	// socket ( read > 0 )
		    // ma il buffer allocato non ha piu' spazio a disposizione
		    // si potrebbe creare una struttura buffer dinamica.
		    // Un buffer a due dimensioni [n][BUFFER_LEN].
#ifdef DEBUG
	    	fprintf(stderr, "\n[recv_packet]Buffer overflow!, perdita dati\n");
#endif
		}
		// fine carattere
		buffer[char_read] = 0;
    }
    if (char_read < 0) {
		fprintf(stderr, "\n[recv_packet]read error");
		return -1;
    }
#ifdef DEBUG
    fprintf(stdout, "\n[recv_packet]buffer read = %s\n", buffer);
#endif
    return char_read;
}

/*
 * buffer deve essere allocato ad una grandezza abbastanza sufficiente
 */
char *recv_http_packet(int sock_descriptor,char *buffer, int *len) {
	u_int4 char_read = 0;
    u_int4 counter = 0;

#ifdef DEBUG
    fprintf(stdout, "\n[recv_http_packet]buffer_ptr %x\n", buffer);
#endif
    if (sock_descriptor < 0) {
		fprintf(stderr, "[recv_http_packet]Socket descriptor not valid, sock_descriptor = %d", sock_descriptor);
		return NULL;
    }
	
	char ch, old_ch = -1;
	char line[100];
	char *iter = buffer;
	char *content_len;
	u_int4 length = 0, data_len, start_data=0, data_counter=0;
    while (((char_read = read(sock_descriptor, &ch, 1)) > 0) && (data_counter<data_len)) {
#ifdef DEBUG
		fprintf(stdout, "\n[recv_packet]char_read = %c\t", ch);
#endif
		if(start_data) {
			printf("data\n");
			*iter = ch;
			iter++;
			length++;
			data_counter++;
		}
			
		if(ch=='\n') {
			line[counter++] = '\n';
			line[counter] = '\0';
			memcpy(iter, line, counter);
			iter+=counter;
			length+=counter;
			printf("line: %s\n", line);
			if(strstr(line, HTTP_CONTENT_LEN)!=NULL) {
				
				char *token = strtok(line, "\r\n");
				content_len = strstr(token, HTTP_CONTENT_LEN);
				data_len = atoi(content_len+strlen(HTTP_CONTENT_LEN));
				printf("data_len: %d\n", data_len);
				
			}
			else if(line[0]=='\r') {
				printf("end2\n");
				start_data = 1;
			}
			memset(line, 0, counter);
			counter = 0;
		}
		else {
			line[counter] = ch;
			counter++;
		}

    }
	buffer[length]='\0';
	
    if (char_read < 0) {
		fprintf(stderr, "\n[recv_packet]read error");
		return NULL;
    }
#ifdef DEBUG
    fprintf(stdout, "\n[recv_packet]buffer read = %s\n", buffer);
#endif
    return buffer;
}
