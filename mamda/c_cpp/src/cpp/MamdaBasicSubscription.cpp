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

#include <stdint.h>
#include <mamda/MamdaBasicSubscription.h>
#include <mamda/MamdaMsgListener.h>       // MamdaBasicMsgListener interface
#include <mamda/MamdaErrorListener.h>     // MamdaBasicErrorListener interface
#include <mamda/MamdaQualityListener.h>   // MamdaQualityListener interface
#include <mama/mamacpp.h>
#include <MamdaErrorCode.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace Wombat
{

    struct MamdaBasicSubscription::MamdaBasicSubscriptionImpl 
        : MamaBasicSubscriptionCallback
    {
    public:
        MamdaBasicSubscriptionImpl (MamdaBasicSubscription& subscription);

        virtual ~MamdaBasicSubscriptionImpl() {};

        void onMsg     (MamaBasicSubscription*  subscription,
                        MamaMsg&                msg);

        void onCreate  (MamaBasicSubscription*  subscription);

        void onError   (MamaBasicSubscription*  subscription,
                        const MamaStatus&       status,
                        const char*             subject);

        MamdaBasicSubscription&             mBasicSubscription;
        string                              mSymbol;
        MamaTransport*                      mTransport;
        MamaQueue*                          mQueue;
        double                              mTimeout;
        void*                               mClosure;
        vector<MamdaBasicMsgListener*>      mMsgListeners;
        vector<MamdaBasicErrorListener*>    mErrorListeners;
        vector<MamdaBasicQualityListener*>  mQualityListeners;
        MamaBasicSubscription*              mMamaBasicSubscription;
    };


    MamdaBasicSubscription::MamdaBasicSubscription ()
        : mImpl (*new MamdaBasicSubscriptionImpl (*this))
    {
    }

    MamdaBasicSubscription::~MamdaBasicSubscription ()
    {
        deactivate();
        delete &mImpl;
    }

    void MamdaBasicSubscription::addMsgListener (
        MamdaBasicMsgListener*  listener)
    {
        mImpl.mMsgListeners.push_back (listener);
    }

    void MamdaBasicSubscription::addQualityListener (
        MamdaBasicQualityListener*  listener)
    {
        mImpl.mQualityListeners.push_back (listener);
    }

    void MamdaBasicSubscription::addErrorListener (
        MamdaBasicErrorListener*  listener)
    {
        mImpl.mErrorListeners.push_back(listener);
    }

    void MamdaBasicSubscription::create (
        MamaTransport*   transport,
        MamaQueue*       queue,
        const char*      symbol,
        void*            closure)
    {
        mImpl.mTransport = transport;
        mImpl.mQueue     = queue;
        mImpl.mSymbol    = symbol;

        if (closure)
        {
            mImpl.mClosure = closure;
        }

        activate();
    }

    void MamdaBasicSubscription::activate ()
    {
        /* Make sure it has been deactivated first */
        if (mImpl.mMamaBasicSubscription != NULL) 
            return;
        
        mImpl.mMamaBasicSubscription = new MamaSubscription;
        mImpl.mMamaBasicSubscription->createBasic (
            mImpl.mTransport,
            mImpl.mQueue,
            &mImpl,
            mImpl.mSymbol.c_str());
    }

    void MamdaBasicSubscription::deactivate ()
    {
        if (mImpl.mMamaBasicSubscription)
        {
            delete mImpl.mMamaBasicSubscription;
        }

        mImpl.mMamaBasicSubscription = NULL;
    }

    void MamdaBasicSubscription::setSymbol (const char* symbol)
    {
        mImpl.mSymbol = symbol;
    }

    const char* MamdaBasicSubscription::getSymbol() const
    {
        return mImpl.mSymbol.c_str();
    }

    void MamdaBasicSubscription::setClosure (void* closure)
    {
        mImpl.mClosure = closure;
    }

    void* MamdaBasicSubscription::getClosure() const
    {
        return mImpl.mClosure;
    }

    void MamdaBasicSubscription::setTransport (MamaTransport* transport)
    {
        mImpl.mTransport = transport;
    }

    MamaTransport* MamdaBasicSubscription::getTransport() const
    {
        return mImpl.mTransport;
    }

    void MamdaBasicSubscription::setQueue (MamaQueue* queue)
    {
        mImpl.mQueue = queue;
    }

    MamaQueue* MamdaBasicSubscription::getQueue() const
    {
        return mImpl.mQueue;
    }


    MamdaBasicSubscription::MamdaBasicSubscriptionImpl::MamdaBasicSubscriptionImpl (
        MamdaBasicSubscription&  subscription)
        : mBasicSubscription     (subscription)
        , mTransport             (NULL)
        , mQueue                 (NULL)
        , mTimeout               (10.0)
        , mClosure               (NULL)
        , mMamaBasicSubscription (NULL)
    {
    }



    void MamdaBasicSubscription::MamdaBasicSubscriptionImpl::onMsg (
        MamaBasicSubscription*  subscription,
        MamaMsg&                msg)
    {
        short   msgType   = 0;
        int     msgStatus = 0;

        try
        {
            msgType   = msg.getType  ();
            msgStatus = msg.getStatus();

            switch (msgType)
            {
                case MAMA_MSG_TYPE_DELETE:
                case MAMA_MSG_TYPE_EXPIRE:
                    return;
            }

            switch (msgStatus)
            {
                case MAMA_MSG_STATUS_BAD_SYMBOL:
                case MAMA_MSG_STATUS_EXPIRED:
                case MAMA_MSG_STATUS_TIMEOUT:
                    return;
            }
        }
        catch (...)
        {
            // silent catch as these fields
            // may not be in the message.
        }

        unsigned long size = mMsgListeners.size();

        for (unsigned long i = 0; i < size; ++i)
        {
            MamdaBasicMsgListener* listener = mMsgListeners[i];

            listener->onMsg (
                &mBasicSubscription, 
                msg, 
                msgType);
        }
    }

    void MamdaBasicSubscription::MamdaBasicSubscriptionImpl::onCreate (
        MamaBasicSubscription* subscription)
    {
    }

    void MamdaBasicSubscription::MamdaBasicSubscriptionImpl::onError (
        MamaBasicSubscription*  subscription,
        const MamaStatus&       status,
        const char*             subject)
    {
        MamdaErrorCode      errorCode     = MAMDA_ERROR_NO_ERROR;
        MamdaErrorSeverity	severity      = MAMDA_SEVERITY_HIGH;
    
        errorCode = errorCodeForMamaStatus (status);
        severity =  severityForErrorCode (errorCode);

        /* Deactivate the subsctription if HIGH severity.  If an
         * application *really* wants to, it could reactivate the
         * subsciption. */
        if (severity == MAMDA_SEVERITY_HIGH)
        {
            mBasicSubscription.deactivate();
        }

        unsigned long size = mErrorListeners.size();

        for (unsigned long i = 0; i < size; ++i)
        {
            MamdaBasicErrorListener* listener = mErrorListeners[i];

            listener->onError (&mBasicSubscription, 
                                severity, errorCode, 
                                stringForErrorCode(errorCode));
        }
    }

} // namespace
