
#include "controller.h"

/*
 * Lancia l'applicazione, inizializzando il controller in due modalità differenti
 * a seconda dei parametri passati da riga di comando. Il primo parametro dev'essere
 * il path del file di configurazione, mentre il secondo (opzionale nel caso non si
 * conosca alcun vicino) dev'essere il path del file in cui sono inseriti ip e 
 * porta dei peer vicini conosciuti. La seconda fase consiste nell'inizializzazione
 * dei thread necessari a gestire la gui e infine si avvia l'interfaccia grafica 
 * tramite la chiamata a controller_init_gui().
 */
int main(int argc, char *argv[])
{
	
	if(argc<2) {
		printf("Usage: <conf_path> [cache_path] [filename]\n");
		return 0;
	}	
	
	//inizializzazione del controller
	if(argc==2) {
		controller_init(argv[1], NULL);
	}
	else {
		controller_init(argv[1], argv[2]);
	}
	
	//inizializzazione dei thread
	g_thread_init(NULL);
	gdk_threads_init();
	
	//inizializzazione della gui
	gtk_init (&argc, &argv);
	controller_init_gui();
	
	printf("started\n");

	return (0);
}
