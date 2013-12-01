#include <coremap.h>

struct pte ** cm_pages;

void coremap_init(paddr_t first, paddr_t last){
	__u32 total_pages = (last + PAGE_SIZE-1)/PAGE_SIZE;
	__u32 free_pages = (last-first + PAGE_SIZE-1)/PAGE_SIZE;
	__u32 kernel_pages = total_pages - free_pages;
	if(PADDR_TO_KVADDR(first) == 0){
		panic("Physical address is 0. Oh noes");
	}
	cm_pages = (struct pte**)PADDR_TO_KVADDR(first);
	//paddr_t kernel_addr = first + pages * sizeof(struct pte);
	
	struct pt ** pages_iter = cm_pages;
	paddr_t address_iter = first;
	size_t size = sizeof(struct pte);
	
	for(__u32 i =0; i < kernel_pages; i++){
		pages_iter->paddr = address_iter;
		pages_iter->valid = 1;
		pages_iter->dirty = 0;
		pages_iter++;
		address_iter+=size;
	}
	
	for(__u32 i =0; i < free_pages; i++){
		pages_iter->paddr = address_iter;
		pages_iter->valid = 1;
		pages_iter->dirty = 1;
		pages_iter++;
		address_iter+=size;
	}
}

