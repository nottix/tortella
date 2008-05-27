#ifndef CONFMANAGER_H
#define CONFMANAGER_H

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
#include "logger.h"
#include "common.h"

//SOCKET
static int qlen = 5; //Coda di servizio per ricezione SYNC
static int buffer_len = 1024; //Lunghezza buffer ricevzione/trasmissione

//PACKET
static char *path = "/tmp/";

//UTILS
static u_int8 gen_start = 100000; //fake IDs range

//SERVENT
static int max_len = 4000;
static int max_thread = 20;
static int max_fd = 100;

//SUPERNODE
static char *datadir = "./data";

//SERVENT
static char *local_ip;
static u_int4 local_port;
static char *nick;

int conf_read(const char *filename);

int conf_save_value(const char *line);

int conf_get_qlen(void);

int conf_get_buffer_len(void);

char *conf_get_path(void);

u_int8 conf_get_gen_start(void);

char *conf_get_datadir(void);

char *conf_get_local_ip(void);

u_int4 conf_get_local_port(void);

char *conf_get_nick(void);

#endif /*CONFMANAGER_H*/
