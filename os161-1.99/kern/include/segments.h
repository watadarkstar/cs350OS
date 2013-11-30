#ifndef _SEGMENTS_H_
#define _SEGMENTS_H_

#include <types.h>
#include "pt.h"

struct segment{
	vaddr_t vbase;
	size_t npages;
	struct pte *ptes;
};

struct pte * ptes_create(struct segment *);

void segment_add(struct segment *seg, vaddr_t vaddr, paddr_t paddr);
void segment_create(struct segment *seg);
void segment_destroy(struct segment *seg);
paddr_t segment_translate(struct segment *seg, vaddr_t vaddr);

#endif
