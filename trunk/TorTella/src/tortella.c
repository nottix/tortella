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
	
	return (0);
}
