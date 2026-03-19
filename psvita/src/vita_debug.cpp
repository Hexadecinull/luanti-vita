#ifdef __vita__

#include "vita_debug.h"

#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <mutex>

static FILE  *g_log_file   = nullptr;
static bool   g_log_open   = false;
static std::mutex g_log_mutex;

extern "C" {

void vita_log_init(const char *path)
{
    std::lock_guard<std::mutex> lock(g_log_mutex);
    if (g_log_open) return;
    g_log_file = fopen(path ? path : "ux0:data/luanti/luanti.log", "w");
    g_log_open = (g_log_file != nullptr);
}

void vita_log_shutdown()
{
    std::lock_guard<std::mutex> lock(g_log_mutex);
    if (g_log_open && g_log_file) {
        fflush(g_log_file);
        fclose(g_log_file);
        g_log_file = nullptr;
        g_log_open = false;
    }
}

void vita_log(const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(g_log_mutex);
    if (!g_log_open || !g_log_file) return;
    va_list args;
    va_start(args, fmt);
    vfprintf(g_log_file, fmt, args);
    va_end(args);
}

void vita_log_error(const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(g_log_mutex);
    va_list args;
    va_start(args, fmt);
    if (g_log_open && g_log_file) {
        vfprintf(g_log_file, fmt, args);
        fflush(g_log_file);
    }
    va_end(args);
}

void vita_log_flush()
{
    std::lock_guard<std::mutex> lock(g_log_mutex);
    if (g_log_open && g_log_file) fflush(g_log_file);
}

}

#endif
