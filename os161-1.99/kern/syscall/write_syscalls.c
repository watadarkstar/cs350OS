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
	if(curproc->p_fdlist[STDIN_FILENO] == NULL){
		init_STD();
	}
	
	//if fd is an invalid number
	if(fd < 0 || fd > __OPEN_MAX){
		return EBADF;
	}
	struct fd ** cur_fd = NULL;
	cur_fd = &(curproc->p_fdlist[fd]);
	
	//if fd points to a closed file
	if(cur_fd == NULL){
		return EBADF;
	}
	//if file does not have write privileges
	else if(((*cur_fd)->fd_flag & O_WRONLY) != O_WRONLY && ((*cur_fd)->fd_flag & O_RDWR) != O_RDWR){
		return EBADF;
	 }
	
	struct iovec iov;
	struct uio u;
	struct vnode * vn =  (*cur_fd)->fd_vfile;
	int result;

	//copy const buf into a non const buf
	void * buffer = kmalloc(sizeof(buf));
	result = copyin(buf, buffer, buflen);
	
	if(result){
		return EFAULT;
	}	
	
	//Write to file
	uio_kinit(&iov, &u, buffer, buflen, 0, UIO_WRITE);
	result = VOP_WRITE(vn, &u);
	
	if(result) {
		return result;
	}
	*retval = u.uio_resid;

 	return 0;
 }
 #endif 

 
