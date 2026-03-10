cmake_minimum_required(VERSION 3.13)
project(xash3d C CXX)

find_package(PkgConfig REQUIRED)

set(RPL_ENTRY_STUB "${CMAKE_CURRENT_BINARY_DIR}/rpl_entry_stub.c")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-threadsafe-statics") #  -mstrict-align

file(GENERATE
    OUTPUT ${RPL_ENTRY_STUB}
    CONTENT
"#include <coreinit/dynload.h>
int rpl_entry(OSDynLoad_Module module, OSDynLoad_EntryReason reason){return 0;}")

# include_directories(
#     3rdparty/library_suffix/include
#     3rdparty/MultiEmulator/include
#     common
#     engine
#     engine/common
#     public
#     pm_shared
#     filesystem
# )

add_definitions(
    -DXASH_BUILD_COMMIT="1337"
    -DXASH_BUILD_BRANCH="wiiu"
    -DXASH_BUILD_COMMIT_DATE="2026-03-03"
    -D_GNU_SOURCE=1
    -DALLOCA_H=<alloca.h>
    -DHAVE_TGMATH_H=1
    -DHAVE_STRNCASECMP=1
    -DHAVE_STRCASECMP=1
    -DHAVE_STRCASESTR=1
    -DHAVE_STRCHRNUL=1
    -DHAVE_STRLCPY=1
    -DHAVE_STRLCAT=1
    -DHAVE_STRNLEN=1
)

file(GLOB PUBLIC_SOURCES public/*)
list(REMOVE_ITEM PUBLIC_SOURCES "public/build_vcs.c")
add_library(public STATIC ${PUBLIC_SOURCES})
target_include_directories(public PUBLIC 3rdparty/library_suffix/include common engine public)

add_library(build_vcs STATIC public/build_vcs.c)

add_subdirectory(3rdparty/library_suffix)

set(OPUS_CUSTOM_MODES ON CACHE BOOL "" FORCE)
set(OPUS_FLOAT_APPROX ON CACHE BOOL "" FORCE)
set(OPUS_HARDENING ON CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/opus/opus)

file(GLOB MULTIEMU_SOURCES 3rdparty/MultiEmulator/src/*)
add_library(MultiEmulator STATIC ${MULTIEMU_SOURCES})
target_include_directories(MultiEmulator PUBLIC 3rdparty/MultiEmulator/include)

file(GLOB FILESYSTEM_SOURCES filesystem/*)
add_executable(filesystem_stdio ${FILESYSTEM_SOURCES} ${RPL_ENTRY_STUB})
target_include_directories(filesystem_stdio PUBLIC)
target_link_libraries(filesystem_stdio public library_suffix build_vcs)
wut_add_exports(filesystem_stdio filesystem/wiiu_exports.def)
wut_create_rpl(filesystem_stdio)

file(GLOB REF_SOFT_SOURCES ref/soft/*)
add_executable(libref_soft ${REF_SOFT_SOURCES} ${RPL_ENTRY_STUB})
target_include_directories(libref_soft PUBLIC pm_shared engine/common filesystem)
target_compile_definitions(libref_soft PRIVATE -DREF_DLL=1)
target_link_libraries(libref_soft public)
wut_add_exports(libref_soft ref/soft/wiiu_exports.def)
wut_create_rpl(libref_soft)

# we cannot use mainui's CMakeLists.txt as it wants to create a shared library
file(GLOB MENU_SOURCES
    3rdparty/mainui/*
    3rdparty/mainui/miniutl/*
    3rdparty/mainui/font/*
    3rdparty/mainui/menus/*
    3rdparty/mainui/menus/dynamic/*
    3rdparty/mainui/model/*
    3rdparty/mainui/controls/*
)
add_executable(libmenu ${MENU_SOURCES} ${RPL_ENTRY_STUB})
target_compile_features(libmenu PRIVATE cxx_std_11)
target_compile_definitions(libmenu PRIVATE -DMAINUI_USE_STB=1 -DMAINUI_USE_CUSTOM_FONT_RENDER=1 -DSTDINT_H=<cstdint>)
target_include_directories(libmenu PRIVATE
    3rdparty/mainui
    3rdparty/mainui/miniutl
    3rdparty/mainui/font
    3rdparty/mainui/controls
    3rdparty/mainui/menus
    3rdparty/mainui/model
    3rdparty/mainui/sdk_includes/common
    3rdparty/mainui/sdk_includes/engine
    3rdparty/mainui/sdk_includes/public
    3rdparty/mainui/sdk_includes/pm_shared
)
wut_add_exports(libmenu 3rdparty/mainui/wiiu_exports.def)
wut_create_rpl(libmenu)

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
target_compile_definitions(xash PRIVATE
    -DXASH_GAMEDIR="valve"
    -DHAVE_LARGEFILE=1
    -DHAVE_OGG=1
    -DHAVE_OPUSFILE=1
    -DHAVE_VORBIS=1
    -DHAVE_VORBISFILE=1
    -DHAVE_OPUS=1
    -DXASH_LOW_MEMORY=0
    -DHAVE_SDL2=1
    -DENGINE_DLL=1
    -DXASH_REF_SOFT_ENABLED=1
    -DXASH_ENABLE_MAIN=1
    -DXASH_SDL=2
)
pkg_check_modules(DEPS REQUIRED sdl2 opusfile vorbis vorbisfile)
target_include_directories(xash PRIVATE
    ${DEPS_INCLUDE_DIRS}
    engine/common
    engine/common/imagelib
    engine/common/soundlib
    engine/client
    engine/client/vgui
    engine/server
    engine/platform
    filesystem
    pm_shared
)
target_link_libraries(xash Opus::opus public library_suffix build_vcs MultiEmulator ${DEPS_LIBRARIES} bz2)
wut_create_rpx(xash)
