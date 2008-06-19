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
 
#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include "utils.h"
#include "controller.h"
#include "datamanager.h"
#include "tortellaprotocol.h"

#define TOP		0x01
#define BOTTOM	0x02
#define CHAT 	0x03
#define PM 		0x04

#define ONLINE "Online"
#define BUSY   "Busy"
#define AWAY   "Away"

static GtkListStore *chat_model;
static GtkTreeIter   chat_iter;
static GHashTable *tree_model_hashtable = NULL;
static GHashTable *pm_data_hashtable = NULL;

GtkWidget *bar_textfield;

/**
 * Contiene i componenti della gui che vengono modificati costantemente, ovvero
 * le due text view (invio e ricezione dei messaggi) e i due componenti necessari
 * per gestire la lista degli utenti.
 */
struct tree_model
{
	GtkListStore *user_model;
	GtkTreeIter user_iter;
	GtkTextView *text_area;
	GtkTreeView *tree_view;
};
typedef struct tree_model tree_model;

/**
 * Contiene i componenti della finestra del pm che vengono modificati, ovvero
 * il widget relativo alla finestra e la text_area dei messaggi ricevuti.
 */
struct pm_data {
	GtkWidget *window;
	GtkTextView *text_area;
};
typedef struct pm_data pm_data;


//!FUNZIONI PER LA CREAZIONE DELLE GUI

/**
 * Provoca la semplice chiusura di una finestra
 */
gint gui_close_window_event(GtkWidget *widget, gpointer gdata);

/**
 * Consente l'uscita corretta dal programma, invia un messaggio di BYE e effettua
 * il leave da ogni chat a cui si e' connessi, infine chiama la controller_exit e 
 * termina l'interfaccia grafica.
 */
void gui_close_event (GtkWidget *widget, gpointer gdata);

/**
 * Funzione chiamata alla selezione di una chat, chiama il controller per gestire
 * la connessione agli utenti della chat e l'invio della JOIN, inoltre lancia la 
 * finestra relativa alla chat.
 */
void gui_open_chat_event (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer userdata);

/**
 * Invocata quando viene chiusa la chat, effettua la chiamata al controller per 
 * fargli gestire l'invio dei pacchetti di tipo LEAVE
 */
gint gui_leave_chat_event(GtkWidget *widget, gpointer gdata);

/**
 * Chiude la finestra relativa ad una conversazione privata e la rimuove dalla
 * hashtable.
 */
gint gui_leave_pm_event(GtkWidget *widget, gpointer gdata);

/**
 * Aggiunge una chat alla lista delle chat in seguito ad una ricerca che ha prodotto
 * risultati validi
 */
gint gui_add_chat(u_int8 chat_id, char *chat_name);

/**
 * Effettua un clear della lista delle chat
 */
gint gui_clear_chat_list();

/**
 * Effettua un clear della text view in cui vengono scritti i messaggi da inviare.
 * Viene invocata ogni volta che viene inviato un messaggio.
 */
gint gui_clear_buffer(GtkTextView *widget);

/**
 * Aggiunge alla chat il messaggio appena inviato e aggiorna lo scrolling della
 * finestra in modo che l'ultimo messaggio sia ben visibile.
 */
gint gui_add_message(GtkTextView *widget, gchar *msg);

/**
 * Aggiunge una utente alla chat, effettuando un controllo su eventuali duplicati 
 * in modo da poterli scartare.
 */
gint gui_add_user_to_chat(u_int8 chat_id, u_int8 id, char *user, u_int1 status);

/**
 * Scorre la lista degli utenti connessi fino a quando non viene trovato l'id 
 * dell'utente da eliminare, dopo di che questo viene eliminato
 */
gint gui_del_user_from_chat(u_int8 chat_id, u_int8 user_id);

/**
 * Scorre la lista degli utenti connessi fino a quando non viene trovato il record
 * da aggiornare, dopo di che viene aggiornato il campo status.
 */
gint gui_change_status(u_int8 user_id, char *status); 

/**
 * Evento che si scatena quando si effettua un doppio click su un utente della chat. 
 * Permette di aprire una nuova finestra per una conversazione privata.
 */ 
