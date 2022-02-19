cmake_minimum_required(VERSION 3.0)

project(MusicPlayer2_ffmpeg_core)

if (MSVC)
    add_compile_options(/utf-8)  # 让编译器使用UTF-8编码
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
find_package(AVFORMAT 58 REQUIRED)
find_package(AVCODEC 58 REQUIRED)
find_package(AVDEVICE 58 REQUIRED)
find_package(AVUTIL 56 REQUIRED)
find_package(AVFILTER 7 REQUIRED)
find_package(SWRESAMPLE 4 REQUIRED)
find_package(SDL2 REQUIRED)

set(ENABLE_ICONV OFF CACHE BOOL "Libiconv is not needed.")
add_subdirectory(utils)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/utils")

set(CORE_FILES
ffmpeg_core.h
src/core.h
src/core.cpp
src/decode.h
src/decode.c
src/open.h
src/open.c
"src/output.h"
"src/output.c"
src/loop.h
src/loop.c
"src/filter.h"
"src/filter.c"
src/volume.h
src/volume.c
src/speed.h
src/speed.c
src/fft_data.h
src/fft_data.c
src/cda.h
src/cda.c
src/file.h
src/file.cpp
src/equalizer_settings.h
src/equalizer_settings.cpp
src/equalizer.h
src/equalizer.c
"${CMAKE_CURRENT_BINARY_DIR}/ffmpeg_core.rc"
"${CMAKE_CURRENT_BINARY_DIR}/ffmpeg_core_version.h"
)

set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/ffmpeg_core.rc" PROPERTIES GENERATED TRUE)
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/ffmpeg_core_version.h" PROPERTIES GENERATED TRUE)

add_custom_target(ffmpeg_core_version
    ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/FFMPEG_CORE_VERSION.cmake"
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(PUBLIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg_core.h" "${CMAKE_CURRENT_BINARY_DIR}/ffmpeg_core_version.h")

add_library(ffmpeg_core SHARED "${CORE_FILES}")
add_dependencies(ffmpeg_core ffmpeg_core_version)
set_target_properties(ffmpeg_core PROPERTIES PREFIX "")
set_target_properties(ffmpeg_core PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
target_compile_definitions(ffmpeg_core PRIVATE BUILD_FFMPEG_CORE)
target_link_libraries(ffmpeg_core AVFORMAT::AVFORMAT)
target_link_libraries(ffmpeg_core AVCODEC::AVCODEC)
target_link_libraries(ffmpeg_core AVDEVICE::AVDEVICE)
target_link_libraries(ffmpeg_core AVUTIL::AVUTIL)
target_link_libraries(ffmpeg_core AVFILTER::AVFILTER)
target_link_libraries(ffmpeg_core SWRESAMPLE::SWRESAMPLE)
target_link_libraries(ffmpeg_core SDL2::Core)
target_link_libraries(ffmpeg_core SDL2::Main)
target_link_libraries(ffmpeg_core utils)

if (NOT MSVC)
    target_link_libraries(ffmpeg_core m)
endif()

install(TARGETS ffmpeg_core)
if (MSVC)
    install(FILES $<TARGET_PDB_FILE:ffmpeg_core> DESTINATION bin OPTIONAL)
endif()
