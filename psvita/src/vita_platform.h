#pragma once

#ifdef __vita__

#include "vita_compat.h"
#include "vita_fs.h"
#include "vita_input.h"
#include "vita_audio.h"

#define PLATFORM_VITA 1

#define VITA_SCREEN_WIDTH  960
#define VITA_SCREEN_HEIGHT 544

#define LUANTI_USER_PATH   "ux0:data/luanti"
#define LUANTI_DATA_PATH   "app0:data"
#define LUANTI_GAMES_PATH  "app0:data/games"
#define LUANTI_WORLD_PATH  "ux0:data/luanti/worlds"

#define DISABLE_NETWORKING 0

#define VITA_MAX_MEMORY_MB 380

#define LUANTI_VITA_RENDER_DISTANCE_DEFAULT 3
#define LUANTI_VITA_MESH_BUFFER_SIZE        (1 * 1024 * 1024)

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN PATH_MAX
#endif

#ifndef SIGPIPE
#define SIGPIPE 13
#endif

#ifndef SIGCHLD
#define SIGCHLD 20
#endif

#ifndef SIG_IGN
#define SIG_IGN ((void(*)(int))1)
#endif

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

#ifndef NI_MAXSERV
#define NI_MAXSERV 32
#endif

#ifdef __cplusplus
#include <functional>
namespace vita {
    void run_on_main_thread(std::function<void()> fn);
    bool is_main_thread();
}
#endif

#endif
