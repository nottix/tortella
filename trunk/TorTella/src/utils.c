/*
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
 
#include "utils.h"

u_int8 generate_id(void) {
	srandom(time(0));
	return (gen_start+random())^time(NULL);
}

char *to_string(u_int8 num) {
	char *ret = (char*)malloc(60);
	sprintf(ret, "%lld", num);
	ret = realloc(ret, strlen(ret)+1);
	return ret;
}

/*char *to_string(u_int8 num1, u_int8 num2) {
	char *ret = (char*)malloc(512);
	sprintf(ret, "%lld;%lld", num1, num2);
	ret = realloc(ret, strlen(ret);
	return ret;
}*/

char *hex_dump(const char *packet, int len, int n)
{
	int i=0;
	int count = 0;
	int modulo =0;
	int div = len/n;
	int k = 0;
	int divtemp = div;
	if(len%n!=0) {
		modulo = n-(len%n);
		divtemp++;
	}

	int length =   (len*4+(divtemp)*4+(modulo)*3)*2;
	//int length = 5000;
	char *buffer = (char*)malloc(length);
	char *strtemp = (char*)calloc(4,1);

	strcat(buffer, "\n");

	if(len == 1) {
		sprintf(strtemp,"%02x ",(u_int1)packet[i]);
		strcat(buffer,strtemp);
		int templ = 0;
		templ = templ%n;

		for(k=n;k>templ+1;k--) {
			sprintf(strtemp,"   ");
			strcat(buffer,strtemp);
		}
		sprintf(strtemp,"\t  ");
		strcat(buffer,strtemp);
		sprintf(strtemp,"%c",packet[i]);
		strcat(buffer,strtemp); 
		sprintf(strtemp,"\n\n");
		strcat(buffer,strtemp);
		return buffer;
	}

	for(i=0;i<len;i++) {	
		
		if(i==len-1 && ((len)%n)!=0) {
			if((len-1)%n==0) {
				for(k=n;k>0;k--) {  
					sprintf(strtemp,"%02x ", (u_int1)packet[i-k]);
					strcat(buffer,strtemp);
				}
				sprintf(strtemp,"\t  ");
				strcat(buffer,strtemp);
				for(k=n;k>0;k--) {
					sprintf(strtemp,"%c", ((isprint(packet[i-k])!=0)?packet[i-k]:'.'));
					strcat(buffer,strtemp);
				}
				sprintf(strtemp,"\n");
				strcat(buffer,strtemp);
			}
			int temp = 0;
			for(k=div*n;k<len;k++) {
				sprintf(strtemp,"%02x ",(u_int1)packet[k]);
				strcat(buffer,strtemp);
				temp = k;
			}
			
			temp = temp%n;
			for(k=n;k>temp+1;k--) {
				sprintf(strtemp,"   ");
				strcat(buffer,strtemp);
			}
			sprintf(strtemp,"\t  ");
			strcat(buffer,strtemp);
			for(k=div*n;k<len;k++) {  
				sprintf(strtemp,"%c",((isprint(packet[k])!=0)?packet[k]:'.')); 
				strcat(buffer,strtemp);
			}
			sprintf(strtemp,"\n");
			strcat(buffer,strtemp);
			break;
		}
		if(i==len-1 && (len)%n==0) {
			for(k=count;k<len;k++) {
				sprintf(strtemp,"%02x ",(u_int1)packet[k]);
				strcat(buffer,strtemp);
			}
			sprintf(strtemp,"\t  ");
			strcat(buffer,strtemp);
			for(k=count;k<len;k++) {
				sprintf(strtemp,"%c",((isprint(packet[k])!=0)?packet[k]:'.')); 
				strcat(buffer,strtemp);
			}
			sprintf(strtemp,"\n");
			strcat(buffer,strtemp);
			break;
		}
		if(i!= 0 && i%n==0) {
			count = i;
			for(k=n;k>0;k--) {  
				sprintf(strtemp,"%02x ", (u_int1)packet[i-k]);
				strcat(buffer,strtemp);
			}
			sprintf(strtemp,"\t  ");
			strcat(buffer,strtemp);
			for(k=n;k>0;k--) {
				sprintf(strtemp,"%c", ((isprint(packet[i-k])!=0)?packet[i-k]:'.'));
				strcat(buffer,strtemp);
			}
			sprintf(strtemp,"\n");
			strcat(buffer,strtemp);
			continue;
		}

	}

	strcat(buffer, "\n");
	return buffer;
}

char *dump_data(const char *packet, int len) {
	return hex_dump(packet, len, 10);
	//return packet;
}
