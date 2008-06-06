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

#include <string.h> 
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
int write_to_file(const char *filename, chat *chat_str, u_int4 mode) {
	if(chat_str==NULL || filename==NULL || strcmp(filename, "")==0)
		return -1;
	
	int fd;
	if(mode==MODE_TRUNC) {
		
		pthread_mutex_lock(&chat_str->mutex);
		
		if((fd=open(filename, O_TRUNC|O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0)
			return 0;
		
		GList *listclient;
		chatclient *chatclient_str;
		char buffer[2048];
		char bufferclient[1024];
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
			return -1;
		
		close(fd);
		
		pthread_mutex_unlock(&chat_str->mutex);
		
		return len;
	}
	
	return 0;
}

int write_all(u_int4 mode) {
	
	if(mode==MODE_TRUNC) {
		GList *list = g_hash_table_get_values(chat_hashtable);
		printf("[write_all]size list: %d\n", g_list_length(list));
		chat *chat_str;
		int i;
		for(i=0; i<g_list_length(list); i++) {
			chat_str = (chat*)g_list_nth_data(list, i);
			logger(SYS_INFO, "[write_all]Writing file: %lld\n", chat_str->id);
			char *path = calloc(strlen(to_string(chat_str->id))+strlen(conf_get_datadir())+2, 1);
			strcpy(path, conf_get_datadir());
			strcat(path, "/");
			strcat(path, to_string(chat_str->id));
			write_to_file(path, chat_str, mode);
		}
	}
	
	return 0;
}

/*
 * Legge le informazioni delle chat e degli utenti dal file specificato.
 * Aggiunge i dati sulla chat alla hashtable relativa, inoltre i dati degli utenti
 * alla hashtable relativa.
 */
int read_from_file(const char *filename) {
	char *saveptr;
	if(filename==NULL || strcmp(filename, "")==0) {
		printf("[read_from_file]Errore filename\n");
		return -1;
	}
	
	int fd;
	if((fd=open(filename, O_RDONLY|O_EXCL))<0) {
		printf("[read_from_file]Errore apertura file\n");
		return -2;
	}
	
	char ch;
	char line[256];
	char *buf;
	int line_count=0;
	int index=0;
	
	char *chat_id=NULL;
	char *title;
	char *id;
	char *nick;
	char *ip;
	char *port;
	while(read(fd, &ch, 1)>0) {
		if(ch=='\n') {
			if(line_count==0) {
				buf = strdup(line);
				chat_id = strtok_r(buf, ";",&saveptr);
				title = strtok_r(NULL, ";",&saveptr);
				add_chat(strtoull(chat_id, NULL, 10), strdup(title));
				memset(line, 0, 256);;
				index=0;
			}
			else if(line_count>0) {
				buf = strdup(line);
				id = strtok_r(buf, ";",&saveptr);
				nick = strtok_r(NULL, ";",&saveptr);
				ip = strtok_r(NULL, ";",&saveptr);
				port = strtok_r(NULL, ";",&saveptr);
				add_user_to_chat(strtoull(chat_id, NULL, 10), strtoull(id, NULL, 10), strdup(nick), strdup(ip), strtod(port, NULL));
				memset(line, 0, 256);
				index=0;
			}
			line_count++;
		}
		else {
			line[index++]=ch;
		}
	}
	
	return 0;
}

int read_all(void) {
	DIR *dir=opendir(conf_get_datadir());
	struct dirent *ent;
	char buf[100];
	GList *dir_list = NULL;
	while (0!=(ent=readdir(dir))) {
		printf("[read_all]Opening %s\n",ent->d_name);
		if(strcmp(ent->d_name, ".")!=0 && strcmp(ent->d_name, "..")!=0 && strcmp(ent->d_name, ".svn")!=0 && strstr(ent->d_name, "init_data")==NULL) {
			sprintf(buf, "%s/%s", conf_get_datadir(), ent->d_name);
			dir_list = g_list_append(dir_list, (gpointer)strdup(buf));
		}
	}
	closedir(dir);
	
	int i;
	for(i=0; i<g_list_length(dir_list); i++) {
		read_from_file((char*)g_list_nth_data(dir_list, i));
	}
	return 1;
}

int add_chat(u_int8 id, const char *title) {
	if(chat_hashtable==NULL) {
		chat_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	}
	if(g_hash_table_lookup(chat_hashtable, (gpointer)to_string(id))!=NULL) {
		printf("[add_chat]Elemento già presente\n");
		return -1;
	}
	chat *chat_str = (chat*)calloc(1, sizeof(chat));
	chat_str->id = id;
	chat_str->title = (char*)strdup(title);
	chat_str->users = g_hash_table_new(g_str_hash, g_str_equal);
	pthread_mutex_init(&chat_str->mutex, NULL);
	
	logger(SYS_INFO, "[add_chat]Table created\n");
	
	g_hash_table_insert(chat_hashtable, (gpointer)to_string(id), (gpointer)chat_str); //Aggiunta la chat alla hashtable
	
	return 0;
}

int add_all_to_chat(GList *chats) {
	if(chats==NULL) {
		return -1;
	}
	
	int i=0, j=0;
	chat *elem;
	chatclient *user;
	GList *user_list;
	for(; i<g_list_length(chats); i++) {
		elem = (chat*)g_list_nth_data(chats, i);
		add_chat(elem->id, elem->title);
		if(elem->users!=NULL) {
			logger(SYS_INFO, "[add_all_to_chat]Utenti presenti\n");
			user_list = g_hash_table_get_values(elem->users);
			for(j=0; j<g_list_length(user_list); j++) {
				user = (chatclient*)g_list_nth_data(user_list, j);
				add_user_to_chat(elem->id, user->id, user->nick, user->ip, user->port);
			}
		}
	}
	return 0;
}

int del_chat(u_int8 id) {
	if(chat_hashtable==NULL)
		return -1;
	
	g_hash_table_remove(chat_hashtable, (gconstpointer)to_string(id));
	
	return 0;
}

int add_exist_user_to_chat(u_int8 chat_id, u_int8 id) {
	if(chat_hashtable==NULL || chatclient_hashtable==NULL)
		return -1;
	
	chat *chat_elem = (chat*)g_hash_table_lookup(chat_hashtable, (gconstpointer)to_string(chat_id));
	if(chat_elem!=NULL) {
		chatclient *chatclient_elem = (chatclient*)g_hash_table_lookup(chatclient_hashtable, to_string(id));
		if(chatclient_elem!=NULL) {
			g_hash_table_insert(chat_elem->users, (gpointer)to_string(chatclient_elem->id), (gpointer)chatclient_elem);
			return 1;
		}
	}
	return 0;
}

int add_user_to_chat(u_int8 chat_id, u_int8 id, const char *nick, const char *ip, u_int4 port) {
	
	if(chat_hashtable==NULL)
		return -1;
	
	if(chatclient_hashtable==NULL)
		chatclient_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	
	add_user(id, nick, ip, port);
	
	chatclient *chatclient_str = (chatclient*)calloc(1, sizeof(chatclient));
	chatclient_str->id = id;
	chatclient_str->nick = (char*)strdup(nick);
	chatclient_str->ip = (char*)strdup(ip);
	chatclient_str->port = port;
	g_hash_table_insert(chatclient_hashtable, (gpointer)to_string(id), (gpointer)chatclient_str);
	
	chat *chat_str = (chat*)g_hash_table_lookup(chat_hashtable, (gconstpointer)to_string(chat_id));
	if(chat_str==NULL) {
		printf("[add_user_to_chat]chat non presente\n");
		return -1;
	}
	
	pthread_mutex_lock(&chat_str->mutex);
	if(g_hash_table_lookup(chat_str->users, (gpointer)to_string(id))==NULL) {
		g_hash_table_insert(chat_str->users, (gpointer)to_string(id), (gpointer)chatclient_str);
	}
	printf("[add_user_to_chat]Inserted\n");
	pthread_mutex_unlock(&chat_str->mutex);
	
	return 0;
}

int add_user(u_int8 id, const char *nick, const char *ip, u_int4 port) {
	
	if(chatclient_hashtable==NULL)
		chatclient_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	
	chatclient *chatclient_str = (chatclient*)malloc(sizeof(chatclient));
	chatclient_str->id = id;
	chatclient_str->nick = (char*)nick;
	chatclient_str->ip = (char*)ip;
	chatclient_str->port = port;
	g_hash_table_insert(chatclient_hashtable, (gpointer)to_string(id), (gpointer)chatclient_str);
	
	return 0;
}

int del_user(u_int8 id) {
	if(chatclient_hashtable==NULL)
		return -1;
	
	g_hash_table_remove(chatclient_hashtable, (gconstpointer)to_string(id));
	
	return 0;
}

int del_user_from_chat(u_int8 chat_id, u_int8 id) {
	if(chat_hashtable==NULL || chatclient_hashtable==NULL)
		return -1;
	
	g_hash_table_remove(chatclient_hashtable, (gconstpointer)to_string(id));
	chat *chat_str = (chat*)g_hash_table_lookup(chat_hashtable, (gconstpointer)to_string(chat_id));
	g_hash_table_remove(chat_str->users, (gconstpointer)to_string(id));

	return 0;
}

chat *search_chat_local(const char *title) {
	return search_chat(title);
}

chat *search_chat(const char *title) {
	if(title==NULL || chat_hashtable==NULL)
		return NULL;
	
	GList *listchat = g_hash_table_get_values(chat_hashtable);
	chat *chatval;
	int j;
	for(j=0; j<g_list_length(listchat); j++) {
		chatval = (chat*)g_list_nth_data(listchat, j);
		if(strcmp(chatval->title, title)==0)
			return chatval;
	}
	return NULL;
}

GList *search_all_chat(const char *title) {
	if(title==NULL) {
		printf("[search_all_chat]title NULL\n");
		return NULL;
	}
	if(chat_hashtable==NULL) {
		printf("[search_all_chat]chat_hashtable NULL\n");
		return NULL;
	}
	
	GList *listallchat=NULL;
	GList *listchat = g_hash_table_get_values(chat_hashtable);
	if(listchat==NULL) {
		printf("[search_all_chat]null list\n");
		return NULL;
	}
	chat *chatval;
	int j;
	for(j=0; j<g_list_length(listchat); j++) {
		chatval = (chat*)g_list_nth_data(listchat, j);
		if(strstr(chatval->title, title)!=NULL) {
			printf("[search_all_chat]found\n");
			listallchat = g_list_prepend(listallchat, (gpointer)chatval);
		}
	}
	return listallchat;
}

GList *search_all_local_chat(const char *title) {
	return search_all_chat(title);
}

chatclient *search_chatclient(const char *nick) {
	if(nick==NULL || chatclient_hashtable==NULL) {
		return NULL;
	}
	
	GList *listclient = g_hash_table_get_values(chatclient_hashtable);
	chatclient *clientval;
	int j;
	for(j=0; j<g_list_length(listclient); j++) {
		clientval = (chatclient*)g_list_nth_data(listclient, j);
		if(strcmp(clientval->nick, nick)==0)
			return clientval;
	}
	return NULL;
}

/*
 * Converte la lista di chat in una stringa del tipo:
 * 111;test;
 * 22;simone;127.0.0.1;2110;
 * 33;simon;127.0.0.1;2110;
 */
char *chatlist_to_char(GList *chat_list, int *len) {
	if(chat_list==NULL) {
		printf("[chatlist_to_char]chat_list NULL\n");
		return NULL;
	}
	if(g_list_length(chat_list)==0)
		return NULL;
	
	chat *chat_elem;
	chatclient *chatclient_elem;
	int cur_size = 512;
	int cur = 0;
	char *ret = (char*)calloc(cur_size, 1);
	char *line = (char*)calloc(512, 1);
	int i, j;
	GList *chatclient_list;
	for(i=0; i<g_list_length(chat_list); i++) {
		chat_elem = (chat*)g_list_nth_data(chat_list, i);
		sprintf(line, "%lld;%s;\n", chat_elem->id, chat_elem->title);
		cur += strlen(line);
		if(cur>=cur_size) {
			cur_size *= 2;
			ret = realloc(ret, cur_size);
		}
		strcat(ret, line);
		
		chatclient_list = g_hash_table_get_values(chat_elem->users);
		for(j=0; j<g_list_length(chatclient_list); j++) {
			chatclient_elem = (chatclient*)g_list_nth_data(chatclient_list, j);
			sprintf(line, "%lld;%s;%s;%d;\n", chatclient_elem->id, chatclient_elem->nick, chatclient_elem->ip, chatclient_elem->port);
			cur += strlen(line);
			if(cur>=cur_size) {
				cur_size *= 2;
				ret = realloc(ret, cur_size);
			}
			strcat(ret, line);
		}
		
		cur++;
		if(cur>=cur_size) {
				cur_size *= 2;
				ret = realloc(ret, cur_size);
		}
		strcat(ret, "|");
	}
	ret = realloc(ret, cur);
	*len = cur;
	
	return ret;
}

/*
 * Converte una stringa in una lista di chat con i relativi utenti
 * 111;test;
 * 22;simone;127.0.0.1;2110;
 * 33;simon;127.0.0.1;2110;
 * |222;test;
 * 333;si.......
 */
GList *char_to_chatlist(const char *buffer, int len) {
	char *saveptr, *saveptr2;
	char *buffer2 = strdup(buffer);
	char *token;
	int i=0;
	int line=-1;
	
	GList *chat_list=NULL;	
	while((token = strtok_r(buffer2, "|", &saveptr))!=NULL) {
		line = -1;
		for(i=0;i<strlen(token);i++) {
			if(token[i]=='\n')
				line++;
		}
		if(line>-1) {
			chat *chat_elem=(chat *)calloc(1, sizeof(chat));
			chat_elem->id= atoll(strtok_r(strdup(token), ";", &saveptr2));
			chat_elem->title=strdup(strtok_r(NULL, ";", &saveptr2)); 
			chat_elem->users=g_hash_table_new(g_str_hash, g_str_equal);
			for(i=0;i<line;i++) {
				chatclient *chat_client=(chatclient *)calloc(1,sizeof(chatclient));	
				chat_client->id=atoll(strtok_r(NULL, ";", &saveptr2));
				chat_client->nick=strdup(strtok_r(NULL, ";", &saveptr2));
				chat_client->ip=strdup(strtok_r(NULL, ";", &saveptr2));
				chat_client->port= atoi(strtok_r(NULL, ";", &saveptr2));
				g_hash_table_insert(chat_elem->users,(gpointer)to_string(chat_client->id),(gpointer)chat_client);
			}
			chat_list=g_list_append(chat_list,(gpointer)chat_elem);	 
			buffer2 = NULL;
			line = 0;
		}
	}
	return chat_list;
}

/*
 * Converte la lista di utenti in una stringa del tipo:
 * 22;simone;127.0.0.1;2110;
 * 33;simon;127.0.0.1;2110;
 */
char *userlist_to_char(GList *user_list, int *len) {
		if(chat_list==NULL) {
		printf("[userlist_to_char]user_list NULL\n");
		return NULL;
	}
	if(g_list_length(user_list)==0)
		return NULL;
	
	chatclient *chatclient_elem;
	int cur_size = 512;
	int cur = 0;
	char *ret = (char*)calloc(cur_size, 1);
	char *line = (char*)calloc(512, 1);
	int i, j;
	for(j=0; j<g_list_length(user_list); j++) {
		chatclient_elem = (chatclient*)g_list_nth_data(user_list, j);
		sprintf(line, "%lld;%s;%s;%d;\n", chatclient_elem->id, chatclient_elem->nick, chatclient_elem->ip, chatclient_elem->port);
		cur += strlen(line);
		if(cur>=cur_size) {
			cur_size *= 2;
			ret = realloc(ret, cur_size);
		}
		strcat(ret, line);
	}
	
	cur++;
	if(cur>=cur_size) {
		cur_size *= 2;
		ret = realloc(ret, cur_size);
	}
	ret = realloc(ret, cur);
	*len = cur;
	
	return ret;
}

/*
 * Converte una stringa in una lista di utenti
 */
GList *char_to_userlist(const char *buffer,int len) {
	char *saveptr, *saveptr2;
	char *buffer2 = strdup(buffer);
	char *token;
	int i=0;
	int line=-1;
	
	GList *user_list = NULL;	
	while((token = strtok_r(buffer2, "\n", &saveptr))!=NULL) {
		chatclient *chat_client=(chatclient *)calloc(1, sizeof(chatclient));	
		chat_client->id=atoll(strtok_r(buffer2, ";", &saveptr2));
		chat_client->nick=strdup(strtok_r(NULL, ";", &saveptr2));
		chat_client->ip=strdup(strtok_r(NULL, ";", &saveptr2));
		chat_client->port= atoi(strtok_r(NULL, ";", &saveptr2));
		g_hash_table_insert(user_list, (gpointer)to_string(chat_client->id),(gpointer)chat_client);
		buffer2 = NULL;
	}
	return user_list;
}

/*
 * Ritorna una lista di tutti i client della chat specificata
 */
GList *get_chatclient_from_chat(const char *title) {
	chat *chatval;
	if((chatval=search_chat(title))!=NULL)
		return g_hash_table_get_values(chatval->users);
	return NULL;
}

chat *get_chat(u_int8 chat_id) {
	return g_hash_table_lookup(chat_hashtable, to_string(chat_id));
}

chatclient *get_chatclient(u_int8 id) {
	return g_hash_table_lookup(chatclient_hashtable, to_string(id));
}
