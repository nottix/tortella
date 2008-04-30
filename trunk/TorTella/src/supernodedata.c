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
	printf("[read_from_file]opening filename: %s\n", filename);
	char *saveptr;
	if(filename==NULL || strcmp(filename, "")==0) {
		printf("[read_from_file]Errore filename\n");
		return 0;
	}
	
	int fd;
	if((fd=open(filename, O_RDONLY|O_EXCL))<0) {
		printf("[read_from_file]Errore apertura file\n");
		return 0;
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
	//char *token;
	//printf("[read_from_file]Before while\n");
	while(read(fd, &ch, 1)>0) {
		//printf("[read_from_file]char: %c\n", ch);
		if(ch=='\n') {
			if(line_count==0) {
				//printf("[read_from_file]line_count: %d\n", line_count);
				buf = strdup(line);
				chat_id = strtok_r(buf, ";",&saveptr);
				title = strtok_r(NULL, ";",&saveptr);
				printf("[read_from_file]title: %s\n", title);
				add_chat(strtoull(chat_id, NULL, 10), strdup(title), chat_table);
				//printf("[read_from_file]add done\n");
				memset(line, 0, 100);
				//printf("[read_from_file]memset done\n");
				index=0;
			}
			else if(line_count>0) {
				buf = strdup(line);
				id = strtok_r(buf, ";",&saveptr);
				nick = strtok_r(NULL, ";",&saveptr);
				ip = strtok_r(NULL, ";",&saveptr);
				port = strtok_r(NULL, ";",&saveptr);
				printf("[read_from_file]add_user\n");
				add_user_to_chat(strtoull(chat_id, NULL, 10), strtoull(id, NULL, 10), strdup(nick), strdup(ip), strtod(port, NULL), *chat_table, chatclient_table);
				memset(line, 0, 100);
				index=0;
			}
			line_count++;
		}
		else {
			line[index++]=ch;
			//printf("[read_from_file]line: %s\n", line);
		}
	}
	
	return 1;
}

u_int4 read_all(GHashTable **chat_table, GHashTable **chatclient_table) {
	DIR *dir=opendir(DATADIR);
	struct dirent *ent;
	char buf[100];
	GList *dir_list = NULL;
	while (0!=(ent=readdir(dir))) {
		printf("[read_all]Opening %s\n",ent->d_name);
		if(strcmp(ent->d_name, ".")!=0 && strcmp(ent->d_name, "..")!=0 && strcmp(ent->d_name, ".svn")!=0 && strstr(ent->d_name, "init_data")==NULL) {
			sprintf(buf, "%s/%s", DATADIR, ent->d_name);
			dir_list = g_list_append(dir_list, (gpointer)strdup(buf));
		}
	}
	closedir(dir);
	
	int i;
	for(i=0; i<g_list_length(dir_list); i++) {
		printf("[read_all]read from file %d\n", i);
		read_from_file((char*)g_list_nth_data(dir_list, i), chat_table, chatclient_table);
	}
	return 1;
}

