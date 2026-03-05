/*
lib_wiiu.c - libdl compatibility layer for Wii U
Copyright (C) 2015-2026 Xash3D FWGS contributors

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "platform/platform.h"
#include "platform/wiiu/lib_wiiu.h"
#include <coreinit/dynload.h>

static OSDynLoad_Error g_last_dynload_error = 0;

void *dlsym( void *handle, const char *symbol )
{
    void* addr;
	g_last_dynload_error = OSDynLoad_FindExport(handle, OS_DYNLOAD_EXPORT_FUNC, symbol, &addr);
	return addr;
}

void *dlopen( const char *name, int flag )
{
    void *module;
    char basepath[256] = {0};
    Q_snprintf(basepath, sizeof(basepath), "~/wiiu/xash3d/%s", name);
    Con_Printf("Attempting to load library: %s (%s)\n", name, basepath);
	g_last_dynload_error = OSDynLoad_Acquire(basepath, &module);
	return module;
}

int dlclose( void *handle )
{
	OSDynLoad_Release(handle);
    return 0;
}

const char *dlerror( void )
{
    switch (g_last_dynload_error)
    {
        case OS_DYNLOAD_OUT_OF_MEMORY:
            return "Out of memory";
        case OS_DYNLOAD_INVALID_NOTIFY_PTR:
            return "Invalid notify pointer";
        case OS_DYNLOAD_INVALID_MODULE_NAME_PTR:
            return "Invalid module name pointer";
        case OS_DYNLOAD_INVALID_MODULE_NAME:
            return "Invalid module name";
        case OS_DYNLOAD_INVALID_ACQUIRE_PTR:
            return "Invalid acquire pointer";
        case OS_DYNLOAD_EMPTY_MODULE_NAME:
            return "Empty module name";
        case OS_DYNLOAD_INVALID_ALLOCATOR_PTR:
            return "Invalid allocator pointer";
        case OS_DYNLOAD_OUT_OF_SYSTEM_MEMORY:
            return "Out of system memory";
        case OS_DYNLOAD_TLS_ALLOCATOR_LOCKED:
            return "TLS allocator locked";
        case OS_DYNLOAD_MODULE_NOT_FOUND:
            return "Module not found";
        default:
            return NULL;
    }
}

int dladdr( const void *addr, Dl_info *info )
{
    // TODO: OSGetSymbolName
	return 0;
}