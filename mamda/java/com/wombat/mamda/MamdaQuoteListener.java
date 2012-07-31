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

package com.wombat.mamda;

import com.wombat.mama.*;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.logging.*;
import java.io.*;

/**
 * MamdaQuoteListener is a class that specializes in handling quote
 * updates.  Developers provide their own implementation of the
 * MamdaQuoteHandler interface and will be delivered notifications for
 * quotes and quote closing prices.  An obvious application for this
 * MAMDA class is any kind of quote tick capture application.
 *
 * Note: The MamdaQuoteListener class caches quote-related field
 * values.  Among other reasons, caching of these fields makes it
 * possible to provide complete quote-related callbacks, even when the
 * publisher (e.g., feed handler) is only publishing deltas containing
 * modified fields.
 */

public class MamdaQuoteListener implements MamdaMsgListener,
                                           MamdaQuoteRecap,
                                           MamdaQuoteUpdate,
                                           MamdaQuoteGap,
                                           MamdaQuoteClosing
{
    private static Logger mLogger =
                Logger.getLogger("com.wombat.mamda.MamdaQuoteListener");
    // A single static instance for all Listener instances
    // will be sudfficient as each updater will take a reference to the cache
    // as an argument.
    private static QuoteUpdate  mUpdaters []        = null;
    private static boolean      mUpdatersComplete   = false;
    private static Object       mUpdatersLock       = new Object ();

    private final LinkedList    mHandlers           = new LinkedList();

    protected final MamdaQuoteCache mQuoteCache =
                            new MamdaQuoteCache ();

    private MamaMsgField  tmpField      = new MamaMsgField();
    private boolean       mIgnoreUpdate = false;
   
    // The following fields are used for caching the offical last
    // price and related fields.  These fields can be used by
    // applications for reference and will be passed for recaps.

    private class MamdaQuoteCache
    {
        public MamaString     mSymbol                   = new MamaString();
        public MamaString     mIssueSymbol              = new MamaString();
        public MamaString     mPartId                   = new MamaString();
        public MamaDateTime   mSrcTime                  = new MamaDateTime ();
        public MamaDateTime   mActTime                  = new MamaDateTime ();
        public MamaDateTime   mLineTime                 = new MamaDateTime ();
        public MamaDateTime   mSendTime                 = new MamaDateTime ();
        public MamaString     mPubId                    = new MamaString();
        public MamaPrice      mBidPrice                 = new MamaPrice();
        public MamaDouble     mBidSize                  = new MamaDouble();
        public MamaDouble     mBidDepth                 = new MamaDouble();
        public MamaString     mBidPartId                = new MamaString();
        public MamaPrice      mBidClosePrice            = new MamaPrice();
        public MamaDateTime   mBidCloseDate             = new MamaDateTime ();
        public MamaPrice      mBidPrevClosePrice        = new MamaPrice();
        public MamaDateTime   mBidPrevCloseDate         = new MamaDateTime ();
        public MamaPrice      mBidHigh                  = new MamaPrice();
        public MamaPrice      mBidLow                   = new MamaPrice();
        public MamaPrice      mAskPrice                 = new MamaPrice();
        public MamaDouble     mAskSize                  = new MamaDouble();
        public MamaDouble     mAskDepth                 = new MamaDouble();
        public MamaString     mAskPartId                = new MamaString();
        public MamaPrice      mAskClosePrice            = new MamaPrice();
        public MamaDateTime   mAskCloseDate             = new MamaDateTime ();
        public MamaPrice      mAskPrevClosePrice        = new MamaPrice();
        public MamaDateTime   mAskPrevCloseDate         = new MamaDateTime ();
        public MamaPrice      mAskHigh                  = new MamaPrice();
        public MamaPrice      mAskLow                   = new MamaPrice();
        public MamaPrice      mMidPrice                 = new MamaPrice();
        public MamaLong       mEventSeqNum              = new MamaLong();
        public MamaDateTime   mEventDate                = new MamaDateTime ();
        public MamaDateTime   mEventTime                = new MamaDateTime ();
        public MamaString     mQuoteQualStr             = new MamaString();
        public MamaString     mQuoteQualNative          = new MamaString();
        public MamaChar       mShortSaleBidTick         = new MamaChar();    
        public MamaChar       mShortSaleCircuitBreaker  = new MamaChar();
        private MamaDateTime  mAskTime                  = new MamaDateTime ();
        private MamaDateTime  mBidTime                  = new MamaDateTime ();
        private MamaString    mAskIndicator             = new MamaString();
        private MamaString    mBidIndicator             = new MamaString();
        private MamaLong      mAskUpdateCount           = new MamaLong();
        private MamaLong      mBidUpdateCount           = new MamaLong();
        private MamaDouble    mAskYield                 = new MamaDouble();
        private MamaDouble    mBidYield                 = new MamaDouble(); 
        private MamaString    mBidSizesList             = new MamaString();
        private MamaString    mAskSizesList             = new MamaString();

        public MamaLong       mTmpQuoteCount  = new MamaLong();
        public long           mQuoteCount     = 0; 
        public boolean        mGotBidPrice    = false;
        public boolean        mGotAskPrice    = false;
        public boolean        mGotBidSize     = false;
        public boolean        mGotAskSize     = false;
        public boolean        mGotBidDepth    = false;
        public boolean        mGotAskDepth    = false;
        public boolean        mGotPartId      = false;
        public boolean        mGotBidPartId   = false;
        public boolean        mGotAskPartId   = false;
        public boolean        mGotQuoteCount  = false;
        public boolean        mGotIssueSymbol = false;
        public MamaLong       mConflateCount  = new MamaLong();
        
        // temp fields
        private MamaDouble tmpDouble = new MamaDouble();
        private MamaPrice  tmpPrice  = new MamaPrice();
        private MamaString tmpString = new MamaString();


        /*      FieldState Accessors        */
        
        public MamdaFieldState   mSymbolFieldState                  = new MamdaFieldState();
        public MamdaFieldState   mIssueSymbolFieldState             = new MamdaFieldState();
        public MamdaFieldState   mPartIdFieldState                  = new MamdaFieldState();
        public MamdaFieldState   mSrcTimeFieldState                 = new MamdaFieldState();
        public MamdaFieldState   mActTimeFieldState                 = new MamdaFieldState();
        public MamdaFieldState   mLineTimeFieldState                = new MamdaFieldState();
        public MamdaFieldState   mSendTimeFieldState                = new MamdaFieldState();
        public MamdaFieldState   mPubIdFieldState                   = new MamdaFieldState();
        public MamdaFieldState   mBidPriceFieldState                = new MamdaFieldState();
        public MamdaFieldState   mBidSizeFieldState                 = new MamdaFieldState();
        public MamdaFieldState   mBidDepthFieldState                = new MamdaFieldState();
        public MamdaFieldState   mBidPartIdFieldState               = new MamdaFieldState();
        public MamdaFieldState   mBidClosePriceFieldState           = new MamdaFieldState();
        public MamdaFieldState   mBidCloseDateFieldState            = new MamdaFieldState();
        public MamdaFieldState   mBidPrevClosePriceFieldState       = new MamdaFieldState();
        public MamdaFieldState   mBidPrevCloseDateFieldState        = new MamdaFieldState();
        public MamdaFieldState   mBidHighFieldState                 = new MamdaFieldState();
        public MamdaFieldState   mBidLowFieldState                  = new MamdaFieldState();
        public MamdaFieldState   mAskPriceFieldState                = new MamdaFieldState();
        public MamdaFieldState   mAskSizeFieldState                 = new MamdaFieldState();
        public MamdaFieldState   mAskDepthFieldState                = new MamdaFieldState();
        public MamdaFieldState   mAskPartIdFieldState               = new MamdaFieldState();
        public MamdaFieldState   mAskClosePriceFieldState           = new MamdaFieldState();
        public MamdaFieldState   mAskCloseDateFieldState            = new MamdaFieldState();
        public MamdaFieldState   mAskPrevClosePriceFieldState       = new MamdaFieldState();
        public MamdaFieldState   mAskPrevCloseDateFieldState        = new MamdaFieldState();
        public MamdaFieldState   mAskHighFieldState                 = new MamdaFieldState();
        public MamdaFieldState   mAskLowFieldState                  = new MamdaFieldState();
        public MamdaFieldState   mMidPriceFieldState                = new MamdaFieldState();
        public MamdaFieldState   mEventSeqNumFieldState             = new MamdaFieldState();
        public MamdaFieldState   mEventDateFieldState               = new MamdaFieldState();
        public MamdaFieldState   mEventTimeFieldState               = new MamdaFieldState();
        public MamdaFieldState   mQuoteQualStrFieldState            = new MamdaFieldState();
        public MamdaFieldState   mQuoteQualNativeFieldState         = new MamdaFieldState();
        public MamdaFieldState   mShortSaleBidTickFieldState        = new MamdaFieldState(); 
        public MamdaFieldState   mShortSaleCircuitBreakerFieldState = new MamdaFieldState();
        private MamdaFieldState  mAskTimeFieldState                 = new MamdaFieldState();
        private MamdaFieldState  mBidTimeFieldState                 = new MamdaFieldState();
        private MamdaFieldState  mAskIndicatorFieldState            = new MamdaFieldState();
        private MamdaFieldState  mBidIndicatorFieldState            = new MamdaFieldState();
        private MamdaFieldState  mAskUpdateCountFieldState          = new MamdaFieldState();
        private MamdaFieldState  mBidUpdateCountFieldState          = new MamdaFieldState();
        private MamdaFieldState  mAskYieldFieldState                = new MamdaFieldState();
        private MamdaFieldState  mBidYieldFieldState                = new MamdaFieldState(); 
        public MamdaFieldState   mTmpQuoteCountFieldState           = new MamdaFieldState();
        public MamdaFieldState   mQuoteCountFieldState              = new MamdaFieldState();
        public MamdaFieldState   mConflateCountFieldState           = new MamdaFieldState();
        public MamdaFieldState   mBidSizesListFieldState            = new MamdaFieldState();
        public MamdaFieldState   mAskSizesListFieldState            = new MamdaFieldState();
    }

    // Additional fields for gaps:
    private long                 mGapBegin = 0;
    private long                 mGapEnd   = 0;
    public MamdaFieldState       mGapBeginFieldState     = new MamdaFieldState();
    public MamdaFieldState       mGapEndFieldState       = new MamdaFieldState();

    // true if an generic update is infact a quote
    private boolean              mLastGenericMsgWasQuote = false;

    /**
     * Create a specialized quote listener.  This listener handles
     * quote updates, quote recaps, and quote gap notifications.
     */
    public MamdaQuoteListener ()
    {
        //Initialize the cache
        clearCache (mQuoteCache);
    }

    public void clearCache (MamdaQuoteCache cache)
    {
        cache.mSymbol.setValue          (null);          
        cache.mIssueSymbol.setValue     (null);     
        cache.mPartId.setValue          (null);          
        cache.mSrcTime.clear            ();               
        cache.mActTime.clear            ();               
        cache.mLineTime.clear           ();              
        cache.mSendTime.clear           ();              
        cache.mPubId.setValue           (null);           
        cache.mBidPrice.clear           ();              
        cache.mBidSize.setValue         (0);            
        cache.mBidDepth.setValue        (0);           
        cache.mBidPartId.setValue       (null);       
        cache.mBidClosePrice.clear      ();         
        cache.mBidCloseDate.clear       ();          
        cache.mBidPrevClosePrice.clear  ();     
        cache.mBidPrevCloseDate.clear   ();      
        cache.mBidHigh.clear            ();               
        cache.mBidLow.clear             ();                
        cache.mAskPrice.clear           ();              
        cache.mAskSize.setValue         (0);            
        cache.mAskDepth.setValue        (0);           
        cache.mAskPartId.setValue       (null);       
        cache.mAskClosePrice.clear      ();         
        cache.mAskCloseDate.clear       ();          
        cache.mAskPrevClosePrice.clear  ();     
        cache.mAskPrevCloseDate.clear   ();      
        cache.mAskHigh.clear            ();               
        cache.mAskLow.clear             ();                
        cache.mMidPrice.clear           ();              
        cache.mEventSeqNum.setValue     (0);        
        cache.mEventDate.clear          ();             
        cache.mEventTime.clear          ();             
        cache.mQuoteQualStr.setValue    (null);    
        cache.mQuoteQualNative.setValue (null); 
        cache.mShortSaleBidTick.setValue('Z');
        cache.mShortSaleCircuitBreaker.setValue(' ');
        cache.mAskTime.clear            ();               
        cache.mBidTime.clear            ();               
        cache.mAskIndicator.setValue    (null);    
        cache.mBidIndicator.setValue    (null);    
        cache.mAskUpdateCount.setValue  (0);     
        cache.mBidUpdateCount.setValue  (0);     
        cache.mAskYield.setValue        (0);           
        cache.mBidYield.setValue        (0);           
        cache.mAskSizesList.setValue    (null);           
        cache.mBidSizesList.setValue    (null);           

        cache.mTmpQuoteCount.setValue   (0);      
        cache.mQuoteCount               = 0;              
        cache.mGotBidPrice              = false;
        cache.mGotAskPrice              = false;
        cache.mGotBidSize               = false;
        cache.mGotAskSize               = false;
        cache.mGotBidDepth              = false;
        cache.mGotAskDepth              = false;
        cache.mGotPartId                = false;
        cache.mGotBidPartId             = false;
        cache.mGotAskPartId             = false;
        cache.mGotQuoteCount            = false;
        cache.mConflateCount.setValue   (0);

        cache.mSymbolFieldState.setState            (MamdaFieldState.NOT_INITIALISED);
        cache.mIssueSymbolFieldState.setState       (MamdaFieldState.NOT_INITIALISED);
        cache.mPartIdFieldState.setState            (MamdaFieldState.NOT_INITIALISED);
        cache.mSrcTimeFieldState.setState           (MamdaFieldState.NOT_INITIALISED);
        cache.mActTimeFieldState.setState           (MamdaFieldState.NOT_INITIALISED);
        cache.mLineTimeFieldState.setState          (MamdaFieldState.NOT_INITIALISED);
        cache.mSendTimeFieldState.setState          (MamdaFieldState.NOT_INITIALISED);
        cache.mPubIdFieldState.setState             (MamdaFieldState.NOT_INITIALISED);
        cache.mBidPriceFieldState.setState          (MamdaFieldState.NOT_INITIALISED);
        cache.mBidSizeFieldState.setState           (MamdaFieldState.NOT_INITIALISED);
        cache.mBidDepthFieldState.setState          (MamdaFieldState.NOT_INITIALISED);
        cache.mBidPartIdFieldState.setState         (MamdaFieldState.NOT_INITIALISED);
        cache.mBidClosePriceFieldState.setState     (MamdaFieldState.NOT_INITIALISED);
        cache.mBidCloseDateFieldState.setState      (MamdaFieldState.NOT_INITIALISED);
        cache.mBidPrevClosePriceFieldState.setState (MamdaFieldState.NOT_INITIALISED);
        cache.mBidPrevCloseDateFieldState.setState  (MamdaFieldState.NOT_INITIALISED);
        cache.mBidHighFieldState.setState           (MamdaFieldState.NOT_INITIALISED);
        cache.mBidLowFieldState.setState            (MamdaFieldState.NOT_INITIALISED);
        cache.mAskPriceFieldState.setState          (MamdaFieldState.NOT_INITIALISED);
        cache.mAskSizeFieldState.setState           (MamdaFieldState.NOT_INITIALISED);
        cache.mAskDepthFieldState.setState          (MamdaFieldState.NOT_INITIALISED);
        cache.mAskPartIdFieldState.setState         (MamdaFieldState.NOT_INITIALISED);
        cache.mAskClosePriceFieldState.setState     (MamdaFieldState.NOT_INITIALISED);
        cache.mAskCloseDateFieldState.setState      (MamdaFieldState.NOT_INITIALISED);
        cache.mAskPrevClosePriceFieldState.setState (MamdaFieldState.NOT_INITIALISED);
        cache.mAskPrevCloseDateFieldState.setState  (MamdaFieldState.NOT_INITIALISED);
        cache.mAskHighFieldState.setState           (MamdaFieldState.NOT_INITIALISED);
        cache.mAskLowFieldState.setState            (MamdaFieldState.NOT_INITIALISED);
        cache.mMidPriceFieldState.setState          (MamdaFieldState.NOT_INITIALISED);
        cache.mEventSeqNumFieldState.setState       (MamdaFieldState.NOT_INITIALISED);
        cache.mEventDateFieldState.setState         (MamdaFieldState.NOT_INITIALISED);
        cache.mEventTimeFieldState.setState         (MamdaFieldState.NOT_INITIALISED);
        cache.mQuoteQualStrFieldState.setState      (MamdaFieldState.NOT_INITIALISED);
        cache.mQuoteQualNativeFieldState.setState   (MamdaFieldState.NOT_INITIALISED);
        cache.mShortSaleBidTickFieldState.setState  (MamdaFieldState.NOT_INITIALISED);
        cache.mShortSaleCircuitBreakerFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mAskTimeFieldState.setState           (MamdaFieldState.NOT_INITIALISED);
        cache.mBidTimeFieldState.setState           (MamdaFieldState.NOT_INITIALISED);
        cache.mAskIndicatorFieldState.setState      (MamdaFieldState.NOT_INITIALISED);
        cache.mBidIndicatorFieldState.setState      (MamdaFieldState.NOT_INITIALISED);
        cache.mAskUpdateCountFieldState.setState    (MamdaFieldState.NOT_INITIALISED);
        cache.mBidUpdateCountFieldState.setState    (MamdaFieldState.NOT_INITIALISED);
        cache.mAskYieldFieldState.setState          (MamdaFieldState.NOT_INITIALISED);
        cache.mBidYieldFieldState.setState          (MamdaFieldState.NOT_INITIALISED);

        cache.mTmpQuoteCountFieldState.setState     (MamdaFieldState.NOT_INITIALISED);
        cache.mQuoteCountFieldState.setState        (MamdaFieldState.NOT_INITIALISED);    
        cache.mConflateCountFieldState.setState     (MamdaFieldState.NOT_INITIALISED);
        cache.mAskSizesListFieldState.setState      (MamdaFieldState.NOT_INITIALISED);    
        cache.mBidSizesListFieldState.setState      (MamdaFieldState.NOT_INITIALISED);

    }

    /**
     * Add a specialized quote handler.  Currently, only one
     * handler can (and must) be registered.  
     */
    public void addHandler (MamdaQuoteHandler  handler)
    {
        mHandlers.addLast(handler);
    }

    public String getSymbol()
    {
        return mQuoteCache.mSymbol.getValue();
    }

    public String getPartId()
    {
        return mQuoteCache.mPartId.getValue();
    }

    public MamaDateTime getSrcTime()
    {
        return mQuoteCache.mSrcTime;
    }

    public MamaDateTime getActivityTime()
    {
        return mQuoteCache.mActTime;
    }

    public MamaDateTime getLineTime()
    {
        return mQuoteCache.mLineTime;
    }

    public MamaDateTime getSendTime()
    {
        return mQuoteCache.mSendTime;
    }

    public String getPubId()
    {
        return mQuoteCache.mPubId.getValue();
    }

    public MamaPrice getBidPrice()
    {
        return mQuoteCache.mBidPrice;
    }

    public double getBidSize()
    {
        return mQuoteCache.mBidSize.getValue();
    }

    public String getBidPartId()
    {
        return mQuoteCache.mBidPartId.getValue();
    }

    public MamaPrice getBidClosePrice()
    {
        return mQuoteCache.mBidClosePrice;
    }

    public MamaDateTime getBidCloseDate()
    {
        return mQuoteCache.mBidCloseDate;
    }

    public MamaPrice getBidPrevClosePrice()
    {
        return mQuoteCache.mBidPrevClosePrice;
    }

    public MamaDateTime getBidPrevCloseDate()
    {
        return mQuoteCache.mBidPrevCloseDate;
    }

    public MamaPrice getBidHigh()
    {
        return mQuoteCache.mBidHigh;
    }

    public MamaPrice getBidLow()
    {
        return mQuoteCache.mBidLow;
    }

    public MamaPrice getAskPrice()
    {
        return mQuoteCache.mAskPrice;
    }

    public double getAskSize()
    {
        return mQuoteCache.mAskSize.getValue();
    }

    public String getAskPartId()
    {
        return mQuoteCache.mAskPartId.getValue();
    }
    public double getAskDepth()
    {
      return mQuoteCache.mAskDepth.getValue();
    }

    public double getBidDepth()
    {
      return mQuoteCache.mBidDepth.getValue();
    }

    public MamaPrice getAskClosePrice()
    {
        return mQuoteCache.mAskClosePrice;
    }

    public MamaDateTime getAskCloseDate()
    {
        return mQuoteCache.mAskCloseDate;
    }

    public MamaPrice getAskPrevClosePrice()
    {
        return mQuoteCache.mAskPrevClosePrice;
    }

    public MamaDateTime getAskPrevCloseDate()
    {
        return mQuoteCache.mAskPrevCloseDate;
    }

    public MamaPrice getAskHigh()
    {
        return mQuoteCache.mAskHigh;
    }

    public MamaPrice getAskLow()
    {
        return mQuoteCache.mAskLow;
    }

    public MamaPrice getQuoteMidPrice()
    {
        if ((mQuoteCache.mBidPrice.getValue() > 0.0) && 
            (mQuoteCache.mAskPrice.getValue() > 0.0))
        {
            mQuoteCache.mMidPrice.setValue ((
                mQuoteCache.mAskPrice.getValue() + 
            mQuoteCache.mBidPrice.getValue()) / 2.0);
        }
        else
        {
            mQuoteCache.mMidPrice.clear();
        }
        return mQuoteCache.mMidPrice;
    }

    public String getQuoteQual()
    {
        return mQuoteCache.mQuoteQualStr.getValue();
    }

    public String getQuoteQualNative()
    {
        return mQuoteCache.mQuoteQualNative.getValue();
    }

    public char getShortSaleBidTick ()
    {
        return mQuoteCache.mShortSaleBidTick.getValue();
    }

    public char getShortSaleCircuitBreaker()
    {
        return mQuoteCache.mShortSaleCircuitBreaker.getValue();
    }

    public MamaDateTime getAskTime()
    {
        return mQuoteCache.mAskTime;
    }
    
    public MamaDateTime getBidTime()
    {
        return mQuoteCache.mBidTime;
    }
    
    public String getAskIndicator()
    {
        return mQuoteCache.mAskIndicator.getValue();
    }
        
    public String getBidIndicator()
    {
        return mQuoteCache.mBidIndicator.getValue();
    }

    public long getAskUpdateCount()
    {
        return mQuoteCache.mAskUpdateCount.getValue();
    }

    public long getBidUpdateCount()
    {
        return mQuoteCache.mBidUpdateCount.getValue();
    }

    public double getAskYield()
    {
        return mQuoteCache.mAskYield.getValue();
    }

    public double getBidYield()
    {
        return mQuoteCache.mBidYield.getValue();
    }

    public long getQuoteCount()
    {
        return mQuoteCache.mQuoteCount;
    }

    public long getEventSeqNum()
    {
        return mQuoteCache.mEventSeqNum.getValue();
    }

    public MamaDateTime getEventDate()
    {
        return mQuoteCache.mEventDate;
    }

    public MamaDateTime getEventTime()
    {
        return mQuoteCache.mEventTime;
    }

        
    public String getAskSizesList()
    {
      return mQuoteCache.mBidSizesList.getValue();
    }

    public String getBidSizesList()
    {
      return mQuoteCache.mAskSizesList.getValue();
    }
    
    public MamaDateTime getEventDateTime()
    {
        MamaDateTime mEventDateTime = new MamaDateTime (mQuoteCache.mEventTime, mQuoteCache.mEventDate);
        
        return mEventDateTime;
    }
    
    public MamaDateTime getQuoteDate()
    {
        MamaDateTime mEventDateTime = new MamaDateTime (mQuoteCache.mEventTime, mQuoteCache.mEventDate);
        
        return mEventDateTime;
    }
    
    public long getBeginGapSeqNum()
    {
        return mGapBegin;
    }

    public long getEndGapSeqNum()
    {
        return mGapEnd;
    }


    /*      FieldState Accessors        */
    
    public short getSymbolFieldState()
    {
        return mQuoteCache.mSymbolFieldState.getState();
    }

    public short getPartIdFieldState()
    {
        return mQuoteCache.mPartIdFieldState.getState();
    }

    public short getSrcTimeFieldState()
    {
        return mQuoteCache.mSrcTimeFieldState.getState();
    }

    public short getActivityTimeFieldState()
    {
        return mQuoteCache.mActTimeFieldState.getState();
    }

    public short getLineTimeFieldState()
    {
        return mQuoteCache.mLineTimeFieldState.getState();
    }

    public short getSendTimeFieldState()
    {
        return mQuoteCache.mSendTimeFieldState.getState();
    }

    public short getPubIdFieldState()
    {
        return mQuoteCache.mPubIdFieldState.getState();
    }

    public short getBidPriceFieldState()
    {
        return mQuoteCache.mBidPriceFieldState.getState();
    }

    public short getBidSizeFieldState()
    {
        return mQuoteCache.mBidSizeFieldState.getState();
    }

    public short getBidPartIdFieldState()
    {
        return mQuoteCache.mBidPartIdFieldState.getState();
    }

    public short getBidClosePriceFieldState()
    {
        return mQuoteCache.mBidClosePriceFieldState.getState();
    }

    public short getBidCloseDateFieldState()
    {
        return mQuoteCache.mBidCloseDateFieldState.getState();
    }

    public short getBidPrevClosePriceFieldState()
    {
        return mQuoteCache.mBidPrevClosePriceFieldState.getState();
    }

    public short getBidPrevCloseDateFieldState()
    {
        return mQuoteCache.mBidPrevCloseDateFieldState.getState();
    }

    public short getBidHighFieldState()
    {
        return mQuoteCache.mBidHighFieldState.getState();
    }

    public short getBidLowFieldState()
    {
        return mQuoteCache.mBidLowFieldState.getState();
    }

    public short getAskPriceFieldState()
    {
        return mQuoteCache.mAskPriceFieldState.getState();
    }

    public short getAskSizeFieldState()
    {
        return mQuoteCache.mAskSizeFieldState.getState();
    }

    public short getAskPartIdFieldState()
    {
        return mQuoteCache.mAskPartIdFieldState.getState();
    }

    public short getAskClosePriceFieldState()
    {
        return mQuoteCache.mAskClosePriceFieldState.getState();
    }

    public short getAskCloseDateFieldState()
    {
        return mQuoteCache.mAskCloseDateFieldState.getState();
    }

    public short getAskPrevClosePriceFieldState()
    {
        return mQuoteCache.mAskPrevClosePriceFieldState.getState();
    }

    public short getAskPrevCloseDateFieldState()
    {
        return mQuoteCache.mAskPrevCloseDateFieldState.getState();
    }

    public short getAskHighFieldState()
    {
        return mQuoteCache.mAskHighFieldState.getState();
    }

    public short getAskLowFieldState()
    {
        return mQuoteCache.mAskLowFieldState.getState();
    }

    public short getAskDepthFieldState()
    {
        return mQuoteCache.mAskDepthFieldState.getState();
    }

    public short getBidDepthFieldState()
    {
        return mQuoteCache.mBidDepthFieldState.getState();
    }

    public short getQuoteMidPriceFieldState()
    {
        return mQuoteCache.mMidPriceFieldState.getState();
    }

    public short getQuoteQualFieldState()
    {
        return mQuoteCache.mQuoteQualStrFieldState.getState();
    }

    public short getQuoteQualNativeFieldState()
    {
        return mQuoteCache.mQuoteQualNativeFieldState.getState();
    }

    public short getShortSaleBidTickFieldState()
    {
        return mQuoteCache.mShortSaleBidTickFieldState.getState();
    }

    public short getShortSaleCircuitBreakerFieldState()
    {          
        return mQuoteCache.mShortSaleCircuitBreakerFieldState.getState();
    }        

    public short getAskTimeFieldState()
    {
        return mQuoteCache.mAskTimeFieldState.getState();
    }
    
    public short getBidTimeFieldState()
    {
        return mQuoteCache.mBidTimeFieldState.getState();
    }
    
    public short getAskIndicatorFieldState()
    {
        return mQuoteCache.mAskIndicatorFieldState.getState();
    }
        
    public short getBidIndicatorFieldState()
    {
        return mQuoteCache.mBidIndicatorFieldState.getState();
    }

    public short getAskUpdateCountFieldState()
    {
        return mQuoteCache.mAskUpdateCountFieldState.getState();
    }

    public short getBidUpdateCountFieldState()
    {
        return mQuoteCache.mBidUpdateCountFieldState.getState();
    }

    public short getAskYieldFieldState()
    {
        return mQuoteCache.mAskYieldFieldState.getState();
    }

    public short getBidYieldFieldState()
    {
        return mQuoteCache.mBidYieldFieldState.getState();
    }

    public short getQuoteCountFieldState()
    {
        return mQuoteCache.mQuoteCountFieldState.getState();
    }

    public short getEventSeqNumFieldState()
    {
        return mQuoteCache.mEventSeqNumFieldState.getState();
    }

    public short getEventDateFieldState()
    {
        return mQuoteCache.mEventDateFieldState.getState();
    }

    public short getEventTimeFieldState()
    {
        return mQuoteCache.mEventTimeFieldState.getState();
    }

    public short getAskSizesListFieldState()
    {
        return mQuoteCache.mAskSizesListFieldState.getState();
    }

    public short getBidSizesListFieldState()
    {
        return mQuoteCache.mBidSizesListFieldState.getState();
    }
    
    public short getBeginGapSeqNumFieldState()
    {
        return mGapBeginFieldState.getState();
    }

    public short getEndGapSeqNumFieldState()
    {
        return mGapEndFieldState.getState();
    }
    
    /*    End FieldState Accessors         */


    /**
     * Implementation of MamdaListener interface.
     * NB! For internal use only.
     */
    public void onMsg (MamdaSubscription subscription,
                       MamaMsg           msg,
                       short             msgType)
    {
        // Listeners may be created on multiple threads and we only
        // want a single list of updaters.
        if (!mUpdatersComplete)
        {
            synchronized (mUpdatersLock)
            {
                if (!MamdaQuoteFields.isSet())
                {
                    mLogger.warning ("MamdaQuoteFields::setDictionary() has not been called.");
                    return;
                }
                if (!mUpdatersComplete)
                {
                    createUpdaters ();
                    mUpdatersComplete = true;
                }
            }
        }
        
        // If msg is a quote-related message, invoke the
        // appropriate callback:
     
        switch (msgType)
        {
            case MamaMsgType.TYPE_INITIAL:
            case MamaMsgType.TYPE_RECAP:
            case MamaMsgType.TYPE_PREOPENING:
                handleRecap (subscription, msg);
                break;
            case MamaMsgType.TYPE_QUOTE:
                handleQuote (subscription, msg);
                break;
            case MamaMsgType.TYPE_TRADE:
            case MamaMsgType.TYPE_UPDATE:
                handleUpdate (subscription, msg);
                break;
        }
    }

    private void handleRecap (MamdaSubscription  subscription,
                              MamaMsg            msg)
    {
        updateFieldStates();
        updateQuoteFields (msg);

        checkQuoteCount (subscription, msg, false);

        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaQuoteHandler handler = (MamdaQuoteHandler) i.next();
            handler.onQuoteRecap (subscription, this, msg, this);
        }
    }

    private void handleQuote (MamdaSubscription  subscription,
                              MamaMsg            msg)
    {
        updateFieldStates();
        updateQuoteFields (msg);
        checkQuoteCount (subscription, msg, true);
        
        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }
    
        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaQuoteHandler handler = (MamdaQuoteHandler) i.next();
            handler.onQuoteUpdate (subscription, this, msg, this, this);
        }
    }

    private void handleUpdate (MamdaSubscription  subscription,
                               MamaMsg            msg)
    {
        updateFieldStates();
        updateQuoteFields (msg);
        if (mLastGenericMsgWasQuote)
        {
            checkQuoteCount (subscription, msg, true);

            if (mIgnoreUpdate)
            {
                mIgnoreUpdate = false;
                return;
            }

            Iterator i = mHandlers.iterator();
            while (i.hasNext())
            {
                MamdaQuoteHandler handler = (MamdaQuoteHandler) i.next();
                handler.onQuoteUpdate (subscription, this, msg, this, this);
            }
            mLastGenericMsgWasQuote = false;
        }
    }
    
   private void checkQuoteCount (MamdaSubscription  subscription,
                                 MamaMsg            msg,
                                 boolean            checkForGap)
    {
        mIgnoreUpdate = false;

        // Check number of quotes for gaps
        long quoteCount = mQuoteCache.mTmpQuoteCount.getValue();
        mQuoteCache.mConflateCount.setValue(1);

        if (msg.tryU32 ("wConflateQuoteCount", 23, mQuoteCache.mConflateCount))
            mQuoteCache.mConflateCountFieldState.setState(MamdaFieldState.MODIFIED);

        if (checkForGap && mQuoteCache.mGotQuoteCount)
        {
            if ((mQuoteCache.mQuoteCount> 0) &&
                (quoteCount > (mQuoteCache.mQuoteCount+mQuoteCache.mConflateCount.getValue())))
            {
                mGapBegin = mQuoteCache.mQuoteCount+mQuoteCache.mConflateCount.getValue();
                mGapEnd   = quoteCount-1;
                mGapBeginFieldState.setState (MamdaFieldState.MODIFIED);
                mGapEndFieldState.setState   (MamdaFieldState.MODIFIED);

                mQuoteCache.mQuoteCount=quoteCount;
                mQuoteCache.mQuoteCountFieldState.setState (MamdaFieldState.MODIFIED);

                Iterator i = mHandlers.iterator();
                while (i.hasNext())
                {
                    MamdaQuoteHandler handler = (MamdaQuoteHandler) i.next();
                    handler.onQuoteGap (subscription, this, msg, this, this);
                }
            }
        }
   
        /* Check for duplicates.  Only check if quote count > 0 in case it isn't being sent down. */ 
        if ( mQuoteCache.mGotQuoteCount && 
             quoteCount > 0 &&
             (quoteCount == mQuoteCache.mQuoteCount) )
        {
            mIgnoreUpdate = true;
        }

        mQuoteCache.mQuoteCount = quoteCount;
        mQuoteCache.mQuoteCountFieldState.setState (MamdaFieldState.MODIFIED);
    }

    private static void createUpdaters ()
    {
        int i = 0;
        if (mUpdaters == null)
        {
            mUpdaters = new QuoteUpdate [MamdaQuoteFields.getMaxFid() + 1];
        }

        if (MamdaCommonFields.SYMBOL != null)
            mUpdaters[i++] = new MamdaQuoteSymbol ();
        
        if (MamdaCommonFields.ISSUE_SYMBOL != null)
            mUpdaters[i++] = new MamdaQuoteIssueSymbol ();
        
        if (MamdaCommonFields.PART_ID != null)
            mUpdaters[i++] = new MamdaQuotePartId ();
        
        if (MamdaCommonFields.SRC_TIME != null)
            mUpdaters[i++] = new MamdaQuoteSrcTime ();
        
        if (MamdaCommonFields.ACTIVITY_TIME != null)
            mUpdaters[i++] = new MamdaQuoteActivityTime ();
        
        if (MamdaCommonFields.LINE_TIME != null)
            mUpdaters[i++] = new MamdaQuoteLineTime ();
        
        if (MamdaCommonFields.SEND_TIME != null)
            mUpdaters[i++] = new MamdaQuoteSendTime ();
        
        if (MamdaCommonFields.PUB_ID != null)
            mUpdaters[i++] = new MamdaQuotePubId ();
        
        if (MamdaQuoteFields.BID_PRICE != null)
            mUpdaters[i++] = new MamdaQuoteBidPrice ();
            
        if (MamdaQuoteFields.BID_SIZE != null)
            mUpdaters[i++] = new MamdaQuoteBidSize ();
        
        if (MamdaQuoteFields.BID_PART_ID != null)
            mUpdaters[i++] = new MamdaQuoteBidPartId ();
            
        if (MamdaQuoteFields.BID_DEPTH != null)
            mUpdaters[i++] = new MamdaQuoteBidDepth ();
            
        if (MamdaQuoteFields.BID_CLOSE_PRICE != null)
            mUpdaters[i++] = new MamdaQuoteBidClosePrice ();
            
        if (MamdaQuoteFields.BID_CLOSE_DATE != null)
            mUpdaters[i++] = new MamdaQuoteBidCloseDate ();
            
        if (MamdaQuoteFields.BID_PREV_CLOSE_PRICE != null)
            mUpdaters[i++] = new MamdaQuoteBidPrevClosePrice ();
            
        if (MamdaQuoteFields.BID_PREV_CLOSE_DATE != null)
            mUpdaters[i++] = new MamdaQuoteBidPrevCloseDate ();
            
        if (MamdaQuoteFields.BID_HIGH != null)
            mUpdaters[i++] = new MamdaQuoteBidHigh ();
            
        if (MamdaQuoteFields.BID_LOW != null)
            mUpdaters[i++] = new MamdaQuoteBidLow ();
            
        if (MamdaQuoteFields.ASK_PRICE != null)
            mUpdaters[i++] = new MamdaQuoteAskPrice ();
            
        if (MamdaQuoteFields.ASK_SIZE != null)
            mUpdaters[i++] = new MamdaQuoteAskSize ();
            
        if (MamdaQuoteFields.ASK_PART_ID != null)
            mUpdaters[i++] = new MamdaQuoteAskPartId ();
            
        if (MamdaQuoteFields.ASK_DEPTH != null)
            mUpdaters[i++] = new MamdaQuoteAskDepth ();
            
        if (MamdaQuoteFields.ASK_CLOSE_PRICE != null)
            mUpdaters[i++] = new MamdaQuoteAskClosePrice ();
        
        if (MamdaQuoteFields.ASK_CLOSE_DATE != null)
            mUpdaters[i++] = new MamdaQuoteAskCloseDate ();
            
        if (MamdaQuoteFields.ASK_PREV_CLOSE_PRICE != null)
            mUpdaters[i++] = new MamdaQuoteAskPrevClosePrice ();
        
        if (MamdaQuoteFields.ASK_PREV_CLOSE_DATE != null)
            mUpdaters[i++] = new MamdaQuoteAskPrevCloseDate ();
            
        if (MamdaQuoteFields.ASK_HIGH != null)
            mUpdaters[i++] = new MamdaQuoteAskHigh ();
        
        if (MamdaQuoteFields.ASK_LOW != null)
            mUpdaters[i++] = new MamdaQuoteAskLow ();       
            
        if (MamdaQuoteFields.QUOTE_SEQ_NUM != null)
            mUpdaters[i++] = new MamdaQuoteSeqNum ();
        
        if (MamdaQuoteFields.QUOTE_DATE != null)
            mUpdaters[i++] = new MamdaQuoteDate ();
        
        if (MamdaQuoteFields.QUOTE_TIME != null)
            mUpdaters[i++] = new MamdaQuoteTime ();
        
        if (MamdaQuoteFields.QUOTE_QUAL != null)
            mUpdaters[i++] = new MamdaQuoteQual ();
            
        if (MamdaQuoteFields.QUOTE_QUAL_NATIVE != null)
            mUpdaters[i++] = new MamdaQuoteQualNative ();
        
        if (MamdaQuoteFields.QUOTE_COUNT != null)
            mUpdaters[i++] = new MamdaQuoteCount ();
        
        if (MamdaQuoteFields.SHORT_SALE_BID_TICK != null)
            mUpdaters[i++] = new MamdaShortSaleBidTick ();

         if (MamdaQuoteFields.SHORT_SALE_CIRCUIT_BREAKER != null)
            mUpdaters[i++] = new MamdaShortSaleCircuitBreaker();
            
        if (MamdaQuoteFields.BID_TICK != null)
            mUpdaters[i++] = new MamdaBidTick ();
        
        if (MamdaQuoteFields.ASK_TIME != null)
            mUpdaters[i++] = new MamdaAskTime ();
        
        if (MamdaQuoteFields.BID_TIME != null)
            mUpdaters[i++] = new MamdaBidTime ();
        
        if (MamdaQuoteFields.ASK_INDICATOR != null)
            mUpdaters[i++] = new MamdaAskIndicator ();
        
        if (MamdaQuoteFields.BID_INDICATOR != null)
            mUpdaters[i++] = new MamdaBidIndicator ();
        
        if (MamdaQuoteFields.ASK_UPDATE_COUNT != null)
            mUpdaters[i++] = new MamdaAskUpdateCount ();
        
        if (MamdaQuoteFields.BID_UPDATE_COUNT != null)
            mUpdaters[i++] = new MamdaBidUpdateCount ();

        if (MamdaQuoteFields.ASK_YIELD != null)
            mUpdaters[i++] = new MamdaAskYield ();
        
        if (MamdaQuoteFields.BID_YIELD != null)
            mUpdaters[i++] = new MamdaBidYield ();
        
        if (MamdaQuoteFields.ASK_SIZES_LIST != null)
          mUpdaters[i++] = new MamdaAskSizesList ();
        
        if (MamdaQuoteFields.BID_SIZES_LIST != null)
          mUpdaters[i++] = new MamdaBidSizesList ();

    }

    private void getQuoteFields(MamaMsg msg)
    {
        int i =0;
        while (mUpdaters[i] != null)
        {
            mUpdaters[i++].onUpdate(msg, MamdaQuoteListener.this);
        }

    }
    private void updateFieldStates()
    {
      if (mQuoteCache.mSymbolFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mSymbolFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mIssueSymbolFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mIssueSymbolFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mPartIdFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mSrcTimeFieldState.getState() == MamdaFieldState.MODIFIED) 
            mQuoteCache.mSrcTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mActTimeFieldState.getState() == MamdaFieldState.MODIFIED)  
            mQuoteCache.mActTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mLineTimeFieldState.getState() == MamdaFieldState.MODIFIED) 
            mQuoteCache.mLineTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mSendTimeFieldState.getState() == MamdaFieldState.MODIFIED)  
            mQuoteCache.mSendTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mPubIdFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mPubIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidPriceFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mBidPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidSizeFieldState.getState() == MamdaFieldState.MODIFIED)   
            mQuoteCache.mBidSizeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidDepthFieldState.getState() == MamdaFieldState.MODIFIED) 
            mQuoteCache.mBidDepthFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidPartIdFieldState.getState() == MamdaFieldState.MODIFIED) 
            mQuoteCache.mBidPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidClosePriceFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mBidClosePriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidCloseDateFieldState.getState() == MamdaFieldState.MODIFIED)   
            mQuoteCache.mBidCloseDateFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidPrevClosePriceFieldState.getState() == MamdaFieldState.MODIFIED)    
            mQuoteCache.mBidPrevClosePriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidPrevCloseDateFieldState.getState() == MamdaFieldState.MODIFIED)   
            mQuoteCache.mBidPrevCloseDateFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidHighFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mBidHighFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidLowFieldState.getState() == MamdaFieldState.MODIFIED)   
            mQuoteCache.mBidLowFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskPriceFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mAskPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskSizeFieldState.getState() == MamdaFieldState.MODIFIED)   
            mQuoteCache.mAskSizeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskDepthFieldState.getState() == MamdaFieldState.MODIFIED) 
            mQuoteCache.mAskDepthFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskPartIdFieldState.getState() == MamdaFieldState.MODIFIED) 
            mQuoteCache.mAskPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskClosePriceFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mAskClosePriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskCloseDateFieldState.getState() == MamdaFieldState.MODIFIED)   
            mQuoteCache.mAskCloseDateFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskPrevClosePriceFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mAskPrevClosePriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskPrevCloseDateFieldState.getState() == MamdaFieldState.MODIFIED)   
            mQuoteCache.mAskPrevCloseDateFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskHighFieldState.getState() == MamdaFieldState.MODIFIED) 
            mQuoteCache.mAskHighFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskLowFieldState.getState() == MamdaFieldState.MODIFIED)   
            mQuoteCache.mAskLowFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mMidPriceFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mMidPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mEventSeqNumFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mEventSeqNumFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mEventDateFieldState.getState() == MamdaFieldState.MODIFIED)    
            mQuoteCache.mEventDateFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mEventTimeFieldState.getState() == MamdaFieldState.MODIFIED)  
            mQuoteCache.mEventTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mQuoteQualStrFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mQuoteQualStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mQuoteQualNativeFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mQuoteQualNativeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mShortSaleBidTickFieldState.getState() == MamdaFieldState.MODIFIED) 
            mQuoteCache.mShortSaleBidTickFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mShortSaleCircuitBreakerFieldState.getState() == MamdaFieldState.MODIFIED) 
            mQuoteCache.mShortSaleCircuitBreakerFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
      if (mQuoteCache.mAskTimeFieldState.getState() == MamdaFieldState.MODIFIED) 
            mQuoteCache.mAskTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidTimeFieldState.getState() == MamdaFieldState.MODIFIED)  
            mQuoteCache.mBidTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskIndicatorFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mAskIndicatorFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidIndicatorFieldState.getState() == MamdaFieldState.MODIFIED)   
            mQuoteCache.mBidIndicatorFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskUpdateCountFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mAskUpdateCountFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidUpdateCountFieldState.getState() == MamdaFieldState.MODIFIED)  
            mQuoteCache.mBidUpdateCountFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskYieldFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mAskYieldFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidYieldFieldState.getState() == MamdaFieldState.MODIFIED)  
            mQuoteCache.mBidYieldFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mTmpQuoteCountFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mTmpQuoteCountFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mQuoteCountFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mQuoteCountFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mConflateCountFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mConflateCountFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mQuoteCountFieldState.getState() == MamdaFieldState.MODIFIED)
            mQuoteCache.mQuoteCountFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mAskSizesListFieldState.getState() == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskSizesListFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mQuoteCache.mBidSizesListFieldState.getState() == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidSizesListFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      
      if (mGapBeginFieldState.getState() == MamdaFieldState.MODIFIED)
            mGapBeginFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      if (mGapEndFieldState.getState() == MamdaFieldState.MODIFIED)
            mGapEndFieldState.setState(MamdaFieldState.NOT_MODIFIED);
      
      
    }
    private void updateQuoteFields (MamaMsg msg )
    {
        mLastGenericMsgWasQuote     = false;
        mQuoteCache.mGotQuoteCount  = false;
        mQuoteCache.mGotIssueSymbol = false;
		mQuoteCache.mGotBidPrice    = false;
		mQuoteCache.mGotAskPrice    = false;
		mQuoteCache.mGotBidSize     = false;
		mQuoteCache.mGotAskSize     = false;
		mQuoteCache.mGotBidDepth    = false;
		mQuoteCache.mGotAskDepth    = false;
		
		mQuoteCache.mGotPartId      = false;
		mQuoteCache.mGotBidPartId   = false;
		mQuoteCache.mGotAskPartId   = false;
		

        // Iterate over all of the fields in the message.
        synchronized (this)
        {
            getQuoteFields(msg);        
        }

        if (mQuoteCache.mGotIssueSymbol)
        {
            mQuoteCache.mSymbol.setValue(mQuoteCache.mIssueSymbol.getValue());
            mQuoteCache.mSymbolFieldState.setState(MamdaFieldState.MODIFIED);
        }
        
        // Check certain special fields.
        if (mQuoteCache.mGotBidSize   || mQuoteCache.mGotAskSize   ||
            mQuoteCache.mGotBidDepth  || mQuoteCache.mGotAskDepth  ||
            mQuoteCache.mGotBidPrice  || mQuoteCache.mGotAskPrice  ||
            mQuoteCache.mGotBidPartId || mQuoteCache.mGotAskPartId)
        {
            mLastGenericMsgWasQuote = true;
        }

        if (mQuoteCache.mGotPartId == false && mQuoteCache.mSymbol.getValue() != null)
        {
            // No explicit part ID in message, but maybe in symbol.
            int lastDot = mQuoteCache.mSymbol.getValue().indexOf (".");
            if (lastDot != -1)
            {
                lastDot++;
                if (lastDot < mQuoteCache.mSymbol.getValue().length ())
                {
                    mQuoteCache.mPartId.setValue (mQuoteCache.mSymbol.getValue().substring (lastDot));
                    mQuoteCache.mPartIdFieldState.setState (MamdaFieldState.MODIFIED);
                    mQuoteCache.mGotPartId = true; 
                }
            }
        }
    }


    private interface QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener);
    }
    
    private static class MamdaQuoteSymbol implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (listener.mQuoteCache.mSymbol.getValue() == null)
            {
                msg.tryString (null, MamdaCommonFields.SYMBOL.getFid(), listener.mQuoteCache.mSymbol);
                listener.mQuoteCache.mSymbolFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }
    
    private static class MamdaQuoteIssueSymbol implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryString (null, MamdaCommonFields.ISSUE_SYMBOL.getFid(), listener.mQuoteCache.mIssueSymbol))
            {
                listener.mQuoteCache.mGotIssueSymbol = true;
                listener.mQuoteCache.mIssueSymbolFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class MamdaQuotePartId implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {

            if (msg.tryString (null, MamdaCommonFields.PART_ID.getFid(), listener.mQuoteCache.mPartId))
            {
                listener.mQuoteCache.mGotPartId = true; 
                listener.mQuoteCache.mPartIdFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class MamdaQuoteBidPrice implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryPrice (null, MamdaQuoteFields.BID_PRICE.getFid(), listener.mQuoteCache.tmpPrice))
            {
                if (listener.mQuoteCache.tmpPrice.getValue() != listener.mQuoteCache.mBidPrice.getValue())
                {
                    listener.mQuoteCache.mBidPrice.setValue (listener.mQuoteCache.tmpPrice.getValue());
                    listener.mQuoteCache.mGotBidPrice = true;
                    listener.mQuoteCache.mBidPriceFieldState.setState (MamdaFieldState.MODIFIED);
                }
            }
        }
    }

    private static class MamdaQuoteBidSize implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryF64 (null, MamdaQuoteFields.BID_SIZE.getFid(), listener.mQuoteCache.tmpDouble))
            {
                if (listener.mQuoteCache.tmpDouble.getValue() != listener.mQuoteCache.mBidSize.getValue())
                {
                    listener.mQuoteCache.mBidSize.setValue (listener.mQuoteCache.tmpDouble.getValue());
                    listener.mQuoteCache.mGotBidSize = true;
                    listener.mQuoteCache.mBidSizeFieldState.setState (MamdaFieldState.MODIFIED);
                }
            }
        }
    }

    private static class MamdaQuoteBidPartId implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryString (null, MamdaQuoteFields.BID_PART_ID.getFid(), listener.mQuoteCache.tmpString))
            {
                //1st Update
                if (listener.mQuoteCache.mBidPartId.getValue() == null)
                {
                    listener.mQuoteCache.mBidPartId.setValue (listener.mQuoteCache.tmpString.getValue());
                    listener.mQuoteCache.mGotBidPartId = true;
                    listener.mQuoteCache.mBidPartIdFieldState.setState (MamdaFieldState.MODIFIED);
                }
                else if (!listener.mQuoteCache.tmpString.getValue().equals(
                         listener.mQuoteCache.mBidPartId.getValue()))
                {
                    listener.mQuoteCache.mBidPartId.setValue (listener.mQuoteCache.tmpString.getValue());
                    listener.mQuoteCache.mGotBidPartId = true;
                    listener.mQuoteCache.mBidPartIdFieldState.setState (MamdaFieldState.MODIFIED);
                }
            }
        }
    }

    private static class MamdaQuoteBidDepth implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryF64 (null, MamdaQuoteFields.BID_DEPTH.getFid(), listener.mQuoteCache.tmpDouble))
            {
                if (listener.mQuoteCache.mBidDepth.getValue() != listener.mQuoteCache.tmpDouble.getValue())
                {
                    listener.mQuoteCache.mBidDepth.setValue (listener.mQuoteCache.tmpDouble.getValue());
                    listener.mQuoteCache.mBidDepthFieldState.setState (MamdaFieldState.MODIFIED);
                    listener.mQuoteCache.mGotBidDepth = true;
                }
            }
        }
    }

    private static class MamdaQuoteBidClosePrice implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryPrice (null, MamdaQuoteFields.BID_CLOSE_PRICE.getFid(), listener.mQuoteCache.mBidClosePrice))
                listener.mQuoteCache.mBidClosePriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteBidCloseDate implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaQuoteFields.BID_CLOSE_DATE.getFid(), listener.mQuoteCache.mBidCloseDate))
                listener.mQuoteCache.mBidCloseDateFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteBidPrevClosePrice implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryPrice (null, MamdaQuoteFields.BID_PREV_CLOSE_PRICE.getFid(), listener.mQuoteCache.mBidPrevClosePrice))
                listener.mQuoteCache.mBidPrevClosePriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteBidPrevCloseDate implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaQuoteFields.BID_PREV_CLOSE_DATE.getFid(), listener.mQuoteCache.mBidPrevCloseDate))
                listener.mQuoteCache.mBidPrevCloseDateFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteBidHigh implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryPrice (null, MamdaQuoteFields.BID_HIGH.getFid(), listener.mQuoteCache.mBidHigh))
                listener.mQuoteCache.mBidHighFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteBidLow implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryPrice (null, MamdaQuoteFields.BID_LOW.getFid(), listener.mQuoteCache.mBidLow))
                listener.mQuoteCache.mBidLowFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteAskPrice implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryPrice (null, MamdaQuoteFields.ASK_PRICE.getFid(), listener.mQuoteCache.tmpPrice))
            {
                if (listener.mQuoteCache.mAskPrice.getValue() != listener.mQuoteCache.tmpPrice.getValue())
                {
                    listener.mQuoteCache.mAskPrice.setValue (listener.mQuoteCache.tmpPrice.getValue());
                    listener.mQuoteCache.mAskPriceFieldState.setState (MamdaFieldState.MODIFIED);
                    listener.mQuoteCache.mGotAskPrice = true;
                }
            }
        }
    }

    private static class MamdaQuoteAskSize implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryF64 (null, MamdaQuoteFields.ASK_SIZE.getFid(), listener.mQuoteCache.tmpDouble))
            {
                if (listener.mQuoteCache.mAskSize.getValue() != listener.mQuoteCache.tmpDouble.getValue())
                {
                    listener.mQuoteCache.mAskSize.setValue (listener.mQuoteCache.tmpDouble.getValue());
                    listener.mQuoteCache.mAskSizeFieldState.setState (MamdaFieldState.MODIFIED);
                    listener.mQuoteCache.mGotAskSize = true;
                }
            }
        }
    }
    private static class MamdaQuoteAskPartId implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryString (null, MamdaQuoteFields.ASK_PART_ID.getFid(), listener.mQuoteCache.tmpString))
            {
                //1st Update
                if (listener.mQuoteCache.mAskPartId.getValue() == null)
                {
                    listener.mQuoteCache.mAskPartId.setValue (listener.mQuoteCache.tmpString.getValue());
                    listener.mQuoteCache.mGotAskPartId = true;
                    listener.mQuoteCache.mAskPartIdFieldState.setState (MamdaFieldState.MODIFIED);
                }
                else if (!listener.mQuoteCache.mAskPartId.getValue().equals(
                         listener.mQuoteCache.tmpString.getValue()))
                {
                    listener.mQuoteCache.mAskPartId.setValue (listener.mQuoteCache.tmpString.getValue());
                    listener.mQuoteCache.mGotAskPartId = true;
                    listener.mQuoteCache.mAskPartIdFieldState.setState (MamdaFieldState.MODIFIED);
                }
            }
        }
    }

    private static class MamdaQuoteAskDepth implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryF64 (null, MamdaQuoteFields.ASK_DEPTH.getFid(), listener.mQuoteCache.tmpDouble))
            {
                if (listener.mQuoteCache.mAskDepth.getValue() != listener.mQuoteCache.tmpDouble.getValue())
                {
                    listener.mQuoteCache.mAskDepth.setValue (listener.mQuoteCache.tmpDouble.getValue());
                    listener.mQuoteCache.mAskDepthFieldState.setState (MamdaFieldState.MODIFIED);
                    listener.mQuoteCache.mGotAskDepth  = true;
                }
            }
        }
    }

    private static class MamdaQuoteAskClosePrice implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryPrice (null, MamdaQuoteFields.ASK_CLOSE_PRICE.getFid(), listener.mQuoteCache.mAskClosePrice))
                listener.mQuoteCache.mAskClosePriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteAskCloseDate implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaQuoteFields.ASK_CLOSE_DATE.getFid(), listener.mQuoteCache.mAskCloseDate))
                listener.mQuoteCache.mAskCloseDateFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteAskPrevClosePrice implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryPrice (null, MamdaQuoteFields.ASK_PREV_CLOSE_PRICE.getFid(), listener.mQuoteCache.mAskPrevClosePrice))
                listener.mQuoteCache.mAskPrevClosePriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteAskPrevCloseDate implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaQuoteFields.ASK_PREV_CLOSE_DATE.getFid(), listener.mQuoteCache.mAskPrevCloseDate))
                listener.mQuoteCache.mAskPrevCloseDateFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteAskHigh implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryPrice (null, MamdaQuoteFields.ASK_HIGH.getFid(), listener.mQuoteCache.mAskHigh))
                listener.mQuoteCache.mAskHighFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteAskLow implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryPrice (null, MamdaQuoteFields.ASK_LOW.getFid(), listener.mQuoteCache.mAskLow))
                listener.mQuoteCache.mAskLowFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteSrcTime implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaCommonFields.SRC_TIME.getFid(), listener.mQuoteCache.mSrcTime))
                listener.mQuoteCache.mSrcTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }
    private static class MamdaQuoteActivityTime implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaCommonFields.ACTIVITY_TIME.getFid(), listener.mQuoteCache.mActTime))
                listener.mQuoteCache.mActTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteLineTime implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaCommonFields.LINE_TIME.getFid(), listener.mQuoteCache.mLineTime))
                listener.mQuoteCache.mLineTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteSendTime implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaCommonFields.SEND_TIME.getFid(), listener.mQuoteCache.mSendTime))
                listener.mQuoteCache.mSendTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuotePubId implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryString (null, MamdaCommonFields.PUB_ID.getFid(), listener.mQuoteCache.mPubId))
                listener.mQuoteCache.mPubIdFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteSeqNum implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryI64 (null, MamdaQuoteFields.QUOTE_SEQ_NUM.getFid(), listener.mQuoteCache.mEventSeqNum))
                listener.mQuoteCache.mEventSeqNumFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteDate implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
             if (msg.tryDateTime (null, MamdaQuoteFields.QUOTE_DATE.getFid(), listener.mQuoteCache.mEventDate))
                 listener.mQuoteCache.mEventDateFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteTime implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaQuoteFields.QUOTE_TIME.getFid(), listener.mQuoteCache.mEventTime))
                listener.mQuoteCache.mEventTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteQual implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryString (null, MamdaQuoteFields.QUOTE_QUAL.getFid(), listener.mQuoteCache.mQuoteQualStr))
                listener.mQuoteCache.mQuoteQualStrFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaQuoteQualNative implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            listener.tmpField = msg.getField (null, MamdaQuoteFields.QUOTE_QUAL_NATIVE.getFid(), null);
            if (listener.tmpField != null)
            {
                if (listener.tmpField.getType() == MamaFieldDescriptor.STRING)
                {
                    listener.mQuoteCache.mQuoteQualNative.setValue(
                            listener.tmpField.getString());
                    listener.mQuoteCache.mQuoteQualNativeFieldState.setState(
                        MamdaFieldState.MODIFIED);
                }
                else if (listener.tmpField.getType() == MamaFieldDescriptor.CHAR)
                {
                    listener.mQuoteCache.mQuoteQualNative.setValue(
                            Character.toString (listener.tmpField.getChar()));
                    listener.mQuoteCache.mQuoteQualNativeFieldState.setState(
                        MamdaFieldState.MODIFIED);
                }
            }
        }
    }
    
    private static class MamdaQuoteCount implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryI64 (null, MamdaQuoteFields.QUOTE_COUNT.getFid(), listener.mQuoteCache.mTmpQuoteCount))
            {
                listener.mQuoteCache.mTmpQuoteCountFieldState.setState (MamdaFieldState.MODIFIED);
                listener.mQuoteCache.mGotQuoteCount = true;
            }
        }
    }

    private static class MamdaShortSaleBidTick implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            // There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            // FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            // Adding support for this in MAMDA for client apps coded to expect this behaviour
            listener.tmpField = msg.getField (null, MamdaQuoteFields.SHORT_SALE_BID_TICK.getFid(), null);
            if (listener.tmpField != null)
            {
                switch (listener.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8 :
                    case MamaFieldDescriptor.CHAR :
                        listener.mQuoteCache.mShortSaleBidTick.setValue(
                            listener.tmpField.getChar ());
                        listener.mQuoteCache.mShortSaleBidTickFieldState.setState(
                            MamdaFieldState.MODIFIED);
                        break;

                    case MamaFieldDescriptor.STRING :
                        if (listener.tmpField.getString().length() > 0)
                        {
                            listener.mQuoteCache.mShortSaleBidTick.setValue(
                                listener.tmpField.getString().charAt(0));
                            listener.mQuoteCache.mShortSaleBidTickFieldState.setState(
                                MamdaFieldState.MODIFIED);
                        }
                        else
                        {
                            listener.mQuoteCache.mShortSaleBidTick.setValue(' ');
                            listener.mQuoteCache.mShortSaleBidTickFieldState.setState(
                                MamdaFieldState.MODIFIED);
                        }
                        break;

                    default: 
                        break;
                }
            }
        }
    }

    private static class MamdaShortSaleCircuitBreaker implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg,  MamdaQuoteListener listener)                             
        { 
            if (msg.tryChar (null, MamdaQuoteFields.SHORT_SALE_CIRCUIT_BREAKER.getFid(), listener.mQuoteCache.mShortSaleCircuitBreaker))
                listener.mQuoteCache.mShortSaleCircuitBreakerFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }
    
    private static class MamdaBidTick implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            // There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            // FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            // Adding support for this in MAMDA for client apps coded to expect this behaviour
            listener.tmpField =  msg.getField (null, MamdaQuoteFields.BID_TICK.getFid(), null);
            if (listener.tmpField != null)
            {
                switch (listener.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8 :
                    case MamaFieldDescriptor.CHAR :
                        listener.mQuoteCache.mShortSaleBidTick.setValue(
                            listener.tmpField.getChar ());
                        listener.mQuoteCache.mShortSaleBidTickFieldState.setState(
                            MamdaFieldState.MODIFIED);
                        break;

                    case MamaFieldDescriptor.STRING :
                        if (listener.tmpField.getString().length() > 0)
                        {
                            listener.mQuoteCache.mShortSaleBidTick.setValue(
                                listener.tmpField.getString().charAt(0));
                            listener.mQuoteCache.mShortSaleBidTickFieldState.setState(
                                MamdaFieldState.MODIFIED);
                        }
                        else
                        {
                            listener.mQuoteCache.mShortSaleBidTick.setValue(' ');
                            listener.mQuoteCache.mShortSaleBidTickFieldState.setState(
                                MamdaFieldState.MODIFIED);
                        }
                        break;

                    default: 
                        break;
                }
            }
        }
    }
    
    private static class MamdaAskTime implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaQuoteFields.ASK_TIME.getFid(), listener.mQuoteCache.mAskTime))
                listener.mQuoteCache.mAskTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }
    
    private static class MamdaBidTime implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryDateTime (null, MamdaQuoteFields.BID_TIME.getFid(), listener.mQuoteCache.mBidTime))
                listener.mQuoteCache.mBidTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }
    
    private static class MamdaAskIndicator implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            listener.tmpField =  msg.getField (null, MamdaQuoteFields.ASK_INDICATOR.getFid(), null);
            if (listener.tmpField != null)
            {
                if (listener.tmpField.getType() == MamaFieldDescriptor.STRING)
                {
                    listener.mQuoteCache.mAskIndicator.setValue(
                            listener.tmpField.getString());
                    listener.mQuoteCache.mAskIndicatorFieldState.setState (MamdaFieldState.MODIFIED);
                }
                else if (listener.tmpField.getType() == MamaFieldDescriptor.CHAR)
                {
                    listener.mQuoteCache.mAskIndicator.setValue(
                            Character.toString (listener.tmpField.getChar()));
                    listener.mQuoteCache.mAskIndicatorFieldState.setState (MamdaFieldState.MODIFIED);
                }
            }
        }
    }
    
    private static class MamdaBidIndicator implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            listener.tmpField =  msg.getField (null, MamdaQuoteFields.BID_INDICATOR.getFid(), null);
            if (listener.tmpField != null)
            {
                if (listener.tmpField.getType() == MamaFieldDescriptor.STRING)
                {
                    listener.mQuoteCache.mBidIndicator.setValue(
                            listener.tmpField.getString());
                    listener.mQuoteCache.mBidIndicatorFieldState.setState (MamdaFieldState.MODIFIED);
                }
                else if (listener.tmpField.getType() == MamaFieldDescriptor.CHAR)
                {
                    listener.mQuoteCache.mBidIndicator.setValue(
                            Character.toString (listener.tmpField.getChar()));
                    listener.mQuoteCache.mBidIndicatorFieldState.setState (MamdaFieldState.MODIFIED);
                }
            }
        }
    }
    
    private static class MamdaAskUpdateCount implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryU32 (null, MamdaQuoteFields.ASK_UPDATE_COUNT.getFid(), listener.mQuoteCache.mAskUpdateCount))
                listener.mQuoteCache.mAskUpdateCountFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }
    
    private static class MamdaBidUpdateCount implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryU32 (null, MamdaQuoteFields.BID_UPDATE_COUNT.getFid(), listener.mQuoteCache.mBidUpdateCount))
                listener.mQuoteCache.mBidUpdateCountFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }
    
    private static class MamdaAskYield implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryF64 (null, MamdaQuoteFields.ASK_YIELD.getFid(), listener.mQuoteCache.mAskYield))
                listener.mQuoteCache.mAskYieldFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }
    
    private static class MamdaBidYield implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryF64 (null, MamdaQuoteFields.BID_YIELD.getFid(), listener.mQuoteCache.mBidYield))
                listener.mQuoteCache.mBidYieldFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }
    
    private static class MamdaBidSizesList implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryString (null, MamdaQuoteFields.BID_SIZES_LIST.getFid(), listener.mQuoteCache.mBidSizesList))
                listener.mQuoteCache.mBidSizesListFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }
    
    private static class MamdaAskSizesList implements QuoteUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaQuoteListener listener)
        {
            if (msg.tryString (null, MamdaQuoteFields.ASK_SIZES_LIST.getFid(), listener.mQuoteCache.mAskSizesList))
                listener.mQuoteCache.mAskSizesListFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }
}
