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

#ifndef MAMA_PUBLISHER_IMPL_H__
#define MAMA_PUBLISHER_IMPL_H__

#include <mama/MamaSendCompleteCallback.h>

namespace Wombat
{

    /******************************************************************************
     * MamaPublisher implementation.
     */

    class MamaInbox;
    class MamaMsg;
    class MamaPublisher;
    class MamaSendCompleteCallback;

    class MamaPublisherImpl
    {
    public:
        MamaPublisherImpl (void);

        MamaPublisherImpl (MamaPublisher* publisher);

        virtual ~MamaPublisherImpl (void);

        virtual void destroy (void);

        virtual void destroyEx (void);

        virtual void create (MamaTransport*  transport,
                             const char*     topic,
                             const char*     source = NULL,
                             const char*     root   = NULL);

        virtual void createWithCallbacks (MamaTransport*  transport,
                                          MamaQueue*      queue,
                                          MamaPublisherCallback* cb,
                                          void*           closure,
                                          const char*     topic,
                                          const char*     source,
                                          const char*     root);

        virtual void send (MamaMsg* msg) const;

        virtual void sendWithThrottle (MamaMsg*                   msg,
                                       MamaSendCompleteCallback*  cb,
                                       void*                      closure) const;

        virtual void sendFromInbox (MamaInbox* inbox,
                                    MamaMsg*   msg) const;

        virtual void sendFromInboxWithThrottle (MamaInbox*                 inbox,
                                                MamaMsg*                   msg,
                                                MamaSendCompleteCallback*  cb,
                                                void*                      closure) const;


        virtual void sendReplyToInbox (const MamaMsg&  request,
                                       MamaMsg*        reply) const;

        virtual void sendReplyToInbox (mamaMsgReply    replyHandle,
                                       MamaMsg*        reply) const;

        virtual mamaPublisherState getState() const;

        virtual const char* stringForState (mamaPublisherState state) const;

        virtual const char* getRoot () const;

        virtual const char* getSource () const;

        virtual const char* getSymbol () const;

        MamaPublisher* mParent;

    private:
        static void MAMACALLTYPE onPublisherCreate (mamaPublisher publisher,
                                                    void*         closure);
        static void MAMACALLTYPE onPublisherDestroy (mamaPublisher publisher,
                                                     void*         closure);

        static void MAMACALLTYPE onPublisherError (mamaPublisher publisher,
                                                   mama_status status,
                                                   const char* info,
                                                   void*       closure);

        mamaPublisher mPublisher;
        MamaPublisherCallback* mCallback;
        void* mClosure;
    };

} // namespace Wombat

#endif // MAMA_PUBLISHER_IMPL_H__
