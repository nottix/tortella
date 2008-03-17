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
 
#ifndef SUPERNODEDATA_H
#define SUPERNODEDATA_H

#include "common.h"
#include "utils.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MODE_TRUNC 0x900
#define MODE_APPEND 0x901

struct chatclient {
	u_int8 id;
	char *nick;
	char *ip;
	u_int4 port;
};
typedef struct chatclient chatclient;

struct chat {
	u_int8 id;
	char *title;
	GHashTable *users;
};
typedef struct chat chat;

GHashTable *chatclient_hashtable;
GHashTable *chat_hashtable;

/*
 * Scrive la struttura dati 'chat' in un file, nel seguente modo:
 * chat_id;chat_title;
 * user_id;user_nick;user_ip;user_port;
 * ...
 *
 * In modalità TRUNC crea ogni volta un nuovo file, mentre
 * nella modalità APPEND modifica il file esistente.
 */
u_int4 write_to_file(const char *filename, chat *chat_str, u_int4 mode);

/*
 * Legge le informazioni delle chat e degli utenti dal file specificato.
 * Aggiunge i dati sulla chat alla hashtable relativa, inoltre i dati degli utenti
 * alla hashtable relativa.
 */
u_int4 read_from_file(const char *filename, GHashTable **chat_table, GHashTable **chatclient_table);

u_int4 add_chat(u_int8 id, const char *title, GHashTable **chat_table);

u_int4 del_chat(u_int8 id, GHashTable *chat_table);

u_int4 add_user(u_int8 chat_id, u_int8 id, const char *nick, const char *ip, u_int4 port, GHashTable *chat_table, GHashTable **chatclient_table);

u_int4 del_user(u_int8 chat_id, u_int8 id, GHashTable *chat_table, GHashTable *chatclient_table);


//-----THREAD---------

#endif //SUPERNODEDATA_H
