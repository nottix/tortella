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

/*
 * Genera l'id dei pacchetti e degli utenti, avviene  tramite una combinazione 
 * del MAC-ADDRESS della macchina, il valore generato dalla funzione pseudo-randomica 
 * “srandom(time(NULL))”, e dall'ID iniziale presente nel file di configurazione 
 *  di ogni peer.
 */
u_int8 generate_id(void);

//Obsoleto
int generate_id4(void);

/*
 * Conversione in stringa di un unsigned long long.
 */
char *to_string(u_int8 num);

/*
 * Preparazione di un messaggio da inviare agli altri utenti: al messaggio originale
 * da inviare vengono aggiunti data e orario di invio e il nick del mittente.
 * Esempio: Data - ora - nickname : messaggio. 
 */
char *prepare_msg(time_t timestamp, const char *nick, char *msg, int msg_len);

/* 
 * Effettua il dump in esadecimale di un pacchetto.
 * Esempio di dump:
 *  50 4f 53 54 20 2a 20 48 54 54 	  POST * HTT
 *  50 2f 31 2e 31 0d 0a 55 73 65 	  P/1.1..Use
 *  72 2d 41 67 65 6e 74 3a 20 54 	  r-Agent: T
 *  6f 72 54 65 6c 6c 61 2f 30 2e 	  orTella/0.
 *  31 0d 0a 43 6f 6e 74 65 6e 74 	  1..Content	
 */
char *hex_dump(const char *packet, int len, int n);

/*
 * Chiama la funzione hex_dump specificando quanti caratteri devono essere stampati
 * su una riga. Il parametro è impostato di default a 10.
 */
char *dump_data(const char *packet, int len);

/*
 * Ottiene l'indirizzo MAC dell'interfaccia di rete disponibile.
 */
char *get_mac_addr(void);

#endif //UTILS_H
