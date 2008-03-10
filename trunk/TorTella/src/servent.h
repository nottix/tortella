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
 
#ifndef SERVENT_H
#define SERVENT_H

#include "packetmanager.h"
#include "utils.h"

#define RECV_MAX_LEN 4000;

u_int1 last_request_type = 0;

//Crea un server socket
u_int4 servent_create_server(char *src_ip, u_int4 src_port);

//Crea un client socket
u_int4 servent_create_client(char *dst_ip, u_int4 dst_port);

//Thread che riceve i pacchetti e risponde adeguatamente
u_int4 servent_listen_and_responde(void *parm);

u_int4 servent_start();

#endif //SERVENT_H
