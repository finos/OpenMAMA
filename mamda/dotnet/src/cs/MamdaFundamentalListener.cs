/* $Id: MamdaFundamentalListener.cs,v 1.7.30.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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

using System;
using System.Collections;

namespace Wombat
{
	/// <summary>
	/// MamdaFundamentalListener is a class that specializes in handling
	/// fundamental equity pricing/analysis attributes, indicators and
	/// ratios.  Developers provide their own implementation of the
	/// MamdaFundamentalHandler interface and will be delivered
	/// notifications for updates in the fundamental data.  An obvious
	/// application for this MAMDA class is any kind of pricing analysis
	/// application.
	/// </summary>
	/// <remarks>
	/// Note: The MamdaFundamentalListener class caches equity
	/// pricing/analysis attributes, indicators and ratios. Among other
	/// reasons, caching of these fields makes it possible to provide
	/// complete fundamental callbacks, even when the publisher (e.g., feed
	/// handler) is only publishing deltas containing modified fields.
	/// </remarks>
	public class MamdaFundamentalListener : MamdaMsgListener, MamdaFundamentals
	{
		/// <summary>
		/// Create a specialized fundamental field listener.
		/// </summary>
		public MamdaFundamentalListener()
		{
		}

		/// <summary>
		/// Add a specialized fundamentals handler.
		/// </summary>
		/// <param name="handler"></param>
		public void addHandler(MamdaFundamentalHandler handler)
		{
			mHandlers.Add(handler);
		} 

		public DateTime getSrcTime()
		{
			return mSrcTimeStr;
		}

		public DateTime getActivityTime()
		{
			return mActTimeStr;
		}

		public string getCorporateActionType()
		{
			return mCorpActType;
		}

		public double getDividendPrice()
		{
			return mDividendPrice;
		}

		public string getDividendFrequency()
		{
			return mDivFreq;
		}

		public string getDividendExDate()
		{
			return mDivExDate;
		}

		public string getDividendPayDate()
		{
			return mDivPayDate;
		}

		public string getDividendRecordDate()
		{
			return mDivRecordDate;
		}

		public string getDividendCurrency()
		{
			return mDivCurrency;
		}

		public long getSharesOut()
		{
			return mSharesOut;
		}

		public long getSharesFloat()
		{
			return mSharesFloat;
		}

		public long getSharesAuthorized()
		{
			return mSharesAuth;
		}

		public double getEarningsPerShare()
		{
			return mEarnPerShare;
		}

		public double getVolatility()
		{
			return mVolatility;
		}

		public double getPriceEarningsRatio()
		{
			return mPeRatio;
		}

		public double getYield()
		{
			return mYield;
		}

		public string getMarketSegmentNative()
		{
			return mMrktSegmNative;
		}

		public string getMarketSectorNative()
		{
			return mMrktSectNative;
		}

		public string getMarketSegment()
		{
			return mMarketSegment;
		}

		public string getMarketSector()
		{
			return mMarketSector;
		}

		public double getHistoricalVolatility()
		{
			return mHistVolatility;
		}

		public double getRiskFreeRate()
		{
			return mRiskFreeRate;
		}
        
        public MamdaFieldState getSrcTimeFieldState()
        {
			return mSrcTimeStrFieldState;
        }

        public MamdaFieldState getActivityTimeFieldState()
        {
			return mActTimeStrFieldState;
        }

        public MamdaFieldState getCorporateActionTypeFieldState()
        {
			return mCorpActTypeFieldState;
        }

        public MamdaFieldState getDividendPriceFieldState()
        {
			return mDividendPriceFieldState;
        }

        public MamdaFieldState getDividendFrequencyFieldState()
        {
			return mDivFreqFieldState;
        }

        public MamdaFieldState getDividendExDateFieldState()
        {
			return mDivExDateFieldState;
        }

        public MamdaFieldState getDividendPayDateFieldState()
        {
			return mDivPayDateFieldState;
        }

        public MamdaFieldState getDividendRecordDateFieldState()
        {
			return mDivRecordDateFieldState;
        }

        public MamdaFieldState getDividendCurrencyFieldState()
        {
			return mDivCurrencyFieldState;
        }

        public MamdaFieldState getSharesOutFieldState()
        {
			return mSharesOutFieldState;
        }

        public MamdaFieldState getSharesFloatFieldState()
        {
			return mSharesFloatFieldState;
        }

        public MamdaFieldState getSharesAuthorizedFieldState()
        {
			return mSharesAuthFieldState;
        }

        public MamdaFieldState getEarningsPerShareFieldState()
        {
			return mEarnPerShareFieldState;
        }

        public MamdaFieldState getVolatilityFieldState()
        {
			return mVolatilityFieldState;
        }

        public MamdaFieldState getPriceEarningsRatioFieldState()
        {
			return mPeRatioFieldState;
        }

        public MamdaFieldState getYieldFieldState()
        {
			return mYieldFieldState;
        }

        public MamdaFieldState getMarketSegmentNativeFieldState()
        {
			return mMrktSegmNativeFieldState;
        }

        public MamdaFieldState getMarketSectorNativeFieldState()
        {
			return mMrktSectNativeFieldState;
        }

        public MamdaFieldState getMarketSegmentFieldState()
        {
			return mMarketSegmentFieldState;
        }

        public MamdaFieldState getMarketSectorFieldState()
        {
			return mMarketSectorFieldState;
        }

        public MamdaFieldState getHistoricalVolatilityFieldState()
        {
			return mHistVolatilityFieldState;
        }

        public MamdaFieldState getRiskFreeRateFieldState()
        {
			return mRiskFreeRateFieldState;
        }

		/// <summary>
		/// Implementation of MamdaListener interface.
		/// </summary>
		/// <param name="subscription"></param>
		/// <param name="msg"></param>
		/// <param name="msgType"></param>
		public void onMsg(
			MamdaSubscription subscription,
			MamaMsg           msg,
			mamaMsgType       msgType)
		{
			if (!MamdaFundamentalFields.isSet())
			{
				return;
			}
			// If msg is a trade-related message, invoke the
			// appropriate callback:
			switch (msgType)
			{
				case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
				case mamaMsgType.MAMA_MSG_TYPE_RECAP:
					handleRecap (subscription, msg);
					break;
				case mamaMsgType.MAMA_MSG_TYPE_UPDATE:
					handleUpdate (subscription, msg);
					break;
			}
		}

		private void handleRecap(
			MamdaSubscription  subscription,
			MamaMsg            msg)
		{
            updateFieldStates();
            updateFundamentalFields(msg);
			foreach (MamdaFundamentalHandler handler in mHandlers)
			{
				handler.onFundamentals(subscription, this, msg, this);
			}
		}

		private void handleUpdate(
			MamdaSubscription  subscription,
			MamaMsg            msg)
		{
			// Same handling as recap (for now, at least)
			handleRecap(subscription, msg);
		}

        private void updateFieldStates()
        {
          if (mCorpActTypeFieldState == MamdaFieldState.MODIFIED)       mCorpActTypeFieldState = MamdaFieldState.NOT_MODIFIED;
          if (mDividendPriceFieldState == MamdaFieldState.MODIFIED)     mDividendPriceFieldState = MamdaFieldState.NOT_MODIFIED; 
          if (mDivFreqFieldState == MamdaFieldState.MODIFIED)           mDivFreqFieldState = MamdaFieldState.NOT_MODIFIED;       
          if (mDivExDateFieldState == MamdaFieldState.MODIFIED)         mDivExDateFieldState = MamdaFieldState.NOT_MODIFIED;     
          if (mDivPayDateFieldState == MamdaFieldState.MODIFIED)        mDivPayDateFieldState = MamdaFieldState.NOT_MODIFIED;    
          if (mDivRecordDateFieldState == MamdaFieldState.MODIFIED)     mDivRecordDateFieldState = MamdaFieldState.NOT_MODIFIED; 
          if (mDivCurrencyFieldState == MamdaFieldState.MODIFIED)       mDivCurrencyFieldState = MamdaFieldState.NOT_MODIFIED;   
          if (mSharesOutFieldState == MamdaFieldState.MODIFIED)         mSharesOutFieldState = MamdaFieldState.NOT_MODIFIED;     
          if (mSharesFloatFieldState == MamdaFieldState.MODIFIED)       mSharesFloatFieldState = MamdaFieldState.NOT_MODIFIED;   
          if (mSharesAuthFieldState == MamdaFieldState.MODIFIED)        mSharesAuthFieldState = MamdaFieldState.NOT_MODIFIED;    
          if (mEarnPerShareFieldState == MamdaFieldState.MODIFIED)      mEarnPerShareFieldState = MamdaFieldState.NOT_MODIFIED;  
          if (mVolatilityFieldState == MamdaFieldState.MODIFIED)        mVolatilityFieldState = MamdaFieldState.NOT_MODIFIED;    
          if (mPeRatioFieldState == MamdaFieldState.MODIFIED)           mPeRatioFieldState = MamdaFieldState.NOT_MODIFIED;       
          if (mYieldFieldState == MamdaFieldState.MODIFIED)             mYieldFieldState = MamdaFieldState.NOT_MODIFIED;         
          if (mMrktSegmNativeFieldState == MamdaFieldState.MODIFIED)    mMrktSegmNativeFieldState = MamdaFieldState.NOT_MODIFIED;
          if (mMrktSectNativeFieldState == MamdaFieldState.MODIFIED)    mMrktSectNativeFieldState = MamdaFieldState.NOT_MODIFIED;
          if (mMarketSegmentFieldState == MamdaFieldState.MODIFIED)     mMarketSegmentFieldState = MamdaFieldState.NOT_MODIFIED; 
          if (mMarketSectorFieldState == MamdaFieldState.MODIFIED)      mMarketSectorFieldState = MamdaFieldState.NOT_MODIFIED;  
          if (mHistVolatilityFieldState == MamdaFieldState.MODIFIED)    mHistVolatilityFieldState = MamdaFieldState.NOT_MODIFIED;
          if (mRiskFreeRateFieldState == MamdaFieldState.MODIFIED)      mRiskFreeRateFieldState = MamdaFieldState.NOT_MODIFIED;  
        }

        private void updateFundamentalFields(MamaMsg msg)
        {
            /*
            * NOTE: fields which are enums can be pubished as integers if feedhandler
            * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
            * publish the numerical value as a string. All enumerated fields must be handled
            * by getting the value based on the field type.
            */

            if (msg.tryDateTime(MamdaFundamentalFields.SRC_TIME, ref mSrcTimeStr))
              mSrcTimeStrFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryDateTime(MamdaFundamentalFields.ACTIVITY_TIME, ref mActTimeStr))
              mActTimeStrFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryString(MamdaFundamentalFields.CORP_ACT_TYPE, ref mCorpActType))
              mCorpActTypeFieldState = MamdaFieldState.MODIFIED;
            
            if(msg.tryField (MamdaFundamentalFields.DIVIDEND_FREQ, ref mTmpfield))
            {
                mDivFreq = getFieldAsString(mTmpfield);
                mDivFreqFieldState = MamdaFieldState.MODIFIED;
            }

            if (msg.tryString(MamdaFundamentalFields.DIVIDEND_EX_DATE, ref mDivExDate))
              mDivExDateFieldState = MamdaFieldState.MODIFIED;
            
            if(msg.tryString(MamdaFundamentalFields.DIVIDEND_PAY_DATE, ref mDivPayDate))
              mDivPayDateFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryString(MamdaFundamentalFields.DIVIDEND_REC_DATE, ref mDivRecordDate))
              mDivRecordDateFieldState = MamdaFieldState.MODIFIED;
            
            if(msg.tryString(MamdaFundamentalFields.DIVIDEND_CURRENCY, ref mDivCurrency))
              mDivCurrencyFieldState = MamdaFieldState.MODIFIED;

            if (msg.tryString(MamdaFundamentalFields.MRKT_SEGM_NATIVE, ref mMrktSegmNative))
              mMrktSegmNativeFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryString(MamdaFundamentalFields.MRKT_SECT_NATIVE, ref mMrktSectNative))
              mMrktSectNativeFieldState = MamdaFieldState.MODIFIED;

            if(msg.tryField (MamdaFundamentalFields.MRKT_SEGMENT, ref mTmpfield))
            {
                mMarketSegment = getFieldAsString(mTmpfield);
                mMarketSegmentFieldState = MamdaFieldState.MODIFIED;
            }

            if(msg.tryField (MamdaFundamentalFields.MRKT_SECTOR, ref mTmpfield))
            {
                mMarketSector = getFieldAsString(mTmpfield);
                mMarketSectorFieldState = MamdaFieldState.MODIFIED;
            }

            if (msg.tryI64  (MamdaFundamentalFields.SHARES_OUT, ref mSharesOut))
              mSharesOutFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryI64  (MamdaFundamentalFields.SHARES_FLOAT, ref mSharesFloat))
              mSharesFloatFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryI64  (MamdaFundamentalFields.SHARES_AUTH, ref mSharesAuth))
              mSharesAuthFieldState = MamdaFieldState.MODIFIED;

            if (msg.tryF64(MamdaFundamentalFields.DIVIDEND_PRICE, ref mDividendPrice))
              mDividendPriceFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryF64(MamdaFundamentalFields.EARN_PER_SHARE, ref mEarnPerShare))
              mEarnPerShareFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryF64(MamdaFundamentalFields.VOLATILITY, ref mVolatility))
              mVolatilityFieldState = MamdaFieldState.MODIFIED;

            if (msg.tryF64(MamdaFundamentalFields.PRICE_EARN_RATIO, ref mPeRatio))
              mPeRatioFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryF64(MamdaFundamentalFields.YIELD, ref mYield))
              mYieldFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryF64(MamdaFundamentalFields.HIST_VOLATILITY, ref mHistVolatility))
              mHistVolatilityFieldState = MamdaFieldState.MODIFIED;
            
            if (msg.tryF64(MamdaFundamentalFields.RISK_FREE_RATE, ref mRiskFreeRate))
              mRiskFreeRateFieldState = MamdaFieldState.MODIFIED;
        }

        public string getFieldAsString(MamaMsgField field)
        {
            switch (field.getType())
            {
                case mamaFieldType.MAMA_FIELD_TYPE_I8:
                case mamaFieldType.MAMA_FIELD_TYPE_U8:
                case mamaFieldType.MAMA_FIELD_TYPE_I16:
                case mamaFieldType.MAMA_FIELD_TYPE_U16:
                case mamaFieldType.MAMA_FIELD_TYPE_I32:
                case mamaFieldType.MAMA_FIELD_TYPE_U32:
                    return field.getU32().ToString();
                case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                    return field.getString();
                default:
                    break;
            }
			return null;
        }

		private readonly ArrayList	mHandlers       = new ArrayList();
		private DateTime			mSrcTimeStr     = DateTime.MinValue;
		private DateTime			mActTimeStr     = DateTime.MinValue;
        private MamdaFieldState  mSrcTimeStrFieldState    = new MamdaFieldState();
        private MamdaFieldState  mActTimeStrFieldState    = new MamdaFieldState();
        
		// Fundamental Fields 
		// The following fields are used for caching the last reported
		// fundamental equity pricing/analysis attributes, indicators and ratios.
		// The reason for cahcing these values is to allow a configuration that 
		// passes the minimum amount of data  (unchanged fields not sent).
        private string      mCorpActType    = null;     private MamdaFieldState  mCorpActTypeFieldState    = new MamdaFieldState();
        private double      mDividendPrice  = 0.0;      private MamdaFieldState  mDividendPriceFieldState  = new MamdaFieldState();
        private string      mDivFreq        = null;     private MamdaFieldState  mDivFreqFieldState        = new MamdaFieldState();
        private string      mDivExDate      = null;     private MamdaFieldState  mDivExDateFieldState      = new MamdaFieldState();
        private string      mDivPayDate     = null;     private MamdaFieldState  mDivPayDateFieldState     = new MamdaFieldState();
        private string      mDivRecordDate  = null;     private MamdaFieldState  mDivRecordDateFieldState  = new MamdaFieldState();
        private string      mDivCurrency    = null;     private MamdaFieldState  mDivCurrencyFieldState    = new MamdaFieldState();
        private long        mSharesOut      = 0;        private MamdaFieldState  mSharesOutFieldState      = new MamdaFieldState();
        private long        mSharesFloat    = 0;        private MamdaFieldState  mSharesFloatFieldState    = new MamdaFieldState();
        private long        mSharesAuth     = 0;        private MamdaFieldState  mSharesAuthFieldState     = new MamdaFieldState();
        private double      mEarnPerShare   = 0.0;      private MamdaFieldState  mEarnPerShareFieldState   = new MamdaFieldState();
        private double      mVolatility     = 0.0;      private MamdaFieldState  mVolatilityFieldState     = new MamdaFieldState();
        private double      mPeRatio        = 0.0;      private MamdaFieldState  mPeRatioFieldState        = new MamdaFieldState();
        private double      mYield          = 0.0;      private MamdaFieldState  mYieldFieldState          = new MamdaFieldState();
        private string      mMrktSegmNative = null;     private MamdaFieldState  mMrktSegmNativeFieldState = new MamdaFieldState();
        private string      mMrktSectNative = null;     private MamdaFieldState  mMrktSectNativeFieldState = new MamdaFieldState();
        private string      mMarketSegment  = null;     private MamdaFieldState  mMarketSegmentFieldState  = new MamdaFieldState();
        private string      mMarketSector   = null;     private MamdaFieldState  mMarketSectorFieldState   = new MamdaFieldState();
        private double      mHistVolatility = 0.0;      private MamdaFieldState  mHistVolatilityFieldState = new MamdaFieldState();
        private double      mRiskFreeRate   = 0.0;      private MamdaFieldState  mRiskFreeRateFieldState   = new MamdaFieldState();
		
		// Additional general fields (future):
        private MamaMsgField mTmpfield      = new MamaMsgField();
	}
}
