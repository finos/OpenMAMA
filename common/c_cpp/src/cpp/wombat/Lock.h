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

#ifndef LockCppH__
#define LockCppH__

#include <memory>
#include "wombat/wConfig.h"

namespace Wombat {

class COMMONExpDLL Lock
{
public:
    Lock       ();
    ~Lock      ();
    void lock   ();
    void unlock ();
private:
    Lock (const Lock& copy);    // no copy
    Lock& operator= (const Lock& rhs); // no assignment
    struct LockImpl;
    struct LockImpl* mImpl;
};

// NoLock class is used a template parameter when unlocked access is required
class COMMONExpDLL NoLock
{
public:
    NoLock ()
    {}

    ~NoLock ()
    {}

    void lock ()
    {}

    void unlock ()
    {}

private:
    NoLock (const NoLock& copy);    // no copy
    NoLock& operator= (const NoLock& rhs); // no assignment
};

template < class LockT >
class COMMONExpDLL AutoLock
{
public:

    AutoLock (LockT & l)
        : mLock (l)
    {
        mLock.lock ();
    }

    ~AutoLock ()
    {
        mLock.unlock ();
    }

private:
    AutoLock (const AutoLock& copy);    // no copy
    AutoLock& operator= (const AutoLock& rhs); // no assignment

    LockT   &mLock;
};

}
#endif
