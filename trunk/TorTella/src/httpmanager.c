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

/**
 * HTTP_REQ_GET
 * GET filename HTTP/1.1
 * User-Agent: TorTella/0.1
 * Range: bytes=start-end
 * Connection: Keep-Alive
 * ...tortella_packet...
 *
 * HTTP_RES_GET
 * HTTP/1.1 200 OK
 * Server: TorTella/0.1
 * Content-Type: application/binary
 * Content-Length: 4
 * ...data...
 *
 *
 * HTTP_REQ_POST
 * POST * HTTP/1.1
 * User-Agent: TorTella/0.1
 * Connection: Keep-Alive
 * Content-Length: 4 oppure chunked
 * ...data...
 *
 * HTTP_RES_POST
 * HTTP/1.1 200 OK
 * Server: TorTella/0.1
 * Content-Type: application/binary
 * Content-Length: 4
 * non dovrebbero esserci dati
 */

#include "httpmanager.h"

/**
 * Creazione del pacchetto http. Converte il pacchetto tortella in stringa e crea 
 * il pacchetto a seconda del tipo necessario differenziando il tipo request da 
 * quello response in modo da creare i rispettivi header
 */
http_packet *http_create_packet(tortella_packet *packet, u_int4 type, u_int4 status, char *filename, u_int4 range_start, u_int4 range_end, char *data, u_int4 data_len) {
	char *temp;
	u_int4 tortella_len = 0;
	http_packet *ret = NULL;
	if(type==HTTP_REQ_POST) {
		//!conversione del pacchetto http in stringa
		temp = tortella_bin_to_char(packet, &tortella_len);
		if(temp==NULL)
			return NULL;
	}
	if(type==HTTP_REQ_POST || type==HTTP_REQ_GET) {
		logger(HTTP_INFO, "[http_create_packet]Creating packet POST or GET request\n");
		http_header_request *request = (http_header_request*)calloc(1, sizeof(http_header_request));
		//!creazione dell'header della request
		request = http_create_header_request(request, type, filename, range_start, range_end, tortella_len);
		ret = (http_packet*)malloc(sizeof(http_packet));
		//!settaggio dei parametri dell'header
		ret->header_request = request;
		ret->header_response = NULL;
		if(type==HTTP_REQ_POST) {
			ret->data = packet;
			ret->data_string = temp;
			ret->data_len = tortella_len;
		}
		else if(type==HTTP_REQ_GET) {
			ret->data = NULL;
			ret->data_string = NULL;
			ret->data_len = 0;
		}

		ret->type = type;

	}
	else if((type==HTTP_RES_POST || type==HTTP_RES_GET) && status>=HTTP_STATUS_OK) {
		http_header_response *response = (http_header_response*)malloc(sizeof(http_header_response));
		logger(HTTP_INFO, "[http_create_packet]Creating packet POST or GET response status %d\n", status);
		//!creazione dell'header della response
		response = http_create_header_response(response, type, status, data_len);
		if(response==NULL) {
			logger(HTTP_INFO, "[http_create_packet]Response not created\n");
			return NULL;
		}
		ret = (http_packet*)calloc(1, sizeof(http_packet));
		//!settaggio dei parametri dell'header
		ret->header_request = NULL;
		ret->header_response = response;
		ret->data = NULL;
		ret->data_string = data;
		ret->data_len = data_len;
		ret->type = type;
	}
	return ret;
}

/**
 * Crea l'header dedicato alla request settando tutti i campi in modo appropriato
 */
http_header_request *http_create_header_request(http_header_request *header, u_int4 type, char *filename, \
	u_int4 range_start, u_int4 range_end, u_int4 data_len) {
	
	//!settaggio dei parametri comuni dell'header
	header->user_agent = "TorTella/0.1";
	header->connection = "Keep-Alive";
	header->range_start = range_start;
	header->range_end = range_end;
	//!settaggio dei parametri in relazione al tipo di pacchetto
	if(type==HTTP_REQ_POST) {
		header->request = "POST * HTTP/1.1";
		header->content_len = data_len;
	}
	else if(type==HTTP_REQ_GET) {
		header->request = calloc(4+strlen(filename)+9, 1);
		sprintf(header->request, "GET %s HTTP/1.1", filename);
	}
	else
		header=NULL;
	return header;
}

/**
 * Crea l'header dedicato alla response settando tutti i campi in modo opportuno
 */
http_header_response *http_create_header_response(http_header_response *header, u_int4 type, u_int4 status, u_int4 content_len) {

	//!settaggio dei parametri dell'header response
	header->server = "TorTella/0.1";
	header->content_type = "application/binary";
	header->content_len = content_len;

	switch(status) {
		case HTTP_STATUS_OK:
			header->response = HTTP_OK;
			break;
		case HTTP_STATUS_CERROR:
			header->response = HTTP_CERROR;
			break;
		case HTTP_STATUS_SERROR:
			header->response = HTTP_SERROR;
			break;
		default:
			header->response = NULL;
	}

	return header;
}

