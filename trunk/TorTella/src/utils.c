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
	return random()^time(NULL);
}

char *to_string(u_int8 num) {
	//char *ret = (char*)malloc(60);
	//sprintf(ret, "%lld", num);
	//ret = realloc(ret, strlen(ret)+1);
	//return ret;
	return "aio";
}

/*char *to_string(u_int8 num1, u_int8 num2) {
	char *ret = (char*)malloc(512);
	sprintf(ret, "%lld;%lld", num1, num2);
	ret = realloc(ret, strlen(ret);
	return ret;
}*/

char *dump_data(const char *packet, int len)
{
	int mod=5;
	int i=0;
	int count = 0;
	int div = len/mod;
	char *out = calloc(1, 2048);
	char *line = calloc(1, 512);

	strcat(out, "\n");
	/*for(i=0;i<len;i++) {	
		if(i==len-1 && ((len)%mod)!=0) {
			if((len-1)%mod==0) {
				sprintf(line, "%3.3d %3.3d %3.3d %3.3d %3.3d %3.3c %3.3c %3.3c %3.3c %3.3c\n",packet[i-5],packet[i-4],packet[i-3],packet[i-2],packet[i-1],packet[i-5], packet[i-4], packet[i-3], packet[i-2], packet[i-1]);
				strcat(out, line);
			}
			int j=0;
			for(j=div*mod;j<len;j++) {
				sprintf(line, " %3.3d", packet[j]);
				strcat(out, line);
			}
			for(j=div*mod;j<len;j++) {
				sprintf(line, " %3.3c", packet[j]);
				strcat(out, line);
			}
			break;
		}
		if(i==len-1 && (len)%mod==0) {
			int j=0;
			for(j=count;j<len;j++) {
				sprintf(line, " %3.3d",packet[j]);
				strcat(out, line);
			}
			for(j=count;j<len;j++) {
				sprintf(line, " %3.3c",packet[j]);
				strcat(out, line);
			}
			sprintf(line, "\n");
			strcat(out, line);
			break;
		}
		if(i!= 0 && i%mod==0) {
			count = i;
			sprintf(line, "%3.3d %3.3d %3.3d %3.3d %3.3d %3.3c %3.3c %3.3c %3.3c %3.3c\n",packet[i-5],packet[i-4],packet[i-3],packet[i-2],packet[i-1],packet[i-5], packet[i-4], packet[i-3], packet[i-2], packet[i-1]);
			strcat(out, line);
			continue;
		}
	}*/
	
	return out;
}
