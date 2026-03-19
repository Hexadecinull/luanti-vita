#pragma once

#include <cstddef>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

void *vita_memalign(size_t alignment, size_t size);
void  vita_aligned_free(void *ptr);

int vita_gettimeofday(struct timeval *tv, void *tz);

unsigned int vita_sleep(unsigned int seconds);
int          vita_usleep(unsigned int us);

long vita_sysconf(int name);

int vita_isatty(int fd);

const char *vita_getenv(const char *name);
int         vita_setenv(const char *name, const char *value, int overwrite);

int vita_getpid();

#ifdef __cplusplus
}
#endif

#ifdef __vita__
#include <psp2/rtc.h>
#include <psp2/kernel/processmgr.h>

#ifndef __DEFINED_timeval
#define __DEFINED_timeval
struct timeval {
    long tv_sec;
    long tv_usec;
};
#endif

#ifndef SYSCONF_NPROCESSORS_ONLN
#define _SC_NPROCESSORS_ONLN 1
#endif

#define gettimeofday(tv, tz) vita_gettimeofday((tv), (tz))
#define sleep(s)             vita_sleep(s)
#define usleep(us)           vita_usleep(us)
#define isatty(fd)           vita_isatty(fd)
#define sysconf(n)           vita_sysconf(n)
#define getpid()             vita_getpid()
#endif
