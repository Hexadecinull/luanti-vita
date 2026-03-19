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

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(vitaGL
    REQUIRED_VARS
        VITAGL_INCLUDE_DIR
        VITAGL_LIBRARY
)

if(vitaGL_FOUND AND NOT TARGET vitaGL::vitaGL)
    add_library(vitaGL::vitaGL STATIC IMPORTED)
    set_target_properties(vitaGL::vitaGL PROPERTIES
        IMPORTED_LOCATION             "${VITAGL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${VITAGL_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(VITAGL_INCLUDE_DIR VITAGL_LIBRARY)
