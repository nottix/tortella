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
 
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	
	list *root = NULL;
	list *child = list_malloc(NULL, (void*)25);
	root = list_add(root, child);
	printf("root: %d\n", (int)root->data);
	list *child1 = list_malloc(NULL, (void*)35);
	root = list_add(root, child1);
	printf("child1: %d\n", (int)child1->data);
	//list_del(root, child1, DATA_INT);
	child = list_get(root, child1, DATA_INT);
	printf("get di child1: %d\n", (int)child->data);
	
	printf("elem at %d is %d\n", 1, LIST_GET_INT(root, 1));
	
	/*list *root = NULL;
	pthread_t *thread1 = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_t *thread2 = (pthread_t*)malloc(sizeof(pthread_t));
	list *child = list_malloc(NULL, (void*)thread1);
	root = list_add(root, child);

	list *child1 = list_malloc(NULL, (void*)thread2);
	root = list_add(root, child1);

	//printf("child1: %d\n", (int)child1->data);
	//list_del(root, child1, DATA_THREAD);
	child = list_get(root, child1, DATA_THREAD);
	printf("get di child1: %d\n", (pthread_t*)child->data);
	*/
	return 0;
}
