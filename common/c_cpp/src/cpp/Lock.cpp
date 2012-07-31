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

#include <wlock.h>
#include "wombat/Lock.h"

namespace Wombat {

struct Lock::LockImpl
{
    LockImpl ()
        : mLock     (wlock_create ())
        , mIsLocked (false)
    {}

    ~LockImpl ()
    {
        if (mIsLocked)
        {
            wlock_unlock (mLock);
        }

        wlock_destroy (mLock);
    }

    wLock mLock;
    bool  mIsLocked;
};

Lock::Lock ()
    : mImpl (new LockImpl)
{}

Lock::~Lock ()
{}

void Lock::lock ()
{
    wlock_lock (mImpl->mLock);
    mImpl->mIsLocked = true;
}

void Lock::unlock ()
{
    wlock_unlock (mImpl->mLock);
    mImpl->mIsLocked = false;
}

}
