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
#include "common.h"
#include "logger.h"

int conf_read(const char *filename);

int conf_save_value(const char *line);

#endif /*CONFMANAGER_H*/
