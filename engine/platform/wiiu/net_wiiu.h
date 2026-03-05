/*
net_psvita.h - psvita network stubs
Copyright (C) 2021-2023 fgsfds

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
#ifndef NET_WIIU_H
#define NET_WIIU_H

#include "platform/posix/net.h"

/* we're missing IPv6 support; define some trash */
#define XASH_NO_IPV6_RESOLVE 1

#ifndef IN6_IS_ADDR_V4MAPPED
#define IN6_IS_ADDR_V4MAPPED( p ) ( 0 )
#endif

#ifndef IPPROTO_IPV6
#define IPPROTO_IPV6 41
#endif

#ifndef IPV6_MULTICAST_LOOP
#define IPV6_MULTICAST_LOOP 19
#endif

#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 26
#endif

#ifndef AF_INET6
#define AF_INET6 23
#endif

#ifndef PF_INET6
#define PF_INET6 AF_INET6
#endif

#ifndef SO_RCVTIMEO
#define SO_RCVTIMEO -1
#endif

typedef struct in6_addr {
	uint8_t s6_addr[16];
} in6_addr;

typedef struct sockaddr_in6 {
	sa_family_t     sin6_family;
	in_port_t       sin6_port;
	uint32_t        sin6_flowinfo;
	struct in6_addr sin6_addr;
	uint32_t        sin6_scope_id;
} sockaddr_in6;

#endif // NET_WIIU_H
