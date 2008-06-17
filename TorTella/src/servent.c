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
 
#include "servent.h"

/**
 * Crea un server socket
 */
int servent_create_server(char *src_ip, u_int4 src_port) {
	logger(SOCK_INFO, "[servent_create_server]Creating listen tcp socket on: %s:%d\n", src_ip, src_port);
	return create_listen_tcp_socket(src_ip, src_port);
}

/**
 * Crea un client socket
 */
int servent_create_client(char *dst_ip, u_int4 dst_port) {
	logger(SOCK_INFO, "[servent_create_client]Creating tcp socket on: %s:%d\n", dst_ip, dst_port);
	return create_tcp_socket(dst_ip, dst_port);
}

/**
 * Avvia il server thread che viene usato per accettare
 * le nuove connessioni e quindi lanciare nuovi thread
 * per gestirle
 */
int servent_start_server(char *local_ip, u_int4 local_port) {
	int serfd = servent_create_server(local_ip, local_port); //Avvia il server di ascolto
	if(serfd<0) {
		logger(ALARM_INFO, "[servent_start]Errore nella creazione del server\n");
		return -1;
	}
	pthread_t *serthread = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(serthread, NULL, servent_listen, (void*)serfd);
	
	//Aggiunta socket descriptor alle liste
	server_fd = g_list_prepend(server_fd, (gpointer)serfd);
	server_thread = g_list_prepend(server_thread, (gpointer)serthread);
	
	return 0;
}

/**
 * Avvia un client thread usato per gestire tutte le richieste di invio
 * pacchetti al peer associato.
 * 
 * \param id Se si conosce l'id del peer a cui connettersi lo si specifica, altrimenti 0.
 */
servent_data *servent_start_client(char *dest_ip, u_int4 dest_port, u_int8 id) {
	pthread_t *clithread = (pthread_t*)calloc(1, sizeof(pthread_t));
	u_int8 cliid = new_connection_counter++; //Incrementa il contatore degli ID falsi
	if(id >= conf_get_gen_start())	//Verifica che l'id inserito non sia falso (condizione di sicurezza)
		cliid = id;
	
	//Inizializzazione dati del peer all'interno della servent_data
	servent_data *servent = (servent_data*)calloc(1, sizeof(servent_data));
	servent->id = cliid;
	logger(SYS_INFO, "[servent_start_client]cliid: %lld\n", cliid);
	servent->ip = dest_ip;
	servent->port = dest_port;
	servent->chat_list = NULL;
	servent->queue = g_queue_new();
	servent->res_queue = g_queue_new();
	servent->is_online = 0;
	pthread_rwlock_init(&servent->rwlock_data, NULL);
	
	//Aggiunge il peer all'elenco degli utenti conosciuti (ma non necessariamente connessi)
	data_add_user(servent->id, servent->nick, servent->ip, servent->port);

	//Aggiunge la servent_data generata alla hashtable dei serventi
	g_hash_table_insert(servent_hashtable, (gpointer)to_string(cliid), (gpointer)servent);
	//Lancia il client thread associato al peer
	pthread_create(clithread, NULL, servent_connect, (void*)&cliid);
	client_thread = g_list_prepend(client_thread, (gpointer)(*clithread));
	
	//Attende l'avvenuta ricezione del messaggio di OK (o timeout)
	char *ret = servent_pop_response(servent);
	if(ret!=NULL && strcmp(ret, TIMEOUT)==0) {
		servent->post_type = CLOSE_ID;
		servent_send_packet(servent);
		return NULL;
	}
	
	return servent;
}

/**
 * Questa funzione viene utilizzata per il boot iniziale, in quanto si connette
 * alla lista dei peer conosciuti, presi dal file di init. Inoltre inizializza
 * tutte le variabili necessarie e avvia il server di ascolto nuove connessioni.
 *
 * \param init_servent Lista dei serventi necessari per il boot iniziale
 */
int servent_start(GList *init_servent) {
	//Inizializzazione servent locale
	if(servent_init(conf_get_local_ip(), conf_get_local_port(), ONLINE_ID)<0) {
		logger(SYS_INFO, "[servent_start]Error initializing data\n");
		return -1;
	}
	
	//Avvio server di ascolto richieste
	if(servent_start_server(conf_get_local_ip(), conf_get_local_port())<0) {
		logger(SYS_INFO, "[servent_start]Error starting server\n");
		return -2;
	}
	
	//Fase iniziale di reperimento degli utenti iniziali
	if(init_servent!=NULL) {
		if(servent_init_connection(init_servent)<0) {
			logger(SYS_INFO, "[servent_start]Error boot connections\n");
			return -3;
		}
		
	}
	return 0;
}

/**
 * Tale funzione non fa altro che avviare il timer thread utilizzato per supportare
 * il meccanismo di failure detection.
 */
int servent_start_timer(void) {
	timer_thread = (pthread_t*)calloc(1, sizeof(pthread_t));
	pthread_create(timer_thread, NULL, servent_timer, NULL);
	return 0;
}

/**
 * Si connette alla lista dei peer specificati, se qualcuno non è disponibile lo salta.
 */
int servent_init_connection(GList *init_servent) {
	int i;
	init_data *peer;
	for(i=0; i<g_list_length(init_servent); i++) {
		peer = (init_data*)g_list_nth_data(init_servent, i);
		logger(SOCK_INFO, "[servent_init_connection]Connecting to ip: %s, port: %d\n", peer->ip, peer->port);
		if(servent_start_client(peer->ip, peer->port, 0)==NULL) {
			logger(SYS_INFO, "[servent_init_connection]Unable to connect to known peer\n");
		}
	}
	return 0;
}

/**
 * Questa funzione viene chiamata alla chiusura di TorTella, serve per chiudere
 * tutti i socket aperti (non brutalmente).
 */
void servent_close_all(void) {
	int i;
	for(i=0; i<g_list_length(server_fd); i++) {
		logger(SYS_INFO, "[servent_close_all]Closing server %d\n", (int)g_list_nth_data(server_fd, i));
		delete_socket((int)g_list_nth_data(server_fd, i));
	}
	for(i=0; i<g_list_length(client_fd); i++) {
		logger(SYS_INFO, "[servent_close_all]Closing client %d\n", (int)g_list_nth_data(client_fd, i));
		delete_socket((int)g_list_nth_data(client_fd, i));
	}
	for(i=0; i<g_list_length(server_connection_fd); i++) {
		logger(SYS_INFO, "[servent_close_all]Closing server_connection %d\n", (int)g_list_nth_data(server_connection_fd, i));
		delete_socket((int)g_list_nth_data(server_connection_fd, i));
	}
	
}

