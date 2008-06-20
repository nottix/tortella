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

/**
 * \file Servent
 *
 * \brief Gestisce il comportamento dei peer
 *
 * Crea le connessioni tra peer, gestisce l'invio e la ricezione dei pacchetti.
 *
 * \author $Author: Simone Notargiaco, Lorenzo Fortunato, Ibrahim Khalili $
 *
 * \version $Revision: 0.1 $
 *
 * \date $Date: 2008/06/18 14:16:20 $
 *
 * Contact: notargiacomo.s@gmail.com
 */

#ifndef SERVENT_H
#define SERVENT_H

#define _GNU_SOURCE

#include "common.h"
#include "packetmanager.h"
#include "utils.h"
#include "datamanager.h"
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
#include <sys/stat.h>
#include <string.h>

#define TIMEOUT "timeout error"

/**
 * Tale struttura dati viene utilizzata per le operazioni di comunicazioni
 * con un servente a cui si e' connessi. Ne e' presente una anche per il peer locale
 */
struct servent_data {
 u_int8 id;				//!ID del peer rappresentato dalla struttura dati
 GQueue *queue;			//!Coda utilizzata per serializzare le richieste di invio pacchetti
 GQueue *res_queue;		//!Coda utilizzata per serializzare le risposte dei pacchetti inviati
 char *ip;
 u_int4 port;
 u_int1 status;			//!Stato del peer (ONLINE, BUSY, AWAY)
 char *nick;
 time_t timestamp;		//!Timestamp ricezione pacchetto
 
 GList *chat_list;		//!Lista delle chat a cui e' connesso il peer
 
 pthread_rwlock_t rwlock_data;	//!Serve per sincronizzare gli accessi ai dati del peer
 
 char *msg; 				//!Messaggio da inviare
 u_int4 msg_len;			//!Lunghezza messaggio
 
 GList *chat_res; 		//!Risultati della ricerca richiesta dal peer
 u_int1 ttl;				//!ttl da inviare
 u_int1 hops;			//!hops da inviare
 u_int8 packet_id;		//!ID del pacchetto da ritrasmettere
 
 u_int4 post_type;		//!Tipo di pacchetto da inviare
 u_int1 is_online;		//!Flag che indica se il peer e' pronto a ricevere pacchetti (viene impostato alla ricezione del primo PING): 1 e 0
 
 //!FLOODING
 u_int8 user_id_req;		//!Utente che si vuole connettere alla chat con id: chat_id_req
 u_int8 chat_id_req; 	//!Chat a cui connettersi o creare
 u_int4 port_req;		//!PORT del join da rinviare
 char *nick_req;			//!NICK del join da rinviare
 char *ip_req;			//!IP del join da rinviare
 u_int1 status_req;		//!Status per il redirect del join
 char *title;			//!Titolo chat da creare o ricercare
 u_int4 title_len;		//!Lunghezza del titolo
 
};
typedef struct servent_data servent_data;

static GHashTable *servent_hashtable;	//!Hashtable di servent_data, una per ogni peer

//!---Routing Hashtables-----

static GHashTable *search_packet_hashtable = NULL;

static GHashTable *join_packet_hashtable = NULL;

static GHashTable *leave_packet_hashtable = NULL;

//!---------------------------

static servent_data *local_servent;		//!Dati del peer locale

static u_int8 new_connection_counter;	//!Limite inferiore generazione ID

static pthread_t *timer_thread;			//!identificatore timer thread;

static GList *client_fd;				//!Lista client socket
static GList *server_fd;				//!Lista server socket in attesa di connessioni
static GList *server_connection_fd;		//!Lista server socket connessi

static GList *client_thread;			//!Lista identificatori client thread
static GList *server_thread;			//!Lista identificatori server thread in attesa di connessioni
static GList *server_connection_thread;	//!Lista identificatori server thread connessi

//!Macro di utilita'

#define WLOCK(servent)			logger(SYS_INFO, "[WLOCK]Try locking %lld\n", servent); \
if(servent_get(servent)!=NULL) { \
pthread_rwlock_wrlock( &((servent_get(servent)->rwlock_data))); \
logger(SYS_INFO, "[WLOCK]Lock %lld\n", servent); \
}

#define RLOCK(servent)			logger(SYS_INFO, "[RLOCK]Try locking %lld\n", servent); \
if(servent_get(servent)!=NULL) { \
pthread_rwlock_rdlock( &((servent_get(servent)->rwlock_data))); \
logger(SYS_INFO, "[RLOCK]Lock %lld\n", servent); \
}

#define UNLOCK(servent)			logger(SYS_INFO, "[UNLOCK]Try unlocking %lld\n", servent); \
if(servent_get(servent)!=NULL) { \
pthread_rwlock_unlock( &((servent_get(servent)->rwlock_data))); \
logger(SYS_INFO, "[UNLOCK]Unlock %lld\n", servent); \
}

#define UNLOCK_F(servent)		pthread_rwlock_unlock( &(((servent)->rwlock_data)) ); \
logger(SYS_INFO, "[UNLOCK_F]Unlock %lld\n", servent->id);

//!Tale macro viene utilizzata per copiare un servent_data in un altro
#define COPY_SERVENT(servent, copy)			copy=calloc(1, sizeof(servent_data)); \
memcpy(copy, servent, sizeof(servent_data))

/**
  * Crea un server socket
  */
int servent_create_server(char *src_ip, u_int4 src_port);

/**
  * Crea un client socket
  */
int servent_create_client(char *dst_ip, u_int4 dst_port);

/**
  * Avvia il server thread che viene usato per accettare
  * le nuove connessioni e quindi lanciare nuovi thread
  * per gestirle
  */
int servent_start_server(char *local_ip, u_int4 local_port);

