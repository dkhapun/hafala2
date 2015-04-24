#ifndef __LINUX_HW2_SYSCALL_H
#define __LINUX_HW2_SYSCALL_H

#include <errno.h>
#include <stdio.h>

int is_SHORT(int pid){
	long __res;
	__asm__ volatile (
		"movl $243, %%eax;"
		"movl %1, %%ebx;"
		"int $0x80;"
		"movl %%eax,%0"
		: "=m" (__res)
		: "m" (pid)
		: "%eax","%ebx"
	);
	if ((unsigned long)(__res) >= (unsigned long)(-125)) {
		errno = -(__res); __res = -1;
	}
	return (int)(__res);
}


int remaining_time(int pid) {
	long __res;
	__asm__ volatile (
		"movl $244, %%eax;"
		"movl %1, %%ebx;"
		"int $0x80;"
		"movl %%eax,%0"
		: "=m" (__res)
		: "m" (pid)
		: "%eax","%ebx"
	);
	if ((unsigned long)(__res) >= (unsigned long)(-125)) {
		errno = -(__res); __res = -1;
	}
	return (int)(__res);
}

int remaining_trails(int pid) {
	long __res;
	__asm__ volatile (
		"movl $245, %%eax;"
		"movl %1, %%ebx;"
		"int $0x80;"
		"movl %%eax,%0"
		: "=m" (__res)
		: "m" (pid)
		: "%eax","%ebx"
	);
	if ((unsigned long)(__res) >= (unsigned long)(-125)) {
		errno = -(__res); __res = -1;
	}
	return (int)(__res);
}

#endif 

