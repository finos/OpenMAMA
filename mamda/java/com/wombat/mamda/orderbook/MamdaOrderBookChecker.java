/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.orderbook;

import com.wombat.mamda.*;
import com.wombat.mama.*;

import java.util.Random;
import java.util.logging.Logger;
import java.util.logging.Level;

public class MamdaOrderBookChecker 
{
    private static Logger mLogger =
                           Logger.getLogger("com.wombat.mamda.MamdaOrderBookChecker");

    private MamdaOrderBookCheckerHandler mHandler               = null;  
    private MamdaSubscription            mRealTimeSubsc         = null;
    private MamdaOrderBookListener       mRealTimeListener      = null;
    private MamdaSubscription            mSnapShotSubsc         = new MamdaSubscription();
    private MamdaOrderBookListener       mSnapShotListener      = new MamdaOrderBookListener();
    private RealTimeChecker              mRealTimeHandler       = new RealTimeChecker();
    private SnapShotChecker              mSnapShotHandler       = new SnapShotChecker (); 
    private MamaTimer                    mTimer                 = new MamaTimer();
    private double                       mIntervalSecs          = 0;
    private boolean                      mRandomTimerFired      = false;
    private long                         mSuccessCount          = 0;
    private long                         mInconclusiveCount     = 0;
    private long                         mFailureCount          = 0;
    private MamdaOrderBook               mRealTimeBook          = new MamdaOrderBook ();

    
    public MamdaOrderBookChecker(
            MamaTransport                  transport,
            MamaQueue                      queue,
            MamdaOrderBookCheckerHandler   handler,
            final String                   source,
            final String                   symbol,
            double                         intervalSeconds)
    {     
        mRealTimeSubsc                 = new MamdaSubscription(); 
        mRealTimeListener              = new MamdaOrderBookListener(mRealTimeBook); 
        mHandler                       = handler;
        mIntervalSecs                  = intervalSeconds;
        mRandomTimerFired              = false; 
        mRealTimeSubsc.setType         (MamaSubscriptionType.BOOK);
        mRealTimeSubsc.create          (transport,queue,source,symbol,null);
        mRealTimeSubsc.addMsgListener  (mRealTimeListener);
        init();        
    }
   
    public MamdaOrderBookChecker(
             final MamdaSubscription       realTimeSubsc,
             MamdaOrderBookListener        realTimeListener,
             MamdaOrderBookCheckerHandler  handler,
             double                        intervalSeconds)
    {
        mHandler                       = handler;
        mRealTimeSubsc                 = realTimeSubsc;
        mRealTimeListener              = realTimeListener;
        mIntervalSecs                  = intervalSeconds;
        mRandomTimerFired              = false;  
        init();
    }
    
    public long getSuccessCount()
    {
        return mSuccessCount;
    }
    
    public long getInconclusiveCount()
    {
        return mFailureCount;
    }
    
    public long getFailureCount()
    {
        return mInconclusiveCount;
    }
   
    private void init()
    {
        mRealTimeListener.addHandler  (mRealTimeHandler);
        mSnapShotListener.addHandler  (mSnapShotHandler);
        mSnapShotSubsc.addMsgListener (mSnapShotListener);        
                

        if (mIntervalSecs > 0)
        {
            Random generator        = new Random();
            double randomSeconds    = generator.nextInt(65536)  % (int)mIntervalSecs; 
            mTimer.create (mRealTimeSubsc.getQueue(), new TimerCallback(),randomSeconds);
        }        
    }

    public void checkSnapShotNow()
    {
        // Send a snapshot subscription request for the order book
        try
        {
            mLogger.finer                  ("MamdaOrderBookChecker: sending snapshot request...");
            mSnapShotSubsc.setType         (MamaSubscriptionType.BOOK);
            mSnapShotSubsc.setServiceLevel (MamaServiceLevel.SNAPSHOT,(short)0);             
            mSnapShotSubsc.create          (mRealTimeSubsc.getTransport(),
                                            mRealTimeSubsc.getQueue(),
                                            mRealTimeSubsc.getSource(),
                                            mRealTimeSubsc.getSymbol(),
                                            null);
        }
        catch (Exception e)
        {
            System.err.println ("Error in Subscription");
        }
    }
   
    private class TimerCallback implements MamaTimerCallback
    {
        public synchronized void onTimer (MamaTimer timer)
        {        
            if (!mRandomTimerFired)
            {
               mTimer.destroy ();
               mRandomTimerFired = true;
               mTimer.create (mRealTimeSubsc.getQueue(), new TimerCallback(),mIntervalSecs);
            }                      
            checkSnapShotNow();
        }

        public void onDestroy (MamaTimer timer)
        {
        }
    }
    
    private class RealTimeChecker implements MamdaOrderBookHandler
    {
        private MamdaOrderBook mAggDeltaBook = new MamdaOrderBook ();

