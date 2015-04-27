#include <iostream>
#include <string.h>
#include <map>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <hw2_syscalls.h>
#include <stdio.h>
#include <sys/types.h>
#include <sched.h>


using namespace std;

enum Command {FORK, KILL, SET_SCHED, WAIT, IS_SHORT, GET_TIME, GET_TRIALS
};

struct glob {
    struct {
        Command command;
        pid_t pid, child;
        bool ready;
        int limit;
        int priority;
        int retries;
        int slice;
        int policy;
    } in; //input
    struct {
        int result;
        bool ready; //means 'done' here
        int code;
    } out; //output
    bool exit; // test has completed, we should exit.
} *shared;

const struct timespec req={0,5000000}; //5ms

void do_commands() {
    pid_t pid=getpid();
    pid_t child;
    while(true) {
        sched_param sp = {0};
        while (!shared->in.ready) nanosleep(&req, NULL); //so this function won't take up all the CPU
        if (shared->exit) exit(0); //when tests have completed
        if (shared->in.pid!=pid)
            continue;
        shared->in.ready=false;
        switch (shared->in.command) {
            case KILL:
                shared->out.ready=true;
                exit(0);
            case WAIT:
                waitpid(shared->in.child,NULL,0);
                shared->out.result=0;
                break;
            case FORK:
                child=fork();
                if (child==0) { //child process
                    pid=getpid(); //update pid
                    continue;
                }
                shared->out.result=child; //pid of child
                break;
            case SET_SCHED:
                sp.sched_priority = shared->in.priority;
                sp.number_of_trails = shared->in.retries;
                sp.requested_time = shared->in.slice;
                shared->out.result = sched_setscheduler(pid, shared->in.policy, &sp);
                break;
            case IS_SHORT:
                shared->out.result = is_SHORT(pid);
                break;
            case GET_TIME:
                shared->out.result = remaining_time(pid);
                break;
            case GET_TRIALS:
                shared->out.result = remaining_trails(pid);
                break;
        };
        if (shared->out.result<0) shared->out.code=errno;
        shared->out.ready=true;
    }
}

