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

#include <mamda/MamdaTradeChecker.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <mama/mamacpp.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaDataException.h>
#include <mamda/MamdaTradeHandler.h>

namespace Wombat
{

    class RealTimeTradeChecker : public MamdaTradeHandler
    {
    public:
        RealTimeTradeChecker () {}
        virtual ~RealTimeTradeChecker () {}
        
        void onTradeRecap (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeRecap&          recap);
        
        void onTradeReport (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeReport&         event,
            const MamdaTradeRecap&          recap);
            
        void onTradeGap (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeGap&            event,
            const MamdaTradeRecap&          recap);
        
        void onTradeCancelOrError (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeCancelOrError&  event,
            const MamdaTradeRecap&          recap) {}
            
        void onTradeCorrection (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeCorrection&     event,
            const MamdaTradeRecap&          recap) {}
            
        void onTradeClosing (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeClosing&        event,
            const MamdaTradeRecap&          recap) {}
            
        void onTradeOutOfSequence (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeOutOfSequence&  event,
            const MamdaTradeRecap&          recap) {}
            
        void onTradePossiblyDuplicate (
            MamdaSubscription*                  subscription,
            MamdaTradeListener&                 listener,
            const MamaMsg&                      msg,
            const MamdaTradePossiblyDuplicate&  event,
            const MamdaTradeRecap&              recap) {}
    };

    class SnapShotTradeChecker : public MamdaTradeHandler
    {
    public:
        SnapShotTradeChecker (MamdaTradeChecker::MamdaTradeCheckerImpl& impl)
            : mImpl (impl) {}
        virtual ~SnapShotTradeChecker() {}
        
        void onTradeRecap (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeRecap&          recap);
        
        void onTradeReport (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeReport&         event,
            const MamdaTradeRecap&          recap) {}
            
        void onTradeGap (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeGap&            event,
            const MamdaTradeRecap&          recap) {}
        
        void onTradeCancelOrError (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeCancelOrError&  event,
            const MamdaTradeRecap&          recap) {}
            
        void onTradeCorrection (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeCorrection&     event,
            const MamdaTradeRecap&          recap) {}
            
        void onTradeClosing (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeClosing&        event,
            const MamdaTradeRecap&          recap) {}
            
        void onTradeOutOfSequence (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeOutOfSequence&  event,
            const MamdaTradeRecap&          recap) {}
            
        void onTradePossiblyDuplicate (
            MamdaSubscription*                  subscription,
            MamdaTradeListener&                 listener,
            const MamaMsg&                      msg,
            const MamdaTradePossiblyDuplicate&  event,
            const MamdaTradeRecap&              recap) {}
        
    private:
        MamdaTradeChecker::MamdaTradeCheckerImpl& mImpl;    
    };

    class MamdaTradeChecker::MamdaTradeCheckerImpl : public MamaTimerCallback
    {
    public:
        MamdaTradeCheckerImpl(        
            MamaQueue*            queue,
            MamdaCheckerHandler*  handler,
            MamaSource*           source,
            const char*           symbol,
            mama_f64_t            intervalSeconds);
       
        virtual ~MamdaTradeCheckerImpl();
       
        virtual void onTimer (MamaTimer *timer);

        void checkSnapShotNow ();

        void init();
        
        MamdaCheckerHandler*     mHandler;
        const MamdaSubscription* mRealTimeSubsc;
        MamdaSubscription        mSnapShotSubsc;
        MamdaTradeListener*      mRealTimeListener;
        MamdaTradeListener       mSnapShotListener;
        MamaTimer                mTimer;
        RealTimeTradeChecker     mRealTimeHandler;
        SnapShotTradeChecker     mSnapShotHandler;
        bool                     mRealTimeObjsAreLocal;
        bool                     mRandomTimerFired;
        mama_f64_t               mIntervalSecs;
        mama_u32_t               mSuccessCount;
        mama_u32_t               mInconclusiveCount;
        mama_u32_t               mFailureCount;
    };

    MamdaTradeChecker :: MamdaTradeChecker(
        MamaQueue*            queue,
        MamdaCheckerHandler*  handler,
        MamaSource*           source,
        const char*           symbol,
        mama_f64_t            intervalSeconds)
        : mImpl (*new MamdaTradeCheckerImpl (queue,
                                             handler,
                                             source,
                                             symbol,
                                             intervalSeconds))
    {
    }

    MamdaTradeChecker::~MamdaTradeChecker()
    {
        delete &mImpl;
    }

    void MamdaTradeChecker::checkSnapShotNow()
    {
        mImpl.checkSnapShotNow();
    }

    mama_u32_t MamdaTradeChecker::getSuccessCount() const
    {    
        return mImpl.mSuccessCount;
    }

    mama_u32_t MamdaTradeChecker::getInconclusiveCount() const
    {
        return mImpl.mInconclusiveCount;
    }

    mama_u32_t MamdaTradeChecker::getFailureCount() const
    {    
        return mImpl.mFailureCount;
    }

