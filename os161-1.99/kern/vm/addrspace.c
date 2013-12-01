/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <addrspace.h>
#include <vm.h>
#ifdef UW
#include <proc.h>
#endif
#if OPT_A3
#include "opt-A3.h"
#include <spl.h>
#include <mips/tlb.h>
#include <uw-vmstats.h>
#include <segments.h>
#include <pt.h>
#include <swapfile.h>
#endif


#if OPT_A3

 	/*
	 * Wrap rma_stealmem in a spinlock.
	 * based on dumb vm
	 */
	struct spinlock stealmem_lock = SPINLOCK_INITIALIZER;

	// based on dumbvm code
	paddr_t
	getppages(unsigned long npages)
	{
		paddr_t pa;
		spinlock_acquire(&stealmem_lock);
    /*
     * TODO: Eventually this needs to be replaced with properly kmallocing
     * space which can be freed later.
     */
		pa = ram_stealmem(npages);
		spinlock_release(&stealmem_lock);

		/* Check that we did get a physical address otherwise this is a problem and the os hangs silently */
		if(pa == 0){
			panic("getppages failed no memory - probably due to the sys.conf file your using; use sys161-8MB.conf \n");
		}
		return pa;
	}

	// based on dumbvm
	void
	as_zero_region(paddr_t paddr, unsigned npages)
	{
		bzero((void *)PADDR_TO_KVADDR(paddr), npages * PAGE_SIZE);
	}
#endif

/*
 * Note! If OPT_DUMBVM is set, as is the case until you start the VM
 * assignment, this file is not compiled or linked or in any way
 * used. The cheesy hack versions in dumbvm.c are used instead.
 */

struct addrspace *
as_create(void)
{
	// based on dumbvm code
	#if OPT_A3
		struct addrspace *as = kmalloc(sizeof(struct addrspace));
		if (as==NULL) {
			return NULL;
		}

		/* Adrian: For now we leave this here to support the old code as well */
		as->as_vbase1 = 0;
		as->as_pbase1 = 0;
		as->as_npages1 = 0;
		as->as_vbase2 = 0;
		as->as_pbase2 = 0;
		as->as_npages2 = 0;
		as->as_stackpbase = 0;

		/* Create the code segment */
		segment_create(&as->code, CODE);

		/* Create the data segment */
		segment_create(&as->data, DATA);

		/* Create the stack segment */
		segment_create(&as->stack, STACK);

		/* Mark the address space as not fully loaded */
		as->loaded = false;

		return as;
	#else
		struct addrspace *as;

		as = kmalloc(sizeof(struct addrspace));
		if (as == NULL) {
			return NULL;
		}

		/*
		 * Initialize as needed.
		 */

		return as;
	#endif
}

int
as_copy(struct addrspace *old, struct addrspace **ret)
{
	#if OPT_A3
		struct addrspace *new;

		new = as_create();
		if (new==NULL) {
			return ENOMEM;
		}

		new->as_vbase1 = old->as_vbase1;
		new->as_npages1 = old->as_npages1;
		new->as_vbase2 = old->as_vbase2;
		new->as_npages2 = old->as_npages2;

		// (Mis)use as_prepare_load to allocate some physical memory.
		if (as_prepare_load(new)) {
			as_destroy(new);
			return ENOMEM;
		}

		KASSERT(new->as_pbase1 != 0);
		KASSERT(new->as_pbase2 != 0);
		KASSERT(new->as_stackpbase != 0);

		memmove((void *)PADDR_TO_KVADDR(new->as_pbase1),
			(const void *)PADDR_TO_KVADDR(old->as_pbase1),
			old->as_npages1*PAGE_SIZE);

		memmove((void *)PADDR_TO_KVADDR(new->as_pbase2),
			(const void *)PADDR_TO_KVADDR(old->as_pbase2),
			old->as_npages2*PAGE_SIZE);

		memmove((void *)PADDR_TO_KVADDR(new->as_stackpbase),
			(const void *)PADDR_TO_KVADDR(old->as_stackpbase),
			STACKPAGES*PAGE_SIZE);

		*ret = new;
		return 0;
	#else
		struct addrspace *newas;

		newas = as_create();
		if (newas==NULL) {
			return ENOMEM;
		}

		/*
		 * Write this.
		 */

		(void)old;

		*ret = newas;
		return 0;
	#endif
}

