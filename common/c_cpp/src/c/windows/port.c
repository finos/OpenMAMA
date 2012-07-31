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

#include "wombat/port.h"

int gettimeofday( struct timeval *result, void *dummy )
{
    time_t rawTime;
    SYSTEMTIME t;
    GetSystemTime( &t );
    time(&rawTime);
    result->tv_sec = (long)rawTime;
    result->tv_usec = (t.wMilliseconds*1000);

    return 0;
}


typedef struct
{
    HANDLE mThread;
    int   mIsStopping; /* Reads and writes to 32 bit vars are atomic in WIN32 */
} threadContext;



int wthread_set_affinity_mask( wthread_t h, 
                               CPU_AFFINITY_SET* dwThreadAffinityMask)
{
    return (int) SetThreadAffinityMask( ((threadContext*) h)->mThread, 
                                        (DWORD_PTR) dwThreadAffinityMask );
}


int wthread_create( wthread_t *h, void *atts, void *(*startProc)( void * ), void *arg )
{
    threadContext *result = (threadContext *)calloc( 1, sizeof( threadContext ) );

    if( result == NULL )
    {
        return 1;
    }

    *h = result;

    result->mIsStopping = 0;

    result->mThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)startProc, arg, 0, NULL );
    if (result->mThread == 0)
    {
        free( result );
        return 1;
    }

    return 0;
}

void wthread_destroy(wthread_t thread)
{
    if(NULL != thread)
    {
        /* Get the impl. */
        threadContext *impl = (threadContext *)thread;

        /* Close the thread handle. */
        CloseHandle(impl->mThread);

        /* Clean the structure. */
        free(impl);
    }
}

int wthread_join (wthread_t t, void **value_ptr)
{
     threadContext *ctx = (threadContext*)t;
     WaitForSingleObject (ctx->mThread, INFINITE);
     if (value_ptr!=NULL)
        *value_ptr = NULL;
    return 0;
}

void wthread_testcancel( wthread_t h )
{
     threadContext *ctx = (threadContext*)h;
     if( ctx->mIsStopping )
     {
        ExitThread(0);
     }
}

void wthread_cancel( wthread_t h )
{
     threadContext *ctx = (threadContext*)h;
     ctx->mIsStopping = 1;
     WaitForSingleObject( ctx->mThread, INFINITE );
     free( ctx );
}

void wthread_attr_init (int* attr)
{
}

void wthread_attr_setdetachstate (int* attr, int param)
{
}

void wthread_mutexattr_init (int* attr)
{
}

void wthread_mutexattr_settype (int* attr, int param)
{
}

DWORD wthread_cond_wait( HANDLE *event, LPCRITICAL_SECTION *cs )
{
    DWORD rval;

    LeaveCriticalSection( cs );
    rval = WaitForSingleObject( *event, INFINITE );
    ResetEvent( *event );
    EnterCriticalSection( cs );

    return rval;
}

const char* index( const char *str, char c )
{
    unsigned int i = 0;
    while( i < strlen( str ) )
    {
        if( str[i] == c )
        { 
            return str + i;
        }
        i++;
    }
    return NULL;
}

int getpid()
{
    return GetCurrentProcessId();
}

struct tm* localtime_r (const time_t* t, struct tm* result)
{
    localtime_s (result, t);
    return result;
}

char user[256];

const char *getlogin()
{
   /* NOTE: This will fail if user name is over 256 characters */
   unsigned long len = 256;
   GetUserName( user, &len );
   return user;
}

int
wsocketstartup ()
{
    WSADATA wsaData;
    int err =WSAStartup(MAKEWORD(2,2), &wsaData);
    if (err)
    {
        return -1;
    }
    return 0;
}

void
wsocketcleanup ()
{
    WSACleanup( );
}

int
wsocketpair (int domain, int type, int protocol, int* pair)
{
    struct sockaddr_in addr;
    int len = sizeof(addr);
    int l; 

    l = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == l) 
        return -1;

    ZeroMemory (&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = 0; /* any available */
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (0 != bind (l, (struct sockaddr*)(&addr), sizeof(addr)))
    {
        _close (l);
        return -1;
    }

    /* Get the port and address for the other end */
    if( 0 != getsockname (l, (struct sockaddr*)(&addr), &len))
    {
        _close (l);
        return -1;
    }

    if (0 != listen (l, 1))
    {
        _close (l);
        return -1;
    }
    
    pair[0] = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == pair[0]) 
    {
        _close (l);
        return -1;
    }

    if (0 != connect (pair[0], (struct sockaddr*)(&addr), sizeof(addr)))
    {
        _close (l);
        _close (pair[0]);
        return -1;
    }

    pair[1] = accept (l, NULL, NULL);
    if (INVALID_SOCKET == pair[1])
    {
        _close (l);
        _close (pair[0]);
        return -1;
    }

    return 0;
}

int
wthread_key_create(wthread_key_t* key, void* val)
{
    *key = TlsAlloc();
    if (TLS_OUT_OF_INDEXES == *key) 
        return 1;
    TlsSetValue(*key, val);
    return 0;
}

time_t wtimegm (struct tm *tm) 
{
    time_t ret;
    char *tz;
    
    tz = environment_getVariable("TZ");
    environment_setVariable("TZ", "");
    tzset();
    ret = mktime(tm);
    if (tz)
        environment_setVariable("TZ", tz);
    else
        environment_deleteVariable("TZ");
    tzset();
    return ret;
}

int wnanosleep (struct wtimespec* ts, struct timnespec* remain)
{
    DWORD millis = ts->tv_sec * 1000 + ts->tv_nsec/1000;
    /* if tv_nsec > 0 add at least 1 milli */
    if (ts->tv_nsec > 0 && ts->tv_nsec < 1000) 
        millis += 1;
    Sleep(millis);
    return 0;
}
