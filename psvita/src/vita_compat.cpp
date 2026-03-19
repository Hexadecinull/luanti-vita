#include "vita_compat.h"

#include <psp2/rtc.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <sys/time.h>

#include <map>
#include <string>
#include <mutex>

static std::map<std::string, std::string> g_env_map;
static std::mutex                          g_env_mutex;

extern "C" {

void *vita_memalign(size_t alignment, size_t size)
{
    if (alignment < sizeof(void *)) alignment = sizeof(void *);
    size_t total = size + alignment + sizeof(void *);
    void *raw = malloc(total);
    if (!raw) return nullptr;
    uintptr_t addr = reinterpret_cast<uintptr_t>(raw) + sizeof(void *);
    uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
    reinterpret_cast<void **>(aligned)[-1] = raw;
    return reinterpret_cast<void *>(aligned);
}

void vita_aligned_free(void *ptr)
{
    if (!ptr) return;
    free(reinterpret_cast<void **>(ptr)[-1]);
}

int vita_gettimeofday(struct timeval *tv, void *tz)
{
    (void)tz;
    if (!tv) return -1;
    SceRtcTick tick;
    sceRtcGetCurrentTick(&tick);
    uint64_t us = tick.tick / 1;
    tv->tv_sec  = static_cast<long>(us / 1000000ULL);
    tv->tv_usec = static_cast<long>(us % 1000000ULL);
    return 0;
}

unsigned int vita_sleep(unsigned int seconds)
{
    sceKernelDelayThread(static_cast<SceUInt>(seconds) * 1000000U);
    return 0;
}

int vita_usleep(unsigned int us)
{
    sceKernelDelayThread(static_cast<SceUInt>(us));
    return 0;
}

long vita_sysconf(int name)
{
    switch (name) {
        case 1: return 2;
        default: return -1;
    }
}

int vita_isatty(int fd)
{
    (void)fd;
    return 0;
}

const char *vita_getenv(const char *name)
{
    if (!name) return nullptr;
    std::lock_guard<std::mutex> lock(g_env_mutex);
    auto it = g_env_map.find(name);
    if (it == g_env_map.end()) return nullptr;
    return it->second.c_str();
}

int vita_setenv(const char *name, const char *value, int overwrite)
{
    if (!name || !value) return -1;
    std::lock_guard<std::mutex> lock(g_env_mutex);
    auto it = g_env_map.find(name);
    if (it != g_env_map.end() && !overwrite) return 0;
    g_env_map[name] = value;
    return 0;
}

int vita_getpid()
{
    return static_cast<int>(sceKernelGetProcessId());
}

}
