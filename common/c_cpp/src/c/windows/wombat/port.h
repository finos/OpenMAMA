/*
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
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

#ifndef WINDOWS_H__
#define WINDOWS_H__

#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <io.h>
#include <malloc.h>
#include <float.h>
#include <tchar.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <tlhelp32.h>
#include <time.h>

#include "wombat/targetsxs.h"
#include "wombat/wConfig.h"
#include "windows/lock.h"
#include "windows/mmap.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* So Windows compiler ignores gcc __attribute__(x) */
#define __attribute__(a)

/* Type for handle to dynamically loaded library */
typedef HINSTANCE   LIB_HANDLE;

/* Missing typedefs */
typedef void * caddr_t;
typedef u_long in_addr_t;

typedef unsigned char    uint8_t;
typedef char             int8_t;
typedef unsigned short   uint16_t;
typedef short            int16_t;
typedef unsigned int     uint32_t;
typedef int              int32_t;
typedef unsigned __int64 uint64_t;
typedef __int64          int64_t;


/* suffix for shared libraries */
#ifdef _DEBUG
#define LIB_EXTENSION "mdd.dll"
#else
#define LIB_EXTENSION "md.dll"
#endif

/* 8 byte int typedefs */
typedef unsigned __int64 w_u64_t;
typedef __int64 w_i64_t;

/* Network conversion function */
#define htonll(x) \
    ((uint64_t)htonl((uint32_t)((x)>>32)) | (uint64_t)htonl((uint32_t)(x))<<32)
#define ntohll(x) \
    ((uint64_t)ntohl((uint32_t)((x)>>32)) | (uint64_t)ntohl((uint32_t)(x))<<32)

/* For delimiting multiple paths in env variables properties */
#define PATH_DELIM ';'

#define PATHSEP  "\\"
COMMONExpDLL int 
wsocketstartup (void);

COMMONExpDLL void
wsocketcleanup (void);

/* Socket Pair and set non blocking */
COMMONExpDLL int
wsocketpair (int domain, int type, int protocol, int* pair);

COMMONExpDLL int
wsetnonblock (int s);

#define shutdown(x, y) closesocket((x))

/* net work utility functions */
COMMONExpDLL const char* getIpAddress (void);
COMMONExpDLL const char* getHostName (void);
COMMONExpDLL const char* getlogin (void);
COMMONExpDLL struct in_addr wresolve_ip (const char * arg);

/* Thread local storage */
typedef DWORD wthread_key_t;

COMMONExpDLL int
wthread_key_create(wthread_key_t* key, void* val);

#define wthread_key_delete(x) TlsFree(x)
#define wthread_setspecific(x, val) TlsSetValue((x),(void*)((val)))
#define wthread_getspecific(x) TlsGetValue((x))

/* Posix Semaphores for Windows */
typedef void* wsem_t;

COMMONExpDLL
int wsem_init (wsem_t* sem, int dummy, int count);

COMMONExpDLL
int wsem_destroy (wsem_t* sem);

COMMONExpDLL
int wsem_post (wsem_t* sem);

COMMONExpDLL
int wsem_wait (wsem_t* sem);

COMMONExpDLL
int wsem_timedwait (wsem_t* sem, unsigned int ts);

COMMONExpDLL
int wsem_trywait (wsem_t* sem);

COMMONExpDLL
int wsem_getvalue (wsem_t*, int* items);

/* These functions are different on Windows */
#define bzero( x, y ) ZeroMemory( ((void *)(x)), (y) )
#define strtok_r(x, y, z) strtok((x),(y))
#define snprintf _snprintf
#define strdup _strdup
#define strncasecmp _strnicmp
#define strcasecmp  _stricmp
#define wread(x,y,z) recv((x),(y),(z),0)
#define wwrite(x,y,z) send((x),(y),(z),0)
#define close _close
#define sleep(x) Sleep( (x)*1000)

COMMONExpDLL
int gettimeofday( struct timeval *result, void *dummy );

COMMONExpDLL
int getpid();

const char *index( const char *str, char c );

#define ctime_r ctime

