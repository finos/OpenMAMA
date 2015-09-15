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

/* Linux Specific macros, #includes, and prototypes. 
 *
 * For Linux, most should be macros. Some of the contents of this file may be
 * moved to a common "unix.h' file when we port to Solaris and other unix
 * variants.
 */

#ifndef  PORT_LINUX_H__
#define  PORT_LINUX_H__

#include <pthread.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/vfs.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <semaphore.h>
#include <dirent.h>
#include <dlfcn.h>
#include <unistd.h>
#include <inttypes.h>
#include <pwd.h>

#include "wConfig.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/* PTHREAD static locks are easy */
typedef pthread_mutex_t wthread_static_mutex_t;
#define WSTATIC_MUTEX_INITIALIZER PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#define WTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#define wthread_static_mutex_lock(x) pthread_mutex_lock((x))
#define wthread_static_mutex_unlock(x) pthread_mutex_unlock((x))

/* Queue Max Size */
#ifdef SEM_VALUE_MAX
#define WOMBAT_QUEUE_MAX_SIZE SEM_VALUE_MAX /* 2_147_483_647 on Linux */
#else
#define WOMBAT_QUEUE_MAX_SIZE 2147483647 /* max possible size */
#endif

/* Type for handle to dynamically loaded library */
typedef void*       LIB_HANDLE;

/* suffix for shared libraries */
#define LIB_EXTENSION ".so"

/* Network conversion function */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htonll(x) \
    ((uint64_t)htonl((uint32_t)((x)>>32)) | (uint64_t)htonl((uint32_t)(x))<<32)
#define ntohll(x) \
    ((uint64_t)ntohl((uint32_t)((x)>>32)) | (uint64_t)ntohl((uint32_t)(x))<<32)
#else
#define htonll(x) ((uint64_t)(x))
#define ntohll(x) ((uint64_t)(x))
#endif

/* For delimiting multiple paths in env variables properties */
#define PATH_DELIM ':'

#define PATHSEP  "/"

/* Thread local storage */
typedef pthread_key_t wthread_key_t;
#define wthread_key_create(x, val) pthread_key_create((x), (val))
#define wthread_key_delete(x) pthread_key_delete((x))
#define wthread_setspecific(x, val) pthread_setspecific((x),(void*)((val)))
#define wthread_getspecific(x) pthread_getspecific((x))

/* Use Posix semaphores for Linux */
#define wsem_t          sem_t          
#define wsem_init       sem_init       
#define wsem_post       sem_post       
#define wsem_wait       sem_wait       
#define wsem_trywait    sem_trywait    
#define wsem_destroy    sem_destroy    
#define wsem_getvalue   sem_getvalue   

int wsem_timedwait (wsem_t* sem, unsigned int ts);

/* Windows does not support AF_UNIX sockets, socketpairs, etc */
#define wsocketstartup()
#define wsocketcleanup()
#define wclosesocket(x) close(x)

#define wsocketpair(dom, type, prot, pair) (socketpair((dom),(type),(prot),(pair)))
#define wsetnonblock(s) (fcntl((s), F_SETFL, fcntl((s), F_GETFL) | O_NONBLOCK))

#define wread	read    
#define wwrite	write   

#define CPU_AFFINITY_SET 				cpu_set_t

/* Use pthreads for linux */
#define INVALID_THREAD (-1)

#define wthread_mutex_t         pthread_mutex_t    
#define wthread_mutex_init      pthread_mutex_init
#define wthread_mutex_unlock    pthread_mutex_unlock
#define wthread_mutex_lock      pthread_mutex_lock
#define wthread_destroy
#define wthread_mutex_destroy   pthread_mutex_destroy
#define wthread_t               pthread_t
#define wthread_detach          pthread_detach
#define wthread_self            pthread_self
#define wthread_equal           pthread_equal
#define wthread_cleanup_push    pthread_cleanup_push
#define wthread_cleanup_pop     pthread_cleanup_pop
#define wthread_join		pthread_join
#define wthread_create		pthread_create
#define wthread_exit            pthread_exit

#define wthread_cond_t	        pthread_cond_t
#define wthread_cond_init       pthread_cond_init
#define wthread_cond_signal     pthread_cond_signal
#define wthread_cond_destroy    pthread_cond_destroy
#define wthread_cond_wait	pthread_cond_wait

#define wthread_spinlock_t     pthread_spinlock_t    
#define wthread_spin_init      pthread_spin_init
#define wthread_spin_unlock    pthread_spin_unlock
#define wthread_spin_lock      pthread_spin_lock
#define wthread_spin_destroy   pthread_spin_destroy
#define wthread_attr_t   pthread_attr_t
#define wthread_attr_init pthread_attr_init
#define wthread_attr_setdetachstate pthread_attr_setdetachstate

