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

#include <mama/MamaSourceManager.h>
#include <mama/MamaSource.h>
#include "mamacppinternal.h"
#include <map>
#include <string>

// Note: The C++ implementation is completely independent of the C
// implementation -- primarily because it is so simple and because the
// find methods return C++ objects and we wouldn't want to have to
// deal with wrapping the C ones.


namespace Wombat
{

    typedef  map<string,MamaSource*>  SourceMap;

    struct MamaSourceManager::MamaSourceManagerImpl
    {
        SourceMap  myMap;
    };


    MamaSourceManager::MamaSourceManager ()
        : myImpl (*new MamaSourceManagerImpl)
    {
        mamaSourceManager_create (&myManager);
    }

    MamaSourceManager::~MamaSourceManager ()
    {
        // Delete all the maps
        delete &myImpl;
        mamaSourceManager_destroy (myManager);
    }

    MamaSource* MamaSourceManager::create (const char*  sourceName)
    {
        SourceMap::const_iterator found = myImpl.myMap.find (sourceName);

        if (found != myImpl.myMap.end ())
        {
            // throw exception instead???
            return found->second;
        }

        MamaSource*  source = new MamaSource;
        source->setId (sourceName);
        myImpl.myMap.insert (SourceMap::value_type (sourceName, source));

        return source;
    }

    MamaSource* MamaSourceManager::findOrCreate (const char* sourceName)
    {
        // This is the same as create ()
        return create (sourceName);
    }

    MamaSource* MamaSourceManager::find (const char*  sourceName)
    {
        SourceMap::const_iterator found = myImpl.myMap.find (sourceName);

        if (found != myImpl.myMap.end ())
        {
            return found->second;
        }
        else
        {
            return NULL;
        }
    }

    const MamaSource* MamaSourceManager::find (const char*  sourceName) const
    {
        SourceMap::const_iterator found = myImpl.myMap.find (sourceName);

        if (found != myImpl.myMap.end ())
        {
            return found->second;
        }
        else
        {
            return NULL;
        }
    }

    void  MamaSourceManager::add (MamaSource*  source)
    {
        const char* sourceName = source->getId ();
        add (sourceName, source);
    }

    void  MamaSourceManager::add (const char*  sourceName,
                                  MamaSource*  source)
    {
        MamaSource* found = find (sourceName);

        if (found != NULL)
        {
            return;
        }

        source->setMappedId (sourceName); 
        myImpl.myMap.insert (SourceMap::value_type (sourceName, source));
    }


    struct MamaSourceManager::iterator::iteratorImpl
    {
        iteratorImpl () 
        {
        }

        iteratorImpl (const iteratorImpl& copy)
            : myIterator (copy.myIterator)
        {
        }

        iteratorImpl (SourceMap::iterator  iter)
            : myIterator (iter)
        {
        }

        ~iteratorImpl () 
        {
        }

        SourceMap::iterator myIterator;
    };

    MamaSourceManager::iterator::iterator ()
        : mImpl (*new iteratorImpl)
    {
    }

    MamaSourceManager::iterator::iterator (const iterator& copy)
        : mImpl (*new iteratorImpl (copy.mImpl.myIterator))
    {
    }

    MamaSourceManager::iterator::iterator (const iteratorImpl& copy)
        : mImpl (*new iteratorImpl (copy.myIterator))
    {
    }

    MamaSourceManager::iterator::~iterator ()
    {
        delete &mImpl;
    }

    mama_size_t MamaSourceManager::size () const
    {
        return myImpl.myMap.size ();
    }

    MamaSourceManager::iterator MamaSourceManager::begin ()
    {
        return iterator (iterator::iteratorImpl (myImpl.myMap.begin()));
    }

    MamaSourceManager::iterator MamaSourceManager::end ()
    {
        return iterator (iterator::iteratorImpl (myImpl.myMap.end ()));
    }

    MamaSourceManager::const_iterator MamaSourceManager::begin () const
    {
        return const_iterator (iterator::iteratorImpl (myImpl.myMap.begin ()));
    }

    MamaSourceManager::const_iterator MamaSourceManager::end () const
    {
        return const_iterator (iterator::iteratorImpl (myImpl.myMap.end ()));
    }

    MamaSourceManager::iterator&
    MamaSourceManager::iterator::operator= (const iterator& rhs)
    {
        if (&rhs != this)
        {
            mImpl.myIterator = rhs.mImpl.myIterator;
        }        
        return *this;
    }

    const MamaSourceManager::iterator&
    MamaSourceManager::iterator::operator= (const iterator& rhs) const
    {
        if (&rhs != this)
        {
           mImpl.myIterator = rhs.mImpl.myIterator; 
        }
        return *this;
    }

    const MamaSourceManager::iterator&
    MamaSourceManager::iterator::operator++ () const
    {
        ++mImpl.myIterator;
        return *this;
    }

    MamaSourceManager::iterator&
    MamaSourceManager::iterator::operator++ ()
    {
        ++mImpl.myIterator;
        return *this;
    }

    bool MamaSourceManager::iterator::operator== (const iterator& rhs) const
    {
        return ((&rhs == this) || (mImpl.myIterator == rhs.mImpl.myIterator));
    }

    bool MamaSourceManager::iterator::operator!= (const iterator& rhs) const
    {
        return !operator==(rhs);
    }

    MamaSource* MamaSourceManager::iterator::operator*()
    {
        return mImpl.myIterator->second;
    }

    const MamaSource* MamaSourceManager::iterator::operator*() const
    {
        return mImpl.myIterator->second;
    }

} /* namespace  Wombat */
