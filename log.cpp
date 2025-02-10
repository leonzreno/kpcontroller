

#include <cstring>
#include <mutex>

#include "log.h"

std::mutex log_lck;

void log_impl(FILE* stream, const char* category, const char* format, ...)
{
    std::lock_guard<std::mutex> lock(log_lck);
    if( !strchr(format, '%'))
    {
        printf("%s", format);
        return;
    }

    va_list args;
    va_start(args, format);
    fprintf(stream, category);
    vfprintf(stream, format, args);
    va_end(args);
}


void log_err(const char* format, ...)
{
    log_impl(stderr, "[error]", format);
}


void log_info(const char* format, ...)
{
    log_impl(stdout, "[info]", format);
}