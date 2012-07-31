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

#include "mama/MamaSource.h"
#include "mama/MamaTransport.h"
#include "mama/MamaSubscription.h"
#include "mama/MamaTransportMap.h"
#include "mama/MamaStatus.h"
#include "mamacppinternal.h"
#include <stdexcept>
#include <wombat/wtable.h>

namespace Wombat
{

    typedef struct MamaSourceSubscriptionIteratorClosure_
    {
        MamaSource*                               mSource;
        MamaSubscriptionIteratorCallback*         mCallback;
        void*                                     mData;
    } MamaSourceSubscriptionIteratorClosure;

}

extern "C"
{
    static void Wombat_MamaSource_iterateSources (wtable_t       t,
                                                  void*          data,
                                                  const char*    key,
                                                  void*          closure)
    {
        Wombat::MamaSubscription* sub = (Wombat::MamaSubscription*)data;

        if (!sub) 
        {
            return;
        }

        bool* activate = (bool*)closure;

        if (activate && *activate)
        {
            try
            {
                sub->activate();
            }
            catch (Wombat::MamaStatus& status)
            {
                // Ignoring ... may be already active
            }
        }
        else
        {
            try
            {
                sub->deactivate();
            }
            catch (Wombat::MamaStatus& status)
            {
                // Ignoring ... may be already inactive
            }
        }
    }

    static void Wombat_MamaSource_iterateSubscriptions (wtable_t     t,
                                                        void*        data,
                                                        const char*  key,
                                                        void*        closure) 
    {
        Wombat::MamaSubscription* sub = (Wombat::MamaSubscription*)data;
        Wombat::MamaSourceSubscriptionIteratorClosure* iteratorClosure = 
                    (Wombat::MamaSourceSubscriptionIteratorClosure*)closure;

        if (iteratorClosure->mCallback)
        {
            (iteratorClosure->mCallback)->onSubscription (iteratorClosure->mSource, 
                                                          sub, 
                                                          iteratorClosure->mData);
        }
    }
} // extern "C"

namespace Wombat
{

    MamaSource::MamaSource ()
        : myParent    (NULL)
        , mySubs      (NULL)
        , myTransport (NULL)
        , myBridge    (NULL)
    {
        mamaTry (mamaSource_create (&mySource));
    }

    MamaSource::MamaSource (const char*  id,
                            const char*  transportName,
                            const char*  subscSourceName,
                            mamaBridge   bridge,
                            bool         createTransport)
        : myParent    (NULL)
        , mySubs      (NULL)
        , myTransport (NULL)
        , myBridge    (bridge)
    {
        mamaTry (mamaSource_create (&mySource));

        if (createTransport)
        {
            setTransport (MamaTransportMap::findOrCreate (transportName, bridge));
        }

        setId                  (id);
        setPublisherSourceName (subscSourceName);  
        setTransportName       (transportName);
    }

    MamaSource::MamaSource (const char*     id,
                            MamaTransport*  transport,
                            const char*     subscSourceName)
        : myParent    (NULL)
        , mySubs      (NULL)
        , myTransport (NULL)
    {
        mamaTry (mamaSource_create (&mySource));

        setId                  (id);
        setTransport           (transport);
        setPublisherSourceName (subscSourceName);
    }

    MamaSource::~MamaSource ()
    {
        mamaTry (mamaSource_destroy (mySource));

        if (mySubs)
        {
            wtable_destroy (mySubs);
        }
    }

    void MamaSource::setId (const char*  id)
    {
        mamaTry (mamaSource_setId (mySource, id));
    }

    void MamaSource::setMappedId (const char*  id)
    {
        mamaTry (mamaSource_setMappedId (mySource, id));
    }

    void MamaSource::setDisplayId (const char*  id)
    {
        mamaTry (mamaSource_setDisplayId (mySource, id));
    }

    void MamaSource::setQuality (mamaQuality  quality)
    {
        mamaTry (mamaSource_setQuality (mySource, quality));
    }

    void MamaSource::setState (mamaSourceState  source)
    {
        mamaTry (mamaSource_setState (mySource, source));
    }

    void MamaSource::setParent (MamaSource*  parent)
    {
        myParent = parent;
    }

    void MamaSource::setTransportName (const char*  transportName)
    {
        mamaTry (mamaSource_setTransportName (mySource, transportName));
    }

    void MamaSource::setTransport (MamaTransport*  transport)
    {
        mamaTry (mamaSource_setTransport (mySource, transport->getCValue()));
        myTransport = transport;
    }


    void MamaSource::setPublisherSourceName (const char*  sourceName)
    {
        mamaTry (mamaSource_setSymbolNamespace (mySource, sourceName));
    }

