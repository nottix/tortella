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
#include <pthread.h>
#include <dirent.h>
#include "logger.h"

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
	pthread_mutex_t mutex;
};
typedef struct chat chat;

static GHashTable *chatclient_hashtable;
static GHashTable *chat_hashtable;

/*
 * Scrive la struttura dati 'chat' in un file, nel seguente modo:
 * chat_id;chat_title;
 * user_id;user_nick;user_ip;user_port;
 * ...
 *
 * In modalità TRUNC crea ogni volta un nuovo file, mentre
 * nella modalità APPEND modifica il file esistente.
 */
int write_to_file(const char *filename, chat *chat_str, u_int4 mode);

int write_all(u_int4 mode);

/*
 * Legge le informazioni delle chat e degli utenti dal file specificato.
 * Aggiunge i dati sulla chat alla hashtable relativa, inoltre i dati degli utenti
 * alla hashtable relativa.
 */
int read_from_file(const char *filename);

int read_all(void);

int data_add_chat(u_int8 id, const char *title);

int data_add_all_to_chat(GList *chats);

int data_del_chat(u_int8 id);

int data_add_user(u_int8 id, const char *nick, const char *ip, u_int4 port);

int data_add_existing_user_to_chat(u_int8 chat_id, u_int8 id);

int data_add_users_to_chat(u_int8 chat_id, GList *users);

int data_add_user_to_chat(u_int8 chat_id, u_int8 id, const char *nick, const char *ip, u_int4 port);

int data_del_user(u_int8 id);

int data_del_user_from_chat(u_int8 chat_id, u_int8 id);

chat *data_search_chat_local(const char *title);

/*
 * Cerca nella hashtable chat_table l'occorrenza della chat title
 * Ritorna la struttura dati della chat
 */
chat *data_search_chat(const char *title);

/*
 * Cerca nella hashtable chat_table tutte le chat che hanno come titolo *title*
 * Ritorna le chat in una slist
 */
GList *data_search_all_chat(const char *title);

GList *data_search_all_local_chat(const char *title);

chatclient *data_search_chatclient(const char *nick);

/*
 * Converte la lista di chat in una stringa del tipo:
 * 111;test;
 * 22;simone;127.0.0.1;2110;
 * 33;simon;127.0.0.1;2110;
 */
char *data_chatlist_to_char(GList *chat_list, int *len);

/*
 * Converte una stringa in una lista di chat con i relativi utenti
 */
GList *data_char_to_chatlist(const char *buffer,int len);

/*
 * Converte la lista di utenti in una stringa del tipo:
 * 22;simone;127.0.0.1;2110;
 * 33;simon;127.0.0.1;2110;
 */
char *data_userlist_to_char(GList *user_list, int *len);

/*
 * Converte una stringa in una lista di utenti
 */
GList *data_char_to_userlist(const char *buffer,int len);

/*
 * Ritorna una lista di tutti i client della chat specificata
 */
GList *data_get_chatclient_from_chat(u_int8 id);

chat *data_get_chat(u_int8 chat_id);

chatclient *data_get_chatclient(u_int8 id);

#endif //SUPERNODEDATA_H
