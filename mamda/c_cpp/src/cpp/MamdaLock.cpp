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

#include <pthread.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <mama/mamacpp.h>
#include <mamda/MamdaLock.h>
 
using std::string;
using std::ostringstream;
using std::runtime_error;

namespace Wombat
{

    class MamdaLock::MamdaLockImpl
    {
    public:
        MamdaLockImpl           (MamdaLock::Scheme     scheme, 
                                 const char*           context,
                                 int                   threads);
        ~MamdaLockImpl          ();

        bool acquire            (MamdaLock::Type       type);
        bool release            (MamdaLock::Type       type);

    private:
        MamdaLockImpl           (const MamdaLockImpl&  impl);     // No copying!

        void handlePthreadError (int                   status,   
                                 const string&         context);

        bool lockMutex          (bool                  throwEx, 
                                 const char*           context);

        bool unlockMutex        (bool                  throwEx, 
                                 const char*           context);

        MamdaLock::Scheme       mScheme;

        pthread_mutex_t         mMutex;    
        pthread_cond_t          mCond;
        pthread_mutexattr_t     mMutexAttr;

        // For rwlocking
        pthread_rwlock_t        mRwLock;
        pthread_rwlockattr_t    mRwLockAttr;

        int                     mCount;
        int                     mThreads;

        char                    mName    [129];
        char                    mContext [129];
    };

    MamdaLock::MamdaLock(
        Scheme       scheme, 
        const char*  context, 
        int          threads)
        : mImpl (*new MamdaLockImpl(scheme, context, threads)) 
    {
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

        snprintf (mName,    128, "%s",   context);
        snprintf (mContext, 128, "[%s]", context);

        if (MamdaLock::SHARED == scheme)
        {
            status = pthread_rwlockattr_init (&mRwLockAttr);
            if (0 != status)
            {
                handlePthreadError (status,
			                        "MamdaLock::MamdaLock(): Failed to init rwlock attr");
            }
            status = pthread_rwlock_init (&mRwLock, &mRwLockAttr);
            if (0 != status)
            {
                handlePthreadError (status,
			                        "MamdaLock::MamdaLock(): Failed to init rwlock attr");
            }
            return;
        }
        status = pthread_mutexattr_init (&mMutexAttr);
        if (0 != status)
        {
            handlePthreadError (status,
		                        "MamdaLock::MamdaLock(): Failed to init mutex attr");
        }
        if (MamdaLock::EXCLUSIVE == scheme)
        {

            status = pthread_mutexattr_settype(&mMutexAttr, 
						    PTHREAD_MUTEX_RECURSIVE_NP);

            if (0 != status)
            {
                handlePthreadError (status,
                                    "MamdaLock::MamdaLock(): Failed to set mutex "
                                    "attr: PTHREAD_MUTEX_RECURSIVE_NP");
            }
        }

        status =  pthread_mutex_init (&mMutex, &mMutexAttr);
        if (0 != status)
        {
            handlePthreadError (status,
                                "MamdaLock::MamdaLock(): "
			                    "Failed to init mutex");
        }

        status = pthread_cond_init (&mCond, NULL);
        if (0 != status)
        {
            handlePthreadError (status,
                                "MamdaLock::Lock(): Failed to init cond");
        }
    } 

    MamdaLock::~MamdaLock()
    {
        delete &mImpl;
    }

    MamdaLock::MamdaLockImpl::~MamdaLockImpl()
    {
        if (mScheme == MamdaLock::SHARED)
        {
            int status = pthread_rwlock_destroy (&mRwLock);
            if (0 == status)
            {
                handlePthreadError (status,
                                    "MamdaLock::~MamdaLock(): Failed to destroy rwlock");
            }
            status = pthread_rwlockattr_destroy (&mRwLockAttr);
            if (0 == status)
            {
                handlePthreadError (status,
                                    "MamdaLock::~MamdaLock(): Failed to destroy rwlockattr");
            }

            return;
        }
        int status = pthread_mutex_destroy (&mMutex);
        if (0 == status)
        {
            handlePthreadError (status,
                                "MamdaLock::~MamdaLock(): Failed to destroy mutex");
        }
        status = pthread_cond_destroy (&mCond);
        if (0 != status)
        {
            handlePthreadError (status,
                                "MamdaLock::MamdaLock(): Failed to destroy cond");
        }

        status = pthread_mutexattr_destroy (&mMutexAttr);
        if (0 != status)
        {
            handlePthreadError (status,
                                "MamdaLock::MamdaLock(): Failed to destroy mutex attr");
        }
    }

    bool MamdaLock::acquire (Type type)
    {
        return mImpl.acquire (type);
    }

    bool MamdaLock::MamdaLockImpl::acquire (MamdaLock::Type type)
    {
        bool isLogFinest = (MAMA_LOG_LEVEL_FINEST == mama_getLogLevel());

        if (mThreads == -1) 
        {
            mCount++;
            if (mCount % 10000 == 0)
            {
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "%s: Acquired %d locks so far\n",
                          mContext, 
                          mCount);
            }
            return true;
        }

