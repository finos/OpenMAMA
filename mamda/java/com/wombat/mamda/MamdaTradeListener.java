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
import java.util.logging.Logger;
import java.util.Iterator;
import java.util.LinkedList;
import java.io.*;
import java.lang.*;

/**
* MamdaTradeListener is a class that specializes in handling trade
* updates.  Developers provide their own implementation of the
* MamdaTradeHandler interface and will be delivered notifications for
* trades, trade cancels/error/corrections, and trade closing prices.
* An obvious application for this MAMDA class is any kind of trade
* tick capture application.
*
* Note: The MamdaTradeListener class caches trade-related field
* values.  Among other reasons, caching of these fields makes it
* possible to provide complete trade-related callbacks, even when the
* publisher (e.g., feed handler) is only publishing deltas containing
* modified fields.
*/

public class MamdaTradeListener implements MamdaMsgListener, 
                                           MamdaTradeRecap,
                                           MamdaTradeReport,
                                           MamdaTradeGap,
                                           MamdaTradeCancelOrError,
                                           MamdaTradeCorrection,
                                           MamdaTradeClosing
{
    private static Logger mLogger =
            Logger.getLogger("com.wombat.mamda.MamdaTradeListener");

    // We only need a single instance for use accross all Listener instances.
    // Each updater instance maintains no state.
    private static TradeUpdate  mUpdaters []        = null;
    private static boolean      mUpdatersComplete   = false;
    private static Object       mUpdatersLock       = new Object ();

    private final LinkedList    mHandlers           = new LinkedList();

    // Whether we should try to identify updates as trades.
    // This is true by default. Any feed wishing to not have updates processed
    // as trades will send a flag in the initial value indicating the fact.
    private MamaBoolean mProcessUpdateAsTrade =  new MamaBoolean();

    // The following fields are used for caching the offical last
    // price and related fields.  These fields can be used by
    // applications for reference and will be passed for recaps.
    protected final MamdaTradeCache mTradeCache =
                new MamdaTradeCache ();

    private boolean mIgnoreUpdate = false;

    private class MamdaTradeCache 
    {
        // The following fields are used for caching the offical last
        // price and related fields.  These fields can be used by
        // applications for reference and will be passed for recaps.
        public MamaString       mSymbol            = new MamaString();     
        public MamaString       mIssueSymbol       = new MamaString();
        public MamaString       mPartId            = new MamaString();
        public MamaDateTime     mSrcTime           = new MamaDateTime();
        public MamaDateTime     mActTime           = new MamaDateTime();
        public MamaDateTime     mLineTime          = new MamaDateTime();
        public MamaDateTime     mSendTime          = new MamaDateTime();
        public MamaString       mPubId             = new MamaString();
        public MamaString       mTradeId           = new MamaString ();
        public MamaString       mOrigTradeId       = new MamaString ();
        public MamaString       mCorrTradeId       = new MamaString ();
        public MamaBoolean      mIsIrregular       = new MamaBoolean();
        public boolean          mWasIrregular      = false;
        public MamaPrice        mLastPrice         = new MamaPrice();
        public MamaDouble       mLastVolume        = new MamaDouble();
        public MamaString       mLastPartId        = new MamaString();
        public MamaDateTime     mLastDate          = new MamaDateTime();
        public MamaDateTime     mLastTime          = new MamaDateTime();
        public MamaPrice        mIrregPrice        = new MamaPrice();
        public MamaDouble       mIrregVolume       = new MamaDouble();
        public MamaString       mIrregPartId       = new MamaString();
        public MamaDateTime     mIrregTime         = new MamaDateTime();
        public MamaDouble       mAccVolume         = new MamaDouble();
        public MamaDouble       mOffExAccVolume    = new MamaDouble();
        public MamaDouble       mOnExAccVolume     = new MamaDouble();
        public MamaString       mTradeDirection    = new MamaString ();
        public MamaChar         mTmpChar           = new MamaChar();     
        public MamdaTradeSide   mTmpSide           = new MamdaTradeSide();
        public MamaPrice        mNetChange         = new MamaPrice();
        public MamaDouble       mPctChange         = new MamaDouble();
        public MamaPrice        mOpenPrice         = new MamaPrice();
        public MamaPrice        mHighPrice         = new MamaPrice();
        public MamaPrice        mLowPrice          = new MamaPrice();
        public MamaPrice        mClosePrice        = new MamaPrice();
        public MamaPrice        mPrevClosePrice    = new MamaPrice();
        public MamaDateTime     mPrevCloseDate     = new MamaDateTime();
        public MamaPrice        mAdjPrevClose      = new MamaPrice();
        public long             mTradeCount        = 0;
        public MamaDouble       mBlockVolume       = new MamaDouble();
        public MamaLong         mBlockCount        = new MamaLong();
        public MamaDouble       mVwap              = new MamaDouble();
        public MamaDouble       mOffExVwap         = new MamaDouble();
        public MamaDouble       mOnExVwap          = new MamaDouble();
        public MamaDouble       mTotalValue        = new MamaDouble();
        public MamaDouble       mOffExTotalValue   = new MamaDouble();
        public MamaDouble       mOnExTotalValue    = new MamaDouble();
        public MamaDouble       mStdDev            = new MamaDouble();
        public MamaDouble       mStdDevSum         = new MamaDouble();
        public MamaDouble       mStdDevSumSquares  = new MamaDouble();
        public MamaLong         mOrderId           = new MamaLong();
        public MamaPrice        mSettlePrice       = new MamaPrice();
        public MamaDateTime     mSettleDate        = new MamaDateTime();
        public MamaChar         mShortSaleCircuitBreaker = new MamaChar();
        

        // The following fields are used for caching the last reported
        // trade, which might have been out of order.  The reason for
        // cahcing these values is to allow a configuration that passes
        // the minimum amount of data  (unchanged fields not sent).
        public MamaLong         mEventSeqNum       = new MamaLong();
        public MamaDateTime     mEventTime         = new MamaDateTime ();
        public MamaPrice        mTradePrice        = new MamaPrice();
        public MamaDouble       mTradeVolume       = new MamaDouble();       
        public MamaString       mTradePartId       = new MamaString ();
        public MamaString       mTradeQualStr      = new MamaString ();
        public MamaString       mTradeQualNativeStr = new MamaString ();
        public MamaLong         mSellersSaleDays   = new MamaLong();
        public MamaChar         mStopStockInd      = new MamaChar ();

        public MamaString       mTradeExecVenue    = new MamaString();
        public MamaPrice        mOffExTradePrice   = new MamaPrice();
        public MamaPrice        mOnExTradePrice    = new MamaPrice();

        //mTmpTradeCount ~ see below

        // Additional fields for cancels/error/corrections:
        public boolean          mIsCancel          = false;
        public MamaLong         mOrigSeqNum        = new MamaLong();
        public MamaPrice        mOrigPrice         = new MamaPrice();
        public MamaDouble       mOrigVolume        = new MamaDouble();
        public MamaString       mOrigPartId        = new MamaString ();
        public MamaString       mOrigQualStr       = new MamaString ();
        public MamaString       mOrigQualNativeStr = new MamaString ();
        public MamaLong         mOrigSellersSaleDays = new MamaLong();
        public MamaChar         mOrigStopStockInd  = new MamaChar ();
        public MamaChar         mOrigShortSaleCircuitBreaker = new MamaChar();
        public MamaPrice        mCorrPrice         = new MamaPrice();
        public MamaDouble       mCorrVolume        = new MamaDouble();
        public MamaString       mCorrPartId        = new MamaString ();
        public MamaString       mCorrQualStr       = new MamaString ();
        public MamaString       mCorrQualNativeStr = new MamaString ();
        public MamaLong         mCorrSellersSaleDays = new MamaLong();
        public MamaChar         mCorrStopStockInd  = new MamaChar ();
        public MamaDateTime     mCorrTime          = new MamaDateTime ();
        public MamaDateTime     mCancelTime        = new MamaDateTime ();
        public MamaString       mTradeUnits        = new MamaString ();
        public MamaLong         mLastSeqNum        = new MamaLong();
        public MamaLong         mHighSeqNum        = new MamaLong();
        public MamaLong         mLowSeqNum         = new MamaLong();
        public MamaLong         mTotalVolumeSeqNum = new MamaLong();
        public MamaString       mCurrencyCode      = new MamaString ();

        // Not in C++.
        public MamaString       mTradeCondStr      = new MamaString ();
        public MamaString       mOrigCondStr       = new MamaString ();
        public MamaString       mCorrCondStr       = new MamaString ();

        // This is an aggreggated field for aggressorside and trade side.
        public String           mSide              = new String ();

        public MamaLong         mConflateCount     = new MamaLong();

        public boolean          mGotPartId         = false;
        public boolean          mLastGenericMsgWasTrade = false;
        public boolean          mGotTradeTime      = false;
        public boolean          mGotTradePrice     = false;
        public boolean          mGotTradeSize      = false;
        public boolean          mGotTradeCount     = false;
        public boolean          mGotIssueSymbol    = false;
        

        /*      FieldState          */     
        public MamdaFieldState  mSymbolFieldState            = new MamdaFieldState();
        public MamdaFieldState  mIssueSymbolFieldState       = new MamdaFieldState();
        public MamdaFieldState  mPartIdFieldState            = new MamdaFieldState();
        public MamdaFieldState  mSrcTimeFieldState           = new MamdaFieldState();
        public MamdaFieldState  mActTimeFieldState           = new MamdaFieldState();
        public MamdaFieldState  mLineTimeFieldState          = new MamdaFieldState();
        public MamdaFieldState  mSendTimeFieldState          = new MamdaFieldState();
        public MamdaFieldState  mPubIdFieldState             = new MamdaFieldState();
        public MamdaFieldState  mIsIrregularFieldState       = new MamdaFieldState();
        public MamdaFieldState  mWasIrregularFieldState      = new MamdaFieldState();
        public MamdaFieldState  mLastPriceFieldState         = new MamdaFieldState();
        public MamdaFieldState  mLastVolumeFieldState        = new MamdaFieldState();
        public MamdaFieldState  mLastPartIdFieldState        = new MamdaFieldState();
        public MamdaFieldState  mLastDateFieldState          = new MamdaFieldState();
        public MamdaFieldState  mLastTimeFieldState          = new MamdaFieldState();
        public MamdaFieldState  mIrregPriceFieldState        = new MamdaFieldState();
        public MamdaFieldState  mIrregVolumeFieldState       = new MamdaFieldState();
        public MamdaFieldState  mIrregPartIdFieldState       = new MamdaFieldState();
        public MamdaFieldState  mIrregTimeFieldState         = new MamdaFieldState();
        public MamdaFieldState  mAccVolumeFieldState         = new MamdaFieldState();
        public MamdaFieldState  mOffExAccVolumeFieldState    = new MamdaFieldState();
        public MamdaFieldState  mOnExAccVolumeFieldState     = new MamdaFieldState();
        public MamdaFieldState  mTradeDirectionFieldState    = new MamdaFieldState ();    
        public MamdaFieldState  mSideFieldState              = new MamdaFieldState();   
        public MamdaFieldState  mNetChangeFieldState         = new MamdaFieldState();
        public MamdaFieldState  mPctChangeFieldState         = new MamdaFieldState();
        public MamdaFieldState  mOpenPriceFieldState         = new MamdaFieldState();
        public MamdaFieldState  mHighPriceFieldState         = new MamdaFieldState();
        public MamdaFieldState  mLowPriceFieldState          = new MamdaFieldState();
        public MamdaFieldState  mClosePriceFieldState        = new MamdaFieldState();
        public MamdaFieldState  mPrevClosePriceFieldState    = new MamdaFieldState();
        public MamdaFieldState  mPrevCloseDateFieldState     = new MamdaFieldState();
        public MamdaFieldState  mAdjPrevCloseFieldState      = new MamdaFieldState();
        public MamdaFieldState  mTradeCountFieldState        = new MamdaFieldState();
        public MamdaFieldState  mBlockVolumeFieldState       = new MamdaFieldState();
        public MamdaFieldState  mBlockCountFieldState        = new MamdaFieldState();
        public MamdaFieldState  mVwapFieldState              = new MamdaFieldState();
        public MamdaFieldState  mOffExVwapFieldState         = new MamdaFieldState();
        public MamdaFieldState  mOnExVwapFieldState          = new MamdaFieldState();
        public MamdaFieldState  mTotalValueFieldState        = new MamdaFieldState();
        public MamdaFieldState  mOffExTotalValueFieldState   = new MamdaFieldState();
        public MamdaFieldState  mOnExTotalValueFieldState    = new MamdaFieldState();
        public MamdaFieldState  mStdDevFieldState            = new MamdaFieldState();
        public MamdaFieldState  mStdDevSumFieldState         = new MamdaFieldState();
        public MamdaFieldState  mStdDevSumSquaresFieldState  = new MamdaFieldState();
        public MamdaFieldState  mOrderIdFieldState           = new MamdaFieldState();
        public MamdaFieldState  mSettlePriceFieldState       = new MamdaFieldState();
        public MamdaFieldState  mSettleDateFieldState        = new MamdaFieldState();
        public MamdaFieldState  mShortSaleCircuitBreakerFieldState  = new MamdaFieldState();

        // The following fields are used for caching the last reported
        // trade, which might have been out of order.  The reason for
        // cahcing these values is to allow a configuration that passes
        // the minimum amount of data  (unchanged fields not sent).
        public MamdaFieldState  mEventSeqNumFieldState       = new MamdaFieldState();
        public MamdaFieldState  mEventTimeFieldState         = new MamdaFieldState ();
        public MamdaFieldState  mTradePriceFieldState        = new MamdaFieldState();
        public MamdaFieldState  mTradeVolumeFieldState       = new MamdaFieldState();
        public MamdaFieldState  mTradePartIdFieldState       = new MamdaFieldState ();
        public MamdaFieldState  mTradeIdFieldState           = new MamdaFieldState ();
        public MamdaFieldState  mOrigTradeIdFieldState       = new MamdaFieldState ();
        public MamdaFieldState  mCorrTradeIdFieldState       = new MamdaFieldState ();
        public MamdaFieldState  mTradeQualStrFieldState      = new MamdaFieldState ();
        public MamdaFieldState  mTradeQualNativeStrFieldState = new MamdaFieldState ();
        public MamdaFieldState  mSellersSaleDaysFieldState   = new MamdaFieldState();
        public MamdaFieldState  mStopStockIndFieldState      = new MamdaFieldState ();

        public MamdaFieldState  mTradeExecVenueFieldState    = new MamdaFieldState();
        public MamdaFieldState  mOffExTradePriceFieldState   = new MamdaFieldState();
        public MamdaFieldState  mOnExTradePriceFieldState    = new MamdaFieldState();

        //mTmpTradeCount ~ see below

        // Additional fields for cancels/error/corrections:
        public MamdaFieldState  mIsCancelFieldState          = new MamdaFieldState();
        public MamdaFieldState  mOrigSeqNumFieldState        = new MamdaFieldState();
        public MamdaFieldState  mOrigPriceFieldState         = new MamdaFieldState();
        public MamdaFieldState  mOrigVolumeFieldState        = new MamdaFieldState();
        public MamdaFieldState  mOrigPartIdFieldState        = new MamdaFieldState ();
        public MamdaFieldState  mOrigQualStrFieldState       = new MamdaFieldState ();
        public MamdaFieldState  mOrigQualNativeStrFieldState = new MamdaFieldState ();
        public MamdaFieldState  mOrigSellersSaleDaysFieldState = new MamdaFieldState();
        public MamdaFieldState  mOrigStopStockIndFieldState  = new MamdaFieldState ();
        public MamdaFieldState  mCorrPriceFieldState         = new MamdaFieldState();
        public MamdaFieldState  mCorrVolumeFieldState        = new MamdaFieldState();
        public MamdaFieldState  mCorrPartIdFieldState        = new MamdaFieldState ();
        public MamdaFieldState  mCorrQualStrFieldState       = new MamdaFieldState ();
        public MamdaFieldState  mCorrQualNativeStrFieldState = new MamdaFieldState ();
        public MamdaFieldState  mCorrSellersSaleDaysFieldState = new MamdaFieldState();
        public MamdaFieldState  mCorrStopStockIndFieldState  = new MamdaFieldState ();
        public MamaChar         mCorrShortSaleCircuitBreaker = new MamaChar();
        public MamdaFieldState  mCorrTimeFieldState          = new MamdaFieldState ();
        public MamdaFieldState  mCancelTimeFieldState        = new MamdaFieldState ();
        public MamdaFieldState  mTradeUnitsFieldState        = new MamdaFieldState ();
        public MamdaFieldState  mLastSeqNumFieldState        = new MamdaFieldState();
        public MamdaFieldState  mHighSeqNumFieldState        = new MamdaFieldState();
        public MamdaFieldState  mLowSeqNumFieldState         = new MamdaFieldState();
        public MamdaFieldState  mTotalVolumeSeqNumFieldState = new MamdaFieldState();
        public MamdaFieldState  mCurrencyCodeFieldState      = new MamdaFieldState ();

        // Not in C++.
        //public MamdaFieldState      mTradeCondStrFieldState      = new MamdaFieldState ();
        public MamdaFieldState  mOrigCondStrFieldState       = new MamdaFieldState ();
        public MamdaFieldState  mCorrCondStrFieldState       = new MamdaFieldState ();
        public MamdaFieldState  mOrigShortSaleCircuitBreakerFieldState = new MamdaFieldState();
        public MamdaFieldState  mCorrShortSaleCircuitBreakerFieldState = new MamdaFieldState();       


        public MamdaFieldState  mConflateCountFieldState     = new MamdaFieldState();
        public MamdaFieldState  mTradeCondStrFieldState      = new MamdaFieldState();      
    }

    // Additional fields for gaps:
    private long                mGapBegin          = 0;
    private long                mGapEnd            = 0;

    private MamdaFieldState     mGapBeginFieldState          = new MamdaFieldState();
    private MamdaFieldState     mGapEndFieldState            = new MamdaFieldState();

    // Fields for closing reports:
    private final MamaBoolean   mIsIndicative                = new MamaBoolean();
    private MamdaFieldState     mIsIndicativeFieldState      = new MamdaFieldState();
    private final MamaLong      mTmpTradeCount               = new MamaLong();

    private MamaMsgField        tmpField                     = new MamaMsgField();

    /**
    * Create a specialized trade listener.  This listener handles
    * trade reports, trade recaps, trade errors/cancels, trade
    * corrections, and trade gap notifications.
    */
    public MamdaTradeListener ()
    {
        clearCache (mTradeCache);
    }

    public void printCache (MamdaTradeCache cache)
    {
        System.out.println ("mSymbol = "      + cache.mSymbol.getValue     ());
        System.out.println ("mTradePrice = "  + cache.mLastPrice.getValue  ());
        System.out.println ("mHighPrice = "   + cache.mHighPrice.getValue  ());
        System.out.println ("mLowPrice = "    + cache.mLowPrice.getValue   ());
        System.out.println ("mTradePrice = "  + cache.mTradePrice.getValue ());
        System.out.println ("mTradeVolume = " + cache.mTradeVolume.getValue());
        System.out.println ("mTradePartId = " + cache.mTradePartId.getValue());
        System.out.println ("mTradeId = "     + cache.mTradeId.getValue    ());
        System.out.println ("mOrigTradeId = " + cache.mOrigTradeId.getValue());
        System.out.println ("mCorrTradeId = " + cache.mCorrTradeId.getValue());
    }

    public void clearCache (MamdaTradeCache cache)
    {
        cache.mSymbol.setValue(null);
        cache.mIssueSymbol.setValue(null);
        cache.mPartId.setValue(null);
        cache.mSrcTime.clear(); 
        cache.mActTime.clear(); 
        cache.mLineTime.clear();
        cache.mSendTime.clear();
        cache.mPubId.setValue(null);
        cache.mIsIrregular.setValue(false);
        cache.mLastPrice.clear();
        cache.mLastVolume.setValue(0);
        cache.mLastPartId.setValue(null);  
        cache.mLastDate.clear();
        cache.mLastTime.clear();
        cache.mIrregPrice .clear();    
        cache.mIrregVolume.setValue(0);
        cache.mIrregPartId.setValue(null); 
        cache.mIrregTime.clear();          
        cache.mAccVolume.setValue(0);      
        cache.mOffExAccVolume.setValue(0); 
        cache.mOnExAccVolume.setValue(0);  
        cache.mTradeDirection.setValue(null); 
        cache.mTmpChar.setValue(' ');
        cache.mSide = "";
        cache.mTmpSide.setState((short)MamdaTradeSide.TRADE_SIDE_UNKNOWN);
        cache.mNetChange.clear();                 
        cache.mPctChange.setValue(0);             
        cache.mOpenPrice.clear();                 
        cache.mHighPrice.clear();                 
        cache.mLowPrice.clear();                  
        cache.mClosePrice.clear();                
        cache.mShortSaleCircuitBreaker.setValue(' ');
        cache.mOrigShortSaleCircuitBreaker.setValue(' ');
        cache.mCorrShortSaleCircuitBreaker.setValue(' ');
        cache.mPrevClosePrice.clear();            
        cache.mPrevCloseDate.clear();             
        cache.mAdjPrevClose.clear();              
        cache.mTradeCount = 0;                    
        cache.mBlockVolume.setValue(0);           
        cache.mBlockCount.setValue(0);            
        cache.mVwap.setValue(0);                  
        cache.mOffExVwap.setValue(0);             
        cache.mOnExVwap.setValue(0);              
        cache.mTotalValue.setValue(0);            
        cache.mOffExTotalValue.setValue(0);       
        cache.mOnExTotalValue.setValue(0);        
        cache.mStdDev.setValue(0);                
        cache.mStdDevSum.setValue(0);             
        cache.mStdDevSumSquares.setValue(0);      
        cache.mOrderId.setValue(0);               
        cache.mSettlePrice.clear();               
        cache.mSettleDate.clear();                
        cache.mEventSeqNum.setValue(0);           
        cache.mEventTime.clear();                 
        cache.mTradePrice.clear();                
        cache.mTradeVolume.setValue(0);           
        cache.mTradePartId.setValue(null);  
        cache.mTradeId.setValue(null);  
        cache.mOrigTradeId.setValue(null);  
        cache.mCorrTradeId.setValue(null);  
        cache.mTradeQualStr.setValue(null);       
        cache.mTradeQualNativeStr.setValue(null); 
        cache.mSellersSaleDays.setValue(0);       
        cache.mStopStockInd.setValue('0');        
        cache.mTradeExecVenue.setValue(null);     
        cache.mOffExTradePrice.clear();           
        cache.mOnExTradePrice.clear();            
        cache.mIsCancel = false;                  
        cache.mOrigSeqNum .setValue(0);           
        cache.mOrigPrice.clear();                 
        cache.mOrigVolume.setValue(0);            
        cache.mOrigPartId.setValue(null);         
        cache.mOrigQualStr.setValue(null);        
        cache.mOrigQualNativeStr.setValue(null);  
        cache.mOrigSellersSaleDays.setValue(0);   
        cache.mOrigStopStockInd.setValue('0');    
        cache.mCorrPrice.clear();                 
        cache.mCorrVolume.setValue(0);            
        cache.mCorrPartId .setValue(null);        
        cache.mCorrQualStr.setValue(null);        
        cache.mCorrQualNativeStr.setValue(null);  
        cache.mCorrSellersSaleDays.setValue(0);   
        cache.mCorrStopStockInd.setValue('0');
        cache.mCorrTime.clear();
        cache.mCancelTime.clear();                
        cache.mTradeCondStr.setValue(null);       
        cache.mOrigCondStr.setValue(null);        
        cache.mCorrCondStr.setValue(null);        
        cache.mTradeUnits.setValue(null);         
        cache.mLastSeqNum.setValue(0);            
        cache.mHighSeqNum.setValue(0);            
        cache.mLowSeqNum.setValue(0);             
        cache.mTotalVolumeSeqNum.setValue(0);     
        cache.mCurrencyCode.setValue(null);       
        cache.mConflateCount.setValue(0);         

        cache.mGotPartId               = false;
        cache.mLastGenericMsgWasTrade  = false;
        cache.mGotTradeTime            = false;
        cache.mGotTradePrice           = false;
        cache.mGotTradeSize            = false;
        cache.mGotTradeCount           = false;

        mProcessUpdateAsTrade.setValue(true);
        
        cache.mSymbolFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mIssueSymbolFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mPartIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mSrcTimeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mActTimeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mLineTimeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mSendTimeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mPubIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mIsIrregularFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mLastPriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mLastVolumeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mLastPartIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mLastDateFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mLastTimeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mIrregPriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mIrregVolumeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mIrregPartIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mIrregTimeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mAccVolumeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mShortSaleCircuitBreakerFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigShortSaleCircuitBreakerFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrShortSaleCircuitBreakerFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOffExAccVolumeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOnExAccVolumeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradeDirectionFieldState.setState(MamdaFieldState.NOT_INITIALISED);      
        cache.mSideFieldState.setState(MamdaFieldState.NOT_INITIALISED);      
        cache.mNetChangeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mPctChangeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOpenPriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mHighPriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mLowPriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mClosePriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mPrevClosePriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mPrevCloseDateFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mAdjPrevCloseFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradeCountFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mBlockVolumeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mBlockCountFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mVwapFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOffExVwapFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOnExVwapFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTotalValueFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOffExTotalValueFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOnExTotalValueFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mStdDevFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mStdDevSumFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mStdDevSumSquaresFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrderIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mSettlePriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mSettleDateFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mEventSeqNumFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mEventTimeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradePriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradeVolumeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradePartIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradeIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigTradeIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrTradeIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradeQualStrFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradeQualNativeStrFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mSellersSaleDaysFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mStopStockIndFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradeExecVenueFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOffExTradePriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOnExTradePriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mIsCancelFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigSeqNumFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigPriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigVolumeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigPartIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigQualStrFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigQualNativeStrFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigSellersSaleDaysFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigStopStockIndFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrPriceFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrVolumeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrPartIdFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrQualStrFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrQualNativeStrFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrSellersSaleDaysFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrStopStockIndFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrTimeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCancelTimeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradeCondStrFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mOrigCondStrFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCorrCondStrFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTradeUnitsFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mLastSeqNumFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mHighSeqNumFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mLowSeqNumFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mTotalVolumeSeqNumFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mCurrencyCodeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mConflateCountFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        mGapBeginFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        mGapEndFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        mIsIndicativeFieldState.setState(MamdaFieldState.NOT_INITIALISED);
    }

    /**
     * Add a specialized trade handler.  Currently, only one
     * handler can (and must) be registered.
     */
    public void addHandler (MamdaTradeHandler handler)
    {
        mHandlers.addLast(handler);
    }

    // Inherited from MamdaBasicRecap and MamdaBasicEvent: 

    public String getSymbol()
    {
        return mTradeCache.mSymbol.getValue();
    }

    public String getPartId()
    {
        return mTradeCache.mPartId.getValue();
    }

    /**
     * @see MamdaBasicEvent#getSrcTime()
     */
    public MamaDateTime getSrcTime()
    {
        return mTradeCache.mSrcTime;
    }

    /**
     * @see MamdaBasicEvent#getActivityTime()
     */
    public MamaDateTime getActivityTime()
    {
        return mTradeCache.mActTime;
    }

    public MamaDateTime getLineTime()
    {
        return mTradeCache.mLineTime;
    }

    public MamaDateTime getSendTime()
    {
        return mTradeCache.mSendTime;
    }

    public String getPubId()
    {
        return mTradeCache.mPubId.getValue();
    }

    /**
     * @see MamdaBasicEvent#getEventSeqNum()
     */
    public long getEventSeqNum()
    {
        return mTradeCache.mEventSeqNum.getValue();
    }

    /**
     * @see MamdaBasicEvent#getEventTime()
     */
    public MamaDateTime getEventTime()
    {
        return mTradeCache.mEventTime;
    }

    // Trade recap fields access:

    /**
     * @see MamdaTradeRecap#getLastPrice()
     */
    public MamaPrice getLastPrice()
    {
        return mTradeCache.mLastPrice;
    }

    /**
     * @see MamdaTradeRecap#getLastVolume()
     */
    public double getLastVolume()
    {
        return mTradeCache.mLastVolume.getValue();
    }

    public char getShortSaleCircuitBreaker()
    {
        return mTradeCache.mShortSaleCircuitBreaker.getValue();
    }

    public char getOrigShortSaleCircuitBreaker()
    {
        return mTradeCache.mOrigShortSaleCircuitBreaker.getValue();
    }
   
    public char getCorrShortSaleCircuitBreaker()
    {
        return mTradeCache.mCorrShortSaleCircuitBreaker.getValue();
    }

    /**
     * @see MamdaTradeRecap#getLastPartId()
     */
    public String getLastPartId()
    {
        return mTradeCache.mLastPartId.getValue();
    }

    /**
     * @see MamdaTradeRecap#getLastDate()
     */
    public MamaDateTime getLastDate()
    {
        return mTradeCache.mLastDate;
    }

    /**
     * @see MamdaTradeRecap#getLastTime()
     */
    public MamaDateTime getLastTime()
    {
        return mTradeCache.mLastTime;
    }

    /**
     * @see MamdaTradeRecap#getLastTime()
     */
    public MamaDateTime getLastDateTime()
    {
        MamaDateTime mLastDateTime = new MamaDateTime (mTradeCache.mLastTime, mTradeCache.mLastDate);

        return mLastDateTime;
    }

    /**
     * @see MamdaTradeRecap#getIrregPrice()
     */
    public MamaPrice getIrregPrice()
    {
        return mTradeCache.mIrregPrice;
    }

    /**
     * @see MamdaTradeRecap#getIrregVolume()
     */
    public double getIrregVolume()
    {
        return mTradeCache.mIrregVolume.getValue();
    }

    /**
     * @see MamdaTradeRecap#getIrregPartId()
     */
    public String getIrregPartId()
    {
        return mTradeCache.mIrregPartId.getValue();
    }

    /**
     * @see MamdaTradeRecap#getLastTime()
     */
    public MamaDateTime getIrregTime()
    {
        return mTradeCache.mIrregTime;
    }

    public MamaDateTime getTradeDate()
    {
        MamaDateTime mLastDateTime = new MamaDateTime (mTradeCache.mLastTime, mTradeCache.mLastDate);

        return mLastDateTime;
    }

    /**
     * @see MamdaTradeRecap#getAccVolume()
     */
    public double getAccVolume()
    {
        return mTradeCache.mAccVolume.getValue();
    }

    /**
     * @see MamdaTradeRecap#getOffExAccVolume()
     */
    public double getOffExAccVolume()
    {
        return mTradeCache.mOffExAccVolume.getValue();
    }

    /**
     * @see MamdaTradeRecap#getOnExAccVolume()
     */
    public double getOnExAccVolume()
    {
        return mTradeCache.mOnExAccVolume.getValue();
    }

    /**
     * @see MamdaTradeRecap#getNetChange()
     */
    public MamaPrice getNetChange()
    {
        return mTradeCache.mNetChange;
    }

    /**
     * @see MamdaTradeRecap#getPctChange()
     */
    public double getPctChange()
    {
        return mTradeCache.mPctChange.getValue();
    }

    /**
     * @see MamdaTradeRecap#getTradeDirection()
     */
    public String getTradeDirection()
    {
        return mTradeCache.mTradeDirection.getValue();
    }

    /**
     * @see MamdaTradeRecap#getSide()
     */
    public String getSide()
    {       
        return mTradeCache.mSide;
    }

    /**
     * @see MamdaTradeRecap#getOpenPrice()
     */
    public MamaPrice getOpenPrice()
    {
        return mTradeCache.mOpenPrice;
    }

    /**
     * @see MamdaTradeRecap#getHighPrice()
     */
    public MamaPrice getHighPrice()
    {
        return mTradeCache.mHighPrice;
    }

    /**
     * @see MamdaTradeRecap#getLowPrice()
     */
    public MamaPrice getLowPrice()
    {
        return mTradeCache.mLowPrice;
    }

    /**
     * @see MamdaTradeRecap#getClosePrice()
     */
    public MamaPrice getClosePrice()
    {
        return mTradeCache.mClosePrice;
    }

    /**
     * @see MamdaTradeRecap#getPrevClosePrice()
     */
    public MamaPrice getPrevClosePrice()
    {
        return mTradeCache.mPrevClosePrice;
    }

    /**
     * @see MamdaTradeRecap#getPrevCloseDate()
     */
    public MamaDateTime getPrevCloseDate()
    {
        return mTradeCache.mPrevCloseDate;
    }

    /**
     * @see MamdaTradeRecap#getAdjPrevClose()
     */
    public MamaPrice getAdjPrevClose()
    {
        return mTradeCache.mAdjPrevClose;
    }

    /**
     * @see MamdaTradeRecap#getBlockCount()
     */
    public long   getBlockCount()
    {
        return mTradeCache.mBlockCount.getValue();
    }

    /**
     * @see MamdaTradeRecap#getBlockVolume()
     */
    public double  getBlockVolume()
    {
        return mTradeCache.mBlockVolume.getValue();
    }

    /**
     * @see MamdaTradeRecap#getVwap()
     */
    public double getVwap()
    {
        return mTradeCache.mVwap.getValue();
    }

    /**
     * @see MamdaTradeRecap#getOffExVwap()
     */
    public double getOffExVwap()
    {
        return mTradeCache.mOffExVwap.getValue();
    }

    /**
     * @see MamdaTradeRecap#getOnExVwap()
     */
    public double getOnExVwap()
    {
        return mTradeCache.mOnExVwap.getValue();
    }

    /**
     * @see MamdaTradeRecap#getTotalValue()
     */
    public double getTotalValue()
    {
        return mTradeCache.mTotalValue.getValue();
    }

    /**
     * @see MamdaTradeRecap#getOffExTotalValue()
     */
    public double getOffExTotalValue()
    {
        return mTradeCache.mOffExTotalValue.getValue();
    }

    /**
     * @see MamdaTradeRecap#getOnExTotalValue()
     */
    public double getOnExTotalValue()
    {
        return mTradeCache.mOnExTotalValue.getValue();
    }

    /**
     * @see MamdaTradeRecap#getStdDev()
     */
    public double getStdDev()
    {
        return mTradeCache.mStdDev.getValue();
    }

    /**
     * @see MamdaTradeRecap#getStdDevSum()
     */
    public double getStdDevSum()
    {
        return mTradeCache.mStdDevSum.getValue();
    }

    /**
     * @see MamdaTradeRecap#getStdDevSumSquares()
     */
    public double getStdDevSumSquares()
    {
        return mTradeCache.mStdDevSumSquares.getValue();
    }

    /**
     * @see MamdaTradeRecap#getOrderId
     */
    public long getOrderId ()
    {
        return mTradeCache.mOrderId.getValue();
    }

    /**
     * @see MamdaTradeRecap#getSettlePrice()
     */
    public MamaPrice getSettlePrice()
    {
        return mTradeCache.mSettlePrice;
    }

    /**
     * @see MamdaTradeRecap#getSettleDate()
     */
    public MamaDateTime getSettleDate()
    {
        return mTradeCache.mSettleDate;
    }

    // Trade Report field access:
    /**
     * @see MamdaTradeReport#getTradePrice()
     */
    public MamaPrice getTradePrice()
    {
        return mTradeCache.mTradePrice;
    }

    /**
     * @see MamdaTradeReport#getTradeVolume()
     */
    public double getTradeVolume()
    {
        return mTradeCache.mTradeVolume.getValue();
    }

    /**
     * @see MamdaTradeReport#getTradePartId()
     */
    public String getTradePartId()
    {
        return mTradeCache.mTradePartId.getValue();
    }

    /**
     * @see MamdaTradeReport#getTradeId()
     */
    public String getTradeId()
    {
        return mTradeCache.mTradeId.getValue();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigTradeId()
     */
    public String getOrigTradeId()
    {
        return mTradeCache.mOrigTradeId.getValue();
    }

    /**
     * @see MamdaTradeCorrection#getCorrTradeId()
     */
    public String getCorrTradeId()
    {
        return mTradeCache.mCorrTradeId.getValue();
    }

    /**
     * @see MamdaTradeReport#getTradeQual()
     */
    public String getTradeQual()
    {
        return mTradeCache.mTradeQualStr.getValue();
    }

    /**
     * @see MamdaTradeReport#getTradeQualNativeStr()
     */
    public String getTradeQualNativeStr()
    {
        return mTradeCache.mTradeQualNativeStr.getValue();
    }

    /**
     * @see MamdaTradeReport#getTradeCondition()
     */
    public String getTradeCondition()
    {
        return mTradeCache.mTradeCondStr.getValue();
    }

    /**
     * @see MamdaTradeReport#getTradeSellersSaleDays()
     */
    public long getTradeSellersSaleDays()
    {
        return mTradeCache.mSellersSaleDays.getValue();
    }

    /**
     * @see MamdaTradeReport#getTradeStopStock()
     */
    public char getTradeStopStock()
    {
        return mTradeCache.mStopStockInd.getValue();
    }

    /**
     * @see MamdaTradeReport#gettradeExecVenue()
     */
    public String getTradeExecVenue()
    {
        return mTradeCache.mTradeExecVenue.getValue();
    }

    /**
     * @see MamdaTradeReport#geOffExchangetTradePrice()
     */
    public MamaPrice getOffExchangeTradePrice()
    {
        return mTradeCache.mOffExTradePrice;
    }

    /**
     * @see MamdaTradeReport#getOnExchangeTradePrice()
     */
    public MamaPrice getOnExchangeTradePrice()
    {
        return mTradeCache.mOnExTradePrice;
    }

    /**
     * @see MamdaTradeRecap#getTradeCount()
     */
    public long getTradeCount()
    {
        return mTradeCache.mTradeCount;
    }

    /**
     * @see MamdaTradeRecap#getTradeUnits()
     */
    public String getTradeUnits()
    {
        return mTradeCache.mTradeUnits.getValue();
    } 

    /**
     * @see MamdaTradeRecap#getLastSeqNum()
     */
    public long getLastSeqNum()
    {
        return mTradeCache.mLastSeqNum.getValue();
    }

    /**
     * @see MamdaTradeRecap#getHighSeqNum()
     */
    public long getHighSeqNum()
    {
        return mTradeCache.mHighSeqNum.getValue();
    }

    /**
     * @see MamdaTradeRecap#getLowSeqNum()
     */ 
    public long getLowSeqNum()
    {
        return mTradeCache.mLowSeqNum.getValue();
    }

    /**
     * @see MamdaTradeRecap#getTotalVolumeSeqNum()
     */
    public long getTotalVolumeSeqNum()
    {
        return mTradeCache.mTotalVolumeSeqNum.getValue();
    }

    /**
     * @see MamdaTradeRecap#getCurrencyCode()
     */
    public String getCurrencyCode()
    {
        return mTradeCache.mCurrencyCode.getValue();
    } 

    /**
     * @see MamdaTradeReport#getIsIrregular()
     */
    public boolean getIsIrregular()
    {
        return mTradeCache.mIsIrregular.getValue();
    }

    /**
     * @see MamdaTradeGap#getBeginGapSeqNum()
     */
    public long getBeginGapSeqNum()
    {
        return mGapBegin;
    }

    /**
     * @see MamdaTradeGap#getEndGapSeqNum()
     */
    public long getEndGapSeqNum()
    {
        return mGapEnd;
    }

    /**
     * @see MamdaTradeCancelOrError#getIsCancel()
     */
    public boolean getIsCancel()
    {
        return mTradeCache.mIsCancel;
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigSeqNum()
     */
    public long    getOrigSeqNum()
    {
        return mTradeCache.mOrigSeqNum.getValue();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigPrice()
     */
    public double  getOrigPrice()
    {
        return mTradeCache.mOrigPrice.getValue();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigVolume()
     */
    public double   getOrigVolume()
    {
        return mTradeCache.mOrigVolume.getValue();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigPartId()
     */
    public String  getOrigPartId()
    {
        return mTradeCache.mOrigPartId.getValue();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigQual()
     */
    public String  getOrigQual()
    {
        return mTradeCache.mOrigQualStr.getValue();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigQualNative()
     */
    public String getOrigQualNative()
    {
        return mTradeCache.mOrigQualNativeStr.getValue();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigCondition()
     */
    public String  getOrigCondition()
    {
        return mTradeCache.mOrigCondStr.getValue();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigSellersSaleDays()
     */
    public long    getOrigSellersSaleDays()
    {
        return mTradeCache.mOrigSellersSaleDays.getValue();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigStopStock()
     */
    public char    getOrigStopStock()
    {
        return mTradeCache.mOrigStopStockInd.getValue();
    }

    /**
     * @see MamdaTradeCorrection#getCorrPrice()
     */
    public double  getCorrPrice()
    {
        return mTradeCache.mCorrPrice.getValue();
    }

    /**
     * @see MamdaTradeCorrection#getCorrVolume()
     */
    public double   getCorrVolume()
    {
        return mTradeCache.mCorrVolume.getValue();
    }

    /**
     * @see MamdaTradeCorrection#getCorrPartId()
     */
    public String  getCorrPartId()
    {
        return mTradeCache.mCorrPartId.getValue();
    }

    /**
     * @see MamdaTradeCorrection#getCorrQual()
     */
    public String  getCorrQual()
    {
        return mTradeCache.mCorrQualStr.getValue();
    }

    /**
     * @see MamdaTradeCorrection#getCorrQualNative()
     */
    public String getCorrQualNative()
    {
        return mTradeCache.mCorrQualNativeStr.getValue();
    }

    /**
     * @see MamdaTradeCorrection#getCorrCondition()
     */
    public String  getCorrCondition()
    {
        return mTradeCache.mCorrCondStr.getValue();
    }

    /**
     * @see MamdaTradeCorrection#getCorrSellersSaleDays()
     */
    public long    getCorrSellersSaleDays()
    {
        return mTradeCache.mCorrSellersSaleDays.getValue();
    }

    /**
     * @see MamdaTradeCorrection#getCorrStopStock()
     */
    public char    getCorrStopStock()
    {
        return mTradeCache.mCorrStopStockInd.getValue();
    }

    /**
     * @see MamdaTradeClosing#getIsIndicative()
     */
    public boolean getIsIndicative()
    {
        return mIsIndicative.getValue();
    }    
    
    
/*      FieldState Accessors        */

    public short getSymbolFieldState()
    {
        return mTradeCache.mSymbolFieldState.getState();
    }

    public short getPartIdFieldState()
    {
        return mTradeCache.mPartIdFieldState.getState();
    }

    /**
     * @see MamdaBasicEvent#getSrcTimeFieldState()
     */
    public short getSrcTimeFieldState()
    {
        return mTradeCache.mSrcTimeFieldState.getState();
    }

    /**
     * @see MamdaBasicEvent#getActivityTimeFieldState()
     */
    public short getActivityTimeFieldState()
    {
        return mTradeCache.mActTimeFieldState.getState();
    }

    public short getLineTimeFieldState()
    {
        return mTradeCache.mLineTimeFieldState.getState();
    }

    public short getSendTimeFieldState()
    {
        return mTradeCache.mSendTimeFieldState.getState();
    }
         
    public short getShortSaleCircuitBreakerFieldState()
    {          
        return mTradeCache.mShortSaleCircuitBreakerFieldState.getState();
    }

    public short getOrigShortSaleCircuitBreakerFieldState()
    {          
        return mTradeCache.mOrigShortSaleCircuitBreakerFieldState.getState();
    }

    public short getCorrShortSaleCircuitBreakerFieldState()
    {          
        return mTradeCache.mCorrShortSaleCircuitBreakerFieldState.getState();
    }

    public short getPubIdFieldState()
    {
        return mTradeCache.mPubIdFieldState.getState();
    }

    /**
     * @see MamdaBasicEvent#getEventSeqNumFieldState()
     */
    public short getEventSeqNumFieldState()
    {
        return mTradeCache.mEventSeqNumFieldState.getState();
    }

    /**
     * @see MamdaBasicEvent#getEventTimeFieldState()
     */
    public short getEventTimeFieldState()
    {
        return mTradeCache.mEventTimeFieldState.getState();
    }

    // Trade recap fields access:

    /**
     * @see MamdaTradeRecap#getLastPriceFieldState()
     */
    public short getLastPriceFieldState()
    {
        return mTradeCache.mLastPriceFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getLastVolumeFieldState()
     */
    public short getLastVolumeFieldState()
    {
        return mTradeCache.mLastVolumeFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getLastPartIdFieldState()
     */
    public short getLastPartIdFieldState()
    {
        return mTradeCache.mLastPartIdFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getLastDateFieldState()
     */
    public short getLastDateFieldState()
    {
        return mTradeCache.mLastDateFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getLastTimeFieldState()
     */
    public short getLastTimeFieldState()
    {
        return mTradeCache.mLastTimeFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getIrregPriceFieldState()
     */
    public short getIrregPriceFieldState()
    {
        return mTradeCache.mIrregPriceFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getIrregVolumeFieldState()
     */
    public short getIrregVolumeFieldState()
    {
        return mTradeCache.mIrregVolumeFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getIrregPartIdFieldState()
     */
    public short getIrregPartIdFieldState()
    {
        return mTradeCache.mIrregPartIdFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getLastTimeFieldState()
     */
    public short getIrregTimeFieldState()
    {
        return mTradeCache.mIrregTimeFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getAccVolumeFieldState()
     */
    public short getAccVolumeFieldState()
    {
        return mTradeCache.mAccVolumeFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getOffExAccVolumeFieldState()
     */
    public short getOffExAccVolumeFieldState()
    {
        return mTradeCache.mOffExAccVolumeFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getOnExAccVolumeFieldState()
     */
    public short getOnExAccVolumeFieldState()
    {
        return mTradeCache.mOnExAccVolumeFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getNetChangeFieldState()
     */
    public short getNetChangeFieldState()
    {
        return mTradeCache.mNetChangeFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getPctChangeFieldState()
     */
    public short getPctChangeFieldState()
    {
        return mTradeCache.mPctChangeFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getTradeDirectionFieldState()
     */
    public short getTradeDirectionFieldState()
    {
        return mTradeCache.mTradeDirectionFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getSideFieldState()
     */
    public short getSideFieldState()
    {      
        return mTradeCache.mSideFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getOpenPriceFieldState()
     */
    public short getOpenPriceFieldState()
    {
        return mTradeCache.mOpenPriceFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getHighPriceFieldState()
     */
    public short getHighPriceFieldState()
    {
        return mTradeCache.mHighPriceFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getLowPriceFieldState()
     */
    public short getLowPriceFieldState()
    {
        return mTradeCache.mLowPriceFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getClosePriceFieldState()
     */
    public short getClosePriceFieldState()
    {
        return mTradeCache.mClosePriceFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getPrevClosePriceFieldState()
     */
    public short getPrevClosePriceFieldState()
    {
        return mTradeCache.mPrevClosePriceFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getPrevCloseDateFieldState()
     */
    public short getPrevCloseDateFieldState()
    {
        return mTradeCache.mPrevCloseDateFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getAdjPrevCloseFieldState()
     */
    public short getAdjPrevCloseFieldState()
    {
        return mTradeCache.mAdjPrevCloseFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getBlockCountFieldState()
     */
    public short   getBlockCountFieldState()
    {
        return mTradeCache.mBlockCountFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getBlockVolumeFieldState()
     */
    public short  getBlockVolumeFieldState()
    {
        return mTradeCache.mBlockVolumeFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getVwapFieldState()
     */
    public short getVwapFieldState()
    {
        return mTradeCache.mVwapFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getOffExVwapFieldState()
     */
    public short getOffExVwapFieldState()
    {
        return mTradeCache.mOffExVwapFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getOnExVwapFieldState()
     */
    public short getOnExVwapFieldState()
    {
        return mTradeCache.mOnExVwapFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getTotalValueFieldState()
     */
    public short getTotalValueFieldState()
    {
        return mTradeCache.mTotalValueFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getOffExTotalValueFieldState()
     */
    public short getOffExTotalValueFieldState()
    {
        return mTradeCache.mOffExTotalValueFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getOnExTotalValueFieldState()
     */
    public short getOnExTotalValueFieldState()
    {
        return mTradeCache.mOnExTotalValueFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getStdDevFieldState()
     */
    public short getStdDevFieldState()
    {
        return mTradeCache.mStdDevFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getStdDevSumFieldState()
     */
    public short getStdDevSumFieldState()
    {
        return mTradeCache.mStdDevSumFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getStdDevSumSquaresFieldState()
     */
    public short getStdDevSumSquaresFieldState()
    {
        return mTradeCache.mStdDevSumSquaresFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getOrderId
     */
    public short getOrderIdFieldState()
    {
        return mTradeCache.mOrderIdFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getSettlePriceFieldState()
     */
    public short getSettlePriceFieldState()
    {
        return mTradeCache.mSettlePriceFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getSettleDateFieldState()
     */
    public short getSettleDateFieldState()
    {
        return mTradeCache.mSettleDateFieldState.getState();
    }

    // Trade Report field access:

    /**
     * @see MamdaTradeReport#getTradePriceFieldState()
     */
    public short getTradePriceFieldState()
    {
        return mTradeCache.mTradePriceFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#getTradeVolumeFieldState()
     */
    public short getTradeVolumeFieldState()
    {
        return mTradeCache.mTradeVolumeFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#getTradePartIdFieldState()
     */
    public short getTradePartIdFieldState()
    {
        return mTradeCache.mTradePartIdFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#getTradeIdFieldState()
     */
    public short getTradeIdFieldState()
    {
        return mTradeCache.mTradeIdFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigTradeIdFieldState()
     */
    public short getOrigTradeIdFieldState()
    {
        return mTradeCache.mOrigTradeIdFieldState.getState();
    }

    /**
     * @see MamdaTradeCorrection#getCorrTradeIdFieldState()
     */
    public short getCorrTradeIdFieldState()
    {
        return mTradeCache.mCorrTradeIdFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#getTradeQualFieldState()
     */
    public short getTradeQualFieldState()
    {
        return mTradeCache.mTradeQualStrFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#getTradeQualNativeStrFieldState()
     */
    public short getTradeQualNativeStrFieldState()
    {
        return mTradeCache.mTradeQualNativeStrFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#getTradeConditionFieldState()
     */
    public short getTradeConditionFieldState()
    {
        return mTradeCache.mTradeCondStrFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#getTradeSellersSaleDaysFieldState()
     */
    public short getTradeSellersSaleDaysFieldState()
    {
        return mTradeCache.mSellersSaleDaysFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#getTradeStopStockFieldState()
     */
    public short getTradeStopStockFieldState()
    {
        return mTradeCache.mStopStockIndFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#gettradeExecVenueFieldState()
     */
    public short getTradeExecVenueFieldState()
    {
        return mTradeCache.mTradeExecVenueFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#geOffExchangetTradePriceFieldState()
     */
    public short getOffExchangeTradePriceFieldState()
    {
        return mTradeCache.mOffExTradePriceFieldState.getState();
    }

    /**
     * @see MamdaTradeReport#getOnExchangeTradePriceFieldState()
     */
    public short getOnExchangeTradePriceFieldState()
    {
        return mTradeCache.mOnExTradePriceFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getTradeCountFieldState()
     */
    public short getTradeCountFieldState()
    {
        return mTradeCache.mTradeCountFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getTradeUnitsFieldState()
     */
    public short getTradeUnitsFieldState()
    {
        return mTradeCache.mTradeUnitsFieldState.getState();
    } 

    /**
     * @see MamdaTradeRecap#getLastSeqNumFieldState()
     */
    public short getLastSeqNumFieldState()
    {
        return mTradeCache.mLastSeqNumFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getHighSeqNumFieldState()
     */
    public short getHighSeqNumFieldState()
    {
        return mTradeCache.mHighSeqNumFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getLowSeqNumFieldState()
     */ 
    public short getLowSeqNumFieldState()
    {
        return mTradeCache.mLowSeqNumFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getTotalVolumeSeqNumFieldState()
     */
    public short getTotalVolumeSeqNumFieldState()
    {
        return mTradeCache.mTotalVolumeSeqNumFieldState.getState();
    }

    /**
     * @see MamdaTradeRecap#getCurrencyCodeFieldState()
     */
    public short getCurrencyCodeFieldState()
    {
        return mTradeCache.mCurrencyCodeFieldState.getState();
    } 

    /**
     * @see MamdaTradeReport#getIsIrregularFieldState()
     */
    public short getIsIrregularFieldState()
    {
        return mTradeCache.mIsIrregularFieldState.getState();
    }

    /**
     * @see MamdaTradeGap#getBeginGapSeqNumFieldState()
     */
    public short getBeginGapSeqNumFieldState()
    {
        return mGapBeginFieldState.getState();
    }

    /**
     * @see MamdaTradeGap#getEndGapSeqNumFieldState()
     */
    public short   getEndGapSeqNumFieldState()
    {
        return mGapEndFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getIsCancelFieldState()
     */
    public short getIsCancelFieldState()
    {
        return mTradeCache.mIsCancelFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigSeqNumFieldState()
     */
    public short    getOrigSeqNumFieldState()
    {
        return mTradeCache.mOrigSeqNumFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigPriceFieldState()
     */
    public short  getOrigPriceFieldState()
    {
        return mTradeCache.mOrigPriceFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigVolumeFieldState()
     */
    public short   getOrigVolumeFieldState()
    {
        return mTradeCache.mOrigVolumeFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigPartIdFieldState()
     */
    public short  getOrigPartIdFieldState()
    {
        return mTradeCache.mOrigPartIdFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigQualFieldState()
     */
    public short  getOrigQualFieldState()
    {
        return mTradeCache.mOrigQualStrFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigQualNativeFieldState()
     */
    public short getOrigQualNativeFieldState()
    {
        return mTradeCache.mOrigQualNativeStrFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigConditionFieldState()
     */
    public short  getOrigConditionFieldState()
    {
        return mTradeCache.mOrigCondStrFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigSellersSaleDaysFieldState()
     */
    public short    getOrigSellersSaleDaysFieldState()
    {
        return mTradeCache.mOrigSellersSaleDaysFieldState.getState();
    }

    /**
     * @see MamdaTradeCancelOrError#getOrigStopStockFieldState()
     */
    public short    getOrigStopStockFieldState()
    {
        return mTradeCache.mOrigStopStockIndFieldState.getState();
    }

    /**
     * @see MamdaTradeCorrection#getCorrPriceFieldState()
     */
    public short  getCorrPriceFieldState()
    {
        return mTradeCache.mCorrPriceFieldState.getState();
    }

    /**
     * @see MamdaTradeCorrection#getCorrVolumeFieldState()
     */
    public short   getCorrVolumeFieldState()
    {
        return mTradeCache.mCorrVolumeFieldState.getState();
    }

    /**
     * @see MamdaTradeCorrection#getCorrPartIdFieldState()
     */
    public short  getCorrPartIdFieldState()
    {
        return mTradeCache.mCorrPartIdFieldState.getState();
    }

    /**
     * @see MamdaTradeCorrection#getCorrQualFieldState()
     */
    public short  getCorrQualFieldState()
    {
        return mTradeCache.mCorrQualStrFieldState.getState();
    }

    /**
     * @see MamdaTradeCorrection#getCorrQualNativeFieldState()
     */
    public short getCorrQualNativeFieldState()
    {
        return mTradeCache.mCorrQualNativeStrFieldState.getState();
    }

    /**
     * @see MamdaTradeCorrection#getCorrConditionFieldState()
     */
    public short  getCorrConditionFieldState()
    {
        return mTradeCache.mCorrCondStrFieldState.getState();
    }

    /**
     * @see MamdaTradeCorrection#getCorrSellersSaleDaysFieldState()
     */
    public short    getCorrSellersSaleDaysFieldState()
    {
        return mTradeCache.mCorrSellersSaleDaysFieldState.getState();
    }

    /**
     * @see MamdaTradeCorrection#getCorrStopStockFieldState()
     */
    public short    getCorrStopStockFieldState()
    {
        return mTradeCache.mCorrStopStockIndFieldState.getState();
    }

    /**
     * @see MamdaTradeClosing#getIsIndicativeFieldState()
     */
    public short getIsIndicativeFieldState()
    {
        return mIsIndicativeFieldState.getState();
    }

    /*      End FieldState Accessors        */

    /**
     * Implementation of MamdaListener interface.
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
                if (!MamdaTradeFields.isSet())
                {
                    mLogger.warning ("MamdaTradeFields::setDictionary() has not been called.");
                    return;
                }
                if (!mUpdatersComplete)
                {
                    createUpdaters ();
                    mUpdatersComplete = true;
                }
            }
        }

        // If msg is a trade-related message, invoke the
        // appropriate callback:
        mTradeCache.mWasIrregular = mTradeCache.mIsIrregular.getValue();

        switch (msgType)
        {
            case MamaMsgType.TYPE_INITIAL:
            case MamaMsgType.TYPE_RECAP:
            case MamaMsgType.TYPE_PREOPENING:
                handleRecap (subscription, msg);
                break;

            case MamaMsgType.TYPE_TRADE:
                handleTrade (subscription, msg);
                break;

            case MamaMsgType.TYPE_CANCEL:
                handleCancelOrError (subscription, msg, true);
                break;

            case MamaMsgType.TYPE_ERROR:
                handleCancelOrError (subscription, msg, false);
                break;

            case MamaMsgType.TYPE_CORRECTION:
                handleCorrection (subscription, msg);
                break;

            case MamaMsgType.TYPE_CLOSING:
                handleClosing (subscription, msg);
                break;

            case MamaMsgType.TYPE_UPDATE:
                /*We may not be attempting to process updates as trades*/
                if (mProcessUpdateAsTrade.getValue())
                {
                    handleUpdate (subscription, msg);
                }
                break;
        }
    }

    private void handleRecap (MamdaSubscription  subscription,
                              MamaMsg            msg)
    {
        updateFieldStates();
        updateTradeFields (msg);

        mTradeCache.mIsIrregular.setValue(mTradeCache.mWasIrregular);
        mTradeCache.mIsIrregularFieldState.setState(MamdaFieldState.MODIFIED);

        checkTradeCount (subscription, msg, false);

        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaTradeHandler handler = (MamdaTradeHandler) i.next();
            handler.onTradeRecap (subscription, this, msg, this);
        }
    }

    private void handleTrade (MamdaSubscription  subscription,
                              MamaMsg            msg)
    {
        updateFieldStates();
        updateTradeFields (msg);

        checkTradeCount (subscription, msg, true);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }

        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaTradeHandler handler = (MamdaTradeHandler) i.next();
            handler.onTradeReport (subscription, this, msg, this, this);
        }
    }

    private void handleCancelOrError (MamdaSubscription  subscription,
                                      MamaMsg            msg,
                                      boolean            isCancel)
    {
        updateFieldStates();
        updateTradeFields (msg);
        mTradeCache.mIsCancel = isCancel;
        mTradeCache.mIsCancelFieldState.setState (MamdaFieldState.MODIFIED);

        checkTradeCount (subscription, msg, true);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }

        if (!mTradeCache.mCancelTime.isEmpty())
        {
            mTradeCache.mEventTime = mTradeCache.mCancelTime;
            mTradeCache.mEventTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }

        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaTradeHandler handler = (MamdaTradeHandler) i.next();
            handler.onTradeCancelOrError (subscription,this,msg,this,this);
        }
    }

    private void handleCorrection (MamdaSubscription  subscription,
                                   MamaMsg            msg)
    {
        updateFieldStates();  
        updateTradeFields (msg);
        checkTradeCount (subscription, msg, true);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }

        if (!mTradeCache.mCorrTime.isEmpty())
        {
            mTradeCache.mEventTime = mTradeCache.mCorrTime;
            mTradeCache.mEventTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }

        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaTradeHandler handler = (MamdaTradeHandler) i.next();
            handler.onTradeCorrection (subscription, this, msg, this, this);
        }
    }

    private void handleUpdate (MamdaSubscription  subscription,
                               MamaMsg            msg)
    {
        updateFieldStates();
        updateTradeFields (msg);
        checkTradeCount (subscription, msg, true);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }

        if (mTradeCache.mLastGenericMsgWasTrade)
        {
            Iterator i = mHandlers.iterator();
            while (i.hasNext())
            {
                MamdaTradeHandler handler = (MamdaTradeHandler) i.next();
                handler.onTradeReport (subscription, this, msg, this, this);
            }
        }
    }

    private void handleClosing (MamdaSubscription  subscription,
                                MamaMsg            msg)
    {
        updateFieldStates();
        updateTradeFields (msg);
        checkTradeCount (subscription, msg, true);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }

        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaTradeHandler handler = (MamdaTradeHandler) i.next();
            handler.onTradeClosing (subscription, this, msg, this, this);
        }
    }

    private void getTradeFields (MamaMsg msg)
    {
        int i =0;
        while (mUpdaters[i] != null)
        {
            mUpdaters[i++].onUpdate(msg, MamdaTradeListener.this);
        }
    }

    private void updateFieldStates ()
    {
        if (mTradeCache.mSymbolFieldState.getState() ==  MamdaFieldState.MODIFIED)
            mTradeCache.mSymbolFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mIssueSymbolFieldState.getState() == MamdaFieldState.MODIFIED)
            mTradeCache.mIssueSymbolFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mPartIdFieldState.getState() == MamdaFieldState.MODIFIED) 
            mTradeCache.mPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mSrcTimeFieldState.getState() == MamdaFieldState.MODIFIED)      
            mTradeCache.mSrcTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mActTimeFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mActTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mLineTimeFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mLineTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mSendTimeFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mSendTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mPubIdFieldState.getState() == MamdaFieldState.MODIFIED)       
            mTradeCache.mPubIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mIsIrregularFieldState.getState() == MamdaFieldState.MODIFIED)
            mTradeCache.mIsIrregularFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mLastPriceFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mLastPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mLastVolumeFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mLastVolumeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mLastPartIdFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mLastPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mLastDateFieldState.getState() == MamdaFieldState.MODIFIED)     
            mTradeCache.mLastDateFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mLastTimeFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mLastTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mIrregPriceFieldState.getState() == MamdaFieldState.MODIFIED) 
            mTradeCache.mIrregPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mIrregVolumeFieldState.getState() == MamdaFieldState.MODIFIED)  
            mTradeCache.mIrregVolumeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mIrregPartIdFieldState.getState() == MamdaFieldState.MODIFIED)
            mTradeCache.mIrregPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mIrregTimeFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mIrregTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mAccVolumeFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mAccVolumeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOffExAccVolumeFieldState.getState() == MamdaFieldState.MODIFIED)
            mTradeCache.mOffExAccVolumeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOnExAccVolumeFieldState.getState() == MamdaFieldState.MODIFIED)     
            mTradeCache.mOnExAccVolumeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTradeDirectionFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mTradeDirectionFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mNetChangeFieldState.getState() == MamdaFieldState.MODIFIED)         
            mTradeCache.mNetChangeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mPctChangeFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mPctChangeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOpenPriceFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mOpenPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mHighPriceFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mHighPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mLowPriceFieldState.getState() == MamdaFieldState.MODIFIED)     
            mTradeCache.mLowPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mClosePriceFieldState.getState() == MamdaFieldState.MODIFIED)  
            mTradeCache.mClosePriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mPrevClosePriceFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mPrevClosePriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mPrevCloseDateFieldState.getState() == MamdaFieldState.MODIFIED)     
            mTradeCache.mPrevCloseDateFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mAdjPrevCloseFieldState.getState() == MamdaFieldState.MODIFIED)     
            mTradeCache.mAdjPrevCloseFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTradeCountFieldState.getState() == MamdaFieldState.MODIFIED)      
            mTradeCache.mTradeCountFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mBlockVolumeFieldState.getState() == MamdaFieldState.MODIFIED)       
            mTradeCache.mBlockVolumeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mBlockCountFieldState.getState() == MamdaFieldState.MODIFIED)     
            mTradeCache.mBlockCountFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mVwapFieldState.getState() == MamdaFieldState.MODIFIED)          
            mTradeCache.mVwapFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOffExVwapFieldState.getState() == MamdaFieldState.MODIFIED)         
            mTradeCache.mOffExVwapFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOnExVwapFieldState.getState() == MamdaFieldState.MODIFIED)     
            mTradeCache.mOnExVwapFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTotalValueFieldState.getState() == MamdaFieldState.MODIFIED)  
            mTradeCache.mTotalValueFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOffExTotalValueFieldState.getState() == MamdaFieldState.MODIFIED)  
            mTradeCache.mOffExTotalValueFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOnExTotalValueFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mOnExTotalValueFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mStdDevFieldState.getState() == MamdaFieldState.MODIFIED)            
            mTradeCache.mStdDevFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mStdDevSumFieldState.getState() == MamdaFieldState.MODIFIED) 
            mTradeCache.mStdDevSumFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mStdDevSumSquaresFieldState.getState() == MamdaFieldState.MODIFIED)  
            mTradeCache.mStdDevSumSquaresFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrderIdFieldState.getState() == MamdaFieldState.MODIFIED)           
            mTradeCache.mOrderIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mSettlePriceFieldState.getState() == MamdaFieldState.MODIFIED)       
            mTradeCache.mSettlePriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mSettleDateFieldState.getState() == MamdaFieldState.MODIFIED)     
            mTradeCache.mSettleDateFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mEventSeqNumFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mEventSeqNumFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mEventTimeFieldState.getState() == MamdaFieldState.MODIFIED)      
            mTradeCache.mEventTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTradePriceFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mTradePriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTradeVolumeFieldState.getState() == MamdaFieldState.MODIFIED)       
            mTradeCache.mTradeVolumeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTradePartIdFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mTradePartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTradeIdFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mTradeIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrigTradeIdFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mOrigTradeIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCorrTradeIdFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mCorrTradeIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTradeQualStrFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mTradeQualStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTradeQualNativeStrFieldState.getState() == MamdaFieldState.MODIFIED)
            mTradeCache.mTradeQualNativeStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mSellersSaleDaysFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mSellersSaleDaysFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mStopStockIndFieldState.getState() == MamdaFieldState.MODIFIED)      
            mTradeCache.mStopStockIndFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTradeExecVenueFieldState.getState() == MamdaFieldState.MODIFIED)  
            mTradeCache.mTradeExecVenueFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOffExTradePriceFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mOffExTradePriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOnExTradePriceFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mOnExTradePriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mIsCancelFieldState.getState() == MamdaFieldState.MODIFIED)      
            mTradeCache.mIsCancelFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrigSeqNumFieldState.getState() == MamdaFieldState.MODIFIED) 
            mTradeCache.mOrigSeqNumFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrigPriceFieldState.getState() == MamdaFieldState.MODIFIED)      
            mTradeCache.mOrigPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrigVolumeFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mOrigVolumeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrigPartIdFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mOrigPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrigQualStrFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mOrigQualStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrigQualNativeStrFieldState.getState() == MamdaFieldState.MODIFIED) 
            mTradeCache.mOrigQualNativeStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrigSellersSaleDaysFieldState.getState() == MamdaFieldState.MODIFIED) 
            mTradeCache.mOrigSellersSaleDaysFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrigStopStockIndFieldState.getState() == MamdaFieldState.MODIFIED)      
            mTradeCache.mOrigStopStockIndFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCorrPriceFieldState.getState() == MamdaFieldState.MODIFIED)           
            mTradeCache.mCorrPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCorrVolumeFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mCorrVolumeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCorrPartIdFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mCorrPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCorrQualStrFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mCorrQualStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCorrQualNativeStrFieldState.getState() == MamdaFieldState.MODIFIED)
            mTradeCache.mCorrQualNativeStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCorrSellersSaleDaysFieldState.getState() == MamdaFieldState.MODIFIED)  
            mTradeCache.mCorrSellersSaleDaysFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCorrStopStockIndFieldState.getState() == MamdaFieldState.MODIFIED)      
            mTradeCache.mCorrStopStockIndFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCorrTimeFieldState.getState() == MamdaFieldState.MODIFIED)          
            mTradeCache.mCorrTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCancelTimeFieldState.getState() == MamdaFieldState.MODIFIED)          
            mTradeCache.mCancelTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTradeCondStrFieldState.getState() == MamdaFieldState.MODIFIED)  
             mTradeCache.mTradeCondStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mOrigCondStrFieldState.getState() == MamdaFieldState.MODIFIED)     
           mTradeCache.mOrigCondStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCorrCondStrFieldState.getState() == MamdaFieldState.MODIFIED)    
           mTradeCache.mCorrCondStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mShortSaleCircuitBreakerFieldState.getState() == MamdaFieldState.MODIFIED) 
            mTradeCache.mShortSaleCircuitBreakerFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
        if (mTradeCache.mOrigShortSaleCircuitBreakerFieldState.getState() == MamdaFieldState.MODIFIED) 
            mTradeCache.mOrigShortSaleCircuitBreakerFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
        if (mTradeCache.mCorrShortSaleCircuitBreakerFieldState.getState() == MamdaFieldState.MODIFIED) 
            mTradeCache.mCorrShortSaleCircuitBreakerFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
        if (mTradeCache.mTradeUnitsFieldState.getState() == MamdaFieldState.MODIFIED)     
            mTradeCache.mTradeUnitsFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mLastSeqNumFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mLastSeqNumFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mHighSeqNumFieldState.getState() == MamdaFieldState.MODIFIED)    
            mTradeCache.mHighSeqNumFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mLowSeqNumFieldState.getState() == MamdaFieldState.MODIFIED)             
            mTradeCache.mLowSeqNumFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mTotalVolumeSeqNumFieldState.getState() == MamdaFieldState.MODIFIED)
            mTradeCache.mTotalVolumeSeqNumFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mCurrencyCodeFieldState.getState() == MamdaFieldState.MODIFIED)         
            mTradeCache.mCurrencyCodeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mTradeCache.mConflateCountFieldState.getState() == MamdaFieldState.MODIFIED)   
            mTradeCache.mConflateCountFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mGapBeginFieldState.getState() == MamdaFieldState.MODIFIED)     
            mGapBeginFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mGapEndFieldState.getState() == MamdaFieldState.MODIFIED)     
            mGapEndFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (mIsIndicativeFieldState.getState() == MamdaFieldState.MODIFIED)     
            mIsIndicativeFieldState.setState(MamdaFieldState.NOT_MODIFIED);

    }   
        
    private void updateTradeFields (MamaMsg  msg)
    {   
        mTradeCache.mGotPartId                = false;
        mTradeCache.mLastGenericMsgWasTrade   = false;
        mTradeCache.mGotTradeTime             = false;
        mTradeCache.mGotTradePrice            = false;
        mTradeCache.mGotTradeSize             = false;
        mTradeCache.mGotTradeCount            = false;
        mTradeCache.mGotIssueSymbol           = false;
        
        synchronized (this)
        {
            // Iterate over all of the fields in the message.
            getTradeFields (msg);
        }
        
        if (mTradeCache.mGotIssueSymbol)
        {
            mTradeCache.mSymbol.setValue(mTradeCache.mIssueSymbol.getValue());
            mTradeCache.mSymbolFieldState.setState(MamdaFieldState.MODIFIED);
        }
        
        // Check certain special fields.
        if (mTradeCache.mIsIrregular.getValue())
        {
            // This is an irregular trade (does not update "last" fields).
            mTradeCache.mTradePrice = mTradeCache.mIrregPrice;
            mTradeCache.mTradePriceFieldState.setState(MamdaFieldState.MODIFIED);
            mTradeCache.mTradeVolume.setValue(mTradeCache.mIrregVolume.getValue());
            mTradeCache.mTradeVolumeFieldState.setState(MamdaFieldState.MODIFIED);
            mTradeCache.mTradePartId.setValue(mTradeCache.mIrregPartId.getValue());
            mTradeCache.mTradePartIdFieldState.setState(MamdaFieldState.MODIFIED);
            if(mTradeCache.mIrregTime.isEmpty())
            {
                // Some feeds only use the one time field
                mTradeCache.mEventTime    = mTradeCache.mLastTime;
                mTradeCache.mEventTimeFieldState.setState(MamdaFieldState.MODIFIED);
            }
            else
            {
                mTradeCache.mEventTime    = mTradeCache.mIrregTime;
                mTradeCache.mEventTimeFieldState.setState(MamdaFieldState.MODIFIED);
            }
        }
        else
        {
            // This is a regular trade
            mTradeCache.mTradePrice  = mTradeCache.mLastPrice;
            mTradeCache.mTradePriceFieldState.setState(MamdaFieldState.MODIFIED);
            mTradeCache.mTradeVolume.setValue(mTradeCache.mLastVolume.getValue());
            mTradeCache.mTradeVolumeFieldState.setState(MamdaFieldState.MODIFIED);
            mTradeCache.mTradePartId.setValue(mTradeCache.mLastPartId.getValue());
            mTradeCache.mTradePartIdFieldState.setState(MamdaFieldState.MODIFIED);
            mTradeCache.mEventTime   = mTradeCache.mLastTime;
            mTradeCache.mEventTimeFieldState.setState(MamdaFieldState.MODIFIED);
        }
        
        if (mTradeCache.mGotPartId == false)
        {
            // No explicit part ID in message, but maybe in symbol.
            if (mTradeCache.mSymbol.getValue() != null)
            {
                int lastDot = mTradeCache.mSymbol.getValue().indexOf (".");
                if (lastDot != -1)
                {
                    lastDot++;
                    if (lastDot != mTradeCache.mSymbol.getValue().length ())
                    {
                        mTradeCache.mPartId.setValue(
                            mTradeCache.mSymbol.getValue().substring (lastDot));
                        mTradeCache.mPartIdFieldState.setState(MamdaFieldState.MODIFIED);
                        mTradeCache.mGotPartId = true;
                    }
                }
            }
        }
        
        if (mTradeCache.mGotTradeTime || mTradeCache.mGotTradePrice 
            || mTradeCache.mGotTradeSize)
        {
            mTradeCache.mLastGenericMsgWasTrade = true;
        }
    }   
        
    private void checkTradeCount (MamdaSubscription  subscription,
                                  MamaMsg            msg,
                                  boolean            checkForGap)
    {   
        // Check number of trades for gaps
        long tradeCount = mTmpTradeCount.getValue();
        long conflateCount = 0;
        mIgnoreUpdate = false;
        
        if (!msg.tryU32 ("wConflateTradeCount", 24,
                          mTradeCache.mConflateCount))
        {
            conflateCount = 1;
            mTradeCache.mConflateCountFieldState.setState (MamdaFieldState.MODIFIED);
        }
        else
        {
            conflateCount = mTradeCache.mConflateCount.getValue();
            mTradeCache.mConflateCountFieldState.setState (MamdaFieldState.MODIFIED);
        }

        if (checkForGap && mTradeCache.mGotTradeCount)
        {
            if ((mTradeCache.mTradeCount > 0)
                && (tradeCount > (mTradeCache.mTradeCount + conflateCount)))
            {
                mGapBegin = mTradeCache.mTradeCount+conflateCount;
                mGapBeginFieldState.setState(MamdaFieldState.MODIFIED);
                mGapEnd   = tradeCount-1;
                mGapEndFieldState.setState(MamdaFieldState.MODIFIED);
                mTradeCache.mTradeCount=tradeCount;
                mTradeCache.mTradeCountFieldState.setState(MamdaFieldState.MODIFIED);
                Iterator i = mHandlers.iterator();
                while (i.hasNext())
                {
                    MamdaTradeHandler handler = (MamdaTradeHandler) i.next();
                    handler.onTradeGap (subscription, this, msg, this, this);
                }
            }
        }

        /* Check for duplicates.  Only check if tradecount was actually present 
           in the message, in case it isn't being sent down. */
        if ( mTradeCache.mGotTradeCount && 
             tradeCount > 0 &&
             (tradeCount == mTradeCache.mTradeCount) )
        {
            mIgnoreUpdate = true;
        }

        mTradeCache.mTradeCount=tradeCount;
        mTradeCache.mTradeCountFieldState.setState (MamdaFieldState.MODIFIED);
    }

    private interface TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener);
    }

    private static class MamdaTradeSymbol implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (listener.mTradeCache.mSymbol.getValue() == null)
            {
                msg.tryString (null, MamdaCommonFields.SYMBOL.getFid(), listener.mTradeCache.mSymbol);
                listener.mTradeCache.mSymbolFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class MamdaTradeIssueSymbol implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if(msg.tryString (null, MamdaCommonFields.ISSUE_SYMBOL.getFid(), listener.mTradeCache.mIssueSymbol))
            {
                listener.mTradeCache.mGotIssueSymbol = true;
                listener.mTradeCache.mIssueSymbolFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class TradeLastPartId implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.TRADE_PART_ID.getFid(), listener.mTradeCache.mLastPartId))
            {
                listener.mTradeCache.mIsIrregular.setValue(false);
                listener.mTradeCache.mIsIrregularFieldState.setState (MamdaFieldState.MODIFIED);
                listener.mTradeCache.mLastPartIdFieldState.setState  (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class TradePartId implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaCommonFields.PART_ID.getFid(), listener.mTradeCache.mPartId))
            {
                listener.mTradeCache.mGotPartId = true;
                listener.mTradeCache.mIsIrregular.setValue(false);
                listener.mTradeCache.mPartIdFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }
 
    private static class TradeUpdateAsTrade implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            msg.tryBoolean (null, MamdaTradeFields.UPDATE_AS_TRADE.getFid(), listener.mProcessUpdateAsTrade);   
        }
    }

    private static class TradeSrcTime implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime (null, MamdaCommonFields.SRC_TIME.getFid(), listener.mTradeCache.mSrcTime))
                listener.mTradeCache.mSrcTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeActivityTime implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime (null, MamdaCommonFields.ACTIVITY_TIME.getFid(), listener.mTradeCache.mActTime))
                listener.mTradeCache.mActTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    public static class TradeLineTime implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime (null, MamdaCommonFields.LINE_TIME.getFid(), listener.mTradeCache.mLineTime))
                listener.mTradeCache.mLineTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    public static class TradeSendTime implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime (null, MamdaCommonFields.SEND_TIME.getFid(), listener.mTradeCache.mSendTime))
                listener.mTradeCache.mSendTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradePubId implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaCommonFields.PUB_ID.getFid(), listener.mTradeCache.mPubId))
                listener.mTradeCache.mPubIdFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeId implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
           listener.tmpField = msg.getField (null, MamdaTradeFields.TRADE_ID.getFid(), null);
            if (listener.tmpField != null)
            {
                switch (listener.tmpField.getType())
                {
                    case MamaFieldDescriptor.STRING:
                        listener.mTradeCache.mTradeId.setValue(listener.tmpField.getString ());
                        listener.mTradeCache.mTradeIdFieldState.setState (MamdaFieldState.MODIFIED);
                        break;
                    case MamaFieldDescriptor.I8:
                    case MamaFieldDescriptor.U8:
                    case MamaFieldDescriptor.I16:
                    case MamaFieldDescriptor.U16:
                    case MamaFieldDescriptor.I32:
                    case MamaFieldDescriptor.U32:
                    case MamaFieldDescriptor.I64:
                    case MamaFieldDescriptor.U64:
                        listener.mTradeCache.mTradeId.setValue(String.valueOf(listener.tmpField.getU64()));
                        listener.mTradeCache.mTradeIdFieldState.setState (MamdaFieldState.MODIFIED);
                    default:
                        break;
                }
            }
        }
    }

    private static class OrigTradeId implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
           listener.tmpField = msg.getField (null, MamdaTradeFields.ORIG_TRADE_ID.getFid(), null);
            if (listener.tmpField != null)
            {
                switch (listener.tmpField.getType())
                {
                    case MamaFieldDescriptor.STRING:
                        listener.mTradeCache.mOrigTradeId.setValue(listener.tmpField.getString ());
                        listener.mTradeCache.mOrigTradeIdFieldState.setState (MamdaFieldState.MODIFIED);
                        break;
                    case MamaFieldDescriptor.I8:
                    case MamaFieldDescriptor.U8:
                    case MamaFieldDescriptor.I16:
                    case MamaFieldDescriptor.U16:
                    case MamaFieldDescriptor.I32:
                    case MamaFieldDescriptor.U32:
                    case MamaFieldDescriptor.I64:
                    case MamaFieldDescriptor.U64:
                        listener.mTradeCache.mOrigTradeId.setValue(String.valueOf(listener.tmpField.getU64()));
                        listener.mTradeCache.mOrigTradeIdFieldState.setState (MamdaFieldState.MODIFIED);
                    default:
                        break;
                }
            }
        }
    }

    private static class CorrTradeId implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.CORR_TRADE_ID.getFid(), listener.mTradeCache.mCorrTradeId))
            {
                listener.mTradeCache.mCorrTradeIdFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class TradeAccVolume implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.TOTAL_VOLUME.getFid(), listener.mTradeCache.mAccVolume))
                listener.mTradeCache.mAccVolumeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeOffExAccVolume implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.OFF_EXCHANGE_TOTAL_VOLUME.getFid(), listener.mTradeCache.mOffExAccVolume))
                listener.mTradeCache.mOffExAccVolumeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeOnExAccVolume implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.ON_EXCHANGE_TOTAL_VOLUME.getFid(), listener.mTradeCache.mOnExAccVolume))
                listener.mTradeCache.mOnExAccVolumeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeNetChange implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.NET_CHANGE.getFid(), listener.mTradeCache.mNetChange))
                listener.mTradeCache.mNetChangeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class MamdaShortSaleCircuitBreaker implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryChar  (null, MamdaTradeFields.SHORT_SALE_CIRCUIT_BREAKER.getFid(), listener.mTradeCache.mShortSaleCircuitBreaker))
                listener.mTradeCache.mShortSaleCircuitBreakerFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }    

    private static class MamdaOrigShortSaleCircuitBreaker implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryChar  (null, MamdaTradeFields.ORIG_SHORT_SALE_CIRCUIT_BREAKER.getFid(), listener.mTradeCache.mOrigShortSaleCircuitBreaker))
                listener.mTradeCache.mOrigShortSaleCircuitBreakerFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }    

    private static class MamdaCorrShortSaleCircuitBreaker implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryChar  (null, MamdaTradeFields.CORR_SHORT_SALE_CIRCUIT_BREAKER.getFid(), listener.mTradeCache.mCorrShortSaleCircuitBreaker))
                listener.mTradeCache.mCorrShortSaleCircuitBreakerFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }    

    private static class TradePctChange implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.PCT_CHANGE.getFid(), listener.mTradeCache.mPctChange))
                listener.mTradeCache.mPctChangeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class AggressorSide implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {           
            if (msg.tryChar (null, MamdaTradeFields.AGGRESSOR_SIDE.getFid(), listener.mTradeCache.mTmpChar))
            {               
                listener.mTradeCache.mSide = Character.toString(listener.mTradeCache.mTmpChar.getValue());  
                listener.mTradeCache.mSideFieldState.setState (MamdaFieldState.MODIFIED);        
            }
        }
    }

    private static class TradeSide implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {  
            listener.tmpField = msg.getField (null, MamdaTradeFields.TRADE_SIDE.getFid(), null);  
            if( listener.tmpField != null)
            {
                switch (listener.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8:
                    case MamaFieldDescriptor.U8:
                    case MamaFieldDescriptor.I16:
                    case MamaFieldDescriptor.U16:
                    case MamaFieldDescriptor.I32:
                    case MamaFieldDescriptor.U32:          
                        listener.mTradeCache.mTmpSide.setState ((short)listener.tmpField.getU32()); 
                        listener.mTradeCache.mSide = MamdaTradeSide.toString (listener.mTradeCache.mTmpSide.getState());
                        listener.mTradeCache.mSideFieldState.setState (MamdaFieldState.MODIFIED);               
                        break;  
                    case MamaFieldDescriptor.STRING:
                        listener.mTradeCache.mTmpSide.setState (MamdaTradeSide.mamdaTradeSideFromString(listener.tmpField.getString()));  
                        listener.mTradeCache.mSide = MamdaTradeSide.toString (listener.mTradeCache.mTmpSide.getState());
                        listener.mTradeCache.mSideFieldState.setState (MamdaFieldState.MODIFIED);
                    default:
                        break;
                }
            }
        }
    }

    private static class TradeDirection implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            /* Allow trade direction as either string or integer - 
             * allows it to be handled if (eg) feeds have mama-publish-enums-as-ints
             * turned on.
             */
            listener.tmpField = msg.getField (null, MamdaTradeFields.TRADE_DIRECTION.getFid(), null);
            if (listener.tmpField != null)
            {
                switch (listener.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8:
                    case MamaFieldDescriptor.U8:
                    case MamaFieldDescriptor.I16:
                    case MamaFieldDescriptor.U16:
                    case MamaFieldDescriptor.I32:
                    case MamaFieldDescriptor.U32:
                        listener.mTradeCache.mTradeDirection.setValue (String.valueOf (listener.tmpField.getU32()));
                        listener.mTradeCache.mTradeDirectionFieldState.setState (MamdaFieldState.MODIFIED);
                        break;
                    case MamaFieldDescriptor.STRING:
                        listener.mTradeCache.mTradeDirection.setValue (listener.tmpField.getString ());
                        listener.mTradeCache.mTradeDirectionFieldState.setState (MamdaFieldState.MODIFIED);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    private static class TradeOpenPrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.OPEN_PRICE.getFid(), listener.mTradeCache.mOpenPrice))
                listener.mTradeCache.mOpenPriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeHighPrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.HIGH_PRICE.getFid(), listener.mTradeCache.mHighPrice))
                listener.mTradeCache.mHighPriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeLowPrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.LOW_PRICE.getFid(), listener.mTradeCache.mLowPrice))
                listener.mTradeCache.mLowPriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeClosePrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.CLOSE_PRICE.getFid(), listener.mTradeCache.mClosePrice))
                listener.mTradeCache.mClosePriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradePrevClosePrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.PREV_CLOSE_PRICE.getFid(), listener.mTradeCache.mPrevClosePrice))
                listener.mTradeCache.mPrevClosePriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradePrevCloseDate implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime (null, MamdaTradeFields.PREV_CLOSE_DATE.getFid(), listener.mTradeCache.mPrevCloseDate))
                listener.mTradeCache.mPrevCloseDateFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeAdjPrevClose implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.ADJ_PREV_CLOSE.getFid(), listener.mTradeCache.mAdjPrevClose))
                listener.mTradeCache.mAdjPrevCloseFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeBlockCount implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.BLOCK_COUNT.getFid(), listener.mTradeCache.mBlockCount))
                listener.mTradeCache.mBlockCountFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeBlockVolume implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if(msg.tryF64  (null, MamdaTradeFields.BLOCK_VOLUME.getFid(), listener.mTradeCache.mBlockVolume))
                listener.mTradeCache.mBlockVolumeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeVWap implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.VWAP.getFid(), listener.mTradeCache.mVwap))
                listener.mTradeCache.mVwapFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeOffExVWap implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.OFF_EXCHANGE_VWAP.getFid(), listener.mTradeCache.mOffExVwap))
                listener.mTradeCache.mOffExVwapFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeOnExVWap implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.ON_EXCHANGE_VWAP.getFid(), listener.mTradeCache.mOnExVwap))
                listener.mTradeCache.mOnExVwapFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeTotalValue implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.TOTAL_VALUE.getFid(), listener.mTradeCache.mTotalValue))
                listener.mTradeCache.mTotalValueFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeOffExTotalValue implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.OFF_EXCHANGE_TOTAL_VALUE.getFid(), listener.mTradeCache.mOffExTotalValue))
                listener.mTradeCache.mOffExTotalValueFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeOnExTotalValue implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.ON_EXCHANGE_TOTAL_VALUE.getFid(), listener.mTradeCache.mOnExTotalValue))
                listener.mTradeCache.mOnExTotalValueFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeStdDev implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.STD_DEV.getFid(), listener.mTradeCache.mStdDev))
                listener.mTradeCache.mStdDevFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeStdDevSum implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.STD_DEV_SUM.getFid(), listener.mTradeCache.mStdDevSum))
                listener.mTradeCache.mStdDevSumFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeStdDevSumSquares implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.STD_DEV_SUM_SQUARES.getFid(), listener.mTradeCache.mStdDevSumSquares))
                listener.mTradeCache.mStdDevSumSquaresFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeOrderId implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.ORDER_ID.getFid(), listener.mTradeCache.mOrderId))
                listener.mTradeCache.mOrderIdFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeSettlePrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.SETTLE_PRICE.getFid(), listener.mTradeCache.mSettlePrice))
                listener.mTradeCache.mSettlePriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeSettleDate implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime (null, MamdaTradeFields.SETTLE_DATE.getFid(), listener.mTradeCache.mSettleDate))
                listener.mTradeCache.mSettleDateFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeEventSeqNum implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.TRADE_SEQNUM.getFid(), listener.mTradeCache.mEventSeqNum))
                listener.mTradeCache.mEventSeqNumFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeLastDate implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime  (null, MamdaTradeFields.TRADE_DATE.getFid(), listener.mTradeCache.mLastDate))
                listener.mTradeCache.mLastDateFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeLastTime implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime (null, MamdaTradeFields.TRADE_TIME.getFid(), listener.mTradeCache.mLastTime))
            {
              listener.mTradeCache.mGotTradeTime = true;
              listener.mTradeCache.mLastTimeFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class TradeIrregPrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.IRREG_PRICE.getFid(), listener.mTradeCache.mIrregPrice))
            {
                if (listener.mTradeCache.mIrregPrice.getValue() != 0.0 && listener.mTradeCache.mIsIrregular.getValue() == false)
                {
                    listener.mTradeCache.mIsIrregular.setValue (true);
                    listener.mTradeCache.mIsIrregularFieldState.setState (MamdaFieldState.MODIFIED);
                }
                listener.mTradeCache.mGotTradePrice = true;
                listener.mTradeCache.mIrregPriceFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class TradeIrregVolume implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.IRREG_SIZE.getFid(), listener.mTradeCache.mIrregVolume))
            {
                if (listener.mTradeCache.mIrregVolume.getValue() != 0 && listener.mTradeCache.mIsIrregular.getValue() == false)
                {
                    listener.mTradeCache.mIsIrregular.setValue(true);
                    listener.mTradeCache.mIsIrregularFieldState.setState (MamdaFieldState.MODIFIED);
                }
                listener.mTradeCache.mGotTradeSize = true;
                listener.mTradeCache.mIrregVolumeFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class TradeIrregPartId implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.IRREG_PART_ID.getFid(), listener.mTradeCache.mIrregPartId))
            {
                if (listener.mTradeCache.mIrregPartId.getValue() == "" && listener.mTradeCache.mIsIrregular.getValue() == false)
                {
                    listener.mTradeCache.mIsIrregular.setValue(true);
                    listener.mTradeCache.mIsIrregularFieldState.setState (MamdaFieldState.MODIFIED);
                }
                listener.mTradeCache.mIrregPartIdFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class TradeIrregTime implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime (null, MamdaTradeFields.IRREG_TIME.getFid(), listener.mTradeCache.mIrregTime))
            {
                if (listener.mTradeCache.mIrregTime.hasTime() && listener.mTradeCache.mIsIrregular.getValue() == false)
                {
                    listener.mTradeCache.mIsIrregular.setValue(true);
                    listener.mTradeCache.mIsIrregularFieldState.setState (MamdaFieldState.MODIFIED);
                }
                listener.mTradeCache.mGotTradeTime = true;
                listener.mTradeCache.mIrregTimeFieldState.setState (MamdaFieldState.MODIFIED);
            }
        }
    }

    private static class TradeLastPrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.TRADE_PRICE.getFid(), listener.mTradeCache.mLastPrice))
            {
                listener.mTradeCache.mIsIrregular.setValue(false);
                listener.mTradeCache.mIsIrregularFieldState.setState (MamdaFieldState.MODIFIED);
                listener.mTradeCache.mGotTradePrice = true;
                listener.mTradeCache.mLastPriceFieldState.setState (MamdaFieldState.MODIFIED);
            }
        } 
    }

    private static class TradeLastVolume implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.TRADE_SIZE.getFid(), listener.mTradeCache.mLastVolume))
            {
                listener.mTradeCache.mIsIrregular.setValue(false);
                listener.mTradeCache.mIsIrregularFieldState.setState (MamdaFieldState.MODIFIED);
                listener.mTradeCache.mGotTradeSize = true;
                listener.mTradeCache.mLastVolumeFieldState.setState (MamdaFieldState.MODIFIED);
            }
        } 
    }

    private static class TradeQualStr implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.TRADE_QUALIFIER.getFid(), listener.mTradeCache.mTradeQualStr))
                listener.mTradeCache.mTradeQualStrFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeQualNativeStr implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.SALE_CONDITION.getFid(), listener.mTradeCache.mTradeQualNativeStr))
                listener.mTradeCache.mTradeQualNativeStrFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeSellerSalesDays implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.SELLERS_SALE_DAYS.getFid(), listener.mTradeCache.mSellersSaleDays))
                listener.mTradeCache.mSellersSaleDaysFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeStopStockInd implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            // There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            // FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            // Adding support for this in MAMDA for client apps coded to expect this behaviour
            listener.tmpField = msg.getField (null, MamdaTradeFields.STOP_STOCK_IND.getFid(), null);
            if (listener.tmpField != null)
            {
                switch (listener.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8 :
                    case MamaFieldDescriptor.CHAR :
                        listener.mTradeCache.mStopStockInd.setValue(
                            listener.tmpField.getChar ());
                        listener.mTradeCache.mStopStockIndFieldState.setState (MamdaFieldState.MODIFIED);
                        break;
                    case MamaFieldDescriptor.STRING :
                        if (listener.tmpField.getString().length() > 0)
                        {
                            listener.mTradeCache.mStopStockInd.setValue(listener.tmpField.getString().charAt(0));
                            listener.mTradeCache.mStopStockIndFieldState.setState (MamdaFieldState.MODIFIED);
                        }
                        else
                        {
                            listener.mTradeCache.mStopStockInd.setValue(' ');
                            listener.mTradeCache.mStopStockIndFieldState.setState (MamdaFieldState.MODIFIED);
                        }
                        break;
                    default : break;
                }
            }
        }
    }

    private static class TradeExecVenue implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            /* Allow trade execution venue as either string or integer - 
             * allows it to be handled if (eg) feeds have mama-publish-enums-as-ints
             * turned on.
             */
            listener.tmpField = msg.getField (null, MamdaTradeFields.TRADE_EXEC_VENUE.getFid(), null);
            if (listener.tmpField != null)
            {
                switch (listener.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8:
                    case MamaFieldDescriptor.U8:
                    case MamaFieldDescriptor.I16:
                    case MamaFieldDescriptor.U16:
                    case MamaFieldDescriptor.I32:
                    case MamaFieldDescriptor.U32:
                        listener.mTradeCache.mTradeExecVenue.setValue (String.valueOf (listener.tmpField.getU32()));
                        listener.mTradeCache.mTradeExecVenueFieldState.setState (MamdaFieldState.MODIFIED);
                        break;
                    case MamaFieldDescriptor.STRING:
                        listener.mTradeCache.mTradeExecVenue.setValue (listener.tmpField.getString ());
                        listener.mTradeCache.mTradeExecVenueFieldState.setState (MamdaFieldState.MODIFIED);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    private static class OffExTradePrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.OFF_EXCHANGE_TRADE_PRICE.getFid(), listener.mTradeCache.mOffExTradePrice))
              listener.mTradeCache.mOffExTradePriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class OnExTradePrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.ON_EXCHANGE_TRADE_PRICE.getFid(), listener.mTradeCache.mOnExTradePrice))
                listener.mTradeCache.mOnExTradePriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeCount implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.TRADE_COUNT.getFid(), listener.mTmpTradeCount))
            {
                listener.mTradeCache.mGotTradeCount = true;
            }
        } 
    }

    private static class TradeUnits implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.TRADE_UNITS.getFid(), listener.mTradeCache.mTradeUnits))
                listener.mTradeCache.mTradeUnitsFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeLastSeqNum implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.LAST_SEQNUM.getFid(), listener.mTradeCache.mLastSeqNum))
                listener.mTradeCache.mLastSeqNumFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeHighSeqNum implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.HIGH_SEQNUM.getFid(), listener.mTradeCache.mHighSeqNum))
                listener.mTradeCache.mHighSeqNumFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeLowSeqNum implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.LOW_SEQNUM.getFid(), listener.mTradeCache.mLowSeqNum))
                listener.mTradeCache.mLowSeqNumFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeTotalVolumeSeqNum implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.TOTAL_VOLUME_SEQNUM.getFid(), listener.mTradeCache.mTotalVolumeSeqNum))
                listener.mTradeCache.mTotalVolumeSeqNumFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeCurrencyCode implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.CURRENCY_CODE.getFid(), listener.mTradeCache.mCurrencyCode))
                listener.mTradeCache.mCurrencyCodeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeOrigSeqNum implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.ORIG_SEQNUM.getFid(), listener.mTradeCache.mOrigSeqNum))
                listener.mTradeCache.mOrigSeqNumFieldState.setState (MamdaFieldState.MODIFIED);
        } 
    }

    private static class TradeOrigPrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.ORIG_PRICE.getFid(), listener.mTradeCache.mOrigPrice))
                listener.mTradeCache.mOrigPriceFieldState.setState (MamdaFieldState.MODIFIED);
        } 
    }

    private static class TradeOrigVolume implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.ORIG_SIZE.getFid(), listener.mTradeCache.mOrigVolume))
                listener.mTradeCache.mOrigVolumeFieldState.setState (MamdaFieldState.MODIFIED);
        } 
    }

    private static class TradeOrigPartId implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.ORIG_PART_ID.getFid(), listener.mTradeCache.mOrigPartId))
                listener.mTradeCache.mOrigPartIdFieldState.setState (MamdaFieldState.MODIFIED);
        } 
    }

    private static class TradeOrigQualStr implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.ORIG_TRADE_QUALIFIER.getFid(), listener.mTradeCache.mOrigQualStr))
                listener.mTradeCache.mOrigQualStrFieldState.setState (MamdaFieldState.MODIFIED);
        } 
    }

    private static class TradeOrigQualNativeStr implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.ORIG_SALE_CONDITION.getFid(), listener.mTradeCache.mOrigQualNativeStr))
                listener.mTradeCache.mOrigQualNativeStrFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeOrigSellersSaleDays implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.ORIG_SELLERS_SALE_DAYS.getFid(), listener.mTradeCache.mOrigSellersSaleDays))
                listener.mTradeCache.mOrigSellersSaleDaysFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeOrigStopStockInd implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            // There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            // FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            // Adding support for this in MAMDA for client apps coded to expect this behaviour
            listener.tmpField = msg.getField (null, MamdaTradeFields.ORIG_STOP_STOCK_IND.getFid(), null);
            if (listener.tmpField != null)
            {
                switch (listener.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8 :
                    case MamaFieldDescriptor.CHAR :                
                        listener.mTradeCache.mOrigStopStockInd.setValue(
                            listener.tmpField.getChar ());
                        listener.mTradeCache.mOrigStopStockIndFieldState.setState (MamdaFieldState.MODIFIED);
                        break;
                    case MamaFieldDescriptor.STRING :
                        if (listener.tmpField.getString().length() > 0)
                        {
                            listener.mTradeCache.mOrigStopStockInd.setValue(
                                listener.tmpField.getString().charAt(0));
                            listener.mTradeCache.mOrigStopStockIndFieldState.setState (MamdaFieldState.MODIFIED);
                        }
                        else
                        {
                            listener.mTradeCache.mOrigStopStockInd.setValue(' ');
                            listener.mTradeCache.mOrigStopStockIndFieldState.setState (MamdaFieldState.MODIFIED);
                        }
                        break;
                    default : break;
                }
            }
        }
    }

    private static class TradeCorrPrice implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryPrice  (null, MamdaTradeFields.CORR_PRICE.getFid(), listener.mTradeCache.mCorrPrice))
                listener.mTradeCache.mCorrPriceFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeCorrVolume implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryF64  (null, MamdaTradeFields.CORR_SIZE.getFid(), listener.mTradeCache.mCorrVolume))
                listener.mTradeCache.mCorrVolumeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeCorrPartId implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.CORR_PART_ID.getFid(), listener.mTradeCache.mCorrPartId))
                listener.mTradeCache.mCorrPartIdFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeCorrQualStr implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.CORR_TRADE_QUALIFIER.getFid(), listener.mTradeCache.mCorrQualStr))
                listener.mTradeCache.mCorrQualStrFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeCorrQualNativeStr implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryString (null, MamdaTradeFields.CORR_SALE_CONDITION.getFid(), listener.mTradeCache.mCorrQualNativeStr))
                listener.mTradeCache.mCorrQualNativeStrFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeCorrSellersSaleDays implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryI64  (null, MamdaTradeFields.CORR_SELLERS_SALE_DAYS.getFid(), listener.mTradeCache.mCorrSellersSaleDays))
                listener.mTradeCache.mCorrSellersSaleDaysFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeCorrStopStockInd implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            // There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            // FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            // Adding support for this in MAMDA for client apps coded to expect this behaviour
            listener.tmpField = msg.getField (null, MamdaTradeFields.CORR_STOP_STOCK_IND.getFid(), null);
            if (listener.tmpField != null)
            {
                switch (listener.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8 :
                    case MamaFieldDescriptor.CHAR :
                        listener.mTradeCache.mCorrStopStockInd.setValue(
                            listener.tmpField.getChar ());
                        listener.mTradeCache.mCorrStopStockIndFieldState.setState (MamdaFieldState.MODIFIED);
                        break;
                    case MamaFieldDescriptor.STRING :
                        if (listener.tmpField.getString().length() > 0)
                        {
                            listener.mTradeCache.mCorrStopStockInd.setValue(
                                listener.tmpField.getString().charAt(0));
                            listener.mTradeCache.mCorrStopStockIndFieldState.setState (MamdaFieldState.MODIFIED);
                        }
                        else
                        {
                            listener.mTradeCache.mCorrStopStockInd.setValue(' ');
                            listener.mTradeCache.mCorrStopStockIndFieldState.setState (MamdaFieldState.MODIFIED);
                        }
                        break;
                    default : break;
                }
            }
        }
    }

    private static class TradeCorrTime implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime  (null, MamdaTradeFields.CORR_TIME.getFid(), listener.mTradeCache.mCorrTime))
                listener.mTradeCache.mCorrTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeCancelTime implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryDateTime  (null, MamdaTradeFields.CANCEL_TIME.getFid(), listener.mTradeCache.mCancelTime))
                listener.mTradeCache.mCancelTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class TradeIsIrregular implements TradeUpdate
    {
        public void onUpdate (MamaMsg msg, MamdaTradeListener listener)
        {
            if (msg.tryBoolean  (null, MamdaTradeFields.IS_IRREGULAR.getFid(), listener.mTradeCache.mIsIrregular))
                listener.mTradeCache.mIsIrregularFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static void createUpdaters ()
    {
        int i = 0;
        if (mUpdaters == null)
        {
            mUpdaters = new TradeUpdate [MamdaTradeFields.getMaxFid() + 1];
        }

        if (MamdaCommonFields.SYMBOL != null)
            mUpdaters[i++] = new  MamdaTradeSymbol();

        if (MamdaCommonFields.ISSUE_SYMBOL != null)
            mUpdaters[i++] = new  MamdaTradeIssueSymbol();

        if (MamdaCommonFields.PART_ID != null)
            mUpdaters[i++] = new  TradePartId ();

        if (MamdaCommonFields.SRC_TIME != null)
            mUpdaters[i++] = new  TradeSrcTime();

        if (MamdaCommonFields.ACTIVITY_TIME != null)
            mUpdaters[i++] = new  TradeActivityTime();

        if (MamdaCommonFields.LINE_TIME != null)
            mUpdaters[i++] = new  TradeLineTime();

        if (MamdaCommonFields.SEND_TIME != null)
            mUpdaters[i++] = new  TradeSendTime();

        if (MamdaCommonFields.PUB_ID != null)
            mUpdaters[i++] = new  TradePubId();

        if (MamdaTradeFields.TRADE_ID != null)
            mUpdaters[i++] = new  TradeId();

        if (MamdaTradeFields.ORIG_TRADE_ID != null)
            mUpdaters[i++] = new  OrigTradeId();

        if (MamdaTradeFields.CORR_TRADE_ID != null)
            mUpdaters[i++] = new  CorrTradeId();

        if (MamdaTradeFields.TRADE_PRICE != null)
            mUpdaters[i++] = new  TradeLastPrice();

        if (MamdaTradeFields.TRADE_SIZE != null)
            mUpdaters[i++] = new  TradeLastVolume ();

        if (MamdaTradeFields.TRADE_DATE != null)
            mUpdaters[i++] = new  TradeLastDate ();

        if (MamdaTradeFields.TRADE_TIME != null)
            mUpdaters[i++] = new  TradeLastTime();

        if (MamdaTradeFields.SHORT_SALE_CIRCUIT_BREAKER != null)
            mUpdaters[i++] = new  MamdaShortSaleCircuitBreaker();

        if (MamdaTradeFields.ORIG_SHORT_SALE_CIRCUIT_BREAKER != null)
            mUpdaters[i++] = new  MamdaOrigShortSaleCircuitBreaker();

        if (MamdaTradeFields.CORR_SHORT_SALE_CIRCUIT_BREAKER != null)
            mUpdaters[i++] = new  MamdaCorrShortSaleCircuitBreaker();

        if (MamdaTradeFields.TRADE_DIRECTION != null)
            mUpdaters[i++] = new  TradeDirection();

        if (MamdaTradeFields.NET_CHANGE != null)
            mUpdaters[i++] = new  TradeNetChange();

        if (MamdaTradeFields.PCT_CHANGE != null)
            mUpdaters[i++] = new  TradePctChange();
    
        if (MamdaTradeFields.AGGRESSOR_SIDE != null)
            mUpdaters[i++] = new AggressorSide();

        if (MamdaTradeFields.TRADE_SIDE != null)
            mUpdaters[i++] = new TradeSide();

        if (MamdaTradeFields.TOTAL_VOLUME != null)
            mUpdaters[i++] = new  TradeAccVolume();

        if (MamdaTradeFields.OFF_EXCHANGE_TOTAL_VOLUME != null)
            mUpdaters[i++] = new  TradeOffExAccVolume();

        if (MamdaTradeFields.ON_EXCHANGE_TOTAL_VOLUME != null)
            mUpdaters[i++] = new  TradeOnExAccVolume();

        if (MamdaTradeFields.HIGH_PRICE != null)
            mUpdaters[i++] = new  TradeHighPrice ();

        if (MamdaTradeFields.LOW_PRICE != null)
            mUpdaters[i++] = new  TradeLowPrice();

        if (MamdaTradeFields.OPEN_PRICE != null)
            mUpdaters[i++] = new  TradeOpenPrice ();

        if (MamdaTradeFields.CLOSE_PRICE != null)
            mUpdaters[i++] = new  TradeClosePrice();

        if (MamdaTradeFields.PREV_CLOSE_PRICE != null)
            mUpdaters[i++] = new  TradePrevClosePrice();

        if (MamdaTradeFields.TRADE_SEQNUM != null)
            mUpdaters[i++] = new  TradeEventSeqNum();

        if (MamdaTradeFields.TRADE_QUALIFIER != null)
            mUpdaters[i++] = new  TradeQualStr();

        if (MamdaTradeFields.SALE_CONDITION != null)
            mUpdaters[i++] = new  TradeQualNativeStr();

        if (MamdaTradeFields.TRADE_PART_ID != null)
            mUpdaters[i++] = new  TradeLastPartId();

        if (MamdaTradeFields.TOTAL_VALUE != null)
            mUpdaters[i++] = new  TradeTotalValue();

        if (MamdaTradeFields.OFF_EXCHANGE_TOTAL_VALUE != null)
            mUpdaters[i++] = new  TradeOffExTotalValue();

        if (MamdaTradeFields.ON_EXCHANGE_TOTAL_VALUE != null)
            mUpdaters[i++] = new  TradeOnExTotalValue();

        if (MamdaTradeFields.VWAP != null)
            mUpdaters[i++] = new  TradeVWap();

        if (MamdaTradeFields.OFF_EXCHANGE_VWAP != null)
            mUpdaters[i++] = new  TradeOffExVWap();

        if (MamdaTradeFields.ON_EXCHANGE_VWAP != null)
            mUpdaters[i++] = new  TradeOnExVWap();

        if (MamdaTradeFields.STD_DEV != null)
            mUpdaters[i++] = new  TradeStdDev();

        if (MamdaTradeFields.STD_DEV_SUM != null)
            mUpdaters[i++] = new  TradeStdDevSum();

        if (MamdaTradeFields.STD_DEV_SUM_SQUARES != null)
            mUpdaters[i++] = new  TradeStdDevSumSquares();

        if (MamdaTradeFields.ORDER_ID != null)
            mUpdaters[i++] = new  TradeOrderId();

        if (MamdaTradeFields.SETTLE_PRICE != null)
            mUpdaters[i++] = new  TradeSettlePrice();

        if (MamdaTradeFields.SETTLE_DATE != null)
            mUpdaters[i++] = new  TradeSettleDate();

        if (MamdaTradeFields.SELLERS_SALE_DAYS != null)
            mUpdaters[i++] = new  TradeSellerSalesDays();

        if (MamdaTradeFields.STOP_STOCK_IND != null)
            mUpdaters[i++] = new  TradeStopStockInd();

        if (MamdaTradeFields.TRADE_EXEC_VENUE != null)
            mUpdaters[i++] = new  TradeExecVenue();

        if (MamdaTradeFields.OFF_EXCHANGE_TRADE_PRICE != null)
            mUpdaters[i++] = new  OffExTradePrice();

        if (MamdaTradeFields.ON_EXCHANGE_TRADE_PRICE != null)
            mUpdaters[i++] = new  OnExTradePrice();

        if (MamdaTradeFields.IS_IRREGULAR != null)
            mUpdaters[i++] = new  TradeIsIrregular();

        if (MamdaTradeFields.TRADE_UNITS != null)
            mUpdaters[i++] = new  TradeUnits();

        if (MamdaTradeFields.LAST_SEQNUM != null)
            mUpdaters[i++] = new  TradeLastSeqNum();

        if (MamdaTradeFields.HIGH_SEQNUM != null)
            mUpdaters[i++] = new  TradeHighSeqNum();

        if (MamdaTradeFields.LOW_SEQNUM != null)
            mUpdaters[i++] = new  TradeLowSeqNum();

        if (MamdaTradeFields.TOTAL_VOLUME_SEQNUM != null)
            mUpdaters[i++] = new  TradeTotalVolumeSeqNum();

        if (MamdaTradeFields.CURRENCY_CODE != null)
            mUpdaters[i++] = new  TradeCurrencyCode();

        if (MamdaTradeFields.ORIG_PART_ID  != null)
            mUpdaters[i++] = new  TradeOrigPartId();

        if (MamdaTradeFields.ORIG_SIZE != null)
            mUpdaters[i++] = new  TradeOrigVolume();

        if (MamdaTradeFields.ORIG_PRICE != null)
            mUpdaters[i++] = new  TradeOrigPrice();

        if (MamdaTradeFields.ORIG_SEQNUM != null)
            mUpdaters[i++] = new  TradeOrigSeqNum();

        if (MamdaTradeFields.ORIG_TRADE_QUALIFIER != null)
            mUpdaters[i++] = new  TradeOrigQualStr();

        if (MamdaTradeFields.ORIG_SALE_CONDITION != null)
            mUpdaters[i++] = new  TradeOrigQualNativeStr();

        if (MamdaTradeFields.ORIG_SELLERS_SALE_DAYS != null)
            mUpdaters[i++] = new  TradeOrigSellersSaleDays();

        if (MamdaTradeFields.ORIG_STOP_STOCK_IND != null)
            mUpdaters[i++] = new  TradeOrigStopStockInd();

        if (MamdaTradeFields.CORR_PART_ID != null)
            mUpdaters[i++] = new  TradeCorrPartId();

        if (MamdaTradeFields.CORR_SIZE != null)
            mUpdaters[i++] = new  TradeCorrVolume();

        if (MamdaTradeFields.CORR_PRICE != null)
            mUpdaters[i++] = new  TradeCorrPrice();

        if (MamdaTradeFields.CORR_TRADE_QUALIFIER != null)
            mUpdaters[i++] = new  TradeCorrQualStr();

        if (MamdaTradeFields.CORR_SALE_CONDITION != null)
            mUpdaters[i++] = new  TradeCorrQualNativeStr();

        if (MamdaTradeFields.CORR_SELLERS_SALE_DAYS != null)
            mUpdaters[i++] = new  TradeCorrSellersSaleDays();

        if (MamdaTradeFields.CORR_STOP_STOCK_IND != null)
            mUpdaters[i++] = new  TradeCorrStopStockInd();

        if (MamdaTradeFields.CANCEL_TIME != null)
            mUpdaters[i++] = new  TradeCancelTime();

        if (MamdaTradeFields.TRADE_COUNT != null)
            mUpdaters[i++] = new  TradeCount();

        if (MamdaTradeFields.BLOCK_COUNT != null)
            mUpdaters[i++] = new  TradeBlockCount();

        if (MamdaTradeFields.BLOCK_VOLUME != null)
            mUpdaters[i++] = new  TradeBlockVolume();

        if (MamdaTradeFields.PREV_CLOSE_DATE != null)
            mUpdaters[i++] = new  TradePrevCloseDate();

        if (MamdaTradeFields.ADJ_PREV_CLOSE != null)
            mUpdaters[i++] = new  TradeAdjPrevClose ();

        if (MamdaTradeFields.IRREG_PRICE != null)
            mUpdaters[i++] = new  TradeIrregPrice();

        if (MamdaTradeFields.IRREG_SIZE != null)
            mUpdaters[i++] = new  TradeIrregVolume();

        if (MamdaTradeFields.IRREG_PART_ID != null)
            mUpdaters[i++] = new  TradeIrregPartId();

        if (MamdaTradeFields.IRREG_TIME != null)
            mUpdaters[i++] = new  TradeIrregTime();

        if (MamdaTradeFields.UPDATE_AS_TRADE != null)
            mUpdaters[i++] = new  TradeUpdateAsTrade();
    }
}


