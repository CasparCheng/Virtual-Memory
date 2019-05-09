#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

static int head_of_fifo_queue;  /* head of FIFO queue */

/* Page to evict is chosen using the fifo algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int fifo_evict() {
	/* O(1) in time
	   O(1) in space
	*/

	int frame = head_of_fifo_queue;  /* the chosen victim frame */

	if (debug) {
		printf("\tFIFO queue head: %d\n", head_of_fifo_queue);
	}

	/* Increment the head of FIFO queue, wrap it on boundary */
	head_of_fifo_queue = (head_of_fifo_queue + 1) % memsize;

	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the fifo algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void fifo_ref(pgtbl_entry_t *p) {
	/* O(1) in time
	   O(1) in space
	*/

	/* Do nothing here */
	return;
}

/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void fifo_init() {
	/* O(1) in time
	   O(1) in space
	*/

	/* Initialize the head of FIFO queue */
	head_of_fifo_queue = 0;
}