        public void onBookRecap (MamdaSubscription          subscription,
                                 MamdaOrderBookListener     listener,
                                 MamaMsg                    msg,
                                 MamdaOrderBookComplexDelta delta,
                                 MamdaOrderBookRecap        event,
                                 MamdaOrderBook             book)
        {
            mLogger.fine ( "MamdaOrderBookChecker: received book recap for " + 
                           "symbol " + subscription.getSymbol () +
                           "seq)#: " + subscription.getSeqNum () + ")");

            mAggDeltaBook.copy (book);
            try
            {            
                mAggDeltaBook.assertEqual (mRealTimeBook);

                // Successful result
                mSuccessCount++;
                if (mHandler != null)
                    mHandler.onSuccess (MamdaOrderBookCheckType.APPLY_DELTA,
                                        mRealTimeBook);
            }
            catch (MamdaOrderBookException e)
            {
                // Failure
                mFailureCount++;
                if (mHandler != null)
                    mHandler.onFailure(
                        MamdaOrderBookCheckType.APPLY_DELTA,
                        e.toString(), msg,
                        mRealTimeBook, mAggDeltaBook);

                if (mLogger.isLoggable (Level.FINE))
                {
                    System.out.println ("Failed delta book check: current book:");
                    mRealTimeBook.dump();
                    System.out.println ("Failed delta book check: aggregate delta book:");
                    mAggDeltaBook.dump();
                    System.out.println ("");
                }
            }

        }

       public void onBookDelta (MamdaSubscription         subscription,
                                MamdaOrderBookListener    listener, 
                                MamaMsg                   msg,
                                MamdaOrderBookSimpleDelta delta,
                                MamdaOrderBook            book)
        {
            
            mLogger.fine (
                      "MamdaOrderBookChecker: received book delta for symbol " +
                      subscription.getSymbol () + " (seq#: " +
                      subscription.getSeqNum () + ")");
            mAggDeltaBook.apply (delta);

            try
            {
                mAggDeltaBook.assertEqual (mRealTimeBook);

                // Successful result
                mSuccessCount++;
                if (mHandler != null)
                    mHandler.onSuccess(
                        MamdaOrderBookCheckType.APPLY_DELTA,
                        mRealTimeBook);
            }
            catch (MamdaOrderBookException e)
            {
                // Failure
                mFailureCount++;
                if (mHandler != null)
                    mHandler.onFailure(
                        MamdaOrderBookCheckType.APPLY_DELTA,
                        e.toString(), msg,
                        mRealTimeBook, mAggDeltaBook);
                if (mLogger.isLoggable (Level.FINE))
                {
                    System.out.println ("Failed delta book check: current book:");
                    mRealTimeBook.dump();
                    System.out.println ("Failed delta book check: aggregate delta book:");
                    mAggDeltaBook.dump();
                    System.out.println ("");
                }
            }
        }

        public void onBookComplexDelta (MamdaSubscription          subscription,
                                        MamdaOrderBookListener     listener,
                                        MamaMsg                    msg,
                                        MamdaOrderBookComplexDelta delta,
                                        MamdaOrderBook             book)
        {
            mLogger.fine (
                      "MamdaOrderBookChecker: received complex book delta for symbol" +
                      subscription.getSymbol () + " (seq#: " +
                      subscription.getSeqNum () + ")");
            mAggDeltaBook.apply (delta);

            try
            {
                mAggDeltaBook.assertEqual (mRealTimeBook);

                // Successful result
                mSuccessCount++;
                if (mHandler != null)
                    mHandler.onSuccess(
                        MamdaOrderBookCheckType.APPLY_DELTA,
                        mRealTimeBook);
            }
            catch (MamdaOrderBookException e)
            {
                // Failure
                mFailureCount++;
                if (mHandler != null)
                    mHandler.onFailure(
                        MamdaOrderBookCheckType.APPLY_DELTA,
                        e.toString(), msg,
                        mRealTimeBook, mAggDeltaBook);
                if (mLogger.isLoggable (Level.FINE))
                {
                    System.out.println ("Failed complex delta book check: current book:");
                    mRealTimeBook.dump();
                    System.out.println ("Failed delta book check: aggregate delta book:");
                    mAggDeltaBook.dump();
                    System.out.println ("");
                }
            }
        }

