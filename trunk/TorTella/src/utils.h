#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#include "confmanager.h"
#include "common.h"

//#define GEN_START 100000

u_int8 generate_id(void);

int generate_id4(void);

char *to_string(u_int8 num);

char *prepare_msg(time_t timestamp, const char *nick, char *msg, int msg_len);

//char *to_string(u_int8 num1, u_int8 num2);

char *hex_dump(const char *packet, int len, int n);

char *dump_data(const char *packet, int len);

char *get_mac_addr(void);

#endif //UTILS_H
