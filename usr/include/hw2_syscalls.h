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

int remaining_trials(int pid) {
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

typedef struct switch_info
{
	int previous_pid;
	int next_pid;
	int previous_policy;
	int next_policy;
	unsigned long time;
	int reason;
} switch_info_t;


int get_scheduling_statistic(struct switch_info * info) {
	long __res;
	__asm__ volatile (
		"movl $246, %%eax;"
		"movl %1, %%ebx;"
		"int $0x80;"
		"movl %%eax,%0"
		: "=m" (__res)
		: "m" (info)
		: "%eax","%ebx"
	);
	if ((unsigned long)(__res) >= (unsigned long)(-125)) {
		errno = -(__res); __res = -1;
	}
	return (int)(__res);
}

#endif 

