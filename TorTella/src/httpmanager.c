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

http_packet *http_create_packet(tortella_packet *packet, u_int4 type, u_int4 status, char *filename, u_int4 range_start, u_int4 range_end, char *data, u_int4 data_len) {
	char *temp;
	u_int4 tortella_len = 0;
	http_packet *ret = NULL;
	if(type==HTTP_REQ_POST) {
		temp = tortella_bin_to_char(packet, &tortella_len);
		if(temp==NULL)
			return NULL;
	}
	printf("[http_create_packet]Entered %d\n", type);
	if(type==HTTP_REQ_POST || type==HTTP_REQ_GET) {
		printf("[http_create_packet]REQ POST or REQ_GET\n");
		http_header_request *request = (http_header_request*)malloc(sizeof(http_header_request));
		request = http_create_header_request(request, type, filename, range_start, range_end, tortella_len);
		ret = (http_packet*)malloc(sizeof(http_packet));
		ret->header_request = request;
		ret->header_response = NULL;
		if(type==HTTP_REQ_POST) {
			ret->data = packet;
			ret->data_string = temp;
			ret->data_len = tortella_len;
			//dump_data(temp, tortella_len);
		}
		else if(type==HTTP_REQ_GET) {
			ret->data = NULL;
			ret->data_string = NULL;
			ret->data_len = 0;
		}

		ret->type = type;
		//int sock = createTcpSocket(ip, port);
		//send_packet(sock, http_bin_to_char(ret, type));;

	}
	else if((type==HTTP_RES_POST || type==HTTP_RES_GET) && status>=HTTP_STATUS_OK) {
		http_header_response *response = (http_header_response*)malloc(sizeof(http_header_response));
		printf("[http_create_packet]before response creating with status %d\n", status);
		response = http_create_header_response(response, type, status, data_len);
		if(response!=NULL)
			printf("[http_create_packet]response created with status %s\n", response->response);
		else {
			printf("[http_create_packet]response not created\n");
			return NULL;
		}
		ret = (http_packet*)malloc(sizeof(http_packet));
		ret->header_request = NULL;
		ret->header_response = response;
		if(type==HTTP_RES_POST) {
			/*ret->data = packet;
			ret->data_string = temp;
			ret->data_len = tortella_len;*/
			ret->data = NULL;
			ret->data_string = data;
			ret->data_len = data_len;
		}
		else if(type==HTTP_RES_GET) {
		/*	ret->data = packet;  //Serve solamente desc_len
			ret->data_string = data;
			ret->data_len = data_len;*/
			ret->data = NULL;  //Serve solamente desc_len
			ret->data_string = data;
			ret->data_len = data_len;
		}

		ret->type = type;
		//int sock = createTcpSocket(ip, port);
		//send_packet(sock, http_bin_to_char(ret, type));;

	}
	return ret;
}

