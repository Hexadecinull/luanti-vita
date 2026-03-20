#ifdef __vita__

#include "vita_network.h"

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/sysmodule.h>

#include <cstdlib>
#include <cstdio>

static constexpr int NET_INIT_SIZE = 1 * 1024 * 1024;
static void         *g_net_memory  = nullptr;
static bool          g_net_ready   = false;

int vita_network_init()
{
    if (g_net_ready) return 0;

    sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

    g_net_memory = malloc(NET_INIT_SIZE);
    if (!g_net_memory) return -1;

    SceNetInitParam param;
    param.memory    = g_net_memory;
    param.size      = NET_INIT_SIZE;
    param.flags     = 0;

    int ret = sceNetInit(&param);
    if (ret < 0) {
        free(g_net_memory);
        g_net_memory = nullptr;
        return ret;
    }

    ret = sceNetCtlInit();
    if (ret < 0) {
        sceNetTerm();
        free(g_net_memory);
        g_net_memory = nullptr;
        return ret;
    }

    g_net_ready = true;
    return 0;
}

void vita_network_shutdown()
{
    if (!g_net_ready) return;
    sceNetCtlTerm();
    sceNetTerm();
    if (g_net_memory) {
        free(g_net_memory);
        g_net_memory = nullptr;
    }
    g_net_ready = false;
}

bool vita_network_is_connected()
{
    if (!g_net_ready) return false;
    SceNetCtlInfo info;
    int ret = sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &info);
    return ret >= 0;
}

#endif