void
as_destroy(struct addrspace *as)
{
	/*
	 * Clean up as needed.
	 */

	kfree(as);
}

void
as_activate(void)
{
	// based on dumb vm code
	#if OPT_A3
		int i, spl;
		struct addrspace *as;

		as = curproc_getas();
		#ifdef UW
		        /* Kernel threads don't have an address spaces to activate */
		#endif
		if (as == NULL) {
			return;
		}

		/* Disable interrupts on this CPU while frobbing the TLB. */
		spl = splhigh();

		for (i=0; i<NUM_TLB; i++) {
			tlb_write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
		}

		/* track stats for tlb invalidations */
		vmstats_inc(VMSTAT_TLB_INVALIDATE);

		splx(spl);
	#else
		struct addrspace *as;

		as = curproc_getas();
		if (as == NULL) {
			/*
			 * Kernel thread without an address space; leave the
			 * prior address space in place.
			 */
			return;
		}

		/*
		 * Write this.
		 */
	#endif
}

void
#ifdef UW
as_deactivate(void)
#else
as_dectivate(void)
#endif
{
	/*
	 * Write this. For many designs it won't need to actually do
	 * anything.
	 */
}

/*
 * Set up a segment at virtual address VADDR of size MEMSIZE. The
 * segment in memory extends from VADDR up to (but not including)
 * VADDR+MEMSIZE.
 *
 * The READABLE, WRITEABLE, and EXECUTABLE flags are set if read,
 * write, or execute permission should be set on the segment. At the
 * moment, these are ignored. When you write the VM system, you may
 * want to implement them.
 */
int
as_define_region(struct addrspace *as, vaddr_t vaddr, size_t sz,
		 int readable, int writeable, int executable)
{
	// based on dumbvm code
	#if OPT_A3
		size_t npages;

		/* Align the region. First, the base... */
		sz += vaddr & ~(vaddr_t)PAGE_FRAME;
		vaddr &= PAGE_FRAME;

		/* ...and now the length. */
		sz = (sz + PAGE_SIZE - 1) & PAGE_FRAME;

		npages = sz / PAGE_SIZE;

		/* We don't use these - all pages are read-write */
		(void)readable;
		(void)writeable;
		(void)executable;

		/* Adrian: For now we leave the old code here as well */
		if (as->as_vbase1 == 0 && as->code.vbase == 0) {
			/* Setup the segment */
			as->as_vbase1 = vaddr;
			as->as_npages1 = npages;
            as->code.npages = npages;

            /* Prepare the page table now that we have the npages needed */
			as->code.ptable = segment_prepare(&as->code);
			as->code.vbase = vaddr;

			return 0;
		}

		/* Adrian: For now we leave the old code here as well */
		if (as->as_vbase2 == 0 && as->data.vbase == 0) {
			/* Setup the segment */
			as->as_vbase2 = vaddr;
			as->as_npages2 = npages;
            as->data.npages = npages;

            /* Prepare the page table now that we have the npages needed */
			as->data.ptable = segment_prepare(&as->data);
			as->data.vbase = vaddr;

			return 0;
		}

		/*
		 * Support for more than two regions is not available.
		 */
		kprintf("dumbvm: Warning: too many regions\n");
		return EUNIMP;
	#else
		/*
		 * Write this.
		 */

		(void)as;
		(void)vaddr;
		(void)sz;
		(void)readable;
		(void)writeable;
		(void)executable;
		return EUNIMP;
	#endif
}

int
as_prepare_load(struct addrspace *as)
{
	// based on dumbvm code
	#if OPT_A3
		(void)as;
		return 0;
	#else
		/*
		 * Write this.
		 */

		(void)as;
		return 0;
	#endif
}

int
as_complete_load(struct addrspace *as)
{
	#if OPT_A3
		/* Mark the address space as fully loaded */
		as->loaded = true;
	#endif

	(void)as;
	return 0;
}

int
as_define_stack(struct addrspace *as, vaddr_t *stackptr)
{
	/*
	 * Write this.
	 */
	 #if OPT_A3
	 	// KASSERT(as->as_stackpbase != 0);
	 	// *stackptr = USERSTACK;
	 #endif

	(void)as;

	/* Initial user-level stack pointer */
	*stackptr = USERSTACK;

	return 0;
}

