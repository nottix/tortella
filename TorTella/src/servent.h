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

#include "common.h"
#include "packetmanager.h"
#include "utils.h"
#include "list.h"
#include "supernodedata.h"
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <glib.h>

#define RECV_MAX_LEN 4000;
#define THREAD_MAX 20
#define FD_MAX 100

static u_int4 timer_interval = 5;

struct servent_data {
	u_int8 id;
	char *ip;
	u_int4 port;
	u_int1 status;
	char *nick;
	time_t timestamp;	//Timestamp ricezione pacchetto
	
	GSList *chat_list;	//Lista delle chat a cui è connesso il servente
	
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	pthread_mutex_t mutex_data;
	
	char *msg; 			//Messaggio da inviare o ricevuto
	u_int4 msg_len;		//Lunghezza messaggio

	u_int1 is_supernode;	//Indica se il servente è un supernodo 1:si 0:no
	
	//USATE SOLO IN LOCALE
	u_int4 post_type;	//Tipo di pacchetto da inviare
	
	u_int8 chat_id_req; //Chat a cui connettersi o creare
	
	char *title;	//Titolo chat da creare
	u_int4 title_len;	//Lunghezza del titolo
	
};
typedef struct servent_data servent_data;

GHashTable *servent_hashtable;

static servent_data *local_servent;

static u_int1 last_request_type = 0;
static u_int4 server_connection_num = 0;

//static list *connection_list;

/*static list *client_fd;
static list *server_fd;
static list *server_connection_fd;

static list *client_thread;
static list *server_thread;
static list *server_connection_thread;*/

pthread_t *timer_thread;

GSList *client_fd;
GSList *server_fd;
GSList *server_connection_fd;

GSList *client_thread;
GSList *server_thread;
GSList *server_connection_thread;

#define LOCK(servent)			pthread_mutex_lock( &((servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(servent)))->mutex_data )
#define UNLOCK(servent)			pthread_mutex_unlock( &((servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(servent)))->mutex_data )

//Crea un server socket
u_int4 servent_create_server(char *src_ip, u_int4 src_port);

//Crea un client socket
u_int4 servent_create_client(char *dst_ip, u_int4 dst_port);

u_int4 servent_start(char *ip, u_int4 port);

void servent_close_all(void);

void kill_all_thread(int sig);

void servent_init(char *ip, u_int4 port, u_int1 status);

//u_int4 servent_add_chat(servent_data *s_data, u_int8 id, u_int8 chat_id);

//-----THREAD--------------

//Thread che riceve i pacchetti e risponde adeguatamente
void *servent_listen(void *parm);

void *servent_responde(void *parm);

void *servent_connect(void *parm);

void *servent_timer(void *parm);

#endif //SERVENT_H
