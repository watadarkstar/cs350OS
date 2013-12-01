#include <segments.h>
#include <pt.h>
#include <vm.h>
#include <addrspace.h>

void segment_create(struct segment *seg, segment_t tsegment){
	seg->ptable = NULL;
	seg->vbase = 0;
	seg->npages = 0;
	seg->tsegment = tsegment;

	/* If its a stack segment then we can initialize its pages base on the PAGE_SIZE */
	if(tsegment == STACK){
		seg->npages = STACKPAGES;
        seg->ptable = segment_prepare(seg);
		seg->vbase = USERSTACK - seg->npages*PAGE_SIZE;
	}
}

struct pte *
segment_prepare(struct segment *seg){
	/* Ensure that the segment has been setup properly */
	KASSERT(seg->npages != 0);

	/* Allocate space for the page table based on the number of pages needed */
	seg->ptable = kmalloc(sizeof(struct pte) * seg->npages);
	for(unsigned int i = 0; i<seg->npages; i++){ pte_create(&seg->ptable[i]); }
	return seg->ptable;
}

paddr_t 
segment_lookup(struct segment *seg, vaddr_t vaddr) {
	int vpn = (vaddr - seg->vbase) / PAGE_SIZE;
	struct pte *pte = &seg->ptable[vpn];
	paddr_t paddr;

	/* 
	 	We have two cases: 
	 	(1) pte did not exist and we need to load a new pte
	 	(2) pte already existed and we just reload the page table entry
	*/
	if(pte->valid == 0){
		paddr = pte_load(&seg->ptable[vpn]);
		return paddr;
	} else {
		paddr = pte_reload(&seg->ptable[vpn], (vaddr - seg->vbase) % PAGE_SIZE);
		return paddr;
	}
}

