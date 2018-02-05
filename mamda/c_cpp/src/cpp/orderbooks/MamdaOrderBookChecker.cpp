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

#include <mamda/MamdaOrderBookChecker.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaSubscription.h>
#include <mama/mamacpp.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using std::cout;

namespace Wombat
{


    class RealTimeChecker : public MamdaOrderBookHandler
    {
    public:
        RealTimeChecker (MamdaOrderBookChecker::MamdaOrderBookCheckerImpl&  impl)
            : mImpl(impl) {}
        virtual ~RealTimeChecker() {}

        void onBookRecap (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookComplexDelta*  delta,
            const MamdaOrderBookRecap&         event,
            const MamdaOrderBook&              book);

        void onBookDelta (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookSimpleDelta&   delta,
            const MamdaOrderBook&              book);

        void onBookComplexDelta (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookComplexDelta&  delta,
            const MamdaOrderBook&              book);

        void onBookClear (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookClear&         event,
            const MamdaOrderBook&              book);

        void onBookGap (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookGap&           event,
            const MamdaOrderBook&              book);

    private:
        MamdaOrderBookChecker::MamdaOrderBookCheckerImpl&  mImpl;
    };

    class SnapShotChecker : public MamdaOrderBookHandler
    {
    public:
        SnapShotChecker (MamdaOrderBookChecker::MamdaOrderBookCheckerImpl&  impl)
            : mImpl(impl) {}
        virtual ~SnapShotChecker() {}

        /* Note: we only expect initial values (recaps) in the result. */
        void onBookRecap (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookComplexDelta*  delta,
            const MamdaOrderBookRecap&         event,
            const MamdaOrderBook&              book);

        void onBookDelta (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookSimpleDelta&   delta,
            const MamdaOrderBook&              book) {}

        void onBookComplexDelta (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookComplexDelta&  delta,
            const MamdaOrderBook&              book) {}

        void onBookClear (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookClear&         event,
            const MamdaOrderBook&              book) {}

        void onBookGap (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookGap&           event,
            const MamdaOrderBook&              book) {}

    private:
        MamdaOrderBookChecker::MamdaOrderBookCheckerImpl&  mImpl;
    };

    class MamdaOrderBookChecker::MamdaOrderBookCheckerImpl
                                    : public MamaTimerCallback
    {
    public:
        MamdaOrderBookCheckerImpl (
            MamaQueue*                     queue,
            MamdaOrderBookCheckerHandler*  handler,
            MamaSource*                    source,
            const char*                    symbol,
            mama_f64_t                     intervalSeconds);

        MamdaOrderBookCheckerImpl (
            const MamdaOrderBook&          realTimeBook,
            const MamdaSubscription&       realTimeSubsc,
            MamdaOrderBookListener&        realTimeListener,
            MamdaOrderBookCheckerHandler*  handler,
            mama_f64_t                     intervalSeconds);

        virtual ~MamdaOrderBookCheckerImpl();

        void          init             ();

        virtual void  onTimer          (MamaTimer*  timer);

        void          checkSnapShotNow ();

        MamdaOrderBookCheckerHandler*  mHandler;
        const MamdaSubscription*       mRealTimeSubsc;
        const MamdaOrderBook*          mRealTimeBook;
        MamdaOrderBook                 mRealTimeBookTracer;
        MamdaOrderBook                 mAggDeltaBook;
        MamdaSubscription              mSnapShotSubsc;
        MamdaOrderBookListener*        mRealTimeListener;
        MamdaOrderBookListener         mSnapShotListener;
        RealTimeChecker                mRealTimeHandler;
        SnapShotChecker                mSnapShotHandler;
        MamaTimer                      mTimer;
        mama_f64_t                     mIntervalSecs;
        bool                           mRealTimeObjsAreLocal;
        bool                           mRandomTimerFired;
        mama_u32_t                     mSuccessCount;
        mama_u32_t                     mInconclusiveCount;
        mama_u32_t                     mFailureCount;
        mama_seqnum_t                  mLiveSeqNum;
    };

    MamdaOrderBookChecker::MamdaOrderBookChecker (
        MamaQueue*                     queue,
        MamdaOrderBookCheckerHandler*  handler,
        MamaSource*                    source,
        const char*                    symbol,
        mama_f64_t                     intervalSeconds)
        : mImpl (*new MamdaOrderBookCheckerImpl (queue, handler,
                                                  source, symbol, intervalSeconds))
    {
    }

