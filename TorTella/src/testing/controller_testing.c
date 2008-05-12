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
#include "confmanager.h"
#include "controller.h"

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
	
	conf_read("./conf/tortella.conf");
	
	servent_start(argv[1], atoi(argv[2]), init_list);
	
	controller_menu();
	
	kill_all_thread(1);

	printf("Exiting\n");
	
	return 0;
}
