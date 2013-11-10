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
// int exit(int code)
void sys__exit(int code){
	// struct proc *proc_temp = curthread->t_proc;
	// proc_remthread(curthread);
	// proc_destroy(proc_temp);
	/* This causes the err in ftest */
	set_exit_code(code);
	acquire_lock(curproc->pid);
	wake_up(curproc->pid);
	thread_exit();
	(void)code;
}
