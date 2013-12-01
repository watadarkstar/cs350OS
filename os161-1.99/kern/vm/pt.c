#include <types.h>
#include <lib.h>
#include <addrspace.h>
#include <pt.h>

void
pte_create(struct pte *pte) {
	pte->valid = 0;
}

