#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "servent.h"
#include "common.h"
#include "socketmanager.h"
#include "supernodedata.h"
#include "confmanager.h"
#include <glib.h>

//EXTERN
//extern GHashTable *servent_hashtable;
extern GHashTable *chat_hashtable;
extern GHashTable *chatclient_hashtable;

//Aggiunto il flag subset per vedere se il messaggio è da mandare solo a determinati  utenti, userslist contiene la lista degli utenti a cui inviare il messaggio
int controller_send_chat_users(u_int8 chat_id, u_int4 msg_len, char *msg);

//Se devo mandare il mess ad un sottoinsieme di utenti
int controller_send_subset_users(u_int8 chat_id, u_int4 msg_len, char *msg, GList *users);

int controller_send_pm(u_int4 msg_len, char *msg, u_int8 recv_id);

int controller_join_chat(u_int8 chat_id);

int controller_leave_chat(u_int8 chat_id);

int controller_connect_users(GList *users);

int controller_init(const char *filename, const char *cache);

int controller_menu(void);

int controller_search(const char *query);

#endif /*CONTROLLER_H_*/