void handle_input(pid_t first_child) {
    /* Possible commands:
       kill <process>
       fork <father> <son>
       do_job <process> <time>
       set_sched <process> <policy> <number_of_trails> <time>
       is_short <process>
       get_time <process>
       get_trails <process>
       sleep <process> <time>
    */
    map<string,pid_t> m;
    map<string,string> parent;
    map<string,pid_t> zombies;
    map<string,int> policies;
    policies["SHORT"] = SCHED_SHORT;
    policies["OTHER"] = SCHED_OTHER;
    m["main"]=first_child;
    string command, arg1, arg2;
    while (cin>>command) {
        shared->out.code=0;
        shared->out.ready=false;
        if ("kill" == command)
        {
            cin>>arg1;
            if (m.find(arg1)==m.end()) {
                cout<<"unknown process: "<<arg1<<endl;
                continue;
            }
            shared->in.command=KILL;
            shared->in.pid=m[arg1];
            shared->in.ready=true;
            while (!shared->out.ready)
                nanosleep(&req, NULL);
            if (arg1=="main") {
                waitpid(m[arg1],NULL,0);
            } else if (m.find(parent[arg1])!=m.end()) {
                shared->out.ready=false;
                shared->in.pid=m[parent[arg1]];
                shared->in.child=m[arg1];
                shared->in.command=WAIT;
                shared->in.ready=true;
                while (!shared->out.ready)
                    nanosleep(&req, NULL);
            }
            m.erase(arg1);
            if (arg1!="main")
                parent.erase(arg1);
        } else if (command=="zombie") {
            cin>>arg1;
            if (m.find(arg1)==m.end()) {
                cout<<"unknown process: "<<arg1<<endl;
                continue;
            }
            shared->in.command=KILL;
            shared->in.pid=m[arg1];
            shared->in.ready=true;
            while (!shared->out.ready)
                nanosleep(&req, NULL);
            zombies[arg1] = m[arg1];
            m.erase(arg1);
        } else if (command=="waitz") {
            cin>>arg1;
            if (zombies.find(arg1)==zombies.end()) {
                cout<<"unknown zombie process: "<<arg1<<endl;
                continue;
            }
            if (arg1=="main") {
                waitpid(zombies[arg1],NULL,0);
            } else if (m.find(parent[arg1])!=m.end()) {
                shared->out.ready=false;
                shared->in.pid=m[parent[arg1]];
                shared->in.child=zombies[arg1];
                shared->in.command=WAIT;
                shared->in.ready=true;
                while (!shared->out.ready)
                    nanosleep(&req, NULL);
            } else {
                cout<<"process already dead: "<<arg1<<endl;
            }
            zombies.erase(arg1);
            if (arg1!="main")
                parent.erase(arg1);
        } else if (command=="fork") {
            cin>>arg1>>arg2;
            if (m.find(arg1)==m.end()) {
                cout<<"unknown process: "<<arg1<<endl;
                continue;
            }
            if (m.find(arg2)!=m.end()) {
                cout<<"process "<<arg2<<" already exists"<<endl;
                continue;
            }
            shared->in.command=FORK;
            shared->in.pid=m[arg1];
            shared->in.ready=true;
            while (!shared->out.ready)
                nanosleep(&req, NULL);
            if (shared->out.code==0) {
                m[arg2] = shared->out.result;
                parent[arg2] = arg1;
            }
        } else if (command=="set_sched") {
            int retry = 0;
            int time = 0;
            string policy;
            cin>>arg1>>policy>>retry>>time;
            if (m.find(arg1)==m.end()) {
                cout<<"unknown process: "<<arg1<<endl;
                continue;
            }
            if (policies.find(policy)==policies.end()) {
                cout<<"unknown policy: "<<arg2<<endl;
                continue;
            }
            shared->in.command = SET_SCHED;
            shared->in.pid     = m[arg1];
            shared->in.policy  = policies[policy];
            shared->in.retries = retry;
            shared->in.slice   = time;
            shared->in.ready=true;
            while (!shared->out.ready) nanosleep(&req, NULL);
            if (shared->out.result > 0) {
                cout<<"process "<<arg1<<": set policy successfull" << endl;
            }
        } else if (command=="get_time") {
            cin>>arg1;
            if (m.find(arg1)==m.end()) {
                cout<<"unknown process: "<<arg1<<endl;
                continue;
            }
            shared->in.command=GET_TIME;
            shared->in.pid=m[arg1];
            shared->in.ready=true;
            while (!shared->out.ready) nanosleep(&req, NULL);
            if (shared->out.result>=0) {
                cout<<"process "<<arg1<<" has remaining time of ";
                cout<<shared->out.result<<endl;
            }
        }
        else if (command=="get_trials") {
            cin>>arg1;
            if (m.find(arg1)==m.end()) {
                cout<<"unknown process: "<<arg1<<endl;
                continue;
            }
            shared->in.command = GET_TRIALS;
            shared->in.pid     =m[arg1];
            shared->in.ready   =true;
            while (!shared->out.ready) nanosleep(&req, NULL);
            if (shared->out.result>=0) {
                cout<<"process "<<arg1<<" has remaining trials of ";
                cout<<shared->out.result<<endl;
            }
        }
        else if ("is_short" == command) {
            cin>>arg1;
            if (m.find(arg1)==m.end()) {
                cout<<"unknown process: "<<arg1<<endl;
                continue;
            }
            shared->in.command = IS_SHORT;
            shared->in.pid     =m[arg1];
            shared->in.ready   =true;
            while (!shared->out.ready) nanosleep(&req, NULL);
            if (shared->out.result>=0) {
                cout<<"process "<<arg1<<" is short";
                cout<<shared->out.result<<endl;
            }
        }
        else if ("quit" == command) {
            exit(0);
        } else {
            cout<<"command "<<command<<" not found"<<endl;
            continue;
        }
        cout<<"errno: "<<strerror(shared->out.code)<<endl;
    }
    shared->exit=true;
    shared->in.ready=true;
}

int main() {
    shared=(glob*)mmap(NULL, sizeof *shared, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shared->in.ready=false;
    shared->out.ready=false;
    shared->exit=false;
    pid_t child=fork();
    if (child==0) {
        do_commands();
    } else {
        handle_input(child);
    }
    return 0;
}
