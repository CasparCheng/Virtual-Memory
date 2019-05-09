#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

/* LRU queue node structure */
typedef struct _node_t {
	int frame;             /* frame number */
	struct _node_t *prev;  /* previous node */
	struct _node_t *next;  /* next node */
} node_t;

static node_t *head_of_lru_queue;  /* head of LRU queue */
static node_t *tail_of_lru_queue;  /* tail of LRU queue */

static node_t **lookup_table;  /* lookup table of nodes */

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	/* O(1) in time
	   O(1) in space
	*/

	/* Evict from the head of LRU queue */

	if (!head_of_lru_queue) {
		fprintf(stderr, "No LRU records found\n");
		exit(1);
	}

	int frame = head_of_lru_queue->frame;  /* the victim frame */
	node_t *node = head_of_lru_queue;      /* node related to frame */

	/* Replace head of LRU queue */
	head_of_lru_queue = node->next;
	free(node);
	/* Clear node pointer in records */
	lookup_table[frame] = NULL;

	/* Relink other nodes */
	if (head_of_lru_queue) {
		/* non-empty LRU queue */
		head_of_lru_queue->prev = NULL;
	} else {
		/* empty LRU queue */
		tail_of_lru_queue = NULL;
	}

	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	/* O(1) in time
	   O(1) in space
	*/

	/* Insert to the tail of LRU queue */

	int frame = p->frame >> PAGE_SHIFT;  /* frame number */
	node_t *node = lookup_table[frame];  /* node related to frame */

	if (node) {
		/* frame in LRU queue */
		if (node == tail_of_lru_queue) {
			/* Do nothing here */

			if (debug) {
				printf("\tLRU queue not changed\n");
			}

			return;
		}
		/* Relink other nodes */
		if (node->prev) {
			node->prev->next = node->next;
			node->next->prev = node->prev;
		} else {
			head_of_lru_queue = node->next;
			head_of_lru_queue->prev = NULL;
		}
	} else {
		/* frame not in LRU queue */
		node = (node_t *)malloc(sizeof(node_t));
		if (!node) {
			perror("malloc");
			exit(1);
		}
		/* Record node pointer */
		lookup_table[frame] = node;
		node->frame = frame;
	}

	/* Link node */
	if (tail_of_lru_queue) {
		/* non-empty LRU queue */
		tail_of_lru_queue->next = node;
		node->prev = tail_of_lru_queue;
		tail_of_lru_queue = node;
	} else {
		/* empty LRU queue */
		head_of_lru_queue = tail_of_lru_queue = node;
		node->prev = NULL;
	}
	node->next = NULL;

	if (debug) {
		node = head_of_lru_queue;
		printf("\tLRU queue:");
		while (node) {
			printf(" %d", node->frame);
			node = node->next;
		}
		printf("\n");
	}

	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	/* O(M) in time
	   O(M) in space
	*/

	/* Initialize the head and tail of LRU queue */
	head_of_lru_queue = NULL;
	tail_of_lru_queue = NULL;
	/* Initialize the lookup table of nodes */
	lookup_table = (node_t **)calloc(memsize, sizeof(node_t *));
}
