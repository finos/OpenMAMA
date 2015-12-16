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

#include "mama/mamacpp.h"
#include "mamacppinternal.h"
#include "mama/conflation/MamaConnection.h"
#include "mama/conflation/MamaServerConnection.h"
#include "mamainternal.h"

namespace Wombat
{

    struct MamaTransport::MamaTransportImpl 
    {
        MamaTransportImpl ();
         
        void  setSymbolMap       (const MamaSymbolMap*  map);
        const MamaSymbolMap*     getSymbolMap () const;

        MamaTransportTopicEventCallback*    myTopicEventCallback;
        MamaQueue*               myTopicEventQueue;
        MamaTransportCallback*   mCallback;
        const MamaSymbolMap*     mMap;
        bool                     mDeleteCTransport;
    };
        

    extern "C" 
    {
        int MAMACALLTYPE symbolMapWrapperFunc (void*        closure,
                                  char*        result,
                                  const char*  symbol,
                                  size_t       maxLen)
        {
            const MamaSymbolMap*  map = (const MamaSymbolMap*) closure;
            bool found = map->map (result, symbol, maxLen);
            return found == true ? 1 : 0;
        }
    }

    extern "C"
    {
        void MAMACALLTYPE transportTopicEventCb (mamaTransport tport,
                                            mamaTransportTopicEvent event,
                                            const char* topic,
                                            const void* platformInfo,
                                            void* closure)
    {
        MamaTransport* transport = static_cast<MamaTransport*> (closure);

        switch (event)
        {
            case MAMA_TRANSPORT_TOPIC_SUBSCRIBED:
                transport->mPimpl->myTopicEventCallback->onTopicSubscribe (transport, topic, platformInfo);
                return;
            case MAMA_TRANSPORT_TOPIC_UNSUBSCRIBED:
                transport->mPimpl->myTopicEventCallback->onTopicUnsubscribe (transport, topic, platformInfo);
                return;
            case MAMA_TRANSPORT_TOPIC_PUBLISH_ERROR:
                transport->mPimpl->myTopicEventCallback->onTopicPublishError (transport, topic, platformInfo);
                return;
            case MAMA_TRANSPORT_TOPIC_PUBLISH_ERROR_NOT_ENTITLED:
                transport->mPimpl->myTopicEventCallback->onTopicPublishErrorNotEntitled (transport, topic, platformInfo);
                return;
            case MAMA_TRANSPORT_TOPIC_PUBLISH_ERROR_BAD_SYMBOL:
                transport->mPimpl->myTopicEventCallback->onTopicPublishErrorBadSymbol (transport, topic, platformInfo);
                return;
            default:
                return;
        }
    }
    }

    extern "C"
    {
        void MAMACALLTYPE transportCb (mamaTransport       tport,
                          mamaTransportEvent  ev,
                          short               cause,
                          const void*         platformInfo,
                          void*               closure)
        {
            MamaTransport* transport = static_cast<MamaTransport*> (closure);
            switch (ev)
            {
            case MAMA_TRANSPORT_DISCONNECT:
                 transport->mPimpl->mCallback->onDisconnect (transport, platformInfo);
                 return;
            case MAMA_TRANSPORT_RECONNECT:
                 transport->mPimpl->mCallback->onReconnect (transport, platformInfo);
                 return;
            case MAMA_TRANSPORT_QUALITY:
                 transport->mPimpl->mCallback->onQuality (transport, cause, platformInfo);
                 return;
            case MAMA_TRANSPORT_CONNECT:
                 transport->mPimpl->mCallback->onConnect   (transport, platformInfo);
                 return;
            case MAMA_TRANSPORT_ACCEPT:
                 transport->mPimpl->mCallback->onAccept   (transport, platformInfo);
                 return;
            case MAMA_TRANSPORT_ACCEPT_RECONNECT:
                 transport->mPimpl->mCallback->onAcceptReconnect   (transport, platformInfo);
                 return;
            case MAMA_TRANSPORT_PUBLISHER_DISCONNECT:
                 transport->mPimpl->mCallback->onPublisherDisconnect   (transport, platformInfo);
                 return;
            case MAMA_TRANSPORT_NAMING_SERVICE_CONNECT:
                 transport->mPimpl->mCallback->onNamingServiceConnect   (transport, platformInfo);
                 return;
            case MAMA_TRANSPORT_NAMING_SERVICE_DISCONNECT:
                 transport->mPimpl->mCallback->onNamingServiceDisconnect   (transport, platformInfo);
                 return;

            default:
                 return;
            }
        }
    }

