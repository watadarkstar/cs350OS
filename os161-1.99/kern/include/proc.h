/*
 * Copyright (c) 2013
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

#ifndef _PROC_H_
#define _PROC_H_

/*
 * Definition of a process.
 *
 * Note: curproc is defined by <current.h>.
 */

#include <spinlock.h>
#include <thread.h> /* required for struct threadarray */
#include <wchan.h>
#include <synch.h>
#include "opt-A2.h"

struct addrspace;
struct vnode;

/*
 * Process structure.
 */
struct proc {
	char *p_name;			/* Name of this process */
	struct spinlock p_lock;		/* Lock for this structure */
	struct threadarray p_threads;	/* Threads in this process */

	/* VM */
	struct addrspace *p_addrspace;	/* virtual address space */

	/* VFS */
	struct vnode *p_cwd;		/* current working directory */

	/* add more material here as needed */
	#if OPT_A2
	pid_t pid;
	struct fd * p_fdlist[__OPEN_MAX];
	#endif
};

//Couldn't find a better place to put this
// -Aaron
typedef struct fd {
	int fd_fid;
	char fd_name[__PATH_MAX+1];
	int fd_flag;
	struct vnode* fd_vfile;
}fd;

typedef struct pd {
	pid_t pd_parent_pid;
	pid_t pd_pid;
	int pd_exitcode;
	bool pd_exiting;
	struct cv * pd_cv;
	struct lock * pd_lock;
} pd;

/* This is the process structure for the kernel and for kernel-only threads. */
extern struct proc *kproc;

/* Call once during system startup to allocate data structures. */
void proc_bootstrap(void);

/* Create a fresh process for use by runprogram(). */
struct proc *proc_create_runprogram(const char *name);

/* Destroy a process. */
void proc_destroy(struct proc *proc);

/* Attach a thread to a process. Must not already have a process. */
int proc_addthread(struct proc *proc, struct thread *t);

/* Detach a thread from its process. */
void proc_remthread(struct thread *t);

/* Fetch the address space of the current process. */
struct addrspace *curproc_getas(void);

/* Change the address space of the current process, and return the old one. */
struct addrspace *curproc_setas(struct addrspace *);

#if OPT_A2
/* Initialize global process array */
void proc_array_init(void);
//Releases pids, so it can be used again
void release_pid(pid_t pid);
//Sets parent pid on child proc
void set_parent(pid_t parent, pid_t child);
//Checks for valid pid
int valid_pid(pid_t pid);
//Checks if an exit code exists
bool has_exit_code(pid_t pid);
//Returns exit code
int get_exit_code(pid_t pid);
//Sets exit code
void set_exit_code(int exit_code);
// Puts parent thread to sleep. Goodnight
void put_to_sleep(pid_t pid);
// Wakes parents up once child is done
void wake_up(pid_t pid);
void release_lock(pid_t pid);
void acquire_lock(pid_t pid);

/* Exposing proc_create for the sake of sys_fork */
struct proc *proc_create(const char *name);
#endif


#endif /* _PROC_H_ */
