/* $Id$
 *
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

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <mamda/MamdaLock.h>
#include <wombat/wincompat.h>
#include <mama/mamacpp.h>

 
using std::string;
using std::ostringstream;
using std::runtime_error;

namespace Wombat
{

    class MamdaLock::MamdaLockImpl
    {
        public:
        MamdaLockImpl          (MamdaLock::Scheme    scheme, 
                                const char*          context,
                                int                  threads);
        ~MamdaLockImpl         ();

        bool acquire           (MamdaLock::Type      type);
        bool release           (MamdaLock::Type      type);


        private:
        MamdaLockImpl         (const MamdaLockImpl&  impl); // No copying!
        void handleLastError  ();

        MamdaLock::Scheme     mScheme;
        CRITICAL_SECTION      mCritSect;    
        HANDLE                mHandle;

        int                   mCount;
        int                   mThreads;

        char                  mName    [129];
        char                  mContext [129];
    };

    MamdaLock::MamdaLock(
        Scheme       scheme, 
        const char*  context, 
        int          threads)
        : mImpl (*new MamdaLockImpl (MamdaLock::EXCLUSIVE, context, threads)) 
    {
        if (scheme == MamdaLock::SHARED)
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,
		              "%s: Only EXCLUSIVE locking scheme available on Win32.\n"
		              "\t - READ and WRITE locks treated similarly.\n",
		              context);
        }
    }

    MamdaLock::MamdaLockImpl::MamdaLockImpl (
        MamdaLock::Scheme  scheme, 
        const char*        context, 
        int                threads)
        : mScheme  (scheme)
        , mCount   (0)
        , mThreads (threads)
    {
        int status = 0;

        snprintf(mName,    128, "%s",   context);
        snprintf(mContext, 128, "[%s]", context);

        mHandle = CreateEvent (NULL, 1, 0, NULL);

        if (!mHandle)
        {    
            handleLastError();
        }

        try
        {
            InitializeCriticalSection (&mCritSect);
        }
        catch (...)
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,    
                      "%s: FAILED to initialise lock using scheme %s PID(%d)\n",
                      mContext, 
                      MamdaLock::scheme (mScheme), 
                      GetCurrentThread());
        }
    } 

    MamdaLock::~MamdaLock()
    {
        delete &mImpl;
    }

    MamdaLock::MamdaLockImpl::~MamdaLockImpl()
    {
        DeleteCriticalSection (&mCritSect);

        bool handleClosed = CloseHandle (mHandle);

        if (!handleClosed == 0)
        {
            handleLastError();
        }
    }

    bool MamdaLock::acquire (Type type)
    {
        return mImpl.acquire (type);
    }

    bool MamdaLock::MamdaLockImpl::acquire (MamdaLock::Type type)
    {
        if (mThreads == -1) 
        {
            mCount++;
        	if(mCount % 10000 == 0)
        	{
        	    mama_log (MAMA_LOG_LEVEL_FINEST,
        		          "%s: Acquired %d locks so far\n",
                          mContext, 
                          mCount);
        	}
            return true;
        }

        try
        {
            // EnterCriticalSection synchronizes code with other threads.
            EnterCriticalSection (&mCritSect);
        }
        catch (...)
        {
            // If exception thrown, unlock critical section 
            mama_log (MAMA_LOG_LEVEL_FINEST,    
		              "%s: FAILED to acquired %s lock using scheme %s PID(%d)\n",
                      mContext, 
                      MamdaLock::type (type), 
                      MamdaLock::scheme (mScheme), 
                      (void*)GetCurrentThread());
        
            LeaveCriticalSection (&mCritSect);    
               
	    mama_log (MAMA_LOG_LEVEL_FINEST,
		          "%s: Exception Deadlock Encountered\n", 
                  mContext);
        }

        return true;
    }

    void MamdaLock::MamdaLockImpl::handleLastError()
    {
        TCHAR  szBuf [80]; 
        LPVOID lpMsgBuf;
        DWORD  dw = GetLastError(); 

        FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                       FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL,
                       dw,
                       MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPTSTR) &lpMsgBuf,
                       0, 
                       NULL );

        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "%s: MamdaLock: %s\n",
                  mContext, 
                  lpMsgBuf);

        LocalFree (lpMsgBuf);
    }

    bool MamdaLock::release(MamdaLock::Type type)
    {
        return mImpl.release(type);

    }

    bool MamdaLock::MamdaLockImpl::release(MamdaLock::Type type)
    {
        if(mThreads != -1)
        { 
            mama_log (MAMA_LOG_LEVEL_FINEST,
		              "%s: Releasing  %s lock using scheme %s PID(%d)\n",
                      mContext, 
                      MamdaLock::type (type), 
                      MamdaLock::scheme (mScheme), 
                      GetCurrentThread());
	
            LeaveCriticalSection (&mCritSect);
        }
        return true;
    }
     
    const char* MamdaLock::scheme (Scheme scheme)
    {
        switch(scheme)
        {
            case (SHARED):     return "SHARED";
            case (EXCLUSIVE):  return "EXCLUSIVE";
            default:           return "UNKNOWN SCHEME";
        }
    }

    const char* MamdaLock::type (Type type)
    {
        switch(type)
        {
            case (READ):   return "READ";
            case (WRITE):  return "WRITE";
            default:       return "UNKNOWN TYPE";
        }
    }

} // namespace Wombat
