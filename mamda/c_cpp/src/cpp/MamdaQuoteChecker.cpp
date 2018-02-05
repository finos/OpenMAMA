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

#include <mamda/MamdaQuoteChecker.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <mama/mamacpp.h>
#include <mamda/MamdaQuoteChecker.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaQuoteHandler.h>
#include <mamda/MamdaDataException.h>
#include <wombat/wincompat.h>

namespace Wombat
{

    class RealTimeQuoteChecker : public MamdaQuoteHandler
    {
    public:
        RealTimeQuoteChecker          () {} 
        virtual ~RealTimeQuoteChecker () {}

        void onQuoteRecap (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteRecap&  recap);
        
        void onQuoteUpdate (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteUpdate& quote,
            const MamdaQuoteRecap&  recap);
            
        void onQuoteGap (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteGap&    event,
            const MamdaQuoteRecap&  recap);
            
        void onQuoteClosing (
            MamdaSubscription*        subscription,
            MamdaQuoteListener&       listener,
            const MamaMsg&            msg,
            const MamdaQuoteClosing&  event,
            const MamdaQuoteRecap&    recap) {}
        
        void onQuoteOutOfSequence (
            MamdaSubscription*              subscription,
            MamdaQuoteListener&             listener,
            const MamaMsg&                  msg,
            const MamdaQuoteOutOfSequence&  event,
            const MamdaQuoteRecap&          recap) {}
            
        void onQuotePossiblyDuplicate (
            MamdaSubscription*                  subscription,
            MamdaQuoteListener&                 listener,
            const MamaMsg&                      msg,
            const MamdaQuotePossiblyDuplicate&  event,
            const MamdaQuoteRecap&              recap) {}
    };

    class SnapShotQuoteChecker : public MamdaQuoteHandler
    {
    public:
        SnapShotQuoteChecker (MamdaQuoteChecker::MamdaQuoteCheckerImpl& impl)
             : mImpl(impl) {}
        virtual ~SnapShotQuoteChecker () {}
        
        void onQuoteRecap (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteRecap&  recap);
        
        void onQuoteUpdate (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteUpdate& quote,
            const MamdaQuoteRecap&  recap) {}
            
        void onQuoteGap (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteGap&    event,
            const MamdaQuoteRecap&  recap){}
            
        void onQuoteClosing (
            MamdaSubscription*        subscription,
            MamdaQuoteListener&       listener,
            const MamaMsg&            msg,
            const MamdaQuoteClosing&  event,
            const MamdaQuoteRecap&    recap) {}
        
        void onQuoteOutOfSequence (
            MamdaSubscription*              subscription,
            MamdaQuoteListener&             listener,
            const MamaMsg&                  msg,
            const MamdaQuoteOutOfSequence&  event,
            const MamdaQuoteRecap&          recap) {}
            
        void onQuotePossiblyDuplicate (
            MamdaSubscription*                  subscription,
            MamdaQuoteListener&                 listener,
            const MamaMsg&                      msg,
            const MamdaQuotePossiblyDuplicate&  event,
            const MamdaQuoteRecap&              recap) {}

    private:
        MamdaQuoteChecker::MamdaQuoteCheckerImpl& mImpl;    
    };

    class MamdaQuoteChecker::MamdaQuoteCheckerImpl : public MamaTimerCallback
    {
    public:
        MamdaQuoteCheckerImpl (
            MamaQueue*            queue,
            MamdaCheckerHandler*  handler,
            MamaSource*           source,
            const char*           symbol,
            mama_f64_t            intervalSeconds);
        virtual ~MamdaQuoteCheckerImpl ();
        
        void init();  
 
        virtual void onTimer (MamaTimer* timer);

        void checkSnapShotNow ();       
        
        MamdaCheckerHandler*     mHandler;
        const MamdaSubscription* mRealTimeSubsc;
        MamdaSubscription        mSnapShotSubsc;
        MamdaQuoteListener*      mRealTimeListener;
        MamdaQuoteListener       mSnapShotListener;
        RealTimeQuoteChecker     mRealTimeHandler;
        SnapShotQuoteChecker     mSnapShotHandler;
        MamaTimer                mTimer;
        bool                     mRealTimeObjsAreLocal;
        bool                     mRandomTimerFired;
        mama_f64_t               mIntervalSecs;
        mama_u32_t               mSuccessCount;
        mama_u32_t               mInconclusiveCount;
        mama_u32_t               mFailureCount;
    };

    MamdaQuoteChecker :: MamdaQuoteChecker (
        MamaQueue*            queue,
        MamdaCheckerHandler*  handler,
        MamaSource*           source,
        const char*           symbol,
        mama_f64_t            intervalSeconds)
        : mImpl (*new MamdaQuoteCheckerImpl (queue,
                                             handler,
                                             source,
                                             symbol,
                                             intervalSeconds))
    {
    }

