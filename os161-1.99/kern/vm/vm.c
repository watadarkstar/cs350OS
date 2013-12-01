
#ifdef UW
/* This was added just to see if we can get things to compile properly and
 * to provide a bit of guidance for assignment 3 */

#include "opt-vm.h"
#if OPT_VM

#include <types.h>
#include <lib.h>
#include <addrspace.h>
#include <vm.h>

#if OPT_A3
#include <addrspace.h>
#include "opt-A3.h"
#include <kern/errno.h>
#include <proc.h>
#include <current.h>
#include <spl.h>
#include <mips/tlb.h>
#include <uw-vmstats.h>
#include <syscall.h>
#endif

void
vm_bootstrap(void)
{
	/* May need to add code. */
	#if OPT_A3
		vmstats_init();
	#endif
}

#if 0 
/* You will need to call this at some point */
static
paddr_t
getppages(unsigned long npages)
{
   /* Adapt code form dumbvm or implement something new */
	 (void)npages;
	 panic("Not implemented yet.\n");
   return (paddr_t) NULL;
}
#endif

#if OPT_A3

static unsigned int next_victim = 0;// we probably want a lock for this
/* Round robin function for A3 */
int
tlb_get_rr_victim() 
{
	int victim;
	victim = next_victim;
	next_victim = (next_victim + 1) % NUM_TLB;
	return victim;
}

#endif

/* Allocate/free some kernel-space virtual pages */
vaddr_t 
alloc_kpages(int npages)
{
	// based on dumbvm
	#if OPT_A3
		paddr_t pa;
		pa = getppages(npages);
		if (pa==0) {
			return 0;
		}
		return PADDR_TO_KVADDR(pa);
	#else
		/* Adapt code form dumbvm or implement something new */
		(void)npages;
		panic("Not implemented yet.\n");
		return (vaddr_t) NULL;
	#endif
}

void 
free_kpages(vaddr_t addr)
{
	/* nothing - leak the memory. */

	(void)addr;
}

void
vm_tlbshootdown_all(void)
{
	panic("Not implemented yet.\n");
}

void
vm_tlbshootdown(const struct tlbshootdown *ts)
{
	(void)ts;
	panic("Not implemented yet.\n");
}

int
vm_fault(int faulttype, vaddr_t faultaddress)
{
	#if OPT_A3
		vaddr_t vbase1, vtop1, vbase2, vtop2, stackbase, stacktop;
		paddr_t paddr;
		int i;
		uint32_t ehi, elo;
		struct addrspace *as;
		int spl;
		int victim;
		bool readonly = false; 

		faultaddress &= PAGE_FRAME;

		DEBUG(DB_VM, "dumbvm: fault: 0x%x\n", faultaddress);

		switch (faulttype) {
		    case VM_FAULT_READONLY:
				kprintf("VM_FAULT_READONLY - exiting...\n");
				sys__exit(0);
		    case VM_FAULT_READ:
		    case VM_FAULT_WRITE:
			break;
		    default:
			return EINVAL;
		}

		if (curproc == NULL) {
			/*
			 * No process. This is probably a kernel fault early
			 * in boot. Return EFAULT so as to panic instead of
			 * getting into an infinite faulting loop.
			 */
			return EFAULT;
		}

		as = curproc_getas();
		if (as == NULL) {
			/*
			 * No address space set up. This is probably also a
			 * kernel fault early in boot.
			 */
			return EFAULT;
		}

		/* Assert that the address space has been set up properly. */
		KASSERT(as->code.vbase != 0);
		KASSERT(as->code.npages != 0);
		KASSERT(as->data.vbase != 0);
		KASSERT(as->data.npages != 0);
		KASSERT(as->stack.vbase != 0);
		KASSERT((as->code.vbase & PAGE_FRAME) == as->code.vbase);
		KASSERT((as->code.vbase & PAGE_FRAME) == as->code.vbase);
		KASSERT((as->stack.vbase & PAGE_FRAME) == as->stack.vbase);

		vbase1 = as->code.vbase;
		vtop1 = vbase1 + as->code.npages * PAGE_SIZE;
		vbase2 = as->data.vbase;
		vtop2 = vbase2 + as->data.npages * PAGE_SIZE;
		stackbase = USERSTACK - DUMBVM_STACKPAGES * PAGE_SIZE;
		stacktop = USERSTACK;

		if (faultaddress >= vbase1 && faultaddress < vtop1) {
			if(as->loaded) readonly = true;
			// paddr = (faultaddress - vbase1) + as->as_pbase1;
			// kprintf("CODE %d\n", faultaddress);
			paddr = segment_translate(&as->code, faultaddress);
		}
		else if (faultaddress >= vbase2 && faultaddress < vtop2) {
			// paddr = (faultaddress - vbase2) + as->as_pbase2;
			// kprintf("DATA %d\n", faultaddress);
			paddr = segment_translate(&as->data, faultaddress);
		}
		else if (faultaddress >= stackbase && faultaddress < stacktop) {
			// paddr = (faultaddress - stackbase) + as->as_stackpbase;
			// kprintf("STACK%d\n", faultaddress);
			paddr = segment_translate(&as->stack, faultaddress);
		}
		else {
			return EFAULT;
		}

		/* Disable interrupts on this CPU while frobbing the TLB. */
		spl = splhigh();

		/* New TLB Management stuff for victim eviction */
		victim = tlb_get_rr_victim();

		/* make sure it's page-aligned */
		KASSERT((paddr & PAGE_FRAME) == paddr);

		/* track stats for tlb fault */
		vmstats_inc(VMSTAT_TLB_FAULT);

		/* Adrian: This checks for invalid entries and writes the first invalid entry found */
        for (i=0; i<NUM_TLB; i++) {
                tlb_read(&ehi, &elo, i);
                if (elo & TLBLO_VALID) {
                        continue;
                }
                ehi = faultaddress;
                if (readonly) {
                        elo = paddr | TLBLO_VALID;
                } else {
                        elo = paddr | TLBLO_DIRTY | TLBLO_VALID;
                }
                DEBUG(DB_VM, "dumbvm: 0x%x -> 0x%x\n", faultaddress, paddr);
                tlb_write(ehi, elo, i);

                // track stats for tlb fault free 
                vmstats_inc(VMSTAT_TLB_FAULT_FREE);
                
                splx(spl);
                return 0;
        } 

        /* Adrian: In the case that there are no invalid entries we must evict one and replace it */
        /* We use the round robin method to choose our victim to evict */
        ehi = faultaddress;
        if (readonly) {
                elo = paddr | TLBLO_VALID;
        } else {
                elo = paddr | TLBLO_DIRTY | TLBLO_VALID;
        }
        i = tlb_get_rr_victim();
        tlb_write(ehi, elo, i);

        /* track stats for tlb fault replace */
        vmstats_inc(VMSTAT_TLB_FAULT_REPLACE);

        splx(spl);
		return 0;
	#else
		/* Adapt code form dumbvm or implement something new */
		(void)faulttype;
		(void)faultaddress;
		panic("Not implemented yet.\n");
		return 0;
	#endif
}
#endif /* OPT_VM */

#endif /* UW */

