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
 
#ifndef COMMON_H
#define COMMON_H

typedef unsigned char u_int1;
typedef unsigned short u_int2;
typedef unsigned int u_int4;
typedef unsigned long long u_int8;

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

#endif
