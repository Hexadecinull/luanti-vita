#pragma once

#ifdef __vita__

#include <cstdio>
#include <cstdarg>

#ifdef __cplusplus
extern "C" {
#endif

void vita_log_init(const char *path);
void vita_log_shutdown();
void vita_log(const char *fmt, ...);
void vita_log_error(const char *fmt, ...);
void vita_log_flush();

#ifdef __cplusplus
}
#endif

#ifndef NDEBUG
#define VITA_LOG(fmt, ...)       vita_log("[LOG] " fmt "\n", ##__VA_ARGS__)
#define VITA_LOG_ERR(fmt, ...)   vita_log_error("[ERR] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define VITA_LOG(fmt, ...)       ((void)0)
#define VITA_LOG_ERR(fmt, ...)   vita_log_error("[ERR] " fmt "\n", ##__VA_ARGS__)
#endif

#endif
