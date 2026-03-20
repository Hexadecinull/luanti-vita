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

#endif