    class TransportTopicTestCallback : public MamaTransportTopicEventCallback
    {
    public:
        TransportTopicTestCallback ( MamaTransportTopicEventCallback* ucallback) 
        {
            usercallback = ucallback;
        }
        
        virtual ~TransportTopicTestCallback() {}

        void onTopicSubscribe (MamaTransport* tport,
                               const char* topic,
                               const void* platformInfo)
        {
            try
            {
                usercallback->onTopicSubscribe(tport, topic, platformInfo);
            }
            catch (...)
            {
                fprintf(stderr, "MamaTransportTopicEventCallback onTopicSubscribe, EXCEPTION CAUGHT\n");
            }    
        }
        
        void onTopicUnsubscribe (MamaTransport* tport,
                                 const char* topic,
                                 const void* platformInfo)
        {
            try
            {
                usercallback->onTopicUnsubscribe(tport, topic, platformInfo);
            }
            catch (...)
            {
                fprintf(stderr, "MAMATransportTopicEventCallback onTopicUnsubscribe, EXCEPTION CAUGHT\n");
            }
        }

        void onTopicPublishError (MamaTransport* tport,
                                  const char* topic,
                                  const void* platformInfo)
        {
            try
            {
                usercallback->onTopicPublishError(tport, topic, platformInfo);
            }
            catch (...)
            {
                fprintf(stderr, "MAMATransportTopicEventCallback onTopicPublishError, EXCEPTION CAUGHT\n");
            }
        }

        void onTopicPublishErrorNotEntitled (MamaTransport* tport,
                                             const char* topic,
                                             const void* platformInfo)
        {
            try
            {
                usercallback->onTopicPublishErrorNotEntitled(tport, topic, platformInfo);
            }
            catch (...)
            {
                fprintf(stderr, "MAMATransportTopicEventCallback onTopicPublishErrorNotEntitled, EXCEPTION CAUGHT\n");
            }
        }

        void onTopicPublishErrorBadSymbol (MamaTransport* tport,
                                           const char* topic,
                                           const void* platformInfo)
        {
            try
            {
                usercallback->onTopicPublishErrorBadSymbol(tport, topic, platformInfo);
            }
            catch (...)
            {
                fprintf(stderr, "MAMATransportTopicEventCallback onTopicPublishErrorBadSymbol, EXCEPTION CAUGHT\n");
            }
        }

    private:
      MamaTransportTopicEventCallback* usercallback;  

    };
    class TransportTestCallback : public MamaTransportCallback
    {
    public:
        TransportTestCallback          (MamaTransportCallback* userCallback) 
        {
            mUserCallback = userCallback;
        }

        virtual ~TransportTestCallback () {}

        void onDisconnect (MamaTransport  *tport, 
                             const void*    platformInfo)
        {
            try {
                mUserCallback->onDisconnect (tport, platformInfo);
            }
            catch (...)
            {
                fprintf (stderr, "MamaTransportCallback onDisconnect EXCEPTION CAUGHT\n");
            }
        }

        void onReconnect (MamaTransport  *tport, 
                            const void*    platformInfo)
        {
            try 
            {
                mUserCallback->onReconnect (tport, platformInfo);
            }
            catch (...)
            {
                fprintf (stderr, "MamaTransportCallback onReconnect EXCEPTION CAUGHT\n");
            }
        }

        void onQuality (MamaTransport*  tport,
                        short           cause,
                        const void*     platformInfo)
        {
            try 
            {
                mUserCallback->onQuality (tport, cause, platformInfo);
            }
            catch (...)
            {
                fprintf (stderr, "MamaTransportCallback onQuality EXCEPTION CAUGHT\n");
            }
        }

        void onConnect (MamaTransport*  transport,
                          const void*     platformInfo)
        {
            try 
            {
                mUserCallback->onConnect(transport, platformInfo);
            }
            catch (...)
            {
                fprintf (stderr, "MamaTransportCallback onConnect EXCEPTION CAUGHT\n");
            }
        }

        void onAccept (MamaTransport*  transport,
                         const void*     platformInfo)
        {
            try 
            {
                mUserCallback->onAccept(transport, platformInfo);
            }
            catch (...)
            {
                fprintf (stderr, "MamaTransportCallback onAccept EXCEPTION CAUGHT\n");
            }
        }

        void onAcceptReconnect (MamaTransport*  transport,
                                  const void*     platformInfo)
        {
            try 
            {
                mUserCallback->onAcceptReconnect (transport, platformInfo);
            }
            catch (...)
            {
                fprintf (stderr, "MamaTransportCallback onAcceptReconnect EXCEPTION CAUGHT\n");
            }
        }

