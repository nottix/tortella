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
 
#include "logger.h"

int logger_init(char *filename) {
	int fd;
	char *pathname = (char*)malloc(strlen(PATH)+strlen(filename));
	strcpy(pathname, PATH);
	strcat(pathname, filename);
	
	fd = open(pathname, O_CREAT|O_APPEND|O_WRONLY, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
	close(fd);
	return fd;
}

char *get_timestamp() {
	time_t t = time(NULL);
	char *ret = (char*)malloc(25);
	sprintf(ret, "%s", asctime(localtime(&t)));
	ret[strlen(ret)-1]=':';
	ret[strlen(ret)]=' ';
	ret[strlen(ret)+1]='\0';
	return ret;
}

int logger_add(char* filename, char *text) {
	int fd;
	char *pathname = (char*)malloc(strlen(PATH)+strlen(filename));
	strcpy(pathname, PATH);
	strcat(pathname, filename);
	
	if((fd=open(pathname, O_RDONLY|O_EXCL))<0) {
		logger_init(filename);
	}
	close(fd);
	
	if((fd=open(pathname, O_APPEND|O_RDWR|O_EXCL))<0) {
		fprintf(stderr, "Errore nell'apertura del file\n");
		return -1;
	}
	
	char *timestamp = get_timestamp();
	char *buffer = (char*)malloc(strlen(text)+strlen(timestamp)+1);
	strcpy(buffer, timestamp);
	strcat(buffer, text);
	int len = strlen(buffer);
	buffer[len] = '\n';
	buffer[len+1] = '\0';
	if(write(fd, buffer, strlen(buffer))<0) {
		fprintf(stderr, "Errore nella scrittura nel file\n");
		return -1;
	}
	close(fd);
	return 0;
}