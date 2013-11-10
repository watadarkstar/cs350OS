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
#include <addrspace.h>
#include <spl.h>

/*
  Duplicates the currently running process
  The two copies are identical except for the child having a new pid
*/
int
sys_fork(struct trapframe *tf, int32_t *retval) {

  // Prevent interrupts so address space doesn't change before getting
  // copied
  int spl = splhigh();

  // Store trapframe on the heap for copying to child
  // thread later
  struct trapframe *child_tf;
  child_tf = kmalloc(sizeof(tf));
  if (child_tf == NULL) {
    splx(spl);
    return ENOMEM;
  }

  //memcpy(child_tf, tf, sizeof(tf));
  child_tf = tf;

  // Copy parent address space
  struct addrspace *original_as;
  struct addrspace *new_as;
  original_as = curproc_getas();
  int result = as_copy(original_as, &new_as);
  if (result) {
    splx(spl);
    return ENOMEM;
  }

  // Create new process
  struct proc *new_proc = proc_create(curproc->p_name);

  //Child process needs to have parent pid attached to it
  set_parent(curproc->pid, new_proc->pid);

  // Fork new thread, attach to new proc
  result = thread_fork("Child process", new_proc, enter_forked_process, (void *)child_tf, (unsigned long)new_as);
  if (result) {
    kfree(child_tf);
    splx(spl);
    return result;  // Error code will be returned from thread_fork
  }

  // Set retval to child process pid
  *retval = new_proc->pid;
  splx(spl);
  return 0;
}
#endif

