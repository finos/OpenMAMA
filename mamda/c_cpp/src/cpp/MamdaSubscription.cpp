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

#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaFieldState.h>
#include <mama/mamacpp.h>
#include <mama/MamaSource.h>
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;

namespace Wombat
{

    struct MamdaSubscription::MamdaSubscriptionImpl : public MamaSubscriptionCallback
    {
    public:
        MamdaSubscriptionImpl (MamdaSubscription&  subscription);

        virtual ~MamdaSubscriptionImpl () {};

        void destroy   ();

        void onMsg     (MamaSubscription*  subscription,
                        MamaMsg&     msg);

        void onCreate  (MamaSubscription*  subscription);

        void onError   (MamaSubscription*  subscription,
                        const MamaStatus&  status,
                        const char*        subject);

        void onQuality (MamaSubscription*  subscription,
                        mamaQuality        quality,
                        const char*        symbol,
                        short              cause,
                        const void*        platformInfo);

        void clearMsgListeners     ();
        void clearErrorListeners   ();
        void clearQualityListeners ();

        MamdaSubscription&             mSubscription;
        string                         mSymbol;
        string                         mExchange;
        MamaSource*                    mSource;
        MamaQueue*                     mQueue;
        mamaMdDataType                 mMdDataType;
        mamaSubscriptionType           mType;
        mamaServiceLevel               mServiceLevel;
        long                           mServiceLevelOpt;
        void*                          mClosure;
        bool                           mRequireInitial;
        double                         mTimeout;
        int                            mRetries;
        vector<MamdaMsgListener*>      mMsgListeners;
        vector<MamdaErrorListener*>    mErrorListeners;
        vector<MamdaQualityListener*>  mQualityListeners;
        const MamaMsg*                 mLatestMsg;
        MamaSubscription*              mMamaSubscription;  
        bool*                          implValid;
    };


    MamdaSubscription::MamdaSubscription ()
        : mImpl (*new MamdaSubscriptionImpl(*this))
    {
    }

    MamdaSubscription::~MamdaSubscription ()
    {
        destroy();
        delete &mImpl;
    }

    MamdaSubscription* MamdaSubscription::clone() const
    {
        MamdaSubscription* result = new MamdaSubscription;
        if(mImpl.mMamaSubscription)
        {
            result->setMamaSubscription(new MamaSubscription);
        }

        result->setSource         (getSource ());
        result->setSymbol         (getSymbol ());
        result->setQueue          (getQueue());
        result->setType           (getType());
        result->setServiceLevel   (getServiceLevel(), getServiceLevelOpt());
        result->setRequireInitial (getRequireInitial());
        result->setTimeout        (getTimeout());
        result->setRetries        (getRetries());

        return result;
    }

    void MamdaSubscription::create (
        MamaQueue*   queue,
        MamaSource*  source,
        const char*  symbol,
        void*        closure)
    {
        mImpl.mMamaSubscription = new MamaSubscription;  
     
        setSource (source);
        setSymbol (symbol);
        setQueue  (queue);

        if (closure != NULL)
        {
            setClosure (closure);
        }

        activate ();
    }

    void MamdaSubscription::destroy ()
    {
       mImpl.destroy ();
       delete mImpl.mMamaSubscription;
       mImpl.mMamaSubscription = NULL;
    }

    bool MamdaSubscription::isActive () const
    {
        if (mImpl.mMamaSubscription)
        {
            return mImpl.mMamaSubscription->isActive();
        }
        
        return false;
    }

    bool MamdaSubscription::checkDebugLevel (MamaLogLevel level) const
    {
        if (mImpl.mMamaSubscription)
        {
            return mImpl.mMamaSubscription->checkDebugLevel (level);
        }
        
        return false;
    }

    void* MamdaSubscription::getItemClosure()
    {
        if (mImpl.mMamaSubscription)
        {
            return mImpl.mMamaSubscription->getItemClosure ();
        }
        
        return NULL;
    }

    void MamdaSubscription::setItemClosure (void* closure)
    {
        if (mImpl.mMamaSubscription)
        {
            mImpl.mMamaSubscription->setItemClosure (closure);
        }
    }

    void MamdaSubscription::setSource (MamaSource*  source)
    {
        mImpl.mSource = source;
    }

    void MamdaSubscription::setSymbol (const char*  symbol)
    {
        mImpl.mSymbol = symbol;
    }

    void MamdaSubscription::setQueue (MamaQueue*  queue)
    {
        mImpl.mQueue = queue;
    }

    void MamdaSubscription::setMdDataType (mamaMdDataType  mdDataType)
    {
        mImpl.mMdDataType = mdDataType;
    }

