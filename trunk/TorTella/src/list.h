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
 
#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define DATA_THREAD	0x110
#define DATA_INT	DATA_THREAD+1

#define LIST_GET_INT(root, index)	(int)(list_get_index(root, index)->data)
#define LIST_GET_THREAD(root, index)	(pthread_t*)(list_get_index(root, index)->data)

struct list {
	struct list *next;
	void *data; //Con void* è possibile memorizzare ogni tipo di dato
};
typedef struct list list;

list *list_add(list *root, list *child);

list *list_add_int(list *root, int data);

list *list_add_thread(list *root, pthread_t *thread);

list *list_del(list *root, list *child, int type);

list *list_del_index(list *root, int index);

void list_clear(list *root);

list *list_get(list *root, list* child, int type);

list *list_get_index(list *root, int index);

int list_size(list *root);

list *list_malloc(list *next, void *data);

#endif //LIST_H
