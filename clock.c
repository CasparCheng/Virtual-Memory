#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int clock_hand;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	/* O(M) in time
	   O(1) in space
	*/

	int frame;  /* the chosen victim frame */

	/* loop to rotate clock hand */
	while (1) {
		if (coremap[clock_hand].pte->frame & PG_CLKREF) {
			/* clkref-bit is set */
			/* Clear clkref-bit of page */
			coremap[clock_hand].pte->frame &= ~PG_CLKREF;

			if (debug) {
				printf("\tclock_hand (clr refbit): %d\n", clock_hand);
			}

			/* Increment clock hand */
			clock_hand = (clock_hand + 1) % memsize;
		} else {
			/* clkref-bit is not set */
			/* Take the frame pointed by clock hand */
			frame = clock_hand;

			if (debug) {
				printf("\tclock_hand (chose): %d\n", clock_hand);
			}

			/* Increment clock hand once more */
			clock_hand = (clock_hand + 1) % memsize;
			break;
		}
	}

	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	/* O(1) in time
	   O(1) in space
	*/

	/* Set clkref-bit of page */
	p->frame |= PG_CLKREF;

	if (debug) {
		printf("\tclock_hand (set refbit): %d\n", p->frame >> PAGE_SHIFT);
	}

	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	/* O(1) in time
	   O(1) in space
	*/

	/* Initialize clock hand */
	clock_hand = 0;
}
