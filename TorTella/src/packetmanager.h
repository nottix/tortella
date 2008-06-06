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
 
#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include "common.h"
#include "tortellaprotocol.h"
#include "httpmanager.h"
#include "socketmanager.h"
#include "utils.h"
#include "logger.h"

int send_search_packet(u_int4 fd, u_int8 packet_id, u_int8 sender_id, u_int8 recv_id, u_int1 ttl, u_int1 hops, u_int4 string_len, char *string);

int send_searchhits_packet(u_int4 fd, u_int8 packet_id, u_int8 sender_id, u_int8 recv_id, u_int4 num_res, u_int4 res_len, char *res);

int send_join_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int1 status, u_int8 chat_id, char *nick);

int send_leave_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int8 chat_id);

int send_ping_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, char *nick, u_int4 port, u_int1 status);

int send_list_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int8 chat_id);

int send_listhits_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int4 user_num, u_int4 res_len, char *res);

int send_bye_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id);

int send_message_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id, u_int8 chat_id, u_int4 msg_len, char *msg);

int send_post_response_packet(u_int4 fd, u_int4 status, u_int4 data_len, char *data);

int send_get_request_packet(u_int4 fd, char *filename, u_int4 range_start, u_int4 range_end);

int send_get_response_packet(u_int4 fd, u_int4 status, u_int4 data_len, char *data);

#endif //PACKETMANAGER_H
