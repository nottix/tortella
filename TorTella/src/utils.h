#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

u_int8 generate_id(void);

char *to_string(u_int8 num);

//char *to_string(u_int8 num1, u_int8 num2);

void hex_dump(char *packet, int len, int n);

#endif //UTILS_H
