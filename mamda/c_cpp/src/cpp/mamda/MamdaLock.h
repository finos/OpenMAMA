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

#ifndef MamdaLockH
#define MamdaLockH

#include <mamda/MamdaConfig.h>

namespace Wombat
{
    #define ACQUIRE_WLOCK(lock) (lock).acquire(MamdaLock::WRITE)
    #define ACQUIRE_RLOCK(lock) (lock).acquire(MamdaLock::READ)
    #define RELEASE_WLOCK(lock) (lock).release(MamdaLock::WRITE)
    #define RELEASE_RLOCK(lock) (lock).release(MamdaLock::READ)

    #define ACQUIRE_LOCK(lock, writeNeeded)     \
    do {                                        \
        if (writeNeeded)                        \
            (lock).acquire(MamdaLock::WRITE);   \
        else                                    \
            (lock).acquire(MamdaLock::READ);    \
    } while (0) 

    #define RELEASE_LOCK(lock, isWrite)         \
    do {                                        \
        if (isWrite)                            \
            (lock).release(MamdaLock::WRITE);   \
        else                                    \
            (lock).release(MamdaLock::READ);    \
    } while (0) 


    class MAMDAExpDLL MamdaLock 
    {
    public:
        enum Scheme
        {
            SHARED,
            EXCLUSIVE
        }; 

        enum Type
        {
            READ,
            WRITE
        };

        MamdaLock (Scheme       scheme, 
                   const char*  context,
                   int          threads = 0);
 
        ~MamdaLock ();

        bool                acquire  (Type type);
        bool                release  (Type type);    
        static const char*  type     (Type type);
        static const char*  scheme   (Scheme scheme);

        static bool lockingEnabled()
        {
            // This is to be a configuration parameter at some point
            return true;
        }

    private:
        MamdaLock (const MamdaLock& lock);
        class MamdaLockImpl;
        MamdaLockImpl& mImpl;
    };

    #endif // MamdaLockH


} // namsespace Wombat 
