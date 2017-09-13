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

using System;

namespace Wombat
{
	/// <summary>
	/// </summary>
	public class MamdaConcreteOrderImbalanceRecap
	{
		public MamdaConcreteOrderImbalanceRecap()
		{
		}

		/// <summary>
		/// Clear the recap data
		/// </summary>
		public void clear()
		{
			mHighIndicationPrice = null;
			mLowIndicationPrice  = null;
			mIndicationPrice     = null;
			mBuyVolume           = 0;
			mSellVolume          = 0;
			mMatchVolume         = 0;
			mSecurityStatusQual  = null;
			mInsideMatchPrice    = null;
			mFarClearingPrice    = null;
			mNearClearingPrice   = null;
			mNoClearingPrice     = '\0';
			mPriceVarInd         = '\0';
			mCrossType           = '\0';
			mEventTime           = DateTime.MinValue;
			mEventSeqNum         = 0;
			mSrcTime             = DateTime.MinValue;
			mActTime             = DateTime.MinValue;
			mMsgType             = 0;
			mIssueSymbol         = null;
			mPartId              = null;
			mSeqNum              = 0;
			mSecurityStatusOrig  = null;
			mSecurityStatusTime  = DateTime.MinValue;
			mAuctionTime         = DateTime.MinValue;
			mSymbol				 = null;
		}

		/// <summary>
		/// <see cref="MamdaBasicRecap.getActivityTime"/>
		/// </summary>
		/// <returns></returns>
		public DateTime getActivityTime()
		{
			return mActTime;
		}

        /// <summary>
        /// <see cref="MamdaBasicRecap.getActivityTimeFieldState"/>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getActivityTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		/// </summary>
		/// <param name="actTime"></param>
		public void setActivityTime(DateTime actTime)
		{
			if (actTime != DateTime.MinValue)
				this.mActTime = actTime;
		}