/**
  * Avvia un client thread usato per gestire tutte le richieste di invio
  * pacchetti al peer associato.
  * 
  * \param id Se si conosce l'id del peer a cui connettersi lo si specifica, altrimenti 0.
  */
servent_data *servent_start_client(char *dest_ip, u_int4 dest_port, u_int8 id);

/**
  * Questa funzione viene utilizzata per il boot iniziale, in quanto si connette
  * alla lista dei peer conosciuti, presi dal file di init. Inoltre inizializza
  * tutte le variabili necessarie e avvia il server di ascolto nuove connessioni.
  *
  * \param init_servent Lista dei serventi necessari per il boot iniziale
  */
int servent_start(GList *init_servent);

/**
  * Tale funzione non fa altro che avviare il timer thread utilizzato per supportare
  * il meccanismo di failure detection.
  */
int servent_start_timer(void);

/**
  * Si connette alla lista dei peer specificati, se qualcuno non e' disponibile lo salta.
  */
int servent_init_connection(GList *init_servent);

/**
  * Questa funzione viene chiamata alla chiusura di TorTella, serve per chiudere
  * tutti i socket aperti (non brutalmente).
  */
void servent_close_all(void);

/**
  * Ultima funzione chiamata alla chiusura del programma, termina tutti i thread attivi.
  */
void servent_kill_all_thread(int sig);

/**
  * Inizializza: le variabili del peer locale, il seed, le hashtable, i lock etc...
  */
int servent_init(char *ip, u_int4 port, u_int1 status);

/**
  * Funzione utilizzata per il recupero delle chat conosciute da file,
  * attualmente non piu' utilizzata.
  */
void servent_init_supernode();

/**
  * Funzione utilizzata per il salvataggio delle chat conosciute su file,
  * attualmente non piu' utilizzata.
  */
void servent_close_supernode();

//!-----Gestione servent_data-----

/**
  * Restituisce il servent_data associato all'id richiesto.
  */
servent_data *servent_get(u_int8 id);

/**
  * Restituisce la lista completa delle servent_data.
  */
GList *servent_get_values(void);

/**
  * Restituisce la lista completa delle chiavi associate alle servent_data.
  */
GList *servent_get_keys(void);

/**
  * Restituisce la servent_data del peer locale.
  */
servent_data *servent_get_local(void);

//!-----Gestione Queue-----

/**
  * Aggiunge alla coda di pacchetti da inviare ad uno specifico peer.
  * In particolare si passa una servent_data contenete tutti i dati
  * necessari all'invio del pacchetto.
  */
void servent_send_packet(servent_data *sd);

/**
  * Rimuove dalla coda di pacchetti da inviare ad uno specifico peer.
  * Se non ci sono pacchetti da rimuovere rimane in attesa.
  */
servent_data *servent_pop_queue(servent_data *sd);

/**
  * Appende alla coda di risposta di uno specifico peer.
  * In particolare aggiunge la risposta ricevuta dopo l'invio di un pacchetto.
  */
void servent_append_response(servent_data *sd, const char *response);

/**
  * Rimuove dalla coda di risposta un elemento se presente,
  * nel frattempo avvia un timer per rilevare eventuali timeout di risposta.
  */
char *servent_pop_response(servent_data *sd);

/**
  * Pulisce la lista dei pacchetti ricevuti che serve per scartare
  * pacchetti con ID uguale a quelli ricevuti recentemente.
  */
void servent_flush_data(void);

//!-----Routing-----

/**
  * Ritorna l'ID del pacchetto richiesto, se presente.
  * E' una sorta di verifica presenta pacchetto.
  * Utilizzato per gestire i duplicati.
  */
char *servent_get_search_packet(u_int8 id);

/**
  * Aggiunge il pacchetto alla lista dei pacchetti ricevuti.
  */
void servent_new_search_packet(u_int8 id);

/**
  * Ritorna l'ID del pacchetto richiesto, se presente.
  * E' una sorta di verifica presenta pacchetto.
  * Utilizzato per gestire i duplicati.
  */
char *servent_get_join_packet(u_int8 id);

/**
  * Aggiunge il pacchetto alla lista dei pacchetti ricevuti.
  */
void servent_new_join_packet(u_int8 id);

/**
  * Ritorna l'ID del pacchetto richiesto, se presente.
  * E' una sorta di verifica presenta pacchetto.
  * Utilizzato per gestire i duplicati.
  */
char *servent_get_leave_packet(u_int8 id);

/**
  * Aggiunge il pacchetto alla lista dei pacchetti ricevuti.
  */
void servent_new_leave_packet(u_int8 id);

//!---------THREAD---------------

/**
  * Thread che riceve le richieste di connessione e avvia nuovi thread.
  * Ogni nuovo peer (client) che richiede di connettersi al server locale viene
  * assegnato ad un nuovo Thread che si occupera' di rispondere alle richieste del client.
  */
void *servent_listen(void *parm);

/**
  * Server thread che riceve i pacchetti e risponde adeguatamente. Ne esiste uno per ogni
  * peer a cui si e' connessi. Questa funzione e' il vero cuore di TorTella,
  * infatti gestisce tutti i comportamente dei programma in base ai pacchetti ricevuti.
  *
  * \param parm Socket descriptor della connessione
  */
void *servent_respond(void *parm);

/**
  * Client thread utilizzato per gestire il canale di invio pacchetti ad un peer.
  */
void *servent_connect(void *parm);

/**
  * Thread utilizzato per gestire il meccanismo di failure detection e per pulire
  * la lista dei pacchetti ricevuti. L'intervallo di tempo e' impostato nel file
  * di configurazione.
  */
void *servent_timer(void *parm);

#endif //!SERVENT_H
