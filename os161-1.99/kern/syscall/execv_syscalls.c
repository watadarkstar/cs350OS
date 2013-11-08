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

#if OPT_A2
// man page
// int execv(const char *program, char **args);
int sys_execv(const char *program, char **args){
	(void)program;
	(void)args;
	return 0;
}
#endif
