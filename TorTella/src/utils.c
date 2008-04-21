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
	char *ret = (char*)malloc(512);
	sprintf(ret, "%lld", num);
	ret = realloc(ret, strlen(ret));
	return ret;
}

/*char *to_string(u_int8 num1, u_int8 num2) {
	char *ret = (char*)malloc(512);
	sprintf(ret, "%lld;%lld", num1, num2);
	ret = realloc(ret, strlen(ret);
	return ret;
}*/
