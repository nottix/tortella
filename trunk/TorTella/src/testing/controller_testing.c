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

	if(argc<2) {
		printf("Usage: <conf_path> [cache_path]\n");
		return 0;
	}	
	
	if(argc==2) {
		controller_init(argv[1], NULL);
	}
	else {
		controller_init(argv[1], argv[2]);
	}
	
	controller_menu();
	
	kill_all_thread(1);

	printf("Exiting\n");
	
	return 0;
}
