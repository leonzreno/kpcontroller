


#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

#include "defs.h"



extern fsm_states g_fsm_state;

extern std::string g_json_path;

extern kernel_state_enum g_kernel_state;

extern syscall_enum g_syscalls_set;

extern bool g_Is_log_verbose;

extern int g_device_file;

extern const std::unordered_map<std::string, short> g_syscall_lookup;


// when kernel module detects a syscall which is differen from our expected syscall
// finite state machine will move to next state when this flag is enabled
// otherwise will stick to current sate.
extern bool g_fsm_forced_to_next_state;

#endif