/**
 * Ultima funzione chiamata alla chiusura del programma, termina tutti i thread attivi.
 */
void servent_kill_all_thread(int sig) {
	logger(SYS_INFO, "[servent_kill_all_thread]Killing threads\n");
	servent_close_supernode(); //Viene utilizzata per il salvataggio delle chat su file (non utilizzata)
	logger(SYS_INFO, "[servent_kill_all_thread]Closing supernode\n");
	servent_close_all();

	int i;
	for(i=0; i<g_list_length(server_thread); i++) {
		pthread_kill((pthread_t)g_list_nth_data(server_thread, i), SIGKILL);
	}
	for(i=0; i<g_list_length(client_thread); i++) {
		pthread_kill((pthread_t)g_list_nth_data(client_thread, i), SIGKILL);
	}
	for(i=0; i<g_list_length(server_connection_thread); i++) {
		pthread_kill((pthread_t)g_list_nth_data(server_connection_thread, i), SIGKILL);
	}
	
	if(timer_thread!=NULL)
		pthread_kill(*timer_thread, SIGKILL);
}

/**
 * Inizializza: le variabili del peer locale, il seed, le hashtable, i lock etc...
 */
int servent_init(char *ip, u_int4 port, u_int1 status) {
	
	//Inizializza la lista delle chat conosciute leggendo da un file predefinito (non utilizzato)
	servent_init_supernode();
	//Inizializza il seed
	srandom(time(NULL));
	//Recupera il numero iniziale da cui generare fake ID
	new_connection_counter = conf_get_connection_id_limit();
	logger(SYS_INFO, "[servent_init]Local Peer initialized on %s:%d\n", conf_get_local_ip(), conf_get_local_port());
	
	servent_hashtable = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);
	
	//----Routing-----
	search_packet_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	join_packet_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	leave_packet_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	//----------------
	
	local_servent = (servent_data*)calloc(1, sizeof(servent_data));
	u_int8 id = local_servent->id = generate_id();
	logger(SYS_INFO, "[servent_init]Local ID: %lld\n", local_servent->id);
	local_servent->ip = ip;
	local_servent->port = port;
	local_servent->queue = g_queue_new();
	local_servent->res_queue = g_queue_new();
	local_servent->status = status;
	local_servent->nick = conf_get_nick();
	local_servent->is_online = 1;
	//Aggiunta utente locale alle liste di utenti conosciuti
	data_add_user(local_servent->id, local_servent->nick, local_servent->ip, local_servent->port);
	
	pthread_rwlock_init(&local_servent->rwlock_data, NULL);
	g_hash_table_insert(servent_hashtable, (gpointer)to_string(id), (gpointer)local_servent);
	
	server_fd = NULL;
	client_fd = NULL;
	server_connection_fd = NULL;
	
	server_thread = NULL;
	client_thread = NULL;
	server_connection_thread = NULL;
	
	timer_thread = NULL;
	
	return 0;
}

/**
 * Funzione utilizzata per il recupero delle chat conosciute da file,
 * attualmente non più utilizzata.
 */
void servent_init_supernode() {
	//read_all();
}

/**
 * Funzione utilizzata per il salvataggio delle chat conosciute su file,
 * attualmente non più utilizzata.
 */
void servent_close_supernode() {
	//write_all(MODE_TRUNC);
}

//-----Gestione servent_data-----

/**
 * Restituisce il servent_data associato all'id richiesto.
 */
servent_data *servent_get(u_int8 id) {
	return (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id));
}

/**
 * Restituisce la lista completa delle servent_data.
 */
GList *servent_get_values(void) {
	return g_hash_table_get_values(servent_hashtable);
}

/**
 * Restituisce la lista completa delle chiavi associate alle servent_data.
 */
GList *servent_get_keys(void) {
	return g_hash_table_get_keys(servent_hashtable);
}

/**
 * Restituisce la servent_data del peer locale.
 */
servent_data *servent_get_local(void) {
	return local_servent;
}

//-----Gestione Queue-----

/**
 * Aggiunge alla coda di pacchetti da inviare ad uno specifico peer.
 * In particolare si passa una servent_data contenete tutti i dati
 * necessari all'invio del pacchetto.
 */
void servent_send_packet(servent_data *sd) {
	if(sd != NULL) 
		g_queue_push_tail(sd->queue, (gpointer)sd);
}

/**
 * Rimuove dalla coda di pacchetti da inviare ad uno specifico peer.
 * Se non ci sono pacchetti da rimuovere rimane in attesa.
 */
servent_data *servent_pop_queue(servent_data *sd) {
	servent_data *servent;
	if(sd==NULL || sd->queue==NULL) {
		logger(ALARM_INFO, "[servent_pop_queue]Queue error\n");
		return NULL;
	}
	//Ciclo utilizzato per attendere la richiesta di invio pacchetti
	while((servent = (servent_data*)g_queue_pop_head(sd->queue))==NULL) {
		usleep(100000);	//Attende prima di controllare di nuovo la coda
	}
	return servent;
}

/**
 * Appende alla coda di risposta di uno specifico peer.
 * In particolare aggiunge la risposta ricevuta dopo l'invio di un pacchetto.
 */
void servent_append_response(servent_data *sd, const char *response) {
	if(sd != NULL)
		g_queue_push_tail(sd->res_queue,(gpointer)strdup(response));
}

/**
 * Rimuove dalla coda di risposta un elemento se presente,
 * nel frattempo avvia un timer per rilevare eventuali timeout di risposta.
 */
char *servent_pop_response(servent_data *sd) {
	char *buf;
	if(sd==NULL || sd->res_queue==NULL) {
		logger(SYS_INFO, "[servent_append_responde]Response queue error\n");
		return NULL;
	}
	
	int counter = 0; //Contatore utilizzato per dare un timeout al superamento di una soglia
	while((buf = (char*)g_queue_pop_head(sd->res_queue))==NULL) {
		if(counter>10) { //Serve per il timeout
			buf = TIMEOUT;
			break;
		}
		usleep(200000);
		counter++;
	}
	return buf;
}

//-----Routing-----

/**
 * Ritorna l'ID del pacchetto richiesto, se presente.
 * E' una sorta di verifica presenta pacchetto.
 * Utilizzato per gestire i duplicati.
 */
char *servent_get_search_packet(u_int8 id) {
	return (char*)g_hash_table_lookup(search_packet_hashtable,(gconstpointer)to_string(id));
}

/**
 * Aggiunge il pacchetto alla lista dei pacchetti ricevuti.
 */
void servent_new_search_packet(u_int8 id) {
	g_hash_table_insert(search_packet_hashtable,(gpointer)to_string(id),(gpointer)to_string(id));
}

