#ifndef TORTELLA_PROTOCOL_H
#define TORTELLA_PROTOCOL_H

//#define TORTELLA_DEBUG

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

//Descriptor ID
#define PING_ID		0x00
#define PONG_ID		0x01
#define LIST_ID		0x02
#define LISTHITS_ID	0x03
#define JOIN_ID		0x04
#define LEAVE_ID	0x05
#define MESSAGE_ID	0x06

//Status ID
#define ONLINE_ID	0x80
#define BUSY_ID		0x81

/*
                          TorTella Header
 +----+---------+-----------+-------------+----------+----------+
 | ID | desc_ID | sender_ID | recv_ID | desc_len | data_len |
 +----+---------+-----------+-------------+----------+----------+

 */
struct tortella_header {
	u_int8 id;		//ID univoco del pacchetto
	u_int4 desc_id; //PING_ID, PONG_ID, etc..
	u_int8 sender_id;   //ID del servente che invia
	u_int8 recv_id; //ID del servente che riceve
	//u_int1 ttl;
	//u_int1 hops;
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
};
typedef struct list_desc list_desc;

struct listhits_desc {
	//Riempire campo data
};
typedef struct listhits_desc listhits_desc;

struct join_desc {
	u_int1 status;
	u_int8 chat_id; //ID della chat room (hash del nome della chat)
};
typedef struct join_desc join_desc;

struct leave_desc {
	u_int8 chat_id; //ID della chat room (hash del nome della chat)
};
typedef struct leave_desc leave_desc;

/*
        Data
 +---------+-----+
 | ID_dest | msg |
 +---------+-----+

 */
struct message_desc {
	u_int4 id_dest_len; //Lunghezza campo dati contenente gli ID (da 8 byte) dei destinatari
	u_int4 msg_len;     //Lunghezza campo dati contenete il messaggio
	//Campo dati: id destinatari del msg e poi il msg
};
typedef struct message_desc message_desc;

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

char *dump_data(char *data, u_int4 len);

tortella_packet *tortella_create_packet(tortella_header *header, char *desc, char *data);

tortella_header *tortella_create_header(u_int8 id, u_int4 desc_id, u_int8 sender_id, u_int8 recv_id, u_int4 desc_len, u_int4 data_len);

tortella_packet *tortella_create_packet_header(u_int8 id, u_int4 desc_id, u_int8 sender_id, u_int8 recv_id, u_int4 desc_len, u_int4 data_len, char *desc, char *data);

tortella_header* tortella_get_header(const char *buffer);

char *tortella_get_desc(const char *buffer);

char *tortella_get_data(const char *buffer);

#endif //TORTELLA_PROTOCOL_H
