#ifndef GNUTELLA_PROTOCOL_H
#define GNUTELLA_PROTOCOL_H

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
#define QUERY_ID	0x80
#define QUERYHITS_ID	0x81
#define PUSH_ID		0x40
#define BYE_ID		0x02

//Gnutella descriptor Header
struct GnutellaHeader {
	u_int8 id;
	u_int4 payloadDesc; //PING_ID, PONG_ID, etc..
	u_int1 ttl;
	u_int1 hops;
	u_int4 dataLen;
	u_int4 payloadLen;
};
typedef struct GnutellaHeader GnutellaHeader;

struct PongDesc {
	u_int2 port;
	u_int4 ip;
	u_int4 files;
	u_int4 kbytes;
};
typedef struct PongDesc PongDesc;

struct QueryDesc {
	u_int4 minSpeed;
	//char *criteria;
};
typedef struct QueryDesc QueryDesc;

struct QueryHitsDesc {
	u_int1 nHits;
	u_int2 port;
	u_int4 ip;
	u_int4 speed;
	u_int8 serventId;
	//char *resultSet;
};
typedef struct QueryHitsDesc QueryHitsDesc;

struct PushDesc {
	u_int8 serventId;
	u_int4 fileIndex;
	u_int4 ip;
	u_int2 port;
};
typedef struct PushDesc PushDesc;

struct GnutellaPacket {
	GnutellaHeader *header;
	char *desc;	//payloadLen
	char *data;	//payloadLen
};
typedef struct GnutellaPacket GnutellaPacket;

char *GnutellaBinToChar(GnutellaPacket *packet, u_int4 *len);

GnutellaPacket *GnutellaCharToBin(char *packet);

#endif //GNUTELLA_PROTOCOL_H
