#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void hex_dump(char *packet, int len)
{
	int i=0;
        int count = 0;
        int div = len/5;
        printf("packet %s\n",packet);
	for(i=0;i<len;i++)
	{	

          if(i==len-1 && ((len)%5)!=0)
          {
             if((len-1)%5==0)
             {
                printf("%d %d %d %d %d %c %c %c %c %c\n",packet[i-5],packet[i-4],packet[i-3],packet[i-2],packet[i-1],packet[i-5], packet[i-4], packet[i-3], packet[i-2], packet[i-1]);
             }
             int j=0;
             for(j=div*5;j<len;j++)
               printf(" %d",packet[j]);
             for(j=div*5;j<len;j++)
               printf(" %c",packet[j]);
            
            break;
          }
          if(i==len-1 && (len)%5==0)
          {

             int j=0;
             for(j=count;j<len;j++)
               printf(" %d",packet[j]);
             for(j=count;j<len;j++)
               printf(" %c",packet[j]);
            
            printf("\n");
            break;
          }
	  if(i!= 0 && i%5==0)
          {
           count = i;
           printf("%d %d %d %d %d %c %c %c %c %c\n",packet[i-5],packet[i-4],packet[i-3],packet[i-2],packet[i-1],packet[i-5], packet[i-4], packet[i-3], packet[i-2], packet[i-1]);
           continue;
          }
          
        }

}

int main(int argc, char **argv)
{
  hex_dump(argv[1],strlen(argv[1]));
  return 0;
}

u_int4 send_searchhits_packet(u_int4 fd, u_int8 sender_id, u_int8 recv_id,u_int8 data_len, char *data) {
	
	tortella_packet* packet = (tortella_packet*)malloc(sizeof(tortella_packet));
	tortella_header* header = (tortella_header*)malloc(sizeof(tortella_header));
	
	header->id = generate_id();
	header->desc_id = SEARCHHITS_ID;
	header->sender_id = sender_id;
	header->recv_id = recv_id;
	header->timestamp = time(NULL);
	header->data_len = data_len;
	
	searchhits_desc *search = (searchhits_desc*)malloc(sizeof(searchhits_desc));
	packet->desc = (char*)search;

	header->desc_len = sizeof(search_desc);
		
	packet->header = header;
	packet->data = data;
	
	char *buffer;
	int len;
	http_packet *h_packet = http_create_packet(packet, HTTP_REQ_POST, 0, NULL, 0, 0, NULL, 0); 
	buffer = http_bin_to_char(h_packet, &len);
	if(buffer==NULL)
		printf("Errore\n");
	
	return send_packet(fd, buffer, len);
}

