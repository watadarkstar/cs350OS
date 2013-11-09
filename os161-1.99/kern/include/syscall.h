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

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "opt-A2.h"

struct trapframe; /* from <machine/trapframe.h> */

/*
 * The system call dispatcher.
 */

void syscall(struct trapframe *tf);

/*
 * Support functions.
 */

/* Helper for fork(). You write this. */
#if OPT_A2
void enter_forked_process(void *tf, unsigned long as);
#else
void enter_forked_process(struct trapframe *tf);
#endif

/* Enter user mode. Does not return. */
void enter_new_process(int argc, userptr_t argv, vaddr_t stackptr,
		       vaddr_t entrypoint);


/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);
int sys___time(userptr_t user_seconds, userptr_t user_nanoseconds);

#if OPT_A2
/* A2 Functions */
// ------------------------------------------------------------------

// MAN file:
// int open(const char *filename, int flags);
// int open(const char *filename, int flags, int mode);
int sys_open(userptr_t filename, int flags, int mode, int32_t *retval);
void init_STD(void); //Aaron - Used to initilize STDIN, STDOUT and STDERR. Can't attach it to thread_create, unfortunately. 
int sys_close(int fd);
int sys_write(int fd, const void *buf, size_t nbytes, int32_t *retval);
int sys_read(int fd, void * buf, size_t buflen, int32_t *retval);
int sys_execv(char *program, char **args);
void sys__exit(int code);
int sys_getpid(int32_t *retval);
int sys_fork(struct trapframe *tf, int32_t *retval);

// ------------------------------------------------------------------
#endif

#endif /* _SYSCALL_H_ */
