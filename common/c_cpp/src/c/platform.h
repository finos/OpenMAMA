/* $Id: platform.h,v 1.9.16.2 2011/08/10 14:53:24 nicholasmarriott Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef _PLATFORM_H__
#define _PLATFORM_H__
                                                                                                                                              
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
#include "wombat/wincompat.h"
#include <dlfcn.h>
#include <sys/types.h>
#include <netinet/in.h>

#ifdef WIN32
typedef HINSTANCE   LIB_HANDLE;
#else
typedef void*       LIB_HANDLE;
#endif

/* The platform specific prefix and postfix will automatically be added to the libname
     e.g on Linux "mamaImpl" will load "libmamaimpl.so" */
COMMONExpDLL 
LIB_HANDLE openSharedLib (const char* libName, const char* path);

COMMONExpDLL 
int closeSharedLib (LIB_HANDLE handle);

COMMONExpDLL 
void* loadLibFunc (LIB_HANDLE handle, const char* funcName);

COMMONExpDLL 
char* getLibError (void);

/* Network conversion function */
#if defined(WIN32) || __BYTE_ORDER == __LITTLE_ENDIAN
#define htonll(x) \
    ((uint64_t)htonl((uint32_t)((x)>>32)) | (uint64_t)htonl((uint32_t)(x))<<32)
#define ntohll(x) \
    ((uint64_t)ntohl((uint32_t)((x)>>32)) | (uint64_t)ntohl((uint32_t)(x))<<32)
#else
#define htonll(x) ((uint64_t)(x))
#define ntohll(x) ((uint64_t)(x))
#endif
 
#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /*PLATFORM_H_*/
