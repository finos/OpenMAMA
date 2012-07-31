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

#ifndef MAMA_TRANSPORT_CPP_H__
#define MAMA_TRANSPORT_CPP_H__

#include "mama/mama.h"

namespace Wombat 
{

    class MamaSymbolMap;
    class MamaTransport;
    class MamaConnection;
    class MamaServerConnection;

    /**
     * Transport callback.
     */
    class MAMACPPExpDLL MamaTransportCallback
    {
    public:
        virtual ~MamaTransportCallback() 
        {
        };

        /**
         * Invoked on a publisher when a subscriber disconnects.
         *
         * @param transport The transport which has disconnected.
         * @param platformInfo Info associated with the event.
         *
         * The cause and platformInfo are supplied only by some middlewares.
         * The information provided by platformInfo is middleware specific.
         * The following middlewares are supported:
         *
         * tibrv: provides the char* version of the tibrv advisory message.
         * wmw:   provides a pointer to a C mamaConnection struct for the event
         */
        virtual void onDisconnect (
            MamaTransport*  transport, 
            const void*     platformInfo)
        {
            return;
        }

        /**
         * Invoked when the transport reconnects 
         *
         * @param transport The transport which has reconnected.
         * @param platformInfo Info associated with the event.
         *
         * The cause and platformInfo are supplied only by some middlewares.
         * The information provided by platformInfo is middleware specific.
         * The following middlewares are supported:
         *
         * tibrv: provides the char* version of the tibrv advisory message.
         * wmw:   provides a pointer to a C mamaConnection struct for the event
         */
        virtual void onReconnect (
            MamaTransport*  transport,
            const void*     platformInfo)
        {
            return;
        }

        /**
         * Invoked when the quality of this transport changes.
         *
         * @param transport The transport on which the quality has changed.
         * @param cause The cause of the quality event.
         * @param platformInfo Info associated with the quality event.
         *
         * The cause and platformInfo are supplied only by some middlewares.
         * The information provided by platformInfo is middleware specific.
         * The following middlewares are supported:
         *
         * tibrv: provides the char* version of the tibrv advisory message.
         */
        virtual void onQuality (
            MamaTransport*     transport,
            short              cause,
            const void*        platformInfo) = 0;

        /** 
         * Invoked on the subscriber when the transport connects.
         * 
         * @param transport The transport which has connected.
         * @param platformInfo Info associated with the event.
         *
         * The cause and platformInfo are supplied only by some middlewares.
         * The information provided by platformInfo is middleware specific.
         * The following middlewares are supported:
         *
         * wmw:   provides a pointer to a C mamaConnection struct for the event
         */
        virtual void onConnect (
            MamaTransport*  transport,
            const void*     platformInfo)
        {
            return;
        }

        /**
         * Invoked on the publisher when the transport accepts a connection.
         *
         * @param transport The transport which has accepted.
         * @param platformInfo Info associated with the event.
         *
         * The cause and platformInfo are supplied only by some middlewares.
         * The information provided by platformInfo is middleware specific.
         * The following middlewares are supported:
         *
         * wmw:   provides a pointer to a C mamaConnection struct for the event
         */
        virtual void onAccept (
            MamaTransport*   transport,
            const void*      platformInfo)
        {
            return;
        }

         /**
         * Invoked on the publisher when the transport accepts a reconnection.
         *
         * @param transport The transport which has reconnected on
         * @param platformInfo Info associated with the event.
         *
         * The cause and platformInfo are supplied only by some middlewares.
         * The information provided by platformInfo is middleware specific.
         * The following middlewares are supported:
         *
         * wmw:   provides a pointer to a C mamaConnection struct for the event
         */
        virtual void onAcceptReconnect (
            MamaTransport*  transport,
            const void*     platformInfo)
        {
            return;
        }

        /**
         * Invoked on the subscriber when the transport disconnects from the publisher.
         *
         * @param transport The transport which has disconnected on
         * @param platformInfo Info associated with the event.
         *
         * The cause and platformInfo are supplied only by some middlewares.
         * The information provided by platformInfo is middleware specific.
         * The following middlewares are supported:
         *
         * wmw:   provides a pointer to a C mamaConnection struct for the event
         */
        virtual void onPublisherDisconnect (
            MamaTransport*  transport,
            const void*     platformInfo)
        {
            return;
        }

        /** 
         * Invoked on the subscriber when the naming service connects.
         * 
         * @param transport The transport which has connected.
         * @param platformInfo Info associated with the event.
         */
        virtual void onNamingServiceConnect (
            MamaTransport*  transport,
            const void*     platformInfo)
        {
            return;
        }

        /** 
         * Invoked on the subscriber when the naming service disconnects.
         * 
         * @param transport The transport which has connected.
         * @param platformInfo Info associated with the event.
         */
        virtual void onNamingServiceDisconnect (
            MamaTransport*  transport,
            const void*     platformInfo)
        {
            return;
        }
    };

    /**
     * The MamaTransport class provides market data functionality.
     */
    class MAMACPPExpDLL MamaTransport 
    {
    public:
        /**
         * Construct a MamaTransport. Call create to create the transport.
         */
        MamaTransport          ();
        virtual ~MamaTransport ();

        /**
         * Construct a MamaTransport that wraps a mamaTransport from the C API.
         * Mama uses this internally. C++ Applications should create C++
         * MamaTransport objects through the no-argument constructor, and call
         * MamaTransport::create().
         *
         * MamaTransport objects created with this method do not deallocate or
         * destroy the underlying c Transport because that responsibility lies
         * with the creator.
         */
        MamaTransport (
            mamaTransport  cTransport);
        
