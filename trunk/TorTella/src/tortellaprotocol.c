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

#include "tortellaprotocol.h"

/**
 * Ha il compito di eseguire il parser del pacchetto tortella.
 * In paritcolare prende in input il pacchetto, memorizzato nella sua
 * struttura dati, e restituisce tutto il suo contenuto in un buffer di caratteri.
 * Inoltre il parametro len ritorna la lunghezza di tale buffer.
 */
char *tortella_bin_to_char(tortella_packet *packet, u_int4 *len) {
	
	char *header = (char *) packet->header;
	char *desc = (char *) packet->desc;
	u_int4 header_len = sizeof(tortella_header);
	u_int4 desc_len = packet->header->desc_len;
	u_int4 data_len = packet->header->data_len;

	*len = header_len + desc_len + data_len;
	char *ret = calloc(header_len + desc_len + data_len, 1);
	memcpy(ret, header, header_len);
	char *iter = ret;
	iter += header_len;
	
	memcpy(iter, desc, desc_len);
	iter += desc_len;		//!Si posiziona all'inizio del campo data
	memcpy(iter, packet->data, data_len);

	iter += data_len;
	
	return ret;
}

/**
 * Svolge le funzionalita' di parser inverso rispetto alla funzione tortella_bin_to_char. 
 * La procedura riceve come parametro il buffer, contenente i dati, i quali vengono memorizzati nella 
 * struttura dati tortella_packet.
 */
tortella_packet *tortella_char_to_bin(char *packet) {

	tortella_packet *ret = (tortella_packet *) calloc(1, sizeof(tortella_packet));
	tortella_header *header = (tortella_header *) packet;
	char *desc = (packet + sizeof(tortella_header));
	char *data = NULL;
	
	u_int4 desc_id = header->desc_id;

	data = (packet + sizeof(tortella_header) + header->data_len);
	
	ret->header = header;
	ret->desc = desc;
	ret->data = data;
		
	return ret;
}

/**
 * funzione adibita alla stampa ordinata di tutte le informazioni contenute nella struttura dati
 * tortella_packet, ricevuta come parametro d'input.
 */
void print_packet(tortella_packet * packet) {
	if (packet != NULL) {
		printf("--tortella_header--\n");
		printf("id:        %lld\n", packet->header->id);
		printf("desc_id:   %d\n", packet->header->desc_id);
		printf("sender_id: %lld\n", packet->header->sender_id);
		printf("recv_id:   %lld\n", packet->header->recv_id);
		printf("timestamp: %ld\n", packet->header->timestamp);
		printf("desc_len:  %d\n", packet->header->desc_len);
		printf("data_len:  %d\n", packet->header->data_len);
		
		if(packet->header->desc_id==LIST_ID) {
			printf("--list_desc--\n");
		}
		else if(packet->header->desc_id==LISTHITS_ID) {
			printf("--listhits_desc--\n");
		}
		else if(packet->header->desc_id==JOIN_ID) {
			join_desc *join = (join_desc*)packet->desc;
			printf("--join_desc--\n");
			printf("status:  %d\n", join->status);
			printf("chat_id: %lld\n", join->chat_id);
		}
		
		printf("data: %s\n", dump_data(packet->data, packet->header->data_len));
	}
}

/**
 * funzione che si occupa della creazione del pacchetto tortella.   
 * La procedura si occupa dell'allocazione dello spazio di memoria per la struttura dati tortella_packet
 * e del relativo setting dei campi con i valori contenuti nei parametri d'input.
 */
tortella_packet *tortella_create_packet(tortella_header *header, char *desc, char *data) {
	
	tortella_packet *packet = (tortella_packet*)calloc(1, sizeof(tortella_packet));
	packet->header = header;
	packet->desc = desc;
	packet->data = data;
	
	return packet;
}

/**
 * funzione che si occupa della creazione della struttura dati tortella_header e del relativo setting
 * dei vari campi dati contenutti nella struttura stessa.
 */
tortella_header *tortella_create_header(u_int8 id, u_int4 desc_id, u_int8 sender_id, u_int8 recv_id, u_int4 desc_len, u_int4 data_len) {
	
	tortella_header *header = (tortella_header*)calloc(1, sizeof(tortella_header));
	header->id = id;
	header->desc_id = desc_id;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->desc_len = desc_len;
	header->data_len = data_len;
	
	return header;
}

/**
 * funzione che sioccupa della creazione contemporanea sia della struttura dati relativa all'header del pacchetto,
 * sia della struttura dati relativo al l'intero pacchetto tortella. In realta' tale funzione contiene la chiamata
 * alla funzione tortella_create_header e tortella_create_packet. 
 */
tortella_packet *tortella_create_packet_header(u_int8 id, u_int4 desc_id, u_int8 sender_id, u_int8 recv_id, u_int4 desc_len, u_int4 data_len, char *desc, char *data) {
	
	tortella_header *header = tortella_create_header(id, desc_id, sender_id, recv_id, desc_len, data_len);
	tortella_packet *packet = tortella_create_packet(header, desc, data);
	
	return packet;	
}

/**
 * funzione che ritorna il puntatore all'header del pacchetto, facendo un casting del buffer,
 * il quale contiene come dati iniziali quelli relativi all'header.
 */
tortella_header* tortella_get_header(const char *buffer) {
	if(buffer==NULL)
		return NULL;
	tortella_header *header = (tortella_header*)buffer;
	
	return header;
}

char *tortella_get_desc(const char *buffer) {
	if(buffer==NULL)
		return NULL;
	tortella_header *header = tortella_get_header(buffer);
	char *desc = (char*)calloc(header->desc_len, 1);
	memcpy(desc, buffer+sizeof(tortella_header), header->desc_len);
	
	return desc;	
}

char *tortella_get_data(const char *buffer) {
	if(buffer==NULL)
		return NULL;
	tortella_header *header = tortella_get_header(buffer);
	char *data = (char*)calloc(header->data_len+1, 1);
	memcpy(data, buffer+sizeof(tortella_header)+header->desc_len, header->data_len);

	return data;
}
