#include <sys/types.h>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <hw2_syscalls.h>
#include <iostream>
#include <utility>
#include <vector>
#include <iomanip>

using namespace std;

int fibonaci(int n)
{
	if (n < 2)
		return n;
	return fibonaci(n-1) + fibonaci(n-2);
}

struct test_task
{
	int pid;
	int trials;
	int fnum;
};

void print_test_results(struct switch_info *si, int size, const vector<test_task>& tasks)
{
	cout << "Test Results:" << endl;
	cout << "=============" << endl;
	cout << "Tested Processes:" << endl;
	cout << "-----------------" << endl;
	for(int i = 0; i < tasks.size(); i++)
	{
		cout << "pid:" << tasks[i].pid << " trials:" << tasks[i].trials << " fibonaci: " << tasks[i].fnum << endl;
	}
	cout << "----------------------------------------" << endl;

	cout << "| previous_pid | next_pid     | previous_policy | next_policy  | time         | reason" << endl;    
	for(int i = 0; i < size; i++)
	{
		if(si[i].previous_pid != -1)
			cout << "|" << setw(14) << si[i].previous_pid <<
			 	    "|" << setw(14) << si[i].next_pid <<
			 	    "|" << setw(17) << si[i].previous_policy <<
			 	    "|" << setw(14) << si[i].next_policy <<
			 	    "|" << setw(14) << si[i].time << 
			 	    "|" << setw(20) << SWITCH_REASONS_STR[si[i].reason] << endl;
	}
}
int main(int argc, char const *argv[])
{
	int pid = getpid();
	int res = 0;
	if((argc - 1) % 2 != 0)
		return -1;

	int tasks_num = (argc - 1) / 2;
	vector<test_task> tasks;
	for(int i = 1; i <= tasks_num; i++)
	{
		test_task tt;
		tt.trials = atoi (argv[i*2 - 1]);
		tt.fnum = atoi (argv[i*2]);
		tt.pid = fork();
		if(tt.pid == 0)
		{
			int cpid = getpid();
			sched_param sp = {0};

			sp.sched_priority = 1;
			sp.requested_time = 10;
			sp.number_of_trials = tt.trials;
			res = sched_setscheduler(cpid, SCHED_SHORT, &sp);

			cout << "test pid:" << cpid << endl;
			cout << "================" << endl;

			res = is_SHORT(cpid);
			cout << "is_SHORT res = " << res << ";\terrno = "<< errno << ";\tpid = " << cpid << endl;
			
			res = remaining_time(cpid);
			cout << "remaining_time res = " << res << ";\terrno = "<< errno << ";\tpid = " << cpid << endl;
			
			res = remaining_trials(cpid);
			cout << "remaining_trails res = " << res << ";\terrno = "<< errno << ";\tpid = " << cpid << endl;
			
			fibonaci(tt.fnum);
			return 0;
		}else if(tt.pid > 0)
		{
			tasks.push_back(tt);
		}
	}
	int status = 0;
	res = 0;
	struct switch_info si[150] = {0};

	for(int i = 0; i < tasks.size(); i++)
	{
		cout << "waiting for " << tasks[i].pid << endl;
		res = waitpid(tasks[i].pid, &status, 0);
	}
	int size = get_scheduling_statistic(si);
	print_test_results(si, size, tasks);
	cout << " res=" << res << ";errno=" << errno << endl;
	return 0;
}
