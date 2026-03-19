if(NOT DEFINED VITASDK)
    set(VITASDK "$ENV{VITASDK}")
endif()

set(_VITA_LIB_DIR  "${VITASDK}/arm-vita-eabi/lib")
set(_VITA_INC_DIR  "${VITASDK}/arm-vita-eabi/include")

macro(vita_find_stub _name _var)
    find_library(${_var}
        NAMES ${_name}
        PATHS "${_VITA_LIB_DIR}"
        NO_DEFAULT_PATH
    )
endmacro()

vita_find_stub(SceAudioOut_stub        VITA_AUDIO_OUT_STUB)
vita_find_stub(SceAudioIn_stub         VITA_AUDIO_IN_STUB)
vita_find_stub(SceCtrl_stub            VITA_CTRL_STUB)
vita_find_stub(SceTouch_stub           VITA_TOUCH_STUB)
vita_find_stub(SceDisplay_stub         VITA_DISPLAY_STUB)
vita_find_stub(SceGxm_stub             VITA_GXM_STUB)
vita_find_stub(SceKernel_stub          VITA_KERNEL_STUB)
vita_find_stub(ScePower_stub           VITA_POWER_STUB)
vita_find_stub(SceSysmodule_stub       VITA_SYSMODULE_STUB)
vita_find_stub(SceCommonDialog_stub    VITA_COMMON_DIALOG_STUB)
vita_find_stub(SceIme_stub             VITA_IME_STUB)
vita_find_stub(SceHid_stub             VITA_HID_STUB)
vita_find_stub(SceMotion_stub          VITA_MOTION_STUB)
vita_find_stub(SceNet_stub             VITA_NET_STUB)
vita_find_stub(SceNetCtl_stub          VITA_NETCTL_STUB)
vita_find_stub(SceAppMgr_stub          VITA_APPMGR_STUB)
vita_find_stub(SceAppUtil_stub         VITA_APPUTIL_STUB)
vita_find_stub(ScePgf_stub             VITA_PGF_STUB)

find_library(VITA_SDL2_LIB      NAMES SDL2       PATHS "${_VITA_LIB_DIR}" NO_DEFAULT_PATH)
find_library(VITA_FREETYPE_LIB  NAMES freetype   PATHS "${_VITA_LIB_DIR}" NO_DEFAULT_PATH)
find_library(VITA_PNG_LIB       NAMES png16 png  PATHS "${_VITA_LIB_DIR}" NO_DEFAULT_PATH)
find_library(VITA_JPEG_LIB      NAMES jpeg       PATHS "${_VITA_LIB_DIR}" NO_DEFAULT_PATH)
find_library(VITA_ZLIB_LIB      NAMES z          PATHS "${_VITA_LIB_DIR}" NO_DEFAULT_PATH)
find_library(VITA_VORBISFILE_LIB NAMES vorbisfile PATHS "${_VITA_LIB_DIR}" NO_DEFAULT_PATH)
find_library(VITA_VORBIS_LIB    NAMES vorbis     PATHS "${_VITA_LIB_DIR}" NO_DEFAULT_PATH)
find_library(VITA_OGG_LIB       NAMES ogg        PATHS "${_VITA_LIB_DIR}" NO_DEFAULT_PATH)

find_path(VITA_SDL2_INCLUDE     NAMES SDL.h      PATHS "${_VITA_INC_DIR}/SDL2" NO_DEFAULT_PATH)
find_path(VITA_FREETYPE_INCLUDE NAMES ft2build.h PATHS "${_VITA_INC_DIR}/freetype2" NO_DEFAULT_PATH)

set(VITA_ALL_STUB_LIBS
    ${VITA_AUDIO_OUT_STUB}
    ${VITA_AUDIO_IN_STUB}
    ${VITA_CTRL_STUB}
    ${VITA_TOUCH_STUB}
    ${VITA_DISPLAY_STUB}
    ${VITA_GXM_STUB}
    ${VITA_KERNEL_STUB}
    ${VITA_POWER_STUB}
    ${VITA_SYSMODULE_STUB}
    ${VITA_COMMON_DIALOG_STUB}
    ${VITA_IME_STUB}
    ${VITA_HID_STUB}
    ${VITA_MOTION_STUB}
    ${VITA_NET_STUB}
    ${VITA_NETCTL_STUB}
    ${VITA_APPMGR_STUB}
    ${VITA_APPUTIL_STUB}
    ${VITA_PGF_STUB}
)

set(VITA_ALL_LIBS
    ${VITA_SDL2_LIB}
    ${VITA_FREETYPE_LIB}
    ${VITA_PNG_LIB}
    ${VITA_JPEG_LIB}
    ${VITA_ZLIB_LIB}
    ${VITA_VORBISFILE_LIB}
    ${VITA_VORBIS_LIB}
    ${VITA_OGG_LIB}
    ${VITA_ALL_STUB_LIBS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VitaLibs
    REQUIRED_VARS
        VITA_SDL2_LIB
        VITA_KERNEL_STUB
        VITA_GXM_STUB
)
