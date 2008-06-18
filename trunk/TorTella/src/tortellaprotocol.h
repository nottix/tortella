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

#ifndef TORTELLA_PROTOCOL_H
#define TORTELLA_PROTOCOL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
#include "utils.h"

#define GET_PING(packet)		((ping_desc*)packet->desc)
#define GET_JOIN(packet)		((join_desc*)packet->desc)
#define GET_LEAVE(packet)		((leave_desc*)packet->desc)
#define GET_MESSAGE(packet)		((message_desc*)packet->desc)
#define GET_SEARCH(packet)		((search_desc*)packet->desc)
#define GET_SEARCHHITS(packet)  ((searchhits_desc*)packet->desc)
#define GET_LIST(packet)		((list_desc*)packet->desc)
#define GET_LISTHITS(packet)	((listhits_desc*)packet->desc)
#define GET_BYE(packet)			((bye_desc*)packet->desc)

//Descriptor ID
#define PING_ID			0x01
#define LIST_ID			0x03
#define LISTHITS_ID		0x04
#define JOIN_ID			0x05
#define LEAVE_ID		0x06
#define MESSAGE_ID		0x07
#define SEARCH_ID		0x09
#define SEARCHHITS_ID	0x10
#define BYE_ID			0x11
#define CLOSE_ID 		0x12

//Status ID
#define ONLINE_ID	0x80
#define BUSY_ID		0x81
#define AWAY_ID		0x82

/*
 *                           TorTella Header
 * +----+---------+-----------+---------+-----------+----------+----------+
 * | ID | desc_ID | sender_ID | recv_ID | timestamp | desc_len | data_len |
 * +----+---------+-----------+---------+-----------+----------+----------+
 */
/** struttura dati dove vengono memorizzati rispettivamente l'id univoco del
 * pacchetto, il descrittore del pacchetto,l'id del servente che invia il pacchetto,
 * l'id del ricevente, il timestamp e la relativa lunghezza sia del descrittore che 
 * del campo dati.
 */
struct tortella_header {
	u_int8 id;		
	u_int4 desc_id;
	u_int8 sender_id;   
	u_int8 recv_id; 
	time_t timestamp;
	u_int4 desc_len; 
	u_int4 data_len; 
};
typedef struct tortella_header tortella_header;

/**
 * struttura dati per il pacchetto di tipo PING, in cui viene memorizzato
 * lo status dell'utente al momento dell'invio del pacchetto
 */
struct ping_desc {
	u_int4 port;
	u_int1 status;
	//Cambo dati: nickname

};
typedef struct ping_desc ping_desc;

/**
 * struttura dati per il pacchetto LIST, in cui viene memorizzato il valore
 * del TTL e dell' HOPS, e l'ID della chat di cui si richiede la lista degli
 * utenti presenti
 */
struct list_desc {
	u_int8 chat_id;	
	u_int1 ttl;
	u_int1 hops;
};
typedef struct list_desc list_desc;

/**
 * struttura dati per il pacchetto di risposta al LIST, in cui sono salvati
 *  gli stessi dati contenuti nella struttura list_desc con aggiunta del numero 
 *   di utenti connessi alla chat, di cui era stata richiesta la lista degli users  
 */

struct listhits_desc {
	u_int4 user_num;	
	u_int1 ttl;
	u_int1 hops;
	u_int8 chat_id;
	//Campo dati: elenco utenti della chat
};
typedef struct listhits_desc listhits_desc;

/**
 * struttura dati per il pacchetto di join ad una chat.
 * Nella struct sono salvate tutte le info relative all'utente 
 * che vuole accedere alla chat: stato(ONLINE-BUSY-AWAY)-ID dell'utente-
 * -Id della chat-numero di porta del socket-indirizzo ip. Inoltre sono 
 * salvati i valori dell TTL e dell'HOPS per il flooding del pacchetto.
 * Tutti i dati contenuti da tale descrittore rimarranno identici(salvo per il TTL e HOPS) 
 * per tutto il percorso fatto dal pacchetto nella rete.
 */

struct join_desc {
	u_int1 status;
	u_int8 user_id;
	u_int8 chat_id; 
	u_int4 port;
	char ip[16];
	u_int1 ttl;
	u_int1 hops;
	//Campo dati: nickname

};
typedef struct join_desc join_desc;

/**
 * struttura dati per l'invio del pacchetto LEAVE ad una chat.
 * I dati salvati sono rispettivamente l'ID dell'utente che vuole uscire
 * dalla chat e il rispettivo identificativo della chat. Come per il pacchetto
 * JOIN anche in questo caso i dati memorizzati rimarranno identici(a meno del TTL e HOPS)
 * per tutto il percorso fatto nella rete.
 */
 
