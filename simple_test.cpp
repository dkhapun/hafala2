#include <sys/types.h>
#include <sched.h>
#include <unistd.h>
#include <errno.h>
#include <hw2_syscalls.h>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
	int pid = getpid();
	int res = 0;
	cout << "test pid:" << pid << endl;
	res = is_SHORT(pid);
	cout << "is_SHORT res=" << res << ";errno="<< errno << endl;
	res = remaining_time(pid);
	cout << "remaining_time res=" << res << ";errno="<< errno << endl;
	res = remaining_trails(pid);
	cout << "remaining_trails res=" << res << ";errno="<< errno << endl;

	sched_param sp = {0};
	sp.sched_priority = 0;
	sp.requested_time = 0;
	sp.number_of_trails = 0;
	res = sched_setscheduler(pid, SCHED_SHORT, &sp);

	cout << "remaining_trails res=" << res << ";errno="<< errno << endl;
	return 0;
}
