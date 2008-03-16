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
 
#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "common.h"
#include "packetmanager.h"
#include <glib.h>

struct chatclient {
	u_int8 id;
	char *nick;
	char *ip;
	u_int4 port;
};
typedef struct chatclient chatclient;

struct chat {
	u_int8 id;
	char *name;
	GSList *users;
};
typedef struct chat chat;

GSList *chatclient_list;
GSList *

//Avvia la chatserver
u_int4 chatserver_start(char *ip, u_int4 port);

//-----THREAD---------

//Avvia il server
void *chatserver_listen(void *parm);

//Ne viene avviato uno per ogni client
void *chatserver_responde(void *parm);


#endif //CHATSERVER_H
