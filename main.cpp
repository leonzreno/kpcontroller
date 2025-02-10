
#include <string>
#include <pthread.h>

#include <vector>

#include "defs.h"
#include "log.h"
#include "args_reader.h"
#include "fsm_executor.h"
#include "signal.h"






int main( int argc, char** argv)
{
    if( !parse_args(argc, argv) )
    {
        log_err("Parsing args or analyzing json failed.\n");
        return -1;
    }


    Executor exec;
    if( !exec.start() )
    {
        log_err("exec failed to start up.\n");
        return -1;
    }

    Signal sig(&exec);

    //loop
    while(true)
    {
        if(exec.poll_state())
        {
            break;
        }
    }

    log_info("quit...\n");
}
