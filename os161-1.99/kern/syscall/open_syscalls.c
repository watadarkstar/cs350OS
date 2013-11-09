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
	Opens the file, assigns an file descriptor number to it.
	A lot more error checking can occur here. Some I didn't know how to do,
	such as filename existence checking. 
	-Aaron
*/
 int 
 sys_open(userptr_t filename, int flags, int mode, int32_t *retval) {

	//Inits STDIN, STDOUT and STDERR if not already done
	if(curthread->t_fdlist[STDIN_FILENO] == NULL){
		init_STD();
	}
	
	// If filename is not init'd
	if(filename == NULL){
		return EFAULT;
	}
	char fname[__PATH_MAX+1];
	size_t len;
	int index = -1;
	int err = copyinstr(filename, fname, __PATH_MAX, &len);
	// if copy fails
	if(err != 0){
		return EFAULT;
	}
	// If filepath is non-existent
	else if(len == 0){
		return EFAULT;
	}
	// if flag has append attrbiute
	else if(flags & O_APPEND){
		return EINVAL;
	}
	
	// Finds first unitilized fd
	for(int i = 3; i < __OPEN_MAX; i++){
		if(curthread->t_fdlist[i] == NULL){
			index = i;
			break;
		}
	}

	// if there was no empty fd
	if(index == -1){
		return EMFILE;
	}

	struct vnode * vn = (struct vnode *)kmalloc(sizeof(struct vnode));
	int result = vfs_open(fname,flags, mode, &vn);
	if(result != 0){
		return result;
	}
	
	// Assign fd to this file
	struct fd * fid = (struct fd *)kmalloc(sizeof(struct fd));
	fid->flag = flags;
	strcpy(fid->name, fname);
	fid->vfile = vn;
	curthread->t_fdlist[index] =  fid;


	*retval = index;	
 	return 0;
 }
 
 /* Used to initilize STDIN, STDOUT and STDERR.
    Must be run before any other filesystem call is done, as these are always supposed to be open
 */
  void init_STD(){
	int flaglist[3] = {O_RDONLY, O_WRONLY, O_WRONLY};
	for(int i = 0; i <= STDERR_FILENO; i++){
		char * console = NULL;
		struct vnode * vn = kmalloc(sizeof(struct vnode));
		console = kstrdup("con:");
		vfs_open(console, flaglist[i], 0, &vn);
		
		struct fd * fid = kmalloc(sizeof(struct fd));
		fid->flag = flaglist[i];
		strcpy(fid->name, console);
		fid->vfile = vn;
		curthread->t_fdlist[i] =  fid;
		
		kfree(console);
	}
}
 #endif
 