		/// <summary>
		/// <see cref="MamdaBasicRecap.getLineTime"/>
		/// </summary>
		/// <returns></returns>
		public DateTime getLineTime()
		{
			return mLineTime;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getLineTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="lineTime"></param>
		public void setLineTime(DateTime lineTime)
		{
			if (lineTime != DateTime.MinValue)
				this.mLineTime = lineTime;
		}

		/// <summary>
		/// <see cref="MamdaBasicRecap.getActivityTime"/>
		/// </summary>
		/// <returns></returns>
		public DateTime getSendTime()
		{
			return mSendTime;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSendTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="sendTime"></param>
        public void setSendTime(DateTime sendTime)
		{
			if (sendTime != DateTime.MinValue)
				this.mSendTime = sendTime;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public DateTime getAuctionTime()
		{
			return mAuctionTime;
		}

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getAuctionTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="auctionTime"></param>
		public void setAuctionTime(DateTime auctionTime)
		{
			if (auctionTime != DateTime.MinValue)
				this.mAuctionTime = auctionTime;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public long getBuyVolume()
		{
			return mBuyVolume;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getBuyVolumeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="buyVolume"></param>
		public void setBuyVolume(long buyVolume)
		{
			this.mBuyVolume = buyVolume;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public char getCrossType()
		{
			return mCrossType;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getCrossTypeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="crossType"></param>
		public void setCrossType(char crossType)
		{
			this.mCrossType = crossType;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public long getEventSeqNum()
		{
			return mEventSeqNum;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getEventSeqNumFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="eventSeqNum"></param>
		public void setEventSeqNum(long eventSeqNum)
		{
			this.mEventSeqNum = eventSeqNum;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public DateTime getEventTime()
		{
			return mEventTime;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getEventTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="eventTime"></param>
		public void setEventTime(DateTime eventTime)
		{
			if (mEventTime != DateTime.MinValue)
				this.mEventTime = eventTime;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public MamaPrice getFarClearingPrice()
		{
			return mFarClearingPrice;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getFarClearingPriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="farClearingPrice"></param>
		public void setFarClearingPrice(MamaPrice farClearingPrice)
		{
			if (null != farClearingPrice)
				this.mFarClearingPrice = new MamaPrice(farClearingPrice);
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public MamaPrice getHighIndicationPrice()
		{
			return mHighIndicationPrice;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getHighIndicationPriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="highIndicationPrice"></param>
		public void setHighIndicationPrice(MamaPrice highIndicationPrice)
		{
			if (null != highIndicationPrice)
				this.mHighIndicationPrice = new MamaPrice(highIndicationPrice);
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public MamaPrice getImbalancePrice()
		{
			return mIndicationPrice;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getImbalancePriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="indicationPrice"></param>
		public void setImbalancePrice(MamaPrice indicationPrice)
		{
			if (null != indicationPrice)
				this.mIndicationPrice = new MamaPrice(indicationPrice);
		}


		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public MamaPrice getMatchPrice()
		{
			return mInsideMatchPrice;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getMatchPriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="insideMatchPrice"></param>
		public void setMatchPrice(MamaPrice insideMatchPrice)
		{
			if (null != insideMatchPrice)
				this.mInsideMatchPrice = new MamaPrice(insideMatchPrice);
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public string getIssueSymbol()
		{
			return mIssueSymbol;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getIssueSymbolFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="issueSymbol"></param>
		public void setIssueSymbol(String issueSymbol)
		{
			if (issueSymbol != null)
				this.mIssueSymbol = issueSymbol;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public string getSymbol()
		{
			return mSymbol;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSymbolFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="symbol"></param>
        public void setSymbol(String symbol)
		{
			if (symbol != null)
				this.mSymbol = symbol;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public MamaPrice getLowIndicationPrice()
		{
			return mLowIndicationPrice;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getLowIndicationPriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="lowIndicationPrice"></param>
		public void setLowIndicationPrice(MamaPrice lowIndicationPrice)
		{
			if (null != lowIndicationPrice)
				this.mLowIndicationPrice = new MamaPrice(lowIndicationPrice);
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public long getMatchVolume()
		{
			return mMatchVolume;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getMatchVolumeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="matchVolume"></param>
		public void setMatchVolume(long matchVolume)
		{
			this.mMatchVolume = matchVolume;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public int getMsgType()
		{
			return mMsgType;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getMsgTypeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="msgType"></param>
		public void setMsgType(int msgType)
		{
			this.mMsgType = msgType;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public MamaPrice getNearClearingPrice()
		{
			return mNearClearingPrice;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getNearClearingPriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="nearClearingPrice"></param>
		public void setNearClearingPrice(MamaPrice nearClearingPrice)
		{
			if (null != nearClearingPrice)
				this.mNearClearingPrice = new MamaPrice(nearClearingPrice);
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public char getNoClearingPrice()
		{
			return mNoClearingPrice;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getNoClearingPriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="noClearingPrice"></param>
		public void setNoClearingPrice(char noClearingPrice)
		{
			this.mNoClearingPrice = noClearingPrice;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public string getPartId()
		{
			return mPartId;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getPartIdFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="partId"></param>
		public void setPartId(String partId)
		{
			if (null != mPartId)
				this.mPartId = partId;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public char getPriceVarInd()
		{
			return mPriceVarInd;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getPriceVarIndFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="priceVarInd"></param>
		public void setPriceVarInd(char priceVarInd)
		{
			this.mPriceVarInd = priceVarInd;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public String getSecurityStatusOrig()
		{
			return mSecurityStatusOrig;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSecurityStatusOrigFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="securityStatusOrig"></param>
		public void setSecurityStatusOrig(string securityStatusOrig)
		{
			if (null != securityStatusOrig)
				this.mSecurityStatusOrig = securityStatusOrig;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public string getImbalanceState()
		{
			return mSecurityStatusQual;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getImbalanceStateFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="securityStatusQual"></param>
		public void setImbalanceState(string securityStatusQual)
		{
			if (null != securityStatusQual)
				this.mSecurityStatusQual = securityStatusQual;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public DateTime getSecurityStatusTime()
		{
			return mSecurityStatusTime;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSecurityStatusTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="securityStatusTime"></param>
		public void setSecurityStatusTime(DateTime securityStatusTime)
		{
			if (mSecurityStatusTime != DateTime.MinValue)
				this.mSecurityStatusTime = securityStatusTime;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public long getSellVolume()
		{
			return mSellVolume;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSellVolumeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="sellVolume"></param>
		public void setSellVolume(long sellVolume)
		{
			this.mSellVolume = sellVolume;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public int getSeqNum()
		{
			return mSeqNum;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSeqNumFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="seqNum"></param>
		public void setSeqNum(int seqNum)
		{
			this.mSeqNum = seqNum;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		public DateTime getSrcTime()
		{
			return mSrcTime;
		}

        /// <summary>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSrcTimeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		///
		/// </summary>
		/// <param name="srcTime">the datetime to set.</param>
		public void setSrcTime(DateTime srcTime)
		{
			if (mSrcTime != DateTime.MinValue)
				this.mSrcTime = srcTime;
		}

		#region State

		private MamaPrice    mHighIndicationPrice = null;
		private MamaPrice    mLowIndicationPrice  = null;
		private MamaPrice    mIndicationPrice     = null;
		private long         mBuyVolume;
		private long         mSellVolume;
		private long         mMatchVolume;
		private String       mSecurityStatusQual;
		private MamaPrice    mInsideMatchPrice    = null;
		private MamaPrice    mFarClearingPrice    = null;
		private MamaPrice    mNearClearingPrice   = null;
		private char         mNoClearingPrice;
		private char         mPriceVarInd;
		private char         mCrossType;
		private DateTime	 mEventTime           = DateTime.MinValue;
		private long         mEventSeqNum;
		private DateTime	 mSrcTime             = DateTime.MinValue;
		private DateTime	 mActTime             = DateTime.MinValue;
		private DateTime	 mSendTime			  = DateTime.MinValue;
		private DateTime     mLineTime			  = DateTime.MinValue;

		private int          mMsgType;
		private String       mIssueSymbol         = null;
		private String		 mSymbol			  = null;
		private String       mPartId              = null;
		private int          mSeqNum;
		private String       mSecurityStatusOrig  = null;
		private DateTime	 mSecurityStatusTime  = DateTime.MinValue;
		private DateTime	 mAuctionTime         = DateTime.MinValue;

		#endregion State
	}
}
