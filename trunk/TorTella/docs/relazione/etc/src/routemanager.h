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

#ifndef ROUTEMANAGER_H
#define ROUTEMANAGER_H

#include "common.h"
#include "packetmanager.h"
#include "utils.h"
#include "datamanager.h"
#include "init.h"
#include "routemanager.h"
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <glib.h>

struct route_entry {
 u_int8 sender_id;
 u_int8 recv_id;
 u_int4 counter;
};
typedef struct route_entry route_entry;

static GHashTable *route_hashtable = NULL;

//!--------------------------------

/**
  * Aggiunge una regola di routing alla tabella di routing. Se la regola e' gia' presente
  * incrementa il contatore associato alla regola.
  */
int add_route_entry(u_int8 packet_id, u_int8 sender_id, u_int8 recv_id);

/**
  * Decrementa il contatore della regola. Elimina la regola se il contatore e' 0
  */
int del_route_entry(u_int8 id);

/**
  * Ritorna una la regola di routing associata all'id del pacchetto
  */
route_entry *get_route_entry(u_int8 packet_id);

/**
  * Ritorna l'id destinazione associato all'id del pacchetto
  */
u_int8 get_iddest_route_entry(u_int8 id);


#endif //!ROUTEMANAGER_H
