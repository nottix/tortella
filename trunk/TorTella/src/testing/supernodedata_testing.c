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
	
	read_from_file("test", &chat_hashtable, &chatclient_hashtable);
	
	chatclient *user = (chatclient*)g_hash_table_lookup(chatclient_hashtable, (gconstpointer)to_string(33));
	
	printf("user: %s\n", user->nick);
	
	return 0;
}
