/*
 * License: MIT
 *
 * A tiny header-only library for scoped heap allocation by treating each
 * allocated chunk of memory as a node in a singly linked list.
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
 * 	int *f = mg_malloc(mg, sizeof(float));
 * 	int *str = mg_malloc(mg, sizeof(char) * 81);
 * 
 * 	mg_free(mg); // Everything gets freed here.
 * }
 * 
*/

#ifndef MALLOC_GROUP_HEADER
#define MALLOC_GROUP_HEADER

	#include <stdio.h>
	#include <stdlib.h>
	#include <assert.h>

	typedef struct malloc_node {
		struct malloc_node *next;
		size_t data_size;
		unsigned char data[];
	} malloc_node_t;

	typedef struct malloc_group {
		malloc_node_t *head;
	} malloc_group_t;

	#define MG_SCOPED &((malloc_group_t){NULL})

	void *mg_malloc(malloc_group_t *mg, size_t size);
	void mg_free(malloc_group_t *mg);


	#ifdef MALLOC_GROUP_IMPLEMENTATION

		void *mg_malloc(malloc_group_t *mg, size_t size) {
			malloc_node_t *node = malloc(sizeof(malloc_node_t) + size);

			assert(node != NULL);

			node->data_size = size;
			node->next = mg->head;

			mg->head = node;

			return &node->data;
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
