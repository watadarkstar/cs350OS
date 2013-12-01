#ifndef _SEGMENTS_H_
#define _SEGMENTS_H_

#include <types.h>
#include "pt.h"

struct segment{
	struct pte *ptable;
	vaddr_t vbase;
	size_t npages;
};

void segment_create(struct segment *seg);
struct pte * segment_prepare(struct segment *seg);
paddr_t segment_translate(struct segment *seg, vaddr_t vaddr);

#endif
