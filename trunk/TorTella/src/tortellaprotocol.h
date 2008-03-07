#ifndef TORTELLA_PROTOCOL_H
#define TORTELLA_PROTOCOL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char u_int1;
typedef unsigned short u_int2;
typedef unsigned int u_int4;
typedef unsigned long long u_int8;

//Descriptor ID
#define PING_ID		0x00
#define PONG_ID		0x01
#define LIST_ID		0x02
#define LISTHITS_ID	0x03
#define JOIN_ID		0x04
#define LEAVE_ID	0x05
#define PUSH_ID		0x06
#define BYE_ID		0x07

//Status ID
#define ONLINE_ID	0x80
#define BUSY_ID		0x81

//TorTella descriptor Header
struct tortella_header {
	u_int8 id;
	u_int4 desc_id; //PING_ID, PONG_ID, etc..
	u_int1 ttl;
	u_int1 hops;
	u_int4 desc_len;
	u_int4 data_len;
};
typedef struct tortella_header tortella_header;

struct ping_desc {
	u_int2 port;
	u_int4 ip;
	u_int1 status;
	u_int4 kbytes;
};
typedef struct ping_desc ping_desc;

struct pong_desc {
	u_int2 port;
	u_int4 ip;
	u_int1 status;
	u_int8 servent_id; //ID del servente che invia il pacchetto
};
typedef struct pong_desc pong_desc;

struct list_desc {
	u_int2 port;
	u_int4 ip;
	u_int8 servent_id; //ID del servente che riceve il pacchetto
};
typedef struct list_desc list_desc;

struct listhits_desc {
	u_int2 port;
	u_int4 ip;
	u_int8 servent_id;
	//Riempire campo data
};
typedef struct listhits_desc listhits_desc;

struct tortella_packet {
	tortella_header *header;
	char *desc;	//desc_len
	char *data;	//data_len
};
typedef struct tortella_packet tortella_packet;

char *tortella_bin_to_char(tortella_packet *packet, u_int4 *len);

tortella_packet *tortella_char_to_bin(char *packet);

void print_packet(tortella_packet *packet);

#endif //TORTELLA_PROTOCOL_H
