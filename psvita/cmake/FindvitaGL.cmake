if(NOT DEFINED VITASDK)
    set(VITASDK "$ENV{VITASDK}")
endif()

set(_VITAGL_ROOT "${VITASDK}/arm-vita-eabi")

find_path(VITAGL_INCLUDE_DIR
    NAMES vitaGL.h
    PATHS "${_VITAGL_ROOT}/include"
    NO_DEFAULT_PATH
)

find_library(VITAGL_LIBRARY
    NAMES vitaGL
    PATHS "${_VITAGL_ROOT}/lib"
    NO_DEFAULT_PATH
)

find_library(VITAGLESV2_LIBRARY
    NAMES GLESv2
    PATHS "${_VITAGL_ROOT}/lib"
    NO_DEFAULT_PATH
)

find_library(VITAEGL_LIBRARY
    NAMES EGL
    PATHS "${_VITAGL_ROOT}/lib"
    NO_DEFAULT_PATH
)

find_library(VITAGL_GL_LIBRARY
    NAMES GL
    PATHS "${_VITAGL_ROOT}/lib"
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(vitaGL
    REQUIRED_VARS
        VITAGL_INCLUDE_DIR
        VITAGL_LIBRARY
        VITAGLESV2_LIBRARY
        VITAEGL_LIBRARY
)

if(vitaGL_FOUND AND NOT TARGET vitaGL::vitaGL)
    add_library(vitaGL::vitaGL STATIC IMPORTED)
    set_target_properties(vitaGL::vitaGL PROPERTIES
        IMPORTED_LOCATION             "${VITAGL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${VITAGL_INCLUDE_DIR}"
    )

    add_library(vitaGL::GLESv2 STATIC IMPORTED)
    set_target_properties(vitaGL::GLESv2 PROPERTIES
        IMPORTED_LOCATION "${VITAGLESV2_LIBRARY}"
    )

    add_library(vitaGL::EGL STATIC IMPORTED)
    set_target_properties(vitaGL::EGL PROPERTIES
        IMPORTED_LOCATION "${VITAEGL_LIBRARY}"
    )

    if(VITAGL_GL_LIBRARY)
        add_library(vitaGL::GL STATIC IMPORTED)
        set_target_properties(vitaGL::GL PROPERTIES
            IMPORTED_LOCATION "${VITAGL_GL_LIBRARY}"
        )
    endif()
endif()

mark_as_advanced(
    VITAGL_INCLUDE_DIR
    VITAGL_LIBRARY
    VITAGLESV2_LIBRARY
    VITAEGL_LIBRARY
    VITAGL_GL_LIBRARY
)
