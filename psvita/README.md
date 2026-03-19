# Luanti — PS Vita Port

This directory contains all PS Vita-specific code, build system, and assets needed to compile Luanti for the PlayStation Vita.

## Directory Layout

```
psvita/
├── CMakeLists.txt              # PS Vita build entry point
├── cmake/
│   ├── VitaSources.cmake       # Source file aggregation
│   ├── FindvitaGL.cmake        # CMake find module for vitaGL/GLES2
│   └── FindVitaLibs.cmake      # CMake find module for all Vita stubs
├── livearea/
│   └── sce_sys/
│       ├── icon0.png           # App icon (128x128)
│       └── livearea/
│           └── contents/
│               ├── bg.png      # LiveArea background (840x500)
│               ├── startup.png # LiveArea gate image (280x158)
│               └── template.xml
├── scripts/
│   ├── build.sh                # One-shot build script
│   ├── apply_patches.sh        # Applies all source patches
│   ├── root_CMakeLists.patch   # Adds Vita detection to root CMakeLists
│   ├── porting_h_vita.patch    # Adds Vita path declarations to porting.h
│   ├── porting_vita.patch      # Implements Vita paths in porting.cpp
│   ├── irr_sdl_vita.patch      # Forces GLES2 in Irrlicht SDL device
│   └── game_vita.patch         # Injects Vita input poll into game loop
└── src/
    ├── vita_main.cpp            # Entry point, system init, clock OC
    ├── vita_input.cpp/.h        # Controller, touch, IME keyboard, gyro
    ├── vita_audio.cpp/.h        # SceAudio channel management
    ├── vita_fs.cpp/.h           # SceIo filesystem helpers
    ├── vita_compat.cpp/.h       # POSIX shims (memalign, gettimeofday…)
    ├── vita_network.cpp/.h      # SceNet initialisation
    ├── vita_platform.cpp/.h     # Platform umbrella, main-thread dispatch
    ├── vita_thread.cpp/.h       # pthread compatibility extras
    ├── vita_debug.cpp/.h        # File logger → ux0:data/luanti/luanti.log
    ├── vita_irr_driver.cpp      # Irrlicht GLES2 device factory
    └── vita_settings_defaults.cpp/.h  # Conservative defaults for Vita HW
```

## Prerequisites

### Toolchain

