
#include "controller.h"

int main(int argc, char *argv[])
{
	
	if(argc<2) {
		printf("Usage: <conf_path> [cache_path] [filename]\n");
		return 0;
	}	
	
	if(argc==2) {
		controller_init(argv[1], NULL);
	}
	else {
		controller_init(argv[1], argv[2]);
	}
	
	g_thread_init(NULL);
	gdk_threads_init();
	/*--  Initialize GTK --*/
	gtk_init (&argc, &argv);
	
	controller_init_gui();
	printf("started\n");
	//pthread_join(gtk_main_thread, NULL);

	return (0);
}
