/*
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
 * ...data...
 *
 * HTTP_RES_POST
 * HTTP/1.1 200 OK
 * Server: TorTella/0.1
 * Content-Type: application/binary
 * Content-Length: 4
 */

#include "httpmanager.h"

http_packet *http_create_packet(tortella_packet *packet, u_int4 type, u_int4 status, char *filename, u_int4 range_start, u_int4 range_end, char *data, u_int4 data_len) {
	char *temp;
	u_int4 tortella_len = 0;
	http_packet *ret = NULL;
	temp = tortella_bin_to_char(packet, &tortella_len);
	if(temp==NULL)
		return NULL;
	if(type==HTTP_REQ_POST || type==HTTP_REQ_GET) {
		http_header_request *request = (http_header_request*)malloc(sizeof(http_header_request));
		request = http_create_header_request(request, type, filename, range_start, range_end);
		ret = (http_packet*)malloc(sizeof(http_packet));
		ret->header_request = request;
		ret->header_response = NULL;
		if(type==HTTP_REQ_POST) {
			ret->data = packet;
			ret->data_string = temp;
			ret->data_len = tortella_len;
		}
		else if(type==HTTP_REQ_GET) {
			ret->data = NULL;
			ret->data_string = data;
			ret->data_len = data_len;
		}

		ret->type = type;
		//int sock = createTcpSocket(ip, port);
		//sendPacket(sock, http_bin_to_char(ret, type));;

	}
	else if((type==HTTP_RES_POST || type==HTTP_RES_GET) && status>=HTTP_STATUS_OK) {
		http_header_response *response = (http_header_response*)malloc(sizeof(http_header_response));
		response = http_create_header_response(response, type, status, range_end-range_start);
		ret = (http_packet*)malloc(sizeof(http_packet));
		ret->header_request = NULL;
		ret->header_response = response;
		if(type==HTTP_RES_POST) {
			ret->data = packet;
			ret->data_string = temp;
			ret->data_len = tortella_len;
		}
		else if(type==HTTP_RES_GET) {
			ret->data = packet;  //Serve solamente desc_len
			ret->data_string = data;
			ret->data_len = data_len;
		}

		ret->type = type;
		//int sock = createTcpSocket(ip, port);
		//sendPacket(sock, http_bin_to_char(ret, type));;

	}
	return ret;
}

http_header_request *http_create_header_request(http_header_request *header, u_int4 type, char *filename, \
	u_int4 range_start, u_int4 range_end) {

	header->user_agent = "TorTella/0.1";
	header->connection = "Keep-Alive";
	header->range_start = range_start;
	header->range_end = range_end;
	if(type==HTTP_REQ_POST) {
		header->request = "POST * HTTP/1.1";
	}
	else if(type==HTTP_REQ_GET) {
		header->request = malloc(4+strlen(filename)+9);
		sprintf(header->request, "GET %s HTTP/1.1", filename);
	}
	else
		header=NULL;
	return header;
}

http_header_response *http_create_header_response(http_header_response *header, u_int4 type, u_int4 status, u_int4 content_len) {

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

char *http_bin_to_char(http_packet *packet) {
	char *buffer = NULL;	
	u_int4 type = packet->type;
	if(packet->header_request!=NULL) {
		http_header_request *header_request = packet->header_request;
		if(type==HTTP_REQ_POST) {		
			buffer = malloc(strlen(header_request->request)+2\
				+strlen(header_request->user_agent)+strlen(HTTP_AGENT)+2\
				+strlen(HTTP_CONNECTION)+strlen(header_request->connection)+2+2\
				+sizeof(tortella_header)+packet->data->header->desc_len+packet->data->header->data_len);

			sprintf(buffer, "%s\r\nUser-Agent: %s\r\nConnection: %s\r\n\r\n%s", header_request->request,\
				header_request->user_agent, header_request->connection, packet->data_string);
		}
		else if(type==HTTP_REQ_GET) {
			buffer = malloc(strlen(header_request->request)+2\
				+strlen(header_request->user_agent)+strlen(HTTP_AGENT)+2\
				+strlen(HTTP_REQ_RANGE)+10+1+10+2\
				+strlen(HTTP_CONNECTION)+strlen(header_request->connection)+2+2);

			sprintf(buffer, "%s\r\nUser-Agent: %s\r\nRange: bytes=%d-%d\r\nConnection: %s\r\n\r\n", header_request->request, header_request->user_agent, \
				header_request->range_start, header_request->range_end, header_request->connection);
		}
	}
	else if(packet->header_response!=NULL) {
		http_header_response *header_response = packet->header_response;
		if(type==HTTP_RES_POST) {
			buffer = malloc(strlen(header_response->response)+strlen(HTTP_SERVER)+strlen(header_response->server)+2\
				+strlen(HTTP_CONTENT_TYPE)+strlen(header_response->content_type)+2\
				+strlen(HTTP_CONTENT_LEN)+header_response->content_len+2+2\
				+sizeof(tortella_header)+packet->data->header->desc_len+packet->data->header->data_len);

			sprintf(buffer, "%s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s", header_response->response, header_response->server, header_response->content_type, \
			header_response->content_len, packet->data_string);
		}
		else if(type==HTTP_RES_GET) {
			buffer = malloc(strlen(header_response->response)+strlen(HTTP_SERVER)+strlen(header_response->server)+2\
				+strlen(HTTP_CONTENT_TYPE)+strlen(header_response->content_type)+2\
				+strlen(HTTP_CONTENT_LEN)+header_response->content_len+2+2\
				+sizeof(tortella_header)+packet->data->header->desc_len+packet->data->header->data_len);

			sprintf(buffer, "%s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s", header_response->response, header_response->server, header_response->content_type, \
			header_response->content_len, packet->data_string);
		}

	}
	printf("buffer:\n%s", buffer);
	return buffer;
}