http_header_request *http_create_header_request(http_header_request *header, u_int4 type, char *filename, \
	u_int4 range_start, u_int4 range_end, u_int4 data_len) {

	header->user_agent = "TorTella/0.1";
	header->connection = "Keep-Alive";
	header->range_start = range_start;
	header->range_end = range_end;
	if(type==HTTP_REQ_POST) {
		header->request = "POST * HTTP/1.1";
		header->content_len = data_len;
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

char *http_bin_to_char(http_packet *packet, int *len) {
	char *buffer = NULL;	
	u_int4 type = packet->type;
	printf("[http_bin_to_char]Converting bin to char\n");
	if(packet->header_request!=NULL) {
		printf("[http_bin_to_char]request not NULL\n");
		http_header_request *header_request = packet->header_request;
		if(type==HTTP_REQ_POST) {
			printf("[http_bin_to_char]HTTP_REQ_POST\n");
			buffer = (char*)malloc(strlen(header_request->request)+2\
				+strlen(header_request->user_agent)+strlen(HTTP_AGENT)+2\
				+strlen(HTTP_CONTENT_LEN)+strlen(to_string(header_request->content_len))+2\
				+strlen(HTTP_CONNECTION)+strlen(header_request->connection)+2+2\
				+sizeof(tortella_header)+packet->data->header->desc_len+packet->data->header->data_len);

			sprintf(buffer, "%s\r\nUser-Agent: %s\r\nContent-Length: %d\r\nConnection: %s\r\n\r\n", header_request->request,\
				header_request->user_agent, header_request->content_len, header_request->connection);
			
			int buflen = strlen(buffer);
			char *iter = buffer;
			iter += buflen;
			memcpy(iter, packet->data_string, header_request->content_len);
			printf("[http_bin_to_char]data_string: %s\n", packet->data_string);
			
			*len = buflen+header_request->content_len;
			printf("[http_bin_to_char]content_len: %d, data_len: %d, sizeof: %d, buffer: %s\n", header_request->content_len, packet->data->header->data_len, sizeof(tortella_header), dump_data(buffer, *len));
			/*
			int i, j;
			int strpos = strstr(buffer, "\r\n\r\n")-buffer;
			strpos += 4;
			for(j=0, i=strpos; i<(strpos+header_request->content_len); i++, j++) {
				printf("[http_bin_to_char]buf[i]: %c\n", buffer[i]);
				buffer[i] = packet->data_string[j];
			}*/
			//*len = (strlen(buffer)+header_request->content_len);
			//printf("[http_bin_to_char]dump: %s\n", dump_data(buffer, i));
			
			//printf("data:\n%s", dump_data(buffer, strlen(buffer)+39));
		}
		else if(type==HTTP_REQ_GET) {
			buffer = malloc(strlen(header_request->request)+2\
				+strlen(header_request->user_agent)+strlen(HTTP_AGENT)+2\
				+strlen(HTTP_REQ_RANGE)+10+1+10+2\
				+strlen(HTTP_CONNECTION)+strlen(header_request->connection)+2+2);

			sprintf(buffer, "%s\r\nUser-Agent: %s\r\nRange: bytes=%d-%d\r\nConnection: %s\r\n\r\n", header_request->request, header_request->user_agent, \
				header_request->range_start, header_request->range_end, header_request->connection);
			
			*len = strlen(buffer);
		}
	}
	else if(packet->header_response!=NULL) {
		printf("[http_bin_to_char]response not NULL\n");
		http_header_response *header_response = packet->header_response;
		/*if(type==HTTP_RES_POST) {
			buffer = malloc(strlen(header_response->response)+strlen(HTTP_SERVER)+strlen(header_response->server)+2\
				+strlen(HTTP_CONTENT_TYPE)+strlen(header_response->content_type)+2\
				+strlen(HTTP_CONTENT_LEN)+header_response->content_len+2+2\
				+sizeof(tortella_header)+packet->data->header->desc_len+packet->data->header->data_len+2);

			//sprintf(buffer, "%s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s", header_response->response, header_response->server, header_response->content_type,
		//	sprintf(buffer, "%s\r\nServer: %s\r\n\r\n", header_response->response, header_response->server);
			
		//	*len = strlen(buffer);
							
			sprintf(buffer, "%s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s", header_response->response, header_response->server, header_response->content_type, \
			header_response->content_len, packet->data_string);
			
			*len = strlen(buffer)+header_response->content_len;
		}*/
		if(type==HTTP_RES_GET || type==HTTP_RES_POST) {
			
			int con_len = strlen(to_string(header_response->content_len));
			
			printf("[http_bin_to_char]Before check\n");
			if(type==HTTP_RES_POST) {
				header_response->content_type = "text/html";
				printf("[http_bin_to_char]inside check\n");
			}
			
		/*	*len = strlen(header_response->response)+strlen(HTTP_SERVER)+strlen(header_response->server)+2\
				+strlen(HTTP_CONTENT_TYPE)+strlen(header_response->content_type)+2\
				+strlen(HTTP_CONTENT_LEN)+con_len+2+2\
				+packet->data_len+2;
			
			printf("[http_bin_to_char]response: %s, server: %s, data_len: %d, content_len: %d, len: %d", header_response->response, header_response->server, packet->data_len, header_response->content_len, *len);
			
			
			buffer = malloc(*len);
*/
			//to_string(header_response->content_len);
			//printf("[http_bin_to_char]allocated memory, strlen(to_string(header_response->content_len)): %d\n", strlen(to_string(header_response->content_len)));
			//sprintf(buffer, "%s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s", header_response->response, header_response->server, header_response->content_type,
			//header_response->content_len, packet->data_string);
			if(packet->data_string!=NULL) {
				
				*len = strlen(header_response->response)+2
					+strlen(HTTP_SERVER)+strlen(header_response->server)+2
					+strlen(HTTP_CONTENT_TYPE)+strlen(header_response->content_type)+2
					+strlen(HTTP_CONTENT_LEN)+con_len+2+2
					+packet->data_len+1;
			
				printf("[http_bin_to_char]response: %s, server: %s, data_len: %d, content_len: %d, len: %d, con_len: %d", header_response->response, header_response->server, packet->data_len, header_response->content_len, *len, con_len);
			
				buffer = malloc(*len);
				
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
				
				printf("[http_bin_to_char]strlen(header_response->response): %d\n", strlen(header_response->response));
				printf("[http_bin_to_char]strlen(HTTP_SERVER): %d\n", strlen(HTTP_SERVER));
				printf("[http_bin_to_char]strlen(header_response->server): %d\n", strlen(header_response->server));
				printf("[http_bin_to_char]strlen(HTTP_CONTENT_TYPE): %d\n", strlen(HTTP_CONTENT_TYPE));
				printf("[http_bin_to_char]strlen(header_response->content_type): %d\n", strlen(header_response->content_type));
				printf("[http_bin_to_char]strlen(HTTP_CONTENT_LEN): %d\n", strlen(HTTP_CONTENT_LEN));
			
				printf("[http_bin_to_char]response: %s, server: %s, data_len: %d, content_len: %d, len: %d, con_len: %d", header_response->response, header_response->server, packet->data_len, header_response->content_len, *len, con_len);
			
				printf("[http_bin_to_char](header_response->response): %s\n", header_response->response);
				printf("[http_bin_to_char](HTTP_SERVER): %s\n", HTTP_SERVER);
				printf("[http_bin_to_char](header_response->server): %s\n", (header_response->server));
				printf("[http_bin_to_char](HTTP_CONTENT_TYPE): %s\n", (HTTP_CONTENT_TYPE));
				printf("[http_bin_to_char](header_response->content_type): %s\n", (header_response->content_type));
				printf("[http_bin_to_char](HTTP_CONTENT_LEN): %s\n", (HTTP_CONTENT_LEN));
			
				buffer = calloc(*len, 1);
				
				snprintf(buffer, *len+1, "%s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", header_response->response, header_response->server, header_response->content_type, \
					header_response->content_len);
				printf("[http_bin_to_char]buffer result: %s\n", buffer);
			}
		}

	}
	
#ifdef HTTP_DEBUG
	//printf("[http_bin_to_char]buffer:\n%s", buffer);
#endif
	return buffer;
}

http_packet *http_char_to_bin(const char *buffer) {
	char *saveptr;
	printf("[http_char_to_bin]Init\n");
	http_packet *packet = NULL;
	if(buffer==NULL)
		printf("[http_char_to_bin]buffer NULL\n");
	
	if(buffer!=NULL) {
		printf("[http_char_to_bin]prealloc\n");
		packet = (http_packet*)malloc(sizeof(http_packet));
		printf("[http_char_to_bin]allocated\n");
		char *result;

#ifdef HTTP_DEBUG
		printf("[http_char_to_bin]buffer: %s\n", buffer);
#endif
		if((result=strstr(buffer, "GET"))!=NULL) { //TODO
#ifdef HTTP_DEBUG
			printf("[http_char_to_bin]type: GET\n");
#endif
			packet->type = HTTP_REQ_GET;
			http_header_request *header_request = (http_header_request*)malloc(sizeof(http_header_request));
			
			header_request->request = http_get_line(buffer, 0);
			
			header_request->user_agent = http_get_value(buffer, HTTP_AGENT);
			
			char *range = http_get_value(buffer, HTTP_REQ_RANGE);
			header_request->range_start = atoi(strtok_r(range, "-",&saveptr));
			header_request->range_end = atoi(strtok_r(NULL, "-",&saveptr));
			
			header_request->connection = http_get_value(buffer, HTTP_CONNECTION);
			
			packet->header_request = header_request;
			packet->data = NULL;
			packet->data_string = NULL;
			packet->data_len = 0;
			
//#ifdef HTTP_DEBUG
			printf("[http_char_to_bin]request: %s\nagent: %s\nstart: %d\nend: %d\nconnection: %s\n", header_request->request, header_request->user_agent, header_request->range_start,
				   header_request->range_end, header_request->connection);
//#endif
		}
		else if((result=strstr(buffer, "POST"))!=NULL) { //TODO
#ifdef HTTP_DEBUG
			printf("[http_char_to_bin]type: POST\n");
#endif
			packet->type = HTTP_REQ_POST;
			http_header_request *header_request = NULL;
			header_request = calloc(1, sizeof(http_header_request));
			if(header_request == NULL)
				printf("son of a bitch\n");
			//http_header_request header_request_tmp;
			//http_header_request *header_request = &header_request_tmp;
			
			printf("[http_char_to_bin]1\n");
			header_request->request = http_get_line(buffer, 0);
			printf("[http_char_to_bin]2\n");
			header_request->user_agent = http_get_value(buffer, HTTP_AGENT);
			printf("[http_char_to_bin]3\n");
			header_request->content_len = atoi(http_get_value(buffer, HTTP_CONTENT_LEN));
			printf("[http_char_to_bin]4\n");
			header_request->connection = http_get_value(buffer, HTTP_CONNECTION);
			printf("[http_char_to_bin]5\n");
			packet->header_request = header_request;
			packet->data_string = strstr(buffer, "\r\n\r\n")+4;
			tortella_packet *t_packet = (tortella_packet*)malloc(sizeof(tortella_packet));
			tortella_header *t_header = (tortella_header*)packet->data_string;
			char *t_desc = tortella_get_desc(packet->data_string);
			char *t_data = tortella_get_data(packet->data_string);
			t_packet->header = t_header;
			t_packet->desc = t_desc;
			t_packet->data = t_data;
			packet->data = t_packet;
			packet->data_len = header_request->content_len;
			
#ifdef HTTP_DEBUG
			printf("[http_char_to_bin]request: %s\nagent: %s\ncontent_len: %d\nconnection: %s\ndata: %s\n", header_request->request, header_request->user_agent,
				   header_request->content_len, header_request->connection, packet->data_string);
#endif
		}
		else if((result=strstr(buffer, "application/binary"))!=NULL) { //TODO
#ifdef HTTP_DEBUG
			printf("[http_char_to_bin]ype: RES_GET\n");
#endif
			packet->type = HTTP_RES_GET;
			http_header_response *header_response = (http_header_response*)malloc(sizeof(http_header_response));
			
			header_response->response = http_get_line(buffer, 0);
			
			header_response->server = http_get_value(buffer, HTTP_SERVER);
			
			header_response->content_len = atoi(http_get_value(buffer, HTTP_CONTENT_LEN));
			
			packet->header_response = header_response;
			packet->data_string = strstr(buffer, "\r\n\r\n")+4;
			packet->data = NULL;
			packet->data_len = header_response->content_len;
			
#ifdef HTTP_DEBUG
			printf("[http_char_to_bin]response: %s\nserver: %s\ncontent_len: %d\ndata: %s\n", header_response->response, header_response->server, header_response->content_len, packet->data_string);
#endif
		}
		else if((result=strstr(buffer, "text/html"))!=NULL) { //TODO
#ifdef HTTP_DEBUG
			printf("[http_char_to_bin]type: RES_POST\n");
#endif
			packet->type = HTTP_RES_POST;
			http_header_response *header_response = (http_header_response*)malloc(sizeof(http_header_response));
			
			header_response->response = http_get_line(buffer, 0);
			
			header_response->server = http_get_value(buffer, HTTP_SERVER);
			
			packet->header_response = header_response;
			packet->data_string = strstr(buffer, "\r\n\r\n")+4;
			packet->data = NULL;
			packet->data_len = header_response->content_len;
			
#ifdef HTTP_DEBUG
			printf("[http_char_to_bin]response: %s\nserver: %s\n", header_response->response, header_response->server);
#endif
		}
		
		
	}
	return packet;
	
}

char *http_get_value(const char *buffer, const char *name) {
	char *saveptr;
	char *buf = (char*)malloc(strlen(buffer));
	memcpy(buf, buffer, strlen(buffer));
	char *token;
	char *ret;
	
	token=strtok_r(buf, "\r\n",&saveptr);
	if(token==NULL)
		return NULL;
	do {
		//printf("name: %s, token: %s\n", name, token);
		if((ret=strstr(token, name))!=NULL)
			return ret+=strlen(name);
	}while((token=strtok_r(NULL, "\r\n",&saveptr))!=NULL);
	return NULL;
	
}

char *http_get_line(const char *buffer, u_int4 num) {
	char *saveptr;
	char *buf = (char*)malloc(strlen(buffer));
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
