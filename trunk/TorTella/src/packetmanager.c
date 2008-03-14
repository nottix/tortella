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
 
#include "packetmanager.h"

u_int4 send_join_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int1 status, u_int8 chat_id) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = JOIN_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->data_len = 0;
	
	join_desc *join = (join_desc*)malloc(sizeof(join_desc));
	join->status = status;
	join->chat_id = chat_id;
	packet->desc = (char*)join;

	header->desc_len = sizeof(join_desc);
		
	packet->header = header;
	packet->data = NULL;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	//printf("[send_join_packet]type: %d\n", HTTP_REQ_POST);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_leave_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int8 chat_id) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = LEAVE_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->data_len = 0;
	
	leave_desc *leave = (leave_desc*)malloc(sizeof(leave_desc));
	leave->chat_id = chat_id;
	packet->desc = (char*)leave;

	header->desc_len = sizeof(leave_desc);
		
	packet->header = header;
	packet->data = NULL;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_ping_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int1 status) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = PING_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->data_len = 0;
	
	ping_desc *ping = (ping_desc*)malloc(sizeof(ping_desc));
	ping->status = status;
	packet->desc = (char*)ping;

	header->desc_len = sizeof(ping_desc);
		
	packet->header = header;
	packet->data = NULL;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_pong_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int1 status) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = PONG_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->data_len = 0;
	
	pong_desc *pong = (pong_desc*)malloc(sizeof(pong_desc));
	pong->status = status;
	packet->desc = (char*)pong;

	header->desc_len = sizeof(pong_desc);
		
	packet->header = header;
	packet->data = NULL;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	printf("[send_pong_packet]Sending\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_list_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = LIST_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->data_len = 0;
	
	list_desc *list = (list_desc*)malloc(sizeof(list_desc));
	packet->desc = (char*)list;

	header->desc_len = sizeof(list_desc);
		
	packet->header = header;
	packet->data = NULL;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_listhits_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int8 data_len, char *data) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = LISTHITS_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->data_len = data_len;
	
	listhits_desc *listhits = (listhits_desc*)malloc(sizeof(listhits_desc));
	packet->desc = (char*)listhits;

	header->desc_len = sizeof(listhits_desc);
		
	packet->header = header;
	packet->data = data;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_message_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int8 data_len, u_int4 id_dest_len, u_int4 msg_len, char *id_dest, char *msg) {

	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = MESSAGE_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->data_len = data_len;
	
	message_desc *message = (message_desc*)malloc(sizeof(message_desc));
	message->id_dest_len = id_dest_len;
	message->msg_len = msg_len;
	
	packet->desc = (char*)message;

	header->desc_len = sizeof(message_desc);
		
	packet->header = header;
	
	char *temp = (char*)malloc(data_len);
	memcpy(temp, id_dest, id_dest_len);
	char *iter = temp+id_dest_len;
	memcpy(iter, msg, msg_len);
	packet->data = temp;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_post_response_packet(u_int4 fd, u_int4 status) {
	char *buffer;
	int len;
	printf("[send_post_response_packet]Send on socket %d\n", fd);
	http_packet *h_packet = http_create_packet(NULL, HTTP_RES_POST, status, NULL, 0, 0, NULL, 0);
	if(h_packet!=NULL)
		printf("[send_post_response_packet]Http created\n");
	else {
		printf("[send_post_response_packet]Not created\n");
		return 0;
	}
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	printf("[send_post_response_packet]Buffered\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_get_request_packet(u_int4 fd, char *filename, u_int4 range_start, u_int4 range_end) {
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(NULL, HTTP_REQ_GET, 0, filename, range_start, range_end, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_get_response_packet(u_int4 fd, u_int4 status, u_int4 data_len, char *data) {
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(NULL, HTTP_RES_GET, status, NULL, 0, 0, data, data_len);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}