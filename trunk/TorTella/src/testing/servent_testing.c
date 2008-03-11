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
#include <pthread.h>
#include <sys/types.h>
#include <sched.h>
#include <time.h>
#include <signal.h>

/*void *test(void *unused) {
	printf("Hello World %d\n", (int)unused);
	pthread_exit(NULL);
}

void *server(void *fd) {
	char *buffer = (char*)malloc(1500);
	//while(1) {
	listen_http_packet((int)fd, buffer, LP_READ);

	//printf("buf: %s\n", buffer);
	http_packet *packet = http_char_to_bin(buffer);
	//printf("HTTP PACKET: \n%s", packet->header_request->request);
	//print_packet(tortella_char_to_bin(packet->data_string));
	printf("HTTP PACKET: \n%s", packet->header_response->response);
	//};
	pthread_exit(NULL);
}

void *client(void *fd) {
	
	/*tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = 10;
	header->desc_id = LISTHITS_ID;
	
	listhits_desc *list1 = (listhits_desc*)malloc(sizeof(listhits_desc));
	packet->desc = (char*)list1;
	
	header->data_len = 3;
	header->desc_len = sizeof(list_desc);
		
	packet->header = header;
	packet->data = "kk";
	
	char *buffer;
	int len;
	if((buffer=http_bin_to_char(http_create_packet(packet, HTTP_REQ_POST, HTTP_STATUS_OK, "file", 0, 4, "data", 4), &len))==NULL)
		printf("Errore\n");
	
	//while(1) {
		send_packet((int)fd, buffer, len);
		sleep(1);
	//};
	
	//send_join_packet((int)fd, 1, 3, 4, 5, 6);
	//send_get_request_packet((int)fd, "test/test", 0, 10);
	//send_get_response_packet((int)fd, HTTP_STATUS_OK, 4, "dddd");
	send_post_response_packet((int)fd, HTTP_STATUS_OK);
	pthread_exit(NULL);
}
*/
int main(int argc, char **argv) {
/*	pthread_t *thread1;
	pthread_t *thread2;

	printf("Running\n");
	
	int recv = create_listen_tcp_socket("127.0.0.1", 2110);
	
	sleep(1);
	
	int sender = create_tcp_socket("127.0.0.1", 2110);
	
	pthread_create(thread1, NULL, server, (void*)recv);
	
	pthread_create(thread2, NULL, client, (void*)sender);
	
	//while(waitpid(WAIT_ANY, NULL, WNOHANG)>0) {
	//	printf("Thread terminato\n");
	//	sleep(1);
	//}
	sleep(4);
	pthread_kill(*thread2, SIGKILL);
	pthread_kill(*thread1, SIGKILL);
	*/
	
	servent_start("127.0.0.1", atoi(argv[1]));
	sleep(6);

	printf("Exiting\n");
	
	pthread_exit(NULL);
}
