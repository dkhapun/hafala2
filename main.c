#include <syscall_maxproc.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
//#include <sys/wait.h>

int main(int argc, char** argv)
{
	int son1 = fork();
	printf("Is short , %d" ,is_SHORT(son1));
	printf("Remaining time , %d" ,remaining_time(son1));
	printf("Remaining trails , %d" ,remaining_trails(son1));
	
	return 0;
}