    MamdaQuoteChecker :: ~MamdaQuoteChecker() 
    {
        delete &mImpl;
    }

    void MamdaQuoteChecker :: checkSnapShotNow ()
    {
        mImpl.checkSnapShotNow ();
    }

    mama_u32_t MamdaQuoteChecker :: getSuccessCount () const
    {
        return mImpl.mSuccessCount;
    }

    mama_u32_t MamdaQuoteChecker :: getInconclusiveCount () const
    {
        return mImpl.mInconclusiveCount;
    }

    mama_u32_t MamdaQuoteChecker :: getFailureCount () const
    {
        return mImpl.mFailureCount;
    }

    MamdaQuoteChecker::MamdaQuoteCheckerImpl::MamdaQuoteCheckerImpl (
        MamaQueue*              queue,
        MamdaCheckerHandler*    handler,
        MamaSource*             source,
        const char*             symbol,
        mama_f64_t              intervalSeconds)
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
        mRealTimeListener       = new MamdaQuoteListener ();

        aSub->setType        (MAMA_SUBSC_TYPE_NORMAL);
        aSub->create         (queue, source, symbol);
        aSub->addMsgListener (mRealTimeListener);

        init();
    }

    MamdaQuoteChecker::MamdaQuoteCheckerImpl::~MamdaQuoteCheckerImpl ()     
    {
        if (mRealTimeObjsAreLocal)
        {
            delete mRealTimeSubsc; 
            delete mRealTimeListener;       
        }
    }

    void MamdaQuoteChecker::MamdaQuoteCheckerImpl::init()
    {
        mRealTimeListener->addHandler (&mRealTimeHandler);
        
        mSnapShotSubsc.addMsgListener (&mSnapShotListener);
        mSnapShotListener.addHandler  (&mSnapShotHandler);
        
        if (mIntervalSecs > 0)
        {
            //Create the timer with a random initial interval
            double randomSeconds = 1 + (rand() % (int)mIntervalSecs);
            mTimer.create (mRealTimeSubsc->getQueue(), this, randomSeconds);
        }
    }

    void MamdaQuoteChecker::MamdaQuoteCheckerImpl::onTimer (
        MamaTimer* timer)
    {
        if (!mRandomTimerFired)
        {
            mTimer.destroy ();
            mRandomTimerFired = true;
            mTimer.create (mRealTimeSubsc->getQueue(), this, mIntervalSecs);
        }
        checkSnapShotNow();
    }

    void MamdaQuoteChecker::MamdaQuoteCheckerImpl::checkSnapShotNow ()
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
        
        mSnapShotSubsc.destroy ();
        mSnapShotSubsc.addMsgListener (&mSnapShotListener);    
        
        //Send a snapshot subscription    
        mSnapShotSubsc.setType         (MAMA_SUBSC_TYPE_NORMAL);
        mSnapShotSubsc.setServiceLevel (MAMA_SERVICE_LEVEL_SNAPSHOT);
        mSnapShotSubsc.create          (mRealTimeSubsc->getQueue(),
                                        mRealTimeSubsc->getSource(),
                                        mRealTimeSubsc->getSymbol());
    }

    void RealTimeQuoteChecker :: onQuoteRecap (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteRecap&  recap)
    {    
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaQuoteChecker: received quote recap for symbol %s (seq#: %d)",
                  subscription->getSymbol(), 
                  subscription->getSeqNum()); 
                  
    }
        
    void RealTimeQuoteChecker :: onQuoteUpdate (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteUpdate& quote,
            const MamdaQuoteRecap&  recap)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaQuoteChecker: received quote update for symbol %s (seq#: %d)",
                  subscription->getSymbol(), 
                  subscription->getSeqNum()); 
    }
                    
    void RealTimeQuoteChecker :: onQuoteGap (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteGap&    event,
            const MamdaQuoteRecap&  recap)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaQuoteChecker: received quote gap for symbol %s (seq#: %d)",
                  subscription->getSymbol(), 
                  subscription->getSeqNum()); 
    }
            
    void SnapShotQuoteChecker :: onQuoteRecap (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteRecap&  recap)
    {
        mama_seqnum_t snappedSeqNum  = listener.getEventSeqNum();
        mama_seqnum_t realTimeSeqNum = mImpl.mRealTimeListener->getEventSeqNum();
        
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaQuoteChecker: received quote recap for snapshot symbol %s (seq#s: %d %d)",
                  subscription->getSymbol(), 
                  snappedSeqNum, 
                  realTimeSeqNum); 

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
                // Assertion
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

