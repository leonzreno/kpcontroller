#include "defs.h"
#include "global_state.h"


bool Is_support_syscall_desc(const std::string& call)
{

    auto iter = g_syscall_lookup.find(call);
    if( iter == g_syscall_lookup.end())
    {
        return false;
    }

    return true;
}

void fsm_states::onReinitialize()
{
    entries.clear();

    for( auto& state : states)
    {
        fsm_states::fsm_state_entry entry = { state, g_syscall_lookup.at(state)};
        entries.push_back(entry);
    }
}

void fsm_states::initialize_from_json(std::vector<std::string>&& vec)
{
    states = std::move(vec);

    onReinitialize();
}

bool fsm_states::analyse_json_file(const std::string& path)
{

    return true;
}