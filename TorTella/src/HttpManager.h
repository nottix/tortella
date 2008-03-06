#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H

#include "GnutellaProtocol.h"
#include "SocketManager.h"

#define HTTP_REQ_GET		1
#define	HTTP_RES_GET		2
#define HTTP_REQ_POST		3
#define HTTP_RES_POST		4
#define HTTP_STATUS_OK		200
#define	HTTP_STATUS_CERROR	400
#define HTTP_STATUS_SERROR	500

#define HTTP_REQ_POST_LINE	"POST * HTTP/1.1"
#define HTTP_AGENT		"User-Agent "
#define HTTP_REQ_RANGE		"Range: bytes="
#define HTTP_CONNECTION		"Connection: "
#define HTTP_CONTENT_TYPE	"Content-Type: "
#define HTTP_CONTENT_LEN	"Content-Length: "
#define HTTP_SERVER		"Server: "

#define HTTP_OK			"HTTP/1.1 200 OK\r\n"
#define HTTP_CERROR		"HTTP/1.1 400 Bad Request\r\n"
#define HTTP_SERROR		"HTTP/1.1 500 Internal Server Error\r\n"


struct HttpHeaderRequest {
	char *request;
	char *userAgent;
	u_int4 rangeStart;
	u_int4 rangeEnd;
	char *connection;
};
typedef struct HttpHeaderRequest HttpHeaderRequest;

struct HttpHeaderResponse {
	char *response;
	char *server;
	char *contentType;
	u_int4 contentLen;
};
typedef struct HttpHeaderResponse HttpHeaderResponse;

struct HttpPacket {
	HttpHeaderRequest *headerRequest;
	HttpHeaderResponse *headerResponse;
	GnutellaPacket *data;
	char *dataString;
	u_int4 dataLen;
};
typedef struct HttpPacket HttpPacket;

HttpPacket *HttpSendPacket(GnutellaPacket *packet, char *ip, int port, u_int4 type, u_int4 status, char *filename, u_int4 rangeStart, u_int4 rangeEnd, char *data, u_int4 dataLen);

HttpHeaderRequest *HttpCreateHeaderRequest(HttpHeaderRequest *header, u_int4 type, char *filename, u_int4 rangeStart, u_int4 rangeEnd);

HttpHeaderResponse *HttpCreateHeaderResponse(HttpHeaderResponse *header, u_int4 type, u_int4 status, u_int4 contentLen);

char *HttpBinToChar(HttpPacket *packet, u_int4 type);

#endif //HTTP_MANAGER_H
