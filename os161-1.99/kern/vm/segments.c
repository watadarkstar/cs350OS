#include <segments.h>
#include <pt.h>
#include <vm.h>
#include <addrspace.h>

void segment_create(struct segment *seg){
	seg->vbase = 0;
	seg->npages = 0;
	seg->ptable = NULL;
}

struct pte *
segment_prepare(struct segment *seg){
	KASSERT(seg->npages != 0);
	seg->ptable = kmalloc(sizeof(struct pte) * seg->npages);
	for(unsigned int i = 0; i<seg->npages; i++){
		pte_create(&seg->ptable[i]);
	}
	return seg->ptable;
}

paddr_t 
segment_translate(struct segment *seg, vaddr_t vaddr) {
	int offset = (vaddr - seg->vbase) % PAGE_SIZE;
	int vpn = (vaddr - seg->vbase) / PAGE_SIZE;
	struct pte *pte = &seg->ptable[vpn];
	paddr_t paddr;

	if(pte->valid == 0){
		paddr = getppages(1);
		seg->ptable[vpn].paddr = paddr;
		seg->ptable[vpn].valid = 1;
		// kprintf("not found: %d\n", paddr);
		return paddr;
	} else {
		paddr = pte->paddr + offset;
		// kprintf("found: %d\n", paddr);
		return paddr;
	}
}

