#ifndef _SEGMENTS_H_
#define _SEGMENTS_H_

#include <types.h>
#include "pt.h"

/* the different types of segments */
typedef enum {
	CODE,
	STACK,
	DATA
} segment_t;

/* the size of our stack based on dumbvm, always have 48k of user stack */
#define STACKPAGES    12

/* Three types of segments: code, data and stack */
struct segment {
	vaddr_t vbase;
	size_t npages;
	struct pte *ptable;
	segment_t tsegment;
};

/* Creates a segment but it is not fully prepared */
void segment_create(struct segment *seg, segment_t tsegment);

/* Initializes the page table for the segment */
struct pte * segment_prepare(struct segment *seg);

/* Looksup a vaddr and finds the paddr based on the segment */
paddr_t segment_lookup(struct segment *seg, vaddr_t vaddr, bool dirty);

#endif
