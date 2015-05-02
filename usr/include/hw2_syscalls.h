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

typedef enum SWITCH_REASONS {SR_TASK_CREATED, SR_TASK_ENDED, SR_TASK_YIELDS, SR_SHORT_OVER, SR_PREV_TASK_WAIT, SR_HIGHIER_TASK_ACTIVE, SR_TIME_SLICE_OVER};
static const char* SWITCH_REASONS_STR[7] =
{
	"A task was created.",
	"A task ended.",
	"A task yields the CPU.",
	"A SHORT-process became overdue.",
	"The previous task goes out for waiting.",
	"A task with higher priority returns from waiting.",
	"The time slice of the previous task has ended. "
};
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

