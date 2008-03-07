#include "tortellaprotocol.h"

char *tortella_bin_to_char(tortella_packet *packet, u_int4 *len) {
	printf("\nstart\n");
	
	char *header = (char*)packet->header;
	char *desc = (char*)packet->desc;
	u_int4 header_len = sizeof(tortella_header);
	u_int4 desc_len = packet->header->desc_len;
	u_int4 data_len = packet->header->data_len;
	printf("desc_len: %d header_len: %d data_len: %d\n", desc_len, header_len, data_len);

	char *ret = malloc(header_len+desc_len+1); //Il +1 serve per il \0
	memcpy(ret, header, header_len);
	printf("ret: %s\n", ret);
	char *iter = ret;
	iter += header_len;

	memcpy(iter, desc, desc_len);
	iter += desc_len; //Si posiziona all'inizio del campo data
	memcpy(iter, packet->data, data_len);
	printf("data: %s\n", iter);
	iter += data_len;

	*len = header_len+desc_len+data_len;
	printf("len: %d\n", *len);

	/*int i;
	for(i=0; i<(*len); i++) {
		printf("elem[%d]: %c\n", i, ret[i]);
	}*/
	
	printf("end\n\n");

	return ret;
}

tortella_packet *tortella_char_to_bin(char *packet) {
	printf("\nStarting char to bin...\n");

	tortella_packet *ret = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header *header = (tortella_header*)packet;
	char *desc = (packet+sizeof(tortella_header));
	char *data = NULL;

	u_int4 desc_id = header->desc_id;
	if(desc_id==LISTHITS_ID) {
		data = (packet+sizeof(tortella_header)+sizeof(listhits_desc));
	}
	/*if(payloadDesc==QUERY_ID) {
		data = (packet+sizeof(GnutellaHeader)+sizeof(QueryDesc));
	}
	else if(payloadDesc==QUERYHITS_ID) {
		data = (packet+sizeof(GnutellaHeader)+sizeof(QueryHitsDesc));
	}
	printf("data: \"%s\"\n", data);*/

	ret->header = header;
	ret->desc = desc;
	ret->data = data;
	
	printf("Ending char to bin...\n\n");
	return ret;	
}

void print_packet(tortella_packet *packet) {
	if(packet!=NULL) {
		printf("--tortella_header--\n");
		printf("id: %lld\n", packet->header->id);
		printf("desc_id: %d\n", packet->header->desc_id);
		printf("ttl: %d\n", packet->header->ttl);
		printf("hops: %d\n", packet->header->hops);
		printf("desc_len: %d\n", packet->header->desc_len);
		printf("data_len: %d\n", packet->header->data_len);
		
		if(packet->header->desc_id == LIST_ID) {
			list_desc *list = (list_desc*)packet->desc;
			/*int i;
			for(i=0; i<sizeof(list_desc); i++) {
				printf("elem[%d]: %c\n", i, packet->desc[i]);
			}*/
			printf("--list_desc--\n");
			printf("port: %d\n", list->port);
			printf("ip: %d\n", list->ip);
			printf("servent_id: %lld\n", list->servent_id);
		}
		else if(packet->header->desc_id == LISTHITS_ID) {
			listhits_desc *list = (listhits_desc*)packet->desc;
			printf("--listhits_desc--\n");
			printf("port: %d\n", list->port);
			printf("ip: %d\n", list->ip);
			printf("servent_id: %lld\n", list->servent_id);
		}
		
		printf("data: %s\n", packet->data);
	}
}
