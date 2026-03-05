cmake_minimum_required(VERSION 3.13)
project(xash3d C CXX)

find_package(PkgConfig REQUIRED)

set(RPL_ENTRY_STUB "${CMAKE_CURRENT_BINARY_DIR}/rpl_entry_stub.c")

file(GENERATE
    OUTPUT ${RPL_ENTRY_STUB}
    CONTENT
"#include <coreinit/dynload.h>
int rpl_entry(OSDynLoad_Module module, OSDynLoad_EntryReason reason){return 0;}")

include_directories(3rdparty/library_suffix/include)
include_directories(3rdparty/MultiEmulator/include)
include_directories(common)
include_directories(engine)
include_directories(engine/common)
include_directories(public)
include_directories(pm_shared)
include_directories(filesystem)

add_definitions(-DXASH_BUILD_COMMIT="1337")
add_definitions(-DXASH_BUILD_BRANCH="wiiu")
add_definitions(-DXASH_BUILD_COMMIT_DATE="2026-03-03")
add_definitions(-D_GNU_SOURCE=1)
add_definitions(-DALLOCA_H=<alloca.h>)
add_definitions(-DHAVE_TGMATH_H=1)
add_definitions(-DHAVE_STRNCASECMP=1)
add_definitions(-DHAVE_STRCASECMP=1)
add_definitions(-DHAVE_STRCASESTR=1)
add_definitions(-DHAVE_STRCHRNUL=1)
add_definitions(-DHAVE_STRLCPY=1)
add_definitions(-DHAVE_STRLCAT=1)
add_definitions(-DHAVE_STRNLEN=1)

file(GLOB PUBLIC_SOURCES public/*)
list(REMOVE_ITEM PUBLIC_SOURCES "public/build_vcs.c")
add_library(public STATIC ${PUBLIC_SOURCES})
add_library(build_vcs STATIC public/build_vcs.c)

add_subdirectory(3rdparty/library_suffix)

set(OPUS_CUSTOM_MODES ON CACHE BOOL "" FORCE)
set(OPUS_FLOAT_APPROX ON CACHE BOOL "" FORCE)
set(OPUS_HARDENING ON CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/opus/opus)

file(GLOB MULTIEMU_SOURCES 3rdparty/MultiEmulator/src/*)
add_library(MultiEmulator STATIC ${MULTIEMU_SOURCES})

file(GLOB FILESYSTEM_SOURCES filesystem/*)
add_executable(filesystem_stdio ${FILESYSTEM_SOURCES} ${RPL_ENTRY_STUB})
target_link_libraries(filesystem_stdio public library_suffix build_vcs)
wut_add_exports(filesystem_stdio filesystem/wiiu_exports.def)
wut_create_rpl(filesystem_stdio)

file(GLOB REF_SOFT_SOURCES ref/soft/*)
add_executable(ref_soft ${REF_SOFT_SOURCES} ${RPL_ENTRY_STUB})
target_compile_definitions(ref_soft PRIVATE -DREF_DLL=1)
target_link_libraries(ref_soft public)
wut_add_exports(ref_soft ref/soft/wiiu_exports.def)
wut_create_rpl(ref_soft)

file(GLOB ENGINE_SOURCES
    engine/common/*
    engine/common/imagelib/*
    engine/common/soundlib/*
    engine/server/*
    engine/platform/wiiu/*
    engine/platform/posix/*
    engine/platform/sdl2/*
    engine/client/*
    engine/client/avi/*
    engine/client/soundlib/*
    engine/client/soundlib/libmpg/*
    engine/client/vgui/*
    )
add_executable(xash ${ENGINE_SOURCES} engine/common/http/net_http_xash.c )
target_compile_definitions(xash PRIVATE -DXASH_GAMEDIR="valve")
target_compile_definitions(xash PRIVATE -DHAVE_LARGEFILE=1)
target_compile_definitions(xash PRIVATE -DHAVE_OGG=1)
target_compile_definitions(xash PRIVATE -DHAVE_OPUSFILE=1)
target_compile_definitions(xash PRIVATE -DHAVE_VORBIS=1)
target_compile_definitions(xash PRIVATE -DHAVE_VORBISFILE=1)
target_compile_definitions(xash PRIVATE -DHAVE_OPUS=1)
target_compile_definitions(xash PRIVATE -DXASH_LOW_MEMORY=0)
target_compile_definitions(xash PRIVATE -DHAVE_SDL2=1)
target_compile_definitions(xash PRIVATE -DENGINE_DLL=1)
target_compile_definitions(xash PRIVATE -DXASH_REF_SOFT_ENABLED=1)
target_compile_definitions(xash PRIVATE -DXASH_ENABLE_MAIN=1)
target_compile_definitions(xash PRIVATE -DXASH_SDL=2)
pkg_check_modules(DEPS REQUIRED sdl2 opusfile vorbis vorbisfile)
target_include_directories(xash PRIVATE ${DEPS_INCLUDE_DIRS} engine/common/imagelib engine/common/soundlib engine/client engine/client/vgui engine/server engine/platform)
target_link_libraries(xash Opus::opus public library_suffix build_vcs MultiEmulator ${DEPS_LIBRARIES} bz2)
wut_create_rpx(xash)
