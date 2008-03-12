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

list *list_add(list *root, list *child) {
	if(root==NULL) {
		root = list_malloc(child->next, child->data);
		printf("[list_add]root->data :%d\n", (int)root->data);
		return root;
	}
	
	list *iter = root;
	while(iter->next!=NULL) {
		iter = iter->next;
	}
	
	iter->next = child;
	printf("[list_add]child added\n");
	return root;
}

list *list_add_int(list *root, int data) {
	list *lst = list_malloc(NULL, (void*)data);
	root = list_add(root, lst);
	return root;
}

list *list_add_thread(list *root, pthread_t *thread) {
	list *lst = list_malloc(NULL, (void*)thread);
	root = list_add(root, lst);
	return root;
}

list *list_add_conn(list *root, conn_data *data) {
	list *lst = list_malloc(NULL, (void*)data);
	root = list_add(root, lst);
	return root;
}

list *list_del(list *root, list *child, int type) {
	if(root==NULL || child==NULL) {
		return NULL;
	}
	
	list *iter = root;
	list *old_iter = NULL;
	while(iter!=NULL) {
		if(type==DATA_INT) {
			printf("[list_del]type DATA_INT\n");
			int id_del = (int)iter->data;
			int id = (int)child->data;
			printf("[list_del]id_del: %d, id: %d\n", id_del, id);
			if(id_del==id) {
				if(old_iter==NULL) {
					printf("[list_del]old_iter null\n");
					root = iter->next;
					free(iter);
					return root;
				}
				old_iter->next = iter->next;
				printf("[list_del]iter: %d\n", (int)iter->data);
				free(iter);
				return root;
			}
		}
		if(type==DATA_THREAD) {
			printf("[list_del]type DATA_THREAD\n");
			pthread_t *thread_del = (pthread_t*)child->data;
			pthread_t *thread = (pthread_t*)iter->data;
			printf("[list_del]equal: %d\n", pthread_equal(*thread_del, *thread));
			if(pthread_equal(*thread_del, *thread)!=0) {
				if(old_iter==NULL) {
					printf("[list_del]old_iter null\n");
					root = iter->next;
					free(iter);
					return root;
				}
				old_iter->next = iter->next;
				//printf("[list_del]iter: %d\n", (int)iter->data);
				free(iter);
				return root;
			}
		}
		if(type==DATA_CONN) {
			printf("[list_del]type DATA_CONN\n");
			if( ((conn_data*)(child->data))->fd == ((conn_data*)(iter->data))->fd ) {
				if(old_iter==NULL) {
					printf("[list_del]old_iter null\n");
					root = iter->next;
					free(iter);
					return root;
				}
				old_iter->next = iter->next;
				//printf("[list_del]iter: %d\n", (int)iter->data);
				free(iter);
				return root;
			}
		}
		old_iter = iter;
		iter = iter->next; //Si passa al prossimo nodo
	}
	return root;
}

list *list_del_index(list *root, int index) {
	if(root==NULL)
		return NULL;
	
	int max = list_size(root);
	if(index>max) {
		printf("[list_get_index]Index is too big\n");
		return NULL;
	}
	list *iter = root;
	list *old_iter;
	int counter=0;
	while(iter!=NULL && counter<index) {
		counter++;
		old_iter = iter;
		iter=iter->next;
	}
	list *aux = iter;
	old_iter->next = iter->next;
	free(aux);
	return root;
}

void list_clear(list *root) {
	if(root==NULL)
		return;
	
	list *iter = root;
	list *aux;
	while(iter!=NULL) {
		aux = iter;
		iter = iter->next;
		free(aux);
	}
}

list *list_get(list *root, list* child, int type) {
	if(root==NULL || child==NULL) {
		return NULL;
	}
	
	list *iter = root;
	while(iter!=NULL) {
		if(type==DATA_INT) {
			printf("[list_get]type DATA_INT\n");
			int id_get = (int)iter->data;
			int id = (int)child->data;
			printf("[list_get]id_get: %d, id: %d\n", id_get, id);
			if(id_get==id) {
				return iter;
			}
		}
		if(type==DATA_THREAD) {
			printf("[list_get]type DATA_THREAD\n");
			pthread_t *thread_del = (pthread_t*)child->data;
			pthread_t *thread = (pthread_t*)iter->data;
			if(pthread_equal(*thread_del, *thread)!=0) {
				return iter;
			}
		}
		if(type==DATA_CONN) {
			printf("[list_get]type DATA_CONN\n");
			if( ((conn_data*)(child->data))->fd == ((conn_data*)(iter->data))->fd ) {
				return iter;
			}
		}
		iter = iter->next; //Si passa al prossimo nodo
	}
	return iter;
}

list *list_get_index(list *root, int index) {
	if(root==NULL)
		return NULL;
	
	int max = list_size(root);
	if(index>max) {
		printf("[list_get_index]Index is too big\n");
		return NULL;
	}
	list *iter = root;
	int counter=0;
	while(iter!=NULL && counter<index) {
		counter++;
		iter=iter->next;
	}
	return iter;
}

int list_size(list *root) {
	if(root==NULL) {
		return -1;
	}
	
	list *iter = root;
	int counter=0;
	while(iter!=NULL) {
		counter++;
		iter = iter->next;
	}
	return counter;
}

list *list_malloc(list *next, void *data) {
	list *node = (list*)malloc(sizeof(list));
	node->next = next;
	node->data = data;
	
	return node;
}
