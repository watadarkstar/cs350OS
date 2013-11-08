#include <types.h>
#include <thread.h>
#include <proc.h>
#include <uio.h>
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
	Opens the file, assigns an f "le descriptor number to it.
	A lot more error checking can occur here. Some I didn't know how to do,
	such as filename existence checking. 
	-Aaron
*/
 int 
 sys_read(int fd, void * buf, size_t buflen, int32_t *retval) {

	if(fd < 0 || fd > __OPEN_MAX){
		return EBADF;
	}
	else if(curthread->t_fdlist[fd] == NULL){
		return EBADF;
	}
	struct iovec iov;
	struct uio u;
	struct vnode * vn = curthread->t_fdlist[fd]->vfile;	
	struct addrspace * as = curproc_getas();
    int result;
	
	iov.iov_ubase = buf;
	iov.iov_len = buflen;
	u.uio_iov = &iov;
	u.uio_iovcnt = 1;
	u.uio_resid = buflen;
	u.uio_offset = 0;
	u.uio_segflg = UIO_USERSPACE;
	u.uio_rw = UIO_READ;
	u.uio_space = as;
	
	
	uio_kinit(&iov, &u, &buf, buflen, 0, UIO_READ);
	result = VOP_READ(vn, &u);
	if(result) {
		return result;
	}
	*retval = u.uio_resid;
	
 	return 0;
 }
 #endif 

 
