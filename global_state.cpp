
#include "global_state.h"


fsm_states g_fsm_state;

std::string g_json_path;

kernel_state_enum g_kernel_state = STATE_LOG;

syscall_enum g_syscalls_set = syscall_enum::SYSCALL_ALL;

//bool g_Is_log_verbose;

int g_device_file = 0;

bool g_fsm_forced_to_next_state = false;


const std::unordered_map<std::string, short> g_syscall_lookup = 
                                                            {   {"open", syscall_enum::SYSCALL_OPEN},
                                                                {"read", syscall_enum::SYSCALL_READ},
                                                                {"write", syscall_enum::SYSCALL_WRITE},
                                                                {"all", syscall_enum::SYSCALL_ALL} 
                                                            };

