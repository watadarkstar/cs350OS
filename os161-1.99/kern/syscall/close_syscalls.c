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
	Closes the file, when given a valid fd
	-Aaron
*/
 int 
 sys_close(int fd) {

	//If fid is STDIN, STDOUT or STDERR, or if above max number of files
	if(fd < 3 || fd > __OPEN_MAX){
		return EBADF;
	}
	//If fid refers to an already closed file, just return success. 
	else if(curthread->t_fdlist[fd] == NULL){
		return 0;
	}
	
	vfs_close(curthread->t_fdlist[fd]->vfile);
	curthread->t_fdlist[fd] = NULL;
	
	return 0;
}
#endif
	
