#ifndef TORTELLA_PROTOCOL_H
#define TORTELLA_PROTOCOL_H

//#define TORTELLA_DEBUG

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"

#define GET_PING(packet)		((ping_desc*)packet->desc)
#define GET_PONG(packet)		((pong_desc*)packet->desc)
#define GET_JOIN(packet)		((join_desc*)packet->desc)
#define GET_LEAVE(packet)		((leave_desc*)packet->desc)
#define GET_MESSAGE(packet)		((message_desc*)packet->desc)
#define GET_CREATE(packet)		((create_desc*)packet->desc)

//Descriptor ID
#define PING_ID			0x00
#define PONG_ID			0x01
#define LIST_ID			0x02
#define LISTHITS_ID		0x03
#define JOIN_ID			0x04
#define LEAVE_ID		0x05
#define MESSAGE_ID		0x06
#define CREATE_ID		0x07
#define SEARCH_ID		0x08
#define SEARCHHITS_ID	0x10
#define SEARCH_ID	0x08	//Ricerca una chat

//Status ID
#define ONLINE_ID	0x80
#define BUSY_ID		0x81

/*
                          TorTella Header
 +----+---------+-----------+---------+-----------+----------+----------+
 | ID | desc_ID | sender_ID | recv_ID | timestamp | desc_len | data_len |
 +----+---------+-----------+---------+-----------+----------+----------+

 */
struct tortella_header {
	u_int8 id;		//ID univoco del pacchetto
	u_int4 desc_id; //PING_ID, PONG_ID, etc..
	u_int8 sender_id;   //ID del servente che invia
	u_int8 recv_id; //ID del servente che riceve
	//u_int1 ttl;
	//u_int1 hops;
	time_t timestamp;
	u_int4 desc_len; //lunghezza del descrittore
	u_int4 data_len; //lunghezza del campo dati
};
typedef struct tortella_header tortella_header;

struct ping_desc {
	u_int1 status;
};
typedef struct ping_desc ping_desc;

struct pong_desc {
	u_int1 status;
};
typedef struct pong_desc pong_desc;

struct list_desc {
	u_int8 chat_id;	//ID della chat di cui si vuole la lista degli utenti
};
typedef struct list_desc list_desc;

struct listhits_desc {
	u_int4 user_num;	//Numero di utenti nel campo data
	//Riempire campo data
};
typedef struct listhits_desc listhits_desc;

struct search_desc {
	u_int1 ttl;
	u_int1 hops;
	//Campo dati: stringa ricerca
};
typedef struct search_desc search_desc;

struct searchhits_desc {
	u_int1 ttl;
	u_int1 hops;
	u_int4 num_res;
	//Campo dati: risultati separati da ';'
};
typedef struct searchhits_desc searchhits_desc;

struct join_desc {
	u_int1 status;
	u_int8 chat_id; //ID della chat room (hash del nome della chat)
	//Campo dati: nickname
};
typedef struct join_desc join_desc;

struct leave_desc {
	u_int8 chat_id; //ID della chat room (hash del nome della chat)
};
typedef struct leave_desc leave_desc;

/*
   Data
 +-----+
 | msg |
 +-----+

 */
struct message_desc {
	u_int8 chat_id;		//ID della chat a cui va il messaggio
	//Campo dati: il msg
};
typedef struct message_desc message_desc;

struct create_desc {
	u_int8 chat_id;		//ID della chat da creare
	//Campo dati:Title	//Titolo della chat da creare
};
typedef struct create_desc create_desc;

/*
   Data
 +-------+
 | Query |
 +-------+

 */
struct search_desc {
	u_int1 ttl;
	u_int1 hops;
	//Campo dati: stringa ricerca
};
typedef struct search_desc search_desc;

struct searchhits_desc {
	u_int1 ttl;
	u_int1 hops;
	u_int4 num_res;
	//Campo dati: risultati separati da ';'
};
typedef struct searchhits_desc searchhits_desc;

/*
        Tortella Packet
 +--------+------------+------+
 | Header | Descriptor | Data |
 +--------+------------+------+

 */
struct tortella_packet {
	tortella_header *header;
	char *desc;	//desc_len
	char *data;	//data_len
};
typedef struct tortella_packet tortella_packet;

char *tortella_bin_to_char(tortella_packet *packet, u_int4 *len);

tortella_packet *tortella_char_to_bin(char *packet);

void print_packet(tortella_packet *packet);

char *dump_data(const char *data, u_int4 len);

tortella_packet *tortella_create_packet(tortella_header *header, char *desc, char *data);

tortella_header *tortella_create_header(u_int8 id, u_int4 desc_id, u_int8 sender_id, u_int8 recv_id, u_int4 desc_len, u_int4 data_len);

tortella_packet *tortella_create_packet_header(u_int8 id, u_int4 desc_id, u_int8 sender_id, u_int8 recv_id, u_int4 desc_len, u_int4 data_len, char *desc, char *data);

tortella_header* tortella_get_header(const char *buffer);

char *tortella_get_desc(const char *buffer);

char *tortella_get_data(const char *buffer);

#endif //TORTELLA_PROTOCOL_H
