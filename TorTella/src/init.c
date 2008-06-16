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
 
#include "init.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

GList *init_read_file(const char *filename) {
	GList *init_list=NULL;
	char buffer;
	int fd=0;
	int nread=0; 
	char *tmp=(char *)calloc(22,1);
	int i=0;	

	if(filename==NULL || strlen(filename)==0) {
		printf("Il nome del file non è corretto o non è presente\n");
		return NULL;
	}
	
	if((fd=open(filename, O_RDONLY|O_EXCL))<0){
		printf("Errore apertura file\n");
		return NULL;	
	}
	
	while((nread=read(fd,&buffer,1))>0){
		tmp[i]=buffer;
		
		if(tmp[i]=='\n'){
			init_list=g_list_append(init_list,(gpointer)init_char_to_initdata(tmp));
			memset(tmp,0,strlen(tmp));
			i=0;
		}
		else
			i++;
	}
	
	if(close(fd)<0){
		printf("Errore chiusura\n");
		return NULL;
	}		
	return init_list;
}

init_data *init_char_to_initdata(char *buffer){
	
	char *ip;
	char *port;
	char *saveptr;
	init_data *data=calloc(1,sizeof(init_data));	
	
	ip=strtok_r(buffer,";",&saveptr);
	data->ip=strdup(ip);
	printf("data->ip:%s\n",data->ip);
	port=strtok_r(NULL,";",&saveptr); //FIXIT da mettere ;
	data->port=atoi(port);
	printf("data->port:%d\n",data->port);
		
	return data;
}
