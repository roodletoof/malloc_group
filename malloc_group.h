/*
 * License: MIT
 *
 * A tiny header-only library for scoped heap allocation by treating each
 * allocated chunk of memory as a node in a linked list.
 *
 * Example:
 * 
 * for (size_t i = 0; i < 100; i++) {
 * 	malloc_group_t *mg = MG_SCOPED; // Create a malloc group, which
 * 					// exists in the scope it is
 * 					// defined in only.
 * 
 * 	// Allocate whatever you need.
 * 	int *i = mg_malloc(mg, sizeof(int));
 * 	float *f = mg_malloc(mg, sizeof(float));
 * 	char *str = mg_malloc(mg, sizeof(char) * 81);
 * 
 * 	mg_free(mg); // Everything gets freed here.
 * }
 * 
*/

#ifndef MALLOC_GROUP_HEADER
#define MALLOC_GROUP_HEADER

	#include <stdlib.h>
	#include <assert.h>

	typedef struct malloc_node {
		struct malloc_node *next;
		struct malloc_node *prev;
		size_t data_size;
		unsigned char data[];
	} malloc_node_t;

	typedef struct malloc_group {
		malloc_node_t *head;
	} malloc_group_t;

	#define MG_SCOPED &((malloc_group_t){NULL})

	void *mg_malloc(malloc_group_t *mg, size_t size);
	void *mg_realloc(malloc_group_t *mg, void *ptr, size_t size);
	void mg_free(malloc_group_t *mg);


	#ifdef MALLOC_GROUP_IMPLEMENTATION

		void *mg_malloc(malloc_group_t *mg, size_t size) {
			malloc_node_t *node = malloc(sizeof(malloc_node_t) + size);
			assert(node != NULL);
			node->data_size = size;
			node->next = mg->head;
			node->prev = NULL;
			if (node->next != NULL) node->next->prev = node;
			mg->head = node;
			return &node->data;
		}

		void *mg_realloc(malloc_group_t *mg, void *ptr, size_t size) {
			if (ptr == NULL) return mg_malloc(mg, size);
			malloc_node_t *old_node = ((malloc_node_t*) ptr)-1;
			malloc_node_t *new_node = realloc(old_node, sizeof(malloc_node_t) + size);
			assert(new_node != NULL);
			if (new_node->prev != NULL) new_node->prev->next = new_node;
			if (new_node->next != NULL) new_node->next->prev = new_node;
			if (mg->head == old_node) mg->head = new_node;
			new_node->data_size = size;
			return &new_node->data;
		}

		void mg_free(malloc_group_t *mg) {
			malloc_node_t *curr = mg->head;
			while (curr != NULL) {
				malloc_node_t *next = curr->next;
				free(curr);
				curr = next;
			}
			mg->head = NULL;
		}

	#endif
#endif
