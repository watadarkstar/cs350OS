#ifndef _PT_H_
#define _PT_H_

#include "lib.h"

struct pte {
	paddr_t paddr;
  	vaddr_t vaddr;
  	int valid;
};
struct pt {
	struct pte *ptes;
};

void pt * pt_create(struct pt * pt);

#endif
