#pragma once

#ifdef __vita__

#include <psp2/kernel/threadmgr.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

int  vita_thread_set_name(const char *name);
int  vita_thread_get_priority();
void vita_thread_yield();
int  vita_thread_set_priority(int prio);

#ifdef __cplusplus
}
#endif

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN (64 * 1024)
#endif

#ifndef PTHREAD_CANCEL_DISABLE
#define PTHREAD_CANCEL_DISABLE 1
#endif

#ifndef PTHREAD_CANCEL_ENABLE
#define PTHREAD_CANCEL_ENABLE 0
#endif

static inline int pthread_setcancelstate(int state, int *oldstate)
{
    (void)state;
    if (oldstate) *oldstate = PTHREAD_CANCEL_DISABLE;
    return 0;
}

static inline int pthread_cancel(pthread_t t)
{
    (void)t;
    return 0;
}

#endif