/**
 * Ritorna l'ID del pacchetto richiesto, se presente.
 * E' una sorta di verifica presenta pacchetto.
 * Utilizzato per gestire i duplicati.
 */
char *servent_get_join_packet(u_int8 id) {
	return (char*)g_hash_table_lookup(join_packet_hashtable, (gconstpointer)to_string(id));
}

/**
 * Aggiunge il pacchetto alla lista dei pacchetti ricevuti.
 */
void servent_new_join_packet(u_int8 id) {
	g_hash_table_insert(join_packet_hashtable, (gpointer)to_string(id),(gpointer)to_string(id));
}

/**
 * Ritorna l'ID del pacchetto richiesto, se presente.
 * E' una sorta di verifica presenta pacchetto.
 * Utilizzato per gestire i duplicati.
 */
char *servent_get_leave_packet(u_int8 id) {
	return (char*)g_hash_table_lookup(leave_packet_hashtable, (gconstpointer)to_string(id));
}

/**
 * Aggiunge il pacchetto alla lista dei pacchetti ricevuti.
 */
void servent_new_leave_packet(u_int8 id) {
	g_hash_table_insert(leave_packet_hashtable, (gpointer)to_string(id),(gpointer)to_string(id));
}

/**
 * Pulisce la lista dei pacchetti ricevuti che serve per scartare
 * pacchetti con ID uguale a quelli ricevuti recentemente.
 */
void servent_flush_data(void) {
	g_hash_table_remove_all(search_packet_hashtable);
	g_hash_table_remove_all(join_packet_hashtable);
	g_hash_table_remove_all(leave_packet_hashtable);
}


//---------THREAD---------------

/**
 * Thread che riceve le richieste di connessione e avvia nuovi thread.
 * Ogni nuovo peer (client) che richiede di connettersi al server locale viene
 * assegnato ad un nuovo Thread che si occupera' di rispondere alle richieste del client.
 */
void *servent_listen(void *parm) {
	int connFd;
	pthread_t *thread;
	
	while(1) {
		connFd = listen_http_packet((int)parm);
		logger(SYS_INFO, "[servent_listen]Received connection on socket: %d\n", connFd);
		if(connFd!=0) {
			thread = (pthread_t*)calloc(1, sizeof(pthread_t));
			//Aggiunge alla lista dei socket descriptor di connessione
			server_connection_fd = g_list_prepend(server_connection_fd, (gpointer)connFd);
			//Avvia il server thread
			pthread_create(thread, NULL, servent_respond, (void*)connFd);
			//Aggiunge alla lista degli identificatori dei thread di connessione
			server_connection_thread = g_list_prepend(server_connection_thread, (gpointer)(*thread));
		}
	}
	pthread_exit(NULL);
}

/**
 * Server thread che riceve i pacchetti e risponde adeguatamente. Ne esiste uno per ogni
 * peer a cui si è connessi. Questa funzione è il vero cuore di TorTella,
 * infatti gestisce tutti i comportamente dei programma in base ai pacchetti ricevuti.
 *
 * \param parm Socket descriptor della connessione
 */
