/**
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

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "servent.h"
#include "common.h"
#include "socketmanager.h"
#include "datamanager.h"
#include "confmanager.h"
#include <glib.h>
#include "gui.h"
#include "utils.h"

pthread_t gtk_main_thread;

/** 
 * cambia lo status di un peer tramite l'invio di un PING a tutti gli utenti
 * e controlla che il pacchetto sia stato ricevuto correttamente dagli altri peer
 */
int controller_change_status(u_int1 status); 

/** 
 * notifica il cambiamento di status di un peer remoto tramite una chiamata alla gui
 */
int controller_manipulating_status(u_int8 user_id, u_int1 status); 

/** 
 * invia un messaggio a tutti gli utenti di una chat e controlla che il pacchetto
 * sia stato ricevuto correttamente dagli altri peer.
 */
int controller_send_chat_users(u_int8 chat_id, u_int4 msg_len, char *msg);

/** 
 * invia un messaggio ad un sottoinsieme di utenti di una chat e controlla che 
 * il pacchetto sia stato ricevuto correttamente dagli utenti interessati.
 */
int controller_send_subset_users(u_int8 chat_id, u_int4 msg_len, char *msg, GList *users);

/** 
 * invia un messaggio privato ad un utente e si accerta che questo sia stato
 * ricevuto correttamente.
 */
int controller_send_pm(u_int4 msg_len, char *msg, u_int8 recv_id);

/** permette all'utente di abbandonare tutte le chat a cui era connesso */
int controller_leave_all_chat();

/** 
 * stabilisce una connessione tra un peer e una lista di utenti di una chat tramite
 * l'avvio del client.
 */
int controller_connect_users(GList *users);

/** 
 * funzione d'appoggio per la controller_connect_users(), verifica che il peer
 * con cui si sta cercando di stabilire una connessione sia effettivamente pronto.??????????
 */
int controller_check_users_con(GList *users);

/**
 * permette l'invio di un pacchetto di tipo BYE che notifica a tutti gli utenti
 * conosciuti che si sta abbandonando l'applicazione e controlla che tutti abbiano 
 *ricevuto il messaggio correttamente.
 */
int controller_send_bye(); 

/**
 * Alla ricezione di una BYE da un utente remoto chiude tutte le conversazioni
 * private aperte (se esistenti) con quello specifico utente.
 */
int controller_receive_bye();

/**
 * Legge il file di configurazione, avvia il logger, legge il file init_data che
 * contiene ip e porta dei peer vicini, avvia il servent e infine avvia il timer
 * necessario per il meccanismo di failure detection.
 */
int controller_init(const char *filename, const char *cache);

/**
 * Uccide tutti i thread e chiude il file di logger.
 */
int controller_exit();

/**
 * Costruisce la prima finestra dell'interfaccia grafica, relativa alla creazione
 * e alla ricerca delle chat e avvia l'apposito thread per la gestione degli eventi
 */
int controller_init_gui(void);

/**
 * Controlla che la query inserita sia accettabile, dopo di che invia un pacchetto
 * di tipo SEARCH a tutti gli utenti conosciuti, evitando l'invio ai fake id e a 
 * se' stesso.
 */
u_int8 controller_search(const char *query);

/**
 * Invia un pacchetto di tipo JOIN a tutti gli utenti conosciuti (ammesso che ne conosca
 * qualcuno, evitando l'invio ai fake id e a se' stesso; successivamente, ogni volta
 * che riceve un messaggio di OK da un utente, aggiunge questi alla chat; infine
 * aggiunge anche se' stesso.
 */
int controller_join_flooding (u_int8 chat_id);

/**
 * Invia un pacchetto di tipo LEAVE a tutti gli utenti conosciuti per avvertire che
 * si sta abbandonando la chat (rappresentata dal parametro chat_id) e rimuove 
 * questa dall'elenco delle chat a cui si e' connessi, dopo di che attende che tutti 
 * abbiano ricevuto il messaggio correttamente.
 */
int controller_leave_flooding(u_int8 chat_id);

/**
 * Consente ad un utente di creare una chat purche' abbia un nome che non sia nullo o
 * rappresentato da una stringa vuota. Successivamente viene generato un id da associare
 * alla chat e infine viene aperta la gui relativa alla chat con conseguente aggiunta
 * dell'utente alla lista dei peer partecipanti alla chat.
 */
int controller_create(const char *title);

/**
 * Chiama la funzione gui_add_user_to_chat controllando prima che l'utente che
 * si sta cercando di inserire abbia tutti i campi inizializzati correttamente. Nel
 * caso invece l'utente non sia conosciuto e quindi sia inizialmente NULL viene 
 * aggiunto alla chat un utente in modo provvisorio.
 */
int controller_add_user_to_chat(u_int8 chat_id, u_int8 id);

/**
 * Funzione d'appoggio che rimuove l'utente dalla chat a cui era connesso, sia a 
 * livello di gui che a livello di data_manager.
 */
int controller_rem_user_from_chat(u_int8 chat_id, u_int8 id);

/**
 * Permette ad un utente, una volta ricevuto un messaggio da un partecipante
 * alla chat, di aggiornare la text view della chat stessa.
 */
int controller_add_msg_to_chat(u_int8 chat_id, char *msg);

/**
 * Come la controller_add_msg_to_chat, ma utilizzata nel caso venga ricevuto 
 * un messaggio privato.
 */
int controller_add_msg(u_int8 sender_id, char *msg);

#endif /**CONTROLLER_H_*/
