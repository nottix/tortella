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
//#include "confvars.h"

//#define PATH "/tmp/"

#define ALARM_INFO		0
#define INFO			1
#define CTRL_INFO		2
#define SYS_INFO		3
#define PAC_INFO		4
#define HTTP_INFO		5
#define TORTELLA_INFO	6
#define SOCK_INFO		7


static FILE *fd_file;

static pthread_mutex_t logger_mutex;

static int verbose = 0;

int logger_init(int verbose_level, const char *filename);

int logger_close();

char *get_timestamp();

int logger(int type, const char* text, ...);

#endif //LOGGER_H