void *servent_respond(void *parm) {
	logger(SYS_INFO, "[servent_respond]Server initialized\n");
	char *buffer;
	http_packet *h_packet;
	int len;
	int fd = (int)parm;
	u_int8 user_id = 0;
	u_int4 status = 0;
	
	while(1) {
		logger(SYS_INFO, "[servent_respond]Waiting\n");
		//Attesa ricezione pacchetto HTTP
		len = recv_http_packet(fd, &buffer);
		logger(PAC_INFO, "[servent_respond]Data received len: %d, buffer: \nSTART\n%s\nEND\n", len, dump_data(buffer, len));
		
		if(len>0) {
			logger(SYS_INFO, "[servent_respond]Converting\n");
			//Riempimento della struttura dati http_packet con i valori ricevuti
			h_packet = http_char_to_bin((const char*)buffer);
			if(h_packet!=NULL) {
				logger(SYS_INFO, "[servent_respond]Http packet received, type=%d\n", h_packet->type);
				if(h_packet->type==HTTP_REQ_POST) {
					
					logger(SYS_INFO, "[servent_respond]POST received\n");
					
					//Effettua le operazioni adeguatamente al tipo di pacchetto ricevuto
					if(h_packet->data==NULL || h_packet->data->header==NULL) {
						logger(SYS_INFO, "[servent_respond]Invalid HTTP packet\n");
						
						//Imposta lo status del pacchetto di risposta da inviare
						status = HTTP_STATUS_CERROR;
					}
					else if(h_packet->data->header->recv_id!=local_servent->id && h_packet->data->header->recv_id>=conf_get_gen_start()) {
						/**
						 * Entra in questa condizione se l'ID di ricezione del pacchetto è diverso dal locale,
						 * ovvero il pacchetto non è destinato al peer che l'ha ricevuto. Inoltre controlla che
						 * l'ID non sia falso.
						 */
						status = HTTP_STATUS_CERROR;
					}
					else if(h_packet->data->header->desc_id==JOIN_ID) {
						logger(SYS_INFO, "[servent_respond]JOIN received, packet_id: %lld\n", h_packet->data->header->id);

						//Invio di un pacchetto di notifica di avvenuta ricezione del JOIN
						status = HTTP_STATUS_OK;
						send_post_response_packet(fd, status, 0, NULL);
						logger(SYS_INFO, "[servent_respond]Sending post response\n");
						//Si imposta lo status a 0 per evitare di inviare un doppio pacchetto di notifica
						status = 0;
						
						//Verifica che il pacchetto ricevuto non sia un duplicato
						if(servent_get_join_packet(h_packet->data->header->id) == NULL) {
							//Aggiunge il pacchetto ricevuto all'hashtable associata al JOIN
							servent_new_join_packet(h_packet->data->header->id);
							
							GList *servent_list;
							servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
							if(conn_servent==NULL) {
								logger(SYS_INFO, "[servent_responde]conn_servent entry %lld doesn't found\n", h_packet->data->header->sender_id);
								continue;
							}
							//Aggiunta dell'utente che ha inviato il JOIN nelle liste contenenti gli utenti
							data_add_user(GET_JOIN(h_packet->data)->user_id, tortella_get_data(h_packet->data_string), GET_JOIN(h_packet->data)->ip, GET_JOIN(h_packet->data)->port);
							data_add_existing_user_to_chat(GET_JOIN(h_packet->data)->chat_id, GET_JOIN(h_packet->data)->user_id);
							controller_add_user_to_chat(GET_JOIN(h_packet->data)->chat_id, GET_JOIN(h_packet->data)->user_id);
							
							logger(SYS_INFO, "[servent_respond]Sending JOIN packet to others peer\n");
							
							//Verifica che il ttl sia maggiore di uno per il rinvio del pacchetto agli altri peer
							if(GET_JOIN(h_packet->data)->ttl>1) {
								logger(SYS_INFO, "[servent_respond]TTL > 1\n");
								int i;
								servent_list = g_hash_table_get_values(servent_hashtable);
								//Recupero di tutti i peer a cui si è connessi per rinviare il pacchetto (flooding)
								for(i=0; i<g_list_length(servent_list); i++) {
							
									conn_servent = (servent_data*)g_list_nth_data(servent_list, i);
									/**
									 * Evita di rinviare il pacchetto al peer da cui ha ricevuto il JOIN.
									 * Evita di inviare il pacchetto due volte ad uno stesso peer.
									 */
									if(conn_servent->id!=h_packet->data->header->sender_id && conn_servent->id!=servent_get_local()->id && conn_servent->id>=conf_get_gen_start()) {
										RLOCK(conn_servent->id);
										servent_data *sd;
										COPY_SERVENT(conn_servent, sd);
										sd->ttl = GET_JOIN(h_packet->data)->ttl-1;
										sd->hops = GET_JOIN(h_packet->data)->hops+1;
										sd->packet_id = h_packet->data->header->id;
										sd->user_id_req = GET_JOIN(h_packet->data)->user_id;
										sd->chat_id_req = GET_JOIN(h_packet->data)->chat_id;
										sd->status_req = GET_JOIN(h_packet->data)->status;
										sd->ip_req = GET_JOIN(h_packet->data)->ip;
										sd->port_req = GET_JOIN(h_packet->data)->port;
										sd->nick_req = tortella_get_data(h_packet->data_string);
										sd->post_type = JOIN_ID;
										UNLOCK(conn_servent->id);
										//Invio del pacchetto al peer selezionato
										servent_send_packet(sd);
										//Attesa ricezione risposta
										servent_pop_response(sd);
										logger(SYS_INFO, "[servent_respond]Retrasmitted JOIN packet %s to %s\n", sd->nick_req, sd->nick);
									}
								}		   
							}
						}
					}
					else if(h_packet->data->header->desc_id==PING_ID) {
						logger(SYS_INFO, "[servent_respond]PING received from %lld to %lld\n", h_packet->data->header->sender_id, h_packet->data->header->recv_id);
						
						status = HTTP_STATUS_OK;
						//invio del pacchetto di OK
						send_post_response_packet(fd, status, 0, NULL);
						logger(SYS_INFO, "[servent_respond]Sending post response\n");
						status = 0;
						
						user_id = h_packet->data->header->sender_id;
						servent_data *conn_servent;
						u_int8 id = h_packet->data->header->sender_id;
						logger(SYS_INFO, "[servent_responde]Searching in hashtable: %lld\n", id);
						/**
						  * Controlla che il peer che ha inviato il pacchetto sia conosciuto
						  * e quindi viene interpretato come un semplice ping inviato per 
						  * gestire il meccanismo di failure detection o il cambio di status.
						  */
						if((conn_servent=g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id)))!=NULL) {
							logger(SYS_INFO, "[servent_respond]Found: %lld\n", id);
							WLOCK(id);
							conn_servent->status = GET_PING(h_packet->data)->status;
							conn_servent->timestamp = h_packet->data->header->timestamp;
							conn_servent->nick = h_packet->data->data;
							conn_servent->is_online = 1;
							UNLOCK(id);
							data_add_user(conn_servent->id, conn_servent->nick, conn_servent->ip, conn_servent->port);
							logger(SYS_INFO, "[servent_respond]Old PING, nick: %s, status: %c\n", conn_servent->nick, conn_servent->status);
							/**
							  * notifica del cambio di status sulla gui. Presi i lock sulla gui
							  * per consentire l'accesso protetto ai dati della gui.
							  */
							gdk_threads_enter();
							controller_manipulating_status(id, conn_servent->status);
							gdk_threads_leave();
							
						}
						else {
							/**
							  * Entra in questo flusso quando il peer mittente non 
							  * è ancora conosciuto dal peer locale. Serve per 
							  * stabilire una nuova connesione tra i due peer.
							  */
							logger(SYS_INFO, "[servent_respond]New PING\n");
							/**
							  * controllo che l'id del mittente sia falso in modo da capire 
							  * che è la richiesta di una nuova connessione
							  */
							if(h_packet->data->header->recv_id < conf_get_gen_start()) {
								
								conn_servent = (servent_data*)calloc(1, sizeof(servent_data));
								conn_servent->ip = get_dest_ip(fd);
								conn_servent->port = GET_PING(h_packet->data)->port;
								conn_servent->timestamp = h_packet->data->header->timestamp;
								conn_servent->queue = g_queue_new();
								conn_servent->res_queue = g_queue_new();
								conn_servent->is_online = 1;
								
								conn_servent->status = GET_PING(h_packet->data)->status;
								logger(SYS_INFO, "[servent_respond]Status recv: %c\n", conn_servent->status);
								conn_servent->nick = h_packet->data->data;
								conn_servent->id = h_packet->data->header->sender_id;
								
								//aggiunta dell'utente alla lista dei peer conosciuti.
								data_add_user(conn_servent->id, conn_servent->nick, conn_servent->ip, conn_servent->port);
								
								//Si inizializza il mutex
								pthread_rwlock_init(&conn_servent->rwlock_data, NULL);
								
								logger(SYS_INFO, "[servent_respond]Lookup ID: %s\n", to_string(id));
								if(g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id))==NULL) {
									logger(SYS_INFO, "[servent_respond]connection %s added to hashtable\n", to_string(id));
									g_hash_table_insert(servent_hashtable, (gpointer)to_string(id), (gpointer)conn_servent);
								}
								
								//creazione nuovo client thread per gestire la connessione con il nuovo peer.
								pthread_t *cli_thread = (pthread_t*)malloc(sizeof(pthread_t));
								pthread_create(cli_thread, NULL, servent_connect, (void*)&id);
								client_thread = g_list_prepend(client_thread, (gpointer)(*cli_thread));
								
								//attesa risposta di OK (o TIMEOUT).
								servent_pop_response(conn_servent);
							}
							else {
								/**
								  * Rappresenta la seconda fase della connessione ad un peer.
								  * Esempio: il peer locale invia un ping con id falso ad un
								  * peer con cui vuole stabilire la connessione; il peer 
								  * remoto invia un ping con il vero id. Connessione stabilita.
								  */
								GList *users = servent_get_values ();
								int i=0;
								logger(SYS_INFO, "[servent_respond]Changing ID\n");
								for(;i<g_list_length(users);i++) {
									servent_data *tmp = (servent_data*)g_list_nth_data(users, i);
									
									char *nick = tmp->nick;
									char *tmp_ip = tmp->ip;
									u_int4 tmp_port = tmp->port;
									
									logger(SYS_INFO, "[servent_respond]old ID: %lld, new ID: %lld\n", tmp->id, h_packet->data->header->sender_id);
									logger(SYS_INFO, "[servent_respond]nick: %s, ip: %s, port: %d\n", nick, get_dest_ip(fd), GET_PING(h_packet->data)->port);
									/**
									  * recupera la servent_data associata al precedente 
									  * fake id e sostituisce il falso id con quello reale.
									  */ 
									if((strcmp(tmp_ip, get_dest_ip(fd))==0) && (tmp_port == GET_PING(h_packet->data)->port)) {
										logger(SYS_INFO, "[servent_respond]Changing old ID: %lld with new ID: %lld\n", tmp->id, h_packet->data->header->sender_id);
										//rimuove dalla hashtable la chiave con id fasullo
										g_hash_table_remove(servent_hashtable, (gpointer)to_string(tmp->id));
										
										tmp->id = h_packet->data->header->sender_id;
										tmp->status = GET_PING(h_packet->data)->status;
										tmp->timestamp = h_packet->data->header->timestamp;
										tmp->nick = h_packet->data->data;
										tmp->is_online = 1;
										//aggiunge l'utente alla lista dei peer conosciuti
										data_add_user(tmp->id, tmp->nick, tmp->ip, tmp->port);
										
										g_hash_table_insert(servent_hashtable, (gpointer)to_string(tmp->id),(gpointer)tmp);
									}
								}
							}
							
							
						}
					}
					else if(h_packet->data->header->desc_id==LEAVE_ID) { 
						/**
						  * Ricezione di un pacchetto di tipo LEAVE. 
						  */
						logger(SYS_INFO, "[servent_respond]LEAVE received, packet_id: %lld\n", h_packet->data->header->id);

						status = HTTP_STATUS_OK;
						//invio di un pacchetto di OK che conferma l'avvenuta ricezione del LEAVE
						send_post_response_packet(fd, status, 0, NULL);
						logger(SYS_INFO, "[servent_responde]Sending post response\n");
						status = 0;
						
						//controllo dei pacchetti LEAVE duplicati
						if(servent_get_leave_packet(h_packet->data->header->id) == NULL) {
							//aggiunge il pacchetto alla lista dei pacchetti LEAVE
							servent_new_leave_packet(h_packet->data->header->id);
							
							GList *servent_list;
							servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
							if(conn_servent==NULL) {
								logger(SYS_INFO, "[servent_responde]conn_servent entry %lld doesn't found\n", h_packet->data->header->sender_id);
								continue;
							}
							
							u_int8 chat_id = GET_LEAVE(h_packet->data)->chat_id;
						
							//Sconnetti dalla chat l'utente
							logger(SYS_INFO, "[servent_respond]Deleting user\n");
							gdk_threads_enter();
							controller_rem_user_from_chat(chat_id, GET_LEAVE(h_packet->data)->user_id);
							gdk_threads_leave();
							logger(SYS_INFO, "[servent_respond]Deleted user: %lld\n", GET_LEAVE(h_packet->data)->user_id);
							
						
							logger(SYS_INFO, "[servent_respond]Sending LEAVE packet to others peer\n");
						 
							//controllo che il TTL sia maggiore di uno in modo da reinviare il pacchetto
							if(GET_LEAVE(h_packet->data)->ttl>1) {
								logger(SYS_INFO, "[servent_respond]TTL > 1\n");
								int i;
								servent_list = g_hash_table_get_values(servent_hashtable);
								if(servent_list!=NULL) {
									for(i=0; i<g_list_length(servent_list); i++) {

										conn_servent = (servent_data*)g_list_nth_data(servent_list, i);
										/**
									 	  * Evita di rinviare il pacchetto al peer da cui ha ricevuto il LEAVE.
									 	  * Evita di inviare il pacchetto due volte ad uno stesso peer.
									      */
										if(conn_servent->id!=h_packet->data->header->sender_id && conn_servent->id!=servent_get_local()->id && conn_servent->id>=conf_get_gen_start()) {
											RLOCK(conn_servent->id);
											servent_data *sd;
											COPY_SERVENT(conn_servent, sd);
											sd->ttl = GET_LEAVE(h_packet->data)->ttl-1;
											sd->hops = GET_LEAVE(h_packet->data)->hops+1;
											sd->packet_id = h_packet->data->header->id;
											sd->user_id_req = GET_LEAVE(h_packet->data)->user_id;
											sd->chat_id_req = GET_LEAVE(h_packet->data)->chat_id;
											sd->post_type = LEAVE_ID;
											UNLOCK(conn_servent->id);
											//invio del pacchetto al peer selezionato
											servent_send_packet(sd);
											//attesa della ricezione del messaggio di OK (o di TIMEOUT)
											servent_pop_response(sd);
											logger(SYS_INFO, "[servent_respond]Retrasmitted LEAVE packet to other peers\n");
										}
									}
								}
							}
						}
					}
					else if(h_packet->data->header->desc_id==MESSAGE_ID) {
						/**
						  * Ricezione di un pacchetto di tipo MESSAGE.
						  */
						logger(SYS_INFO, "[servent_respond]MESSAGE ricevuto\n");
						
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));

						WLOCK(h_packet->data->header->sender_id);
						conn_servent->msg = h_packet->data->data;
						conn_servent->msg_len = h_packet->data->header->data_len;
						conn_servent->timestamp = h_packet->data->header->timestamp;
						//prepara il messaggio in modo da aggiornare la GUI
						char *send_msg = prepare_msg(conn_servent->timestamp, conn_servent->nick, conn_servent->msg, conn_servent->msg_len);
						UNLOCK(h_packet->data->header->sender_id);
						u_int8 chat_id = GET_MESSAGE(h_packet->data)->chat_id;
						if(chat_id==0) {
							logger(SYS_INFO, "[servent_respond]PM\n");
							//aggiornamento della gui relativa ad un messaggio privato
							gdk_threads_enter();
							controller_add_msg(h_packet->data->header->sender_id, send_msg);
							gdk_threads_leave();
						}
						else {
							logger(SYS_INFO, "[servent_respond]CHAT\n");
							//aggiornamento della gui relativa alla chat
							gdk_threads_enter();
							controller_add_msg_to_chat(chat_id, send_msg);
							gdk_threads_leave();
						}
						logger(SYS_INFO, "[servent_respond]msg: %s, msg_len: %d\n", conn_servent->msg, conn_servent->msg_len);
						

						status = HTTP_STATUS_OK;
					}
					else if(h_packet->data->header->desc_id==SEARCH_ID) {
						/**
						  * Ricezione di un messaggio di tipo SEARCH.
						  */
						logger(SYS_INFO, "[servent_respond]SEARCH ricevuto packet_id: %lld\n", h_packet->data->header->id);

						status = HTTP_STATUS_OK;
						//invio di un mpacchetto di avvenuta ricezione della SEARCH
						send_post_response_packet(fd, status, 0, NULL);
						logger(SYS_INFO, "[servent_respond]Sending post response\n");
						status = 0;
						//controllo dei pacchetti SEARCH duplicati
						if(servent_get_search_packet(h_packet->data->header->id) == NULL) {
							//aggiunge il pacchetto alla lista dei pacchetti LEAVE
							servent_new_search_packet(h_packet->data->header->id);
						
							GList *res;
							GList *servent_list;
							servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
							if(conn_servent==NULL) {
								logger(SYS_INFO, "[servent_respond]conn_servent entry %lld doesn't found\n", h_packet->data->header->sender_id);
								continue;
							}
							logger(SYS_INFO, "[servent_respond]conn_servent entry found\n");
							//controllo dell'integrità del pacchetto
							if(h_packet->data_len>0) {
								servent_data *sd;
								RLOCK(conn_servent->id);
								COPY_SERVENT(conn_servent, sd);
								UNLOCK(conn_servent->id);
							
								logger(SYS_INFO, "[servent_respond]Searching %s\n", tortella_get_data(h_packet->data_string));
								//Ricerca nelle chat conosciute la chat richiesta dalla SEARCH
								res = data_search_all_chat(tortella_get_data(h_packet->data_string));
								logger(SYS_INFO, "[servent_respond]Results number %d\n", g_list_length(res));
				
								logger(SYS_INFO, "[servent_respond]Sending to ID: %lld\n", sd->id);
								//Passa i risultati alla servent_data del peer remoto
								sd->chat_res = res;
								sd->packet_id = h_packet->data->header->id;
								sd->post_type = SEARCHHITS_ID;	
								//invio del pacchetto SEARCHHITS 
								servent_send_packet(sd);
								logger(SYS_INFO, "[servent_respond]Sending SEARCHHITS packet to searching peer\n");
							}
							//controllo che il TTL sia maggiore di uno in modo da reinviare il pacchetto
							if(GET_SEARCH(h_packet->data)->ttl>1) {
								logger(SYS_INFO, "[servent_respond]TTL > 1\n");
								int i;
								servent_list = g_hash_table_get_values(servent_hashtable);
								for(i=0; i<g_list_length(servent_list); i++) {
							
									conn_servent = (servent_data*)g_list_nth_data(servent_list, i);
									/**
									 * Evita di rinviare il pacchetto al peer da cui ha ricevuto il LEAVE.
									 * Evita di inviare il pacchetto due volte ad uno stesso peer.
									 */
									if(conn_servent->id!=h_packet->data->header->sender_id && conn_servent->id!=servent_get_local()->id && conn_servent->id>=conf_get_gen_start()) {
										RLOCK(conn_servent->id);
										servent_data *sd;
										COPY_SERVENT(conn_servent, sd);
										sd->ttl = GET_SEARCH(h_packet->data)->ttl-1;
										sd->hops = GET_SEARCH(h_packet->data)->hops+1;
										sd->title = tortella_get_data(h_packet->data_string);
										sd->title_len = h_packet->data->header->data_len;
										sd->packet_id = h_packet->data->header->id;
										sd->post_type = SEARCH_ID;
										UNLOCK(conn_servent->id);
										//reinvio del pacchetto SEARCH al peer selezionato
										servent_send_packet(sd);
										//Aggiunta regola di routing alla tabella
										add_route_entry(h_packet->data->header->id, h_packet->data->header->sender_id, conn_servent->id);
										logger(SYS_INFO, "[servent_respond]Retrasmitted SEARCH packet to other peers\n");
									}
								}		   
							}
						}
					}
					else if(h_packet->data->header->desc_id==SEARCHHITS_ID) {
						/**
						  * Ricezione di un pacchetto di tipo SEARCHHITS
						  */
						logger(SYS_INFO, "[servent_respond]SEARCHHITS ricevuto\n");

						status = HTTP_STATUS_OK;
						//invio di un pacchetto di avvenuta ricezione del SEARCHHITS
						send_post_response_packet(fd, status, 0, NULL);
						logger(SYS_INFO, "[servent_respond]Sending post response\n");
						status = 0;
						
						/**
						 * Converte la stringa dei risultati ricevuti in una lista di chat
						 * con i relativi utenti.
						 */
						GList *chat_list = data_char_to_chatlist(tortella_get_data(h_packet->data_string), h_packet->data->header->data_len);
						//Aggiunge le chat alle liste locali
						data_add_all_to_chat(chat_list);
						
						//Ritorna la regola di routing associata all'ID del pacchetto
						route_entry *entry = get_route_entry(h_packet->data->header->id);
						if(entry!=NULL) {
							//Entra se la regola esiste
							RLOCK(entry->sender_id);
							servent_data *sd;
							servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(entry->sender_id));
							
							COPY_SERVENT(conn_servent, sd);
							sd->packet_id = h_packet->data->header->id;
							sd->chat_res = chat_list;
							sd->post_type = SEARCHHITS_ID; 
							UNLOCK(entry->sender_id);
							//Invia il pacchetto SEARCHHITS al peer presente nella regola di routing
							servent_send_packet(sd);
							logger(SYS_INFO, "[servent_respond]Routing packet from %lld to %lld\n", h_packet->data->header->sender_id, entry->sender_id);
							//Elimina la regola o decrementa
							del_route_entry(h_packet->data->header->id);
							logger(SYS_INFO, "[servent_respond]Route entry %lld deleted\n", h_packet->data->header->id); 
						}
						else {
							//Se la regola non esiste vuol dire che il peer è colui che ha fatto la richiesta iniziale
							int i=0;
							chat *chat_val;

							for(; i<g_list_length(chat_list); i++) {
								chat_val = (chat*)g_list_nth_data(chat_list, i);																
								logger(SYS_INFO,"[servent_respond] title chat %s\n", chat_val->title);
								GList *local_chat = data_search_all_local_chat(chat_val->title);
								int j=0;
								for(; j<g_list_length(local_chat); j++) {
									chat *tmp = (chat*)g_list_nth_data(local_chat, j);
									logger(SYS_INFO, "[servent_respond] title chat %s\n", tmp->title);
									gdk_threads_enter();
									//Aggiunge la chat alla lista dei risultati nella GUI
									gui_add_chat(tmp->id, tmp->title);
									gdk_threads_leave();
								}
							}
						}
						
					}
					else if(h_packet->data->header->desc_id == BYE_ID) {
						logger(SYS_INFO, "[servent_respond]BYE ricevuto\n");
						
						servent_data *conn_servent = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(h_packet->data->header->sender_id));
						WLOCK(h_packet->data->header->sender_id);
						conn_servent->timestamp = h_packet->data->header->timestamp;
						UNLOCK(h_packet->data->header->sender_id);
						
						status = HTTP_STATUS_OK;
						//Invia il pacchetto di risposta
						send_post_response_packet(fd, status, 0, NULL);
						logger(SYS_INFO, "[servent_respond]Sending post response\n");
						status = 0;
						
						servent_data *sd;
						COPY_SERVENT(conn_servent, sd);
						sd->post_type= CLOSE_ID;
						//Chiusura thread connessione
						servent_send_packet(sd);

						logger(SYS_INFO, "[servent_respond]Deleting user\n");
						
						//Chiusura eventuali finestre PM
						gdk_threads_enter();
						controller_receive_bye(conn_servent->id);
						gdk_threads_leave();

						//Rimozione dalle strutture dati
						g_hash_table_remove(servent_hashtable, (gconstpointer)to_string(conn_servent->id));
						data_del_user(conn_servent->id);
						logger(SYS_INFO, "[servent_respond]Deleted user: %lld\n", conn_servent->id);
						
						//Chiusura forzata socket
						shutdown_socket(fd);
						server_fd = g_list_remove(server_fd, (gconstpointer)fd);
						server_connection_thread = g_list_remove(server_connection_thread, (gconstpointer)pthread_self());
						//Esce dal server thread
						pthread_exit(NULL);
					}
					
					//Invio la conferma di ricezione
					if(status>0) {
						logger(SYS_INFO, "[servent_respond]Sending post response\n");
						send_post_response_packet(fd, status, 0, NULL);
					}
						
				}
				else if(h_packet->type==HTTP_REQ_GET) {
					logger(SYS_INFO, "[servent_respond]GET received\n");
				}
			}
		}
		else {
			logger(SYS_INFO, "[servent_responde]Peer %lld disconnected\n", user_id);
			if(servent_get(user_id)!=NULL) {
				RLOCK(user_id);
				servent_data* srv = servent_get(user_id);
				servent_data* sd;
				COPY_SERVENT(srv, sd);
				sd->post_type = CLOSE_ID;
				//Chiude il client thread associato al peer
				servent_send_packet(sd);
				UNLOCK(user_id);
				
				//Rimuove il peer dalla GUI e dalle strutture dati
				gdk_threads_enter();
				controller_receive_bye(user_id);
				gdk_threads_leave();
				data_destroy_user(user_id);
			}
			
			//Chiusura forzata socket
			shutdown_socket(fd);
			server_connection_fd = g_list_remove(server_connection_fd, (gconstpointer)fd);
			server_connection_thread = g_list_remove(server_connection_thread, (gconstpointer)(pthread_self()));
			pthread_exit(NULL);
		}
	}
	pthread_exit(NULL);
}

