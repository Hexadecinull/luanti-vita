#include "vita_platform.h"

#ifdef __vita__

#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>

#include <functional>
#include <queue>
#include <mutex>

namespace vita {

static SceUID         g_main_thread_id = 0;
static std::queue<std::function<void()>> g_main_queue;
static std::mutex                        g_main_mutex;

static __attribute__((constructor)) void vita_platform_init_ctor()
{
    g_main_thread_id = sceKernelGetThreadId();
}

void run_on_main_thread(std::function<void()> fn)
{
    if (is_main_thread()) {
        fn();
        return;
    }
    std::lock_guard<std::mutex> lock(g_main_mutex);
    g_main_queue.push(std::move(fn));
}

bool is_main_thread()
{
    return sceKernelGetThreadId() == g_main_thread_id;
}

void flush_main_thread_queue()
{
    std::lock_guard<std::mutex> lock(g_main_mutex);
    while (!g_main_queue.empty()) {
        g_main_queue.front()();
        g_main_queue.pop();
    }
}

}

#endif
