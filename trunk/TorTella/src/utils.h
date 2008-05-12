#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

//#define GEN_START 100000

u_int8 generate_id(void);

char *to_string(u_int8 num);

//char *to_string(u_int8 num1, u_int8 num2);

char *hex_dump(const char *packet, int len, int n);

char *dump_data(const char *packet, int len);

#endif //UTILS_H
