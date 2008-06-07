#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "servent.h"
#include "common.h"
#include "socketmanager.h"
#include "supernodedata.h"
#include "confmanager.h"
#include <glib.h>
#include "gui.h"
#include "utils.h"

pthread_t gtk_main_thread;

//EXTERN
//extern GHashTable *servent_hashtable;
extern GHashTable *chat_hashtable;
extern GHashTable *chatclient_hashtable;

int controller_change_status(u_int1 status); 

int controller_manipulating_status(u_int8 user_id, u_int1 status); 

//Aggiunto il flag subset per vedere se il messaggio è da mandare solo a determinati  utenti, userslist contiene la lista degli utenti a cui inviare il messaggio
int controller_send_chat_users(u_int8 chat_id, u_int4 msg_len, char *msg);

//Se devo mandare il mess ad un sottoinsieme di utenti
int controller_send_subset_users(u_int8 chat_id, u_int4 msg_len, char *msg, GList *users);

int controller_send_pm(u_int4 msg_len, char *msg, u_int8 recv_id);

int controller_join_chat(u_int8 chat_id);

int controller_leave_chat(u_int8 chat_id);

int controller_connect_users(GList *users);

int controller_init(const char *filename, const char *cache);

int controller_exit();

void *controller_start_main_thread(void *unused);

int controller_init_gui(void);

int controller_menu(void);

u_int8 controller_search(const char *query);

int controller_create(const char *title);

int controller_add_user_to_chat(u_int8 chat_id, u_int8 id);

int controller_rem_user_from_chat(u_int8 chat_id, u_int8 id);

int controller_add_msg_to_chat(u_int8 chat_id, char *msg);

int controller_add_msg(u_int8 sender_id, char *msg);

#endif /*CONTROLLER_H_*/
