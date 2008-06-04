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

#define _GNU_SOURCE

#include "common.h"
#include "packetmanager.h"
#include "utils.h"
#include "list.h"
#include "supernodedata.h"
#include "init.h"
#include "routemanager.h"
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "controller.h"
#include <glib.h>
#include "logger.h"
#include "confmanager.h"
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <string.h>

#define TIMEOUT "timeout error"

static u_int4 timer_interval = 5;

struct servent_data {
	u_int8 id;
	GQueue *queue;
	GQueue *res_queue;
	char *ip;
	u_int4 port;
	u_int1 status;
	char *nick;
	time_t timestamp;	//Timestamp ricezione pacchetto
	
	GList *chat_list;	//Lista delle chat a cui è connesso il servente
	
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	pthread_rwlock_t rwlock_data;
	
	char *msg; 			//Messaggio da inviare o ricevuto
	u_int4 msg_len;		//Lunghezza messaggio

	u_int1 is_supernode;	//Indica se il servente è un supernodo 1:si 0:no
	
	GList *chat_res; //Risultati della ricerca richiesta dal peer
	u_int1 ttl;		//ttl da inviare
	u_int1 hops;	//hops da inviare
	u_int8 packet_id;	//ID del pacchetto da ritrasmettere
	
	//USATE SOLO IN LOCALE
	u_int4 post_type;	//Tipo di pacchetto da inviare
	
	u_int8 chat_id_req; //Chat a cui connettersi o creare
	
	char *title;	//Titolo chat da creare o ricercare
	u_int4 title_len;	//Lunghezza del titolo
	
};
typedef struct servent_data servent_data;

static GHashTable *servent_hashtable;

static GHashTable *search_packet_hashtable = NULL;

static servent_data *local_servent;

static u_int1 last_request_type = 0;
static u_int4 server_connection_num = 0;

static u_int8 new_connection_counter = 10000;

static pthread_t *timer_thread;

static GSList *client_fd;
static GSList *server_fd;
static GSList *server_connection_fd;

static GSList *client_thread;
static GSList *server_thread;
static GSList *server_connection_thread;

#define WLOCK(servent)			logger(SYS_INFO, "[WLOCK]Lock %lld\n", servent); \
								if(servent_get(servent)!=NULL) \
									pthread_rwlock_wrlock( &((servent_get(servent)->rwlock_data)))

#define RLOCK(servent)			logger(SYS_INFO, "[RLOCK]Lock %lld\n", servent); \
								if(servent_get(servent)!=NULL) \
									pthread_rwlock_rdlock( &((servent_get(servent)->rwlock_data)))

#define UNLOCK(servent)			logger(SYS_INFO, "[UNLOCK]Unlock %lld\n", servent); \
								if(servent_get(servent)!=NULL) \
									pthread_rwlock_unlock( &((servent_get(servent)->rwlock_data)))

#define COPY_SERVENT(servent, copy)			copy=calloc(1, sizeof(servent_data)); \
											memcpy(copy, servent, sizeof(servent_data))

//Crea un server socket
int servent_create_server(char *src_ip, u_int4 src_port);

//Crea un client socket
int servent_create_client(char *dst_ip, u_int4 dst_port);

int servent_start_server(char *local_ip, u_int4 local_port);

int servent_start_client(char *dest_ip, u_int4 dest_port);

int servent_start(GList *init_servent);

int servent_start_timer(void);

int servent_init_connection(GList *init_servent);

void servent_close_all(void);

void kill_all_thread(int sig);

int servent_init(char *ip, u_int4 port, u_int1 status);

void servent_init_supernode();

void servent_close_supernode();

servent_data *servent_get(u_int8 id);

GList *servent_get_values(void);

GList *servent_get_keys(void);

servent_data *servent_get_local(void);

void servent_send_packet(servent_data *sd);

servent_data *servent_pop_queue(servent_data *sd);

void servent_append_response(servent_data *sd, const char *response);

char *servent_pop_response(servent_data *sd);

u_int8 *get_search_packet(u_int8 id);

void new_search_packet(u_int8 id);

//-----THREAD--------------

//Thread che riceve i pacchetti e risponde adeguatamente
void *servent_listen(void *parm);

void *servent_responde(void *parm);

void *servent_connect(void *parm);

void *servent_timer(void *parm);

#endif //SERVENT_H
