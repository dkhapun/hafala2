#include <linux/linkage.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <asm/errno.h>

#include <linux/mm.h>
#include <linux/nmi.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/smp_lock.h>
#include <asm/mmu_context.h>
#include <linux/interrupt.h>

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

//The wrapper will return the number of trials left for the SHORT process, for
//overdue process it should return 0.
asmlinkage int sys_get_scheduling_statistic(struct switch_info * info) 
{
	task_t *my_task = current;
	if (!info)
		return -EINVAL;

	int retval = copy_switch_info_to_user(info) ? -EFAULT : 0;
	if (my_task == NULL)
	{
		return -1; // in case of error
	}
	return 1;
	
	return -EINVAL;
}