    void MamdaSubscription::setType (mamaSubscriptionType  type)
    {
        mImpl.mType = type;
    }

    void MamdaSubscription::setServiceLevel (
        mamaServiceLevel  serviceLevel,
        long              serviceLevelOpt)
    {
        mImpl.mServiceLevel    = serviceLevel;
        mImpl.mServiceLevelOpt = serviceLevelOpt;
    }

    void MamdaSubscription::setRequireInitial (bool  require)
    {
        mImpl.mRequireInitial = require;
    }

    void MamdaSubscription::setTimeout (double  timeout)
    {
        mImpl.mTimeout = timeout;
    }

    void MamdaSubscription::setRetries (int retries)
    {
        mImpl.mRetries = retries;
    }

    void MamdaSubscription::setClosure (void*  closure)
    {
        mImpl.mClosure = closure;
    }

    void MamdaSubscription::setGroupSizeHint (int groupSizeHint)
    {
        if (mImpl.mMamaSubscription)
        {
            mImpl.mMamaSubscription->setGroupSizeHint (groupSizeHint);
        }
    }    

    void MamdaSubscription::addMsgListener (MamdaMsgListener*  listener)
    {
        mImpl.mMsgListeners.push_back(listener);
    }

    vector<MamdaMsgListener*>& MamdaSubscription::getMsgListeners ()
    {
        return mImpl.mMsgListeners;
    }

    void MamdaSubscription::setMamaSubscription (MamaSubscription* subscription)
    {
        mImpl.mMamaSubscription = subscription;
    }

    void MamdaSubscription::addQualityListener (MamdaQualityListener*  listener)
    {
        mImpl.mQualityListeners.push_back(listener);
    }

    void MamdaSubscription::addErrorListener (MamdaErrorListener*  listener)
    {
        mImpl.mErrorListeners.push_back(listener);
    }

    void MamdaSubscription::activate ()
    {
        if (isActive())
        {
            return;
        }
                                                            
        if (!mImpl.mMamaSubscription)
        {
            mImpl.mMamaSubscription = new MamaSubscription();
        }
          
        mImpl.mMamaSubscription->setAppDataType      (mImpl.mMdDataType);
        mImpl.mMamaSubscription->setSubscriptionType (mImpl.mType);
        mImpl.mMamaSubscription->setServiceLevel     (mImpl.mServiceLevel, 
                                                      mImpl.mServiceLevelOpt);
        mImpl.mMamaSubscription->setRequiresInitial  (mImpl.mRequireInitial);
        mImpl.mMamaSubscription->setRetries          (mImpl.mRetries);
        mImpl.mMamaSubscription->setTimeout          (mImpl.mTimeout);

        mImpl.mMamaSubscription->create (mImpl.mQueue,
                                         &mImpl,
                                         mImpl.mSource,
                                         mImpl.mSymbol.c_str(),
                                         NULL);
    }

    void MamdaSubscription::deactivate ()
    {
        if (mImpl.mMamaSubscription)
        {
            mImpl.mMamaSubscription->destroy();
        }    
    }

    void MamdaSubscription::requestRecap ()
    {
    }

    MamaSource* MamdaSubscription::getSource () const
    {
         return mImpl.mSource;
    }

    const char* MamdaSubscription::getSourceName() const
    {
        if (mImpl.mSource)
        {
            return mImpl.mSource->getPublisherSourceName();
        }

        if (mImpl.mMamaSubscription)
        {
            return mImpl.mMamaSubscription->getSubscSource();
        }    
        else
        {
            return NULL;
        }
    }

    const char* MamdaSubscription::getSymbol() const
    {
        return mImpl.mSymbol.c_str();
    }

    const char* MamdaSubscription::getExchange() const
    {
        return mImpl.mExchange.c_str();
    }

    MamaTransport* MamdaSubscription::getTransport() const
    {
        return mImpl.mSource ? mImpl.mSource->getTransport() : NULL;
    }

    MamaQueue* MamdaSubscription::getQueue() const
    {
        return mImpl.mQueue;
    }

    MamaSubscription* MamdaSubscription::getMamaSubscription()
    {
        return mImpl.mMamaSubscription;
    }

    mamaSubscriptionType MamdaSubscription::getType() const
    {
        return mImpl.mType;
    }

    mamaServiceLevel MamdaSubscription::getServiceLevel() const
    {
        return mImpl.mServiceLevel;
    }

    long MamdaSubscription::getServiceLevelOpt() const
    {
        return mImpl.mServiceLevelOpt;
    }

    bool MamdaSubscription::getRequireInitial() const
    {
        return mImpl.mRequireInitial;
    }

