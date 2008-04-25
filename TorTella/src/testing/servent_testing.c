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
 
#include "socketmanager.h"
#include "httpmanager.h"
#include "tortellaprotocol.h"
#include "packetmanager.h"
#include "servent.h"
#include "init.h"
#include <pthread.h>
#include <sys/types.h>
#include <sched.h>
#include <time.h>
#include <signal.h>

int main(int argc, char **argv) {

	if(argc<3) {
		printf("Usage: <local_ip> <local_port> [cache_path]\n");
		return 0;
	}	
	
	GList *init_list = NULL;
	if(argc==4) {
		printf("[main]Init read file\n");
		init_list = init_read_file(argv[3]);
	}
	
	servent_start(argv[1], atoi(argv[2]), init_list);

	printf("Exiting\n");
	
	return 0;
}
