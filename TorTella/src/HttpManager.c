#include "HttpManager.h"

HttpPacket *HttpSendPacket(GnutellaPacket *packet, char *ip, int port, u_int4 type, u_int4 status, char *filename, u_int4 rangeStart, u_int4 rangeEnd, char *data, u_int4 dataLen) {
	char *temp;
	u_int4 gnutellaLen = 0;
	HttpPacket *ret = NULL;
	temp = GnutellaBinToChar(packet, &gnutellaLen);
	if(temp==NULL)
		return NULL;
	if(type==HTTP_REQ_POST || type==HTTP_REQ_GET) {
		HttpHeaderRequest *request = (HttpHeaderRequest*)malloc(sizeof(HttpHeaderRequest));
		request = HttpCreateHeaderRequest(request, type, filename, rangeStart, rangeEnd);
		ret = (HttpPacket*)malloc(sizeof(HttpPacket));
		ret->headerRequest = request;
		ret->headerResponse = NULL;
		if(type==HTTP_REQ_POST) {
			ret->data = packet;
			ret->dataString = temp;
			ret->dataLen = gnutellaLen;
		}
		else if(type==HTTP_REQ_GET) {
			ret->data = NULL;
			ret->dataString = data;
			ret->dataLen = dataLen;
		}

		int sock = createTcpSocket(ip, port);
		sendPacket(sock, HttpBinToChar(ret, type));;

	}
	else if((type==HTTP_RES_POST || type==HTTP_RES_GET) && status>=HTTP_STATUS_OK) {
		HttpHeaderResponse *response = (HttpHeaderResponse*)malloc(sizeof(HttpHeaderResponse));
		response = HttpCreateHeaderResponse(response, type, status, rangeEnd-rangeStart);
		ret = (HttpPacket*)malloc(sizeof(HttpPacket));
		ret->headerRequest = NULL;
		ret->headerResponse = response;
		if(type==HTTP_RES_POST) {
			ret->data = packet;
			ret->dataString = temp;
			ret->dataLen = gnutellaLen;
		}
		else if(type==HTTP_RES_GET) {
			ret->data = packet;  //Serve solamente payloadLen
			ret->dataString = data;
			ret->dataLen = dataLen;
		}

		int sock = createTcpSocket(ip, port);
		sendPacket(sock, HttpBinToChar(ret, type));;

	}
	return ret;
}

HttpHeaderRequest *HttpCreateHeaderRequest(HttpHeaderRequest *header, u_int4 type, char *filename, \
	u_int4 rangeStart, u_int4 rangeEnd) {

	header->userAgent = "TorTella/0.1";
	header->connection = "Keep-Alive";
	header->rangeStart = rangeStart;
	header->rangeEnd = rangeEnd;
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

HttpHeaderResponse *HttpCreateHeaderResponse(HttpHeaderResponse *header, u_int4 type, u_int4 status, u_int4 contentLen) {

	header->server = "TorTella/0.1";
	header->contentType = "application/binary";
	header->contentLen = contentLen;

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

char *HttpBinToChar(HttpPacket *packet, u_int4 type) {
	char *buffer = NULL;	
	if(packet->headerRequest!=NULL) {
		HttpHeaderRequest *headerRequest = packet->headerRequest;
		if(type==HTTP_REQ_POST) {		
			buffer = malloc(strlen(headerRequest->request)+2+strlen(headerRequest->userAgent)\
				+strlen(HTTP_AGENT)+2+strlen(HTTP_CONNECTION)+strlen(headerRequest\
				->connection)+2+2+sizeof(GnutellaHeader)+packet->data->header\
				->payloadLen);

			sprintf(buffer, "%s\r\nUser-Agent %s\r\nConnection: %s\r\n\r\n%s", headerRequest\
			->request, headerRequest->userAgent, headerRequest->connection, packet\
			->dataString);
		}
		else if(type==HTTP_REQ_GET) {
			buffer = malloc(strlen(headerRequest->request)+2+strlen(headerRequest->userAgent)\
				+strlen(HTTP_AGENT)+2+strlen(HTTP_REQ_RANGE)+10+1+10+2+\
				strlen(HTTP_CONNECTION)+strlen(headerRequest->connection)+2\
				+2);

			sprintf(buffer, "%s\r\nUser-Agent %s\r\nRange: bytes=%d-%d\r\nConnection: %s\r\n\r\n", headerRequest->request, headerRequest->userAgent, \
				headerRequest->rangeStart, headerRequest->rangeEnd, headerRequest\
				->connection);
		}
	}
	else if(packet->headerResponse!=NULL) {
		HttpHeaderResponse *headerResponse = packet->headerResponse;
		if(type==HTTP_RES_POST) {
			buffer = malloc(strlen(headerResponse->response)+strlen(HTTP_SERVER)+\
				strlen(headerResponse->server)+2+strlen(HTTP_CONTENT_TYPE)+\
				strlen(headerResponse->contentType)+2+strlen(HTTP_CONTENT_LEN)+\
				headerResponse->contentLen+2+2+sizeof(GnutellaHeader)+\
				packet->data->header->payloadLen);

			sprintf(buffer, "%s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s", headerResponse->response, headerResponse->server, headerResponse->contentType, \
			headerResponse->contentLen, packet->dataString);
		}
		else if(type==HTTP_RES_GET) {
			buffer = malloc(strlen(headerResponse->response)+strlen(HTTP_SERVER)+\
				strlen(headerResponse->server)+2+strlen(HTTP_CONTENT_TYPE)+\
				strlen(headerResponse->contentType)+2+strlen(HTTP_CONTENT_LEN)+\
				headerResponse->contentLen+2+2+packet\
				->data->header->payloadLen);

			sprintf(buffer, "%sServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s", headerResponse->response, headerResponse->server, headerResponse->contentType, \
			headerResponse->contentLen, packet->dataString);
		}

	}
	printf("buffer:\n%s", buffer);
	return buffer;
}
