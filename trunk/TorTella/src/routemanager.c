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
int add_route_entry(u_int8 packet_id, u_int8 sender_id, u_int8 recv_id, GHashTable *route_table) {
	if(route_table==NULL)
		return -1;
	
	/*char *key = to_string(recv_id);
	route_entry *entry = (route_entry*)g_hash_table_lookup(route_table, (gconstpointer)key);
	if(entry!=NULL) {
		entry->counter++;
		entry->sender_id = sender_id;
		g_hash_table_replace(route_table, (gpointer)key, (gpointer)entry);
	}*/

	char *key = to_string(packet_id);
	route_entry *entry = (route_entry*)calloc(sizeof(route_entry), 1);
	entry->sender_id = sender_id;
	entry->recv_id = recv_id;
	g_hash_table_insert(route_table, (gpointer)key, (gpointer)entry);
	
	return 1;
}

int del_route_entry(u_int8 id, GHashTable *route_table) {
	if(route_table==NULL)
		return -1;
	
	char *key = to_string(id);
	g_hash_table_remove(route_table, (gconstpointer)key);
	/*route_entry *entry = (route_entry*)g_hash_table_lookup(route_table, (gconstpointer)key);
	if(entry!=NULL) {
		entry->counter--;
		if(entry->counter==0)
			
		g_hash_table_replace(route_table, (gpointer)key, (gpointer)entry);
	}*/
	
	return 1;
}

route_entry *get_route_entry(u_int8 packet_id, GHashTable *route_table) {
	if(route_table==NULL)
		return NULL;
	
	char *key = to_string(packet_id);
	route_entry *entry = (route_entry*)g_hash_table_lookup(route_table, (gconstpointer)key);
	return entry;
}

u_int8 get_iddest_route_entry(u_int8 id, GHashTable *route_table) {
	
	return ((route_entry*)get_route_entry(id, route_table))->sender_id;
}
