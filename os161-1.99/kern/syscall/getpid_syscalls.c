#include <types.h>
#include <thread.h>
#include <proc.h>
#include <current.h>
#include <vfs.h>
#include <syscall.h>
#include <copyinout.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/fcntl.h>

#include "opt-A2.h"

#if OPT_A2
/*
  Returns process id of the current process
*/
 pid_t
 sys_getpid(int32_t *retval) {
    *retval = curproc->pid;
    return 0;
 }
#endif

