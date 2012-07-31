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

#include <mama/mamacpp.h>
#include <mama/MamaInbox.h>
#include "mamacppinternal.h"
#include "MamaPublisherImpl.h"
#include "mamainternal.h"
#include <mama/MamaSendCompleteCallback.h>

namespace Wombat
{

    class SendCompleteTestCallback : public MamaSendCompleteCallback
    {
    public:
	    SendCompleteTestCallback(MamaSendCompleteCallback* usercallback) 
        {
            mcallback = usercallback;
        }

        virtual ~SendCompleteTestCallback () 
        {
        };

        virtual void onSendComplete (MamaPublisher& publisher,
                                     MamaMsg*       msg,
                                     MamaStatus&    status,
                                     void*          closure);
    private:
	    MamaSendCompleteCallback* mcallback;
    };

    void SendCompleteTestCallback::onSendComplete (MamaPublisher&  publisher, 
                                                   MamaMsg*        msg, 
                                                   MamaStatus&     status,     
                                                   void*           closure)
    {
	    try 
        {
		    mcallback->onSendComplete (publisher, 
                                       msg, 
                                       status, 
                                       closure);
	    }
	    catch (...)
	    {
		    fprintf (stderr, 
                     "MamaSendCompleteCallback onSendComplete EXCEPTION CAUGHT\n");
	    }
    }

    /* Used to store context data for the sendWithThrotle methods */
    struct MamaSendCompleteClosure
    {
        MamaPublisherImpl*        mPublisherImpl;
        MamaSendCompleteCallback* mSendCompleteCallback;
        MamaMsg*                  mSendMessage;
        void*                     mSendCompleteClosure;
    };

    /* Invoked when sendWithThrottle send a message from the queue */
    void MAMACALLTYPE sendCompleteCb (mamaPublisher publisher,
                         mamaMsg       msg,
                         mama_status   status,
                         void*         closure)
    {
        /* Delegate to the C++ callback */
        MamaSendCompleteClosure* sendClosure = 
            static_cast<MamaSendCompleteClosure*>(closure);
        MamaPublisherImpl* pubImpl = sendClosure->mPublisherImpl;
        MamaStatus cppStatus (status);
        
        if (sendClosure->mSendCompleteCallback)
        {
            sendClosure->mSendCompleteCallback->onSendComplete (
                    *(pubImpl->mParent),
                    sendClosure->mSendMessage,
                    cppStatus,
                    sendClosure->mSendCompleteClosure);
        }

        if (mamaInternal_getCatchCallbackExceptions ())
        {
            if (sendClosure->mSendCompleteCallback)
            {
                delete sendClosure->mSendCompleteCallback;
                sendClosure->mSendCompleteCallback = NULL;
            }
        }
        delete sendClosure;
    }

    MamaPublisher::~MamaPublisher (void) 
    {
        if (mPimpl)
        {
            delete mPimpl;
            mPimpl = NULL;
        }
    }

    MamaPublisher::MamaPublisher (MamaPublisherImpl* impl)
        : mPimpl (impl) 
    {
    }

    MamaPublisher::MamaPublisher (void) 
        : mPimpl (new MamaPublisherImpl (this))
    {
    }

    void MamaPublisher::create (MamaTransport*     transport,
                                const char*        topic,
                                const char*        source,
                                const char*        root)
    {
        mPimpl->create (transport, topic, source, root);
    }

    void MamaPublisher::send (MamaMsg* msg) const
    {
        mPimpl->send (msg);
    }

    void MamaPublisher::sendWithThrottle (MamaMsg*                  msg,
                                          MamaSendCompleteCallback* cb,
                                          void*                     closure) const 
    {
        mPimpl->sendWithThrottle (msg, cb, closure);
    }

    void MamaPublisher::sendFromInbox (MamaInbox* inbox, MamaMsg* msg) const
    {
        mPimpl->sendFromInbox (inbox, msg);
    }

    void MamaPublisher::sendFromInboxWithThrottle (MamaInbox*                inbox,
                                                   MamaMsg*                  msg,
                                                   MamaSendCompleteCallback* cb,
                                                   void*                     closure) const
    {
        mPimpl->sendFromInboxWithThrottle (inbox, msg, cb, closure);
    }
       
    void MamaPublisher::sendReplyToInbox (const MamaMsg&  request, 
                                          MamaMsg*        reply) const
    {
        mPimpl->sendReplyToInbox (request, reply);
    }

