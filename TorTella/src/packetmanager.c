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

u_int4 send_search_packet(u_int4 fd, u_int8 packet_id, u_int8 sender_id, u_int8 recv_id, u_int1 ttl, u_int1 hops, u_int4 string_len, char *string) {
	//printf("[send_search_packet]Preparing\n");
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = (packet_id>0 ? packet_id : generate_id());
	header->desc_id = SEARCH_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = string_len;
	
	search_desc *search = (search_desc*)malloc(sizeof(search_desc));
	search->ttl = ttl;
	search->hops = hops;
	packet->desc = (char*)search;

	header->desc_len = sizeof(search_desc);
		
	packet->header = header;
	packet->data = string;
	
	printf("[send_search_packet]sending query: %s\n", string);
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	//printf("[send_join_packet]type: %d\n", HTTP_REQ_POST);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_searchhits_packet(u_int4 fd, u_int8 packet_id, u_int8 sender_id, u_int8 recv_id, u_int4 num_res, u_int4 res_len, char *res) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = (packet_id>0 ? packet_id : generate_id());
	header->desc_id = SEARCHHITS_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = res_len;
	
	searchhits_desc *searchhits = (searchhits_desc*)malloc(sizeof(searchhits_desc));
	searchhits->num_res = num_res;
	packet->desc = (char*)searchhits;

	header->desc_len = sizeof(searchhits_desc);
		
	packet->header = header;
	packet->data = res;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	//printf("[send_join_packet]type: %d\n", HTTP_REQ_POST);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_join_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int1 status, u_int8 chat_id, char *nick) {
	
	printf("[send_join_packet]nick: %s\n", nick);
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = JOIN_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = strlen(nick);
	
	join_desc *join = (join_desc*)malloc(sizeof(join_desc));
	join->status = status;
	join->chat_id = chat_id;
	packet->desc = (char*)join;

	header->desc_len = sizeof(join_desc);
		
	packet->header = header;
	packet->data = nick;
	
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
	header->timestamp = time(NULL);
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

u_int4 send_ping_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, char *nick, u_int4 port, u_int1 status) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = PING_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = strlen(nick);
	
	ping_desc *ping = (ping_desc*)malloc(sizeof(ping_desc));
	ping->status = status;
	ping->port = port;
	packet->desc = (char*)ping;

	header->desc_len = sizeof(ping_desc);
		
	packet->header = header;
	packet->data = nick;
	
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
	header->timestamp = time(NULL);
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
	//printf("[send_pong_packet]Sending\n");
	
	return send_packet(fd, buffer, len);
}

u_int4 send_list_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = LIST_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
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
	header->timestamp = time(NULL);
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

u_int4 send_message_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int8 chat_id, u_int4 msg_len, char *msg) {

	//printf("[send_message_packet]Sending msg: %s, msg_len: %d\n", msg, msg_len);
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = MESSAGE_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = msg_len;
	
	message_desc *message = (message_desc*)malloc(sizeof(message_desc));
	message->chat_id = chat_id;
	
	packet->desc = (char*)message;

	header->desc_len = sizeof(message_desc);
		
	packet->header = header;
	
	/*printf("[send_message_packet]id_dest %s\n", id_dest);
	char *temp = (char*)malloc(header->data_len);
	memcpy(temp, id_dest, id_dest_len);
	printf("[send_message_packet] temp: %s\n", temp);
	char *iter = temp;
	iter += id_dest_len;
	memcpy(iter, msg, msg_len);
	printf("[send_message_packet] temp: %s, msg: %s\n", temp, msg);
	packet->data = temp;*/
	packet->data = msg;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	//printf("[send_message_packet]data: %s\n", dump_data(h_packet->data->data, h_packet->data->header->data_len));
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	//printf("[send_message_packet]Sent %s, id_dest_len: %d\n", dump_data(buffer, len), message->id_dest_len);
	
	return send_packet(fd, buffer, len);
}

u_int4 send_create_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int8 chat_id, u_int4 title_len, char *title) {
	//printf("[send_create_packet]Sending title: %s, title_len: %d\n", title, title_len);
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = CREATE_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = title_len;
	
	create_desc *create = (create_desc*)malloc(sizeof(create_desc));
	create->chat_id = chat_id;
	
	packet->desc = (char*)create;

	header->desc_len = sizeof(create_desc);
		
	packet->header = header;
	
	packet->data = title;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	//printf("[send_create_packet]data: %s\n", dump_data(h_packet->data->data, h_packet->data->header->data_len));
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	//printf("[send_message_packet]Sent %s, id_dest_len: %d\n", dump_data(buffer, len), message->id_dest_len);
	
	return send_packet(fd, buffer, len);
}

u_int4 send_post_response_packet(u_int4 fd, u_int4 status, u_int4 data_len, char *data) {
	char *buffer;
	int len;
	//printf("[send_post_response_packet]Send on socket %d\n", fd);
	//printf("[send_post_response_packet]data_len: %d\n", data_len);
	http_packet *h_packet = http_create_packet(NULL, HTTP_RES_POST, status, NULL, 0, 0, data, data_len);
	if(h_packet!=NULL)
		printf("[send_post_response_packet]Http created\n");
	else {
		printf("[send_post_response_packet]Not created\n");
		return 0;
	}
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	//printf("[send_post_response_packet]Buffered, len: %d\n", len);
	free(h_packet->header_response);
	free(h_packet);
	
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