/**
 * Client thread utilizzato per gestire il canale di invio pacchetti ad un peer.
 */
void *servent_connect(void *parm) {
	int len;
	char *buffer = NULL;
	http_packet *h_packet;
	u_int8 id_dest = *((u_int8*)(parm));
	logger(SYS_INFO, "[servent_connect]ID: %lld\n", id_dest);
	
	//Si prendono l'ip e la porta dalla lista degli id
	servent_data *servent_peer = (servent_data*)g_hash_table_lookup(servent_hashtable, (gconstpointer)to_string(id_dest));
	servent_data *servent_queue;
	if(servent_peer == NULL) {
		logger(SYS_INFO, "[servent_connect]Error\n");
		pthread_exit(NULL);
	}
	char *ip_dest = servent_peer->ip;
	u_int4 port_dest = servent_peer->port;
	
	//Creazione socket client
	int fd = servent_create_client(ip_dest, port_dest);

	client_fd = g_list_prepend(client_fd, (gpointer)fd);

	if(servent_peer->queue==NULL)
		servent_peer->queue = g_queue_new();
	if(servent_peer->res_queue==NULL)
		servent_peer->res_queue = g_queue_new();

	/**
	 * Aggiunta richiesta di PING nella coda del suddetto servent
	 * per iniziare la connessione verso il server.
	 */
	servent_data *tmp;
	COPY_SERVENT(servent_peer, tmp);
	tmp->post_type = PING_ID;
	servent_send_packet(tmp);

	u_int4 post_type;
	u_int8 user_id_req;
	u_int8 chat_id_req;
	u_int4 msg_len;
	char *msg;
	u_int4 title_len;
	char *title;
	u_int1 ttl, hops;
	u_int8 packet_id;
	u_int4 port_req;
	char *ip_req;
	char *nick_req;
	
	u_int1 status;
	u_int1 status_req;
	char *nick;

	//Ora si entra nel ciclo infinito che serve per inviare tutte le richieste
	while(1) {
		logger(SYS_INFO, "[servent_connect]Waiting\n");
		if(servent_peer==NULL) {
			logger(SYS_INFO, "[servent_connect]Peer error\n");
			pthread_exit(NULL);
		}
		//Attesa richiesta di invio pacchetto
		servent_queue = servent_pop_queue(servent_peer);
		/**
		 * Questo passo è fondamentale quando si effettua la connessione iniziale,
		 * Quando viene inviato il nuovo ID tramite PING.
		 */
		id_dest = servent_peer->id;
		if(servent_queue==NULL) {
			logger(SYS_INFO, "[servent_connect]Queue error\n");
			continue;
		}
		logger(SYS_INFO, "[servent_connect]Signal received in id_dest %lld\n", id_dest);
		
		RLOCK(local_servent->id); 
		WLOCK(id_dest); 
		logger(SYS_INFO, "[servent_connect]Post_type %d\n", servent_queue->post_type);
		
		//Si salvano tutti i dati nella struttura dati condivisa
		servent_peer->post_type = servent_queue->post_type;
		servent_peer->chat_id_req = servent_queue->chat_id_req;
		servent_peer->msg_len = servent_queue->msg_len;
		servent_peer->msg = servent_queue->msg;
		servent_peer->title = servent_queue->title;
		servent_peer->title_len = servent_queue->title_len;
		servent_peer->ttl = servent_queue->ttl;
		servent_peer->hops = servent_queue->hops;
		servent_peer->packet_id = servent_queue->packet_id;
		servent_peer->status_req = servent_queue->status_req;
		servent_peer->user_id_req = servent_queue->user_id_req;
		servent_peer->ip_req = servent_queue->ip_req;
		servent_peer->port_req = servent_queue->port_req;
		servent_peer->nick_req = servent_queue->nick_req;
		
		post_type = servent_peer->post_type;
		user_id_req = servent_peer->user_id_req;
		chat_id_req = servent_peer->chat_id_req;
		msg_len = servent_peer->msg_len;
		msg = servent_peer->msg;
		title = servent_peer->title;
		title_len = servent_peer->title_len;
		ttl = servent_peer->ttl;
		hops = servent_peer->hops;
		packet_id = servent_peer->packet_id;
		ip_req = servent_peer->ip_req;
		port_req = servent_peer->port_req;
		nick_req = servent_peer->nick_req;
		
		status = local_servent->status;
		status_req = servent_peer->status_req;
		nick = local_servent->nick;
		
		//Invio dei vari pacchetti
		if(post_type==HTTP_REQ_GET) {
			//Invio di pacchetto GET (non utilizzata)
			//send_get_request_packet(fd, char *filename, u_int4 range_start, u_int4 range_end);
		}
		else {
			if(post_type==JOIN_ID) {
				send_join_packet(fd, packet_id, local_servent->id, id_dest, status_req, user_id_req, chat_id_req, nick_req, port_req, ip_req, ttl, hops);
			}
			else if(post_type==PING_ID) {
				send_ping_packet(fd, local_servent->id, id_dest, nick, local_servent->port, status);
			}
			else if(post_type==BYE_ID) {
				logger(SYS_INFO, "[servent_connect]Sending bye packet\n");
				send_bye_packet(fd, local_servent->id, id_dest);
			}
			else if(post_type==CLOSE_ID) {
				//Richiesta di invio CLOSE, ovvero terminazione thread corrente
				UNLOCK_F(servent_peer);
				UNLOCK(local_servent->id);
				shutdown_socket(fd);
				client_fd = g_list_remove(client_fd, (gconstpointer)fd);
				client_thread = g_list_remove(client_thread, (gconstpointer)pthread_self());
				g_hash_table_remove(servent_hashtable, (gconstpointer)to_string(id_dest));
				pthread_exit(NULL);
			}
			else if(post_type==LEAVE_ID) {
				send_leave_packet(fd, packet_id, local_servent->id, id_dest, user_id_req, chat_id_req, ttl, hops);
			}
			else if(post_type==MESSAGE_ID) {
				send_message_packet(fd, local_servent->id, id_dest, chat_id_req, msg_len, msg);
			}
			else if(post_type==SEARCH_ID) {
				send_search_packet(fd, packet_id, local_servent->id, id_dest, ttl, hops, title_len, title);
			}
			else if(post_type==SEARCHHITS_ID) {
				int length;
				//Converte la lista delle chat in stringa, per inviare tramite pacchetto
				char *buf = data_chatlist_to_char(servent_queue->chat_res, &length);
				if(buf==NULL) {
					length=0;
				}
				else {
					logger(SYS_INFO, "[servent_connect]Results converted in buffer: %s\n", buf);
				}
				send_searchhits_packet(fd, packet_id, local_servent->id, id_dest, g_list_length(servent_queue->chat_res), length, buf);
			}

			//Attesa ricezione risposta
			logger(SYS_INFO, "[servent_connect]Listening response\n");
			len = recv_http_packet(fd, &buffer);
			logger(SYS_INFO, "[sevente_connect]Received response\n");
			if(len>0) {
				logger(SYS_INFO, "[servent_connect]buffer recv: %s\n", dump_data(buffer, len));
				h_packet = http_char_to_bin(buffer);
				if(h_packet!=NULL && h_packet->type==HTTP_RES_POST) {
					if(strcmp(h_packet->header_response->response, HTTP_OK)==0) {
						logger(SYS_INFO, "[servent_connect]OK POST received\n");
					}
					else {
						logger(SYS_INFO, "[servent_connect]Error\n");
					}
				}
			}
			
			if(h_packet != NULL) {
				logger(SYS_INFO, "[servent_connect]Appending response\n");
				if(servent_peer==NULL) {
					logger(SYS_INFO, "[servent_connect] Peer response NULL\n");
					pthread_exit(NULL);
				}
				if(servent_peer->post_type != SEARCH_ID && servent_peer->post_type != SEARCHHITS_ID && servent_peer->post_type != CLOSE_ID) {
					//Appende alla coda delle risposte il tipo di risposta ricevuta
					servent_append_response(servent_peer, h_packet->header_response->response);
					logger(SYS_INFO, "[servent_connect]Appended\n");
				}
			}
			else {
				//In caso di timeout appende alla coda delle risposte l'errore
				logger(SYS_INFO, "[servent_connect]Appending response TIMEOUT\n");
				servent_append_response(servent_peer, TIMEOUT);
			}
		}
		UNLOCK_F(servent_peer);
		UNLOCK(local_servent->id);
	}
	pthread_exit(NULL);
}

