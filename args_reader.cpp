#include <string>
#include <set>
#include <fstream>
#include "json.hpp"

#include "args_reader.h"
#include "global_state.h"


#include "log.h"


using json = nlohmann::json;

bool verify_json_file_consistency(const std::vector<std::string>& states, const std::vector<std::string>& transitions)
{
    std::set<std::string> s;
    for(auto& state: states)
    {
        if( !Is_support_syscall_desc(state) )
        {
            return false;
        }

        s.insert(state);
    }

    for(auto& trans: transitions)
    {
        if( !s.count(trans) )
        {
            return false;
        }
    }

    return true;
}

bool parse_args(int argc, char** argv)
{
    bool Is_file_set = false;
    bool Is_sys_set = false;
    bool Is_state_block_set = false;
    bool Is_state_log_set = false;
    bool Is_state_off_set = false;

    for( int i = 1; i < argc; )
    {
        std::string cur = argv[i];

        if( (cur == "--file" || cur == "-f") && i + 1 <argc )
        {
            std::string json_file_path = argv[i+1];

            i += 2;

            std::ifstream json_file(json_file_path);
            if(!json_file)
            {
                log_err("unable to open json file: %s\n", json_file_path.c_str());
                return false;
            }

            json jsn;

            try
            {
                json_file >> jsn;

                std::vector<std::string> states = jsn[JSON_FSM_STATES].get<std::vector<std::string>>();

                std::vector<std::string> transitions = jsn[JSON_FSM_TRANSITIONS].get<std::vector<std::string>>();
            
                if( !verify_json_file_consistency(states, transitions) )
                {
                    log_err("json file: %s is not compatible with this version.\n", json_file_path.c_str());
                    return false;
                }

                g_fsm_state.initialize_from_json(std::move(transitions));
            }
            catch(const json::parse_error& e)
            {
                log_err("json file: %s parsing error: %s.\n", json_file_path.c_str(), e.what());
                return false;
            }
            catch(const json::type_error& e)
            {
                log_err("json file: % type error: %s.\n", json_file_path.c_str(), e.what());
                return false;
            }
            catch(const json::exception& e)
            {
                log_err("json file: %s error %s.\n", json_file_path.c_str(), e.what());
                return false;
            }

            json_file.close();

            Is_file_set = true;
            continue;
        }

        if( (cur == "--syscall" || cur == "-s") && i + 1 < argc)
        {
            std::string data = argv[i+1];

            Is_sys_set = true;
            if( "open" == data)
            {
                g_syscalls_set = syscall_enum::SYSCALL_OPEN;
            }
            else if( "read" == data)
            {
                g_syscalls_set = syscall_enum::SYSCALL_READ;
            }
            else if( "write" == data)
            {
                g_syscalls_set = syscall_enum::SYSCALL_WRITE;
            }
            else if( "all" == data)
            {
                g_syscalls_set = syscall_enum::SYSCALL_ALL;
            }
            else
            {
                Is_sys_set = false;
                log_err("invalid syscall param: %s\n", data);
                log_err("--syscall should be open, read, write or all.\n");
                return false;
            }

            i += 2;
            continue;

        }
        

        if( (cur == "--block" || cur == "-b"))
        {
            g_kernel_state = kernel_state_enum::STATE_BLOCK;
            Is_state_block_set = true;

            ++i;
            continue;
        }

        if( (cur == "--log" || cur == "-l"))
        {
            g_kernel_state = kernel_state_enum::STATE_LOG;
            Is_state_block_set = true;
            ++i;
            continue;
        }

        if( (cur == "--off" || cur == "-o"))
        {
            g_kernel_state = kernel_state_enum::STATE_OFF;
            Is_state_block_set = true;
            ++i;
            continue;
        }

    }

    if( Is_file_set && Is_sys_set && (Is_state_block_set || Is_state_log_set || Is_state_off_set))
    {
        return true;
    }

    return false;
}