struct leave_desc {
	u_int8 user_id;
	u_int8 chat_id; 
	u_int1 ttl;
	u_int1 hops;
};
typedef struct leave_desc leave_desc;

/**
 * Data
 * +-----+
 * | msg |
 * +-----+
 */
/** struttura dati per l'invio del messaggio ad una specifica chat,
 * tenendo memoria per l'appunto in tlae struttura l'ID della chat a cui
 * è diretto il messaggio di testo.
 */
struct message_desc {
	u_int8 chat_id;		
	//Campo dati: il msg

};
typedef struct message_desc message_desc;

/*
 * Data
 * +-------+
 * | Query |
 * +-------+
 */
/** struct per il flooding del pacchetto SEARCH. Ogni utente
 * invierà una richiesta di ricerca, e in tale
 * struttura verrà decrementato il valore del TTL e incrementato quello 
 * dell'HOPS per ogni nodo attraversato dal pacchetto durante il suo percorso
 * nella rete.
 */
struct search_desc {
	u_int1 ttl;
	u_int1 hops;
	//Campo dati: stringa ricerca

};
typedef struct search_desc search_desc;

/**
 * struct per l'invio del pacchetto SEARCHHITS.
 * Il routing di tale pacchetto segue il percorso inverso 
 * di quello eseguito dal pacchetto SEARCH.
 *
 */
struct searchhits_desc {
	u_int4 num_res;
	//Campo dati: risultati ricerca
};
typedef struct searchhits_desc searchhits_desc;

/**
 * struttura usata per segnalare la disconnessione dalla rete tortella.
 */
struct bye_desc {
};
typedef struct bye_desc bye_desc;

/**
 *        Tortella Packet
 * +--------+------------+------+
 * | Header | Descriptor | Data |
 * +--------+------------+------+
 */
/** struttua del pacchetto Tortella. Nella struttura
 * viene memorizzato il puntatore relativo al suo header,
 * il descrittore di una delle possibili strutture dati(join_desc,search_desc,..)
 */
struct tortella_packet {
	tortella_header *header;
	char *desc;	//desc_len contenuta nell'header del pacchetto
	char *data;	//data_len contenuta nell'header del pacchetto
};
typedef struct tortella_packet tortella_packet;

/**
 * funzione la quale ha il compito di eseguire il parser del pacchetto tortella.
 * In paritcolare prende in input da parametro il pacchetto, memorizzato nella sua
 * struttura dati, e restituisce tutto il suo contenuto in un buffer di caratter.
 * Inoltre il parametro len ritorna la lunghezza di tale buffer.
 */
char *tortella_bin_to_char(tortella_packet *packet, u_int4 *len);

/**
 * funzione che svolge le funzionalità di parser inverso rispetto alla funzione tortella_bin_to_char. 
 * La procedura riceve come parametro il buffer, contenente i dati, i quali vengono memorizzati nella 
 * struttura dati tortella_packet.
 */
tortella_packet *tortella_char_to_bin(char *packet);

/**
 * funzione adibita alla stampa ordinata di tutte le informazioni contenute nella struttura dati
 * tortella_packet, ricevuta come parametro d'input.
 */
void print_packet(tortella_packet *packet);

/**
 * funzione che si occupa della creazione del pacchetto tortella.   
 * La procedura si occupa dell'allocazione dello spazio di memoria per la struttura dati tortella_packet
 * e del relativo setting dei campi con i valori contenuti nei parametri d'input.
 */
tortella_packet *tortella_create_packet(tortella_header *header, char *desc, char *data);

/**
 * funzione che si occupa della creazione della struttura dati tortella_header e del relativo setting
 * dei vari campi dati contenutti nella struttura stessa.
 */
tortella_header *tortella_create_header(u_int8 id, u_int4 desc_id, u_int8 sender_id, u_int8 recv_id, u_int4 desc_len, u_int4 data_len);

/**
 * funzione che sioccupa della creazione contemporanea sia della struttura dati relativa all'header del pacchetto,
 * sia della struttura dati relativo al l'intero pacchetto tortella. In realtà tale funzione contiene la chiamata
 * alla funzione tortella_create_header e tortella_create_packet. 
 */
tortella_packet *tortella_create_packet_header(u_int8 id, u_int4 desc_id, u_int8 sender_id, u_int8 recv_id, u_int4 desc_len, u_int4 data_len, char *desc, char *data);

/**
 * funzione che ritorna il puntatore all'header del pacchetto, facendo un casting del buffer,
 * il quale contiene come dati iniziali quelli relativi all'header.
 */
tortella_header* tortella_get_header(const char *buffer);

char *tortella_get_desc(const char *buffer);

char *tortella_get_data(const char *buffer);

#endif //TORTELLA_PROTOCOL_H