/**
 * Parsing del pacchetto http da binario a puntatore a carattere,
 */
char *http_bin_to_char(http_packet *packet, int *len) {
	char *buffer = NULL;	
	u_int4 type = packet->type;
	logger(HTTP_INFO, "[http_bin_to_char]Converting bin to char\n");
	if(packet->header_request!=NULL) {
		http_header_request *header_request = packet->header_request;
		if(type==HTTP_REQ_POST) {
			//!parsing del pacchetto di invio HTTP REQ POST
			logger(HTTP_INFO, "[http_bin_to_char]HTTP_REQ_POST\n");
			buffer = calloc(strlen(header_request->request)+2\
				+strlen(header_request->user_agent)+strlen(HTTP_AGENT)+2\
				+strlen(HTTP_CONTENT_LEN)+strlen(to_string(header_request->content_len))+2\
				+strlen(HTTP_CONNECTION)+strlen(header_request->connection)+2+2\
				+sizeof(tortella_header)+packet->data->header->desc_len+packet->data->header->data_len+1+1, 1); //!TODO: memory leak

			sprintf(buffer, "%s\r\nUser-Agent: %s\r\nContent-Length: %d\r\nConnection: %s\r\n\r\n", header_request->request,\
				header_request->user_agent, header_request->content_len, header_request->connection);
			
			int buflen = strlen(buffer);
			char *iter = buffer;
			iter += buflen;
			memcpy(iter, packet->data_string, header_request->content_len);
			iter += header_request->content_len;
			memcpy(iter, "\n", 1);
			
			*len = buflen+header_request->content_len+1;

		}
		else if(type==HTTP_REQ_GET) {
			//!parsing del pacchetto di invio HTTP REQ GET
			buffer = calloc(strlen(header_request->request)+2\
				+strlen(header_request->user_agent)+strlen(HTTP_AGENT)+2\
				+strlen(HTTP_REQ_RANGE)+strlen(to_string(header_request->range_start))+1+strlen(to_string(header_request->range_end))+2\
				+strlen(HTTP_CONNECTION)+strlen(header_request->connection)+2+2, 1);

			sprintf(buffer, "%s\r\nUser-Agent: %s\r\nRange: bytes=%d-%d\r\nConnection: %s\r\n\r\n", header_request->request, header_request->user_agent, \
				header_request->range_start, header_request->range_end, header_request->connection);
			
			*len = strlen(buffer);
		}
	}
	else if(packet->header_response!=NULL) {
		//!parsing del pacchetto di risposta.
		http_header_response *header_response = packet->header_response;
		if(type==HTTP_RES_GET || type==HTTP_RES_POST) {
			
			int con_len = strlen(to_string(header_response->content_len));
			
			if(type==HTTP_RES_POST) {
				header_response->content_type = "text/html";
			}
			
			if(packet->data_string!=NULL) {
				//!parsing dei dati, qualora presenti
				*len = strlen(header_response->response)+2
					+strlen(HTTP_SERVER)+strlen(header_response->server)+2
					+strlen(HTTP_CONTENT_TYPE)+strlen(header_response->content_type)+2
					+strlen(HTTP_CONTENT_LEN)+con_len+2+2
					+packet->data_len+1;

				buffer = calloc(*len, 1);
				
				sprintf(buffer, "%s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", header_response->response, header_response->server, header_response->content_type, \
					header_response->content_len);
				
				char *temp = buffer;
				temp+=strlen(buffer);
				
				memcpy(temp, packet->data_string, packet->data_len);
				
				temp+=packet->data_len;
				memcpy(temp, "\n", 1);
			}
			else {
				
				*len = strlen(header_response->response)+2
					+strlen(HTTP_SERVER)+strlen(header_response->server)+2
					+strlen(HTTP_CONTENT_TYPE)+strlen(header_response->content_type)+2
					+strlen(HTTP_CONTENT_LEN)+con_len+2+2;
			
				buffer = calloc((*len)+1, 1);
				
				snprintf(buffer, (*len)+1, "%s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", header_response->response, header_response->server, header_response->content_type, \
					header_response->content_len);
			}
		}

	}
	
	return buffer;
}

/**
 * Parsing del parametro buffer in un pacchetto http.
 */
