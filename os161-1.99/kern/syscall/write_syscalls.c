#include <types.h>
#include <kern/errno.h>
#include <kern/reboot.h>
#include <kern/unistd.h>
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
#include "opt-A0.h"
#include "opt-A2.h"

// man page
// int write(int fd, const void *buf, size_t nbytes);
int sys_write(int fd, const void *buf, size_t nbytes, int32_t *retval) {
	// #define STDIN_FILENO  0      /* Standard input */
	// #define STDOUT_FILENO 1      /* Standard output */
	// #define STDERR_FILENO 2      /* Standard error */
	
	char *buf_c = (char *)buf;
	if (fd == STDOUT_FILENO){
		for (unsigned int i = 0; i < nbytes; i++){
			kprintf("%c", buf_c[i]);
		}
	} else {
		return EBADF;
	}

	(void)fd;
	(void)buf;
	(void)nbytes;
	(void)retval;
	return 0;
}
