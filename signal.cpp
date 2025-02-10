#include <csignal>
#include "signal.h"
#include "log.h"
#include "fsm_executor.h"

Executor* pExec = nullptr;

void notify_fsm_executor(int sig)
{
    if( !pExec)
    {
        log_err("invalid fsm_executor.\n");
        log_err("Will quit forcly.\n ");
        kill(getpid(), SIGKILL);
        return;
    }


    pExec->notify_to_stop();
    log_info("Notified fsm_executor to stop.\n");
}

void Signal::install_sig_handler(Executor* p)
{
    pExec = p;

    struct sigaction sa;
    sa.sa_handler = notify_fsm_executor;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGQUIT, &sa, nullptr);
}

 Signal::Signal(Executor* p)
 {
    install_sig_handler(p);
 }