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

#ifndef MAMA_PUBLISHER_CPP_H__
#define MAMA_PUBLISHER_CPP_H__

#include "mama/mamacpp.h"
#include "mama/MamaSendCompleteCallback.h"

namespace Wombat 
{
    class MamaInbox;
    class MamaPublisherImpl;
    class MamaSendCompleteCallback;

    /**
     * The publisher class publishes messages to basic or market data
     * subscriptions depending on the underlying transport.
     *
     * For basic transports the source parameter must be NULL.
     */
    class MAMACPPExpDLL MamaPublisher
    {
    public: 
        virtual ~MamaPublisher (void);

        MamaPublisher (void);

        /**
         * Create a MAMA publisher for the corresponding transport. If the
         * transport is a market data transport, as opposed to a "basic"
         * transport, the topic corresponds to the symbol. For a basic
         * transport, the source and root get ignored.
         *
         * @param transport The transport to use. Must be a basic transport.
         * @param topic for basic publishers. Symbol for market data topics.
         * @param source The source for market data publishers.
         * @param root The root for market data publishers. Used internally.
         */
        virtual void create (
            MamaTransport*  transport,
            const char*     topic,
            const char*     source = NULL,
            const char*     root   = NULL);

        virtual void send (
            MamaMsg*  msg) const;

        virtual void sendWithThrottle (
            MamaMsg*                   msg, 
            MamaSendCompleteCallback*  cb,
            void*                      closure) const ;

        virtual void sendFromInbox (
            MamaInbox*  inbox, 
            MamaMsg*    msg) const;

        virtual void sendFromInboxWithThrottle(
            MamaInbox*                 inbox,
            MamaMsg*                   msg,
            MamaSendCompleteCallback*  cb,
            void*                      closure) const;
       
        virtual void sendReplyToInbox (
            const MamaMsg&  request, 
            MamaMsg*        reply) const;

        virtual void sendReplyToInbox (
            mamaMsgReply  replyHandle, 
            MamaMsg*      reply) const;

        virtual void destroy (void);

    protected:
        MamaPublisher (MamaPublisherImpl*);
        
        MamaPublisherImpl*  mPimpl;
    };

} // namespace Wombat
#endif // MAMA_PUBLISHER_CPP_H__