        // For the EXCLUSIVE locking scheme, it just uses
        // simple mutexes
        if (mScheme == MamdaLock::SHARED) 
        {
            if (isLogFinest)
            {
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "%s: Acquiring  %s lock using scheme %s PID(%d)\n",
                          mContext, 
                          MamdaLock::type (type), 
                          MamdaLock::scheme (mScheme), 
                          (void*)pthread_self());
            }

            if (type == MamdaLock::READ)
            {
                int status = pthread_rwlock_rdlock (&mRwLock);

                if (isLogFinest)
                {
                    mama_log (MAMA_LOG_LEVEL_FINEST,
                              "%s: Acquired  %s lock using scheme %s PID(%d)\n",
                              mContext, 
                              MamdaLock::type (type), 
                              MamdaLock::scheme (mScheme), 
                              (void*)pthread_self());
                }

                if (status)
                {
                    handlePthreadError (false, "MamdaLock::acquire() rdlock");
                    return false;
                }
                return true;
            }
            else
            { 
                int status = pthread_rwlock_wrlock (&mRwLock);

                if (isLogFinest)
                {
                    mama_log (MAMA_LOG_LEVEL_FINEST,
                              "%s: Acquired  %s lock using scheme %s PID(%d)\n",
                              mContext, 
                              MamdaLock::type (type), 
                              MamdaLock::scheme (mScheme), 
                              (void*)pthread_self());
                }
                if (status)
                {
                    handlePthreadError (false, "MamdaLock::acquire() wrlock");
                    return false;
                }
                return true;
            }
        }
        else
        { 
            if (!lockMutex (false, "MamdaLock::acquire(): (1)"))
                return false;

            if (isLogFinest)
            {
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "%s: Acquired  %s lock using scheme %s PID(%d)\n",
                          mContext, 
                          MamdaLock::type (type), 
                          MamdaLock::scheme (mScheme), 
                          (void*)pthread_self());
            }

            return true;
        }
        return true;
    }

    bool MamdaLock::release (MamdaLock::Type type)
    {
        return mImpl.release (type);
    }

    bool MamdaLock::MamdaLockImpl::release (MamdaLock::Type type)
    {
        if (mThreads == -1) 
            return true;

        if (MAMA_LOG_LEVEL_FINEST == mama_getLogLevel())
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "%s: Releasing  %s lock using scheme %s PID(%d)\n",
                      mContext, 
                      MamdaLock::type (type), 
                      MamdaLock::scheme (mScheme), 
                      (void*)pthread_self());
        }

        if (mScheme == MamdaLock::SHARED)
        {
            int status = pthread_rwlock_unlock (&mRwLock);

            if (status)
            {
                handlePthreadError (false, "MamdaLock::acquire() rdlock");
                return false;
            }
            return true;
        }
        else
        {
            //Release any waiting threads
            if (!unlockMutex (true, "MamdaLock::release(): (1)"))
            {
                return false;
            }
            return true;
        }
    }

    void MamdaLock::MamdaLockImpl::handlePthreadError (
        int            status, 
        const string&  context)
    {
        if (0 != status)
        {
            string errorStr;
            switch (status)
            {
                case (EINVAL):
                    errorStr =  "Mutex not properly initialized!";
                    break;
                case (EDEADLK):
                    errorStr = "Already locked by calling thread!";
                    break;
                case (EPERM):
                    errorStr = "Calling thread does not own the mutex!";
                    break;
                case (EBUSY):
                    errorStr = "thread is currently locked!";
                    break;
                default:
                    errorStr = "unrecognized failure type";
                    break;
            }
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "%s: %s (%d)!\n", 
                      mContext, 
                      errorStr.c_str(),
                      status);
        }
    }

    bool MamdaLock::MamdaLockImpl::lockMutex (
        bool         throwEx,
        const char*  context)
    {
        int status = pthread_mutex_lock (&mMutex);

        if (0 != status)
        {
            string errMsg = context;
            errMsg += " lock failed";

            handlePthreadError (status, errMsg);

            if (throwEx)
                throw runtime_error (errMsg.c_str());
            return false;
        }
        return true;
    }

    bool MamdaLock::MamdaLockImpl::unlockMutex (
        bool         throwEx,
        const char*  context)
    {
        int status = pthread_mutex_unlock (&mMutex);
        if (0 != status)
        {
            string errMsg = context;
            errMsg += " unlock failed";

            handlePthreadError (status, errMsg);

            if (throwEx)
                throw runtime_error (errMsg.c_str());
            return false;
        }
        return true;
    }

    const char* MamdaLock::scheme (Scheme scheme)
    {
        switch(scheme)
        {
            case(SHARED):     return "SHARED";
            case(EXCLUSIVE):  return "EXCLUSIVE";
            default:          return "UNKNOWN SCHEME";
        }
    }

    const char* MamdaLock::type (Type type)
    {
        switch(type)
        {
            case(READ):    return "READ";
            case(WRITE):   return "WRITE";
            default:       return "UNKNOWN TYPE";
        }
    }

} // namespace Wombat
