#ifndef _PT_H_
#define _PT_H_

#include "vm.h"
#include "types.h"
#include "lib.h"
#include "segments.h"

struct pte {
	paddr_t paddr;
  	int valid;
	int dirty;
};

void pte_create(struct pte * pte);

#endif
