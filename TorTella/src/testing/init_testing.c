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
 
#include "init.h"

int main(int argc, char **argv){

	if(argc<2) {
		printf("Usage: <filename>\n");
		return -1;
	}

	GList * data=init_read_file(argv[1]);
	//printf("prova 1\n");
	//printf("Inidirizzo ip: %s- porta: %s\n",((init_data *)g_list_nth_data(data,0))->ip,((init_data *)g_list_nth_data(data,0))->port);
	//printf("prova 2\n");
	//printf("Inidirizzo ip: %s- porta: %s\n",((init_data *)g_list_nth_data(data,1))->ip,((init_data *)g_list_nth_data(data,1))->port);
	//printf("prova 3\n");

return 0;
}
