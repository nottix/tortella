#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H

#include "common.h"
#include "tortellaprotocol.h"
#include "socketmanager.h"

//#define HTTP_DEBUG

#define HTTP_REQ_GET		1
#define	HTTP_RES_GET		2
#define HTTP_REQ_POST		3
#define HTTP_RES_POST		4
#define HTTP_STATUS_OK		200
#define	HTTP_STATUS_CERROR	400
#define HTTP_STATUS_SERROR	500

#define HTTP_REQ_POST_LINE	"POST * HTTP/1.1"
#define HTTP_AGENT			"User-Agent: "
#define HTTP_REQ_RANGE		"Range: bytes="
#define HTTP_CONNECTION		"Connection: "
#define HTTP_CONTENT_TYPE	"Content-Type: "
#define HTTP_CONTENT_LEN	"Content-Length: "
#define HTTP_SERVER			"Server: "

#define HTTP_OK			"HTTP/1.1 200 OK"
#define HTTP_CERROR		"HTTP/1.1 400 Bad Request"
#define HTTP_SERROR		"HTTP/1.1 500 Internal Server Error"


struct http_header_request {
	char *request;
	char *user_agent;
	u_int4 range_start;
	u_int4 range_end;
	u_int4 content_len;
	char *connection;
};
typedef struct http_header_request http_header_request;

struct http_header_response {
	char *response;
	char *server;
	char *content_type;
	u_int4 content_len;
};
typedef struct http_header_response http_header_response;

struct http_packet {
	u_int4 type;
	http_header_request *header_request;
	http_header_response *header_response;
	tortella_packet *data;
	char *data_string;
	u_int4 data_len;
};
typedef struct http_packet http_packet;

http_packet *http_create_packet(tortella_packet *packet, /*char *ip, int port,*/ u_int4 type, u_int4 status, char *filename, u_int4 range_start, u_int4 range_end, char *data, u_int4 data_len);

http_header_request *http_create_header_request(http_header_request *header, u_int4 type, char *filename, u_int4 range_start, u_int4 range_end, u_int4 data_len);

http_header_response *http_create_header_response(http_header_response *header, u_int4 type, u_int4 status, u_int4 content_len);

char *http_bin_to_char(http_packet *packet, int *len);

http_packet *http_char_to_bin(char *buffer);

char *http_get_value(const char *buffer, const char *name);

char *http_get_line(const char *buffer, u_int4 num);

#endif //HTTP_MANAGER_H
