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
import java.util.logging.*;
import java.util.Iterator;
import java.util.LinkedList;

/**
 * MamdaFundamentalListener is a class that specializes in handling
 * fundamental equity pricing/analysis attributes, indicators and
 * ratios.  Developers provide their own implementation of the
 * MamdaFundamentalHandler interface and will be delivered
 * notifications for updates in the fundamental data.  An obvious
 * application for this MAMDA class is any kind of pricing analysis
 * application.
 *
 * Note: The MamdaFundamentalListener class caches equity
 * pricing/analysis attributes, indicators and ratios. Among other
 * reasons, caching of these fields makes it possible to provide
 * complete fundamental callbacks, even when the publisher (e.g., feed
 * handler) is only publishing deltas containing modified fields.
 */

public class MamdaFundamentalListener implements MamdaMsgListener,
                                                 MamdaFundamentals
{
    private static Logger mLogger =
                Logger.getLogger("com.wombat.mamda.MamdaFundamentalListener");

    private final LinkedList      mHandlers       = new LinkedList();
    private final MamaDateTime    mSrcTimeStr     = new MamaDateTime();
    private final MamaDateTime    mActTimeStr     = new MamaDateTime();
    private final MamaDateTime    mSendTimeStr    = new MamaDateTime();
    private final MamaDateTime    mLineTimeStr    = new MamaDateTime();
    private final MamaString      mPartId         = new MamaString();
    private final MamaString      mSymbol         = new MamaString();

    // Fundamental Fields 
    // The following fields are used for caching the last reported
    // fundamental equity pricing/analysis attributes, indicators and ratios.
    // The reason for cahcing these values is to allow a configuration that 
    // passes the minimum amount of data  (unchanged fields not sent).
    private final MamaString      mCorpActType    = new MamaString();
    private final MamaDouble      mDividendPrice  = new MamaDouble();
    private final MamaString      mDivFreq        = new MamaString();
    private final MamaString      mDivExDate      = new MamaString();
    private final MamaString      mDivPayDate     = new MamaString();
    private final MamaString      mDivRecordDate  = new MamaString();
    private final MamaString      mDivCurrency    = new MamaString();
    private final MamaLong        mSharesOut      = new MamaLong();
    private final MamaLong        mSharesFloat    = new MamaLong();
    private final MamaLong        mSharesAuth     = new MamaLong();
    private final MamaDouble      mEarnPerShare   = new MamaDouble();
    private final MamaDouble      mVolatility     = new MamaDouble();
    private final MamaDouble      mPeRatio        = new MamaDouble();
    private final MamaDouble      mYield          = new MamaDouble();
    private final MamaString      mMrktSegmNative = new MamaString();
    private final MamaString      mMrktSectNative = new MamaString();
    private final MamaString      mMarketSegment  = new MamaString();
    private final MamaString      mMarketSector   = new MamaString();
    private final MamaDouble      mHistVolatility = new MamaDouble();
    private final MamaDouble      mRiskFreeRate   = new MamaDouble();

    private MamaMsgField          tmpField        = new MamaMsgField();

    /*  Field State accessors       */
    private final MamdaFieldState    mSrcTimeStrFieldState     = new MamdaFieldState();
    private final MamdaFieldState    mActTimeStrFieldState     = new MamdaFieldState();
    private final MamdaFieldState    mSendTimeStrFieldState    = new MamdaFieldState();
    private final MamdaFieldState    mLineTimeStrFieldState    = new MamdaFieldState();
    private final MamdaFieldState    mPartIdFieldState         = new MamdaFieldState();
    private final MamdaFieldState    mSymbolFieldState         = new MamdaFieldState();
    private final MamdaFieldState    mCorpActTypeFieldState    = new MamdaFieldState();
    private final MamdaFieldState    mDividendPriceFieldState  = new MamdaFieldState();
    private final MamdaFieldState    mDivFreqFieldState        = new MamdaFieldState();
    private final MamdaFieldState    mDivExDateFieldState      = new MamdaFieldState();
    private final MamdaFieldState    mDivPayDateFieldState     = new MamdaFieldState();
    private final MamdaFieldState    mDivRecordDateFieldState  = new MamdaFieldState();
    private final MamdaFieldState    mDivCurrencyFieldState    = new MamdaFieldState();
    private final MamdaFieldState    mSharesOutFieldState      = new MamdaFieldState();
    private final MamdaFieldState    mSharesFloatFieldState    = new MamdaFieldState();
    private final MamdaFieldState    mSharesAuthFieldState     = new MamdaFieldState();
    private final MamdaFieldState    mEarnPerShareFieldState   = new MamdaFieldState();
    private final MamdaFieldState    mVolatilityFieldState     = new MamdaFieldState();
    private final MamdaFieldState    mPeRatioFieldState        = new MamdaFieldState();
    private final MamdaFieldState    mYieldFieldState          = new MamdaFieldState();
    private final MamdaFieldState    mMrktSegmNativeFieldState = new MamdaFieldState();
    private final MamdaFieldState    mMrktSectNativeFieldState = new MamdaFieldState();
    private final MamdaFieldState    mMarketSegmentFieldState  = new MamdaFieldState();
    private final MamdaFieldState    mMarketSectorFieldState   = new MamdaFieldState();
    private final MamdaFieldState    mHistVolatilityFieldState = new MamdaFieldState();
    private final MamdaFieldState    mRiskFreeRateFieldState   = new MamdaFieldState();
    private MamdaFieldState          tmpFieldFieldState        = new MamdaFieldState();
    
    /**
     * Create a specialized fundamental field listener.
     */
    public MamdaFundamentalListener ()
    {
    }

    /**
     * Add a specialized fundamentals handler.
     */
    public void addHandler (MamdaFundamentalHandler handler)
    {
        mHandlers.addLast(handler);
    }

    // Inherited from MamdaBasicRecap and MamdaBasicEvent: 

    public MamaDateTime getSrcTime()
    {
        return mSrcTimeStr;
    }

    public MamaDateTime getActivityTime()
    {
        return mActTimeStr;
    }

    public MamaDateTime getSendTime()
    {
        return mSendTimeStr;
    }

    public MamaDateTime getLineTime()
    {
        return mLineTimeStr;
    }

    public String getPartId()
    {
        return mPartId.getValue();
    }

    public String getSymbol()
    {
        return mPartId.getValue();
    }

    // Fundamental recap fields access:

    public String getCorporateActionType()
    {
        return mCorpActType.getValue();
    }

    public double getDividendPrice()
    {
        return mDividendPrice.getValue();
    }

    public String getDividendFrequency()
    {
        return mDivFreq.getValue();
    }

    public String getDividendExDate()
    {
        return mDivExDate.getValue();
    }

    public String getDividendPayDate()
    {
        return mDivPayDate.getValue();
    }

    public String getDividendRecordDate()
    {
        return mDivRecordDate.getValue();
    }

    public String getDividendCurrency()
    {
        return mDivCurrency.getValue();
    }

    public long   getSharesOut()
    {
        return mSharesOut.getValue();
    }

    public long   getSharesFloat()
    {
        return mSharesFloat.getValue();
    }

    public long   getSharesAuthorized()
    {
        return mSharesAuth.getValue();
    }

    public double getEarningsPerShare()
    {
        return mEarnPerShare.getValue();
    }

    public double getVolatility()
    {
        return mVolatility.getValue();
    }

    public double getPriceEarningsRatio()
    {
        return mPeRatio.getValue();
    }

    public double getYield()
    {
        return mYield.getValue();
    }

    public String getMarketSegmentNative()
    {
        return mMrktSegmNative.getValue();
    }

    public String getMarketSectorNative()
    {
        return mMrktSectNative.getValue();
    }

    public String getMarketSegment()
    {
        return mMarketSegment.getValue();
    }

    public String getMarketSector()
    {
        return mMarketSector.getValue();
    }

    public double getHistoricalVolatility()
    {
        return mHistVolatility.getValue();
    }

    public double getRiskFreeRate()
    {
        return mRiskFreeRate.getValue();
    }



/*  FieldState Accessors        */

    public short getSrcTimeFieldState()
    {
        return mSrcTimeStrFieldState.getState();
    }

    public short getActivityTimeFieldState()
    {
        return mActTimeStrFieldState.getState();
    }

    public short getSendTimeFieldState()
    {
        return mSendTimeStrFieldState.getState();
    }

    public short getLineTimeFieldState()
    {
        return mLineTimeStrFieldState.getState();
    }

    public short getPartIdFieldState()
    {
        return mPartIdFieldState.getState();
    }

    public short getSymbolFieldState()
    {
        return mPartIdFieldState.getState();
    }

    // Fundamental recap fields access:

    public short getCorporateActionTypeFieldState()
    {
        return mCorpActTypeFieldState.getState();
    }

    public short getDividendPriceFieldState()
    {
        return mDividendPriceFieldState.getState();
    }

    public short getDividendFrequencyFieldState()
    {
        return mDivFreqFieldState.getState();
    }

    public short getDividendExDateFieldState()
    {
        return mDivExDateFieldState.getState();
    }

    public short getDividendPayDateFieldState()
    {
        return mDivPayDateFieldState.getState();
    }

    public short getDividendRecordDateFieldState()
    {
        return mDivRecordDateFieldState.getState();
    }

    public short getDividendCurrencyFieldState()
    {
        return mDivCurrencyFieldState.getState();
    }

    public short   getSharesOutFieldState()
    {
        return mSharesOutFieldState.getState();
    }

    public short   getSharesFloatFieldState()
    {
        return mSharesFloatFieldState.getState();
    }

    public short   getSharesAuthorizedFieldState()
    {
        return mSharesAuthFieldState.getState();
    }

    public short getEarningsPerShareFieldState()
    {
        return mEarnPerShareFieldState.getState();
    }

    public short getVolatilityFieldState()
    {
        return mVolatilityFieldState.getState();
    }

    public short getPriceEarningsRatioFieldState()
    {
        return mPeRatioFieldState.getState();
    }

    public short getYieldFieldState()
    {
        return mYieldFieldState.getState();
    }

    public short getMarketSegmentNativeFieldState()
    {
        return mMrktSegmNativeFieldState.getState();
    }

    public short getMarketSectorNativeFieldState()
    {
        return mMrktSectNativeFieldState.getState();
    }

    public short getMarketSegmentFieldState()
    {
        return mMarketSegmentFieldState.getState();
    }

    public short getMarketSectorFieldState()
    {
        return mMarketSectorFieldState.getState();
    }

    public short getHistoricalVolatilityFieldState()
    {
        return mHistVolatilityFieldState.getState();
    }

    public short getRiskFreeRateFieldState()
    {
        return mRiskFreeRateFieldState.getState();
    }

    
    /*  End FieldState Accessors        */
    
    
    /**
     * Implementation of MamdaListener interface.
     */
    public void onMsg (MamdaSubscription subscription,
                       MamaMsg           msg,
                       short             msgType)
    {

        if (!MamdaFundamentalFields.isSet())
        {
            mLogger.warning ("MamdaFundamentalFields::setDictionary() has not been called.");
            return;
        }

        // If msg is a trade-related message, invoke the
        // appropriate callback:
        switch (msgType)
        {
            case MamaMsgType.TYPE_INITIAL:
            case MamaMsgType.TYPE_RECAP:
                handleRecap (subscription, msg);
                break;
            case MamaMsgType.TYPE_UPDATE:
                handleUpdate (subscription, msg);
                break;
        }
    }

    private void handleRecap (MamdaSubscription  subscription,
                              MamaMsg            msg)
    {
        updateFieldStates();
        updateFundamentalFields (msg);
        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaFundamentalHandler handler =
                (MamdaFundamentalHandler) i.next();
            handler.onFundamentals (subscription, this, msg, this);
        }
    }

    private void handleUpdate (MamdaSubscription  subscription,
                               MamaMsg            msg)
    {
        // Same handling as recap (for now, at least)
        handleRecap (subscription, msg);
    }