        /**
         * Create a transport. Platform specific parameters are read from
         * the properties file. The parameters are
         * dependent on the underlying messaging transport.
         * 
         * @param name The transport name
         * @param bridgeImpl The middleware-specific bridge structure
         */
        void create (
            const char*  name,
            mamaBridge   bridgeImpl);

        /**
         * set the transport name.
         * The name string is copied by the object.
         *
         * @param name  The transport name.
         */
        void setName (
            const char* name);
        
        /**
         * get the transport name.
         */
        const char* getName () const;

        /**
         * get the middleware name.
         */
        const char* getMiddleware() const;

        /**
         * Return the outbound throttle rate in messages/second.
         *
         * @return The throttle rate.
         *
         */
        double getOutboundThrottle (
            mamaThrottleInstance instance = MAMA_THROTTLE_DEFAULT) const;

        /**
         * Set the throttle rate for outbound message. This rate controls
         * the rate at which methods sent with sendWithThrottle (void)
         * are sent. A value of 0.0 disables throttling.
         *
         * @param outboundThrottle The rate in messages/second.
         * @param instance the mamaThrottleInstance to use
         */
        void setOutboundThrottle (
            double               outboundThrottle,
            mamaThrottleInstance instance = MAMA_THROTTLE_DEFAULT);

        /**
         * Set the transport callback.
         */
        void setTransportCallback (
            MamaTransportCallback*  callback);
        
         /**
         * Get the transport callback.
         */
        MamaTransportCallback* getTransportCallback ();

        /**
         * Set the symbology mapping class.
         *
         * @param mapper A symbol mapping class.
         */
        void setSymbolMap (
            const MamaSymbolMap*  mapper);

        /**
         * Return the symbology mapping class.
         */
        const MamaSymbolMap* getSymbolMap () const;

        /**
         * Set the description for the transport.
         * The description string is copied by the object.
         *
         * @param description The transport description.
         */
        void setDescription (
            const char*  description);

        /**
         * Return the description string for the transport.
         * Do not alter or free the string returned by this
         * method.
         *
         * @return const char* The transport description.
         */
        const char* getDescription () const;

        /*
         * Find a connection for the specified IP address and port. If the port is 0 
         * the first connection for the specified IP address is returned.
         *
         * The caller must delete() the returned value.
         */
        MamaConnection* findConnection (
            const char*  IpAddress,
            uint16_t     port);

        /*
         * Return a list of all connections.
         *
         * Note. The results (list and len) must be passed to freeAllConnections
         * to free the resources allocated by this call.
         */
        virtual void getAllConnections (
            MamaConnection**&  list,
            uint32_t&          len);

        /*
         * Free array returned by getAllConnections
         */
        virtual void freeAllConnections (
            MamaConnection**  list,
            uint32_t          len);

        /*
         * Return a list of all server connections.
         *
         * Note. The results (list and len) must be passed to
         * freeAllServerConnections to free the resources allocated by this call.
         */
        virtual void getAllServerConnections (
            MamaServerConnection**&  list,
            uint32_t&                len);

        /*
         * Free array returned by getAllServerConnections.
         */
        virtual void freeAllServerConnections (
            MamaServerConnection**  list,
            uint32_t&               len);

        /**
         * Set whether to invoke the quality callback for all subscriptions
         * whenever a data quality event occurs (the default), or 
         * whether to invoke the quality callback only when data subsequently arrives for a
         * subscription.
         *
         * If set to true, an onQuality callback will be invoked for a subscription 
         * whenever a data quality event occurs on the transport, even in between 
         * updates for that description.  If set to false, the onQuality callback 
         * will not be called when the data quality event occurs on the transport.  
         * However, it will still be invoked when an update arrives for the subscription.
         *
         * @param invokeQualityForAllSubscs Whether to invoke quality callback for
         *                                  all subscriptions
         */
        void setInvokeQualityForAllSubscs (
            bool  invokeQualityForAllSubscs);

        /**
         * Get whether the transport has been set to invoke the quality callback
         * for all subscriptions whenever a data quality event occurs.
         *
         * @return Whether transport has been set to invoke quality callback for
         *         all subscriptions
         */
        bool getInvokeQualityForAllSubscs () const;

        /**
         * Get the data quality for the transport.
         *
         * Currently only supported for the Tibco RV
         * middleware. Returns OK for all other middlewares.
         *
         * @return The Quality of the transport
         */
        mamaQuality getQuality () const;

        /**
         * Request conflation for the transport.
         *
         * Currently only supported for WMW.
         */
        void requestConflation () const;

        /**
         * Request an end to conflation for the transport.
         *
         * Currently only supported for WMW.
         */
        void requestEndConflation () const;


        // Access to C types for implementation of related classes.
        mamaTransport        getCValue    ();
        const mamaTransport  getCValue    () const;
        
        /**
         * Return the underlying native transport. Applications should avoid this
         * method if possible as it may result in non-portable, middleware specific
         * code. Callers must cast the nativeTport to the appropriate type. 
         *
         *
         * Note: this method returns the underlying C construct not a C++ object.
         *
         */
        void* getNativeTransport (
            int  index);

        /**
	     * Disable refreshing of subscriptions on this transport.
	     */
        void disableRefresh (
            bool  disable);

        struct              MamaTransportImpl;
        MamaTransportImpl*  mPimpl;

    private:
        mamaTransport       mTransport;
    };

} // namespace Wombat

#endif // MAMA_TRANSPORT_CPP_H__
