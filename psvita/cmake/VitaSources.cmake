set(LUANTI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(VITA_PLATFORM_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_input.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_compat.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_fs.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_audio.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_network.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_platform.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_settings_defaults.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_irr_driver.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_thread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_debug.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vita_prefix.h"
)

file(GLOB_RECURSE LUANTI_CLIENT_SOURCES
    "${LUANTI_ROOT}/src/client/*.cpp"
    "${LUANTI_ROOT}/src/client/*.c"
)

file(GLOB_RECURSE LUANTI_COMMON_SOURCES
    "${LUANTI_ROOT}/src/*.cpp"
    "${LUANTI_ROOT}/src/*.c"
)

list(FILTER LUANTI_COMMON_SOURCES EXCLUDE REGEX ".*/client/.*")
list(FILTER LUANTI_COMMON_SOURCES EXCLUDE REGEX ".*/server/.*")

file(GLOB_RECURSE LUANTI_IRRLICHT_SOURCES
    "${LUANTI_ROOT}/irr/source/Irrlicht/*.cpp"
    "${LUANTI_ROOT}/irr/source/Irrlicht/*.c"
)

list(FILTER LUANTI_IRRLICHT_SOURCES EXCLUDE REGEX ".*Direct3D.*")
list(FILTER LUANTI_IRRLICHT_SOURCES EXCLUDE REGEX ".*WinCE.*")
list(FILTER LUANTI_IRRLICHT_SOURCES EXCLUDE REGEX ".*Windows.*")
list(FILTER LUANTI_IRRLICHT_SOURCES EXCLUDE REGEX ".*OSX.*")
list(FILTER LUANTI_IRRLICHT_SOURCES EXCLUDE REGEX ".*MacOS.*")
list(FILTER LUANTI_IRRLICHT_SOURCES EXCLUDE REGEX ".*Android.*")

file(GLOB_RECURSE LUANTI_LUA_SOURCES
    "${LUANTI_ROOT}/lib/lua/src/*.c"
)
list(FILTER LUANTI_LUA_SOURCES EXCLUDE REGEX ".*luac\\.c$")
list(FILTER LUANTI_LUA_SOURCES EXCLUDE REGEX ".*lua\\.c$")

file(GLOB_RECURSE LUANTI_JSON_SOURCES
    "${LUANTI_ROOT}/lib/jsoncpp/src/*.cpp"
)

file(GLOB_RECURSE LUANTI_TINYGETTEXT_SOURCES
    "${LUANTI_ROOT}/lib/tinygettext/*.cpp"
)

file(GLOB_RECURSE LUANTI_SHA256_SOURCES
    "${LUANTI_ROOT}/lib/sha256/*.c"
)
