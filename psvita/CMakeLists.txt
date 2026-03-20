cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake"
        CACHE PATH "Vita toolchain file")
endif()

project(luanti CXX C)

include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(VITA_APP_NAME "Luanti")
set(VITA_TITLEID  "LNTI00001")
set(VITA_VERSION  "01.00")
set(VITA_MKSFOEX_FLAGS "-d ATTRIBUTE2=12")

set(LUANTI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(CMAKE_MODULE_PATH
    "${LUANTI_ROOT}/cmake/Modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    ${CMAKE_MODULE_PATH}
)

set(VITASDK_ROOT "$ENV{VITASDK}/arm-vita-eabi")



add_compile_definitions(
    __vita__
    __psp2__
    PSVITA
    _GNU_SOURCE
    HAVE_OPENGL
    USE_GLES2
    NDEBUG
    _REENTRANT
    IRRLICHT_VITA
    PROJECT_NAME="luanti"
    PROJECT_NAME_C="Luanti"
    VERSION_MAJOR=5
    VERSION_MINOR=16
    VERSION_PATCH=0
    VERSION_STRING="5.16.0-vita"
    BUILD_DIR_PATH="ux0:data/luanti"
    RUN_IN_PLACE=1
)

set(VITA_C_FLAGS   "-O2 -ffast-math -mtune=cortex-a9 -mfpu=neon -mfloat-abi=hard")
set(VITA_CXX_FLAGS "${VITA_C_FLAGS} -fno-exceptions -Wno-deprecated-declarations -Wno-unused-variable")

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${VITA_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VITA_CXX_FLAGS}")



file(WRITE "${CMAKE_BINARY_DIR}/config.h"
"#pragma once\n"
"#define PROJECT_NAME \"luanti\"\n"
"#define PROJECT_NAME_C \"Luanti\"\n"
"#define VERSION_MAJOR 5\n"
"#define VERSION_MINOR 16\n"
"#define VERSION_PATCH 0\n"
"#define VERSION_STRING \"5.16.0-vita\"\n"
"#define USE_GLES2 1\n"
"#define HAVE_OPENGL 1\n"
"#define RUN_IN_PLACE 1\n"
"#define BUILD_DIR_PATH \"ux0:data/luanti\"\n"
)

find_package(vitaGL REQUIRED)

set(VITASDK_STUB_LIBS
    SceAudioOut_stub
    SceAudioIn_stub
    SceCtrl_stub
    SceTouch_stub
    SceDisplay_stub
    SceGxm_stub
    SceKernel_stub
    ScePower_stub
    SceSysmodule_stub
    SceCommonDialog_stub
    SceIme_stub
    SceHid_stub
    SceMotion_stub
    SceNet_stub
    SceNetCtl_stub
    SceAppMgr_stub
    SceAppUtil_stub
    ScePgf_stub
)

set(VITASDK_PORT_LIBS
    SDL2
    vitaGL::vitaGL
    vorbisfile
    vorbis
    ogg
    freetype
    png16
    jpeg
    z
    m
    c
    stdc++
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/VitaSources.cmake")

add_executable(${PROJECT_NAME}
    ${VITA_PLATFORM_SOURCES}
    ${LUANTI_CLIENT_SOURCES}
    ${LUANTI_COMMON_SOURCES}
    ${LUANTI_IRRLICHT_SOURCES}
    ${LUANTI_LUA_SOURCES}
    ${LUANTI_JSON_SOURCES}
    ${LUANTI_TINYGETTEXT_SOURCES}
    ${LUANTI_SHA256_SOURCES}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -fvisibility=hidden
    -Wno-error
    -Wno-deprecated-declarations
    -Wno-unused-parameter
    -Wno-sign-compare
    -Wno-implicit-fallthrough
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_BINARY_DIR}"
    "${VITASDK_ROOT}/include"
    "${VITASDK_ROOT}/include/SDL2"
    "${VITASDK_ROOT}/include/freetype2"
    "${VITAGL_INCLUDE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    "${LUANTI_ROOT}/src"
    "${LUANTI_ROOT}/src/client"
    "${LUANTI_ROOT}/src/server"
    "${LUANTI_ROOT}/src/util"
    "${LUANTI_ROOT}/irr/include"
    "${LUANTI_ROOT}/lib/lua/src"
    "${LUANTI_ROOT}/lib/jsoncpp/include"
    "${LUANTI_ROOT}/lib/tinygettext"
    "${LUANTI_ROOT}/lib/sha256"
)

target_link_libraries(${PROJECT_NAME}
    -Wl,-q
    -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
    ${VITASDK_PORT_LIBS}
    ${VITASDK_STUB_LIBS}
)

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} UNSAFE)

vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    ${VITA_MKSFOEX_FLAGS}
    FILE "${CMAKE_CURRENT_SOURCE_DIR}/livearea/sce_sys/livearea/contents/bg.png"
         "sce_sys/livearea/contents/bg.png"
    FILE "${CMAKE_CURRENT_SOURCE_DIR}/livearea/sce_sys/livearea/contents/startup.png"
         "sce_sys/livearea/contents/startup.png"
    FILE "${CMAKE_CURRENT_SOURCE_DIR}/livearea/sce_sys/livearea/contents/template.xml"
         "sce_sys/livearea/contents/template.xml"
    FILE "${CMAKE_CURRENT_SOURCE_DIR}/livearea/sce_sys/icon0.png"
         "sce_sys/icon0.png"
    FILE "${LUANTI_ROOT}/builtin"    "data/builtin"
    FILE "${LUANTI_ROOT}/client"     "data/client"
    FILE "${LUANTI_ROOT}/fonts"      "data/fonts"
    FILE "${LUANTI_ROOT}/games"      "data/games"
    FILE "${LUANTI_ROOT}/textures"   "data/textures"
    FILE "${LUANTI_ROOT}/minetest.conf.example" "data/minetest.conf.example"
)

add_custom_target(send
    COMMAND curl -T ${PROJECT_NAME}.vpk "ftp://$ENV{PSVITAIP}:1337/ux0:/"
    DEPENDS ${PROJECT_NAME}.vpk
    COMMENT "Uploading VPK to PS Vita via FTP"
)

add_custom_target(send_eboot
    COMMAND curl -T ${PROJECT_NAME}.self
            "ftp://$ENV{PSVITAIP}:1337/ux0:app/${VITA_TITLEID}/eboot.bin"
    DEPENDS ${PROJECT_NAME}.self
    COMMENT "Uploading eboot.bin to PS Vita via FTP"
)
