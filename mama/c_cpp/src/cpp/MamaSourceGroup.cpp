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

#include <mama/MamaSourceGroup.h>
#include <mama/MamaSource.h>
#include "mama/MamaStatus.h"
#include <map>
#include <set>
#include <list>
#include <string>

using std::string;


namespace Wombat
{

    struct SourceGroupContainer
    {
        MamaSource*  mySource;
        mama_u32_t   myWeight;
    };

    typedef  std::map<string,SourceGroupContainer*>     GroupMap;
    typedef  std::set<SourceGroupContainer*>            GroupSet;
    typedef  std::list<MamaSourceStateChangeCallback*>  StateChangedCallbackList;

    struct MamaSourceGroup::MamaSourceGroupImpl
    {
        string                      myName;
        GroupMap                    myMap;
        SourceGroupContainer*       myTopWeightSource;
        StateChangedCallbackList    myStateChangedCallbacks;
      
    };

    MamaSourceGroup::MamaSourceGroup (const char* name)
        : myImpl (*new MamaSourceGroupImpl)
    {
        myImpl.myName.assign (name);
        myImpl.myTopWeightSource = NULL;
    }

    MamaSourceGroup::~MamaSourceGroup()
    {
        // Delete all the maps
        delete &myImpl;
    }

    const char*  MamaSourceGroup::getName () const
    {
        return myImpl.myName.c_str ();
    }

    mama_size_t MamaSourceGroup::size () const
    {
        return myImpl.myMap.size ();
    }

    MamaSource*  MamaSourceGroup::find (const char*  sourceName)
    {
        GroupMap::const_iterator found = myImpl.myMap.find (sourceName);

        if (found != myImpl.myMap.end ())
        {
            return found->second->mySource;
        }
        else
        {
            return NULL;
        }
    }

    const MamaSource* MamaSourceGroup::find (const char*  sourceName) const
    {
        GroupMap::const_iterator found = myImpl.myMap.find (sourceName);

        if (found != myImpl.myMap.end())
        {
            return found->second->mySource;
        }
        else
        {
            return NULL;
        }
    }

    void  MamaSourceGroup::add (MamaSource*  source,
                                mama_u32_t   weight)
    {
        const char* sourceName = source->getId();
        add (sourceName, weight, source);
    }

    void  MamaSourceGroup::add (const char*  sourceName,
                                mama_u32_t   weight,
                                MamaSource*  source)
    {
        if (!sourceName)
        {
            throw MamaStatus (MAMA_STATUS_INVALID_ARG);
        }

        SourceGroupContainer* container = new SourceGroupContainer;

        container->mySource = source;
        container->myWeight = weight;

        myImpl.myMap.insert (GroupMap::value_type (sourceName, container));
    }

    void  MamaSourceGroup::setWeight (const char*  sourceName,
                                      mama_u32_t   weight)
    {
        GroupMap::const_iterator found = myImpl.myMap.find (sourceName);

        if (found == myImpl.myMap.end())
        {
            return;
        }

        SourceGroupContainer* container = found->second;
        container->myWeight             = weight;
    }

    mama_u32_t  MamaSourceGroup::getWeight (const char*  sourceName) const
    {
        GroupMap::const_iterator found = myImpl.myMap.find (sourceName);

        if (found == myImpl.myMap.end ())
        {
            return 0;
        }

        SourceGroupContainer* container = found->second;

        return container->myWeight;
    }

    bool  MamaSourceGroup::reevaluate ()
    {
        bool  changedStates = false;

        GroupMap::const_iterator end = myImpl.myMap.end ();
        GroupMap::const_iterator i;

        mama_u32_t  topWeight = 0;
        GroupSet    topSet;

        // Two loops: one to determine the "top" member; another to change
        // the states.
        for (i = myImpl.myMap.begin(); i != end; ++i)
        {
            SourceGroupContainer* container = i->second;
            if ((container->myWeight > topWeight))
            {
                // Clear and add to set
                topWeight = container->myWeight;
                topSet.clear();
                topSet.insert (container);
                myImpl.myTopWeightSource=container;
            }
            else if ((container->myWeight == topWeight))
            {
                // Just add to set
                topSet.insert (container);
            }
        }

        for (i = myImpl.myMap.begin(); i != end; ++i)
        {
            SourceGroupContainer* container = i->second;

            if (topSet.find(container) != topSet.end())
            {
                mamaSourceState state = container->mySource->getState();
                if (state != MAMA_SOURCE_STATE_OK)
                {
                    container->mySource->setState (MAMA_SOURCE_STATE_OK);
                    changedStates = true;
                }
            }
            else
            {
                mamaSourceState state = container->mySource->getState();
                if (state != MAMA_SOURCE_STATE_OFF)
                {
                    container->mySource->setState (MAMA_SOURCE_STATE_OFF);
                    changedStates = true;
                }
            }
        }

        if (changedStates) 
        {
            StateChangedCallbackList::iterator end = myImpl.myStateChangedCallbacks.end();
            StateChangedCallbackList::iterator i;
            for (i = myImpl.myStateChangedCallbacks.begin(); i != end; ++i)
            {
                MamaSourceStateChangeCallback* cb = *i;
                cb->onStateChanged (*this, myImpl.myTopWeightSource->mySource);
            }    
        }
        
        return changedStates;
    }

