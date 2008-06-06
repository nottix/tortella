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

//Status ID
#define ONLINE_ID	0x80
#define BUSY_ID		0x81
#define AWAY_ID		0x82
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
	time_t timestamp;
	u_int4 desc_len; //lunghezza del descrittore
	u_int4 data_len; //lunghezza del campo dati
};
typedef struct tortella_header tortella_header;

struct ping_desc {
	u_int4 port;
	u_int1 status;
	//Cambo dati: nickname
};
typedef struct ping_desc ping_desc;

struct list_desc {
	u_int8 chat_id;	//ID della chat di cui si vuole la lista degli utenti
};
typedef struct list_desc list_desc;

struct listhits_desc {
	u_int4 user_num;	//Numero di utenti nel campo data
	//Campo dati: elenco utenti della chat
};
typedef struct listhits_desc listhits_desc;

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
	u_int4 num_res;
	//Campo dati: risultati ricerca
};
typedef struct searchhits_desc searchhits_desc;

struct bye_desc {
};
typedef struct bye_desc bye_desc;

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

//char *dump_data(const char *data, u_int4 len);

tortella_packet *tortella_create_packet(tortella_header *header, char *desc, char *data);

tortella_header *tortella_create_header(u_int8 id, u_int4 desc_id, u_int8 sender_id, u_int8 recv_id, u_int4 desc_len, u_int4 data_len);

tortella_packet *tortella_create_packet_header(u_int8 id, u_int4 desc_id, u_int8 sender_id, u_int8 recv_id, u_int4 desc_len, u_int4 data_len, char *desc, char *data);

tortella_header* tortella_get_header(const char *buffer);

char *tortella_get_desc(const char *buffer);

char *tortella_get_data(const char *buffer);

#endif //TORTELLA_PROTOCOL_H
