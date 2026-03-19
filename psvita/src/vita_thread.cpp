#ifdef __vita__

#include "vita_thread.h"

#include <psp2/kernel/threadmgr.h>
#include <cstring>

extern "C" {

int vita_thread_set_name(const char *name)
{
    (void)name;
    return 0;
}

int vita_thread_get_priority()
{
    return sceKernelGetThreadCurrentPriority();
}

void vita_thread_yield()
{
    sceKernelDelayThread(1);
}

int vita_thread_set_priority(int prio)
{
    return sceKernelChangeThreadPriority(sceKernelGetThreadId(), prio);
}

}

#endif
