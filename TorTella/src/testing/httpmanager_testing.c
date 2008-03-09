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
 
#include "httpmanager.h"

#include "logger.h"

int main(void) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = 10;
	header->desc_id = LISTHITS_ID;
	
	listhits_desc *list1 = (listhits_desc*)malloc(sizeof(listhits_desc));
	packet->desc = (char*)list1;
	
	header->data_len = 3;
	header->desc_len = sizeof(list_desc);
		
	packet->header = header;
	packet->data = "kk";
	
	char *buffer;
	if((buffer=http_bin_to_char(http_create_packet(packet, HTTP_RES_GET, HTTP_STATUS_OK, "file", 0, 4, "data", 4)))==NULL)
		printf("Errore\n");
	
	http_char_to_bin(buffer);
	
	logger_init("test");
	logger_add("test", "tes");
	
	/*if(http_bin_to_char(http_create_packet(packet, HTTP_REQ_POST, 0, "file", 0, 4, "data", 4))==NULL)
		printf("Errore\n");
	
	if(http_bin_to_char(http_create_packet(packet, HTTP_RES_GET, HTTP_STATUS_OK, "file", 0, 4, "data", 4))==NULL)
		printf("Errore\n");
	
	if(http_bin_to_char(http_create_packet(packet, HTTP_RES_POST, HTTP_STATUS_OK, "file", 0, 4, "data", 4))==NULL)
		printf("Errore\n");*/
	
	return 0;
}
