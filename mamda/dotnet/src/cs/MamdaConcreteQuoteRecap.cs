/* $Id: MamdaConcreteQuoteRecap.cs,v 1.10.2.6 2012/08/24 16:12:11 clintonmcdowell Exp $
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

namespace Wombat
{
	/// <summary>
	/// </summary>
	public class MamdaConcreteQuoteRecap : MamdaQuoteRecap
	{
    
    	public MamdaConcreteQuoteRecap ()
		{
			mSymbol					= null;
			mPartId					= null;
			mSrcTime				= DateTime.MinValue;
			mActTime				= DateTime.MinValue;
			mLineTime				= DateTime.MinValue;
			mSendTime				= DateTime.MinValue;
			mPubId					= null;
			mBidPrice				= new MamaPrice();
			mBidSize				= 0;
			mBidDepth				= 0;
			mBidPartId				= null;
			mBidClosePrice			= new MamaPrice();
			mBidCloseDate			= DateTime.MinValue;
			mBidPrevClosePrice		= new MamaPrice();
			mBidPrevCloseDate		= DateTime.MinValue;
			mBidHigh				= new MamaPrice();
			mBidLow					= new MamaPrice();
			mAskPrice				= new MamaPrice();
			mMidPrice				= new MamaPrice();
			mAskSize				= 0;
			mAskDepth				= 0;
			mAskPartId          	= null;
			mAskClosePrice			= new MamaPrice();
			mAskCloseDate			= DateTime.MinValue;
			mAskPrevClosePrice		= new MamaPrice();
			mAskPrevCloseDate		= DateTime.MinValue;
			mAskHigh				= new MamaPrice();
			mAskLow					= new MamaPrice();
			mEventSeqNum			= 0;
			mEventTime				= DateTime.MinValue;
            mQuoteDate				= DateTime.MinValue;
			mQuoteQualStr			= null;
			mQuoteQualNative		= null;
			mTmpQuoteCount			= 0;
			mQuoteCount				= 0;
			mShortSaleBidTick		= '\0';
            mShortSaleCircuitBreaker = ' ';
            
            mAskTime				= DateTime.MinValue;
            mBidTime				= DateTime.MinValue;
            mAskIndicator			= null;
            mBidIndicator			= null;
            mAskUpdateCount			= 0;
            mBidUpdateCount			= 0;
            mAskYield				= 0;
            mBidYield				= 0;
		}
		/// <summary>
		/// Clear the recap data
		/// </summary>
		public void clear ()
		{
			mSymbol                = null;
			mPartId                = null;
			mSrcTime               = DateTime.MinValue;
			mActTime               = DateTime.MinValue;
			mLineTime              = DateTime.MinValue;
			mSendTime              = DateTime.MinValue;
			mPubId                 = null;
			mBidPrice.clear();
			mBidSize               = 0;
			mBidDepth              = 0;
			mBidPartId             = null;
			mBidClosePrice.clear();
			mBidCloseDate          = DateTime.MinValue;
			mBidPrevClosePrice.clear();
			mBidPrevCloseDate      = DateTime.MinValue;
			mBidHigh.clear();
			mBidLow.clear();
			mAskPrice.clear();
			mMidPrice.clear();
			mAskSize               = 0;
			mAskDepth              = 0;
			mAskPartId             = null;
			mAskClosePrice.clear();
			mAskCloseDate          = DateTime.MinValue;
			mAskPrevClosePrice.clear();
			mAskPrevCloseDate      = DateTime.MinValue;
			mAskHigh.clear();
			mAskLow.clear();
			mEventSeqNum           = 0;
			mEventTime             = DateTime.MinValue;
            mQuoteDate             = DateTime.MinValue;
			mQuoteQualStr          = null;
			mQuoteQualNative       = null;
			mTmpQuoteCount         = 0;
			mQuoteCount            = 0;
			mShortSaleBidTick      = '\0';
            mShortSaleCircuitBreaker = ' ';
            
            mAskTime               = DateTime.MinValue;
            mBidTime               = DateTime.MinValue;
            mAskIndicator          = null;
            mBidIndicator          = null;
            mAskUpdateCount        = 0;
            mBidUpdateCount        = 0;
            mAskYield              = 0;
            mBidYield              = 0;
		}
    
		public void setSymbol(string symbol)
		{
				mSymbol = symbol;
		}
		public string getSymbol()
		{
			return mSymbol;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSymbolFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setPartId(string partId)
		{  
            mPartId = partId;
		}
		public string getPartId()
		{
			return mPartId;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getPartIdFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setSrcTime(DateTime srcTime)
		{
				mSrcTime = srcTime;
		}
		public DateTime getSrcTime()
		{
			return mSrcTime;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSrcTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setActivityTime(DateTime activityTime)
		{
				mActTime = activityTime;
		}
		public DateTime getActivityTime ()
		{
			return mActTime;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getActivityTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setLineTime(DateTime lineTime)
		{
				mLineTime = lineTime;
		}
		public DateTime getLineTime()
		{
			return mLineTime;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getLineTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setSendTime(DateTime sendTime)
		{
				mSendTime = sendTime;
		}
		public DateTime getSendTime ()
		{
			return mSendTime;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getSendTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setPubId(string pubId)
		{
				mPubId = pubId;
		}
		public string getPubId()
		{
			return mPubId;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getPubIdFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setBidPrice(MamaPrice bidPrice )
		{
			if (bidPrice != null)
				mBidPrice.copy(bidPrice);
            else
                mBidPrice.clear();
		}
		public MamaPrice getBidPrice()
		{
			return mBidPrice;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getBidPriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setBidSize(long bidSize)
		{
			mBidSize = bidSize;
		}
		public long getBidSize()
		{
			return (long) mBidSize;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getBidSizeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setBidSize(double bidSize)
		{
			mBidSize = bidSize;
		}

		public void setBidDepth(double bidDepth)
		{
			mBidDepth = bidDepth;
		}
		public double getBidDepth()
		{
			return mBidDepth;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getBidDepthFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setBidPartId(string partId)
		{
			mBidPartId = partId;
		}
		public string getBidPartId()
		{
			return mBidPartId;
		}
        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getBidPartIdFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setBidClosePrice(MamaPrice bidClosePrice)
		{
			if (bidClosePrice != null)
				mBidClosePrice.copy(bidClosePrice);
            else
                mBidClosePrice.clear();
		}
		public MamaPrice getBidClosePrice()
		{
			return mBidClosePrice;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getBidClosePriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setBidCloseDate(DateTime bidCloseDate)
		{
				mBidCloseDate = bidCloseDate;
		}
		public DateTime getBidCloseDate ()
		{
			return mBidCloseDate;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getBidCloseDateFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setBidPrevClosePrice(MamaPrice bidPrevClosePrice)
		{
			if (bidPrevClosePrice != null)
                mBidPrevClosePrice.copy(bidPrevClosePrice);
            else
                mBidPrevClosePrice.clear();
		}
		public MamaPrice getBidPrevClosePrice()
		{
			return mBidPrevClosePrice;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getBidPrevClosePriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setBidPrevCloseDate(DateTime bidPrevCloseDate)
		{
				mBidPrevCloseDate = bidPrevCloseDate;
		}
		public DateTime getBidPrevCloseDate()
		{
			return mBidPrevCloseDate;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getBidPrevCloseDateFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setBidHigh(MamaPrice bidHigh)
		{
			if (bidHigh != null)
                mBidHigh.copy(bidHigh);
            else
                mBidHigh.clear();
		}
		public MamaPrice getBidHigh()
		{
			return mBidHigh;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getBidHighFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setBidLow(MamaPrice bidLow)
		{
			if (bidLow != null)
                mBidLow.copy(bidLow);
            else
                mBidLow.clear();
		}
		public MamaPrice getBidLow()
		{
			return mBidLow;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState FieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setAskPrice(MamaPrice askPrice)
		{
			if (askPrice != null)
                mAskPrice.copy(askPrice);
            else
                mAskPrice.clear();
		}
		public MamaPrice getAskPrice()
		{
			return mAskPrice;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getAskPriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setQuoteMidPrice(MamaPrice midPrice)
		{
			if (midPrice != null)
                mMidPrice.copy(midPrice);
            else
                mMidPrice.clear();
		}
		public MamaPrice getQuoteMidPrice()
		{
			return mMidPrice;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getQuoteMidPriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setAskSize(long askSize)
		{
			mAskSize = askSize;
		}
		public long getAskSize()
		{
			return (long) mAskSize;
		} 

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getAskSizeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setAskSize(double askSize)
		{
			mAskSize = askSize;
		}

		public void setAskDepth(double askDepth)
		{
			mAskDepth = askDepth;
		}
		public double getAskDepth()
		{
			return mAskDepth;
		}

		public void setAskPartId(string askPartId)
		{
			mAskPartId = askPartId;
		}
		public string getAskPartId()
		{
			return mAskPartId;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getAskPartIdFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setAskClosePrice(MamaPrice askClosePrice)
		{
			if (askClosePrice != null)
                mAskClosePrice.copy(askClosePrice);
            else
                mAskClosePrice.clear();
		}
		public MamaPrice getAskClosePrice()
		{
			return mAskClosePrice;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getAskClosePriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setAskCloseDate(DateTime askCloseDate)
		{
				mAskCloseDate = askCloseDate;
		}
		public DateTime getAskCloseDate()
		{
			return mAskCloseDate;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getAskCloseDateFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setAskPrevClosePrice(MamaPrice askPrevClosePrice)
		{
			if (askPrevClosePrice != null)
                mAskPrevClosePrice.copy(askPrevClosePrice);
            else
                mAskPrevClosePrice.clear();
		} 
		public MamaPrice getAskPrevClosePrice()
		{
			return mAskPrevClosePrice;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getAskPrevClosePriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setAskPrevCloseDate(DateTime askPrevCloseDate)
		{
				mAskPrevCloseDate = askPrevCloseDate;
		}
		public DateTime getAskPrevCloseDate()
		{
			return mAskPrevCloseDate;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getAskPrevCloseDateFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setAskHigh(MamaPrice askHigh)
		{
			if (askHigh != null)
                mAskHigh.copy(askHigh);
            else
                mAskHigh.clear();
		}
		public MamaPrice getAskHigh()
		{
			return mAskHigh;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getAskHighFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setAskLow(MamaPrice askLow)
		{
			if (askLow != null)
                mAskLow.copy(askLow);
            else
                mAskLow.clear();
		}
		public MamaPrice getAskLow()
		{
			return mAskLow;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getAskLowFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setEventSeqNum(long eventSeqNum)
		{
			mEventSeqNum = eventSeqNum;
		}
		public long getEventSeqNum()
		{
			return mEventSeqNum;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getEventSeqNumFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setEventTime(DateTime eventTime)
		{
				mEventTime = eventTime;
		}
		public DateTime getEventTime()
		{
			return mEventTime;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getEventTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setQuoteDate(DateTime quoteDate)
		{
				mQuoteDate = quoteDate;
		}
		public DateTime getQuoteDate()
		{
			return mQuoteDate;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getQuoteDateFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setQuoteQual(string quoteQualStr)
		{
			mQuoteQualStr = quoteQualStr; 
		}
		public string  getQuoteQual()
		{
			return mQuoteQualStr;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getQuoteQualFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setQuoteQualNative(string quoteQualNative)
		{
            mQuoteQualNative = quoteQualNative;
		}
		public string getQuoteQualNative()
		{
			return mQuoteQualNative;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getQuoteQualNativeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setTmpQuoteCount(long quoteCount)
		{
			mTmpQuoteCount = quoteCount;
		}
		public long getTmpQuoteCount()
		{
			return mTmpQuoteCount;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getTmpQuoteCountFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setQuoteCount(long quoteCount)
		{
			mQuoteCount = quoteCount;
		}
		public long getQuoteCount()
		{
			return mQuoteCount;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getQuoteCountFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public char getShortSaleBidTick()
		{
			return mShortSaleBidTick;
		}

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getShortSaleBidTickFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		public void setShortSaleBidTick(char shortSaleBidTick)
		{
			mShortSaleBidTick = shortSaleBidTick;
		}
    
        public char getShortSaleCircuitBreaker()
        {
            return mShortSaleCircuitBreaker;
        }

        /// <summary>Returns Field State, always MODIFIED
        /// </summary>
        /// <returns>Field State MODIFIED</returns>
        public MamdaFieldState getShortSaleCircuitBreakerFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setShortSaleCircuitBreaker(char shortSaleCircuitBreaker)
        {
            mShortSaleCircuitBreaker = shortSaleCircuitBreaker;
        }        

		public void setAskTime (DateTime askTime)
		{
				mAskTime = askTime;
		}
	    
		public void setBidTime (DateTime bidTime)
		{
				mBidTime = bidTime;
		}
	    
		public void setAskIndicator (string askIndicator)
		{

			mAskIndicator = askIndicator;
		}
	    
		public void setBidIndicator (string bidIndicator)
		{

			mBidIndicator = bidIndicator;
		}
	    
		public void setAskUpdateCount (long askUpdateCount)
		{
			mAskUpdateCount = askUpdateCount;
		}
	    
		public void setBidUpdateCount (long bidUpdateCount)
		{
			mBidUpdateCount = bidUpdateCount;
		}
	    
		public void setAskYield (double askYield)
		{
			mAskYield = askYield;
		}
	    
		public void setBidYield (double bidYield)
		{
			mBidYield = bidYield;
		}
	    
	    
		public DateTime getAskTime () 
		{
			return mAskTime;
		}

		/// <summary>Returns Field State, always MODIFIED
		/// </summary>
		/// <returns>Field State MODIFIED</returns>
		public MamdaFieldState getAskTimeFieldState()
		{
		return MamdaFieldState.MODIFIED;
		}

		public DateTime getBidTime () 
		{
			return mBidTime;
		}

		/// <summary>Returns Field State, always MODIFIED
		/// </summary>
		/// <returns>Field State MODIFIED</returns>
		public MamdaFieldState getBidTimeFieldState()
		{
			return MamdaFieldState.MODIFIED;
		}

		public string getAskIndicator ()
		{
			return mAskIndicator;
		}

		/// <summary>Returns Field State, always MODIFIED
		/// </summary>
		/// <returns>Field State MODIFIED</returns>
		public MamdaFieldState getAskIndicatorFieldState()
		{
			return MamdaFieldState.MODIFIED;
		}

		public string getBidIndicator ()
		{
			return mBidIndicator;
		}

		/// <summary>Returns Field State, always MODIFIED
		/// </summary>
		/// <returns>Field State MODIFIED</returns>
		public MamdaFieldState getBidIndicatorFieldState()
		{
			return MamdaFieldState.MODIFIED;
		}

		public long getAskUpdateCount () 
		{
			return mAskUpdateCount;
		}

		/// <summary>Returns Field State, always MODIFIED
		/// </summary>
		/// <returns>Field State MODIFIED</returns>
		public MamdaFieldState getAskUpdateCountFieldState()
		{
			return MamdaFieldState.MODIFIED;
		}

		public long getBidUpdateCount () 
		{
			return mBidUpdateCount;
		}

		/// <summary>Returns Field State, always MODIFIED
		/// </summary>
		/// <returns>Field State MODIFIED</returns>
		public MamdaFieldState getBidUpdateCountFieldState()
		{
			return MamdaFieldState.MODIFIED;
		}

		public double getAskYield () 
		{
			return mAskYield;
		}

			/// <summary>Returns Field State, always MODIFIED
			/// </summary>
			/// <returns>Field State MODIFIED</returns>
		public MamdaFieldState getAskYieldFieldState()
		{
			return MamdaFieldState.MODIFIED;
		}

		public double getBidYield () 
		{
			return mBidYield;
		}

		/// <summary>Returns Field State, always MODIFIED
		/// </summary>
		/// <returns>Field State MODIFIED</returns>
		public MamdaFieldState getBidYieldFieldState()
		{
			return MamdaFieldState.MODIFIED;
		}

		private string          mSymbol;
		private string          mPartId;
		private DateTime		mSrcTime;
		private DateTime		mActTime;
		private DateTime		mLineTime;
		private DateTime		mSendTime;
		private string          mPubId;
		private MamaPrice       mBidPrice;
		private double          mBidSize;
		private double          mBidDepth;
		private string          mBidPartId;
		private MamaPrice       mBidClosePrice;
		private DateTime		mBidCloseDate;
		private MamaPrice       mBidPrevClosePrice;
		private DateTime		mBidPrevCloseDate;
		private MamaPrice       mBidHigh;
		private MamaPrice       mBidLow;
		private MamaPrice       mAskPrice;
		private MamaPrice       mMidPrice;
		private double          mAskSize;
		private double          mAskDepth;
		private string          mAskPartId;
		private MamaPrice       mAskClosePrice;
		private DateTime		mAskCloseDate;
		private MamaPrice       mAskPrevClosePrice;
		private DateTime		mAskPrevCloseDate;
		private MamaPrice       mAskHigh;
		private MamaPrice       mAskLow;
		private long            mEventSeqNum;
		private DateTime		mEventTime;
        private DateTime		mQuoteDate;
		private string          mQuoteQualStr;
		private string          mQuoteQualNative;
		private long            mTmpQuoteCount;
		private long            mQuoteCount;
		private char            mShortSaleBidTick;
        private char            mShortSaleCircuitBreaker;

        private DateTime        mAskTime;
        private DateTime        mBidTime;
        private string          mAskIndicator;
        private string          mBidIndicator;
        private long            mAskUpdateCount;
        private long            mBidUpdateCount;
        private double          mAskYield;
        private double          mBidYield;
	}
}

