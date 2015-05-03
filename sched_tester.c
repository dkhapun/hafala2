#include <sys/types.h>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <hw2_syscalls.h>
#include <stdlib.h>
#include <stdio.h>

int fibonaci(int n)
{
	if (n < 2)
		return n;
	return fibonaci(n-1) + fibonaci(n-2);
}

typedef struct test_task_t
{
	int pid;
	int trials;
	int fnum;
} test_task ;

char* print_policy(int policy){
	switch(policy){
		case SCHED_OTHER: return "other"; break;
		case SCHED_FIFO: return "real time FIFO"; break;
		case SCHED_RR: return "real time RR"; break;
		case SCHED_SHORT: return "short"; break;
		default : return "unknown"; break;

	}
}

void print_test_results(struct switch_info *si, int size, test_task tasks[], int tasks_size)
{
	printf("Test Results:\n");
	printf("=============\n");
	printf("Tested Processes:\n");
	printf("-----------------\n");
	
	int i;
	for(i = 0; i < tasks_size; i++)
	{
		printf("pid: %d trials: %d fibonaci: %d\n", tasks[i].pid, tasks[i].trials, tasks[i].fnum);
	}
	printf("----------------------------------------\n");

	printf("| previous_pid | next_pid     | previous_policy | next_policy  | time         | reason\n");
		
	for(i = 0; i < size; i++)
	{
		if(si[i].previous_pid != -1){
			printf("|\t%d|\t%d|\t%s|\t%s|\t%lu|\t%s\n", si[i].previous_pid, si[i].next_pid, print_policy(si[i].previous_policy), print_policy(si[i].next_policy), si[i].time, SWITCH_REASONS_STR[si[i].reason]);
		}
		
	}
}
int main(int argc, char const *argv[])
{
	int pid = getpid();
	int res = 0;
	if((argc - 1) % 2 != 0)
		return -1;

	int tasks_num = (argc - 1) / 2;
	test_task tasks[tasks_num];
	int current = 0;
	//vector<test_task> tasks;
	int i;
	for( i= 1; i <= tasks_num; i++)
	{
		test_task tt;
		tt.trials = atoi (argv[i*2 - 1]);
		tt.fnum = atoi (argv[i*2]);
		tt.pid = fork();
		if(tt.pid == 0)
		{
			int cpid = getpid();
			struct sched_param sp;
			sp.sched_priority = 1;
			sp.requested_time = 10;
			sp.number_of_trials = tt.trials;
			res = sched_setscheduler(cpid, SCHED_SHORT, &sp);

			printf("test pid:\n");
			printf("================\n");

			res = is_SHORT(cpid);
			printf("is_SHORT res = %d;\terrno = %d;\tpid = %d\n", res, errno, cpid);
			
			res = remaining_time(cpid);
			printf("remaining_time res = %d;\terrno = %d;\tpid = %d\n", res, errno, cpid);
			
			res = remaining_trials(cpid);
			printf("remaining_trials res = %d;\terrno = %d;\tpid = %d\n", res, errno, cpid);

			fibonaci(tt.fnum);
			return 0;
		}else if(tt.pid > 0)
		{
			tasks[current].pid = tt.pid;
			tasks[current].trials = tt.trials;
			tasks[current++].fnum = tt.fnum;
		}
	}
	int status = 0;
	res = 0;
	struct switch_info si[150] = {0};

	for(i = 0; i < tasks_num; i++)
	{
		printf("waiting for %d\n", tasks[i].pid);
		res = waitpid(tasks[i].pid, &status, 0);
	}
	int size = get_scheduling_statistic(si);
	print_test_results(si, size, tasks, tasks_num);
	printf(" res = %d;errno = %d\n", res, errno);
	return 0;
}
