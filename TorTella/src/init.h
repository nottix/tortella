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
 
#ifndef INIT_H
#define INIT_H

#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"


/** dati dei peer inizialmente conosciuti. */
struct init_data {
	char *ip;
	u_int4 port;
};
typedef struct init_data init_data;

/**
 * Legge il file specificato dal parametro filename (che contiene il path) e 
 * e aggiunge tutti i peer presenti all'interno del file in una lista contenente
 * strutture di tipo init_data. 
 * Il file ha la seguente struttura:
 * 127.0.0.1;2110;
 * 127.0.0.1;2120;
 * ...
 */
GList *init_read_file(const char *filename);

/**
 * istanzia la struttura init_data. Viene invocata da init_read_file per aggiungere
 * gli elementi alla lista. Riceve in ingresso il buffer contenente ip e porta
 * del vicino e tokenizza la stringa riempiendo la struttura dati in modo opportuno.
 */
init_data *init_char_to_initdata(char *buffer);

#endif
