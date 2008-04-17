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

/*
 * Scrive la struttura dati 'chat' in un file, nel seguente modo:
 * chat_id;chat_title;
 * user_id;user_nick;user_ip;user_port;
 * ...
 *
 * In modalità TRUNC crea ogni volta un nuovo file, mentre
 * nella modalità APPEND modifica il file esistente.
 */
u_int4 write_to_file(const char *filename, chat *chat_str, u_int4 mode) {
	if(chat_str==NULL || filename==NULL || strcmp(filename, "")==0)
		return 0;
	
	int fd;
	if(mode==MODE_TRUNC) {
		
		pthread_mutex_lock(&chat_str->mutex);
		
		if((fd=open(filename, O_TRUNC|O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0)
			return 0;
		
		//GList *list = g_hash_table_get_values(chat_table);
		GList *listclient;
		//chat *chat_str;
		chatclient *chatclient_str;
		char buffer[2000];
		char bufferclient[2000];
		int j;

		sprintf(buffer, "%lld;%s;\n", chat_str->id, chat_str->title);
			
		listclient = g_hash_table_get_values(chat_str->users);
		for(j=0; j<g_list_length(listclient); j++) {
			chatclient_str = (chatclient*)g_list_nth_data(listclient, j);
			sprintf(bufferclient, "%lld;%s;%s;%d;\n", chatclient_str->id, chatclient_str->nick, chatclient_str->ip, chatclient_str->port);
			
			strcat(buffer, bufferclient);
		}
				
		int len=0;
		if((len=write(fd, buffer, strlen(buffer)))<0)
			return 0;
		
		close(fd);
		
		pthread_mutex_unlock(&chat_str->mutex);
		
		return len;
	}
	
	return 1;
}

u_int4 write_all(GHashTable *chat_table, u_int4 mode) {
	
	if(mode==MODE_TRUNC) {
		GList *list = g_hash_table_get_values(chat_table);
		chat *chat_str;
		int i;
		for(i=0; i<g_list_length(list); i++) {
			chat_str = (chat*)g_list_nth_data(list, i);
			write_to_file(to_string(chat_str->id), chat_str, mode);
		}
	}
	
	return 1;
}

/*
 * Legge le informazioni delle chat e degli utenti dal file specificato.
 * Aggiunge i dati sulla chat alla hashtable relativa, inoltre i dati degli utenti
 * alla hashtable relativa.
 */
u_int4 read_from_file(const char *filename, GHashTable **chat_table, GHashTable **chatclient_table) {
	if(filename==NULL || strcmp(filename, "")==0)
		return 0;
	
	int fd;
	if((fd=open(filename, O_RDONLY|O_EXCL))<0)
		return 0;
	
	char ch;
	char line[100];
	char *buf;
	int line_count=0;
	int index=0;
	
	char *chat_id=NULL;
	char *title;
	char *id;
	char *nick;
	char *ip;
	char *port;
	//char *token;
	while(read(fd, &ch, 1)>0) {
		printf("[read_from_file]char: %c\n", ch);
		if(ch=='\n') {
			if(line_count==0) {
				printf("[read_from_file]line_count: %d\n", line_count);
				buf = strdup(line);
				chat_id = strtok(buf, ";");
				title = strtok(NULL, ";");
				printf("[read_from_file]title: %s\n", title);
				add_chat(strtoull(chat_id, NULL, 10), strdup(title), chat_table);
				memset(line, 0, strlen(line));
				index=0;
			}
			else if(line_count>0) {
				buf = strdup(line);
				id = strtok(buf, ";");
				nick = strtok(NULL, ";");
				ip = strtok(NULL, ";");
				port = strtok(NULL, ";");
				add_user(strtoull(chat_id, NULL, 10), strtoull(id, NULL, 10), strdup(nick), strdup(ip), strtod(port, NULL), *chat_table, chatclient_table);
				memset(line, 0, strlen(line));
				index=0;
			}
			line_count++;
		}
		else {
			line[index++]=ch;
			printf("read_from_file]line: %s\n", line);
		}
	}
	
	return 1;
}

u_int4 read_all(GHashTable **chat_table, GHashTable **chatclient_table) {
	DIR *dir=opendir(DATADIR);
	struct dirent *ent;
	char buf[100];
	while (0!=(ent=readdir(dir))) {
		printf("[read_all]Opening %s\n",ent->d_name);
		sprintf(buf, "%s/%s", DATADIR, ent->d_name);
		read_from_file(buf, chat_table, chatclient_table);
	}
	closedir(dir);
	return 1;
}

u_int4 add_chat(u_int8 id, const char *title, GHashTable **chat_table) {
	chat *chat_str = (chat*)malloc(sizeof(chat));
	chat_str->id = id;
	chat_str->title = (char*)title;
	pthread_mutex_init(&chat_str->mutex, NULL);
	
	printf("[add_chat]chat created\n");
	if((*chat_table)==NULL) {
		printf("[add_chat]just creating table\n");
		(*chat_table) = g_hash_table_new(g_str_hash, g_str_equal);
	}
	printf("[add_chat]table created\n");
	
	g_hash_table_insert((*chat_table), (gpointer)to_string(id), (gpointer)chat_str); //Aggiunta la chat alla hashtable
	
	return 1;
}

u_int4 del_chat(u_int8 id, GHashTable *chat_table) {
	if(chat_table==NULL)
		return 0;
	
	g_hash_table_remove(chat_table, (gconstpointer)to_string(id));
	
	return 1;
}

u_int4 add_user(u_int8 chat_id, u_int8 id, const char *nick, const char *ip, u_int4 port, GHashTable *chat_table, GHashTable **chatclient_table) {
	if(chat_table==NULL)
		return 0;
	
	if((*chatclient_table)==NULL)
		(*chatclient_table) = g_hash_table_new(g_str_hash, g_str_equal);
	
	chatclient *chatclient_str = (chatclient*)malloc(sizeof(chatclient));
	chatclient_str->id = id;
	chatclient_str->nick = (char*)nick;
	chatclient_str->ip = (char*)ip;
	chatclient_str->port = port;
	g_hash_table_insert((*chatclient_table), (gpointer)to_string(id), (gpointer)chatclient_str);
	
	chat *chat_str = (chat*)g_hash_table_lookup(chat_table, (gconstpointer)to_string(chat_id));
	pthread_mutex_lock(&chat_str->mutex);
	if(chat_str->users==NULL)
		chat_str->users = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(chat_str->users, (gpointer)to_string(id), (gpointer)chatclient_str);
	pthread_mutex_unlock(&chat_str->mutex);
	
	return 1;
}

u_int4 del_user(u_int8 chat_id, u_int8 id, GHashTable *chat_table, GHashTable *chatclient_table) {
	if(chat_table==NULL || chatclient_table==NULL)
		return 0;
	
	g_hash_table_remove(chatclient_table, (gconstpointer)to_string(id));
	chat *chat_str = (chat*)g_hash_table_lookup(chat_table, (gconstpointer)to_string(chat_id));
	g_hash_table_remove(chat_str->users, (gconstpointer)to_string(id));

	return 1;
}

chat *search_chat(const char *title, GHashTable *chat_table) {
	if(title==NULL || chat_table==NULL)
		return NULL;
	
	GHashTable *listchat = g_hash_table_get_values(chat_table);
	chat *chatval;
	int j;
	for(j=0; j<g_list_length(listchat); j++) {
		chatval = (chat*)g_list_nth_data(listchat, j);
		if(strcmp(chatval->title, title)==0)
			return chatval;
	}
	return NULL;
}

chatclient *search_chatclient(const char *nick, GHashTable *chatclient_table) {
	if(nick==NULL || chatclient_table==NULL) {
		return NULL;
	}
	
	GHashTable *listclient = g_hash_table_get_values(chatclient_table);
	chatclient *clientval;
	int j;
	for(j=0; j<g_list_length(listclient); j++) {
		clientval = (chat*)g_list_nth_data(listclient, j);
		if(strcmp(clientval->nick, nick)==0)
			return clientval;
	}
	return NULL;
	
}