u_int4 add_chat(u_int8 id, const char *title, GHashTable **chat_table) {
	if((*chat_table)==NULL) {
		(*chat_table) = g_hash_table_new(g_str_hash, g_str_equal);
	}
	if(g_hash_table_lookup(*chat_table, (gpointer)to_string(id))!=NULL) {
		printf("[add_chat]Elemento già presente\n");
		return 0;
	}
	chat *chat_str = (chat*)malloc(sizeof(chat));
	chat_str->id = id;
	chat_str->title = (char*)title;
	chat_str->users = g_hash_table_new(g_str_hash, g_str_equal);
	pthread_mutex_init(&chat_str->mutex, NULL);
	
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

//FIXIT: da togliere il doppio inserimento nella lista degli utenti
u_int4 add_user_to_chat(u_int8 chat_id, u_int8 id, const char *nick, const char *ip, u_int4 port, GHashTable *chat_table, GHashTable **chatclient_table) {
	
	if(chat_table==NULL)
		return 0;
	
	if((*chatclient_table)==NULL)
		(*chatclient_table) = g_hash_table_new(g_str_hash, g_str_equal);
	
	add_user(id, nick, ip, port, chatclient_table);
	
	printf("[add_user_to_chat]Init\n");
	chatclient *chatclient_str = (chatclient*)malloc(sizeof(chatclient));
	chatclient_str->id = id;
	chatclient_str->nick = (char*)nick;
	chatclient_str->ip = (char*)ip;
	chatclient_str->port = port;
	printf("[add_user_to_chat] ID: %lld\n", chatclient_str->id);
	g_hash_table_insert((*chatclient_table), (gpointer)to_string(id), (gpointer)chatclient_str);
	printf("[add_user_to_chat] ID: %lld\n", chatclient_str->id);
	
	chat *chat_str = (chat*)g_hash_table_lookup(chat_table, (gconstpointer)to_string(chat_id));
	printf("[add_user_to_chat] ID: %lld\n", chatclient_str->id);
	if(chat_str==NULL) {
		printf("[add_user_to_chat]chat non presente\n");
		return 0;
	}
	printf("[add_user_to_chat] ID: %lld\n", chatclient_str->id);
	printf("[add_user_to_chat]Chat presente\n");
	//pthread_mutex_lock(&chat_str->mutex);
	printf("[add_user_to_chat]Locked\n");
	printf("[add_user_to_chat] ID: %lld\n", chatclient_str->id);
	/*if(chat_str->users==NULL) {
		printf("[add_user_to_chat]Creating hashtable\n");
		chat_str->users = g_hash_table_new(g_str_hash, g_str_equal);
	}*/
	printf("[add_user_to_chat] ID: %lld\n", chatclient_str->id);
	/*if(chat_str->users==NULL) {
		printf("[add_user_to_chat]chat NULL\n");
	}*/
	printf("[add_user_to_chat]Created table %s, %lld.\n", to_string(id), chatclient_str->id);
	if(g_hash_table_lookup(chat_str->users, (gpointer)to_string(id))==NULL) {
		printf("[add_user_to_chat]non è presente\n");
		g_hash_table_insert(chat_str->users, (gpointer)to_string(id), (gpointer)chatclient_str);
	}
	printf("[add_user_to_chat]Inserted\n");
	//pthread_mutex_unlock(&chat_str->mutex);
	
	printf("[add_user_to_chat]End\n");
	
	return 1;
}

u_int4 add_user(u_int8 id, const char *nick, const char *ip, u_int4 port, GHashTable **chatclient_table) {
	
	if((*chatclient_table)==NULL)
		(*chatclient_table) = g_hash_table_new(g_str_hash, g_str_equal);
	
	chatclient *chatclient_str = (chatclient*)malloc(sizeof(chatclient));
	chatclient_str->id = id;
	chatclient_str->nick = (char*)nick;
	chatclient_str->ip = (char*)ip;
	chatclient_str->port = port;
	g_hash_table_insert((*chatclient_table), (gpointer)to_string(id), (gpointer)chatclient_str);
	
	return 1;
}

u_int4 del_user(u_int8 id, GHashTable *chatclient_table) {
	if(chatclient_table==NULL)
		return 0;
	
	g_hash_table_remove(chatclient_table, (gconstpointer)to_string(id));
	
	return 1;
}

u_int4 del_user_from_chat(u_int8 chat_id, u_int8 id, GHashTable *chat_table, GHashTable *chatclient_table) {
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
	
	GList *listchat = g_hash_table_get_values(chat_table);
	chat *chatval;
	int j;
	for(j=0; j<g_list_length(listchat); j++) {
		chatval = (chat*)g_list_nth_data(listchat, j);
		if(strcmp(chatval->title, title)==0)
			return chatval;
	}
	return NULL;
}

GList *search_all_chat(const char *title, GHashTable *chat_table) {
	if(title==NULL || chat_table==NULL) {
		printf("[search_all_chat]title e chat_table NULL\n");
		return NULL;
	}
	
	//printf("[search_all_chat]Title to search: %s\n", title);
	GList *listallchat=NULL;
	GList *listchat = g_hash_table_get_values(chat_table);
	if(listchat==NULL) {
		printf("[search_all_chat]null list\n");
		return NULL;
	}
	chat *chatval;
	int j;
	//printf("[search_all_chat]before for\n");
	for(j=0; j<g_list_length(listchat); j++) {
		chatval = (chat*)g_list_nth_data(listchat, j);
		printf("[search_all_chat]searching %s in %s\n", title, chatval->title);
		if(strstr(chatval->title, title)!=NULL) {
			printf("[search_all_chat]found\n");
			listallchat = g_list_prepend(listallchat, (gpointer)chatval);
		}
	}
	return listallchat;
}

chatclient *search_chatclient(const char *nick, GHashTable *chatclient_table) {
	if(nick==NULL || chatclient_table==NULL) {
		return NULL;
	}
	
	GList *listclient = g_hash_table_get_values(chatclient_table);
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
	chat *chat_elem;
	chatclient *chatclient_elem;
	int cur_size = 512;
	int cur = 0;
	char *ret = (char*)calloc(cur_size, 1);
	char *line = (char*)calloc(512, 1);
	int i, j;
	GList *chatclient_list;
	printf("[chatlist_to_char]Init\n");
	for(i=0; i<g_list_length(chat_list); i++) {
		chat_elem = (chat*)g_list_nth_data(chat_list, i);
		printf("[chatlist_to_char]chat: %lld, %s.\n", chat_elem->id, chat_elem->title);
		sprintf(line, "%lld;%s\n", chat_elem->id, chat_elem->title);
		printf("[chatlist_to_char]chat len: %d\n", strlen(line));
		cur += strlen(line);
		if(cur>=cur_size) {
			cur_size *= 2;
			ret = realloc(ret, cur_size);
		}
		strcat(ret, line);
		
		chatclient_list = g_hash_table_get_values(chat_elem->users);
		for(j=0; j<g_list_length(chatclient_list); j++) {
			chatclient_elem = (chatclient*)g_list_nth_data(chatclient_list, j);
			printf("[chatlist_to_char]chatclient: %s\n", chatclient_elem->nick);
			sprintf(line, "%lld;%s;%s;%d\n", chatclient_elem->id, chatclient_elem->nick, chatclient_elem->ip, chatclient_elem->port);
			cur += strlen(line);
			printf("[chatlist_to_char]cur: %d\n", cur);
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
		strcat(ret, "\n");
	}
	ret = realloc(ret, cur);
	*len = cur;
	printf("[chatlist_to_char] strlen(ret): %d, len: %d\n", strlen(ret), *len);
	
	return ret;
}

/*
 * Converte una stringa in una lista di chat con i relativi utenti
 * 111;test;
 * 22;simone;127.0.0.1;2110;
 * 33;simon;127.0.0.1;2110;
 */
GList *char_to_chatlist(const char *buffer,int len) {
	/*printf("[char_to_chatlist]buffer: %s\n", buffer);
	
	char *saveptr, *saveptr2;
	GList *chat_list=NULL;
	chat *chat_elem=NULL;
	
	char *tmp=calloc(len,1);
	
	char *token, *token2;
	int i=0;
	int r=0;	
	int line=0;
    int count = 0;
	
	for(i=0;i<len;i++){
	
		if()
		
		else if(tmp[i]=='\n'){
			count++;
			chat_elem->id==atoll(tmp,";",&saveptr);
			chat_elem->title=strdup(NULL,";",&saveptr);
		}
		
		tmp[i]=buffer[i];
	}*/
	
	

 char *saveptr, *saveptr2;
 char *token, *token2;
 char *tmp = calloc(1,len);
 int i=0;
 int line=-1;
 int count = 0;
 //char * tmp=buffer;
  

 GList *chat_list=NULL;	
 
	
 while(token = strtok_r(buffer,"|",&saveptr)!=NULL)
 {

   for(i=0;i<strlen(token);i++)
   {
     if(token[i]=='\n')
       line++;
   } 
   chat *chat_elem=(char *)calloc(1,sizeof(chat)); 		 
   chat_elem->id= atoll(strtok_r(token,';',&saveptr2));
   chat_elem->title=strdup(strtok_r(NULL;';',&saveptr2)); 
   chat_elem->users=g_hash_table_new(g_str_hash, g_str_equal);
   for(i=0;i<line;i++)
   {
	 chatclient *chat_client=(chatclient *)calloc(1,sizeof(chatclient));	
	 chat_client->id=atoll(strtok_r(NULL,';'&saveptr2));
     chat_client->nick=strdup(strtok_r(NULL,';'&saveptr2));
     chat_client->ip=strdup(strtok_r(NULL,';'&saveptr2));
     chat_client->port= strdup(strtok_r(NULL,';'&saveptr2));
     g_hash	_table_insert(users,(gpointer)to_string(chat_client->id),(gpointer)chat_client);
	   
   }
   chat_list=g_list_append(chat_list,(gpointer)chat_elem);	 
   buffer = NULL;
   line = 0;
 }
 
 

	/*do {
		
		chat_elem=(chat *)calloc(1, sizeof(chat));
		chat_elem->users= g_hash_table_new(g_str_hash, g_str_equal);
		token2 = strtok_r(tmp2, "\n", &saveptr2);
		chat_elem->id=atoll(token);
		token2 = strtok_r(NULL,";",&saveptr2);
		chat_elem->title=strdup(token2);
		chat_list=g_list_append(chat_list, chat_elem);
		
		
		
		//printf("[buffer[%d]: %c\n", i, buffer[i]);
		if (buffer[i]=='\0')
			return chat_list;
		if(buffer[i]=='\n' && count==1){
			chat_list=g_list_append(chat_list, chat_elem);
			line=0;
			count = 0;
			continue;
		}
		//count = 0;
		if(buffer[i]=='\n' && line>0) {
			count =1;
			chatclient *chat_client=(chatclient *)calloc(1, sizeof(chatclient));			
			r=0;
			token=strtok_r(tmp,";",&saveptr);
			chat_client->id=atoll(token);
			token=strtok_r(NULL,";",&saveptr);					
			chat_client->nick=strdup(token);
			token=strtok_r(NULL,";",&saveptr);
			chat_client->ip=strdup(token);
			token=strtok_r(NULL,";",&saveptr);
			chat_client->port=atoi(token);
			
			//printf("[char_to_chatlist]buffer[%d]: %c, line: %d\n", i, buffer[i], line);
			g_hash_table_insert(chat_elem->users,(gpointer)to_string(chat_client->id),(gpointer)chat_client);
			
			memset(tmp,0,strlen(tmp));
			line++;
		}
		else if(buffer[i]== '\n' && line==0) {
			count = 1;
			chat_elem=(chat *)calloc(1, sizeof(chat));
			chat_elem->users= g_hash_table_new(g_str_hash, g_str_equal);
			r=0;
			line++;
			token=strtok_r(tmp,";",&saveptr);
			
			chat_elem->id=atoll(token);
			token=strtok_r(NULL,";",&saveptr);
			chat_elem->title=strdup(token);
			//printf("[char_to_chatlist]title: %s\n", token);
			chat_list=g_list_append(chat_list, chat_elem);
			memset(tmp,0,strlen(tmp));
		}
		tmp[r]=buffer[i];
		r++;
	}while(strtok_r(NULL,"\n\n",&saveptr));

	return chat_list;
 */
}

/*
 * Ritorna una lista di tutti i client della chat specificata
 */
GList *get_chatclient(const char *title, GHashTable *chat_table) {
	chat *chatval;
	if((chatval=search_chat(title, chat_table))!=NULL)
		return g_hash_table_get_values(chatval->users);
	return NULL;
}