/**
 * Thread utilizzato per gestire il meccanismo di failure detection e per pulire
 * la lista dei pacchetti ricevuti. L'intervallo di tempo è impostato nel file
 * di configurazione.
 */
void *servent_timer(void *parm) {
	GList *list;
	servent_data *data, *tmp;
	char *ret;
	int i;
	
	while(1) {
		
		list=g_hash_table_get_values(servent_hashtable);
		
		for(i=0; i<g_list_length(list); i++) {
			data = (servent_data*)g_list_nth_data(list, i);
			if(data!=NULL && (data->id!=local_servent->id) && (data->id>=conf_get_gen_start())) {
			
				RLOCK(data->id);
				COPY_SERVENT(data, tmp);
				UNLOCK(data->id);
				tmp->post_type = PING_ID;
				//Invio PING al peer selezionato
				servent_send_packet(tmp);
				//Attende la risposta
				ret = servent_pop_response(tmp);
				if(ret!=NULL && strcmp(ret, TIMEOUT)==0) {
					//Entra in questo flusso se c'è stato un timeout (failure detection)
					logger(SYS_INFO, "[servent_timer]Timer expired per %lld\n", data->id);
					//Elimina il peer dalla GUI e dalle strutture dati
					gdk_threads_enter();
					controller_receive_bye(data->id);
					gdk_threads_leave();
					data_destroy_user(data->id);
					
					//Uccide il client thread associato al peer
					tmp->post_type = CLOSE_ID;
					servent_send_packet(tmp);
				}
				logger(SYS_INFO, "[servent_timer]Signaling %lld\n", data->id);
			}
		}
		
		//Libera le hashtable dei pacchetti
		servent_flush_data();
		logger(SYS_INFO, "[servent_timer]Sleeping\n");
		sleep(conf_get_timer_interval());
	}
}
