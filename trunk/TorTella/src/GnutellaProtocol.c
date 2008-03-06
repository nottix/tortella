#include "GnutellaProtocol.h"

char *GnutellaBinToChar(GnutellaPacket *packet, u_int4 *len) {
	printf("\nstart\n");
	char *ret, *head, *desc, *iter;
	head = (char*)packet->header;
	desc = (char*)packet->desc;
	u_int4 payloadLen = packet->header->payloadLen;
	printf("payload: %d head: %d\n", payloadLen, sizeof(GnutellaHeader));

	ret = malloc(sizeof(GnutellaHeader)+payloadLen+1); //Il +1 serve per il \0
	memcpy(ret, head, sizeof(GnutellaHeader));
	iter = ret;
	iter += sizeof(GnutellaHeader);

	strcpy(iter, desc);
	iter += packet->header->payloadLen - packet->header->dataLen;
	printf("sizeof(GnutellaHeader): %d, payloadLen: %d, dataLen: %d\n", sizeof(GnutellaHeader), payloadLen, packet->header->dataLen);
	strcpy(iter, packet->data);
	printf("data: %s\n", iter);
	//iter += packet->header->dataLen;
	//*len = iter-ret;
	*len = sizeof(GnutellaHeader)+payloadLen;
	printf("len: %d\n", *len);

	printf("end\n\n");

	return ret;
}

GnutellaPacket *GnutellaCharToBin(char *packet) {
	printf("\nStarting char to bin...\n");

	GnutellaPacket *ret = (GnutellaPacket*)malloc(sizeof(GnutellaPacket));
	GnutellaHeader *header = (GnutellaHeader*)packet;
	char *desc = (packet+sizeof(GnutellaHeader));
	char *data = NULL;

	u_int4 payloadDesc = header->payloadDesc;
	if(payloadDesc==QUERY_ID) {
		data = (packet+sizeof(GnutellaHeader)+sizeof(QueryDesc));
	}
	else if(payloadDesc==QUERYHITS_ID) {
		data = (packet+sizeof(GnutellaHeader)+sizeof(QueryHitsDesc));
	}
	printf("data: \"%s\"\n", data);

	ret->header = header;
	ret->desc = desc;
	ret->data = data;
	printf("Ending char to bin...\n\n");
	return ret;	
}