        public void onBookClear (MamdaSubscription      subscription,
                                 MamdaOrderBookListener listener, 
                                 MamaMsg                msg,
                                 MamdaOrderBookClear    event,
                                 MamdaOrderBook         book)
        {
            mLogger.fine (
                      "MamdaOrderBookChecker: received book clear for symbol " +
                      subscription.getSymbol () + " (seq#: " +
                      subscription.getSeqNum () + ")");
            mAggDeltaBook.clear();

            try
            {
                mAggDeltaBook.assertEqual (mRealTimeBook);

                // Successful result
                mSuccessCount++;
                if (mHandler != null)
                    mHandler.onSuccess(
                        MamdaOrderBookCheckType.APPLY_DELTA,
                        mRealTimeBook);
            }
            catch (MamdaOrderBookException e)
            {
                // Failure
                mFailureCount++;
                if (mHandler != null)
                    mHandler.onFailure(
                        MamdaOrderBookCheckType.APPLY_DELTA,
                        e.toString(), msg,
                        mRealTimeBook, mAggDeltaBook);
                if (mLogger.isLoggable (Level.FINE))
                {
                    System.out.println ("Failed delta book check: current book:");
                    mRealTimeBook.dump();
                    System.out.println ("Failed delta book check: aggregate delta book:");
                    mAggDeltaBook.dump();
                    System.out.println ("");
                }
            }
        }
       
        public void onBookGap (MamdaSubscription      subscription,
                               MamdaOrderBookListener listener, 
                               MamaMsg                msg,
                               MamdaOrderBookGap      event, 
                               MamdaOrderBook         fullBook)
        {
            mLogger.fine (
                      "MamdaOrderBookChecker: received book clear for symbol " +
                      subscription.getSymbol () + " (seq#: " +
                      subscription.getSeqNum () + ")");
        }
    }
    
    private class SnapShotChecker implements MamdaOrderBookHandler
    {        
        public void onBookGap   (MamdaSubscription        subscription,
                                 MamdaOrderBookListener   listener,
                                 MamaMsg                  msg,
                                 MamdaOrderBookGap        event,
                                 MamdaOrderBookRecap      fullBook)
        {
            // Will not be called for a snapshot sub
        }

        public void onBookRecap (MamdaSubscription          subscription,
                                 MamdaOrderBookListener     listener,
                                 MamaMsg                    msg,
                                 MamdaOrderBookComplexDelta delta,
                                 MamdaOrderBookRecap        event,
                                 MamdaOrderBook             book)
        {
            long snappedSeqNum  = subscription.getSeqNum ();
            long realTimeSeqNum = mRealTimeListener.getEventSeqNum ();

            mLogger.finer ("MamdaOrderBookChecker: received book recap (snapshot) for symbol "
                            + subscription.getSymbol () +" snappedSeq#: "
                            + snappedSeqNum + " realTimeSeq#: " + realTimeSeqNum);

            if (snappedSeqNum != realTimeSeqNum)
            {
                mInconclusiveCount++;
                if (mHandler != null)
                {
                    String reason =" sequence numbers differ (" + snappedSeqNum + "!=" + realTimeSeqNum;
                    mHandler.onInconclusive (MamdaOrderBookCheckType.SNAPSHOT,reason);
                }
            }
            else
            {
                try
                {
                    book.setSymbol(mRealTimeListener.getOrderBook ().getSymbol());           
                    book.assertEqual (mRealTimeListener.getOrderBook ());

                    mSuccessCount++;
                    if (mHandler != null)
                    {
                        mHandler.onSuccess (MamdaOrderBookCheckType.SNAPSHOT,
                                             mRealTimeListener.getOrderBook ());
                    }
                }
                catch (MamdaOrderBookException e)
                {
                    mFailureCount++;
                    if (mHandler != null)
                    {
                        mHandler.onFailure (MamdaOrderBookCheckType.SNAPSHOT,
                                            e.toString(),
                                            msg,
                                            mRealTimeListener.getOrderBook (),
                                            book);
                    }
                }
            }
            //Deactivate the subscription
            mSnapShotListener.clear();
            mSnapShotSubsc.deactivate();
        }

        public void onBookDelta (MamdaSubscription         subscription,
                                 MamdaOrderBookListener    listener, 
                                 MamaMsg                   msg,
                                 MamdaOrderBookSimpleDelta event,
                                 MamdaOrderBook            book)
        {
            // Will not be called for a snapshot sub
        }

        public void onBookComplexDelta (MamdaSubscription          subscription,
                                        MamdaOrderBookListener     listener,
                                        MamaMsg                    msg,
                                        MamdaOrderBookComplexDelta event,
                                        MamdaOrderBook             book)
        {
            // Will not be called for a snapshot sub
        }

        public void onBookClear (MamdaSubscription      subscription,
                                 MamdaOrderBookListener listener,
                                 MamaMsg                msg,
                                 MamdaOrderBookClear    event,
                                 MamdaOrderBook         book)
        {
            // Will not be called for a snapshot sub
        }

        public void onBookGap (MamdaSubscription      subscription,
                               MamdaOrderBookListener listener, 
                               MamaMsg                msg,
                               MamdaOrderBookGap      event, 
                               MamdaOrderBook         fullBook)
        {
            // Will not be called for a snapshot sub
        }
    }
}
