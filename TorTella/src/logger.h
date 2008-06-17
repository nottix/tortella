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
 
#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include "common.h"
#include "confmanager.h"

/**
 * Definizione dei livelli del logger.
 */
#define ALARM_INFO		0
#define INFO			1
#define CTRL_INFO		2
#define SYS_INFO		3
#define PAC_INFO		4
#define HTTP_INFO		5
#define TORTELLA_INFO	6
#define SOCK_INFO		7

static char *pathname;

static FILE *fd_file;

static pthread_mutex_t logger_mutex;

static int verbose_l = 0;

/**
 * Inizializza il logger, scegliendo come path del file su cui scrivere il valore
 * presente nel file di configurazione. Il parametro verbose_level serve per 
 * specificare fino a quale livello il logger deve salvare le informazioni. Viene
 * inizializzato anche un mutex per evitare accessi simultanei.
 */
int logger_init(int verbose_level);

/**
 * Chiude il file su cui il logger stava salvando le informazioni.
 */
int logger_close();

/**
 * Ritorna un timestamp.
 * Esempio: Tue Jun 17 16:26:28 2008
 */
char *get_timestamp();

/**
 * Si comporta come una printf, ma oltre alla stampa a video viene eseguita anche
 * una scrittura su file in base al livello di verbosità.
 */
int logger(int type, const char* text, ...);

#endif //LOGGER_H
