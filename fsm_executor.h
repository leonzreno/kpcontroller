
#ifndef FSM_EXECUTOR_H
#define FSM_EXECUTOR_H

#include <thread>
#include <atomic>

 enum status_enum
 {  
    not_started = 0,
    running,
    stopped,
};

class Executor{

public:
    Executor();
    ~Executor();

    bool start();

    void stop();

    void notify_to_stop();

    void do_jobs(Executor* p);

    bool poll_state();

private:

    std::thread monitor;

    std::atomic<status_enum> stat;
    std::atomic<bool> need_to_stop;

    long long last_set_syscall_tm;
    short last_set_syscall_fn;

    const static short max_wait_count = 3;

};



#endif