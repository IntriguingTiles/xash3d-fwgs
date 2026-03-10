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
#include <coreinit/debug.h>
#include <coreinit/dynload.h>

static OSDynLoad_Error g_last_dynload_error = 0;

void *dlsym( void *handle, const char *symbol )
{
    void* addr = NULL;
    Con_Printf("Looking for symbol: %s\n", symbol);
	g_last_dynload_error = OSDynLoad_FindExport(handle, OS_DYNLOAD_EXPORT_FUNC, symbol, &addr);
    Con_Printf("success? 0x%X, addr: 0x%X\n", g_last_dynload_error, addr);
	return addr;
}

void *dlopen( const char *name, int flag )
{
    void *module;
    char basepath[256] = {0};
    char *p;
    // replace "fs:/vol/external01 with ~"
    if (Q_strncmp(name, "fs:/vol/external01", sizeof("fs:/vol/external01") - 1) == 0)
    {
        Q_snprintf(basepath, sizeof(basepath), "~%s", name + sizeof("fs:/vol/external01") - 1);
    }
    else
    {
        Q_snprintf(basepath, sizeof(basepath), "~/wiiu/apps/xash3d/%s", name);
    }
    // strip out "/."
    while ((p = Q_strstr(basepath, "/./")) != NULL) {
        memmove(p, p + 2, Q_strlen(p + 2) + 1);
    }
    Con_Printf("Attempting to load library: %s (%s)\n", name, basepath);
    // Platform_Sleep(5000);
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
    return 0;
#if 0
    // TODO: make this not leak memory
    char* symbol = Mem_Calloc(host.mempool, 128);
    OSGetSymbolName((uint32_t)addr, symbol, 128);
    // output looks like ~|wiiu|apps|xash3d|valve|dlls|hl_wiiu_ppc|_ZN15CAmbientGeneric9, so skip through '|' to get the symbol
    char* pipe = Q_strstr(symbol, "|");
    while (pipe != NULL) {
        symbol = pipe + 1;
        pipe = Q_strstr(symbol, "|");
    }
    Con_Printf("OSGetSymbolName: 0x%X -> %s\n", addr, symbol);
    info->dli_sname = symbol;
	return 1;
#endif
}