#define wthread_mutexattr_t pthread_mutexattr_t
#define wthread_mutexattr_init pthread_mutexattr_init
#define wthread_mutexattr_settype pthread_mutexattr_settype

#define wGetCurrentThreadId     pthread_self 

/* macros for shared libraries */
#define wdlopen dlopen
#define wdlsym dlsym
#define wdlclose dlclose
#define wdlerror dlerror

/* timegm() and nanosleep not available on Windows */
#define wtimegm timegm

struct wtimespec
{
    time_t tv_sec;
    long   tv_nsec;
};

#define wnanosleep(ts, remain) nanosleep(((struct timespec*)(ts)),(remain))

/* Macro for managing the printing of mama_size_t values. */
#define PRI_MAMA_SIZE_T "zu"

/* net work utility functions */
const char* getIpAddress (void);
const char* getHostName (void);

/* GCC and compatible compilers (except ICC) */
#if (defined (__GNUC__) && (!defined (__ICC)))
#  define MAMADeprecated__(MSG)         __attribute__ ((deprecated))
/* Clang versions have their own feature check: */
#  if (defined(__clang__))
#    if (__has_extension(attribute_deprecated_with_message))
#      undef  MAMADeprecated__
#      define MAMADeprecated__(MSG)     __attribute__ ((deprecated(#MSG)))
#    endif
/* GCC 4.5 and greater also supports message with deprecated */
#  elif (__GNUC__ >= 4 && __GNUC_MINOR__ >= 5)
#    undef  MAMADeprecated__
#    define MAMADeprecated__(MSG)       __attribute__ ((deprecated(#MSG)))
#  endif
/* ICC doesn't currently support the deprecated attribute. */
#else
#  define MAMADepreacted__(MSG)
#endif

/* Definitions for each Export type (for Windows compatibility) */
#define COMMONDeprecated(MSG)            MAMADeprecated__(MSG)
#define COMMONExpDeprecatedDLL(MSG)      MAMADeprecated__(MSG)
#define MAMACPPExpDeprecatedDLL(MSG)     MAMADeprecated__(MSG)
#define MAMADeprecated(MSG)              MAMADeprecated__(MSG)
#define MAMAExpDeprecatedBridgeDLL(MSG)  MAMADeprecated__(MSG)
#define MAMAExpDeprecatedDLL(MSG)        MAMADeprecated__(MSG)
#define MAMDAExpDeprecatedDLL(MSG)       MAMADeprecated__(MSG)
#define MAMDAOPTExpDeprecatedDLL(MSG)    MAMADeprecated__(MSG)
#define WCACHEExpDeprecatedDLL(MSG)      MAMADeprecated__(MSG)
#define WMWDeprecated(MSG)               MAMADeprecated__(MSG)
#define WMWExpDeprecatedDLL(MSG)         MAMADeprecated__(MSG)

#define MAMATypeDeprecated(NAME, MSG)    MAMADeprecated__(MSG)

/* Special tags to allow for disabling deprecation messages for code between
 * the MAMAIgnoreDeprecatedOpen and MAMAIgnoreDeprecatedClose tags.
 */
/* Clang Compilers */
#if defined (__clang__)
#  define MAMAIgnoreDeprecatedOpen           \
        _Pragma ("clang diagnostic push")    \
        _Pragma ("clang diagnostic ignored \"-Wdeprecated-declarations\"")

#  define MAMAIgnoreDeprecatedClose         \
        _Pragma ("clang diagnostic pop")

/* Intel Compiler Collection */
#elif defined (__ICC)
#  define MAMAIgnoreDeprecatedOpen
#  define MAMAIgnoreDeprecatedClose

/* Must be GCC */
#elif defined (__GNUC__)
#  if (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 6)
#    define MAMAIgnoreDeprecatedOpen           \
        _Pragma ("GCC diagnostic push")      \
        _Pragma ("GCC diagnostic ignored \"-Wdeprecated\"")

#    define MAMAIgnoreDeprecatedClose         \
        _Pragma ("GCC diagnostic pop")
#  else
/* For older versions of GCC, we can't suitably mask the warnings on use of types
 * therefore we remove the deprecation flag
 */
#    undef  MAMATypeDeprecated
#    define MAMATypeDeprecated(NAME, MSG)
#    define MAMAIgnoreDeprecatedOpen
#    define MAMAIgnoreDeprecatedClose
#  endif
#endif

#if defined (__cplusplus)
} /* extern "c" */
#endif

#endif /* LINUX_H__ */
