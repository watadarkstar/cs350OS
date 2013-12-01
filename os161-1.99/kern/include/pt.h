#ifndef _PT_H_
#define _PT_H_

#include "vm.h"
#include "types.h"
#include "lib.h"
#include "segments.h"

struct pte {
  	int valid;
	paddr_t paddr;
};

/* Creates a page table entry */
void pte_create(struct pte * pte);

/* Load a page table entry by creating a paddr for it */
paddr_t pte_load(struct pte * pte);

/* Reload the page table entry by translating its paddr */
paddr_t pte_reload(struct pte * pte, int offset);

#endif
