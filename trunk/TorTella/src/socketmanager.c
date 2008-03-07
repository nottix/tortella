/*
 * Developed by Mosciatti Federico
 * TorTella socket interface, v.0.1.1 .
 */
#include "SocketManager.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG

//Crea un socket di connessione remota
int createTcpSocket( const char* dst_ip, int dst_port)
{
    struct sockaddr_in sAddr;
    int sockfd;

    //creazione socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        fprintf(stderr, "\nErrore nella creazione del socket tcp");
        return(1);
    }
#ifdef DEBUG
    fprintf(stdout, "\n[createTcpSocket]socket creato\n");
#endif
    memset((char*)&sAddr, 0, sizeof(sAddr) );
    sAddr.sin_family = AF_INET;
    if ( dst_port > 0 )
        sAddr.sin_port = htons(dst_port);
    else
        fprintf(stderr, "\nerrore nella porta: [%d]\n",dst_port);

    if( inet_pton(AF_INET, dst_ip,&sAddr.sin_addr) <= 0) {
        fprintf(stderr,"\nErrore nella conversione dell'indirizzo IP: [%s]\n",dst_ip);
        return(1);
    }
    if(connect(sockfd, (struct sockaddr *)&sAddr, sizeof(sAddr)) < 0){
        fprintf(stderr,"\nUnable to connect to: %s:%d\n",dst_ip,dst_port);
        return(1);
    }
    return sockfd;
}

int createListenTcpSocket( const char* src_ip, int src_port)
{
    int listenfd = 0;
    struct sockaddr_in sAddr;
     //creazione socket
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        fprintf(stderr, "\nErrore nella creazione del socket tcp");
        return(1);
    }
#ifdef DEBUG
    fprintf(stdout, "\n[createListenTcpSocket]socket creato\n");
#endif
    memset((char*)&sAddr, 0, sizeof(sAddr) );
    sAddr.sin_family = AF_INET;
     if ( src_port > 0 )
        sAddr.sin_port = htons(src_port);
    else
        fprintf(stderr, "\nerrore nella porta: [%d]\n",src_port);

    if( inet_pton(AF_INET, src_ip,&sAddr.sin_addr) <= 0) {
        fprintf(stderr,"\nErrore nella conversione dell'indirizzo IP: [%s]\n",src_ip);
        return(-1);
    }
    //Binding
    if ((bind(listenfd, (struct sockaddr *) &sAddr, sizeof(sAddr))) < 0)
    {
        fprintf(stderr, "\nBind error\n");
        return(-1);
    }
    //Listen
    if(listen(listenfd, QLEN) < 0)
    {
        fprintf(stderr, "\nListen error\n");
        return(-1);
    }

 return listenfd;
}

int deleteSocket(int sock_descriptor)
{
    if(sock_descriptor < 0 )
        fprintf(stderr, "\nSocketDescriptor error: [%d]",sock_descriptor);

    //if( shutdown(sock_descriptor, SHUT_RDWR) < 0)
    if( close(sock_descriptor) < 0)
    {
            fprintf(stderr, "\nSocket shutdown error");
        return(1);
    }
    return 0;
}

int listenPacket(int listenSocket, char* buffer, unsigned int mode)
{
    int connFd = 0;
    //for(;;)
    //{
        if((connFd = accept(listenSocket, (struct sockaddr *)NULL, NULL)) < 0)
        {
            fprintf(stderr, "\nerrore nell'accept\n");
            return(-1);
        }
    //}
#ifdef DEBUG
    fprintf(stdout, "\n[listenPacket]Nuova connessione ricevuta, modalita' %d!\n",mode);
#endif
    switch(mode)
    {
            case LP_WRITE :
                /*write*/
#ifdef DEBUG
                fprintf(stdout, "\n[listenPacket]Invio dati...\n");
#endif

                if(write(connFd, buffer, strlen(buffer)) != strlen(buffer))
                {
                    fprintf(stderr, "\nErrore in scrittura!");
                }
#ifdef DEBUG
                fprintf(stdout, "\n[listenPacket]Dati inviati\n");
#endif
                break;
            case LP_READ :
                /*read*/
                if( recvPacket(connFd, buffer) < 0)
                {
                    fprintf(stderr, "\nErrore in lettura!");
                }
#ifdef DEBUG
                fprintf(stdout, "\n[listenPacket]Dati ricevuti\n");
#endif
                break;
            case LP_NONE :
                break;
                default :
                    fprintf(stderr, "\nno selection\n");
    }
        return connFd;
}

//Usando la libreria string.h, si potrebbe omettere il parametro len
int sendPacket(int sock_descriptor, char* buffer/*, int len*/)
{
    int char_write = 0;
    if( sock_descriptor < 0 )
    {
        fprintf(stderr, "[sendPacket]Socket descriptor not valid, sock_descriptor = %d", sock_descriptor);
        return -1;
    }
    //Questa blocco si potrebbe ritentare per n volte, dove n e' un parametro di configurazione.
    if( (char_write = write( sock_descriptor, buffer, strlen(buffer))) != strlen(buffer))
    {
        fprintf(stderr,"\n[sendPacket]Perdita dati in trasmissione");
        return -2;
    }
#ifdef DEBUG
    fprintf(stdout, "\n[createRemoteTcpSocket]char_write = %d\n",char_write);
#endif
    return char_write;
}

//Buffer e' gia allocato?
int recvPacket(int sock_descriptor,char* buffer)
{
    return recvSizedPacket(sock_descriptor, buffer, BUFFER_LEN);
}

int recvSizedPacket(int sock_descriptor, char *buffer, int max_len)
{
    unsigned int char_read = 0;
    unsigned int counter = 0;
    if ( max_len < 0)
        return -1;
    if (buffer == 0 )
        return -1;

    memset(buffer, 0, max_len);
#ifdef DEBUG
    fprintf(stdout, "\n[recvPacket]buffer_ptr %x\n",buffer);
#endif
    if( sock_descriptor < 0 )
    {
        fprintf(stderr, "[recvPacket]Socket descriptor not valid, sock_descriptor = %d", sock_descriptor);
        return -1;
    }
    while( (char_read = read(sock_descriptor, buffer, max_len )) > 0)
    {
#ifdef DEBUG
    fprintf(stdout, "\n[recvPacket]char_read = %d\n",char_read);
#endif
        counter += char_read;
        if ( (counter >= max_len) )
        {
            //questo vuol dire che sono ancora presenti caratteri sul socket ( read > 0 )
            //ma il buffer allocato non ha piu' spazio a disposizione
            //si potrebbe creare una struttura buffer dinamica.
            //Un buffer a due dimensioni [n][BUFFER_LEN].
#ifdef DEBUG
            fprintf(stderr, "\n[recvPacket]Buffer overflow!, perdita dati\n");
#endif
        }
    //fine carattere
    buffer[char_read] = 0;
    }
    if ( char_read < 0)
    {
        fprintf(stderr, "\n[recvPacket]read error");
        return -1;
    }
#ifdef DEBUG
    fprintf(stdout, "\n[recvPacket]buffer read = %s\n",buffer);
#endif
    return char_read;
}
