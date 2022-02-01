find_package(PkgConfig)
if (PkgConfig_FOUND)
    pkg_check_modules(PC_AVFORMAT QUIET IMPORTED_TARGET GLOBAL libavformat)
endif()

if (PC_AVFORMAT_FOUND)
    set(AVFORMAT_FOUND TRUE)
    set(AVFORMAT_VERSION ${PC_AVFORMAT_VERSION})
    set(AVFORMAT_VERSION_STRING ${PC_AVFORMAT_STRING})
    set(AVFORMAT_LIBRARYS ${PC_AVFORMAT_LIBRARIES})
    if (USE_STATIC_LIBS)
        set(AVFORMAT_INCLUDE_DIRS ${PC_AVFORMAT_STATIC_INCLUDE_DIRS})
    else()
        set(AVFORMAT_INCLUDE_DIRS ${PC_AVFORMAT_INCLUDE_DIRS})
    endif()
    if (NOT TARGET AVFORMAT::AVFORMAT)
        add_library(AVFORMAT::AVFORMAT ALIAS PkgConfig::PC_AVFORMAT)
    endif()
else()
    message(FATAL_ERROR "failed.")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AVFORMAT
    FOUND_VAR AVFORMAT_FOUND
    REQUIRED_VARS
        AVFORMAT_LIBRARYS
    VERSION_VAR AVFORMAT_VERSION
)