    MamdaOrderBookChecker::MamdaOrderBookChecker (
        const MamdaOrderBook&          realTimeBook,
        const MamdaSubscription&       realTimeSubsc,
        MamdaOrderBookListener&        realTimeListener,
        MamdaOrderBookCheckerHandler*  handler,
        mama_f64_t                     intervalSeconds)
        : mImpl (*new MamdaOrderBookCheckerImpl (realTimeBook, realTimeSubsc,
                                                  realTimeListener, handler,
                                                  intervalSeconds))
    {
    }

    MamdaOrderBookChecker::~MamdaOrderBookChecker ()
    {
        delete &mImpl;
    }

    void MamdaOrderBookChecker::checkSnapShotNow()
    {
        mImpl.checkSnapShotNow();
    }

    mama_u32_t MamdaOrderBookChecker::getSuccessCount() const
    {
        return mImpl.mSuccessCount;
    }

    mama_u32_t MamdaOrderBookChecker::getInconclusiveCount() const
    {
        return mImpl.mInconclusiveCount;
    }

    mama_u32_t MamdaOrderBookChecker::getFailureCount() const
    {
        return mImpl.mFailureCount;
    }

    MamdaOrderBookChecker::MamdaOrderBookCheckerImpl::MamdaOrderBookCheckerImpl (
        MamaQueue*                     queue,
        MamdaOrderBookCheckerHandler*  handler,
        MamaSource*                    source,
        const char*                    symbol,
        mama_f64_t                     intervalSeconds)
        : mHandler              (handler)
        , mRealTimeHandler      (*this)
        , mSnapShotHandler      (*this)
        , mIntervalSecs         (intervalSeconds)
        , mRealTimeObjsAreLocal (true)
        , mRandomTimerFired     (false)
        , mSuccessCount         (0)
        , mInconclusiveCount    (0)
        , mFailureCount         (0)
        , mLiveSeqNum           (0)
    {
        MamdaOrderBook*    aBook = new MamdaOrderBook;
        MamdaSubscription* aSub  = new MamdaSubscription;
        mRealTimeBook     = aBook;
        mRealTimeSubsc    = aSub;
        mRealTimeListener = new MamdaOrderBookListener (aBook);
        aSub->setType (MAMA_SUBSC_TYPE_BOOK);
        aSub->create (queue, source, symbol);
        aSub->addMsgListener (mRealTimeListener);
        init();
    }

    MamdaOrderBookChecker::MamdaOrderBookCheckerImpl::MamdaOrderBookCheckerImpl (
        const MamdaOrderBook&          realTimeBook,
        const MamdaSubscription&       realTimeSubsc,
        MamdaOrderBookListener&        realTimeListener,
        MamdaOrderBookCheckerHandler*  handler,
        mama_f64_t                     intervalSeconds)
        : mHandler (handler)
        , mRealTimeSubsc    (&realTimeSubsc)
        , mRealTimeBook     (&realTimeBook)
        , mRealTimeListener (&realTimeListener)
        , mRealTimeHandler  (*this)
        , mSnapShotHandler  (*this)
        , mIntervalSecs     (intervalSeconds)
        , mRealTimeObjsAreLocal (false)
        , mRandomTimerFired (false)
        , mSuccessCount     (0)
        , mInconclusiveCount(0)
        , mFailureCount     (0)
        , mLiveSeqNum       (0)
    {
        mAggDeltaBook.copy (realTimeBook);
        init();
    }

    MamdaOrderBookChecker::MamdaOrderBookCheckerImpl::~MamdaOrderBookCheckerImpl()
    {
        if (mRealTimeObjsAreLocal)
        {
            delete mRealTimeSubsc;
            delete mRealTimeListener;
            delete mRealTimeBook;
        }
    }

    void MamdaOrderBookChecker::MamdaOrderBookCheckerImpl::init()
    {
        mRealTimeListener->addHandler (&mRealTimeHandler);
        mSnapShotListener.addHandler  (&mSnapShotHandler);
        mSnapShotSubsc.addMsgListener (&mSnapShotListener);

        if (mIntervalSecs > 0)
        {
            // Create the timer with a random initial interval
            double randomSeconds = 1+(rand()%(int)mIntervalSecs);
            mTimer.create (mRealTimeSubsc->getQueue(), this, randomSeconds);
        }
    }

    void MamdaOrderBookChecker::MamdaOrderBookCheckerImpl::onTimer (
        MamaTimer*  timer)
    {
        if (!mRandomTimerFired)
        {
            mTimer.destroy();
            mRandomTimerFired = true;
            mTimer.create (mRealTimeSubsc->getQueue(), this, mIntervalSecs);
        }
        checkSnapShotNow();
    }

