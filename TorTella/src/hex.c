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