    const char* MamaSource::getId () const
    {
        const char* result = NULL;
        mamaTry (mamaSource_getId (mySource, &result));

        return result;
    }

    const char* MamaSource::getMappedId () const
    {
        const char* result = NULL;
        mamaTry (mamaSource_getMappedId (mySource, &result));

        return result;
    }

    const char* MamaSource::getDisplayId () const
    {
        const char* result = NULL;
        mamaTry (mamaSource_getDisplayId (mySource, &result));

        return result;
    }

    mamaQuality MamaSource::getQuality () const
    {
        mamaQuality result = MAMA_QUALITY_UNKNOWN;
        mamaTry (mamaSource_getQuality (mySource, &result));

        if ((result == MAMA_QUALITY_UNKNOWN) && myParent)
        {
            return myParent->getQuality();
        }
        else
        {
            return result;
        }
    }

    mamaSourceState MamaSource::getState () const
    {
        mamaSourceState result = MAMA_SOURCE_STATE_UNKNOWN;
        mamaTry (mamaSource_getState (mySource, &result));

        if ((result == MAMA_SOURCE_STATE_UNKNOWN) && myParent)
        {
            return myParent->getState();
        }
        else
        {
            return result;
        }
    }

    const MamaSource* MamaSource::getParent () const
    {
        return myParent;
    }

    MamaSource* MamaSource::getParent ()
    {
        return myParent;
    }

    const char* MamaSource::getTransportName () const
    {
        const char* result = NULL;
        mamaTry (mamaSource_getTransportName (mySource, &result));

        return result;
    }

    MamaTransport* MamaSource::getTransport () const
    {
        if (myTransport)
        {
            return myTransport;
        }
        else if (myParent)
        {
            return myParent->getTransport();
        }
        else
        {
            return NULL;
        }
    }


    const char* MamaSource::getPublisherSourceName () const
    {
        const char*  result = NULL;
        mamaTry (mamaSource_getSymbolNamespace (mySource, &result));

        if (!result && myParent)
        {
            return myParent->getPublisherSourceName ();
        }
        return result;
    }

    bool MamaSource::isPartOf (const MamaSource* source) const
    {
        if (this == source) 
        {
            return true;
        }
        if (myParent)
        {
            return myParent->isPartOf (source);
        }

        return false;
    }

    void MamaSource::addSubscription (const char*        symbol,
                                      MamaSubscription*  sub)
    {
        if (!mySubs)
        {
            mySubs = wtable_create ("MamaSource::mySubs", 100);
        }

        MamaSubscription* foundSub = 
            (MamaSubscription*) wtable_lookup (mySubs, symbol);

        if (foundSub)
        {
            return;
        }

        wtable_insert (mySubs, symbol, sub);
    }

    MamaSubscription* MamaSource::removeSubscription (const char*  symbol)
    {
        if (!mySubs)
        {
            return NULL;
        }

        return (MamaSubscription*) wtable_remove (mySubs, symbol);
    }

    void MamaSource::activateSubscriptions ()
    {
        /* Now iterate to change the status of the sources if necessary */
        bool activate = true;
        wtable_for_each (mySubs,
                         Wombat_MamaSource_iterateSources,
                         &activate);


    }

    void MamaSource::deactivateSubscriptions ()
    {
        bool activate = false;
        wtable_for_each (mySubs,
                         Wombat_MamaSource_iterateSources,
                         &activate);
    }

    MamaSubscription* MamaSource::findSubscription (const char*  symbol)
    {
        if (mySubs)
        {
            return (MamaSubscription*) wtable_lookup (mySubs, symbol);
        }
        else if (myParent)
        {
            return myParent->findSubscription (symbol);
        }
        else
        {
            return NULL;
        }
    }

    mamaSource MamaSource::getCValue ()
    {
        return mySource;
    }

    const mamaSource MamaSource::getCValue () const
    {
        return mySource;
    }

    void MamaSource::forEachSubscription (MamaSubscriptionIteratorCallback* callback, 
                                          void*                             closure) 
    {
        MamaSourceSubscriptionIteratorClosure* iteratorClosure;
        
        iteratorClosure = (MamaSourceSubscriptionIteratorClosure*)calloc
                 (1, sizeof (MamaSourceSubscriptionIteratorClosure));

        iteratorClosure->mCallback = callback;
        iteratorClosure->mSource = this;
        
        wtable_for_each (mySubs,
                         Wombat_MamaSource_iterateSubscriptions,
                         (void*)iteratorClosure);

        free ((void*)iteratorClosure);
    }

} /* namespace  Wombat */
