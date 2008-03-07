/*
 * Developed by Mosciatti Federico
 * TorTella socket interface, v.0.1.1 .
 */
#include <sys/types.h>
#include <sys/socket.h>

/*Coda di servizio per ricezione SYNC*/
#define QLEN 5
/*Lunghezza buffer ricevzione/trasmissione*/
#define BUFFER_LEN 1024

/*Definizione operazioni per listenPacket*/
#define LP_NONE     100
#define LP_WRITE    LP_NONE +1
#define LP_READ     LP_NONE +2

/*Crea un socket */
int createTcpSocket(const char* dst_ip, int dst_port);

/*Crea un socket di ascolto, server mode*/
int createListenTcpSocket(const char* src_ip, int src_port);

/*Chiude il socket, il socket viene mandato in TIME_WAIT*/
int deleteSocket(int sock_descriptor);

/*In ascolto per una nuova connessione.
    listensocket e' il descrittore socket di ascolto (restituito da createListenTcpSocket)
    Il parametro mode ha i seguenti significati:
        - LP_WRITE, aperta la connessione invia il buffer al client
        - LP_READ, aperta la connessione attende dati, inserendoli poi nel buffer
        - LP_NONE, crea solo la nuova connessione con il client.
*/
int listenPacket(int listenSocket, char* buffer, unsigned int mode);

/*Invia il contenuto di buffer*/
int sendPacket(int sock_descriptor, char* buffer/*, int len*/);

/*Attende la ricezione di un pacchetto (massima dimensione ricezione pari a BUFFER_LEN*/
int recvPacket(int sock_descriptor,char* buffer);

/*Attende la ricezione di un pacchetto con max_len byte*/
int recvSizedPacket(int sock_descriptor,char* buffer, int max_len);
