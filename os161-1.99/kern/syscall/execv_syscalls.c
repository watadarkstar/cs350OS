#include <types.h>
#include <kern/errno.h>
#include <kern/reboot.h>
#include <kern/unistd.h>
#include <kern/fcntl.h>
#include <lib.h>
#include <spl.h>
#include <clock.h>
#include <thread.h>
#include <proc.h>
#include <current.h>
#include <synch.h>
#include <vm.h>
#include <mainbus.h>
#include <vfs.h>
#include <device.h>
#include <syscall.h>
#include <test.h>
#include <version.h>
#include "autoconf.h"  // for pseudoconfig
#include <addrspace.h>
#include "opt-A0.h"
#include "opt-A2.h"

#if OPT_A2
#include <copyinout.h>
#endif

#if OPT_A2
// man page
// int execv(const char *program, char **args);
int sys_execv(char *progname, char **argv){
	struct addrspace *as;
	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result;
	unsigned long argc = 0;

	/* Open the file. */
	result = vfs_open(progname, O_RDONLY, 0, &v);
	if (result) {
		return result;
	}

	/* Destroy the current address space */
	as_deactivate();
	as = curproc_setas(NULL);
	as_destroy(as);

	/* We should be a new process. */
	KASSERT(curproc_getas() == NULL);

	/* Create a new address space. */
	as = as_create();
	if (as ==NULL) {
		vfs_close(v);
		return ENOMEM;
	}

	/* Switch to it and activate it. */
	curproc_setas(as);
	as_activate();

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* p_addrspace will go away when curproc is destroyed */
		vfs_close(v);
		return result;
	}

	/* Done with the file now. */
	vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(as, &stackptr);
	if (result) {
		/* p_addrspace will go away when curproc is destroyed */
		return result;
	}

	// we need to figure out what argc would be
	for(unsigned long i = 0; argv[i] != NULL; i++){
		argc = i + 1;
	}

	#if OPT_A2
		// How much space do we need for args in stack?
		// We need space for each pointer (4argc)
		// We need space for each character, including NULL termination, plus
		// padding to make multiples of 4
		int stringSpace = 0;
		for (unsigned long i = 0; i < argc; i++) {
			stringSpace += strlen(argv[i]) + 1;
		}

		// Align stack pointer to an 8-byte alignment
		while ((stackptr - (4*argc) - stringSpace) % 8 != 0) {
			stackptr--;
		}

		// Use a vaddr array to track the addresses the strings end up in
		// One bigger than argc to also have the pointer to the final NULL
		// value
		vaddr_t stringAddr[argc+1];

		// Copy argument strings onto stack
		// Array must end with NULL pointer, so do that first
		stackptr -= 4;
		copyout((void *)NULL, (userptr_t)stackptr, (size_t)4);
		stringAddr[argc] = stackptr;
		for (int i = argc-1; i >= 0; i--) {
			stackptr -= strlen(argv[i]) + 1;
			while (stackptr % 4 != 0) stackptr--;
			copyoutstr(argv[i], (userptr_t)stackptr, (size_t)strlen(argv[i]), NULL);
			stringAddr[i] = stackptr;
		}

		// Now use the stored addresses of the string to add the appropriate
		// pointers to the stack
		for (int i = argc; i >= 0; i--) {
			stackptr -= sizeof(vaddr_t);
			copyout(&stringAddr[i], (userptr_t)stackptr, sizeof(vaddr_t));
		}

		/* Warp to user mode. */
		enter_new_process(argc /*argc*/, (userptr_t)stackptr /*userspace addr of argv*/,
				  stackptr, entrypoint);
	#else
		/* Warp to user mode. */
		enter_new_process(0 /*argc*/, NULL /*userspace addr of argv*/,
				  stackptr, entrypoint);
	#endif

	/* enter_new_process does not return. */
	panic("enter_new_process returned\n");
	return EINVAL;

	(void)progname;
	(void)argv;
}
#endif
