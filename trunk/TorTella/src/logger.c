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

int logger_init(int verbose_level, const char *filename) {
	int fd;
	char *pathname = (char*)malloc(strlen(PATH)+strlen(filename));
	strcpy(pathname, PATH);
	strcat(pathname, filename);
	printf("[logger_init]init\n");
	fd = open(pathname, O_CREAT|O_APPEND|O_WRONLY, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
	//close(fd);
	logger_fd = fd;
	pthread_mutex_init(&logger_mutex, NULL);
	verbose = verbose_level;
	return fd;
}

int logger_close() {
	return close(logger_fd);
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

int logger(int type, const char* text, ...) {
	
	char *timestamp = get_timestamp();
	char *buffer = (char*)malloc(6000); //FIXIT
	char *temp = (char*)malloc(4000);

	sprintf(buffer, "%s",timestamp);
	va_list ap;
	va_start(ap, text);
	vsprintf(temp, text, ap);
	va_end(ap);
	
	strcat(buffer, temp);
	int len = strlen(buffer);
	
	pthread_mutex_lock(&logger_mutex);
	if(write(logger_fd, buffer, len)<0) {
		fprintf(stderr, "Errore nella scrittura nel file\n");
		return -1;
	}
	pthread_mutex_unlock(&logger_mutex);
	
	if(type<=verbose)
		printf(buffer);

	return 0;
}

int logger_to(const char* filename, const char *text, ...) {
	int fd;
	char *pathname = (char*)malloc(strlen(PATH)+strlen(filename));
	strcpy(pathname, PATH);
	strcat(pathname, filename);
	
	if((fd=open(pathname, O_RDONLY|O_EXCL))<0) {
		logger_init(8, filename);
	}
	close(fd);
	
	if((fd=open(pathname, O_APPEND|O_RDWR|O_EXCL))<0) {
		fprintf(stderr, "Errore nell'apertura del file\n");
		return -1;
	}
	
	char *timestamp = get_timestamp();
	char *buffer = (char*)malloc(2000); //FIXIT
	char *temp = (char*)malloc(1000);
	//strcpy(buffer, timestamp);
	//strcat(buffer, text);
	sprintf(buffer, "%s",timestamp);
	//printf("[logger_add]text: %s\n", text);
	//printf("[logger_add]%s\n", buffer);
	//buffer[len] = '\n';
	//buffer[len+1] = '\0';
	va_list ap;
	va_start(ap, text);
	vsprintf(temp, text, ap);
	va_end(ap);
	
	strcat(buffer, temp);
	int len = strlen(buffer);
	if(write(fd, buffer, strlen(buffer))<0) {
		fprintf(stderr, "Errore nella scrittura nel file\n");
		return -1;
	}
	close(fd);
	return 0;
}
