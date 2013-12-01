#ifndef __COREMAP__
#define __COREMAP__

#include "types.h"
#include "lib.h"
#include <addrspace.h>

void coremap_init(void);
paddr_t coremap_alloc_contigous(unsigned long);
void coremap_free_segments(struct addrspace *);
void coremap_free_segment(struct segment *);
void coremap_free(vaddr_t);

#endif
