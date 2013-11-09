#include <types.h>
#include <thread.h>
#include <proc.h>
#include <uio.h>
#include <current.h>
#include <vfs.h>
#include <vnode.h>
#include <syscall.h>
#include <kern/errno.h> 
#include <kern/unistd.h>
#include <kern/fcntl.h>

#include <copyinout.h>

#include "opt-A2.h"

#if OPT_A2
/*
	Writes up to buflen characters into buffer.
	-Aaron
*/
 int 
 sys_write(int fd, const void * buf, size_t buflen, int32_t *retval) { 

	//Inits STDIN, STDOUT and STDERR if not already done
	if(curthread->t_fdlist[STDIN_FILENO] == NULL){
		init_STD();
	}
	
	if(fd < 0 || fd > __OPEN_MAX){
		return EBADF;
	}
	struct fd ** cur_fd = NULL;
	cur_fd = &(curthread->t_fdlist[fd]);
		
	if(cur_fd == NULL){
		return EBADF;
	}
	
	//Causing dem errors
	// else if(((*cur_fd)->flag & O_WRONLY) != O_WRONLY || ((*cur_fd)->flag & O_RDWR) != O_RDWR){
		// return EBADF;
	// }
	
	struct iovec iov;
	struct uio u;
	struct vnode * vn =  (*cur_fd)->vfile;
	int result;


	void * buffer = kmalloc(sizeof (*buf) * buflen);
	result = copyin(buf, buffer, buflen);
	
	if(result){
		return EFAULT;
	}	
	
	uio_kinit(&iov, &u, buffer, buflen, 0, UIO_WRITE);
	
	result = VOP_WRITE(vn, &u);
	if(result) {
		return result;
	}
	*retval = u.uio_resid;

 	return 0;
 }
 #endif 

 
