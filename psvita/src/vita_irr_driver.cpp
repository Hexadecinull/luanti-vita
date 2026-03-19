#include <IrrlichtDevice.h>
#include <irrlicht.h>
#include <IVideoDriver.h>

#include <SDL2/SDL.h>
#include <vitaGL.h>

#include "vita_compat.h"

namespace irr {

IrrlichtDevice *createVitaDevice(const SIrrlichtCreationParameters &params)
{
    SIrrlichtCreationParameters p = params;

    p.DriverType      = video::EDT_OGLES2;
    p.WindowSize      = core::dimension2d<u32>(960, 544);
    p.Bits            = 32;
    p.Fullscreen      = true;
    p.Vsync           = false;
    p.AntiAlias       = 0;
    p.Stencilbuffer   = false;
    p.IgnoreInput     = false;
    p.WindowId        = nullptr;

    return createDeviceEx(p);
}

}
