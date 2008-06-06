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
	srandom(time(NULL));
	char *addr;
	addr = get_mac_addr();
	int i=0, j=10;
	unsigned long res = 0;
	for(; i<6; i++, j*=10) {
		res += addr[i]*j;
	}
	//logger(INFO, "[generate_id]MAC: %lld\n", res);
	return (conf_get_gen_start()+((random())^res))*(res/2);
}

int generate_id4(void) {
	srandom(time(NULL));
	char *addr;
	addr = get_mac_addr();
	int i=0, j=10;
	unsigned long res = 0;
	for(; i<6; i++, j*=10) {
		res += addr[i]*j;
	}
	return (random())^res;
}

char *to_string(u_int8 num) {
	char *ret = (char*)malloc(60);
	sprintf(ret, "%lld", num);
	ret = realloc(ret, strlen(ret)+1);
	return ret;
}

char *prepare_msg(time_t timestamp, const char *nick, char *msg, int msg_len) {
	char *time_str = asctime(localtime(&timestamp));
	time_str[strlen(time_str)-1]='\0';
	char *send_msg = calloc(msg_len+strlen(time_str)+strlen(nick)+60, 1);
	msg[msg_len] = '\0';
	sprintf(send_msg, "%s %s:\n%s\n\n", time_str, nick, msg);
	return send_msg;
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
	char *buffer = (char*)calloc(length, 1);
	char *strtemp = (char*)calloc(4, 1);

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



char *get_mac_addr(void) {
	char *addr = calloc(7, 1);
	struct ifreq ifr;
	struct ifreq *IFR;
	struct ifconf ifc;
	char buf[1024];
	int s, i;
	int ok = 0;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s==-1) {
		return NULL;
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	ioctl(s, SIOCGIFCONF, &ifc);

	IFR = ifc.ifc_req;
	
	i = (ifc.ifc_len / sizeof(struct ifreq));
	printf("ok i: %d\n", i);
	for (; i >= 0;) {

		printf("ok1\n");
		strcpy(ifr.ifr_name, IFR->ifr_name);
		printf("ok2\n");
		if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0) {
			printf("ok3\n");
			if (! (ifr.ifr_flags & IFF_LOOPBACK)) {
				printf("ok4\n");
				if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
					printf("ok5\n");
					ok = 1;
					break;
				}
			}
		}
		IFR++;
		i--;
	}

	close(s);
	if (ok) {
		bcopy( ifr.ifr_hwaddr.sa_data, addr, 6);
		addr[6] = '\0';
	}
	else {
		return NULL;
	}
	return addr;
}
