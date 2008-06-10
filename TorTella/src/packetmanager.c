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

int send_search_packet(u_int4 fd, u_int8 packet_id, u_int8 sender_id, u_int8 recv_id, u_int1 ttl, u_int1 hops, u_int4 string_len, char *string) {

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
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_search_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}

int send_searchhits_packet(u_int4 fd, u_int8 packet_id, u_int8 sender_id, u_int8 recv_id, u_int4 num_res, u_int4 res_len, char *res) {
	
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
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_searchhits_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}

int send_join_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int1 status, u_int8 chat_id, char *nick) {
	
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
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_join_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}

int send_leave_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int8 chat_id) {
	
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
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_leave_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}

int send_ping_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, char *nick, u_int4 port, u_int1 status) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	logger(PAC_INFO, "[send_ping_packet]Send PING from %lld to %lld\n", sender_id, recv_id);

	header->id = generate_id();
	header->desc_id = PING_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = strlen(nick);
	
	ping_desc *ping = (ping_desc*)calloc(sizeof(ping_desc), 1);
	ping->status = status;
	ping->port = port;
	packet->desc = (char*)ping;

	header->desc_len = sizeof(ping_desc);
		
	packet->header = header;
	packet->data = strdup(nick);
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_ping_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}

int send_list_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int1 ttl, u_int1 hops, u_int8 chat_id) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = LIST_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = 0;
	
	list_desc *list = (list_desc*)malloc(sizeof(list_desc));
	list->chat_id = chat_id;
	list->ttl = ttl;
	list->hops = hops;
	packet->desc = (char*)list;

	header->desc_len = sizeof(list_desc);
	
	packet->header = header;
	packet->data = NULL;
	
	printf("[send_list_packet]sending request ID: %lld\n", chat_id);
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_list_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}

int send_listhits_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int4 user_num, u_int4 res_len, char *res, u_int8 chat_id) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = LISTHITS_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = res_len;
	
	listhits_desc *listhits = (listhits_desc*)malloc(sizeof(listhits_desc));
	listhits->user_num = user_num;
	listhits->chat_id = chat_id;
	packet->desc = (char*)listhits;

	header->desc_len = sizeof(listhits_desc);
	
	packet->header = header;
	packet->data = res;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_listhits_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}

int send_bye_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id) {
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = BYE_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = 0;
	
	bye_desc *bye = (bye_desc*)malloc(sizeof(bye_desc));
	packet->desc = (char*)bye;

	header->desc_len = sizeof(bye_desc);
		
	packet->header = header;
	packet->data = NULL;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_bye_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}

int send_message_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int8 chat_id, u_int4 msg_len, char *msg) {

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
	
	packet->data = msg;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_message_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}

int send_post_response_packet(u_int4 fd, u_int4 status, u_int4 data_len, char *data) {
	printf("[send_post_response_packet]Sending with status: %d\n", status);
	char *buffer;
	int len;

	http_packet *h_packet = http_create_packet(NULL, HTTP_RES_POST, status, NULL, 0, 0, data, data_len);
	if(h_packet!=NULL)
		printf("[send_post_response_packet]Http created\n");
	else {
		printf("[send_post_response_packet]Not created\n");
		return -1;
	}
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_post_response_packet]Error while creating packet\n");
		return -1;
	}

	free(h_packet->header_response);
	free(h_packet);
	
	return send_packet(fd, buffer, len);
}

int send_get_request_packet(u_int4 fd, char *filename, u_int4 range_start, u_int4 range_end) {
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(NULL, HTTP_REQ_GET, 0, filename, range_start, range_end, NULL, 0);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_get_request_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}

int send_get_response_packet(u_int4 fd, u_int4 status, u_int4 data_len, char *data) {
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(NULL, HTTP_RES_GET, status, NULL, 0, 0, data, data_len);
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL) {
		logger(PAC_INFO, "[send_get_response_packet]Error while creating packet\n");
		return -1;
	}
	
	return send_packet(fd, buffer, len);
}