#define gmtime_r( _clock, _result ) \
        ( *(_result) = *gmtime( (_clock) ), \
                (_result) )

/*
 inttypes.h doesn't exist on windows.
 emulate some definitions here.
*/
#define PRId64 "I64d"
#define PRIu64 "I64u"
#ifndef INT8_MIN
#define INT8_MIN SCHAR_MIN
#endif
#ifndef INT8_MAX
#define INT8_MAX SCHAR_MAX
#endif
#ifndef INT16_MIN
#define INT16_MIN SHRT_MIN
#endif
#ifndef INT16_MAX
#define INT16_MAX SHRT_MAX
#endif
#ifndef INT32_MIN
#define INT32_MIN INT_MIN
#endif
#ifndef INT32_MAX
#define INT32_MAX INT_MAX
#endif
#ifndef INT64_MIN
#define INT64_MIN LLONG_MIN
#endif
#ifndef INT64_MAX
#define INT64_MAX LLONG_MAX
#endif
#ifndef UINT8_MAX
#define UINT8_MAX UCHAR_MAX
#endif
#ifndef UINT16_MAX
#define UINT16_MAX USHRT_MAX
#endif
#ifndef UINT32_MAX
#define UINT32_MAX UINT_MAX
#endif
#ifndef UINT64_MAX
#define UINT64_MAX ULLONG_MAX
#endif

#define wthread_detach( h )           /* noop */
#define wthread_self                 GetCurrentThread
#define wthread_equal( h1, h2 )      ((h1) == (h2))

#define wthread_cond_init( h, zip )         ((*(h)) = CreateEvent( NULL, 1, 0, NULL ))
#define wthread_cond_signal( h )            (SetEvent( *(h) ))
#define wthread_cond_destroy( h )           (CloseHandle( *(h) ))

COMMONExpDLL DWORD 
wthread_cond_wait( HANDLE *event, LPCRITICAL_SECTION *cs );

#define wthread_exit ExitThread

#define wthread_cleanup_push( x, y ) /* noop */
#define wthread_cleanup_pop(x) /* noop */

#define CPU_AFFINITY_SET DWORD

COMMONExpDLL int wthread_create( wthread_t *h, void *atts, void *(*startProc)( void * ), void *arg );             
COMMONExpDLL void wthread_destroy(wthread_t thread);
COMMONExpDLL int wthread_join (wthread_t t, void **value_ptr);
COMMONExpDLL void wthread_testcancel( wthread_t h );
COMMONExpDLL void wthread_cancel( wthread_t h );
COMMONExpDLL struct tm* localtime_r (const time_t* t, struct tm* result);
COMMONExpDLL int wthread_set_affinity_mask( wthread_t h, CPU_AFFINITY_SET* dwThreadAffinityMask);

typedef int   wthread_attr_t;
COMMONExpDLL    void wthread_attr_init (int* attr);
COMMONExpDLL    void wthread_attr_setdetachstate (int* attr, int);

typedef int   wthread_mutexattr_t;
COMMONExpDLL    void wthread_mutexattr_init (int* attr);
COMMONExpDLL    void wthread_mutexattr_settype (int* attr, int);
    
#define wGetCurrentThreadId     GetCurrentThreadId 

/* Macros for shared library access */
#define wdlopen(a,b)          LoadLibrary(a)
#define wdlclose              FreeLibrary
#define wdlerror              GetLastError                
#define wdlsym                GetProcAddress

#define    timersub(tvp, uvp, vvp)                        \
    do {                                                  \
        (vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;    \
        (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec; \
        if ((vvp)->tv_usec < 0) {                         \
            (vvp)->tv_sec--;                              \
            (vvp)->tv_usec += 1000000;                    \
        }                                                 \
    } while (0)

#define timeradd(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;       \
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)


/* time gm not available on Windows */
COMMONExpDLL
time_t wtimegm (struct tm *tm);

struct wtimespec
{
    time_t tv_sec;
    long   tv_nsec;
};

COMMONExpDLL int wnanosleep (struct wtimespec* ts, struct timnespec* remain);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* WINDOWS_H__ */
