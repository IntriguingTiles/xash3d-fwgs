/*
lib_wiiu.h - libdl compatibility layer for Wii U
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
#pragma once
#if XASH_WIIU
#ifndef WIIU_LIB_H
#define WIIU_LIB_H

#define RTLD_NOW 0

typedef struct {
               const char *dli_fname;  /* Pathname of shared object that
                                          contains address */
               void       *dli_fbase;  /* Base address at which shared
                                          object is loaded */
               const char *dli_sname;  /* Name of symbol whose definition
                                          overlaps addr */
               void       *dli_saddr;  /* Exact address of symbol named
                                          in dli_sname */
} Dl_info;

void *dlsym( void *handle, const char *symbol );
void *dlopen( const char *name, int flag );
int dlclose( void *handle );
const char *dlerror( void );
int dladdr( const void *addr, Dl_info *info );

#endif // WIIU_LIB_H
#endif // XASH_WIIU