    double MamdaSubscription::getTimeout() const
    {
        return mImpl.mTimeout;
    }

    int MamdaSubscription::getRetries() const
    {
        return mImpl.mRetries;
    }

    void* MamdaSubscription::getClosure() const
    {
        return mImpl.mClosure;
    }

    uint32_t MamdaSubscription::getSeqNum() const
    {
        if (mImpl.mLatestMsg) 
            return mImpl.mLatestMsg->getSeqNum ();
        else
            return 0;
    }

    MamdaSubscription::MamdaSubscriptionImpl::MamdaSubscriptionImpl (
        MamdaSubscription&    subscription)
        : mSubscription       (subscription)
        , mSource             (NULL)
        , mQueue              (NULL)
        , mMdDataType         (MAMA_MD_DATA_TYPE_STANDARD)
        , mType               (MAMA_SUBSC_TYPE_NORMAL)
        , mServiceLevel       (MAMA_SERVICE_LEVEL_REAL_TIME)
        , mClosure            (NULL)
        , mRequireInitial     (true)
        , mTimeout            (10.0)
        , mRetries            (MAMA_DEFAULT_RETRIES)
        , mLatestMsg          (NULL)
        , mMamaSubscription   (NULL)
        , implValid           (NULL)
    {
    }

    void MamdaSubscription::MamdaSubscriptionImpl::destroy ()
    {
        if (this != NULL)
        {
            if (implValid)
            {
                *implValid = false;
            }

            mSubscription.deactivate ();
            clearMsgListeners        ();
            clearErrorListeners      ();
            clearQualityListeners    ();
        }
    }

    void MamdaSubscription::MamdaSubscriptionImpl::clearMsgListeners ()
    {
        mMsgListeners.clear();
    }
    void MamdaSubscription::MamdaSubscriptionImpl::clearErrorListeners ()
    {
        mErrorListeners.clear();
    }
    void MamdaSubscription::MamdaSubscriptionImpl::clearQualityListeners ()
    {
        mQualityListeners.clear();
    }

    void MamdaSubscription::MamdaSubscriptionImpl::onMsg (
        MamaSubscription*  subscription,
        MamaMsg&           msg)
    {
        short msgType   = msg.getType();
        int   msgStatus = msg.getStatus();

        mLatestMsg = &msg;

        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
           const char* contentSymbol = "N/A";
           msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contentSymbol);
           
