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
	char *pathname = (char*)malloc(strlen(path)+strlen(filename));
	strcpy(pathname, conf_get_path());
	strcat(pathname, filename);
	printf("[logger_init]init\n");

	fd_file = fopen(pathname, "a");
	pthread_mutex_init(&logger_mutex, NULL);
	verbose = verbose_level;
	return 1;
}

int logger_close() {
	return fclose(fd_file);
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
	va_list ap;
	va_start(ap, text);
	pthread_mutex_lock(&logger_mutex);
	fprintf(fd_file, "%s", get_timestamp());
	vfprintf(fd_file, text, ap);
	pthread_mutex_unlock(&logger_mutex);
	
	if(type<=verbose)
		vprintf(text, ap);
	va_end(ap);

	return 0;
}
