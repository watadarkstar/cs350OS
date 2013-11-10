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
#include <kern/wait.h>

#include "opt-A2.h"

#if OPT_A2
pid_t
sys_waitpid(pid_t pid, int * status, int options, int *retval){

	int result = valid_pid(pid);
	
	if(result){
		return result;
	}
	//If there is an option attached, FAIL. NO OPTIONS FOR YOU
	else if(options != 0){
		return EINVAL;
	}
	//If pointer is not aligned, throw an error. NO EXIT CODE FOR YOU
	else if((int)status%4 != 0){
		return EFAULT;
	}
	acquire_lock(pid);
	//While the process has not exited yet,  
	while(has_exit_code(pid) == false){
		put_to_sleep(pid);
	}
	
	*status = _MKWAIT_EXIT(get_exit_code(pid));
	release_lock(pid);
	release_pid(pid);
	*retval = pid;
	return 0;
}

#endif