           mama_forceLog (MAMA_LOG_LEVEL_FINE,
                          "MamdaSubscription (%s.%s(%s)) received msg. type: %s status %s\n",
                          mSource->getPublisherSourceName(),
                          mSymbol.c_str (),
                          contentSymbol,
                          msg.getMsgTypeName(),
                          msg.getMsgStatusString());
        }

        switch (msgType)
        {
            case MAMA_MSG_TYPE_DELETE:
            case MAMA_MSG_TYPE_EXPIRE:
                if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
                {
                    const char *contentSymbol = "N/A";
                    msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contentSymbol);

                    mama_forceLog (MAMA_LOG_LEVEL_FINE,
                                   "MamdaSubscription (%s.%s(%s)) "
                                   "ignoring msg. type: %s\n",
                                   mSource->getPublisherSourceName(),
                                   mSymbol.c_str (),
                                   contentSymbol,
                                   msg.getMsgTypeName());
                }
                return;
        }

        switch (msgStatus)
        {
            case MAMA_MSG_STATUS_BAD_SYMBOL:
            case MAMA_MSG_STATUS_EXPIRED:
            case MAMA_MSG_STATUS_TIMEOUT:
                if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
                {
                    const char *contentSymbol = "N/A";
                    msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contentSymbol);

                    mama_forceLog (MAMA_LOG_LEVEL_FINE,
                                   "MamdaSubscription (%s.%s(%s)) ignoring msg. status: %s\n",
                                   mSource->getPublisherSourceName(),
                                   mSymbol.c_str (),
                                   contentSymbol,
                                   msg.getMsgStatusString());
                }
                return;
        }

        // In order to prevent crashes when a MamdaSubscription is destroyed or deleted
        // it is necessary to maintain a flag that lets us know the subscription is still
        // valid
        // The local variable will not be destroyed until the method finishes so this
        // is set to false if the callback destroys the subscription.
        bool mValid = true;
        implValid   = &mValid;
        
        unsigned long size = mMsgListeners.size();

        for (unsigned long i = 0; i < size; ++i)
        {
            MamdaMsgListener* listener = mMsgListeners[i];

            listener->onMsg (&mSubscription, 
                             msg, 
                             msgType);

            if (!mValid)
            {
                return;
            }
        }

        implValid = NULL;
    }

    void MamdaSubscription::MamdaSubscriptionImpl::onCreate (
        MamaSubscription* subscription)
    {
        mSource = (MamaSource*) subscription->getSource();
    }

    void MamdaSubscription::MamdaSubscriptionImpl::onError (
        MamaSubscription*  subscription,
        const MamaStatus&  status,
        const char*        subject)
    {
        MamdaErrorSeverity  severity = MAMDA_SEVERITY_OK;
        MamdaErrorCode      code     = MAMDA_ERROR_NO_ERROR;
        const char*         errStr   = "unknown";
            
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaSubscription (%s.%s) onError(): %s\n",
                           mSource->getPublisherSourceName(),
                           mSymbol.c_str (),
                           status.toString ());
        }

        switch (status.getStatus())
        {
            case MAMA_STATUS_BAD_SYMBOL:
                severity = MAMDA_SEVERITY_HIGH;
                code     = MAMDA_ERROR_BAD_SYMBOL;
                errStr   = "bad symbol";
                break;
            case MAMA_STATUS_TIMEOUT:
                severity = MAMDA_SEVERITY_HIGH;
                code     = MAMDA_ERROR_TIME_OUT;
                errStr   = "timeout";
                break;
            case MAMA_STATUS_NOT_ENTITLED:
                severity = MAMDA_SEVERITY_HIGH;
                code     = MAMDA_ERROR_ENTITLEMENT;
                errStr   = "entitlement";
                break;
            case MAMA_STATUS_NOT_FOUND:
                severity = MAMDA_SEVERITY_LOW;
                code     = MAMDA_ERROR_NOT_FOUND;
                errStr   = "not found";
                break;
            case MAMA_STATUS_OK:
            case MAMA_STATUS_NOMEM:
            case MAMA_STATUS_PLATFORM:
            case MAMA_STATUS_SYSTEM_ERROR:
            case MAMA_STATUS_INVALID_ARG:
            case MAMA_STATUS_NULL_ARG:
            case MAMA_STATUS_TIMER_FAILURE:
            case MAMA_STATUS_IP_NOT_FOUND:
            case MAMA_STATUS_PROPERTY_TOO_LONG:
            case MAMA_STATUS_MD_NOT_OPENED:
            case MAMA_STATUS_PUB_SUB_NOT_OPENED:
            case MAMA_STATUS_ENTITLEMENTS_NOT_ENABLED:
            case MAMA_STATUS_BAD_TRANSPORT_TYPE:
            case MAMA_STATUS_UNSUPPORTED_IO_TYPE:
            case MAMA_STATUS_TOO_MANY_DISPATCHERS:
            case MAMA_STATUS_NOT_IMPLEMENTED:
            case MAMA_STATUS_WRONG_FIELD_TYPE:
            case MAMA_STATUS_IO_ERROR:
            case MAMA_STATUS_NOT_INSTALLED:
            case MAMA_STATUS_CONFLATE_ERROR:
            case MAMA_STATUS_QUEUE_FULL:
            case MAMA_STATUS_QUEUE_END:
            case MAMA_STATUS_NO_BRIDGE_IMPL:
            case MAMA_STATUS_INVALID_QUEUE:
                break;
        }
        
        /* The error handler wiil destroy any
           failed subscription .
        */
        bool mValid = true;
        implValid   = &mValid;
        
        
        unsigned long size = mErrorListeners.size();

        for (unsigned long i = 0; i < size; ++i)
        {
            MamdaErrorListener* listener = mErrorListeners[i];

            listener->onError (&mSubscription, 
                               severity, 
                               code, 
                               errStr);

            if (!mValid)
            {
                return;
            }
        }

        implValid = NULL;
    }

    void MamdaSubscription::MamdaSubscriptionImpl::onQuality (
        MamaSubscription*  subscription,
        mamaQuality        quality,
        const char*        symbol,
        short              cause,
        const void*        platformInfo)
    {
        if (subscription->checkDebugLevel(MAMA_LOG_LEVEL_FINE))
        {
            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaSubscription onQuality(): %d %s\n",
                           quality,
                           symbol);
        }

        bool mValid = true;
        implValid   = &mValid;
        
        unsigned long size = mQualityListeners.size();

        for (unsigned long i = 0; i < size; ++i)
        {
            MamdaQualityListener* listener = mQualityListeners[i];

            listener->onQuality (&mSubscription, 
                                 quality); 

            if (!mValid)
            {
                return;
            }
        }
        implValid = NULL;
    }

} // namespace