private void updateFieldStates()
{
    if (mSrcTimeStrFieldState.getState() == MamdaFieldState.MODIFIED)        
        mSrcTimeStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);    
    if (mActTimeStrFieldState.getState() == MamdaFieldState.MODIFIED)    
        mActTimeStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);    
    if (mSendTimeStrFieldState.getState() == MamdaFieldState.MODIFIED)   
        mSendTimeStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);   
    if (mLineTimeStrFieldState.getState() == MamdaFieldState.MODIFIED)   
        mLineTimeStrFieldState.setState(MamdaFieldState.NOT_MODIFIED);   
    if (mPartIdFieldState.getState() == MamdaFieldState.MODIFIED)        
        mPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);        
    if (mSymbolFieldState.getState() == MamdaFieldState.MODIFIED)        
        mSymbolFieldState.setState(MamdaFieldState.NOT_MODIFIED);        
    if (mCorpActTypeFieldState.getState() == MamdaFieldState.MODIFIED)   
        mCorpActTypeFieldState.setState(MamdaFieldState.NOT_MODIFIED);   
    if (mDividendPriceFieldState.getState() == MamdaFieldState.MODIFIED) 
        mDividendPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
    if (mDivFreqFieldState.getState() == MamdaFieldState.MODIFIED)       
        mDivFreqFieldState.setState(MamdaFieldState.NOT_MODIFIED);       
    if (mDivExDateFieldState.getState() == MamdaFieldState.MODIFIED)     
        mDivExDateFieldState.setState(MamdaFieldState.NOT_MODIFIED);     
    if (mDivPayDateFieldState.getState() == MamdaFieldState.MODIFIED)    
        mDivPayDateFieldState.setState(MamdaFieldState.NOT_MODIFIED);    
    if (mDivRecordDateFieldState.getState() == MamdaFieldState.MODIFIED) 
        mDivRecordDateFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
    if (mDivCurrencyFieldState.getState() == MamdaFieldState.MODIFIED)   
        mDivCurrencyFieldState.setState(MamdaFieldState.NOT_MODIFIED);   
    if (mSharesOutFieldState.getState() == MamdaFieldState.MODIFIED)     
        mSharesOutFieldState.setState(MamdaFieldState.NOT_MODIFIED);     
    if (mSharesFloatFieldState.getState() == MamdaFieldState.MODIFIED)   
        mSharesFloatFieldState.setState(MamdaFieldState.NOT_MODIFIED);   
    if (mSharesAuthFieldState.getState() == MamdaFieldState.MODIFIED)    
        mSharesAuthFieldState.setState(MamdaFieldState.NOT_MODIFIED);    
    if (mEarnPerShareFieldState.getState() == MamdaFieldState.MODIFIED)  
        mEarnPerShareFieldState.setState(MamdaFieldState.NOT_MODIFIED);  
    if (mVolatilityFieldState.getState() == MamdaFieldState.MODIFIED)    
        mVolatilityFieldState.setState(MamdaFieldState.NOT_MODIFIED);    
    if (mPeRatioFieldState.getState() == MamdaFieldState.MODIFIED)       
        mPeRatioFieldState.setState(MamdaFieldState.NOT_MODIFIED);       
    if (mYieldFieldState.getState() == MamdaFieldState.MODIFIED)         
        mYieldFieldState.setState(MamdaFieldState.NOT_MODIFIED);         
    if (mMrktSegmNativeFieldState.getState() == MamdaFieldState.MODIFIED)
        mMrktSegmNativeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
    if (mMrktSectNativeFieldState.getState() == MamdaFieldState.MODIFIED)
        mMrktSectNativeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
    if (mMarketSegmentFieldState.getState() == MamdaFieldState.MODIFIED) 
        mMarketSegmentFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
    if (mMarketSectorFieldState.getState() == MamdaFieldState.MODIFIED)  
        mMarketSectorFieldState.setState(MamdaFieldState.NOT_MODIFIED);  
    if (mHistVolatilityFieldState.getState() == MamdaFieldState.MODIFIED)
        mHistVolatilityFieldState.setState(MamdaFieldState.NOT_MODIFIED);
    if (mRiskFreeRateFieldState.getState() == MamdaFieldState.MODIFIED)  
        mRiskFreeRateFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
    if (tmpFieldFieldState.getState() == MamdaFieldState.MODIFIED)
        tmpFieldFieldState.setState(MamdaFieldState.NOT_MODIFIED);
    }
    
    private void updateFundamentalFields (MamaMsg   msg)
    {
        /*
         * NOTE: fields which are enums can be pubished as integers if feedhandler
         * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
         * publish the numerical value as a string. All enumerated fields must be handled
         * by getting the value based on the field type.
         */

        if (msg.tryDateTime(MamdaFundamentalFields.SRC_TIME,      mSrcTimeStr))
            mSrcTimeStrFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryDateTime(MamdaFundamentalFields.ACTIVITY_TIME, mActTimeStr))
            mActTimeStrFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryDateTime(MamdaFundamentalFields.SEND_TIME,     mSendTimeStr))
            mSendTimeStrFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryDateTime(MamdaFundamentalFields.LINE_TIME,     mLineTimeStr))
            mLineTimeStrFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryString(MamdaFundamentalFields.PART_ID, mPartId))
            mPartIdFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryString(MamdaFundamentalFields.SYMBOL, mSymbol))
            mSymbolFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryString(MamdaFundamentalFields.CORP_ACT_TYPE, mCorpActType))
            mCorpActTypeFieldState.setState(MamdaFieldState.MODIFIED);

        tmpField = msg.getField (null, MamdaFundamentalFields.DIVIDEND_FREQ.getFid(), null);
        if (tmpField != null)
        {
            mDivFreq.setValue(getFieldAsString(tmpField));
            mDivFreqFieldState.setState(MamdaFieldState.MODIFIED);
        }

        if(msg.tryString(MamdaFundamentalFields.DIVIDEND_EX_DATE, mDivExDate))
            mDivExDateFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryString(MamdaFundamentalFields.DIVIDEND_PAY_DATE,mDivPayDate))
            mDivPayDateFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryString(MamdaFundamentalFields.DIVIDEND_REC_DATE,mDivRecordDate))
            mDivRecordDateFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryString(MamdaFundamentalFields.DIVIDEND_CURRENCY,mDivCurrency))
            mDivCurrencyFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryString(MamdaFundamentalFields.MRKT_SEGM_NATIVE,mMrktSegmNative))
            mMrktSegmNativeFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryString(MamdaFundamentalFields.MRKT_SECT_NATIVE,mMrktSectNative))
            mMrktSectNativeFieldState.setState(MamdaFieldState.MODIFIED);

        tmpField = msg.getField (null, MamdaFundamentalFields.MRKT_SEGMENT.getFid(), null);
        if (tmpField != null)
        {
            mMarketSegment.setValue(getFieldAsString(tmpField));
            mMarketSegmentFieldState.setState(MamdaFieldState.MODIFIED);
        }

        tmpField = msg.getField (null, MamdaFundamentalFields.MRKT_SECTOR.getFid(), null);
        if (tmpField != null)
        {
            mMarketSector.setValue(getFieldAsString(tmpField));
            mMarketSectorFieldState.setState(MamdaFieldState.MODIFIED);
        }

        if (msg.tryI64  (MamdaFundamentalFields.SHARES_OUT,     mSharesOut))
            mSharesOutFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryI64  (MamdaFundamentalFields.SHARES_FLOAT,   mSharesFloat))
            mSharesFloatFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryI64  (MamdaFundamentalFields.SHARES_AUTH,    mSharesAuth))
            mSharesAuthFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryF64(MamdaFundamentalFields.DIVIDEND_PRICE, mDividendPrice))
            mDividendPriceFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryF64(MamdaFundamentalFields.EARN_PER_SHARE,   mEarnPerShare))
            mEarnPerShareFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryF64(MamdaFundamentalFields.VOLATILITY,       mVolatility))
            mVolatilityFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryF64(MamdaFundamentalFields.PRICE_EARN_RATIO, mPeRatio))
            mPeRatioFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryF64(MamdaFundamentalFields.YIELD,            mYield))
            mYieldFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryF64(MamdaFundamentalFields.HIST_VOLATILITY,  mHistVolatility))
            mHistVolatilityFieldState.setState(MamdaFieldState.MODIFIED);

        if (msg.tryF64(MamdaFundamentalFields.RISK_FREE_RATE,   mRiskFreeRate))
            mRiskFreeRateFieldState.setState(MamdaFieldState.MODIFIED);
    }

    private String getFieldAsString(MamaMsgField field)
    {
        switch (field.getType())
        {
            case MamaFieldDescriptor.I8:
            case MamaFieldDescriptor.U8:
            case MamaFieldDescriptor.I16:
            case MamaFieldDescriptor.U16:
            case MamaFieldDescriptor.I32:
            case MamaFieldDescriptor.U32:
                return String.valueOf(field.getU32());
            case MamaFieldDescriptor.STRING:
                return field.getString();
            default:
                mLogger.fine ("Unhandled type " + field.getType() +
                              " for " + field.getName() + 
                              ".  Expected string or integer.");
                break;
        }

        return null;
    }
}