        void onPublisherDisconnect (MamaTransport*  transport,
                                      const void*     platformInfo)
        {
            try 
            {
                mUserCallback->onPublisherDisconnect (transport, platformInfo);
            }
            catch (...)
            {
                fprintf (stderr, "MamaTransportCallback onPublisherDisconnect EXCEPTION CAUGHT\n");
            }
        }

        void onNamingServiceConnect (MamaTransport*  transport,
                                       const void*     platformInfo)
        {
            try 
            {
                mUserCallback->onNamingServiceConnect (transport, platformInfo);
            }
            catch (...)
            {
                fprintf (stderr, "MamaTransportCallback onNamingServiceConnect EXCEPTION CAUGHT\n");
            }
        }

        void onNamingServiceDisconnect (MamaTransport*  transport,
                                          const void*     platformInfo)
        {
            try 
            {
                mUserCallback->onNamingServiceDisconnect (transport, platformInfo);
            }
            catch (...)
            {
                fprintf (stderr, "MamaTransportCallback onNamingServiceDisconnect EXCEPTION CAUGHT\n");
            }
        }

    private:
        MamaTransportCallback* mUserCallback;
    };


    MamaTransport::MamaTransport (void) 
        : mPimpl     (new MamaTransportImpl)
        , mTransport (NULL)
    {
        mamaTry (mamaTransport_allocate (&mTransport));
        mPimpl->mDeleteCTransport = true;
    }

    MamaTransport::MamaTransport (mamaTransport tport) 
        : mPimpl     (new MamaTransportImpl)
        , mTransport (tport)
    {
        mPimpl->mDeleteCTransport = false;
    }

    MamaTransport::~MamaTransport (void) 
    {
        if (mPimpl)
        {
            if ( mamaInternal_getCatchCallbackExceptions() && mPimpl->mCallback)
            {
                delete mPimpl->mCallback;
            }
            if (mPimpl->mDeleteCTransport && NULL != mTransport)
            {
                mamaTransport_destroy (mTransport);
            }
            delete mPimpl;
            mPimpl = NULL;
        }
    }

    void MamaTransport::create (const char* name, mamaBridge bridgeImpl)
    {
        mamaTry (mamaTransport_create (mTransport, name, bridgeImpl)); 
    }

    void MamaTransport::setName (const char* name)
    {
        mamaTry (mamaTransport_setName (mTransport, name));
    }

    const char* MamaTransport::getName (void) const
    {
        const char* value = NULL;
        mamaTry (mamaTransport_getName (mTransport, &value));
        return value;
    }

    const char* MamaTransport::getMiddleware () const
    {
        return (mamaTransport_getMiddleware (mTransport));
    }

    void MamaTransport::disableRefresh (bool disable)
    {
    }

    double MamaTransport::getOutboundThrottle (mamaThrottleInstance instance) const
    {
        double value;
        mamaTry (mamaTransport_getOutboundThrottle (mTransport, instance, &value));
        return value;
    }

    void MamaTransport::setOutboundThrottle (double outboundThrottle,
                                             mamaThrottleInstance instance)
    {
        mamaTry (mamaTransport_setOutboundThrottle (mTransport, 
                                                    instance, 
                                                    outboundThrottle));
    }

    void MamaTransport::setDescription (const char* description)
    {
        mamaTry (mamaTransport_setDescription (mTransport, description));
    }

    const char* MamaTransport::getDescription () const
    {
        const char* description = NULL;
        mamaTry (mamaTransport_getDescription (mTransport, &description));
        return description;
    }

    void MamaTransport::setInvokeQualityForAllSubscs (bool invokeQualityForAllSubscs)
    {
        mamaTry (mamaTransport_setInvokeQualityForAllSubscs (
                                          mTransport, invokeQualityForAllSubscs));
    }

    bool MamaTransport::getInvokeQualityForAllSubscs () const
    {
        int invokeQualityForAllSubscs;
        mamaTry (mamaTransport_getInvokeQualityForAllSubscs (
                                         mTransport, &invokeQualityForAllSubscs));
        return invokeQualityForAllSubscs;
    }

    mamaQuality MamaTransport::getQuality () const
    {
        mamaQuality quality;
        mamaTry (mamaTransport_getQuality (mTransport, &quality));
        return quality;
    }

