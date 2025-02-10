#include <unistd.h>
#include <atomic>
#include <thread>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <iostream>

#include "fsm_executor.h"
#include "global_state.h"
#include "log.h"


long long encode(long long time, short syscall)
{
    time &= 0x0FFFFFFFFFFFFFFF;
    return time | ((long long)(syscall & 0x0F ) << 60);
}

void decode(long long enc, long long& time, short& syscall)
{
    time = enc & 0x0FFFFFFFFFFFFFFF;
    syscall =  (short)((enc >> 60)  & 0x0F);
    return;
}

std::string get_tx(short fn)
{
    for( auto iter = g_syscall_lookup.begin(); iter != g_syscall_lookup.end(); ++iter)
    {
        if( iter->second == fn)
        {
            return iter->first;
        }
    }

    return "unkown";
}


Executor::Executor(): need_to_stop(false), stat(not_started)
{

}

Executor::~Executor()
{
    stop();
}

bool Executor::start()
{
    close(g_device_file);

    g_device_file = open(DEVICE_FILE, O_RDWR);
    if( g_device_file < 0 )
    {
        log_err("Failed to open device file: %s.\n", DEVICE_FILE);
        return false;
    }

    log_info("Open device file: %s successfully.\n", DEVICE_FILE);

    monitor = std::thread(&Executor::do_jobs, this, this);

    last_set_syscall_tm = time(nullptr);
    last_set_syscall_fn = 0;

    return true;
}

void Executor::stop()
{
    short count = 0;
    while( status_enum::running == stat.load() )
    {
        need_to_stop.store(true);

        ++count;
        if( count >= max_wait_count)
        {
            break;
            log_err("failed to stop Executor, tried %d times./n", max_wait_count);
            return;
        }
                
        usleep(SLEEP_TIME_IN_MILLISECONDS);
    }

    if( monitor.joinable())
    {
        monitor.join();
    }
}

void Executor::notify_to_stop()
{
    need_to_stop.store(true);
}


bool Executor::poll_state()
{
    return status_enum::stopped == stat.load();
}


void Executor::do_jobs(Executor* p)
{
    stat.store(status_enum::running);

    short ini_state = g_fsm_state.get_initial_sys();

    ioctl(g_device_file, IOCTL_SET_STATE, ini_state);
    p->last_set_syscall_tm = time(NULL);
    p->last_set_syscall_fn = ini_state;

    usleep(SLEEP_TIME_IN_MILLISECONDS);

    while(true)
    {
        if( need_to_stop.load())
        {
            break;
        }

        long long last_invoke_tm_enc = 0;
        if( -1 == ioctl(g_device_file, IOCTL_GET_LAST_INVOKE_TIME, &last_invoke_tm_enc) )
        {
            log_err("IOCTL_GET_LAST_INVOKE_TIME_ENC error.\n");

            break;
        }

        long long last_invoke_tm;
        short last_invoke_fn;
        decode(last_invoke_tm_enc, last_invoke_tm, last_invoke_fn);

        std::string last_invoke_fn_tx = get_tx(last_invoke_fn);

        if( last_invoke_tm > p->last_set_syscall_tm)
        {
            // After we set current syscall to kernel module,
            // kernel module detected a new syscall. 
            short fn_in_fsm = g_fsm_state.get_current_sys();

            bool need_to_transition = false;

            if( last_invoke_fn != fn_in_fsm )
            {
                //New syscall is different as we desired
               log_info("We expect [%s] called in kernel module, but received [%s] instead.\n", g_fsm_state.get_current_sys_tx().c_str(), last_invoke_fn_tx.c_str());

                if( !g_fsm_forced_to_next_state )
                {
                    log_info("FSM won't transition from current state since g_fsm_forced_to_next_state is Not enabled.\n ");
                }
                else
                {
                    need_to_transition = true;
                    log_info("FSM will transition since g_fsm_forced_to_next_state is enabled.\n");
                }

            }
            
            if( need_to_transition )
            {
                //ethier new syscall is we desired or g_fsm_forced_to_next_state is enabled

                log_info("Transitions moved from [%s] to [%s].\n", g_fsm_state.get_current_sys_tx().c_str(), g_fsm_state.get_next_sys_tx().c_str());

                g_fsm_state.move_to_next();

                if( -1 == ioctl(g_device_file, IOCTL_SET_MONITOR, g_fsm_state.get_current_sys()) )
                {
                    log_err("IOCTL_SET_MONITOR error.\n");
                }

                p->last_set_syscall_tm = time(NULL);
                p->last_set_syscall_fn = g_fsm_state.get_current_sys();

            }
        }
        // no syscall is detected from kernel module after we set current syscall
        else
        {
            //do nothing
        }


        usleep(SLEEP_TIME_IN_MILLISECONDS);

    }

    stat.store(status_enum::stopped);

}

