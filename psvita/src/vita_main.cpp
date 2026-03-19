#include <psp2/kernel/processmgr.h>
#include <psp2/appmgr.h>
#include <psp2/apputil.h>
#include <psp2/power.h>
#include <psp2/sysmodule.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/ime_dialog.h>

#include <SDL2/SDL.h>

#include <vitaGL.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include "vita_compat.h"
#include "vita_fs.h"
#include "vita_audio.h"

int psvita_argc = 0;
char **psvita_argv = nullptr;

extern int luanti_main(int argc, char *argv[]);

static void vita_load_modules()
{
    sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
    sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);
    sceSysmoduleLoadModule(SCE_SYSMODULE_HTTPS);
    sceSysmoduleLoadModule(SCE_SYSMODULE_SSL);
    sceSysmoduleLoadModule(SCE_SYSMODULE_SQLITE);
    sceSysmoduleLoadModule(SCE_SYSMODULE_AVPLAYER);
    sceSysmoduleLoadModule(SCE_SYSMODULE_RUDP);
}

static void vita_init_apputil()
{
    SceAppUtilInitParam init_param;
    SceAppUtilBootParam boot_param;
    memset(&init_param, 0, sizeof(init_param));
    memset(&boot_param, 0, sizeof(boot_param));
    sceAppUtilInit(&init_param, &boot_param);
}

static void vita_init_gl()
{
    vglInitExtended(0, 960, 544, 0x1800000, SCE_GXM_MULTISAMPLE_NONE);
    vglUseVram(GL_TRUE);
    vglMapHeapMem();
}

static void vita_init_sdl()
{
    SDL_setenv("VITA_RESOLUTION", "544", 1);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK |
             SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS);
}

static void vita_setup_paths()
{
    vita_fs_mkdir("ux0:data/luanti");
    vita_fs_mkdir("ux0:data/luanti/worlds");
    vita_fs_mkdir("ux0:data/luanti/games");
    vita_fs_mkdir("ux0:data/luanti/mods");
    vita_fs_mkdir("ux0:data/luanti/textures");
    vita_fs_mkdir("ux0:data/luanti/screenshots");

    vita_fs_copy_if_missing(
        "app0:data/minetest.conf.example",
        "ux0:data/luanti/minetest.conf"
    );
}

static const char *vita_argv_data[] = {
    "luanti",
    "--config",  "ux0:data/luanti/minetest.conf",
    "--user-data-path", "ux0:data/luanti",
    "--game-path", "app0:data/games",
    "--worldpath", "ux0:data/luanti/worlds",
    "--texturepath", "ux0:data/luanti/textures",
    "--run-in-place"
};

int main(int argc, char *argv[])
{
    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    vita_load_modules();
    vita_init_apputil();
    vita_init_gl();
    vita_init_sdl();
    vita_setup_paths();
    vita_audio_init();

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK,  SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);

    psvita_argc = sizeof(vita_argv_data) / sizeof(vita_argv_data[0]);
    psvita_argv = const_cast<char **>(vita_argv_data);

    int ret = luanti_main(psvita_argc, psvita_argv);

    vita_audio_shutdown();
    SDL_Quit();
    vglEnd();
    sceKernelExitProcess(ret);
    return ret;
}