    void MamaPublisher::sendReplyToInbox(mamaMsgReply  replyHandle, 
                                         MamaMsg*      reply) const
    {
        mPimpl->sendReplyToInbox (replyHandle, reply);
    }

    void MamaPublisher::destroy (void)
    {
        mPimpl->destroy ();
    }

    void MamaPublisherImpl::send (MamaMsg *msg) const
    {
        mamaTry (mamaPublisher_send (mPublisher, msg->getUnderlyingMsg ()));
    } 

    void MamaPublisherImpl::sendWithThrottle (MamaMsg*                  msg,
                                              MamaSendCompleteCallback* cb,
                                              void*                     closure) const
    {
        /* Memory for this is deleted in the C sendComplete callback */
        MamaSendCompleteClosure* sendCompleteClosure = new MamaSendCompleteClosure;

        /* Cast needed as this is a const method */
        sendCompleteClosure->mPublisherImpl         = (MamaPublisherImpl*)this;

        if (mamaInternal_getCatchCallbackExceptions ())
        {
            sendCompleteClosure->mSendCompleteCallback 
                    = new SendCompleteTestCallback (cb);
        }
        else
        {
            sendCompleteClosure->mSendCompleteCallback  = cb;
        }

        sendCompleteClosure->mSendMessage           = msg;
        sendCompleteClosure->mSendCompleteClosure   = closure;
        
        mamaTry (mamaPublisher_sendWithThrottle (mPublisher, 
                                                 msg->getUnderlyingMsg (),
                                                 sendCompleteCb,
                                                 sendCompleteClosure));
    } 

    void MamaPublisherImpl::sendFromInbox (MamaInbox *inbox,
                                           MamaMsg   *msg) const 
    {
        mamaTry (mamaPublisher_sendFromInbox (mPublisher, 
                                              inbox->getCValue (),
                                              msg->getUnderlyingMsg ()));

    }

    void MamaPublisherImpl::sendFromInboxWithThrottle (MamaInbox*                inbox,
                                                       MamaMsg*                  msg,
                                                       MamaSendCompleteCallback* cb,
                                                       void*                     closure) const 
    {
        /* Memory for this is deleted in the C sendComplete callback */
        MamaSendCompleteClosure* sendCompleteClosure = new MamaSendCompleteClosure;

        /*Cast needed as this is a const method*/
        sendCompleteClosure->mPublisherImpl = (MamaPublisherImpl*)this;

        if (mamaInternal_getCatchCallbackExceptions ())
        {
            sendCompleteClosure->mSendCompleteCallback = new SendCompleteTestCallback(cb);
        }
        else
        {
            sendCompleteClosure->mSendCompleteCallback  = cb;
        }

        sendCompleteClosure->mSendMessage           = msg;
        sendCompleteClosure->mSendCompleteClosure   = closure;

        mamaTry (mamaPublisher_sendFromInboxWithThrottle (mPublisher, 
                                                          inbox->getCValue (),
                                                          msg->getUnderlyingMsg (),
                                                          sendCompleteCb,
                                                          sendCompleteClosure));
    }   

    void MamaPublisherImpl::sendReplyToInbox (const MamaMsg  &request, 
                                              MamaMsg        *reply) const 
    {
        mamaTry (mamaPublisher_sendReplyToInbox (mPublisher, 
                                                 request.getUnderlyingMsg (),
                                                 reply->getUnderlyingMsg ()));
    }

    void MamaPublisherImpl::sendReplyToInbox (mamaMsgReply  replyHandle,
                                              MamaMsg       *reply) const
    {
        mamaTry (mamaPublisher_sendReplyToInboxHandle (mPublisher,
                                                       replyHandle,
                                                       reply->getUnderlyingMsg ()));
    }

    void MamaPublisherImpl::create (MamaTransport *transport, 
                                    const char    *topic, 
                                    const char    *source, 
                                    const char    *root)
    {
        if (mPublisher)
        {
            throw MamaStatus (MAMA_STATUS_INVALID_ARG);
        }

        mamaTry (mamaPublisher_create (&mPublisher, 
                                       transport->getCValue(), 
                                       topic, 
                                       source,
                                       root));
    }

    void MamaPublisherImpl::destroy (void)
    {
        if (mPublisher)
        {
            mamaTry (mamaPublisher_destroy (mPublisher));
            mPublisher = NULL;
        }
    }

} // namespace Wombat