    void MamdaOrderBookChecker::MamdaOrderBookCheckerImpl::checkSnapShotNow()
    {
        if (mSnapShotSubsc.isActive())
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaOrderBookChecker: subscription still active when "
                      "trying to create new one"); 
            return;
        }
     
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaOrderBookChecker: sending snapshot request..."); 

        // Send a snapshot subscription request for the order book
        mSnapShotListener.clear();

        mSnapShotSubsc.destroy();
        mSnapShotSubsc.addMsgListener(&mSnapShotListener);

        mSnapShotSubsc.setType (MAMA_SUBSC_TYPE_BOOK);
        mSnapShotSubsc.setServiceLevel (MAMA_SERVICE_LEVEL_SNAPSHOT);
        mSnapShotSubsc.create (mRealTimeSubsc->getQueue(),
                                mRealTimeSubsc->getSource(),
                                mRealTimeSubsc->getSymbol());
    }

    void RealTimeChecker::onBookRecap (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta*  delta,
        const MamdaOrderBookRecap&         recap,
        const MamdaOrderBook&              book)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaOrderBookChecker: received book recap for symbol %s (seq#: %d)",
                  subscription->getSymbol(), subscription->getSeqNum()); 
        mImpl.mAggDeltaBook.copy (book);

        try
        {
            mImpl.mAggDeltaBook.assertEqual (*mImpl.mRealTimeBook);

            // Successful result
            mImpl.mSuccessCount++;
            if (mImpl.mHandler)
                mImpl.mHandler->onSuccess (
                    MAMDA_BOOK_CHECK_TYPE_APPLY_DELTA);
        }
        catch (MamdaOrderBookException& e)
        {
            // Failure
            mImpl.mFailureCount++;
            if (mImpl.mHandler)
                mImpl.mHandler->onFailure(
                    MAMDA_BOOK_CHECK_TYPE_APPLY_DELTA,
                    e.what(), msg, 
                    *mImpl.mRealTimeBook, mImpl.mAggDeltaBook);
            if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINE)
            {
                std::cout << "Failed delta book check: current book: \n";
                mImpl.mRealTimeBook->dump(cout);
                std::cout << "Failed delta book check: aggregate delta book: \n";
                mImpl.mAggDeltaBook.dump(cout);
                std::cout << "\n";
            }
        }
    }

    void RealTimeChecker::onBookDelta (
        MamdaSubscription*                subscription,
        MamdaOrderBookListener&           listener,
        const MamaMsg*                    msg,
        const MamdaOrderBookSimpleDelta&  delta,
        const MamdaOrderBook&             book)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaOrderBookChecker: received book delta for symbol %s (seq#: %d)",
                  subscription->getSymbol(), subscription->getSeqNum()); 
        mImpl.mAggDeltaBook.apply (delta);

        try
        {
            mImpl.mAggDeltaBook.assertEqual (*mImpl.mRealTimeBook);

            // Successful result
            mImpl.mSuccessCount++;
            if (mImpl.mHandler)
                mImpl.mHandler->onSuccess(
                    MAMDA_BOOK_CHECK_TYPE_APPLY_DELTA);
        }
        catch (MamdaOrderBookException& e)
        {
            // Failure
            mImpl.mFailureCount++;
            if (mImpl.mHandler)
                mImpl.mHandler->onFailure(
                    MAMDA_BOOK_CHECK_TYPE_APPLY_DELTA,
                    e.what(), msg,
                    *mImpl.mRealTimeBook, mImpl.mAggDeltaBook);
            if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINE)
            {
                std::cout << "Failed delta book check: current book: \n";
                mImpl.mRealTimeBook->dump(cout);
                std::cout << "Failed delta book check: aggregate delta book: \n";
                mImpl.mAggDeltaBook.dump(cout);
                std::cout << "\n";
            }
        }
    }

    void RealTimeChecker::onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  delta,
        const MamdaOrderBook&              book)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaOrderBookChecker: received complex book delta for symbol %s (seq#: %d)",
                  subscription->getSymbol(), subscription->getSeqNum()); 
        mImpl.mAggDeltaBook.apply (delta);

        try
        {
            mImpl.mAggDeltaBook.assertEqual (*mImpl.mRealTimeBook);

            // Successful result
            mImpl.mSuccessCount++;
            if (mImpl.mHandler)
                mImpl.mHandler->onSuccess(
                    MAMDA_BOOK_CHECK_TYPE_APPLY_DELTA);
        }
        catch (MamdaOrderBookException& e)
        {
            // Failure
            mImpl.mFailureCount++;
            if (mImpl.mHandler)
                mImpl.mHandler->onFailure(
                    MAMDA_BOOK_CHECK_TYPE_APPLY_DELTA,
                    e.what(), msg,
                    *mImpl.mRealTimeBook, mImpl.mAggDeltaBook);
            if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINE)
            {
                std::cout << "Failed complex delta book check: current book: \n";
                mImpl.mRealTimeBook->dump(cout);
                std::cout << "Failed delta book check: aggregate delta book: \n";
                mImpl.mAggDeltaBook.dump(cout);
                std::cout << "\n";
            }
        }
    }

    void RealTimeChecker::onBookClear (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookClear&  event,
        const MamdaOrderBook&       book)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaOrderBookChecker: received book clear for symbol %s (seq#: %d)",
                  subscription->getSymbol(), subscription->getSeqNum()); 
        mImpl.mAggDeltaBook.clear();

        try
        {
            mImpl.mAggDeltaBook.assertEqual (*mImpl.mRealTimeBook);

            // Successful result
            mImpl.mSuccessCount++;
            if (mImpl.mHandler)
                mImpl.mHandler->onSuccess(
                    MAMDA_BOOK_CHECK_TYPE_APPLY_DELTA);
        }
        catch (MamdaOrderBookException& e)
        {
            // Failure
            mImpl.mFailureCount++;
            if (mImpl.mHandler)
                mImpl.mHandler->onFailure(
                    MAMDA_BOOK_CHECK_TYPE_APPLY_DELTA,
                    e.what(), msg,
                    *mImpl.mRealTimeBook, mImpl.mAggDeltaBook);
            if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINE)
            {
                std::cout << "Failed delta book check: current book: \n";
                mImpl.mRealTimeBook->dump(cout);
                std::cout << "Failed delta book check: aggregate delta book: \n";
                mImpl.mAggDeltaBook.dump(cout);
                std::cout << "\n";
            }
        }
    }

    void RealTimeChecker::onBookGap (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookGap&    delta,
        const MamdaOrderBook&       book)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaOrderBookChecker: received book clear for symbol %s (seq#: %d)",
                  subscription->getSymbol(), subscription->getSeqNum()); 
    }

    void SnapShotChecker::onBookRecap (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta*  delta,
        const MamdaOrderBookRecap&         recap,
        const MamdaOrderBook&              snappedBook)
    {
        mama_seqnum_t  snappedSeqNum  = subscription->getSeqNum();
        mama_seqnum_t  realTimeSeqNum = mImpl.mRealTimeListener->getEventSeqNum();

        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaOrderBookChecker: received book recap (snap shot) for symbol %s (seq#s: %d %d)",
                  subscription->getSymbol(), snappedSeqNum, realTimeSeqNum); 

        if (snappedSeqNum != realTimeSeqNum)
        {
            // Inconclusive result because the sequence numbers were different.
            mImpl.mInconclusiveCount++;
            if (mImpl.mHandler)
            {
                char msg[256];
                snprintf (msg, 256, "sequence numbers differ (%d != %d)",
                          snappedSeqNum, realTimeSeqNum);
                mImpl.mHandler->onInconclusive (
                    MAMDA_BOOK_CHECK_TYPE_SNAPSHOT, msg);
            }
        }
        else
        {
            try
            {
                snappedBook.assertEqual(*mImpl.mRealTimeBook);

                // Successful result
                mImpl.mSuccessCount++;
                if (mImpl.mHandler)
                    mImpl.mHandler->onSuccess(
                        MAMDA_BOOK_CHECK_TYPE_SNAPSHOT);
            }
            catch (MamdaOrderBookException& e)
            {
                // Failure
                mImpl.mFailureCount++;
                if (mImpl.mHandler)
                    mImpl.mHandler->onFailure(
                        MAMDA_BOOK_CHECK_TYPE_SNAPSHOT,
                        e.what(), msg,
                        *mImpl.mRealTimeBook, snappedBook);
                if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINE)
                {
                    std::cout << "Failed book check: current book: \n";
                    mImpl.mRealTimeBook->dump(cout);
                    std::cout << "Failed book check: snapped book: \n";
                    snappedBook.dump(cout);
                    std::cout << "\n";
                }
            }
        }
    }

} // namespace
