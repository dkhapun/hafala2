#include <sys/types.h>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <hw2_syscalls.h>
#include <iostream>
#include <utility>
#include <vector>
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

int main(int argc, char const *argv[])
{
	int pid = getpid();
	int res = 0;
	if((argc - 1) % 2 != 0)
		return -1;

	int tasks_num = (argc - 1) / 2;
	vector<test_task> tasks;
	for(int i = 0; i < tasks_num; i++)
	{
		test_task tt;
		tt.trials = atoi (argv[i*2]);
		tt.fnum = atoi (argv[i*2 + 1]);
		tt.pid = fork();
		if(tt.pid == 0)
		{
			int cpid = getpid();
			sched_param sp = {0};
			sp.sched_priority = 0;
			sp.requested_time = 0;
			sp.number_of_trails = tt.trials;
			res = sched_setscheduler(cpid, SCHED_SHORT, &sp);

			cout << "test pid:" << cpid << endl;
			cout << "================" << endl;

			res = is_SHORT(cpid);
			cout << "is_SHORT res=" << res << ";errno="<< errno << ";pid=" << cpid << endl;
			
			res = remaining_time(cpid);
			cout << "remaining_time res=" << res << ";errno="<< errno << ";pid=" << cpid << endl;
			
			res = remaining_trials(cpid);
			cout << "remaining_trails res=" << res << ";errno="<< errno << ";pid=" << cpid << endl;
			
			fibonaci(tt.fnum);
			return 0;
		}else if(tt.pid > 0)
		{
			tasks.push_back(tt);
		}
	}
	int status = 0;
	res = 0;
	struct switch_info[150] si = {0};

	for(int i = 0; i < tasks.size(); i++)
	{
		cout << "waiting for " << tasks[i].pid << endl;
		res = waitpid(tasks[i].pid, status, 0);
	}
	get_scheduling_statistic(&si);
	cout << "get_scheduling_statistic res=" << res << ";errno=" << errno << endl;
	return 0;
}
