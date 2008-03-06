
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SocketManager.h"

void testSocket();
void testListenSocket();

//Sto facendo le prove con netcat.
int main(int argc, char **argv)
{

    //testSocket();

    testListenSocket();


return 0;
}

void testSocket()
{
  int sockFd = 0;
    char *buffer = (char*)malloc(BUFFER_LEN);
    //fprintf(stdout, "\nbuffer address %x\n", buffer);
    sockFd = createTcpSocket("127.0.0.1",2008);

    sendPacket(sockFd, "ciao\n");

    if ( (recvPacket(sockFd, buffer)) > 0 )
        fprintf(stdout, "\nbuffer ricevuto  %s\n",buffer);
    deleteSocket(sockFd);
}

void testListenSocket()
{
    int listenfd = 0;
    int connfd = 0;
    char *buffer = (char*)malloc(BUFFER_LEN);
    //char *rcv_buffer = (char*)malloc(BUFFER_LEN);

    buffer = "benvenuto sul mio server\n";
    fprintf(stdout, "\nMessaggio da inviare : %s\n",buffer);
    fprintf(stdout, "\nTest ListenSocket\n");

    //Creo un socket di ascolto.
    listenfd = createListenTcpSocket("127.0.0.1",2008);

    //Mi metto in attesa di una nuova connessione, alla quale
    //inviero' subito i byte contenuti in buffer (LP_WRITE)
    //esiste anche LP_READ, che attende byte dal client
    //e LP_NONE che non usa il buffer.
    if( (connfd = listenPacket(listenfd, buffer, LP_WRITE)) > 0 )
        fprintf(stdout, "\nbuffer inviato! : %s\n",buffer);
    //if( (connfd = listenPacket(listenfd, 0, LP_READ)) > 0 )
    //    fprintf(stdout, "\nbuffer inviato! : %s\n",buffer);


    //recvPacket(connfd, rcv_buffer);
    //    fprintf(stdout, "\n%s\n",rcv_buffer);
    sendPacket(connfd, "send test\n");


    //shutdown(listenfd,SHUT_RDWR);
    deleteSocket(listenfd);
}
