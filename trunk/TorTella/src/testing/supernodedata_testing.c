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
 
#include "supernodedata.h"

int main(void) {
	/*add_chat(111, "test", &chat_hashtable);
	
	chat *test = (chat*)g_hash_table_lookup(chat_hashtable, (gconstpointer)to_string(111));
	
	printf("chat: %lld\n", test->id);
	
	add_user(111, 22, "simone", "127.0.0.1", 2110, chat_hashtable, &chatclient_hashtable);
			 
	chatclient *user = (chatclient*)g_hash_table_lookup(chatclient_hashtable, (gconstpointer)to_string(22));
	
	printf("user: %s\n", user->nick);
	
	user = (chatclient*)g_hash_table_lookup(test->users, (gconstpointer)to_string(22));
	
	printf("userchat: %lld\n", user->id);
	
	write_to_file("test", test, MODE_TRUNC);*/
	
	//read_from_file("111", &chat_hashtable, &chatclient_hashtable);
	read_all(&chat_hashtable, &chatclient_hashtable);
	
	//chatclient *user = (chatclient*)g_hash_table_lookup(chatclient_hashtable, (gconstpointer)to_string(33));
	
	//printf("user: %s\n", user->nick);
	
	int len;
	
	char *buffer = chatlist_to_char(g_hash_table_get_values(chat_hashtable), &len);
	
	printf("buffer:\n%s\n", buffer);
	GList *res = char_to_chatlist(buffer, len);
	printf("chat: \n%s", dump_data(buffer, len));
	
	printf("res: %s\n", ((chat*)g_list_nth_data(res, 0))->title);
	printf("size: %d\n", g_hash_table_size(((chat*)g_list_nth_data(res, 0))->users));
	
	GList *clients = g_hash_table_get_values(((chat*)g_list_nth_data(res, 0))->users);
	
	int i;
	for(i=0; i<g_list_length(clients); i++) {
		printf("port: %d\n", ((chatclient*)g_list_nth_data(clients, i))->port);
	}
	
	return 0;
}
