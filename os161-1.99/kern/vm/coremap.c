#include <coremap.h>
#include <types.h>
#include <lib.h>
#include <vm.h>
#include <pt.h>

struct pte * cm_pages;
__u32 len;

void coremap_init(paddr_t first, paddr_t last){
	__u32 total_pages = (last + PAGE_SIZE-1)/PAGE_SIZE;
	__u32 free_pages = (last-first + PAGE_SIZE-1)/PAGE_SIZE;
	__u32 kernel_pages = total_pages - free_pages;
	(void)kernel_pages;
	len = free_pages;
	if(PADDR_TO_KVADDR(first) == 0){
		panic("Physical address is 0. Oh noes");
	}
	cm_pages = (struct pte*)PADDR_TO_KVADDR(first);

	for(__u32 i =0; i < free_pages; i++){
		cm_pages[i].paddr = first + PAGE_SIZE * i;
		cm_pages[i].vaddr = PADDR_TO_KVADDR(first + PAGE_SIZE*i);
		cm_pages[i].valid = 1;
		cm_pages[i].dirty = 1;
		cm_pages[i].free = 1;
	}
}

paddr_t coremap_alloc_contigous(unsigned long pages){
	
	__u32 i = 0;
	while (i < len){
		if(cm_pages[i].free){
			__u32 incremenet = 1;
			while(cm_pages[increment+i].free && increment + i < pages){
				increment++;
			}
			if(increment+i == pages){
				return cm_pages[i].paddr;
			}
		}
	}
	
	//Basic case, doesn'tcover when it cant find contigous
	return 0;		
}

void coremap_free(vaddr_t va){
	
	for(__u32 i =0; i < len; i++){
		if(cm_pages[i].vaddr == va){
			cm_pages[i].free = 1;
		}
	}
}