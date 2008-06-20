/**
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

#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H

#include "common.h"
#include "tortellaprotocol.h"
#include "socketmanager.h"
#include "utils.h"
#include "logger.h"

#define HTTP_REQ_GET		0x40
#define	HTTP_RES_GET		0x41
#define HTTP_REQ_POST		0x42
#define HTTP_RES_POST		0x43
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

//!header della request http
struct http_header_request {
 char *request;
 char *user_agent;
 u_int4 range_start;
 u_int4 range_end;
 u_int4 content_len;
 char *connection;
};
typedef struct http_header_request http_header_request;

//!header della response http
struct http_header_response {
 char *response;
 char *server;
 char *content_type;
 u_int4 content_len;
};
typedef struct http_header_response http_header_response;

/**
  * pacchetto http, composto da tipo, header (request o response), pacchetto tortella,
  * pacchetto tortella convertito in stringa e relativa lunghezza.
  */
struct http_packet {
 u_int4 type;
 http_header_request *header_request;
 http_header_response *header_response;
 tortella_packet *data;
 char *data_string;
 u_int4 data_len;
};
typedef struct http_packet http_packet;

/**
  * Creazione del pacchetto http. Converte il pacchetto tortella in stringa e crea 
  * il pacchetto a seconda del tipo necessario differenziando il tipo request da 
  * quello response in modo da creare i rispettivi header
  */
http_packet *http_create_packet(tortella_packet *packet, u_int4 type, u_int4 status, char *filename, u_int4 range_start, u_int4 range_end, char *data, u_int4 data_len);

/**
  * Crea l'header dedicato alla request settando tutti i campi in modo appropriato
  */
http_header_request *http_create_header_request(http_header_request *header, u_int4 type, char *filename, u_int4 range_start, u_int4 range_end, u_int4 data_len);

/**
  * Crea l'header dedicato alla response settando tutti i campi in modo opportuno
  */
http_header_response *http_create_header_response(http_header_response *header, u_int4 type, u_int4 status, u_int4 content_len);

/**
  * Parsing del pacchetto http da binario a puntatore a carattere,
  */
char *http_bin_to_char(http_packet *packet, int *len);

/**
  * Parsing del parametro buffer in un pacchetto http.
  */
http_packet *http_char_to_bin(const char *buffer);

/**
  * Ritorna il valore contenuto nel campo rappresentato da name all'interno del
  * pacchetto (buffer).
  */
char *http_get_value(const char *buffer, const char *name);

/**
  * Ritorna la riga i-esima del pacchetto (buffer) specificata nel parametro num. 
  */
char *http_get_line(const char *buffer, u_int4 num);

#endif //!HTTP_MANAGER_H
