#include <linux/linkage.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <asm/errno.h>

typedef struct switch_info
{
	int previous_pid;
	int next_pid;
	int previous_policy;
	int next_policy;
	unsigned long time;
	int reason;
} switch_info_t;

//The wrapper will return 1 if the given process is a SHORT-process, or 0 if it is
//already overdue.
asmlinkage int sys_is_SHORT(int pid) {

	task_t *my_task = find_task_by_pid( pid); 
	if (my_task == NULL){
		return -1; // in case of error
	}	
	if (my_task->policy == SCHED_SHORT) {
		if (my_task->is_overdue == 1){
			return 0;
		}
		return 1;
	}
	
	return -EINVAL; // when it is not short of overdue short
}

//The wrapper will return the time left for the process at the current time slice,
//for overdue process it should return 0.
asmlinkage int sys_remaining_time(int pid) {

	task_t *my_task = find_task_by_pid(pid); 
	if (my_task == NULL){
		return -1; // in case of error
	}
	if (my_task->policy != SCHED_SHORT) { // when it is nor short overdue or short
		return -EINVAL;
	}
	if ( my_task->is_overdue == 1 ){ // it is short now but is it overdue?
		return 0;
	}

	return (((my_task->time_slice) / HZ ) * 1000);//when it is short returns its left time in slice
}

//The wrapper will return the number of trials left for the SHORT process, for
//overdue process it should return 0.
asmlinkage int sys_remaining_trials(int pid) {

	task_t *my_task = find_task_by_pid(pid); 
	if (my_task == NULL){
		return -1; // in case of error
	}
	if (my_task->policy == SCHED_SHORT) {
		if (my_task->is_overdue){
			return 0;
		}
		return (my_task->number_of_trials) - (my_task->current_trial);
	}
	
	return -EINVAL;
}
