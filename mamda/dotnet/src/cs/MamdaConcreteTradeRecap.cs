/* $Id: MamdaConcreteTradeRecap.cs,v 1.8.2.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
    public class MamdaConcreteTradeRecap : MamdaTradeRecap
    {
        public MamdaConcreteTradeRecap ()
        {
            mSymbol            = null;
            mPartId            = null;
            mSrcTime           = DateTime.MinValue;
            mActTime           = DateTime.MinValue;
            mLineTime          = DateTime.MinValue;
            mSendTime          = DateTime.MinValue;
            mPubId             = null;
            mLastPrice         = new MamaPrice();
            mLastVolume        = 0;
            mLastPartId        = null;
            mTradeId           = null;
            mOrigTradeId       = null;
            mCorrTradeId       = null;
            mLastTime          = DateTime.MinValue;
            mTradeDate         = DateTime.MinValue;
            mIrregPrice        = new MamaPrice();
            mIrregVolume       = 0;
            mIrregPartId       = null;
            mIrregTime         = DateTime.MinValue;
            mAccVolume         = 0;
            mOffExAccVolume    = 0;
            mOnExAccVolume     = 0;
            mNetChange         = new MamaPrice();
            mPctChange         = 0.0;
            mTradeDirection    = null;          
            mSide              = null;
            mOpenPrice         = new MamaPrice();
            mHighPrice         = new MamaPrice();
            mLowPrice          = new MamaPrice();
            mClosePrice        = new MamaPrice();
            mPrevClosePrice    = new MamaPrice();
            mPrevCloseDate     = DateTime.MinValue;
            mAdjPrevClose      = new MamaPrice();
            mBlockCount        = 0;
            mBlockVolume       = 0;
            mVwap              = 0.0;
            mOffExVwap         = 0.0;
            mOnExVwap          = 0.0;
            mTotalValue        = 0.0;
            mOffExTotalValue   = 0.0;
            mOnExTotalValue    = 0.0;
            mStdDev            = 0.0;
            mStdDevSum         = 0.0;
            mStdDevSumSquares  = 0.0;
            mOrderId           = 0;
            mSettlePrice       = new MamaPrice();
            mSettleDate        = DateTime.MinValue;
            mShortSaleCircuitBreaker = ' ';
            mEventSeqNum       = 0;
            mEventTime         = DateTime.MinValue;

            mTradePrice        = new MamaPrice();
            mTradeVolume       = 0;
            mTradePartId       = null;
            mTradeQualStr      = null;
            mTradeQualNativeStr = null;
            mTradeCondStr      = null;
            mSellersSaleDays   = 0;
            mStopStockInd      = '\0';

            mTradeExecVenue    = null;
            mOffExTradePrice   = new MamaPrice();
            mOnExTradePrice    = new MamaPrice();
            
            mTradeCount        = 0;
            mTradeUnits        = null;
            mLastSeqNum        = 0;
            mHighSeqNum        = 0;
            mLowSeqNum         = 0;
            mTotalVolumeSeqNum = 0;
            mCurrencyCode      = null;

            mOrigSeqNum        = 0;
            mOrigPrice         = new MamaPrice();
            mOrigVolume        = 0;
            mOrigPartId        = null;
            mOrigQualStr       = null;
            mOrigQualNativeStr = null;
            mOrigCondStr       = null;
            mOrigSellersSaleDays = 0;
            mOrigStopStockInd  = '\0';

            mCorrPrice         = new MamaPrice();
            mCorrVolume        = 0;
            mCorrPartId        = null;
            mCorrQualStr       = null;
            mCorrQualNativeStr = null;
            mCorrCondStr       = null;
            mCorrSellersSaleDays = 0;
            mCorrStopStockInd  = '\0';
            mCancelTime        = DateTime.MinValue;
            mIsIrregular       = false;
            mIsCancel          = false;
        }

        /// <summary>
        /// Clear the recap data
        /// </summary>
        public void clear ()
        {
            mSymbol            = null;
            mPartId            = null;
            mSrcTime           = DateTime.MinValue;
            mActTime           = DateTime.MinValue;
            mLineTime          = DateTime.MinValue;
            mSendTime          = DateTime.MinValue;
            mPubId             = null;
            mLastPrice.clear();
            mLastVolume        = 0;
            mLastPartId        = null;
            mTradeId           = null;
            mOrigTradeId       = null;
            mCorrTradeId       = null;
            mLastTime          = DateTime.MinValue;
            mTradeDate         = DateTime.MinValue;
            mIrregPrice.clear();
            mIrregVolume       = 0;
            mIrregPartId       = null;
            mIrregTime         = DateTime.MinValue;
            mAccVolume         = 0;
            mOffExAccVolume    = 0;
            mOnExAccVolume     = 0;
            mNetChange.clear();
            mPctChange         = 0.0;
            mTradeDirection    = null;           
            mSide              = null;
            mOpenPrice.clear();
            mHighPrice.clear();
            mLowPrice.clear();
            mClosePrice.clear();
            mPrevClosePrice.clear();
            mPrevCloseDate     = DateTime.MinValue;
            mAdjPrevClose.clear();
            mBlockCount        = 0;
            mBlockVolume       = 0;
            mVwap              = 0.0;
            mOffExVwap         = 0.0;
            mOnExVwap          = 0.0;
            mTotalValue        = 0.0;
            mOffExTotalValue   = 0.0;
            mOnExTotalValue    = 0.0;
            mStdDev            = 0.0;
            mStdDevSum         = 0.0;
            mStdDevSumSquares  = 0.0;
            mOrderId           = 0;
            mSettlePrice.clear();
            mSettleDate        = DateTime.MinValue;
            mShortSaleCircuitBreaker = ' ';

            mEventSeqNum       = 0;
            mEventTime         = DateTime.MinValue;

            mTradePrice.clear();
            mTradeVolume       = 0;
            mTradePartId       = null;
            mTradeQualStr      = null;
            mTradeQualNativeStr = null;
            mTradeCondStr      = null;
            mSellersSaleDays   = 0;
            mStopStockInd      = '\0';

            mTradeExecVenue    = null;
            mOffExTradePrice.clear();
            mOnExTradePrice.clear();

            mTradeCount        = 0;
            mTradeUnits        = null;
            mLastSeqNum        = 0;
            mHighSeqNum        = 0;
            mLowSeqNum         = 0;
            mTotalVolumeSeqNum = 0;
            mCurrencyCode      = null;


            mOrigSeqNum        = 0;
            mOrigPrice.clear();
            mOrigVolume        = 0;
            mOrigPartId        = null;
            mOrigQualStr       = null;
            mOrigQualNativeStr = null;
            mOrigCondStr       = null;
            mOrigSellersSaleDays = 0;
            mOrigStopStockInd  = '\0';

            mCorrPrice.clear();
            mCorrVolume        = 0;
            mCorrPartId        = null;
            mCorrQualStr       = null;
            mCorrQualNativeStr = null;
            mCorrCondStr       = null;
            mCorrSellersSaleDays = 0;
            mCorrStopStockInd  = '\0';
            mCancelTime        = DateTime.MinValue;
            mIsIrregular       = false;
            mIsCancel          = false;
        }

        public string getSymbol()
        {
            return mSymbol;
        }

        public MamdaFieldState getSymbolFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setSymbol(string symbol)
        {
            this.mSymbol = symbol;
        }

        public string getPartId()
        {
            return mPartId;
        }

        public MamdaFieldState getPartIdFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setPartId(string partId)
        {
            this.mPartId = partId;
        }

        public long getAccVolume()
        {
            return (long) mAccVolume;
        }

        public MamdaFieldState getAccVolumeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setAccVolume(long accVolume)
        {
            this.mAccVolume = accVolume;
        }
        public void setAccVolume(double accVolume)
        {
            this.mAccVolume = accVolume;
        }
        
        public long getOffExAccVolume()
        {
            return (long) mOffExAccVolume;
        }

        public MamdaFieldState getOffExAccVolumeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOffExAccVolume(long offExAccVolume)
        {
            this.mOffExAccVolume = offExAccVolume;
        }
        public void setOffExAccVolume(double offExAccVolume)
        {
            this.mOffExAccVolume = offExAccVolume;
        }
        
        public long getOnExAccVolume()
        {
            return (long) mOnExAccVolume;
        }

        public MamdaFieldState getOnExAccVolumeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOnExAccVolume(long onExAccVolume)
        {
            this.mOnExAccVolume = onExAccVolume;
        }
        public void setOnExAccVolume(double onExAccVolume)
        {
            this.mOnExAccVolume = onExAccVolume;
        }
        
        public DateTime getActivityTime()
        {
            return mActTime;
        }

        public MamdaFieldState getActivityTimeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setActTime(DateTime actTime)
        {
            this.mActTime = actTime;
        }

        public DateTime getLineTime()
        {
            return mLineTime;
        }

        public MamdaFieldState getLineTimeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setLineTime(DateTime lineTime)
        {
            this.mLineTime = lineTime;
        }

        public DateTime getSendTime()
        {
            return mSendTime;
        }

        public MamdaFieldState getSendTimeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setSendTime(DateTime sendTime)
        {
            this.mSendTime = sendTime;
        }

        public string getPubId()
        {
            return mPubId;
        }

        public MamdaFieldState getPubIdFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setPubId(string pubId)
        {
			this.mPubId = pubId;
        }

        public long getBlockCount()
        {
            return mBlockCount;
        }

        public MamdaFieldState getBlockCountFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setBlockCount(long blockCount)
        {
            this.mBlockCount = blockCount;
        }

        public long getBlockVolume()
        {
            return (long) mBlockVolume;
        }

        public MamdaFieldState getBlockVolumeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setBlockVolume(long blockVolume)
        {
            this.mBlockVolume = blockVolume;
        }

        public void setBlockVolume(double blockVolume)
        {
            this.mBlockVolume = blockVolume;
        }
        
        public MamaPrice getClosePrice()
        {
            return mClosePrice;
        }

        public MamdaFieldState getClosePriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setClosePrice(MamaPrice closePrice)
        {
            if (closePrice != null)
                mClosePrice.copy(closePrice);
            else
                mClosePrice.clear();
        }

        public string getCorrCondStr()
        {
            return mCorrCondStr;
        }

        public MamdaFieldState getCorrCondStrFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setCorrCondStr(string corrCondStr)
        {
            this.mCorrCondStr = corrCondStr;
        }

        public string getCorrPartId()
        {
            return mCorrPartId;
        }

        public MamdaFieldState getCorrPartIdFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setCorrPartId(string corrPartId)
        {
            this.mCorrPartId = corrPartId;
        }

        public MamaPrice getCorrPrice()
        {
            return mCorrPrice;
        }

        public MamdaFieldState getCorrPriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setCorrPrice(MamaPrice corrPrice)
        {
            if (corrPrice != null)
                mCorrPrice.copy(corrPrice);
            else
                mCorrPrice.clear();
        }

        public string getCorrQualStr()
        {
            return mCorrQualStr;
        }

        public MamdaFieldState getCorrQualStrFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setCorrQualStr(string corrQualStr)
        {
            this.mCorrQualStr = corrQualStr;
        }

        public string getCorrQualNativeStr()
        {
            return mCorrQualNativeStr;
        }

        public MamdaFieldState getCorrQualNativeStrFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setCorrQualNativeStr(string corrQualNativeStr)
        {
            this.mCorrQualNativeStr = corrQualNativeStr;
        }

        public long getCorrSellersSaleDays()
        {
            return mCorrSellersSaleDays;
        }

        public MamdaFieldState getCorrSellersSaleDaysFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setCorrSellersSaleDays(long corrSellersSaleDays)
        {
            this.mCorrSellersSaleDays = corrSellersSaleDays;
        }

        public char getCorrStopStockInd()
        {
            return mCorrStopStockInd;
        }

        public MamdaFieldState getCorrStopStockIndFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setCorrStopStockInd(char corrStopStockInd)
        {
            this.mCorrStopStockInd = corrStopStockInd;
        }

        public DateTime getCancelTime()
        {
            return mCancelTime;
        }

        public MamdaFieldState getCancelTimeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setCancelTime(DateTime cancelTime)
        {
            this.mCancelTime = cancelTime;
        }

        public long getCorrVolume()
        {
            return (long) mCorrVolume;
        }

        public MamdaFieldState getCorrVolumeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setCorrVolume(long corrVolume)
        {
            this.mCorrVolume = corrVolume;
        }

        public void setCorrVolume(double corrVolume)
        {
            this.mCorrVolume = corrVolume;
        }
        public long getEventSeqNum()
        {
            return mEventSeqNum;
        }

        public MamdaFieldState getEventSeqNumFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setEventSeqNum(long eventSeqNum)
        {
            this.mEventSeqNum = eventSeqNum;
        }

        public DateTime getEventTime()
        {
            return mEventTime;
        }

        public MamdaFieldState getEventTimeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setEventTime(DateTime eventTime)
        {
            this.mEventTime = eventTime;
        }

        public MamaPrice getHighPrice()
        {
            return mHighPrice;
        }

        public MamdaFieldState getHighPriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setHighPrice(MamaPrice highPrice)
        {
            if (highPrice != null)
                mHighPrice.copy(highPrice);
            else
                mHighPrice.clear();
        }

        public string getLastPartId() 
        {
            return mLastPartId;
        }

        public string getTradeId() 
        {
            return mTradeId;
        }

        public string getCorrTradeId() 
        {
            return mCorrTradeId;
        }

        public string getOrigTradeId() 
        {
            return mOrigTradeId;
        }

        public MamdaFieldState getLastPartIdFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public MamdaFieldState getTradeIdFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public MamdaFieldState getOrigTradeIdFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public MamdaFieldState getCorrTradeIdFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setLastPartId(string lastPartId)
        {
            this.mLastPartId = lastPartId;
        }

        public void setTradeId(string tradeId)
        {
            this.mTradeId = tradeId;
        }
        public void setOrigTradeId(string origTradeId)
        {
            this.mOrigTradeId = origTradeId;
        }
        public void setCorrTradeId(string corrTradeId)
        {
            this.mCorrTradeId = corrTradeId;
        }

        public MamaPrice getLastPrice()
        {
            return mLastPrice;
        }

        public MamdaFieldState getLastPriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setLastPrice(MamaPrice lastPrice)
        {
            if (lastPrice != null)
                mLastPrice.copy(lastPrice);
            else
                mLastPrice.clear();
        }

        public DateTime getLastTime()
        {
            return mLastTime;
        }

        public MamdaFieldState getLastTimeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setLastTime(DateTime lastTime) 
        {
            this.mLastTime = lastTime;
        }

        public DateTime getTradeDate()
        {
            return mTradeDate;
        }

        public MamdaFieldState getTradeDateFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTradeDate(DateTime tradeDate) 
        {
            this.mTradeDate = tradeDate;
        }

        public MamaPrice getIrregPrice()
        {
            return mIrregPrice;
        }

        public MamdaFieldState getIrregPriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setIrregPrice(MamaPrice irregPrice)
        {
            if (irregPrice != null)
                mIrregPrice.copy(irregPrice);
            else
                mIrregPrice.clear();
        }

        public long getIrregVolume()
        {
            return (long) mIrregVolume;
        }

        public MamdaFieldState getIrregVolumeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setIrregVolume(long irregVolume)
        {
            this.mIrregVolume = irregVolume;
        }

        public void setIrregVolume(double irregVolume)
        {
            this.mIrregVolume = irregVolume;
        }
        
        public string getIrregPartId()
        {
            return mIrregPartId;
        }

        public MamdaFieldState getIrregPartIdFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setIrregPartId(string irregPartId)
        {
			this.mIrregPartId = irregPartId;
        }

        public DateTime getIrregTime()
        {
            return mIrregTime;
        }

        public MamdaFieldState getIrregTimeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setIrregTime(DateTime irregTime)
        {
            this.mIrregTime = irregTime;
        }

        public long getLastVolume()
        {
            return (long) mLastVolume;
        }

        public MamdaFieldState getLastVolumeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setLastVolume(long lastVolume)
        {
            this.mLastVolume = lastVolume;
        }

        public void setLastVolume(double lastVolume)
        {
            this.mLastVolume = lastVolume;
        }

        public MamaPrice getLowPrice()
        {
            return mLowPrice;
        }

        public MamdaFieldState getLowPriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setLowPrice(MamaPrice lowPrice)
        {
            if (lowPrice != null)
                mLowPrice.copy(lowPrice);
            else
                mLowPrice.clear();
        }

        public MamaPrice getNetChange()
        {
            return mNetChange;
        }

        public MamdaFieldState getNetChangeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setNetChange(MamaPrice netChange)
        {
            if (netChange != null)
                mNetChange.copy(netChange);
            else
                mNetChange.clear();
        }

        public MamaPrice getOpenPrice()
        {
            return mOpenPrice;
        }

        public MamdaFieldState getOpenPriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOpenPrice(MamaPrice openPrice)
        {
            if (openPrice != null)
                mOpenPrice.copy(openPrice);
            else
                mOpenPrice.clear();
        }

        public string getOrigCondStr()
        {
            return mOrigCondStr;
        }

        public MamdaFieldState getOrigCondStrFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOrigCondStr(string origCondStr)
        {
            this.mOrigCondStr = origCondStr;
        }

        public string getOrigPartId()
        {
            return mOrigPartId;
        }

        public MamdaFieldState getOrigPartIdFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOrigPartId(string origPartId)
        {
			this.mOrigPartId = origPartId;
        }

        public MamaPrice getOrigPrice()
        {
            return mOrigPrice;
        }

        public MamdaFieldState getOrigPriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOrigPrice(MamaPrice origPrice)
        {
            if (origPrice != null) 
                mOrigPrice.copy(origPrice);
            else
                mOrigPrice.clear();
        }

        public string getOrigQualStr() 
        {
            return mOrigQualStr;
        }

        public MamdaFieldState getOrigQualStrFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOrigQualStr(string origQualStr)
        {
			this.mOrigQualStr = origQualStr;
        }

        public string getOrigQualNativeStr()
        {
            return mOrigQualNativeStr;
        }

        public MamdaFieldState getOrigQualNativeStrFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOrigQualNativeStr(string origQualNativeStr)
        {
			this.mOrigQualNativeStr = origQualNativeStr;
        }

        public long getOrigSellersSaleDays()
        {
            return mOrigSellersSaleDays;
        }

        public MamdaFieldState getOrigSellersSaleDaysFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOrigSellersSaleDays(long origSellersSaleDays)
        {
            this.mOrigSellersSaleDays = origSellersSaleDays;
        }

        public char getOrigStopStockInd()
        {
            return mOrigStopStockInd;
        }

        public MamdaFieldState getOrigStopStockIndFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOrigStopStockInd(char origStopStockInd)
        {
            this.mOrigStopStockInd = origStopStockInd;
        }

        public long getOrigSeqNum()
        {
            return mOrigSeqNum;
        }

        public MamdaFieldState getOrigSeqNumFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOrigSeqNum(long origSeqNum)
        {
            this.mOrigSeqNum = origSeqNum;
        }

        public long getOrigVolume()
        {
            return (long) mOrigVolume;
        }

        public MamdaFieldState getOrigVolumeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setOrigVolume(long origVolume)
        {
            this.mOrigVolume = origVolume;
        }

        public void setOrigVolume(double origVolume)
        {
            this.mOrigVolume = origVolume;
        }
        
        public double getPctChange()
        {
            return mPctChange;
        }

        public MamdaFieldState getPctChangeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setPctChange(double pctChange)
        {
            this.mPctChange = pctChange;
        }

        public DateTime getPrevCloseDate()
        {
            return mPrevCloseDate;
        }

        public MamdaFieldState getPrevCloseDateFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setPrevCloseDate(DateTime prevCloseDate)
        {
            this.mPrevCloseDate = prevCloseDate;
        }

        public MamaPrice getAdjPrevClose()
        {
            return mAdjPrevClose;
        }

        public MamdaFieldState getAdjPrevCloseFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setAdjPrevClose(MamaPrice adjPrevClose)
        {
            if (adjPrevClose != null)
                mAdjPrevClose.copy(adjPrevClose);
            else
                mAdjPrevClose.clear();
        }

        public MamaPrice getPrevClosePrice()
        {
            return mPrevClosePrice;
        }

        public MamdaFieldState getPrevClosePriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setPrevClosePrice(MamaPrice prevClosePrice) 
        {
            if (prevClosePrice != null)
                mPrevClosePrice.copy(prevClosePrice);
            else
                mPrevClosePrice.clear();
        }

        public long getSellersSaleDays()
        {
            return mSellersSaleDays;
        }

        public MamdaFieldState getSellersSaleDaysFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setSellersSaleDays(long sellersSaleDays) 
        {
            this.mSellersSaleDays = sellersSaleDays;
        }

        public char getStopStockInd()
        {
            return mStopStockInd;
        }

        public MamdaFieldState getStopStockIndFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setStopStockInd(char stopStockInd)
        {
            this.mStopStockInd = stopStockInd;
        }

        public DateTime getSrcTime()
        {
            return mSrcTime;
        }

        public MamdaFieldState getSrcTimeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setSrcTime(DateTime srcTime)
        {
            this.mSrcTime = srcTime;
        }

        public double getStdDev()
        {
            return mStdDev;
        }

        public MamdaFieldState getStdDevFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setStdDev(double stdDev)
        {
            this.mStdDev = stdDev;
        }

        public double getStdDevSum()
        {
            return mStdDevSum;
        }

        public MamdaFieldState getStdDevSumFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setStdDevSum(double stdDevSum)
        {
            this.mStdDevSum = stdDevSum;
        }

        public double getStdDevSumSquares()
        {
            return mStdDevSumSquares;
        }
        
        public MamdaFieldState getStdDevSumSquaresFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }
        
        public void setStdDevSumSquares(double stdDevSumSquares)
        {
            this.mStdDevSumSquares = stdDevSumSquares;
        }

        public long getOrderId()
        {
            return mOrderId;
        }

        public MamdaFieldState getOrderIdFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setOrderId(long orderId)
        {
            this.mOrderId = orderId;
        }

        public MamaPrice getSettlePrice()
        {
            return mSettlePrice;
        }

        public MamdaFieldState getSettlePriceFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setSettlePrice(MamaPrice settlePrice)
        {
            if (settlePrice != null)
                mSettlePrice.copy(settlePrice);
            else
                mSettlePrice.clear();
        }

        public DateTime getSettleDate()
        {
            return mSettleDate;
        }

        public MamdaFieldState getSettleDateFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setSettleDate(DateTime settleDate) 
        {
            this.mSettleDate = settleDate;
        }
		
		/// <summary>
		/// getShortSaleCircuitBreaker Returns the ShortSaleCircuitBreaker
		/// </summary>
		/// <returns>the ShortSaleCircuitBreaker</returns>
        public char getShortSaleCircuitBreaker()
        {
            return mShortSaleCircuitBreaker;
        }

		/// <summary>
		/// Returns the FieldState, always MODIFIED.
		/// </summary>
		/// <returns>the FieldState</returns>
        public MamdaFieldState getShortSaleCircuitBreakerFieldState()
        {
            return MamdaFieldState.MODIFIED;
        }

		/// <summary>
		/// Set the shortSaleCircuitBreaker
		/// </summary>
		/// <param name="shortSaleCircuitBreaker"></param>
        public void setShortSaleCircuitBreaker(char shortSaleCircuitBreaker)
        {  
            mShortSaleCircuitBreaker = shortSaleCircuitBreaker;
        }       

        public double getTotalValue()
        {
            return mTotalValue;
        }

        public MamdaFieldState getTotalValueFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTotalValue(double totalValue)
        {
            this.mTotalValue = totalValue;
        }

        public double getOffExTotalValue()
        {
            return mOffExTotalValue;
        }

        public MamdaFieldState getOffExTotalValueFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOffExTotalValue(double offExTotalValue)
        {
            this.mOffExTotalValue = offExTotalValue;
        }

        public double getOnExTotalValue()
        {
            return mOnExTotalValue;
        }

        public MamdaFieldState getOnExTotalValueFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOnExTotalValue(double onExTotalValue)
        {
            this.mOnExTotalValue = onExTotalValue;
        }

        public string getTradeCondStr()
        {
            return mTradeCondStr;
        }

        public MamdaFieldState getTradeCondStrFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTradeCondStr(string tradeCondStr)
        {
            this.mTradeCondStr = tradeCondStr;
        }

        public long getTradeCount() 
        {
            return mTradeCount;
        }

        public MamdaFieldState getTradeCountFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setTradeCount(long tradeCount) 
        {
            this.mTradeCount = tradeCount;
        }

        public string getTradeUnits() 
        {
            return mTradeUnits;
        }

        public MamdaFieldState getTradeUnitsFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTradeUnits(string tradeUnits) 
        {
            this.mTradeUnits = tradeUnits;
        }

        public long getLastSeqNum() 
        {
            return mLastSeqNum;
        }

        public MamdaFieldState getLastSeqNumFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setLastSeqNum(long lastSeqNum) 
        {
            this.mLastSeqNum = lastSeqNum;
        }

        public long getHighSeqNum() 
        {
            return mHighSeqNum;
        }

        public MamdaFieldState getHighSeqNumFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setHighSeqNum(long highSeqNum) 
        {
            this.mHighSeqNum = highSeqNum;
        }

        public long getLowSeqNum() 
        {
            return mLowSeqNum;
        }

        public MamdaFieldState getLowSeqNumFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setLowSeqNum(long lowSeqNum) 
        {
            this.mLowSeqNum = lowSeqNum;
        }

        public long getTotalVolumeSeqNum() 
        {
            return mTotalVolumeSeqNum;
        }

        public MamdaFieldState getTotalVolumeSeqNumFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTotalVolumeSeqNum(long totalVolumeSeqNum) 
        {
            this.mTotalVolumeSeqNum = totalVolumeSeqNum;
        }

        public string getCurrencyCode()
        {
            return mCurrencyCode;
        }

        public MamdaFieldState getCurrencyCodeFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setCurrencyCode(string currencyCode)
        {
            this.mCurrencyCode = currencyCode;
        }

        public string getTradeDirection()
        {
            return mTradeDirection;
        }

        public MamdaFieldState getTradeDirectionFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTradeDirection(string tradeDirection)
        {
            this.mTradeDirection = tradeDirection;
        }

        public string getSide()
        {
            return mSide;
        }

        public MamdaFieldState getSideFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setSide(string tradeSide)
        {           
            mSide = tradeSide;
        }

        public string getTradePartId()
        {
            return mTradePartId;
        }

        public MamdaFieldState getTradePartIdFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTradePartId(string tradePartId) 
        {
			this.mTradePartId = tradePartId;
        }

        public MamaPrice getTradePrice()
        {
            return mTradePrice;
        }

        public MamdaFieldState getTradePriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTradePrice(MamaPrice tradePrice) 
        {
            if (tradePrice != null)
                mTradePrice.copy(tradePrice);
            else
                mTradePrice.clear();
        }

        public string getTradeQualStr()
        {
            return mTradeQualStr;
        }

        public MamdaFieldState getTradeQualStrFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTradeQualStr(string tradeQualStr)
        {
            this.mTradeQualStr = tradeQualStr;
        }

        public string getTradeQualNativeStr()
        {
            return mTradeQualNativeStr;
        }

        public MamdaFieldState getTradeQualNativeStrFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTradeQualNativeStr(string tradeQualNativeStr)
        {
            this.mTradeQualNativeStr = tradeQualNativeStr;
        }

        public long getTradeVolume()
        {
            return (long) mTradeVolume;
        }

        public MamdaFieldState getTradeVolumeFieldState()
        {
          return MamdaFieldState.MODIFIED;
        }

        public void setTradeVolume(long tradeVolume)
        {
            this.mTradeVolume = tradeVolume;
        }
        
        public void setTradeVolume(double tradeVolume)
        {
            this.mTradeVolume = tradeVolume;
        }
        
        public double getVwap()
        {
            return mVwap;
        }

        public MamdaFieldState getVwapFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setVwap(double vwap)
        {
            this.mVwap = vwap;
        }

        public double getOffExVwap()
        {
            return mOffExVwap;
        }

        public MamdaFieldState getOffExVwapFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOffExVwap(double offExVwap)
        {
            this.mOffExVwap = offExVwap;
        }

        public double getOnExVwap()
        {
            return mOnExVwap;
        }

        public MamdaFieldState getOnExVwapFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOnExVwap(double onExVwap)
        {
            this.mOnExVwap = onExVwap;
        }

        public bool IsIrregular()
        {
            return mIsIrregular;
        }

        public void setIsIrregular(bool isIrregular)
        {
            mIsIrregular = isIrregular;
        }

        public bool IsCancel()
        {
            return mIsCancel;
        }

        public void setIsCancel(bool isCancel)
        {
            mIsCancel = isCancel;
        }

        public string getTradeExecVenue()
        {
            return mTradeExecVenue;
        }

        public MamdaFieldState getTradeExecVenueFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setTradeExecVenue(string tradeExecVenue)
        {
            this.mTradeExecVenue = tradeExecVenue;
        }

        public MamaPrice getOffExchangeTradePrice()
        {
            return mOffExTradePrice;
        }

        public MamdaFieldState getOffExchangeTradePriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOffExchangeTradePrice(MamaPrice offExTradePrice) 
        {
            if (offExTradePrice != null)
                mOffExTradePrice.copy(offExTradePrice);
            else
                mOffExTradePrice.clear();
        }

        public MamaPrice getOnExchangeTradePrice()
        {
            return mOnExTradePrice;
        }

        public MamdaFieldState getOnExchangeTradePriceFieldState()
        {
			return MamdaFieldState.MODIFIED;
        }

        public void setOnExchangeTradePrice(MamaPrice onExTradePrice) 
        {
            if (onExTradePrice != null)
                mOnExTradePrice.copy(onExTradePrice);
            else
                mOnExTradePrice.clear();
        }

        #region Implementation details

        private string          mSymbol				= null;
        private string          mPartId				= null;
        private DateTime        mSrcTime			= DateTime.MinValue;
        private DateTime        mActTime			= DateTime.MinValue;
        private DateTime        mLineTime			= DateTime.MinValue;
        private DateTime        mSendTime			= DateTime.MinValue;
        private string          mPubId				= null;
        private MamaPrice       mLastPrice			= null;
        private double          mLastVolume			= 0;
        private string          mLastPartId			= null;
        private string          mTradeId			= null;
        private string          mOrigTradeId		= null;
        private string          mCorrTradeId		= null;
        private DateTime        mLastTime			= DateTime.MinValue;
        private DateTime        mTradeDate			= DateTime.MinValue;
        private MamaPrice       mIrregPrice			= null;
        private double          mIrregVolume		= 0;
        private string          mIrregPartId		= null;
        private DateTime        mIrregTime			= DateTime.MinValue;
        private double          mAccVolume			= 0;
        private double          mOffExAccVolume		= 0;
        private double          mOnExAccVolume		= 0;
        private MamaPrice       mNetChange			= null;
        private double          mPctChange			= 0.0;
        private string          mTradeDirection		= null;
        private string          mSide				= null;
        private MamaPrice       mOpenPrice			= null;
        private MamaPrice       mHighPrice			= null;
        private MamaPrice       mLowPrice			= null;
        private MamaPrice       mClosePrice			= null;
        private MamaPrice       mPrevClosePrice		= null;
        private DateTime        mPrevCloseDate		= DateTime.MinValue;
        private MamaPrice       mAdjPrevClose		= null;
        private long            mBlockCount			= 0;
        private double          mBlockVolume		= 0;
        private double          mVwap				= 0.0;
        private double          mOffExVwap			= 0.0;
        private double          mOnExVwap			= 0.0;
        private double          mTotalValue			= 0.0;
        private double          mOffExTotalValue	= 0.0;
        private double          mOnExTotalValue		= 0.0;
        private double          mStdDev				= 0.0;
        private double          mStdDevSum			= 0.0;
        private double          mStdDevSumSquares	= 0.0;
        private long            mOrderId			= 0;
        private MamaPrice       mSettlePrice		= null;
        private DateTime        mSettleDate			= DateTime.MinValue;
        private char            mShortSaleCircuitBreaker;

        // The following fields are used for caching event-related fields:
        private long            mEventSeqNum		= 0;
        private DateTime        mEventTime			= DateTime.MinValue;

        // The following fields are used for caching the last reported
        // trade, which might have been out of order.  The reason for
        // cahcing these values is to allow a configuration that passes
        // the minimum amount of data  (unchanged fields not sent).
        private MamaPrice       mTradePrice			= null;
        private double          mTradeVolume		= 0;
        private string          mTradePartId		= null;
        private string          mTradeQualStr		= null;
        private string          mTradeQualNativeStr = null;
        private string          mTradeCondStr		= null;
        private long            mSellersSaleDays	= 0;
        private char            mStopStockInd		= '\0';

        private string          mTradeExecVenue		= null;
        private MamaPrice       mOffExTradePrice	= null;
        private MamaPrice       mOnExTradePrice		= null;
        
        private long            mTradeCount			= 0;
        private string          mTradeUnits			= null;
        private long            mLastSeqNum			= 0;
        private long            mHighSeqNum			= 0;
        private long            mLowSeqNum			= 0;
        private long            mTotalVolumeSeqNum	= 0;
        private string          mCurrencyCode		= null;
        
        private long            mOrigSeqNum			= 0;
        private MamaPrice       mOrigPrice			= null;
        private double          mOrigVolume			= 0;
        private string          mOrigPartId			= null;
        private string          mOrigQualStr		= null;
        private string          mOrigQualNativeStr	= null;
        private string          mOrigCondStr		= null;
        private long            mOrigSellersSaleDays = 0;
        private char            mOrigStopStockInd	= '\0';

        private MamaPrice       mCorrPrice			= null;
        private double          mCorrVolume			= 0;
        private string          mCorrPartId			= null;
        private string          mCorrQualStr		= null;
        private string          mCorrQualNativeStr	= null;
        private string          mCorrCondStr		= null;
        private long            mCorrSellersSaleDays = 0;
        private char            mCorrStopStockInd	= '\0';
        private DateTime        mCancelTime			= DateTime.MinValue;
        private bool            mIsIrregular		= false;
        private bool            mIsCancel			= false;

        #endregion Implementation details
    }
}

