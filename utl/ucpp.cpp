#include <ucpp.hpp>

ILogger* g_trace_logger   = nullptr;
ILogger* g_debug_logger   = nullptr;
ILogger* g_warning_logger = nullptr;
ILogger* g_error_logger   = nullptr;

EndLine EndL;

void* Allocate(size_t cb_size, uint64_t options)
{
    return malloc(cb_size);
}

void Free(void* p)
{
    free(p);
}