#ifndef DEFS_H
#define DEFS_H


#define DEVICE_NAME "KMMORW"
#define DEVICE_FILE "/dev/" DEVICE_NAME

#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>

// 0 for STATE_OFF, 1 for STATE_LOG and 2 for STATE_BLOCK
#define IOCTL_SET_STATE  _IOW('k', 1, int)


#define IOCTL_SET_MONITOR _IOW('k',2, int)

#define IOCTL_GET_LAST_INVOKE_TIME _IOWR('k',3, int)


// json full name path
//std::string fsm_state_file_path;


// kprobe_filemon's state
enum kernel_state_enum {
    STATE_OFF = 0,
    STATE_LOG,
    STATE_BLOCK,

};

enum syscall_enum {
    SYSCALL_OPEN = 0x01,
    SYSCALL_READ = 0x02,
    SYSCALL_WRITE = 0x04,
    SYSCALL_ALL = SYSCALL_OPEN | SYSCALL_READ | SYSCALL_WRITE,
};


bool Is_support_syscall_desc(const std::string& call);

//json file content definition:
/***
{
  "states": ["open", "read", "write"],
  "transitions": ["open", "read", "write", "open", "write", "read"]
}

***/

#define JSON_FSM_STATES  "states"
#define JSON_FSM_TRANSITIONS    "transitions"

class fsm_states
{
private:
    class fsm_state_entry
    {
    public:
        std::string syscall;
        short number;
    };

public:

    void initialize_from_json(std::vector<std::string>&& transitions);

    bool analyse_json_file(const std::string& path);

    short get_initial_sys( )
    {
        return get_sys(0);
    }

    std::string get_initial_sys_tx()
    {
        return get_sys_tx(0);
    }

    short get_current_sys()
    {
        return get_sys(current_id);
    }

    std::string get_current_sys_tx()
    {
        return get_sys_tx(current_id);
    }

    short get_next_sys()
    {
        return get_sys(get_next_id());
    }

    std::string get_next_sys_tx()
    {
        return get_sys_tx(get_next_id());
    }

    short get_sys(short id)
    {
        assert( len > 0 );
        return entries[id].number;
    }

    std::string get_sys_tx(short id)
    {
        assert( len > 0 );
        return entries[id].syscall;
    }

    short get_current_id()
    {
        return current_id;
    }

    short get_next_id()
    {
        return (current_id + len) % len;
    }

    void move_to_next()
    {
        current_id = (current_id + len) % len;
    }

private:

    void onReinitialize();

    std::vector<std::string> states;
    std::vector<fsm_state_entry> entries;

    unsigned short current_id;
    unsigned short  len;
};


#define SLEEP_TIME_IN_MILLISECONDS  500000

#endif
