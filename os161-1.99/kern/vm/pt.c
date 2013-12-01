#include <types.h>
#include <lib.h>
#include <addrspace.h>
#include <pt.h>

void
pte_create(struct pte *pte) {
	pte->valid = 0;
}

paddr_t pte_load(struct pte *pte){
	/* Obtain a paddr for the newly loaded page */
	paddr_t paddr = getppages(1);
	pte->paddr = paddr;
	pte->valid = 1;
	return paddr;
}

paddr_t pte_reload(struct pte *pte, int offset){
	/* Translate the address based on the offset */
	paddr_t paddr = pte->paddr + offset;
	return paddr;
}
