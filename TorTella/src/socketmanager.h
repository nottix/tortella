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
#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <sys/types.h>
#include <sys/socket.h>
#include "common.h"
#include "httpmanager.h"

/**
 * Crea un socket di connessione remota ovvero
 * si connette ad un server remoto.
 */
int create_tcp_socket(const char* dst_ip, int dst_port);

/**
 * Crea un socket d'ascolto, ovvero un server TCP in attesa di connessioni.
 */
int create_listen_tcp_socket(const char* src_ip, int src_port);

/**
 * Elimina un socket ovvero chiude una connessione, mettendo il socket nello stato di TIME_WAIT. 
 */
int delete_socket(int sock_descriptor);

/*In ascolto per una nuova connessione.
    listensocket e' il descrittore socket di ascolto (restituito da create_listen_tcp_socket)
    Il parametro mode ha i seguenti significati:
        - LP_WRITE, aperta la connessione invia il buffer al client
        - LP_READ, aperta la connessione attende dati, inserendoli poi nel buffer
        - LP_NONE, crea solo la nuova connessione con il client.
*/
//int listen_packet(int listen_socket, char* buffer, unsigned int mode);
//

/**
 * funzione che ritorna il nuovo descrittore di socket e l'indirizzo del client connesso,
 * nel caso la chiamata di sistema accept() abbia avuto un esito positivo
 */
int listen_http_packet(int listen_socket);

/**
 * Legge il pacchetto in ingresso e ritorna il numero di caratteri
 */
int recv_http_packet(int con_socket, char **buffer);

/**
 * Permette di scrivere sul socket connesso specificato dal relativo descrittore
 */
int send_packet(int sock_descriptor, char* buffer, int len);

/**
 * Attende la ricezione di un pacchetto, avente come dimensione massima quella pari al
 * valore assunto dal parametro BUFFER_LEN
 */
int recv_packet(int sock_descriptor,char** buffer);

/** 
 * Attende la ricezione di un pacchetto, prefissando il valore massimo(max_len) di byte di
 *  un blocco di dati del pacchetto
 */
int recv_sized_packet(int sock_descriptor,char** buf, int max_len);

//char *recv_http_packet(int sock_descriptor,char* buffer, int *len);

/**
 * Permette di ottenere l'indirizzo ip e il numero di porta del peer assocciato
 * al socket, passato come parametro
 */
char *get_dest_ip(int socket);

/**
 * Permette di ottenere il numero di porta del peer assocciato
 * al socket, passato come parametro
 */
u_int4 get_dest_port(int socket);

/**
 * Permette la chiusura asimmetrica di una connessione TCP
 */
int shutdown_socket(int sock_descriptor);

#endif //SOCKETMANAGER_H
