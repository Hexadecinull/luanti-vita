#pragma once

#include <cstdint>
#include <cstddef>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int  vita_fs_mkdir(const char *path);
int  vita_fs_mkdir_recursive(const char *path);
bool vita_fs_exists(const char *path);
bool vita_fs_is_dir(const char *path);
int  vita_fs_copy(const char *src, const char *dst);
int  vita_fs_copy_if_missing(const char *src, const char *dst);
int  vita_fs_remove(const char *path);
int  vita_fs_rename(const char *src, const char *dst);
long vita_fs_file_size(const char *path);

#ifdef __cplusplus
}
#endif
