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

#include <mama/MamaSourceGroupManager.h>
#include <mama/MamaSourceGroup.h>
#include "mamacppinternal.h"
#include <map>
#include <string>

// Note: The C++ implementation is completely independent of the C
// implementation -- primarily because it is so simple and because the
// find methods return C++ objects and we wouldn't want to have to
// deal with wrapping the C ones.


namespace Wombat
{

    typedef  map<string,MamaSourceGroup*>  SourceGroupMap;

    struct MamaSourceGroupManager::MamaSourceGroupManagerImpl
    {
        SourceGroupMap  myMap;
    };


    MamaSourceGroupManager::MamaSourceGroupManager ()
        : myImpl (*new MamaSourceGroupManagerImpl)
    {
    }

    MamaSourceGroupManager::~MamaSourceGroupManager ()
    {
        // Delete all the maps
        delete &myImpl;
    }

    MamaSourceGroup*  MamaSourceGroupManager::create (const char*  groupName)
    {
        SourceGroupMap::const_iterator found = myImpl.myMap.find (groupName);

        if (found != myImpl.myMap.end ())
        {
            return found->second;
        }

        MamaSourceGroup*  group = new MamaSourceGroup (groupName);
        myImpl.myMap.insert (SourceGroupMap::value_type (groupName, group));

        return group;
    }

    mama_size_t MamaSourceGroupManager::size () const
    {
        return myImpl.myMap.size ();
    }

    MamaSourceGroup*  MamaSourceGroupManager::findOrCreate (const char*  groupName)
    {
        // This is the same as create ()
        return create (groupName);
    }

    MamaSourceGroup*  MamaSourceGroupManager::find (const char*  groupName)
    {
        SourceGroupMap::const_iterator found = myImpl.myMap.find (groupName);

        if (found != myImpl.myMap.end ())
        {
            return found->second;
        }
        else
        {
            return NULL;
        }
    }

    const MamaSourceGroup* MamaSourceGroupManager::find (const char*  groupName) const
    {
        SourceGroupMap::const_iterator found = myImpl.myMap.find (groupName);

        if (found != myImpl.myMap.end ())
        {
            return found->second;
        }
        else
        {
            return NULL;
        }
    }


    struct MamaSourceGroupManager::iterator::iteratorImpl
    {
        iteratorImpl () 
        {
        }

        iteratorImpl (const iteratorImpl& copy)
            : myIterator (copy.myIterator)
        {
        }

        iteratorImpl (SourceGroupMap::iterator  iter)
            : myIterator (iter)
        {
        }

        ~iteratorImpl () 
        {
        }

        SourceGroupMap::iterator myIterator;
    };

    MamaSourceGroupManager::iterator::iterator ()
        : mImpl (*new iteratorImpl)
    {
    }

    MamaSourceGroupManager::iterator::iterator (const iterator& copy)
        : mImpl (*new iteratorImpl (copy.mImpl.myIterator))
    {
    }

    MamaSourceGroupManager::iterator::iterator (const iteratorImpl& copy)
        : mImpl (*new iteratorImpl (copy.myIterator))
    {}

    MamaSourceGroupManager::iterator::~iterator ()
    {
        delete &mImpl;
    }

    MamaSourceGroupManager::iterator MamaSourceGroupManager::begin ()
    {
        return iterator (iterator::iteratorImpl (myImpl.myMap.begin ()));
    }

    MamaSourceGroupManager::iterator MamaSourceGroupManager::end ()
    {
        return iterator (iterator::iteratorImpl(myImpl.myMap.end ()));
    }

    MamaSourceGroupManager::const_iterator MamaSourceGroupManager::begin () const
    {
        return const_iterator (iterator::iteratorImpl (myImpl.myMap.begin ()));
    }

    MamaSourceGroupManager::const_iterator MamaSourceGroupManager::end () const
    {
        return const_iterator (iterator::iteratorImpl (myImpl.myMap.end ()));
    }

    MamaSourceGroupManager::iterator&
    MamaSourceGroupManager::iterator::operator= (const iterator& rhs)
    {
        if (&rhs != this)
        {
            mImpl.myIterator = rhs.mImpl.myIterator;
        }

        return *this;
    }

    const MamaSourceGroupManager::iterator&
    MamaSourceGroupManager::iterator::operator= (const iterator& rhs) const
    {
        if (&rhs != this)
        {
            mImpl.myIterator = rhs.mImpl.myIterator;
        }

        return *this;
    }

    const MamaSourceGroupManager::iterator&
    MamaSourceGroupManager::iterator::operator++ () const
    {
        ++mImpl.myIterator;

        return *this;
    }

    MamaSourceGroupManager::iterator&
    MamaSourceGroupManager::iterator::operator++ ()
    {
        ++mImpl.myIterator;

        return *this;
    }

    bool MamaSourceGroupManager::iterator::operator== (const iterator& rhs) const
    {
        return ((&rhs == this) || (mImpl.myIterator == rhs.mImpl.myIterator));
    }

    bool MamaSourceGroupManager::iterator::operator!= (const iterator& rhs) const
    {
        return !operator==(rhs);
    }

    MamaSourceGroup* MamaSourceGroupManager::iterator::operator*()
    {
        return mImpl.myIterator->second;
    }

    const MamaSourceGroup* MamaSourceGroupManager::iterator::operator*() const
    {
        return mImpl.myIterator->second;
    }

} /* namespace  Wombat */