http_packet *http_char_to_bin(const char *buffer) {
	char *saveptr;
	http_packet *packet = NULL;

	if(buffer!=NULL) {
		packet = (http_packet*)calloc(1, sizeof(http_packet));
		char *result;

		if((result=strstr(buffer, "GET"))!=NULL) { 
			//!parsing di un pacchetto di tipo GET
			packet->type = HTTP_REQ_GET;
			http_header_request *header_request = (http_header_request*)calloc(1, sizeof(http_header_request));
			//!settaggio dei campi dell'header
			header_request->request = http_get_line(buffer, 0);
			
			header_request->user_agent = http_get_value(buffer, HTTP_AGENT);
			
			char *range = http_get_value(buffer, HTTP_REQ_RANGE);
			header_request->range_start = atoi(strtok_r(range, "-",&saveptr));
			header_request->range_end = atoi(strtok_r(NULL, "-",&saveptr));
			
			header_request->connection = http_get_value(buffer, HTTP_CONNECTION);
			
			//!settaggio dei campi del pacchetto
			packet->header_request = header_request;
			packet->data = NULL;
			packet->data_string = NULL;
			packet->data_len = 0;

		}
		else if((result=strstr(buffer, "POST"))!=NULL) { 
			//!parsing di un pacchetto di tipo POST
			packet->type = HTTP_REQ_POST;
			http_header_request *header_request = NULL;
			
			header_request = calloc(1, sizeof(http_header_request));
			//!settaggio dei campi dell'header
			header_request->request = http_get_line(buffer, 0);
			header_request->user_agent = http_get_value(buffer, HTTP_AGENT);
			header_request->content_len = atoi(http_get_value(buffer, HTTP_CONTENT_LEN));
			header_request->connection = http_get_value(buffer, HTTP_CONNECTION);
			packet->header_request = header_request;
			packet->data_string = strstr(buffer, "\r\n\r\n")+4;
			tortella_packet *t_packet = (tortella_packet*)malloc(sizeof(tortella_packet));
			tortella_header *t_header = (tortella_header*)packet->data_string;
			char *t_desc = tortella_get_desc(packet->data_string);
			char *t_data = tortella_get_data(packet->data_string);
			//!settaggio dei campi del pacchetto
			t_packet->header = t_header;
			t_packet->desc = t_desc;
			t_packet->data = t_data;
			packet->data = t_packet;
			packet->data_len = header_request->content_len;
		}
		else if((result=strstr(buffer, "application/binary"))!=NULL) { 
			//!parsing del pacchetto di ricezione di una GET
			packet->type = HTTP_RES_GET;
			http_header_response *header_response = (http_header_response*)malloc(sizeof(http_header_response));
			//!settaggio dell'header di risposta
			header_response->response = http_get_line(buffer, 0);
			
			header_response->server = http_get_value(buffer, HTTP_SERVER);
			
			header_response->content_len = atoi(http_get_value(buffer, HTTP_CONTENT_LEN));
			//!settaggio dei campi del pacchetto
			packet->header_response = header_response;
			packet->data_string = strstr(buffer, "\r\n\r\n")+4;
			packet->data = NULL;
			packet->data_len = header_response->content_len;
		}
		else if((result=strstr(buffer, "text/html"))!=NULL) { 
			//!parsing del pacchetto di ricezione di una POST
			packet->type = HTTP_RES_POST;
			http_header_response *header_response = (http_header_response*)malloc(sizeof(http_header_response));
			
			//!settaggio dei campi dell'header di risposta
			header_response->response = http_get_line(buffer, 0);
			
			header_response->server = http_get_value(buffer, HTTP_SERVER);
			
			header_response->content_len = atoi(http_get_value(buffer, HTTP_CONTENT_LEN));
			
			//!settaggio dei campi del pacchetto
			packet->header_response = header_response;
			packet->data_string = strstr(buffer, "\r\n\r\n")+4;
			packet->data = NULL;
			packet->data_len = header_response->content_len;
		}
	}
	return packet;
	
}

/**
 * Ritorna il valore contenuto nel campo rappresentato da name all'interno del
 * pacchetto (buffer).
 */
char *http_get_value(const char *buffer, const char *name) {
	char *saveptr;
	char *buf = calloc(strlen(buffer), 1);
	memcpy(buf, buffer, strlen(buffer));
	char *token;
	char *ret;
	
	token=strtok_r(buf, "\r\n",&saveptr);
	if(token==NULL)
		return NULL;
	do {
		if((ret=strstr(token, name))!=NULL)
			return ret+=strlen(name);
	}while((token=strtok_r(NULL, "\r\n",&saveptr))!=NULL);
	return NULL;
	
}

/**
 * Ritorna la riga i-esima del pacchetto (buffer) specificata nel parametro num. 
 */
char *http_get_line(const char *buffer, u_int4 num) {
	char *saveptr;
	char *buf = calloc(strlen(buffer), 1);
	memcpy(buf, buffer, strlen(buffer));
	char *token;
	u_int4 counter = -1;
	
	token=strtok_r(buf, "\r\n",&saveptr);
	if(token==NULL)
		return NULL;
	counter++;
	do {
		if(counter==num)
			return token;
		counter++;
	}while((token=strtok_r(NULL, "\r\n",&saveptr))!=NULL);
	return NULL;
	
}