Install [vitasdk](https://vitasdk.org/):

```bash
git clone https://github.com/vitasdk/vdpm
cd vdpm
./bootstrap-vitasdk.sh
export VITASDK=/usr/local/vitasdk
export PATH=$VITASDK/bin:$PATH
```

### Required vitasdk packages

Install these via `vdpm` or `vita-makepkg`:

| Package         | Provides                         |
|-----------------|----------------------------------|
| SDL2            | Window, events, audio bootstrap  |
| vitaGL          | OpenGL ES 2.0 over GXM           |
| freetype        | Font rendering                   |
| libogg          | Ogg container                    |
| libvorbis       | Vorbis audio decode              |
| libpng          | PNG texture loading              |
| libjpeg         | JPEG texture loading             |
| zlib            | Compression                      |

```bash
vdpm SDL2 vitaGL freetype libogg libvorbis libpng libjpeg zlib
```

All SceXxx stub libraries ship with vitasdk itself.

### Host tools

- `cmake` ≥ 3.12  
- `make`  
- `git`  
- `curl` (for FTP transfer targets)
- `python3` + `Pillow` (only if regenerating LiveArea images)

## Building

### Quick start

```bash
export VITASDK=/usr/local/vitasdk
export PATH=$VITASDK/bin:$PATH

cd psvita/scripts
./apply_patches.sh          # patch luanti source once
./build.sh                  # configure + build → build-vita/luanti.vpk
```

### Manual steps

```bash
export VITASDK=/usr/local/vitasdk

# Patch source tree
bash psvita/scripts/apply_patches.sh

# Configure
mkdir build-vita && cd build-vita
cmake ../psvita \
    -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)
```

### Incremental eboot upload (faster iteration)

After the first VPK install you can push only the updated binary:

```bash
export PSVITAIP=192.168.1.x
make -C build-vita send_eboot
```

### Full VPK upload

```bash
export PSVITAIP=192.168.1.x
make -C build-vita send
```

## Installing on the Vita

1. Enable HENkaku / Ensō homebrew enabler.
2. Enable unsafe homebrew in HENkaku settings.
3. Transfer `build-vita/luanti.vpk` to your Vita via USB or FTP.
4. Install with VitaShell.
5. Launch **Luanti** from the LiveArea.

On first launch, user data is created at `ux0:data/luanti/` and a default `minetest.conf` is copied from the app bundle.

## Control Mapping

| Vita control        | Luanti action                 |
|---------------------|-------------------------------|
| Left stick          | Move (WASD)                   |
| Right stick         | Look / camera (mouse emulation)|
| D-pad               | WASD movement                 |
| ✕ (Cross)           | Confirm / interact            |
| ○ (Circle)          | Cancel / back                 |
| □ (Square)          | Open inventory                |
| △ (Triangle)        | Chat                          |
| L1                  | Sneak (Shift)                 |
| R1                  | Use / aux1 (E)                |
| R2 + right stick    | Left mouse (dig/punch)        |
| L2 + right stick    | Right mouse (place/use)       |
| L3                  | Toggle fly (K)                |
| R3                  | Toggle camera mode (V)        |
| Start               | Pause / escape                |
| Select              | Toggle HUD (F1)               |
| Front touchscreen   | Mouse cursor + left click     |
| Back touchpad       | Scroll / secondary input      |

## Architecture

### Rendering

Luanti's Irrlicht engine is patched to select the OpenGL ES 2 (`EDT_OGLES2`) driver and force a 960×544 fullscreen window. vitaGL provides the GLES2 → GXM translation layer that runs on the PowerVR SGX543MP4 GPU.

Shaders, bumpmapping, waving geometry and 3D clouds are disabled by default in `vita_settings_defaults.cpp` because the GPU and 512 MB of shared RAM cannot sustain them at playable framerates. The view distance defaults to 3 chunks.

### Input

`vita_input.cpp` polls `SceCtrl` and `SceTouch` each frame, translates button events into SDL keyboard/mouse events, and injects them into SDL's event queue before Irrlicht processes them. The right analog stick drives a software mouse cursor for menu navigation and camera look. The PS Vita IME dialog is used for text entry.

### Paths

| Purpose           | Path                              |
|-------------------|-----------------------------------|
| Read-only data    | `app0:data/`                      |
| User saves/worlds | `ux0:data/luanti/`                |
| Log file          | `ux0:data/luanti/luanti.log`      |
| Config            | `ux0:data/luanti/minetest.conf`   |

### Memory

The Vita has 512 MB shared between CPU and GPU. `vita_main.cpp` overclocks the ARM cores to 444 MHz and GPU to 222 MHz at launch. Settings defaults limit the world to small render distances and disable expensive effects so typical gameplay stays within ~380 MB of usable RAM.

## Known Limitations & Future Work

- **Multiplayer networking**: SceNet is initialised and the socket layer is functional, but large servers with high entity counts will stress available memory. LAN play is recommended.
- **Server mode**: The dedicated-server binary is not targeted by this port. `BUILD_SERVER=OFF` is implied.
- **Lua JIT**: The Vita's ARM is not supported by LuaJIT; the standard Lua 5.4 interpreter (already in `lib/lua`) is used instead.
- **Audio**: OpenAL is routed through the SceAudio channel system. Positional audio is functional; the maximum channel count is capped at 8.
- **Mods**: Lua mods work but RAM-intensive content packs may cause out-of-memory crashes. Test with small game packages first.
- **LiveArea assets**: `bg.png`, `startup.png`, and `icon0.png` are placeholder renders. Replace them with proper artwork before a public release.

## Debugging

Enable the file logger by calling `vita_log_init` at startup (already wired in `vita_main.cpp`). After a crash, retrieve `ux0:data/luanti/luanti.log` via VitaShell or FTP for the full log.

For GDB-style debugging, HENkaku's `kubridge` plugin combined with `vitasdk-gdb-stub` can attach to a running process over USB.