    void MamaSourceGroup::registerStateChangeCallback (MamaSourceStateChangeCallback& cb)
    { 
        myImpl.myStateChangedCallbacks.push_back (&cb);
    }

    void MamaSourceGroup::unregisterStateChangeCallback (MamaSourceStateChangeCallback& cb)
    { 
        myImpl.myStateChangedCallbacks.remove (&cb);
    }

    struct MamaSourceGroup::iterator::iteratorImpl
    {
        iteratorImpl () 
        {
        }

        iteratorImpl (const iteratorImpl& copy)
            : myIterator (copy.myIterator)
        {
        }

        iteratorImpl (GroupMap::iterator  iter)
            : myIterator (iter)
        {
        }

        ~iteratorImpl () 
        {
        }

        GroupMap::iterator myIterator;
    };

    MamaSourceGroup::iterator::iterator ()
        : mImpl (*new iteratorImpl)
    {
    }

    MamaSourceGroup::iterator::iterator (const iterator& copy)
        : mImpl (*new iteratorImpl (copy.mImpl.myIterator))
    {
    }

    MamaSourceGroup::iterator::iterator (const iteratorImpl& copy)
        : mImpl (*new iteratorImpl (copy.myIterator))
    {
    }

    MamaSourceGroup::iterator::~iterator ()
    {
        delete &mImpl;
    }

    MamaSourceGroup::iterator MamaSourceGroup::begin ()
    {
        return iterator (iterator::iteratorImpl (myImpl.myMap.begin ()));
    }

    MamaSourceGroup::iterator MamaSourceGroup::end ()
    {
        return iterator (iterator::iteratorImpl(myImpl.myMap.end()));
    }

    MamaSourceGroup::const_iterator MamaSourceGroup::begin () const
    {
        return const_iterator (iterator::iteratorImpl (myImpl.myMap.begin ()));
    }

    MamaSourceGroup::const_iterator MamaSourceGroup::end () const
    {
        return const_iterator (iterator::iteratorImpl (myImpl.myMap.end ()));
    }

    MamaSourceGroup::iterator&
    MamaSourceGroup::iterator::operator= (const iterator& rhs)
    {
        if (&rhs != this)
        {
            mImpl.myIterator = rhs.mImpl.myIterator;
        }        
        return *this;
    }

    const MamaSourceGroup::iterator&
    MamaSourceGroup::iterator::operator= (const iterator& rhs) const
    {
        if (&rhs != this)
        {
            mImpl.myIterator = rhs.mImpl.myIterator;
        }
        return *this;
    }

    const MamaSourceGroup::iterator&
    MamaSourceGroup::iterator::operator++ () const
    {
        ++mImpl.myIterator;
        return *this;
    }

    MamaSourceGroup::iterator&
    MamaSourceGroup::iterator::operator++ ()
    {
        ++mImpl.myIterator;
        return *this;
    }

    bool MamaSourceGroup::iterator::operator== (const iterator& rhs) const
    {
        return ((&rhs == this) || (mImpl.myIterator == rhs.mImpl.myIterator));
    }

    bool MamaSourceGroup::iterator::operator!= (const iterator& rhs) const
    {
        return !operator==(rhs);
    }

    MamaSource* MamaSourceGroup::iterator::operator*()
    {
        return mImpl.myIterator->second->mySource;
    }

    const MamaSource* MamaSourceGroup::iterator::operator*() const
    {
        return mImpl.myIterator->second->mySource;
    }

} /* namespace  Wombat */
