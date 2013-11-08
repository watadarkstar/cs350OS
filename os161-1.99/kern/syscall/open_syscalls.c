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

/*
	Opens the file, assigns an file descriptor number to it.
	A lot more error checking can occur here. Some I didn't know how to do,
	such as filename existence checking. 
	-Aaron
*/
 int 
 sys_open(userptr_t filename, int flags, int mode, int32_t *retval) {

	//If filename is not init'd
	if(filename == NULL){
		return EFAULT;
	}
	
	char fname[__PATH_MAX+1];
	size_t len;
	int index = -1;
	int err = copyinstr(filename, fname, __PATH_MAX, &len);
	
	//if copy fails
	if(err != 0){
		return EFAULT;
	}
	//If filepath is non-existent
	else if(len == 0){
		return EFAULT;
	}
	//if flag is not readonly, write only or read/write
	else if((flags & O_RDONLY) != O_RDONLY && (flags & O_WRONLY) != O_WRONLY && (flags & O_RDWR) != O_RDWR){
		return EINVAL;
	}
	//if flag has append attrbiute
	else if(flags & O_APPEND){
		return EINVAL;
	}
	
	//Finds first unitilized fd
	for(int i = 3; i < __OPEN_MAX; i++){
		if(curthread->t_fdlist[i] == NULL){
			index = i;
			break;
		}
	}

	//if there was no empty fd
	if(index == -1){
		return EMFILE;
	}

	struct vnode * vn;
	int result = vfs_open(fname,flags, mode, &vn);
	if(result != 0){
		return result;
	}
	
	//Assign fd to this file
	struct fd fid;
	fid.flag = flags;
	strcpy(fid.name, fname);
	fid.vfile = vn;
	curthread->t_fdlist[index] =  &fid;


	*retval = index;
	
 	return 0;
 }
