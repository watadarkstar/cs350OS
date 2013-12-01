#include <coremap.h>
#include <types.h>
#include <lib.h>
#include <vm.h>
#include <pt.h>

struct pte * cm_pages;

void coremap_init(paddr_t first, paddr_t last){
	__u32 total_pages = (last + PAGE_SIZE-1)/PAGE_SIZE;
	__u32 free_pages = (last-first + PAGE_SIZE-1)/PAGE_SIZE;
	__u32 kernel_pages = total_pages - free_pages;
	if(PADDR_TO_KVADDR(first) == 0){
		panic("Physical address is 0. Oh noes");
	}
	cm_pages = (struct pte*)PADDR_TO_KVADDR(first);
	//paddr_t kernel_addr = first + pages * sizeof(struct pte);

	for(__u32 i =0; i < free_pages; i++){
		cm_pages[i].paddr = kernel_pages + PAGE_SIZE * i;
		cm_pages[i].valid = 1;
		cm_pages[i].dirty = 1;
	}
}

