//
// Created by Daniel on 26/04/2015.
//

#ifndef HAFALA2_API_TEST_H
#define HAFALA2_API_TEST_H
#include <sched.h>
#include <hw2_syscalls.h>

namespace api_test
{

    struct sched_short_param
    {
        int sched_priority;
        int number_of_trails;
        int requested_time;
    };

    int sched_setscheduler(int pid, int policy, const sched_short_param* ssp)
    {
        sched_param sp = {0};
        sp.requested_time = ssp->requested_time;
        sp.number_of_trails = ssp->number_of_trails;
        sp.sched_priority = ssp->sched_priority;
        return ::sched_setscheduler(pid, policy, &sp);
    }

    int is_SHORT(int pid)
    {
        ::is_SHORT(pid);
    }
    int remaining_time(int pid)
    {
        ::remaining_time(pid);
    }
    int remaining_trails(int pid)
    {
        ::remaining_trails(pid);
    }

}
#endif //HAFALA2_API_TEST_H