    MamdaTradeChecker::MamdaTradeCheckerImpl::MamdaTradeCheckerImpl(
        MamaQueue*            queue,
        MamdaCheckerHandler*  handler,
        MamaSource*           source,
        const char*           symbol,
        mama_f64_t            intervalSeconds)
        : mHandler              (handler)
        , mSnapShotHandler      (*this)
        , mRealTimeObjsAreLocal (false)
        , mRandomTimerFired     (false)
        , mIntervalSecs         (intervalSeconds)
        , mSuccessCount         (0)
        , mInconclusiveCount    (0)
        , mFailureCount         (0)
    {
        MamdaSubscription* aSub = new MamdaSubscription;
        mRealTimeSubsc          = aSub;
        mRealTimeListener       = new MamdaTradeListener ();    
        
        aSub->create (queue, source, symbol);
        aSub->addMsgListener (mRealTimeListener);

        init();
    }

    MamdaTradeChecker::MamdaTradeCheckerImpl::~MamdaTradeCheckerImpl()
    {
        if (mRealTimeObjsAreLocal)
        {
            delete mRealTimeSubsc; 
            delete mRealTimeListener;       
        }    
    }

    void MamdaTradeChecker::MamdaTradeCheckerImpl::init ()
    {    
        mRealTimeListener->addHandler (&mRealTimeHandler);
        
        mSnapShotSubsc.addMsgListener (&mSnapShotListener);
        mSnapShotListener.addHandler  (&mSnapShotHandler);
        
        if (mIntervalSecs > 0)
        {
            //Create the timer with a random initial interval
            double randomSeconds = 1 + (rand() % (int)mIntervalSecs);
            mTimer.create (mRealTimeSubsc->getQueue(), 
                           this, 
                           randomSeconds);
        }    
    }

    void MamdaTradeChecker::MamdaTradeCheckerImpl::onTimer (MamaTimer *timer)
    {
        if (!mRandomTimerFired)
        {
            mTimer.destroy ();
            mRandomTimerFired = true;

            mTimer.create (mRealTimeSubsc->getQueue(), 
                           this, 
                           mIntervalSecs);
        }
        checkSnapShotNow();
    }

    void MamdaTradeChecker::MamdaTradeCheckerImpl::checkSnapShotNow()
    {    
        if (mSnapShotSubsc.isActive())
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaQuoteChecker: subscription still active when "
                      "trying to create new one");
            return;
        }
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaQuoteChecker: sending snapshot request...");
        
        mSnapShotSubsc.destroy();
        mSnapShotSubsc.addMsgListener (&mSnapShotListener);

        //Send a snapshot subscription    
        mSnapShotSubsc.setType         (MAMA_SUBSC_TYPE_NORMAL);
        mSnapShotSubsc.setServiceLevel (MAMA_SERVICE_LEVEL_SNAPSHOT);
        mSnapShotSubsc.create          (mRealTimeSubsc->getQueue(),
                                        mRealTimeSubsc->getSource(),
                                        mRealTimeSubsc->getSymbol());
    }

    void RealTimeTradeChecker::onTradeRecap (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeRecap&  recap)
    {    
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaTradeChecker: received trade recap for symbol %s (seq#: %d)",
                  subscription->getSymbol(), 
                  subscription->getSeqNum());        
    }

    void RealTimeTradeChecker::onTradeReport (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeReport& event,
        const MamdaTradeRecap&  recap)
    {    
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaTradeChecker: received trade report for symbol %s (seq#: %d)",
                  subscription->getSymbol(), 
                  subscription->getSeqNum());        
    }

    void RealTimeTradeChecker::onTradeGap (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeGap&            event,
        const MamdaTradeRecap&          recap)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaTradeChecker: received trade gap for symbol %s (seq#: %d)",
                  subscription->getSymbol(), 
                  subscription->getSeqNum());    
    }

    void SnapShotTradeChecker::onTradeRecap (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeRecap&  recap)
    {
        mama_seqnum_t snappedSeqNum  = listener.getEventSeqNum();
        mama_seqnum_t realTimeSeqNum = mImpl.mRealTimeListener->getEventSeqNum();
        
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaTradeChecker: received trade recap for snapshot symbol %s (seq#: %d)",
                  subscription->getSymbol(), 
                  subscription->getSeqNum());
        
        if (snappedSeqNum != realTimeSeqNum)
        {
            // Inconclusive result because the sequence numbers were different.
            mImpl.mInconclusiveCount++;
            if (mImpl.mHandler)
            {
                char msg[256];
                snprintf (msg, 
                          256, 
                          "sequence numbers differ (%d != %d)",
                          snappedSeqNum,
                          realTimeSeqNum);

                mImpl.mHandler->onInconclusive (MAMDA_CHECK_TYPE_SNAPSHOT, msg);
            }
        }
        else
        {
            try
            {
                listener.assertEqual (mImpl.mRealTimeListener);
                mImpl.mSuccessCount++;
                if (mImpl.mHandler)
                {
                    mImpl.mHandler->onSuccess (MAMDA_CHECK_TYPE_SNAPSHOT);
                }            
            }
            catch (MamdaDataException& e)
            {
                mImpl.mFailureCount++;
                if (mImpl.mHandler)
                {
                    mImpl.mHandler->onFailure (MAMDA_CHECK_TYPE_SNAPSHOT,
                                               e.what(), 
                                               msg);
                }            
            }
        }
    }

} //namespace



