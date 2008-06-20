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

/**
 * Inizializza il logger, scegliendo come path del file su cui scrivere il valore
 * presente nel file di configurazione. Il parametro verbose_level serve per 
 * specificare fino a quale livello il logger deve salvare le informazioni. Viene
 * inizializzato anche un mutex per evitare accessi simultanei.
 */
int logger_init(int verbose_level) {
 pathname = (char*)malloc(128);
 strcpy(pathname, conf_get_path());
 printf("[logger_init]init log file: %s\n", pathname);
 
 fd_file = fopen(pathname, "a");
 pthread_mutex_init(&logger_mutex, NULL);
 verbose_l = verbose_level;
 return 1;
}

/**
  * Chiude il file su cui il logger stava salvando le informazioni.
  */
int logger_close() {
 return fclose(fd_file);
}

/**
  * Ritorna un timestamp.
  * Esempio: Tue Jun 17 16:26:28 2008
  */
char *get_timestamp() {
 time_t t = time(NULL);
 char *ret = (char*)malloc(128);
 sprintf(ret, "%s  ", asctime(localtime(&t)));
 ret[strlen(ret)-3]=':';
 ret[strlen(ret)-2]=' ';
 ret[strlen(ret)-1]='\0';
 return ret;
}

/**
  * Si comporta come una printf, ma oltre alla stampa a video viene eseguita anche
  * una scrittura su file in base al livello di verbosita'.
  */
int logger(int type, const char* text, ...) {
 va_list ap;
 va_start(ap, text);
 pthread_mutex_lock(&logger_mutex);
 fprintf(fd_file, "<%u>%s", (int)pthread_self(), get_timestamp());
 vfprintf(fd_file, text, ap);
 
 if(type<=verbose_l) {
  printf("<%u>", pthread_self());
  vprintf(text, ap);
 }
 va_end(ap);
 
 pthread_mutex_unlock(&logger_mutex);
 return 0;
}
