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
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	
	printf("%d\n", sizeof(u_int8));
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = 0;
	header->desc_id = LISTHITS_ID;
	header->sender_id = 2;
	header->recv_id = 3;
	
	listhits_desc *list1 = (listhits_desc*)malloc(sizeof(listhits_desc));
/*	list1->port = 377;
	list1->ip = 21;
	list1->servent_id = 2;*/
	packet->desc = (char*)list1;
	
	
	header->data_len = 3;
	header->desc_len = sizeof(list_desc);
		
	packet->header = header;
	packet->data = "kk";
	
	u_int4 len;
	//printf("Char: %s\n", );
	
	print_packet(tortella_char_to_bin(tortella_bin_to_char(packet, &len)));
	
	return 0;
}
 
 
