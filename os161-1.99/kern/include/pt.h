#ifndef _PT_H_
#define _PT_H_

#include "vm.h"
#include "types.h"
#include "lib.h"
#include "segments.h"

#define PT_SIZE 1024

struct pte {
	// vaddr -> paddr
	paddr_t paddr;
  	int valid;
};

void pte_create(struct pte * pte);

#endif