gint gui_open_conversation_event(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer userdata);

/**
 * Evento che permette di aprire la finestra relativa all'about.
 */
gint gui_open_about_event(GtkWidget *widget, gpointer gdata);

/**
 * Setta lo stato dell'utente su Online e invoca il controller_change_status
 * in modo che quest'ultimo possa inviare un pacchetto di PING con il nuovo status
 */
gint gui_set_to_online_event(GtkWidget *widget, gpointer gdata);

/**
 * Setta lo stato dell'utente su Busy e invoca il controller_change_status
 * in modo che quest'ultimo possa inviare un pacchetto di PING con il nuovo status
 */
gint gui_set_to_busy_event(GtkWidget *widget, gpointer gdata);

/**
 * Setta lo stato dell'utente su Away e invoca il controller_change_status
 * in modo che quest'ultimo possa inviare un pacchetto di PING con il nuovo status
 */
gint gui_set_to_away_event(GtkWidget *widget, gpointer gdata);

/**
 * Evento che si scatena alla pressione del bottone search, effettua un clear della
 * lista delle chat e invoca il controller_search in modo che quest'ultimo possa 
 * inviare il pacchetto di SEARCH. 
 */
gint gui_search_chat_event(GtkWidget *widget, gpointer gdata);

/**
 * Prepara il corpo del messaggio da inviare a tutta la chat o a un sottoinsieme di
 * questa, dopo di che invoca il controller affinche' questo gestisca l'invio del 
 * messaggio e infine aggiorna la text view locale col nuovo messaggio.
 */
gint gui_send_text_message_event(GtkWidget *widget, GdkEventKey *event, gpointer gdata);

/**
 * Come la send_text_message_event, ma rivolta all'invio di messaggi privati
 */
gint gui_send_pm_message_event(GtkWidget *widget, GdkEventKey *event, gpointer gdata);

/**
 * Crea il componente grafico che contiene la lista degli utenti connessi ad una chat.
 */
GtkWidget *gui_create_users_list(u_int8 index);

/**
 * Crea il componente grafico che contiene la lista delle chat trovate.
 */ 
GtkWidget *gui_create_chat_list(u_int8 index);

/**
 * Crea la barra dei menu situata su ogni finestra. (File -- Status -- Help).
 */
GtkWidget *gui_create_menu(void);

/**
 * Crea le varie text view (di scrittura e lettura) per la chat e per il pm, 
 * impostando i valori in modo opportuno.
 */
GtkWidget *gui_create_text(u_int8 chat_id, int type, int msg_type);

/**
 * Crea il componente grafico presente nella finestra iniziale in cui si puo' 
 * effettuare la creazione o la ricerca di una chat.
 */
GtkWidget *gui_create_searchbar(void);

/**
 * Evento che si scatena alla pressione del bottone create, fa' una semplice 
 * chiamata alla controller_create.
 */
gint gui_create_chat_button(GtkWidget *widget, gpointer gdata);

/**
 * Crea la finestra relativa ad una singola chat e assegna ai componenti gli 
 * eventi appropriati
 */
int gui_open_chatroom(u_int8);

/**
 * Crea la finestra per una conversazione privata
 */
int gui_open_pm(u_int8 user_id, gchar *nickname);

/**
 * Controlla nell'hashtable la presenza o meno del tree_model corrispondente alla 
 * chat.
 */
tree_model *gui_get_tree_model(u_int8 chat_id);

/**
 * Aggiunge alla text view della chat il nuovo messaggio.
 */
int gui_add_msg_to_chat(u_int8 chat_id, char *msg);

/**
 * Aggiunge alla text view della conversazione privata il nuovo messaggio.
 */
int gui_add_msg_pm(u_int8 sender_id, char *msg);

/**
 * Controlla nell'hashtable la presenza o meno della struct pm_data relativa 
 * all'id dell'utente.
 */
pm_data *gui_pm_data_get(u_int8 id);

/**
 * Recupera dal tree_model associato alla gui della chat la lista degli id degli
 * utenti connessi.
 */
GList *gui_get_chat_users(u_int8 chat_id);

#endif //!GUI_H
