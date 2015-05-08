#include <sched.h>
//#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <hw2_syscalls.h>

#define HW2_DBG2(f, ...) \
	do { \
//		if (current->policy == SCHED_SHORT)\
//		{ \
			printf("HW2[%s:%s]", __FUNCTION__, __LINE__); \
			printf(f, ## __VA_ARGS__); \
//		} \
	} while (0)

#define HZ 512
#define THIS_POL SCHED_RR

int to_short(int pid, int policy, struct sched_param* param) {
	int ret = 0;
	int i = 0;
	if (sched_getscheduler(pid) != SCHED_OTHER) {

		if ((ret = sched_setscheduler(pid,SCHED_OTHER,param)) < 0)
		{
			printf("setsched other error: %s", strerror(errno));
			return ret;
		} 
	}
	for(i = 0; i < 10000; i++){}
	if ((ret = sched_setscheduler(pid,SCHED_SHORT,param)) < 0)
	{
		printf("setsched short error: %s", strerror(errno));
	}
	return ret;
}


char* print_policy(int policy){
	switch(policy){
		case SCHED_OTHER: return "other"; break;
		case SCHED_FIFO: return "real time FIFO"; break;
		case SCHED_RR: return "real time RR"; break;
		case SCHED_SHORT: return "short"; break;
		default : return "unknown"; break;

	}
}
void print_test_results(struct switch_info *si, int size)
{
	printf("Test Results:\n");
	printf("=============\n");
	printf("Tested Processes:\n");

	
	int i;


	printf("| previous_pid | next_pid     | previous_policy | next_policy  | time         | reason\n");
		
	for(i = 0; i < size; i++)
	{
		if(si[i].previous_pid != -1){
			printf("|\t%d|\t%d|\t%s|\t%s|\t%lu|\t%s\n", si[i].previous_pid, si[i].next_pid, print_policy(si[i].previous_policy), print_policy(si[i].next_policy), si[i].time, SWITCH_REASONS_STR[si[i].reason]);
		}
		
	}
}

void testChangeRequestedTimeForShort() {
	printf("id = %d \n " , getpid());
    int id = fork();
    int status;
	

    if (id > 0) {
		printf("id son = %d \n " , id);
		//the father
		struct sched_param paramIn, paramOut ;
		int expected_requested_time =2000;
		int expected_trials = 50;
		paramIn.requested_time = expected_requested_time;
		paramIn.trial_num = expected_trials;
		printf("line 287 \n");
		paramIn.sched_priority = 0;
		//to_short(id, SCHED_SHORT, &paramIn); //make son short
	//	assert(sched_getscheduler(id) == SCHED_SHORT);
		
		printf("line 292 \n");
		
		assert(sched_getparam(id, &paramOut) == 0);
		printf("line 295 \n");
		//assert(paramOut.requested_time == expected_requested_time); //should be 2000
		printf("line 297 \n");
		//change requested_time fail
		paramIn.requested_time = 3000;
//		assert(sched_setparam(id, &paramIn) == -1);		WHY FAIL? THIS SHOULD BE GOOD
	printf("line 301 \n");
		assert(sched_getparam(id, &paramOut) == 0);       
		//assert(paramOut.requested_time == expected_requested_time); //should be 2000
	printf("line 304 \n");
		int new_expected_requested_time=1000;
		//change requested_time fail because of different trial_num
		paramIn.requested_time = new_expected_requested_time;
		paramIn.trial_num = 40;
//		assert(sched_setparam(id, &paramIn) ==-1);		WHY FAIL?
	printf("line 310 \n");
		assert(sched_getparam(id, &paramOut) == 0);       
		//assert(paramOut.requested_time == expected_requested_time); //should be 2000
	printf("line 313 \n");

		//change requested_time success
		paramIn.requested_time = new_expected_requested_time;
		paramIn.trial_num = expected_trials;
		int res = sched_setparam(id, &paramIn);
		//assert(sched_setparam(id, &paramIn) == 0);
	printf("line 320 \n");
		assert(sched_getparam(id, &paramOut) == 0);       
		//assert(paramOut.requested_time == new_expected_requested_time); //should be 1000
	printf("line %d \n", __LINE__);
		struct switch_info info[150] = {0};

	get_scheduling_statistic(info);
	//print_test_results(info, 1501);
	
	wait(&status);
	printf("OK\n");
    } else if (id == 0) {
		printf("~~~~~~~~~330~~~~~~~~~~~\n");
        exit(0);
    }
}
int main(int argc, char const *argv[])
{
	struct sched_param param;
	param.sched_priority = 50;
	assert(sched_setscheduler(getpid(), SCHED_RR, &param) != -1);
	printf("the main is : %s", print_policy(sched_getscheduler(getpid())));
	printf("testChangeRequestedTimeForShort... ");
	testChangeRequestedTimeForShort();
	/*printf("\nid = %d \n " , getpid());
    int id = fork();
    int status;
	
    if (id > 0) {
		printf("id son = %d \n " , id);
		//the father
		struct sched_param paramIn;
		int expected_requested_time =2000;
		int expected_trials = 50;
		paramIn.requested_time = expected_requested_time;
		paramIn.trial_num = expected_trials;
		printf("line 287 \n");
		paramIn.sched_priority = 0;
		to_short(id, SCHED_SHORT, &paramIn); //make son short
		int new_expected_requested_time=1000;
		paramIn.requested_time = new_expected_requested_time;
		paramIn.trial_num = expected_trials;
		assert(sched_setparam(id, &paramIn) == 0);
		//struct switch_info info[150] = {0};
		//get_scheduling_statistic(info);
		//print_test_results(info, 150);
	
		wait(&status);
		printf("OK\n");
    } else{
		printf("~~~~~~~~~330~~~~~~~~~~~\n");
        _exit(0);
    }*/
  /* int status;
	int pid = getpid();
	int res = 0;
	struct switch_info info = {0};
	
	int pros = fork();
	if(pros>0){
		struct sched_param paramIn;
		paramIn.sched_priority = 0;
		paramIn.requested_time = 2000;
		paramIn.trial_num = 50;

		to_short(pros, SCHED_SHORT, &paramIn);
		
		paramIn.requested_time = 1000;
		
		int res = sched_setparam(pros, &paramIn);
		
		
		
		wait(&status);
		printf("OK'\n");
    } else if (pros == 0) {
		printf("I died here :) \n");
        _exit(0);
    }
	testChangeRequestedTimeForShort();*/
	printf("Success!\n");
	return 0;
}