    void MamaTransport::setTransportTopicCallback (MamaTransportTopicEventCallback* callback)
    {
        if (mamaInternal_getCatchCallbackExceptions())
        {
            mPimpl->myTopicEventCallback = new TransportTopicTestCallback (callback);
        }
        else 
        {
            mPimpl->myTopicEventCallback = callback;
        }
        mamaTry (mamaTransport_setTransportTopicCallback (
                    mTransport, transportTopicEventCb, this));
    }

    void MamaTransport::setTransportTopicCallback2 (MamaTransportTopicEventCallback* callback, MamaQueue* queue)
    {
        if (mamaInternal_getCatchCallbackExceptions())
        {
            mPimpl->myTopicEventCallback = new TransportTopicTestCallback (callback);
        }
        else 
        {
            mPimpl->myTopicEventCallback = callback;
            mPimpl->myTopicEventQueue = queue;
        }
        mamaTry (mamaTransport_setTransportTopicCallback2 (
                    mTransport, transportTopicEventCb, queue->getCValue(), this));
    }

    MamaQueue* MamaTransport::getTopicEventQueue()
    {
        if (mPimpl) return mPimpl->myTopicEventQueue;
        return NULL;
    }

    void MamaTransport::setTransportCallback (MamaTransportCallback* callback)
    {
        if (mamaInternal_getCatchCallbackExceptions ())
        {
            mPimpl->mCallback = new TransportTestCallback (callback);
        }
        else
        {
            mPimpl->mCallback = callback;
        }
        mamaTry (mamaTransport_setTransportCallback (
                     mTransport, transportCb, this));
    }

    void MamaTransport::setSymbolMap (const MamaSymbolMap*  map)
    {
        mPimpl->mMap = map;
        mamaTransport_setSymbolMapFunc (mTransport, symbolMapWrapperFunc,
                                       (void*)map);
    }

    const MamaSymbolMap* MamaTransport::getSymbolMap () const
    {
        return mPimpl->mMap;
    }

    MamaTransport::MamaTransportImpl::MamaTransportImpl ()
        : myTopicEventCallback  (NULL)
        , myTopicEventQueue     (NULL)
        , mCallback             (NULL)
        , mMap              (NULL)
        , mDeleteCTransport (true)
    {
    }

    MamaConnection* 
    MamaTransport::findConnection (const char* IpAddress, uint16_t port) 
    {
        mamaConnection connection;
        mamaTry (mamaTransport_findConnection (mTransport, 
                                               &connection,
                                               IpAddress, 
                                               port));
        return new MamaConnection (this, connection);
    }

    void MamaTransport::getAllConnections (MamaConnection**& list, uint32_t& len)
    {
        mamaConnection *cons;
        mamaTry (mamaTransport_getAllConnections (mTransport,
                                                  &cons,
                                                  &len));

        list = NULL;
        if (len > 0)
        {
            list = new MamaConnection*[len];
            for (uint32_t i = 0; i < len; i++)
            {
                list[i] = new MamaConnection (this, cons[i]);
            }
            free (cons); // Frees the array but not the elements
        }
    }

    void MamaTransport::freeAllConnections (MamaConnection** list, uint32_t len)
    {
        for (uint32_t i = 0; i < len; i++)
        {
            delete list[i];
        }
        delete [] list;
    }

    void MamaTransport::getAllServerConnections (MamaServerConnection**& list,
                                                 uint32_t&               len)
    {
        mamaServerConnection* serverCons;
        mamaTry (mamaTransport_getAllServerConnections (mTransport,
                                                        &serverCons,
                                                        &len));

        list = NULL;
        if (len > 0)
        {
            list = new MamaServerConnection*[len];
            for (uint32_t i = 0; i < len; i++)
            {
                list[i] = new MamaServerConnection (serverCons[i]);
            }
            free (serverCons); // Frees the array but not the elements
        }
    }

    void MamaTransport::freeAllServerConnections (MamaServerConnection** list,
                                                  uint32_t&              len)
    {
        for (uint32_t i = 0; i < len; i++)
        {
            delete list[i];
        }
        delete [] list;
    }

    mamaTransport MamaTransport::getCValue ()
    {
        return mTransport;
    }

    const mamaTransport MamaTransport::getCValue () const
    {
        return mTransport;
    }

    void* MamaTransport::getNativeTransport (int index) 
    {
        void* result;
        mamaTry (mamaTransport_getNativeTransport (mTransport, index, &result));
        return result;
    }

    void MamaTransport::requestConflation () const
    {
        mamaTry (mamaTransport_requestConflation (mTransport));
    }

    void MamaTransport::requestEndConflation () const
    {
        mamaTry (mamaTransport_requestEndConflation (mTransport));
    }

} // namespace Wombat
