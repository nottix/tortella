/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licen
 as published by
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
 
#include "routemanager.h"

/*
 * Aggiunge una regola di routing alla tabella di routing. Se la regola è già presente
 * incrementa il contatore associato alla regola.
 */
int add_route_entry(u_int8 packet_id, u_int8 sender_id, u_int8 recv_id) {
	if(route_hashtable==NULL) { //Alloca l'hashtable solo la prima volta
		route_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	}

	route_entry *entry;
	char *key = to_string(packet_id);
	if((entry=get_route_entry(packet_id))!=NULL) { //Incrementa il contatore se la regola è già presente
		entry->counter++;
	}
	else { //Aggiunge una nuova regola
		entry = (route_entry*)calloc(sizeof(route_entry), 1);
		entry->sender_id = sender_id;
		entry->recv_id = recv_id;
		entry->counter = 0;
		g_hash_table_insert(route_hashtable, (gpointer)key, (gpointer)entry);
	}
	
	return 1;
}

/*
 * Decrementa il contatore della regola. Elimina la regola se il contatore è 0
 */
int del_route_entry(u_int8 id) {
	if(route_hashtable==NULL) {
		route_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	}
	
	route_entry *entry;
	char *key = to_string(id);
	if((entry=get_route_entry(id))!=NULL) { //Decrementa la regola di routing
		entry->counter--;
		if(entry->counter==0)
			g_hash_table_remove(route_hashtable, (gconstpointer)key);
		return 0;
	}
	
	return -1;
}

/*
 * Ritorna una la regola di routing associata all'id del pacchetto
 */
route_entry *get_route_entry(u_int8 packet_id) {
	if(route_hashtable==NULL) {
		route_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	}
	
	char *key = to_string(packet_id);
	route_entry *entry = (route_entry*)g_hash_table_lookup(route_hashtable, (gconstpointer)key);
	return entry;
}

/*
 * Ritorna l'id destinazione associato all'id del pacchetto
 */
u_int8 get_iddest_route_entry(u_int8 id) {
	
	return ((route_entry*)get_route_entry(id))->sender_id;
}
