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
using System.Collections;
using System.Threading;

namespace Wombat
{
    /// <summary>
    /// MamdaTradeListener is a class that specializes in handling trade
    /// updates.  Developers provide their own implementation of the
    /// MamdaTradeHandler interface and will be delivered notifications for
    /// trades, trade cancels/error/corrections, and trade closing prices.
    /// An obvious application for this MAMDA class is any kind of trade
    /// tick capture application.
    /// </summary>
    /// <remarks>
    /// Note: The MamdaTradeListener class caches trade-related field
    /// values.  Among other reasons, caching of these fields makes it
    /// possible to provide complete trade-related callbacks, even when the
    /// publisher (e.g., feed handler) is only publishing deltas containing
    /// modified fields.
    /// </remarks>
    public class MamdaTradeListener :
        MamdaMsgListener,
        MamdaTradeRecap,
        MamdaTradeReport,
        MamdaTradeGap,
        MamdaTradeCancelOrError,
        MamdaTradeCorrection,
        MamdaTradeClosing
    {
        /// <summary>
        /// Create a specialized trade listener.  This listener handles
        /// trade reports, trade recaps, trade errors/cancels, trade
        /// corrections, and trade gap notifications.
        /// </summary>
        public MamdaTradeListener()
        {
            clearCache(mTradeCache);

            mFieldIterator = new FieldIterator(this);
        }

        private static void clearCache(MamdaTradeCache cache)
        {
            cache.mSymbol = null;
            cache.mIssueSymbol = null;
            cache.mPartId = null;
            cache.mSrcTime = DateTime.MinValue;
            cache.mActTime = DateTime.MinValue;
            cache.mLineTime = DateTime.MinValue;
            cache.mSendTime = DateTime.MinValue;
            cache.mPubId = null;
            cache.mIsIrregular = false;
            cache.mLastPrice = new MamaPrice();
            cache.mLastVolume = 0;
            cache.mLastPartId = null;
            cache.mTradeId = null;
            cache.mOrigTradeId = null;
            cache.mCorrTradeId = null;
            cache.mLastTime = DateTime.MinValue;
            cache.mTradeDate = DateTime.MinValue;
            cache.mIrregPrice = new MamaPrice();
            cache.mIrregVolume = 0;
            cache.mIrregPartId = null;
            cache.mIrregTime = DateTime.MinValue;
            cache.mAccVolume = 0;
            cache.mOffExAccVolume = 0;
            cache.mOnExAccVolume = 0;
            cache.mTradeDirection = null;
            cache.mTmpSide.setState((short)MamdaTradeSide.mamdaTradeSide.TRADE_SIDE_UNKNOWN);
            cache.mNetChange = new MamaPrice();
            cache.mPctChange = 0.0;
            cache.mOpenPrice = new MamaPrice();
            cache.mHighPrice = new MamaPrice();
            cache.mLowPrice = new MamaPrice();
            cache.mClosePrice = new MamaPrice();
            cache.mPrevClosePrice = new MamaPrice();
            cache.mPrevCloseDate = DateTime.MinValue;
            cache.mAdjPrevClose = new MamaPrice();
            cache.mTradeCount = 0;
            cache.mBlockVolume = 0;
            cache.mBlockCount = 0;
            cache.mVwap = 0.0;
            cache.mOffExVwap = 0.0;
            cache.mOnExVwap = 0.0;
            cache.mTotalValue = 0.0;
            cache.mOffExTotalValue = 0.0;
            cache.mOnExTotalValue = 0.0;
            cache.mStdDev = 0.0;
            cache.mStdDevSum = 0.0;
            cache.mStdDevSumSquares = 0.0;
            cache.mOrderId = 0;
            cache.mSettlePrice = new MamaPrice();
            cache.mSettleDate = DateTime.MinValue;
            cache.mShortSaleCircuitBreaker = ' ';
            cache.mOrigShortSaleCircuitBreaker = ' ';
            cache.mCorrShortSaleCircuitBreaker = ' ';
            cache.mEventSeqNum = 0;
            cache.mEventTime = DateTime.MinValue;
            cache.mTradePrice = new MamaPrice();
            cache.mTradeVolume = 0;
            cache.mTradePartId = null;
            cache.mTradeQualStr = null;
            cache.mTradeQualNativeStr = null;
            cache.mSellersSaleDays = 0;
            cache.mStopStockInd = '\0';
            cache.mTradeExecVenue = null;
            cache.mOffExTradePrice = new MamaPrice();
            cache.mOnExTradePrice = new MamaPrice();
            cache.mIsCancel = false;
            cache.mOrigSeqNum = 0;
            cache.mOrigPrice = new MamaPrice();
            cache.mOrigVolume = 0;
            cache.mOrigPartId = null;
            cache.mOrigQualStr = null;
            cache.mOrigQualNativeStr = null;
            cache.mOrigSellersSaleDays = 0;
            cache.mOrigStopStockInd = '\0';
            cache.mCorrPrice = new MamaPrice();
            cache.mCorrVolume = 0;
            cache.mCorrPartId = null;
            cache.mCorrQualStr = null;
            cache.mCorrQualNativeStr = null;
            cache.mCorrSellersSaleDays = 0;
            cache.mCorrStopStockInd = '\0';
            cache.mCorrTime = DateTime.MinValue;
            cache.mCancelTime = DateTime.MinValue;
            cache.mTradeCondStr = null;
            cache.mOrigCondStr = null;
            cache.mCorrCondStr = null;
            cache.mTradeUnits = null;
            cache.mLastSeqNum = 0;
            cache.mHighSeqNum = 0;
            cache.mLowSeqNum = 0;
            cache.mTotalVolumeSeqNum = 0;
            cache.mCurrencyCode = null;
            cache.mConflateCount = 1;

            cache.mGotPartId = false;
            cache.mLastGenericMsgWasTrade = false;
            cache.mGotTradeTime = false;
            cache.mGotTradePrice = false;
            cache.mGotTradeSize = false;
            cache.mGotTradeCount = false;

            mProcessUpdateAsTrade = true;
            //Field State
            cache.mSymbolFieldState = new MamdaFieldState();
            cache.mIssueSymbolFieldState = new MamdaFieldState();
            cache.mPartIdFieldState = new MamdaFieldState();
            cache.mSrcTimeFieldState = new MamdaFieldState();
            cache.mActTimeFieldState = new MamdaFieldState();
            cache.mLineTimeFieldState = new MamdaFieldState();
            cache.mSendTimeFieldState = new MamdaFieldState();
            cache.mPubIdFieldState = new MamdaFieldState();
            cache.mIsIrregularFieldState = new MamdaFieldState();
            cache.mLastPriceFieldState = new MamdaFieldState();
            cache.mLastVolumeFieldState = new MamdaFieldState();
            cache.mLastPartIdFieldState = new MamdaFieldState();
            cache.mTradeIdFieldState = new MamdaFieldState();
            cache.mOrigTradeIdFieldState = new MamdaFieldState();
            cache.mCorrTradeIdFieldState = new MamdaFieldState();
            cache.mLastTimeFieldState = new MamdaFieldState();
            cache.mTradeDateFieldState = new MamdaFieldState();
            cache.mIrregPriceFieldState = new MamdaFieldState();
            cache.mIrregVolumeFieldState = new MamdaFieldState();
            cache.mIrregPartIdFieldState = new MamdaFieldState();
            cache.mIrregTimeFieldState = new MamdaFieldState();
            cache.mAccVolumeFieldState = new MamdaFieldState();
            cache.mOffExAccVolumeFieldState = new MamdaFieldState();
            cache.mOnExAccVolumeFieldState = new MamdaFieldState();
            cache.mTradeDirectionFieldState = MamdaFieldState.NOT_INITIALISED;
            cache.mSideFieldState = MamdaFieldState.NOT_INITIALISED;
            cache.mNetChangeFieldState = new MamdaFieldState();
            cache.mPctChangeFieldState = new MamdaFieldState();
            cache.mOpenPriceFieldState = new MamdaFieldState();
            cache.mHighPriceFieldState = new MamdaFieldState();
            cache.mLowPriceFieldState = new MamdaFieldState();
            cache.mClosePriceFieldState = new MamdaFieldState();
            cache.mPrevClosePriceFieldState = new MamdaFieldState();
            cache.mPrevCloseDateFieldState = new MamdaFieldState();
            cache.mAdjPrevCloseFieldState = new MamdaFieldState();
            cache.mTradeCountFieldState = new MamdaFieldState();
            cache.mBlockVolumeFieldState = new MamdaFieldState();
            cache.mBlockCountFieldState = new MamdaFieldState();
            cache.mVwapFieldState = new MamdaFieldState();
            cache.mOffExVwapFieldState = new MamdaFieldState();
            cache.mOnExVwapFieldState = new MamdaFieldState();
            cache.mTotalValueFieldState = new MamdaFieldState();
            cache.mOffExTotalValueFieldState = new MamdaFieldState();
            cache.mOnExTotalValueFieldState = new MamdaFieldState();
            cache.mStdDevFieldState = new MamdaFieldState();
            cache.mStdDevSumFieldState = new MamdaFieldState();
            cache.mStdDevSumSquaresFieldState = new MamdaFieldState();
            cache.mOrderIdFieldState = new MamdaFieldState();
            cache.mSettlePriceFieldState = new MamdaFieldState();
            cache.mSettleDateFieldState = new MamdaFieldState();
            cache.mShortSaleCircuitBreakerFieldState = new MamdaFieldState();
            cache.mOrigShortSaleCircuitBreakerFieldState = new MamdaFieldState();
            cache.mCorrShortSaleCircuitBreakerFieldState = new MamdaFieldState();
            cache.mEventSeqNumFieldState = new MamdaFieldState();
            cache.mEventTimeFieldState = new MamdaFieldState();
            cache.mTradePriceFieldState = new MamdaFieldState();
            cache.mTradeVolumeFieldState = new MamdaFieldState();
            cache.mTradePartIdFieldState = new MamdaFieldState();
            cache.mTradeQualStrFieldState = new MamdaFieldState();
            cache.mTradeQualNativeStrFieldState = new MamdaFieldState();
            cache.mSellersSaleDaysFieldState = new MamdaFieldState();
            cache.mStopStockIndFieldState = new MamdaFieldState();
            cache.mTradeExecVenueFieldState = new MamdaFieldState();
            cache.mOffExTradePriceFieldState = new MamdaFieldState();
            cache.mOnExTradePriceFieldState = new MamdaFieldState();
            cache.mOrigSeqNumFieldState = new MamdaFieldState();
            cache.mOrigPriceFieldState = new MamdaFieldState();
            cache.mOrigVolumeFieldState = new MamdaFieldState();
            cache.mOrigPartIdFieldState = new MamdaFieldState();
            cache.mOrigQualStrFieldState = new MamdaFieldState();
            cache.mOrigQualNativeStrFieldState = new MamdaFieldState();
            cache.mOrigSellersSaleDaysFieldState = new MamdaFieldState();
            cache.mOrigStopStockIndFieldState = new MamdaFieldState();
            cache.mCorrPriceFieldState = new MamdaFieldState();
            cache.mCorrVolumeFieldState = new MamdaFieldState();
            cache.mCorrPartIdFieldState = new MamdaFieldState();
            cache.mCorrQualStrFieldState = new MamdaFieldState();
            cache.mCorrQualNativeStrFieldState = new MamdaFieldState();
            cache.mCorrSellersSaleDaysFieldState = new MamdaFieldState();
            cache.mCorrStopStockIndFieldState = new MamdaFieldState();
            cache.mCorrTimeFieldState = new MamdaFieldState();
            cache.mCancelTimeFieldState = new MamdaFieldState();
            cache.mTradeCondStrFieldState = new MamdaFieldState();
            cache.mOrigCondStrFieldState = new MamdaFieldState();
            cache.mCorrCondStrFieldState = new MamdaFieldState();
            cache.mTradeUnitsFieldState = new MamdaFieldState();
            cache.mLastSeqNumFieldState = new MamdaFieldState();
            cache.mHighSeqNumFieldState = new MamdaFieldState();
            cache.mLowSeqNumFieldState = new MamdaFieldState();
            cache.mTotalVolumeSeqNumFieldState = new MamdaFieldState();
            cache.mCurrencyCodeFieldState = new MamdaFieldState();
            cache.mConflateCountFieldState = new MamdaFieldState();
        }

        /// <summary>
        /// Add a specialized trade handler.  Currently, only one
        /// handler can (and must) be registered.
        /// </summary>
        /// <param name="handler"></param>
        public void addHandler(MamdaTradeHandler handler)
        {
            mHandlers.Add(handler);
        }

        /// <summary>
        /// <see ref="MamdaBasicEvent.getSymbol()" />
        /// </summary>
        public String getSymbol()
        {
            return mTradeCache.mSymbol;
        }

        /// <summary>
        ///
        /// </summary>
        public String getPartId()
        {
            return mTradeCache.mPartId;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getSrcTime()" />
        /// </summary>
        public DateTime getSrcTime()
        {
            return mTradeCache.mSrcTime;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getActivityTime()" />
        /// </summary>
        public DateTime getActivityTime()
        {
            return mTradeCache.mActTime;
        }

        /// <summary>
        ///
        /// </summary>
        public DateTime getLineTime()
        {
            return mTradeCache.mLineTime;
        }

        /// <summary>
        ///
        /// </summary>
        public DateTime getSendTime()
        {
            return mTradeCache.mSendTime;
        }

        /// <summary>
        ///
        /// </summary>
        public String getPubId()
        {
            return mTradeCache.mPubId;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getEventSeqNum()" />
        /// </summary>
        public long getEventSeqNum()
        {
            return mTradeCache.mEventSeqNum;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getEventTime()" />
        /// </summary>
        public DateTime getEventTime()
        {
            return mTradeCache.mEventTime;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastPrice()" />
        /// </summary>
        public MamaPrice getLastPrice()
        {
            return mTradeCache.mLastPrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastVolume()" />
        /// </summary>
        public long getLastVolume()
        {
            return (long)mTradeCache.mLastVolume;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastPartId()" />
        /// </summary>
        public String getLastPartId()
        {
            return mTradeCache.mLastPartId;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeId()" />
        /// </summary>
        public String getTradeId()
        {
            return mTradeCache.mTradeId;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigTradeId()" />
        /// </summary>
        public String getOrigTradeId()
        {
            return mTradeCache.mOrigTradeId;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrTradeId()" />
        /// </summary>
        public String getCorrTradeId()
        {
            return mTradeCache.mCorrTradeId;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastTime()" />
        /// </summary>
        public DateTime getLastTime()
        {
            return mTradeCache.mLastTime;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getIrregPrice()" />
        /// </summary>
        public MamaPrice getIrregPrice()
        {
            return mTradeCache.mIrregPrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getIrregVolume()" />
        /// </summary>
        public long getIrregVolume()
        {
            return (long)mTradeCache.mIrregVolume;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getIrregPartId()" />
        /// </summary>
        public String getIrregPartId()
        {
            return mTradeCache.mIrregPartId;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getIrregTime()" />
        /// </summary>
        public DateTime getIrregTime()
        {
            return mTradeCache.mIrregTime;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastTime()" />
        /// </summary>
        public DateTime getTradeDate()
        {
            return mTradeCache.mTradeDate;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getAccVolume()" />
        /// </summary>
        public long getAccVolume()
        {
            return (long)mTradeCache.mAccVolume;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOffExAccVolume()" />
        /// </summary>
        public long getOffExAccVolume()
        {
            return (long)mTradeCache.mOffExAccVolume;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOnExAccVolume()" />
        /// </summary>
        public long getOnExAccVolume()
        {
            return (long)mTradeCache.mOnExAccVolume;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getNetChange()" />
        /// </summary>
        public MamaPrice getNetChange()
        {
            return mTradeCache.mNetChange;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getPctChange()" />
        /// </summary>
        public double getPctChange()
        {
            return mTradeCache.mPctChange;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTradeDirection()" />
        /// </summary>
        public String getTradeDirection()
        {
            return mTradeCache.mTradeDirection;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getSide()" />
        /// </summary>
        public string getSide()
        {
            return mTradeCache.mSide;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOpenPrice()" />
        /// </summary>
        public MamaPrice getOpenPrice()
        {
            return mTradeCache.mOpenPrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getHighPrice()" />
        /// </summary>
        public MamaPrice getHighPrice()
        {
            return mTradeCache.mHighPrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLowPrice()" />
        /// </summary>
        public MamaPrice getLowPrice()
        {
            return mTradeCache.mLowPrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getClosePrice()" />
        /// </summary>
        public MamaPrice getClosePrice()
        {
            return mTradeCache.mClosePrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getPrevClosePrice()" />
        /// </summary>
        public MamaPrice getPrevClosePrice()
        {
            return mTradeCache.mPrevClosePrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getPrevCloseDate()" />
        /// </summary>
        public DateTime getPrevCloseDate()
        {
            return mTradeCache.mPrevCloseDate;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getAdjPrevClose()" />
        /// </summary>
        public MamaPrice getAdjPrevClose()
        {
            return mTradeCache.mAdjPrevClose;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getBlockCount()" />
        /// </summary>
        public long getBlockCount()
        {
            return mTradeCache.mBlockCount;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getBlockVolume()" />
        /// </summary>
        public long getBlockVolume()
        {
            return (long)mTradeCache.mBlockVolume;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getVwap()" />
        /// </summary>
        public double getVwap()
        {
            return mTradeCache.mVwap;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOffExVwap()" />
        /// </summary>
        public double getOffExVwap()
        {
            return mTradeCache.mOffExVwap;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOnExVwap()" />
        /// </summary>
        public double getOnExVwap()
        {
            return mTradeCache.mOnExVwap;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTotalValue()" />
        /// </summary>
        public double getTotalValue()
        {
            return mTradeCache.mTotalValue;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOffExTotalValue()" />
        /// </summary>
        public double getOffExTotalValue()
        {
            return mTradeCache.mOffExTotalValue;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOnExTotalValue()" />
        /// </summary>
        public double getOnExTotalValue()
        {
            return mTradeCache.mOnExTotalValue;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getStdDev()" />
        /// </summary>
        public double getStdDev()
        {
            return mTradeCache.mStdDev;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getStdDevSum()" />
        /// </summary>
        public double getStdDevSum()
        {
            return mTradeCache.mStdDevSum;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getStdDevSumSquares()" />
        /// </summary>
        public double getStdDevSumSquares()
        {
            return mTradeCache.mStdDevSumSquares;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOrderId" />
        /// </summary>
        public long getOrderId()
        {
            return mTradeCache.mOrderId;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getSettlePrice()" />
        /// </summary>
        public MamaPrice getSettlePrice()
        {
            return mTradeCache.mSettlePrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getSettleDate()" />
        /// </summary>
        public DateTime getSettleDate()
        {
            return mTradeCache.mSettleDate;
        }

        public char getShortSaleCircuitBreaker()
        {
            return mTradeCache.mShortSaleCircuitBreaker;
        }

        public char getOrigShortSaleCircuitBreaker()
        {
            return mTradeCache.mOrigShortSaleCircuitBreaker;
        }

        public char getCorrShortSaleCircuitBreaker()
        {
            return mTradeCache.mCorrShortSaleCircuitBreaker;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradePrice()" />
        /// </summary>
        public MamaPrice getTradePrice()
        {
            return mTradeCache.mTradePrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeVolume()" />
        /// </summary>
        public long getTradeVolume()
        {
            return (long)mTradeCache.mTradeVolume;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradePartId()" />
        /// </summary>
        public String getTradePartId()
        {
            return mTradeCache.mTradePartId;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeQual()" />
        /// </summary>
        public String getTradeQual()
        {
            return mTradeCache.mTradeQualStr;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeQualNativeStr()" />
        /// </summary>
        public String getTradeQualNativeStr()
        {
            return mTradeCache.mTradeQualNativeStr;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeCondition()" />
        /// </summary>
        public String getTradeCondition()
        {
            return mTradeCache.mTradeCondStr;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeSellersSaleDays()" />
        /// </summary>
        public long getTradeSellersSaleDays()
        {
            return mTradeCache.mSellersSaleDays;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeStopStock()" />
        /// </summary>
        public char getTradeStopStock()
        {
            return mTradeCache.mStopStockInd;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTradeExecVenue()" />
        /// </summary>
        public String getTradeExecVenue()
        {
            return mTradeCache.mTradeExecVenue;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOffExchangeTradePrice()" />
        /// </summary>
        public MamaPrice getOffExchangeTradePrice()
        {
            return mTradeCache.mOffExTradePrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOnExchangeTradePrice()" />
        /// </summary>
        public MamaPrice getOnExchangeTradePrice()
        {
            return mTradeCache.mOnExTradePrice;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTradeCount()" />
        /// </summary>
        public long getTradeCount()
        {
            return mTradeCache.mTradeCount;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTradeUnits()" />
        /// </summary>
        public string getTradeUnits()
        {
            return mTradeCache.mTradeUnits;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastSeqNum()" />
        /// </summary>
        public long getLastSeqNum()
        {
            return mTradeCache.mLastSeqNum;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getHighSeqNum()" />
        /// </summary>
        public long getHighSeqNum()
        {
            return mTradeCache.mHighSeqNum;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLowSeqNum()" />
        /// </summary>
        public long getLowSeqNum()
        {
            return mTradeCache.mLowSeqNum;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTotalVolumeSeqNum()" />
        /// </summary>
        public long getTotalVolumeSeqNum()
        {
            return mTradeCache.mTotalVolumeSeqNum;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getCurrencyCode()" />
        /// </summary>
        public string getCurrencyCode()
        {
            return mTradeCache.mCurrencyCode;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getIsIrregular()" />
        /// </summary>
        public bool getIsIrregular()
        {
            return mTradeCache.mIsIrregular;
        }

        /// <summary>
        /// <see cref="MamdaTradeGap.getBeginGapSeqNum()" />
        /// </summary>
        public long getBeginGapSeqNum()
        {
            return mGapBegin;
        }

        /// <summary>
        /// <see cref="MamdaTradeGap.getEndGapSeqNum()" />
        /// </summary>
        public long getEndGapSeqNum()
        {
            return mGapEnd;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getIsCancel()" />
        /// </summary>
        public bool getIsCancel()
        {
            return mTradeCache.mIsCancel;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigSeqNum()" />
        /// </summary>
        public long getOrigSeqNum()
        {
            return mTradeCache.mOrigSeqNum;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigPrice()" />
        /// </summary>
        public double getOrigPrice()
        {
            return mTradeCache.mOrigPrice.getValue();
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigVolume()" />
        /// </summary>
        public long getOrigVolume()
        {
            return (long)mTradeCache.mOrigVolume;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigPartId()" />
        /// </summary>
        public String getOrigPartId()
        {
            return mTradeCache.mOrigPartId;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigQual()" />
        /// </summary>
        public String getOrigQual()
        {
            return mTradeCache.mOrigQualStr;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigQualNative()" />
        /// </summary>
        public String getOrigQualNative()
        {
            return mTradeCache.mOrigQualNativeStr;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigCondition()" />
        /// </summary>
        public String getOrigCondition()
        {
            return mTradeCache.mOrigCondStr;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigSellersSaleDays()" />
        /// </summary>
        public long getOrigSellersSaleDays()
        {
            return mTradeCache.mOrigSellersSaleDays;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigStopStock()" />
        /// </summary>
        public char getOrigStopStock()
        {
            return mTradeCache.mOrigStopStockInd;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrPrice()" />
        /// </summary>
        public double getCorrPrice()
        {
            return mTradeCache.mCorrPrice.getValue();
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrVolume()" />
        /// </summary>
        public long getCorrVolume()
        {
            return (long)mTradeCache.mCorrVolume;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrPartId()" />
        /// </summary>
        public String getCorrPartId()
        {
            return mTradeCache.mCorrPartId;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrQual()" />
        /// </summary>
        public String getCorrQual()
        {
            return mTradeCache.mCorrQualStr;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrQualNative()" />
        /// </summary>
        public String getCorrQualNative()
        {
            return mTradeCache.mCorrQualNativeStr;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrCondition()" />
        /// </summary>
        public String getCorrCondition()
        {
            return mTradeCache.mCorrCondStr;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrSellersSaleDays()" />
        /// </summary>
        public long getCorrSellersSaleDays()
        {
            return mTradeCache.mCorrSellersSaleDays;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrStopStock()" />
        /// </summary>
        public char getCorrStopStock()
        {
            return mTradeCache.mCorrStopStockInd;
        }

        /// <summary>
        /// <see cref="MamdaTradeClosing.getIsIndicative()" />
        /// </summary>
        public bool getIsIndicative()
        {
            return mIsIndicative.Value;
        }

        /// <summary>
        /// <see ref="MamdaBasicEvent.getSymbolFieldState()" />
        /// </summary>
        public MamdaFieldState getSymbolFieldState()
        {
            return mTradeCache.mSymbolFieldState;
        }

        /// <summary>
        ///
        /// </summary>
        public MamdaFieldState getPartIdFieldState()
        {
            return mTradeCache.mPartIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getSrcTimeFieldState()" />
        /// </summary>
        public MamdaFieldState getSrcTimeFieldState()
        {
            return mTradeCache.mSrcTimeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getActivityTimeFieldState()" />
        /// </summary>
        public MamdaFieldState getActivityTimeFieldState()
        {
            return mTradeCache.mActTimeFieldState;
        }

        /// <summary>
        ///
        /// </summary>
        public MamdaFieldState getLineTimeFieldState()
        {
            return mTradeCache.mLineTimeFieldState;
        }

        /// <summary>
        ///
        /// </summary>
        public MamdaFieldState getSendTimeFieldState()
        {
            return mTradeCache.mSendTimeFieldState;
        }

        /// <summary>
        ///
        /// </summary>
        public MamdaFieldState getPubIdFieldState()
        {
            return mTradeCache.mPubIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getEventSeqNumFieldState()" />
        /// </summary>
        public MamdaFieldState getEventSeqNumFieldState()
        {
            return mTradeCache.mEventSeqNumFieldState;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getEventTimeFieldState()" />
        /// </summary>
        public MamdaFieldState getEventTimeFieldState()
        {
            return mTradeCache.mEventTimeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastPriceFieldState()" />
        /// </summary>
        public MamdaFieldState getLastPriceFieldState()
        {
            return mTradeCache.mLastPriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastVolumeFieldState()" />
        /// </summary>
        public MamdaFieldState getLastVolumeFieldState()
        {
            return mTradeCache.mLastVolumeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastPartIdFieldState()" />
        /// </summary>
        public MamdaFieldState getLastPartIdFieldState()
        {
            return mTradeCache.mLastPartIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeIdFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeIdFieldState()
        {
            return mTradeCache.mTradeIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigTradeIdFieldState()" />
        /// </summary>
        public MamdaFieldState getOrigTradeIdFieldState()
        {
            return mTradeCache.mOrigTradeIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrTradeIdFieldState()" />
        /// </summary>
        public MamdaFieldState getCorrTradeIdFieldState()
        {
            return mTradeCache.mCorrTradeIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastTimeFieldState()" />
        /// </summary>
        public MamdaFieldState getLastTimeFieldState()
        {
            return mTradeCache.mLastTimeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getIrregPriceFieldState()" />
        /// </summary>
        public MamdaFieldState getIrregPriceFieldState()
        {
            return mTradeCache.mIrregPriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getIrregVolumeFieldState()" />
        /// </summary>
        public MamdaFieldState getIrregVolumeFieldState()
        {
            return mTradeCache.mIrregVolumeFieldState;
        }



        /// <summary>
        /// <see cref="MamdaTradeRecap.getIrregPartIdFieldState()" />
        /// </summary>
        public MamdaFieldState getIrregPartIdFieldState()
        {
            return mTradeCache.mIrregPartIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getIrregTimeFieldState()" />
        /// </summary>
        public MamdaFieldState getIrregTimeFieldState()
        {
            return mTradeCache.mIrregTimeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastTimeFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeDateFieldState()
        {
            return mTradeCache.mTradeDateFieldState;
        }
        /// <summary>
        /// <see cref="MamdaTradeRecap.getAccVolumeFieldState()" />
        /// </summary>
        public MamdaFieldState getAccVolumeFieldState()
        {
            return mTradeCache.mAccVolumeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOffExAccVolumeFieldState()" />
        /// </summary>
        public MamdaFieldState getOffExAccVolumeFieldState()
        {
            return mTradeCache.mOffExAccVolumeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOnExAccVolumeFieldState()" />
        /// </summary>
        public MamdaFieldState getOnExAccVolumeFieldState()
        {
            return mTradeCache.mOnExAccVolumeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getNetChangeFieldState()" />
        /// </summary>
        public MamdaFieldState getNetChangeFieldState()
        {
            return mTradeCache.mNetChangeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getPctChangeFieldState()" />
        /// </summary>
        public MamdaFieldState getPctChangeFieldState()
        {
            return mTradeCache.mPctChangeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTradeDirectionFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeDirectionFieldState()
        {
            return mTradeCache.mTradeDirectionFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getSideFieldState()" />
        /// </summary>
        public MamdaFieldState getSideFieldState()
        {
            return mTradeCache.mSideFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOpenPriceFieldState()" />
        /// </summary>
        public MamdaFieldState getOpenPriceFieldState()
        {
            return mTradeCache.mOpenPriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getHighPriceFieldState()" />
        /// </summary>
        public MamdaFieldState getHighPriceFieldState()
        {
            return mTradeCache.mHighPriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLowPriceFieldState()" />
        /// </summary>
        public MamdaFieldState getLowPriceFieldState()
        {
            return mTradeCache.mLowPriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getClosePriceFieldState()" />
        /// </summary>
        public MamdaFieldState getClosePriceFieldState()
        {
            return mTradeCache.mClosePriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getPrevClosePriceFieldState()" />
        /// </summary>
        public MamdaFieldState getPrevClosePriceFieldState()
        {
            return mTradeCache.mPrevClosePriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getPrevCloseDateFieldState()" />
        /// </summary>
        public MamdaFieldState getPrevCloseDateFieldState()
        {
            return mTradeCache.mPrevCloseDateFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getAdjPrevCloseFieldState()" />
        /// </summary>
        public MamdaFieldState getAdjPrevCloseFieldState()
        {
            return mTradeCache.mAdjPrevCloseFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getBlockCountFieldState()" />
        /// </summary>
        public MamdaFieldState getBlockCountFieldState()
        {
            return mTradeCache.mBlockCountFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getBlockVolumeFieldState()" />
        /// </summary>
        public MamdaFieldState getBlockVolumeFieldState()
        {
            return mTradeCache.mBlockVolumeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getVwapFieldState()" />
        /// </summary>
        public MamdaFieldState getVwapFieldState()
        {
            return mTradeCache.mVwapFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOffExVwapFieldState()" />
        /// </summary>
        public MamdaFieldState getOffExVwapFieldState()
        {
            return mTradeCache.mOffExVwapFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOnExVwapFieldState()" />
        /// </summary>
        public MamdaFieldState getOnExVwapFieldState()
        {
            return mTradeCache.mOnExVwapFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTotalValueFieldState()" />
        /// </summary>
        public MamdaFieldState getTotalValueFieldState()
        {
            return mTradeCache.mTotalValueFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOffExTotalValueFieldState()" />
        /// </summary>
        public MamdaFieldState getOffExTotalValueFieldState()
        {
            return mTradeCache.mOffExTotalValueFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOnExTotalValueFieldState()" />
        /// </summary>
        public MamdaFieldState getOnExTotalValueFieldState()
        {
            return mTradeCache.mOnExTotalValueFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getStdDevFieldState()" />
        /// </summary>
        public MamdaFieldState getStdDevFieldState()
        {
            return mTradeCache.mStdDevFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getStdDevSumFieldState()" />
        /// </summary>
        public MamdaFieldState getStdDevSumFieldState()
        {
            return mTradeCache.mStdDevSumFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getStdDevSumSquaresFieldState()" />
        /// </summary>
        public MamdaFieldState getStdDevSumSquaresFieldState()
        {
            return mTradeCache.mStdDevSumSquaresFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOrderId" />
        /// </summary>
        public MamdaFieldState getOrderIdFieldState()
        {
            return mTradeCache.mOrderIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getSettlePriceFieldState()" />
        /// </summary>
        public MamdaFieldState getSettlePriceFieldState()
        {
            return mTradeCache.mSettlePriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getSettleDateFieldState()" />
        /// </summary>
        public MamdaFieldState getSettleDateFieldState()
        {
            return mTradeCache.mSettleDateFieldState;
        }

        public MamdaFieldState getShortSaleCircuitBreakerFieldState()
        {
            return mTradeCache.mShortSaleCircuitBreakerFieldState;
        }

        public MamdaFieldState getOrigShortSaleCircuitBreakerFieldState()
        {
            return mTradeCache.mOrigShortSaleCircuitBreakerFieldState;
        }

        public MamdaFieldState getCorrShortSaleCircuitBreakerFieldState()
        {
            return mTradeCache.mCorrShortSaleCircuitBreakerFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradePriceFieldState()" />
        /// </summary>
        public MamdaFieldState getTradePriceFieldState()
        {
            return mTradeCache.mTradePriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeVolumeFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeVolumeFieldState()
        {
            return (MamdaFieldState)mTradeCache.mTradeVolumeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradePartIdFieldState()" />
        /// </summary>
        public MamdaFieldState getTradePartIdFieldState()
        {
            return mTradeCache.mTradePartIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeQualFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeQualFieldState()
        {
            return mTradeCache.mTradeQualStrFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeQualNativeStrFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeQualNativeStrFieldState()
        {
            return mTradeCache.mTradeQualNativeStrFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeConditionFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeConditionFieldState()
        {
            return mTradeCache.mTradeCondStrFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeSellersSaleDaysFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeSellersSaleDaysFieldState()
        {
            return mTradeCache.mSellersSaleDaysFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getTradeStopStockFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeStopStockFieldState()
        {
            return mTradeCache.mStopStockIndFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTradeExecVenueFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeExecVenueFieldState()
        {
            return mTradeCache.mTradeExecVenueFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOffExchangeTradePriceFieldState()" />
        /// </summary>
        public MamdaFieldState getOffExchangeTradePriceFieldState()
        {
            return mTradeCache.mOffExTradePriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getOnExchangeTradePriceFieldState()" />
        /// </summary>
        public MamdaFieldState getOnExchangeTradePriceFieldState()
        {
            return mTradeCache.mOnExTradePriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTradeCountFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeCountFieldState()
        {
            return mTradeCache.mTradeCountFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTradeUnitsFieldState()" />
        /// </summary>
        public MamdaFieldState getTradeUnitsFieldState()
        {
            return mTradeCache.mTradeUnitsFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLastSeqNumFieldState()" />
        /// </summary>
        public MamdaFieldState getLastSeqNumFieldState()
        {
            return mTradeCache.mLastSeqNumFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getHighSeqNumFieldState()" />
        /// </summary>
        public MamdaFieldState getHighSeqNumFieldState()
        {
            return mTradeCache.mHighSeqNumFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getLowSeqNum()" />
        /// </summary>
        public MamdaFieldState getLowSeqNumFieldState()
        {
            return mTradeCache.mLowSeqNumFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getTotalVolumeSeqNumFieldState()" />
        /// </summary>
        public MamdaFieldState getTotalVolumeSeqNumFieldState()
        {
            return mTradeCache.mTotalVolumeSeqNumFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeRecap.getCurrencyCodeFieldState()" />
        /// </summary>
        public MamdaFieldState getCurrencyCodeFieldState()
        {
            return mTradeCache.mCurrencyCodeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeReport.getIsIrregularFieldState()" />
        /// </summary>
        public MamdaFieldState getIsIrregularFieldState()
        {
            return mTradeCache.mIsIrregularFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeGap.getBeginGapSeqNumFieldState()" />
        /// </summary>
        public MamdaFieldState getBeginGapSeqNumFieldState()
        {
            return mGapBeginFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeGap.getEndGapSeqNumFieldState()" />
        /// </summary>
        public MamdaFieldState getEndGapSeqNumFieldState()
        {
            return mGapEndFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigSeqNumFieldState()" />
        /// </summary>
        public MamdaFieldState getOrigSeqNumFieldState()
        {
            return mTradeCache.mOrigSeqNumFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigPriceFieldState()" />
        /// </summary>
        public MamdaFieldState getOrigPriceFieldState()
        {
            return mTradeCache.mOrigPriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigVolumeFieldState()" />
        /// </summary>
        public MamdaFieldState getOrigVolumeFieldState()
        {
            return (MamdaFieldState)mTradeCache.mOrigVolumeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigPartIdFieldState()" />
        /// </summary>
        public MamdaFieldState getOrigPartIdFieldState()
        {
            return mTradeCache.mOrigPartIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigQualFieldState()" />
        /// </summary>
        public MamdaFieldState getOrigQualFieldState()
        {
            return mTradeCache.mOrigQualStrFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigQualNativeFieldState()" />
        /// </summary>
        public MamdaFieldState getOrigQualNativeFieldState()
        {
            return mTradeCache.mOrigQualNativeStrFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigConditionFieldState()" />
        /// </summary>
        public MamdaFieldState getOrigConditionFieldState()
        {
            return mTradeCache.mOrigCondStrFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigSellersSaleDaysFieldState()" />
        /// </summary>
        public MamdaFieldState getOrigSellersSaleDaysFieldState()
        {
            return mTradeCache.mOrigSellersSaleDaysFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCancelOrError.getOrigStopStockFieldState()" />
        /// </summary>
        public MamdaFieldState getOrigStopStockFieldState()
        {
            return mTradeCache.mOrigStopStockIndFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrPriceFieldState()" />
        /// </summary>
        public MamdaFieldState getCorrPriceFieldState()
        {
            return mTradeCache.mCorrPriceFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrVolumeFieldState()" />
        /// </summary>
        public MamdaFieldState getCorrVolumeFieldState()
        {
            return (MamdaFieldState)mTradeCache.mCorrVolumeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrPartIdFieldState()" />
        /// </summary>
        public MamdaFieldState getCorrPartIdFieldState()
        {
            return mTradeCache.mCorrPartIdFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrQualFieldState()" />
        /// </summary>
        public MamdaFieldState getCorrQualFieldState()
        {
            return mTradeCache.mCorrQualStrFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrQualNativeFieldState()" />
        /// </summary>
        public MamdaFieldState getCorrQualNativeFieldState()
        {
            return mTradeCache.mCorrQualNativeStrFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrConditionFieldState()" />
        /// </summary>
        public MamdaFieldState getCorrConditionFieldState()
        {
            return mTradeCache.mCorrCondStrFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrSellersSaleDaysFieldState()" />
        /// </summary>
        public MamdaFieldState getCorrSellersSaleDaysFieldState()
        {
            return mTradeCache.mCorrSellersSaleDaysFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeCorrection.getCorrStopStockFieldState()" />
        /// </summary>
        public MamdaFieldState getCorrStopStockFieldState()
        {
            return mTradeCache.mCorrStopStockIndFieldState;
        }

        /// <summary>
        /// <see cref="MamdaTradeClosing.getIsIndicativeFieldState()" />
        /// </summary>
        public MamdaFieldState getIsIndicativeFieldState()
        {
            return mIsIndicativeFieldState;
        }

        /// <summary>
        /// </summary>
        /// <param name="recap"></param>
        public void populateRecap(MamdaConcreteTradeRecap recap)
        {
            lock (this)
            {
                recap.setSymbol(mTradeCache.mSymbol);
                recap.setPartId(mTradeCache.mPartId);
                recap.setSrcTime(mTradeCache.mSrcTime);
                recap.setActTime(mTradeCache.mActTime);
                recap.setLineTime(mTradeCache.mLineTime);
                recap.setSendTime(mTradeCache.mSendTime);
                recap.setPubId(mTradeCache.mPubId);
                recap.setLastPrice(mTradeCache.mLastPrice);
                recap.setLastVolume(mTradeCache.mLastVolume);
                recap.setLastPartId(mTradeCache.mLastPartId);
                recap.setTradeId(mTradeCache.mTradeId);
                recap.setOrigTradeId(mTradeCache.mOrigTradeId);
                recap.setCorrTradeId(mTradeCache.mCorrTradeId);
                recap.setLastTime(mTradeCache.mLastTime);
                recap.setTradeDate(mTradeCache.mTradeDate);
                recap.setIrregPrice(mTradeCache.mIrregPrice);
                recap.setIrregVolume(mTradeCache.mIrregVolume);
                recap.setIrregPartId(mTradeCache.mIrregPartId);
                recap.setIrregTime(mTradeCache.mIrregTime);
                recap.setAccVolume(mTradeCache.mAccVolume);
                recap.setOffExAccVolume(mTradeCache.mOffExAccVolume);
                recap.setOnExAccVolume(mTradeCache.mOnExAccVolume);
                recap.setNetChange(mTradeCache.mNetChange);
                recap.setPctChange(mTradeCache.mPctChange);
                recap.setTradeDirection(mTradeCache.mTradeDirection);
                recap.setSide(mTradeCache.mSide);
                recap.setOpenPrice(mTradeCache.mOpenPrice);
                recap.setHighPrice(mTradeCache.mHighPrice);
                recap.setLowPrice(mTradeCache.mLowPrice);
                recap.setClosePrice(mTradeCache.mClosePrice);
                recap.setPrevClosePrice(mTradeCache.mPrevClosePrice);
                recap.setPrevCloseDate(mTradeCache.mPrevCloseDate);
                recap.setAdjPrevClose(mTradeCache.mAdjPrevClose);
                recap.setBlockCount(mTradeCache.mBlockCount);
                recap.setBlockVolume(mTradeCache.mBlockVolume);
                recap.setVwap(mTradeCache.mVwap);
                recap.setOffExVwap(mTradeCache.mOffExVwap);
                recap.setOnExVwap(mTradeCache.mOnExVwap);
                recap.setTotalValue(mTradeCache.mTotalValue);
                recap.setOffExTotalValue(mTradeCache.mOffExTotalValue);
                recap.setOnExTotalValue(mTradeCache.mOnExTotalValue);
                recap.setStdDev(mTradeCache.mStdDev);
                recap.setStdDevSum(mTradeCache.mStdDevSum);
                recap.setStdDevSumSquares(mTradeCache.mStdDevSumSquares);
                recap.setOrderId(mTradeCache.mOrderId);
                recap.setSettlePrice(mTradeCache.mSettlePrice);
                recap.setSettleDate(mTradeCache.mSettleDate);

                recap.setEventSeqNum(mTradeCache.mEventSeqNum);
                recap.setEventTime(mTradeCache.mEventTime);

                recap.setTradePrice(mTradeCache.mTradePrice);
                recap.setTradeVolume(mTradeCache.mTradeVolume);
                recap.setTradePartId(mTradeCache.mTradePartId);
                recap.setTradeQualStr(mTradeCache.mTradeQualStr);
                recap.setTradeQualNativeStr(mTradeCache.mTradeQualNativeStr);
                recap.setTradeCondStr(mTradeCache.mTradeCondStr);
                recap.setSellersSaleDays(mTradeCache.mSellersSaleDays);
                recap.setStopStockInd(mTradeCache.mStopStockInd);

                recap.setTradeExecVenue(mTradeCache.mTradeExecVenue);
                recap.setOffExchangeTradePrice(mTradeCache.mOffExTradePrice);
                recap.setOnExchangeTradePrice(mTradeCache.mOnExTradePrice);

                recap.setTradeUnits(mTradeCache.mTradeUnits);
                recap.setLastSeqNum(mTradeCache.mLastSeqNum);
                recap.setHighSeqNum(mTradeCache.mHighSeqNum);
                recap.setLowSeqNum(mTradeCache.mLowSeqNum);
                recap.setTotalVolumeSeqNum(mTradeCache.mTotalVolumeSeqNum);
                recap.setCurrencyCode(mTradeCache.mCurrencyCode);

                recap.setTradeCount(mTradeCache.mTradeCount);

                recap.setOrigSeqNum(mTradeCache.mOrigSeqNum);
                recap.setOrigPrice(mTradeCache.mOrigPrice);
                recap.setOrigVolume(mTradeCache.mOrigVolume);
                recap.setOrigPartId(mTradeCache.mOrigPartId);
                recap.setOrigQualStr(mTradeCache.mOrigQualStr);
                recap.setOrigQualNativeStr(mTradeCache.mOrigQualNativeStr);
                recap.setOrigCondStr(mTradeCache.mOrigCondStr);
                recap.setOrigSellersSaleDays(mTradeCache.mOrigSellersSaleDays);
                recap.setOrigStopStockInd(mTradeCache.mOrigStopStockInd);

                recap.setCorrPrice(mTradeCache.mCorrPrice);
                recap.setCorrVolume(mTradeCache.mCorrVolume);
                recap.setCorrPartId(mTradeCache.mCorrPartId);
                recap.setCorrQualStr(mTradeCache.mCorrQualStr);
                recap.setCorrQualNativeStr(mTradeCache.mCorrQualNativeStr);
                recap.setCorrCondStr(mTradeCache.mCorrCondStr);
                recap.setCorrSellersSaleDays(mTradeCache.mCorrSellersSaleDays);
                recap.setCorrStopStockInd(mTradeCache.mCorrStopStockInd);
                recap.setCancelTime(mTradeCache.mCancelTime);
                recap.setIsIrregular(mTradeCache.mIsIrregular);
                recap.setIsCancel(mTradeCache.mIsCancel);
            }
        }

        /// <summary>
        /// Implementation of MamdaListener interface.
        /// </summary>
        /// <param name="subscription"></param>
        /// <param name="msg"></param>
        /// <param name="msgType"></param>
        public void onMsg(
            MamdaSubscription subscription,
            MamaMsg msg,
            mamaMsgType msgType)
        {
            // Ensure only a single instance is created.
            // Listeners may be created on separate threads.
            if (mUpdaters == null)
            {
                lock (mUpdatersGuard)
                {
                    if (!MamdaTradeFields.isSet())
                    {
                        return;
                    }

                    if (mUpdaters == null)
                    {
                        TradeUpdate[] updaters = createUpdaters();
                        Thread.MemoryBarrier();
                        mUpdaters = updaters;
                    }
                }
            }

            // If msg is a trade-related message, invoke the
            // appropriate callback:
            mTradeCache.mWasIrregular = mTradeCache.mIsIrregular;
            mTradeCache.mConflateCount = 1;

            switch (msgType)
            {
                case mamaMsgType.MAMA_MSG_TYPE_SNAPSHOT:
                case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
                case mamaMsgType.MAMA_MSG_TYPE_RECAP:
                case mamaMsgType.MAMA_MSG_TYPE_PREOPENING:
                    handleRecap(subscription, msg);
                    break;
                case mamaMsgType.MAMA_MSG_TYPE_TRADE:
                    handleTrade(subscription, msg);
                    break;
                case mamaMsgType.MAMA_MSG_TYPE_CANCEL:
                    handleCancelOrError(subscription, msg, true);
                    break;
                case mamaMsgType.MAMA_MSG_TYPE_ERROR:
                    handleCancelOrError(subscription, msg, false);
                    break;
                case mamaMsgType.MAMA_MSG_TYPE_CORRECTION:
                    handleCorrection(subscription, msg);
                    break;
                case mamaMsgType.MAMA_MSG_TYPE_CLOSING:
                    handleClosing(subscription, msg);
                    break;
                case mamaMsgType.MAMA_MSG_TYPE_UPDATE:
                    if (mProcessUpdateAsTrade)
                    {
                        handleUpdate(subscription, msg);
                    }
                    break;
            }
        }

        #region Implementation details

        #region Implementation helpers

        private void handleRecap(
            MamdaSubscription subscription,
            MamaMsg msg)
        {
            updateFieldStates();
            updateTradeFields(msg);

            mTradeCache.mIsIrregular = mTradeCache.mWasIrregular;

            checkTradeCount(subscription, msg, false);

            foreach (MamdaTradeHandler handler in mHandlers)
            {
                handler.onTradeRecap(subscription, this, msg, this);
            }
        }

        private void handleTrade(
            MamdaSubscription subscription,
            MamaMsg msg)
        {
            updateFieldStates();
            updateTradeFields(msg);

            checkTradeCount(subscription, msg, true);

            if (mIgnoreUpdate)
            {
                mIgnoreUpdate = false;
                return;
            }

            foreach (MamdaTradeHandler handler in mHandlers)
            {
                handler.onTradeReport(subscription, this, msg, this, this);
            }
        }

        private void handleCancelOrError(
            MamdaSubscription subscription,
            MamaMsg msg,
            bool isCancel)
        {
            updateFieldStates();
            updateTradeFields(msg);
            mTradeCache.mIsCancel = isCancel;

            checkTradeCount(subscription, msg, true);

            if (mIgnoreUpdate)
            {
                mIgnoreUpdate = false;
                return;
            }

            if (mTradeCache.mCancelTime != DateTime.MinValue)
            {
                mTradeCache.mEventTime = mTradeCache.mCancelTime;
                mTradeCache.mEventTimeFieldState = MamdaFieldState.MODIFIED;
            }

            foreach (MamdaTradeHandler handler in mHandlers)
            {
                handler.onTradeCancelOrError(subscription, this, msg, this, this);
            }
        }

        private void handleCorrection(MamdaSubscription subscription,
            MamaMsg msg)
        {
            updateFieldStates();
            updateTradeFields(msg);
            checkTradeCount(subscription, msg, true);

            if (mIgnoreUpdate)
            {
                mIgnoreUpdate = false;
                return;
            }

            if (mTradeCache.mCorrTime != DateTime.MinValue)
            {
                mTradeCache.mEventTime = mTradeCache.mCorrTime;
                mTradeCache.mEventTimeFieldState = MamdaFieldState.MODIFIED;
            }

            foreach (MamdaTradeHandler handler in mHandlers)
            {
                handler.onTradeCorrection(subscription, this, msg, this, this);
            }
        }

        private void handleUpdate(MamdaSubscription subscription,
            MamaMsg msg)
        {
            updateFieldStates();
            updateTradeFields(msg);
            checkTradeCount(subscription, msg, true);

            if (mIgnoreUpdate)
            {
                mIgnoreUpdate = false;
                return;
            }

            if (mTradeCache.mLastGenericMsgWasTrade)
            {
                foreach (MamdaTradeHandler handler in mHandlers)
                {
                    handler.onTradeReport(subscription, this, msg, this, this);
                }
            }
        }

        private void handleClosing(MamdaSubscription subscription,
            MamaMsg msg)
        {
            updateFieldStates();
            updateTradeFields(msg);
            checkTradeCount(subscription, msg, true);

            if (mIgnoreUpdate)
            {
                mIgnoreUpdate = false;
                return;
            }

            foreach (MamdaTradeHandler handler in mHandlers)
            {
                handler.onTradeClosing(subscription, this, msg, this, this);
            }
        }

        private void updateFieldStates()
        {
            if (mTradeCache.mSymbolFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mSymbolFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mIssueSymbolFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mIssueSymbolFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mPartIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mPartIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mSrcTimeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mSrcTimeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mActTimeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mActTimeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mLineTimeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mLineTimeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mSendTimeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mSendTimeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mPubIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mPubIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mIsIrregularFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mIsIrregularFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mLastPriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mLastPriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mLastVolumeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mLastVolumeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mLastPartIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mLastPartIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradeIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradeIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigTradeIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigTradeIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCorrTradeIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCorrTradeIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mLastTimeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mLastTimeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradeDateFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradeDateFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mIrregPriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mIrregPriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mIrregVolumeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mIrregVolumeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mIrregPartIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mIrregPartIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mIrregTimeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mIrregTimeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mAccVolumeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mAccVolumeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOffExAccVolumeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOffExAccVolumeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOnExAccVolumeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOnExAccVolumeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradeDirectionFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradeDirectionFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mSideFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mSideFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mNetChangeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mNetChangeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mPctChangeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mPctChangeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOpenPriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOpenPriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mHighPriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mHighPriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mLowPriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mLowPriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mClosePriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mClosePriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mPrevClosePriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mPrevClosePriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mPrevCloseDateFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mPrevCloseDateFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mAdjPrevCloseFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mAdjPrevCloseFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradeCountFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradeCountFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mBlockVolumeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mBlockVolumeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mBlockCountFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mBlockCountFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mVwapFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mVwapFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOffExVwapFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOffExVwapFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOnExVwapFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOnExVwapFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTotalValueFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTotalValueFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOffExTotalValueFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOffExTotalValueFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOnExTotalValueFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOnExTotalValueFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mStdDevFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mStdDevFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mStdDevSumFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mStdDevSumFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mStdDevSumSquaresFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mStdDevSumSquaresFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrderIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrderIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mSettlePriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mSettlePriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mSettleDateFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mSettleDateFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mShortSaleCircuitBreakerFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mShortSaleCircuitBreakerFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigShortSaleCircuitBreakerFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigShortSaleCircuitBreakerFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCorrShortSaleCircuitBreakerFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCorrShortSaleCircuitBreakerFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mEventSeqNumFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mEventSeqNumFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mEventTimeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mEventTimeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradePriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradePriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradeVolumeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradeVolumeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradePartIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradePartIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradeQualStrFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradeQualStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradeQualNativeStrFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradeQualNativeStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mSellersSaleDaysFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mSellersSaleDaysFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mStopStockIndFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mStopStockIndFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradeExecVenueFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradeExecVenueFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOffExTradePriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOffExTradePriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOnExTradePriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOnExTradePriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigSeqNumFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigSeqNumFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigPriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigPriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigVolumeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigVolumeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigPartIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigPartIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigQualStrFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigQualStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigQualNativeStrFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigQualNativeStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigSellersSaleDaysFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigSellersSaleDaysFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigStopStockIndFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigStopStockIndFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCorrPriceFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCorrPriceFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCorrVolumeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCorrVolumeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCorrPartIdFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCorrPartIdFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCorrQualStrFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCorrQualStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCorrQualNativeStrFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCorrQualNativeStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCorrSellersSaleDaysFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCorrSellersSaleDaysFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCorrStopStockIndFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCorrStopStockIndFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCancelTimeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCancelTimeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradeCondStrFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradeCondStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mOrigCondStrFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mOrigCondStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCorrCondStrFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCorrCondStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTradeUnitsFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTradeUnitsFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mLastSeqNumFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mLastSeqNumFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mHighSeqNumFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mHighSeqNumFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mLowSeqNumFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mLowSeqNumFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mTotalVolumeSeqNumFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mTotalVolumeSeqNumFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mCurrencyCodeFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mCurrencyCodeFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mTradeCache.mConflateCountFieldState == MamdaFieldState.MODIFIED)
                mTradeCache.mConflateCountFieldState = MamdaFieldState.NOT_MODIFIED;
        }

        private void updateTradeFields(MamaMsg msg)
        {
            mTradeCache.mGotPartId = false;
            mTradeCache.mLastGenericMsgWasTrade = false;
            mTradeCache.mGotTradeTime = false;
            mTradeCache.mGotTradePrice = false;
            mTradeCache.mGotTradeSize = false;
            mTradeCache.mGotTradeCount = false;
            mTradeCache.mGotIssueSymbol = false;

            lock (this)
            {
                msg.iterateFields(mFieldIterator, null, null);
            }

            if (mTradeCache.mGotIssueSymbol)
            {
                mTradeCache.mSymbol = mTradeCache.mIssueSymbol;
                mTradeCache.mSymbolFieldState = MamdaFieldState.MODIFIED;
            }

            if (mTradeCache.mIsIrregular)
            {
                mTradeCache.mTradePrice = mTradeCache.mIrregPrice;
                mTradeCache.mTradePriceFieldState = MamdaFieldState.MODIFIED;
                mTradeCache.mTradeVolume = mTradeCache.mIrregVolume;
                mTradeCache.mTradeVolumeFieldState = MamdaFieldState.MODIFIED;
                mTradeCache.mTradePartId = mTradeCache.mIrregPartId;
                mTradeCache.mTradePartIdFieldState = MamdaFieldState.MODIFIED;
                if (mTradeCache.mIrregTime == DateTime.MinValue)
                {
                    mTradeCache.mEventTime = mTradeCache.mLastTime;
                    mTradeCache.mEventTimeFieldState = MamdaFieldState.MODIFIED;
                }
                else
                {
                    mTradeCache.mEventTime = mTradeCache.mIrregTime;
                    mTradeCache.mEventTimeFieldState = MamdaFieldState.MODIFIED;
                }
            }
            else
            {
                mTradeCache.mTradePrice = mTradeCache.mLastPrice;
                mTradeCache.mTradePriceFieldState = MamdaFieldState.MODIFIED;
                mTradeCache.mTradeVolume = mTradeCache.mLastVolume;
                mTradeCache.mTradeVolumeFieldState = MamdaFieldState.MODIFIED;
                mTradeCache.mTradePartId = mTradeCache.mLastPartId;
                mTradeCache.mTradePartIdFieldState = MamdaFieldState.MODIFIED;
                mTradeCache.mEventTime = mTradeCache.mLastTime;
                mTradeCache.mEventTimeFieldState = MamdaFieldState.MODIFIED;
            }

            if (mTradeCache.mGotPartId == false)
            {
                // No explicit part ID in message, but maybe in symbol.
                if (mTradeCache.mSymbol != null)
                {
                    int lastDot = mTradeCache.mSymbol.IndexOf(".");
                    if (lastDot != -1)
                    {
                        lastDot++;
                        if (lastDot != mTradeCache.mSymbol.Length)
                        {
                            mTradeCache.mPartId =
                                mTradeCache.mSymbol.Substring(lastDot);
                            mTradeCache.mPartIdFieldState = MamdaFieldState.MODIFIED;
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

        private void checkTradeCount(
            MamdaSubscription subscription,
            MamaMsg msg,
            bool checkForGap)
        {
            mIgnoreUpdate = false;
            // Check number of trades for gaps
            long tradeCount = mTmpTradeCount.GetValueOrDefault(0);
            if (mTradeCache.mGotTradeCount)
            {
                if (checkForGap && (tradeCount > 0))
                {
                    if ((mTradeCache.mTradeCount > 0) && (tradeCount > (mTradeCache.mTradeCount + mTradeCache.mConflateCount)))
                    {
                        mGapBegin = mTradeCache.mTradeCount + mTradeCache.mConflateCount;
                        mGapBeginFieldState = MamdaFieldState.MODIFIED;
                        mGapEnd = tradeCount - 1;
                        mGapEndFieldState = MamdaFieldState.MODIFIED;
                        mTradeCache.mTradeCount = tradeCount;
                        mTradeCache.mTradeCountFieldState = MamdaFieldState.MODIFIED;
                        foreach (MamdaTradeHandler handler in mHandlers)
                        {
                            handler.onTradeGap(subscription, this, msg, this, this);
                        }
                    }
                }
            }

            // Check for duplicates.  Only check if tradeCount > 0, in case it isn't being sent down.
            if (mTradeCache.mGotTradeCount &&
                 tradeCount > 0 &&
                 (tradeCount == mTradeCache.mTradeCount))
            {
                mIgnoreUpdate = true;
            }

            mTradeCache.mTradeCount = tradeCount;
            mTradeCache.mTradeCountFieldState = MamdaFieldState.MODIFIED;
        }

        #region Updaters

        internal interface TradeUpdate
        {
            void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field);
        }

        private class MamdaTradeSymbol : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mSymbol = field.getString();
                listener.mTradeCache.mSymbolFieldState = MamdaFieldState.MODIFIED;
            }
        }
        private class MamdaTradeIssueSymbol : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mIssueSymbol = field.getString();
                listener.mTradeCache.mIssueSymbolFieldState = MamdaFieldState.MODIFIED;
                listener.mTradeCache.mGotIssueSymbol = true;
            }
        }
        private class TradeLastPartId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mLastPartId = field.getString();
                listener.mTradeCache.mLastPartIdFieldState = MamdaFieldState.MODIFIED;
                listener.mTradeCache.mGotPartId = true;
                listener.mTradeCache.mIsIrregular = false;
            }
        }

        private class TradeId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                switch (field.getType())
                {
                    case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                        listener.mTradeCache.mTradeId = field.getString();
                        listener.mTradeCache.mTradeIdFieldState = MamdaFieldState.MODIFIED;
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_I8:
                    case mamaFieldType.MAMA_FIELD_TYPE_U8:
                    case mamaFieldType.MAMA_FIELD_TYPE_I16:
                    case mamaFieldType.MAMA_FIELD_TYPE_U16:
                    case mamaFieldType.MAMA_FIELD_TYPE_I32:
                    case mamaFieldType.MAMA_FIELD_TYPE_U32:
                    case mamaFieldType.MAMA_FIELD_TYPE_U64:
                        listener.mTradeCache.mTradeId = field.getU64().ToString();
                        listener.mTradeCache.mTradeIdFieldState = MamdaFieldState.MODIFIED;
                        break;
                    default:
                        break;
                }
            }
        }

        private class OrigTradeId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                switch (field.getType())
                {
                    case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                        listener.mTradeCache.mOrigTradeId = field.getString();
                        listener.mTradeCache.mOrigTradeIdFieldState = MamdaFieldState.MODIFIED;
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_I8:
                    case mamaFieldType.MAMA_FIELD_TYPE_U8:
                    case mamaFieldType.MAMA_FIELD_TYPE_I16:
                    case mamaFieldType.MAMA_FIELD_TYPE_U16:
                    case mamaFieldType.MAMA_FIELD_TYPE_I32:
                    case mamaFieldType.MAMA_FIELD_TYPE_U32:
                    case mamaFieldType.MAMA_FIELD_TYPE_U64:
                        listener.mTradeCache.mOrigTradeId = field.getU64().ToString();
                        listener.mTradeCache.mOrigTradeIdFieldState = MamdaFieldState.MODIFIED;
                        break;
                    default:
                        break;
                }
            }
        }

        private class CorrTradeId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCorrTradeId = field.getString();
                listener.mTradeCache.mCorrTradeIdFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeUpdateAsTrade : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                mProcessUpdateAsTrade = field.getBool();
            }
        }
        private class TradeSrcTime : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mSrcTime = field.getDateTime();
                listener.mTradeCache.mSrcTimeFieldState = MamdaFieldState.MODIFIED;
            }
        }
        private class TradeActivityTime : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mActTime = field.getDateTime();
                listener.mTradeCache.mActTimeFieldState = MamdaFieldState.MODIFIED;
            }
        }
        public class TradeLineTime : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mLineTime = field.getDateTime();
                listener.mTradeCache.mLineTimeFieldState = MamdaFieldState.MODIFIED;
            }
        }
        public class TradeSendTime : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mSendTime = field.getDateTime();
                listener.mTradeCache.mSendTimeFieldState = MamdaFieldState.MODIFIED;
            }
        }
        private class TradePubId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mPubId = field.getString();
                listener.mTradeCache.mPubIdFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradePrevPartId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mLastPartId = field.getString();
                listener.mTradeCache.mLastPartIdFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeAccVolume : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mAccVolume = field.getF64();
                listener.mTradeCache.mAccVolumeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOffExAccVolume : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOffExAccVolume = field.getF64();
                listener.mTradeCache.mOffExAccVolumeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOnExAccVolume : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOnExAccVolume = field.getF64();
                listener.mTradeCache.mOnExAccVolumeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeNetChange : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mNetChange.copy(field.getPrice());
                listener.mTradeCache.mNetChangeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradePctChange : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mPctChange = field.getF64();
                listener.mTradeCache.mPctChangeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeDirection : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                switch (field.getType())
                {
                    case mamaFieldType.MAMA_FIELD_TYPE_I8:
                    case mamaFieldType.MAMA_FIELD_TYPE_U8:
                    case mamaFieldType.MAMA_FIELD_TYPE_I16:
                    case mamaFieldType.MAMA_FIELD_TYPE_U16:
                    case mamaFieldType.MAMA_FIELD_TYPE_I32:
                    case mamaFieldType.MAMA_FIELD_TYPE_U32:
                        listener.mTradeCache.mTradeDirection = field.getU32().ToString();
                        listener.mTradeCache.mTradeDirectionFieldState = MamdaFieldState.MODIFIED;
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                        listener.mTradeCache.mTradeDirection = field.getString();
                        listener.mTradeCache.mTradeDirectionFieldState = MamdaFieldState.MODIFIED;
                        break;
                    default:
                        break;
                }
            }
        }

        private class AggressorSide : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mSide = field.getChar().ToString();
                listener.mTradeCache.mSideFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeSide : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                switch (field.getType())
                {
                    case mamaFieldType.MAMA_FIELD_TYPE_I8:
                    case mamaFieldType.MAMA_FIELD_TYPE_U8:
                    case mamaFieldType.MAMA_FIELD_TYPE_I16:
                    case mamaFieldType.MAMA_FIELD_TYPE_U16:
                    case mamaFieldType.MAMA_FIELD_TYPE_I32:
                    case mamaFieldType.MAMA_FIELD_TYPE_U32:
                        listener.mTradeCache.mTmpSide.setState((short)field.getU32());
                        listener.mTradeCache.mSide = MamdaTradeSide.toString(listener.mTradeCache.mTmpSide.getState());
                        listener.mTradeCache.mSideFieldState = MamdaFieldState.MODIFIED;
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                        listener.mTradeCache.mSide = field.getString();
                        listener.mTradeCache.mSideFieldState = MamdaFieldState.MODIFIED;
                        break;
                    default:
                        break;
                }
            }
        }

        private class TradeOpenPrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOpenPrice.copy(field.getPrice());
                listener.mTradeCache.mOpenPriceFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeHighPrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mHighPrice.copy(field.getPrice());
                listener.mTradeCache.mHighPriceFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeLowPrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mLowPrice.copy(field.getPrice());
                listener.mTradeCache.mLowPriceFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeClosePrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mClosePrice.copy(field.getPrice());
                listener.mTradeCache.mClosePriceFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradePrevClosePrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mPrevClosePrice.copy(field.getPrice());
                listener.mTradeCache.mPrevClosePriceFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradePrevCloseDate : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mPrevCloseDate = field.getDateTime();
                listener.mTradeCache.mPrevCloseDateFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeAdjPrevClose : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mAdjPrevClose.copy(field.getPrice());
                listener.mTradeCache.mAdjPrevCloseFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeBlockCount : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mBlockCount = field.getI64();
                listener.mTradeCache.mBlockCountFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeBlockVolume : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mBlockVolume = field.getF64();
                listener.mTradeCache.mBlockVolumeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeVWap : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mVwap = field.getF64();
                listener.mTradeCache.mVwapFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOffExVWap : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOffExVwap = field.getF64();
                listener.mTradeCache.mOffExVwapFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOnExVWap : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOnExVwap = field.getF64();
                listener.mTradeCache.mOnExVwapFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeTotalValue : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mTotalValue = field.getF64();
                listener.mTradeCache.mTotalValueFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOffExTotalValue : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOffExTotalValue = field.getF64();
                listener.mTradeCache.mOffExTotalValueFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOnExTotalValue : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOnExTotalValue = field.getF64();
                listener.mTradeCache.mOnExTotalValueFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeStdDev : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mStdDev = field.getF64();
                listener.mTradeCache.mStdDevFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeStdDevSum : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mStdDevSum = field.getF64();
                listener.mTradeCache.mStdDevSumFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeStdDevSumSquares : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mStdDevSumSquares = field.getF64();
                listener.mTradeCache.mStdDevSumSquaresFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOrderId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOrderId = field.getI64();
                listener.mTradeCache.mOrderIdFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeSettlePrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mSettlePrice.copy(field.getPrice());
                listener.mTradeCache.mSettlePriceFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeSettleDate : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mSettleDate = field.getDateTime();
                listener.mTradeCache.mSettleDateFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class MamdaShortSaleCircuitBreaker : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mShortSaleCircuitBreaker = field.getChar();
                listener.mTradeCache.mShortSaleCircuitBreakerFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class MamdaOrigShortSaleCircuitBreaker : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOrigShortSaleCircuitBreaker = field.getChar();
                listener.mTradeCache.mOrigShortSaleCircuitBreakerFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class MamdaCorrShortSaleCircuitBreaker : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCorrShortSaleCircuitBreaker = field.getChar();
                listener.mTradeCache.mCorrShortSaleCircuitBreakerFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeEventSeqNum : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mEventSeqNum = field.getI64();
                listener.mTradeCache.mEventSeqNumFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeLastTime : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mLastTime = field.getDateTime();
                listener.mTradeCache.mLastTimeFieldState = MamdaFieldState.MODIFIED;
                listener.mTradeCache.mGotTradeTime = true;
            }
        }

        private class TradeTradeDate : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mTradeDate = field.getDateTime();
                listener.mTradeCache.mTradeDateFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeIrregPrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mIrregPrice.copy(field.getPrice());
                listener.mTradeCache.mIrregPriceFieldState = MamdaFieldState.MODIFIED;
                if (listener.mTradeCache.mIrregPrice.getValue() != 0.0 && listener.mTradeCache.mIsIrregular == false)
                {
                    listener.mTradeCache.mIsIrregular = true;
                    listener.mTradeCache.mIsIrregularFieldState = MamdaFieldState.MODIFIED;
                }
                listener.mTradeCache.mGotTradePrice = true;
            }
        }

        private class TradeIrregVolume : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mIrregVolume = field.getI64();
                listener.mTradeCache.mIrregVolumeFieldState = MamdaFieldState.MODIFIED;
                if (listener.mTradeCache.mIrregVolume != 0 && listener.mTradeCache.mIsIrregular == false)
                {
                    listener.mTradeCache.mIsIrregular = true;
                    listener.mTradeCache.mIsIrregularFieldState = MamdaFieldState.MODIFIED;
                }
                listener.mTradeCache.mGotTradeSize = true;
            }
        }

        private class TradeIrregPartId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mIrregPartId = field.getString();
                listener.mTradeCache.mIrregPartIdFieldState = MamdaFieldState.MODIFIED;
                if (listener.mTradeCache.mIrregPartId != "" && listener.mTradeCache.mIsIrregular == false)
                {
                    listener.mTradeCache.mIsIrregular = true;
                    listener.mTradeCache.mIsIrregularFieldState = MamdaFieldState.MODIFIED;
                }
            }
        }

        private class TradeIrregTime : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mIrregTime = field.getDateTime();
                listener.mTradeCache.mIrregTimeFieldState = MamdaFieldState.MODIFIED;;
                listener.mTradeCache.mGotTradeTime = true;
            }
        }
        private class TradeLastPrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mLastPrice.copy(field.getPrice());
                listener.mTradeCache.mLastPriceFieldState = MamdaFieldState.MODIFIED;
                listener.mTradeCache.mIsIrregular = false;
                listener.mTradeCache.mGotTradePrice = true;
            }
        }
        private class TradeLastVolume : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mLastVolume = field.getF64();
                listener.mTradeCache.mLastVolumeFieldState = MamdaFieldState.MODIFIED;
                listener.mTradeCache.mIsIrregular = false;
                listener.mTradeCache.mGotTradeSize = true;
            }
        }
        private class TradePartId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mTradePartId = field.getString();
                listener.mTradeCache.mTradePartIdFieldState = MamdaFieldState.MODIFIED;
            }
        }
        private class TradeQualStr : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mTradeQualStr = field.getString();
                listener.mTradeCache.mTradeQualStrFieldState = MamdaFieldState.MODIFIED;
            }
        }
        private class TradeQualNativeStr : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mTradeQualNativeStr = field.getString();
                listener.mTradeCache.mTradeQualNativeStrFieldState = MamdaFieldState.MODIFIED;
            }
        }
        private class TradeCondStr : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mTradeCondStr = field.getString();
                listener.mTradeCache.mTradeCondStrFieldState = MamdaFieldState.MODIFIED;
            }
        }
        private class TradeSellerSalesDays : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mSellersSaleDays = field.getI64();
                listener.mTradeCache.mSellersSaleDaysFieldState = MamdaFieldState.MODIFIED;
            }
        }
        private class TradeStopStockInd : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                switch (field.getType())
                {
                    case mamaFieldType.MAMA_FIELD_TYPE_CHAR:
                        listener.mTradeCache.mStopStockInd = field.getChar();
                        listener.mTradeCache.mStopStockIndFieldState = MamdaFieldState.MODIFIED;
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                        if (field.getString() != String.Empty)
                        {
                            listener.mTradeCache.mStopStockInd = field.getString()[0];
                            listener.mTradeCache.mStopStockIndFieldState = MamdaFieldState.MODIFIED;
                        }
                        else
                        {
                            listener.mTradeCache.mStopStockInd = ' ';
                            listener.mTradeCache.mStopStockIndFieldState = MamdaFieldState.MODIFIED;
                        }
                        break;
                    default: break;
                }
            }
        }
        private class TradeExecVenue : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                switch (field.getType())
                {
                    case mamaFieldType.MAMA_FIELD_TYPE_I8:
                    case mamaFieldType.MAMA_FIELD_TYPE_U8:
                    case mamaFieldType.MAMA_FIELD_TYPE_I16:
                    case mamaFieldType.MAMA_FIELD_TYPE_U16:
                    case mamaFieldType.MAMA_FIELD_TYPE_I32:
                    case mamaFieldType.MAMA_FIELD_TYPE_U32:
                        listener.mTradeCache.mTradeExecVenue = field.getU32().ToString();
                        listener.mTradeCache.mTradeExecVenueFieldState = MamdaFieldState.MODIFIED;
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                        listener.mTradeCache.mTradeExecVenue = field.getString();
                        listener.mTradeCache.mTradeExecVenueFieldState = MamdaFieldState.MODIFIED;
                        break;
                    default:
                        break;
                }
            }
        }

        private class OffExTradePrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOffExTradePrice.copy(field.getPrice());
                listener.mTradeCache.mOffExTradePriceFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class OnExTradePrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOnExTradePrice.copy(field.getPrice());
                listener.mTradeCache.mOnExTradePriceFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeCount : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                long tempTradeCount = field.getI64();
                listener.mTmpTradeCount.Value = tempTradeCount;
                listener.mTradeCache.mGotTradeCount = true;
            }
        }

        private class TradeConflateCount : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mConflateCount = field.getU32();
                listener.mTradeCache.mConflateCountFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeUnits : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mTradeUnits = field.getString();
                listener.mTradeCache.mTradeUnitsFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeLastSeqNum : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mLastSeqNum = field.getI64();
                listener.mTradeCache.mLastSeqNumFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeHighSeqNum : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mHighSeqNum = field.getI64();
                listener.mTradeCache.mHighSeqNumFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeLowSeqNum : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mLowSeqNum = field.getI64();
                listener.mTradeCache.mLowSeqNumFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeTotalVolumeSeqNum : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mTotalVolumeSeqNum = field.getI64();
                listener.mTradeCache.mTotalVolumeSeqNumFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeCurrencyCode : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCurrencyCode = field.getString();
                listener.mTradeCache.mCurrencyCodeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOrigSeqNum : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOrigSeqNum = field.getI64();
                listener.mTradeCache.mOrigSeqNumFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOrigPrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOrigPrice.copy(field.getPrice());
                listener.mTradeCache.mOrigPriceFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOrigVolume : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOrigVolume = field.getF64();
                listener.mTradeCache.mOrigVolumeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOrigPartId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOrigPartId = field.getString();
                listener.mTradeCache.mOrigPartIdFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOrigQualStr : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOrigQualStr = field.getString();
                listener.mTradeCache.mOrigQualStrFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOrigQualNativeStr : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOrigQualNativeStr = field.getString();
                listener.mTradeCache.mOrigQualNativeStrFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOrigCondStr : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOrigCondStr = field.getString();
                listener.mTradeCache.mOrigCondStrFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOrigSellersSaleDays : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mOrigSellersSaleDays = field.getI64();
                listener.mTradeCache.mOrigSellersSaleDaysFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeOrigStopStockInd : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                switch (field.getType())
                {
                    case mamaFieldType.MAMA_FIELD_TYPE_CHAR:
                        listener.mTradeCache.mOrigStopStockInd = field.getChar();
                        listener.mTradeCache.mOrigStopStockIndFieldState = MamdaFieldState.MODIFIED;
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                        if (field.getString() != String.Empty)
                        {
                            listener.mTradeCache.mOrigStopStockInd = field.getString()[0];
                            listener.mTradeCache.mOrigStopStockIndFieldState = MamdaFieldState.MODIFIED;
                        }
                        else
                        {
                            listener.mTradeCache.mOrigStopStockInd = ' ';
                            listener.mTradeCache.mOrigStopStockIndFieldState = MamdaFieldState.MODIFIED;
                        }
                        break;
                    default: break;
                }
            }
        }

        private class TradeCorrPrice : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCorrPrice.copy(field.getPrice());
                listener.mTradeCache.mCorrPriceFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeCorrVolume : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCorrVolume = field.getF64();
                listener.mTradeCache.mCorrVolumeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeCorrPartId : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCorrPartId = field.getString();
                listener.mTradeCache.mCorrPartIdFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeCorrQualStr : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCorrQualStr = field.getString();
                listener.mTradeCache.mCorrQualStrFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeCorrQualNativeStr : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCorrQualNativeStr = field.getString();
                listener.mTradeCache.mCorrQualNativeStrFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeCorrCondStr : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCorrCondStr = field.getString();
                listener.mTradeCache.mCorrCondStrFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeCorrSellersSaleDays : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCorrSellersSaleDays = field.getI64();
                listener.mTradeCache.mCorrSellersSaleDaysFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeCorrStopStockInd : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                switch (field.getType())
                {
                    case mamaFieldType.MAMA_FIELD_TYPE_CHAR:
                        listener.mTradeCache.mCorrStopStockInd = field.getChar();
                        listener.mTradeCache.mCorrStopStockIndFieldState = MamdaFieldState.MODIFIED;
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                        if (field.getString() != String.Empty)
                        {
                            listener.mTradeCache.mCorrStopStockInd = field.getString()[0];
                            listener.mTradeCache.mCorrStopStockIndFieldState = MamdaFieldState.MODIFIED;
                        }
                        else
                        {
                            listener.mTradeCache.mCorrStopStockInd = ' ';
                            listener.mTradeCache.mCorrStopStockIndFieldState = MamdaFieldState.MODIFIED;
                        }
                        break;
                    default: break;
                }
            }
        }

        private class TradeCorrTime : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCorrTime = field.getDateTime();
                listener.mTradeCache.mCorrTimeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeCancelTime : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mCancelTime = field.getDateTime();
                listener.mTradeCache.mCancelTimeFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private class TradeIsIrregular : TradeUpdate
        {
            public void onUpdate(
                MamdaTradeListener listener,
                MamaMsgField field)
            {
                listener.mTradeCache.mIsIrregular = field.getBool();
                listener.mTradeCache.mIsIrregularFieldState = MamdaFieldState.MODIFIED;
            }
        }

        private static TradeUpdate[] createUpdaters()
        {
            mMaxFid = MamdaTradeFields.getMaxFid();
            TradeUpdate[] updaters = new TradeUpdate[mMaxFid + 1];

            addToUpdatersList(updaters, MamdaTradeFields.SYMBOL, new MamdaTradeSymbol());
            addToUpdatersList(updaters, MamdaTradeFields.ISSUE_SYMBOL, new MamdaTradeIssueSymbol());
            addToUpdatersList(updaters, MamdaTradeFields.PART_ID, new TradeLastPartId());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_ID, new TradeId());
            addToUpdatersList(updaters, MamdaTradeFields.ORIG_TRADE_ID, new OrigTradeId());
            addToUpdatersList(updaters, MamdaTradeFields.CORR_TRADE_ID, new CorrTradeId());
            addToUpdatersList(updaters, MamdaTradeFields.SRC_TIME, new TradeSrcTime());
            addToUpdatersList(updaters, MamdaTradeFields.ACTIVITY_TIME, new TradeActivityTime());
            addToUpdatersList(updaters, MamdaTradeFields.LINE_TIME, new TradeLineTime());
            addToUpdatersList(updaters, MamdaTradeFields.SEND_TIME, new TradeSendTime());
            addToUpdatersList(updaters, MamdaTradeFields.PUB_ID, new TradePubId());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_PRICE, new TradeLastPrice());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_SIZE, new TradeLastVolume());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_TIME, new TradeLastTime());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_DATE, new TradeTradeDate());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_DIRECTION, new TradeDirection());
            addToUpdatersList(updaters, MamdaTradeFields.AGGRESSOR_SIDE, new AggressorSide());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_SIDE, new TradeSide());
            addToUpdatersList(updaters, MamdaTradeFields.NET_CHANGE, new TradeNetChange());
            addToUpdatersList(updaters, MamdaTradeFields.PCT_CHANGE, new TradePctChange());
            addToUpdatersList(updaters, MamdaTradeFields.TOTAL_VOLUME, new TradeAccVolume());
            addToUpdatersList(updaters, MamdaTradeFields.OFF_EXCHANGE_TOTAL_VOLUME, new TradeOffExAccVolume());
            addToUpdatersList(updaters, MamdaTradeFields.ON_EXCHANGE_TOTAL_VOLUME, new TradeOnExAccVolume());
            addToUpdatersList(updaters, MamdaTradeFields.HIGH_PRICE, new TradeHighPrice());
            addToUpdatersList(updaters, MamdaTradeFields.LOW_PRICE, new TradeLowPrice());
            addToUpdatersList(updaters, MamdaTradeFields.OPEN_PRICE, new TradeOpenPrice());
            addToUpdatersList(updaters, MamdaTradeFields.CLOSE_PRICE, new TradeClosePrice());
            addToUpdatersList(updaters, MamdaTradeFields.PREV_CLOSE_PRICE, new TradePrevClosePrice());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_SEQNUM, new TradeEventSeqNum());
            addToUpdatersList(updaters, MamdaTradeFields.SHORT_SALE_CIRCUIT_BREAKER, new MamdaShortSaleCircuitBreaker());
            addToUpdatersList(updaters, MamdaTradeFields.ORIG_SHORT_SALE_CIRCUIT_BREAKER, new MamdaOrigShortSaleCircuitBreaker());
            addToUpdatersList(updaters, MamdaTradeFields.CORR_SHORT_SALE_CIRCUIT_BREAKER, new MamdaCorrShortSaleCircuitBreaker());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_QUALIFIER, new TradeQualStr());
            addToUpdatersList(updaters, MamdaTradeFields.SALE_CONDITION, new TradeQualNativeStr());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_PART_ID, new TradeLastPartId());
            addToUpdatersList(updaters, MamdaTradeFields.TOTAL_VALUE, new TradeTotalValue());
            addToUpdatersList(updaters, MamdaTradeFields.OFF_EXCHANGE_TOTAL_VALUE, new TradeOffExTotalValue());
            addToUpdatersList(updaters, MamdaTradeFields.ON_EXCHANGE_TOTAL_VALUE, new TradeOnExTotalValue());
            addToUpdatersList(updaters, MamdaTradeFields.VWAP, new TradeVWap());
            addToUpdatersList(updaters, MamdaTradeFields.OFF_EXCHANGE_VWAP, new TradeOffExVWap());
            addToUpdatersList(updaters, MamdaTradeFields.ON_EXCHANGE_VWAP, new TradeOnExVWap());
            addToUpdatersList(updaters, MamdaTradeFields.STD_DEV, new TradeStdDev());
            addToUpdatersList(updaters, MamdaTradeFields.STD_DEV_SUM, new TradeStdDevSum());
            addToUpdatersList(updaters, MamdaTradeFields.STD_DEV_SUM_SQUARES, new TradeStdDevSumSquares());
            addToUpdatersList(updaters, MamdaTradeFields.ORDER_ID, new TradeOrderId());
            addToUpdatersList(updaters, MamdaTradeFields.SETTLE_PRICE, new TradeSettlePrice());
            addToUpdatersList(updaters, MamdaTradeFields.SETTLE_DATE, new TradeSettleDate());
            addToUpdatersList(updaters, MamdaTradeFields.SELLERS_SALE_DAYS, new TradeSellerSalesDays());
            addToUpdatersList(updaters, MamdaTradeFields.STOP_STOCK_IND, new TradeStopStockInd());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_EXEC_VENUE, new TradeExecVenue());
            addToUpdatersList(updaters, MamdaTradeFields.OFF_EXCHANGE_TRADE_PRICE, new OffExTradePrice());
            addToUpdatersList(updaters, MamdaTradeFields.ON_EXCHANGE_TRADE_PRICE, new OnExTradePrice());
            addToUpdatersList(updaters, MamdaTradeFields.IS_IRREGULAR, new TradeIsIrregular());
            addToUpdatersList(updaters, MamdaTradeFields.ORIG_PART_ID, new TradeOrigPartId());
            addToUpdatersList(updaters, MamdaTradeFields.ORIG_PRICE, new TradeOrigPrice());
            addToUpdatersList(updaters, MamdaTradeFields.ORIG_SIZE, new TradeOrigVolume());
            addToUpdatersList(updaters, MamdaTradeFields.ORIG_SEQNUM, new TradeOrigSeqNum());
            addToUpdatersList(updaters, MamdaTradeFields.ORIG_TRADE_QUALIFIER, new TradeOrigQualStr());
            addToUpdatersList(updaters, MamdaTradeFields.ORIG_SALE_CONDITION, new TradeOrigQualNativeStr());
            addToUpdatersList(updaters, MamdaTradeFields.ORIG_SELLERS_SALE_DAYS, new TradeOrigSellersSaleDays());
            addToUpdatersList(updaters, MamdaTradeFields.ORIG_STOP_STOCK_IND, new TradeOrigStopStockInd());
            addToUpdatersList(updaters, MamdaTradeFields.CORR_PART_ID, new TradeCorrPartId());
            addToUpdatersList(updaters, MamdaTradeFields.CORR_PRICE, new TradeCorrPrice());
            addToUpdatersList(updaters, MamdaTradeFields.CORR_SIZE, new TradeCorrVolume());
            addToUpdatersList(updaters, MamdaTradeFields.CORR_TRADE_QUALIFIER, new TradeCorrQualStr());
            addToUpdatersList(updaters, MamdaTradeFields.CORR_SALE_CONDITION, new TradeCorrQualNativeStr());
            addToUpdatersList(updaters, MamdaTradeFields.CORR_SELLERS_SALE_DAYS, new TradeCorrSellersSaleDays());
            addToUpdatersList(updaters, MamdaTradeFields.CORR_STOP_STOCK_IND, new TradeCorrStopStockInd());
            addToUpdatersList(updaters, MamdaTradeFields.CORR_TIME, new TradeCorrTime());
            addToUpdatersList(updaters, MamdaTradeFields.CANCEL_TIME, new TradeCancelTime());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_COUNT, new TradeCount());
            addToUpdatersList(updaters, MamdaTradeFields.CONFLATE_COUNT, new TradeConflateCount());
            addToUpdatersList(updaters, MamdaTradeFields.TRADE_UNITS, new TradeUnits());
            addToUpdatersList(updaters, MamdaTradeFields.HIGH_SEQNUM, new TradeHighSeqNum());
            addToUpdatersList(updaters, MamdaTradeFields.LOW_SEQNUM, new TradeLowSeqNum());
            addToUpdatersList(updaters, MamdaTradeFields.LAST_SEQNUM, new TradeLastSeqNum());
            addToUpdatersList(updaters, MamdaTradeFields.TOTAL_VOLUME_SEQNUM, new TradeTotalVolumeSeqNum());
            addToUpdatersList(updaters, MamdaTradeFields.CURRENCY_CODE, new TradeCurrencyCode());
            addToUpdatersList(updaters, MamdaTradeFields.BLOCK_COUNT, new TradeBlockCount());
            addToUpdatersList(updaters, MamdaTradeFields.BLOCK_VOLUME, new TradeBlockVolume());
            addToUpdatersList(updaters, MamdaTradeFields.PREV_CLOSE_DATE, new TradePrevCloseDate());
            addToUpdatersList(updaters, MamdaTradeFields.ADJ_PREV_CLOSE, new TradeAdjPrevClose());
            addToUpdatersList(updaters, MamdaTradeFields.IRREG_PRICE, new TradeIrregPrice());
            addToUpdatersList(updaters, MamdaTradeFields.IRREG_SIZE, new TradeIrregVolume());
            addToUpdatersList(updaters, MamdaTradeFields.IRREG_PART_ID, new TradeIrregPartId());
            addToUpdatersList(updaters, MamdaTradeFields.IRREG_TIME, new TradeIrregTime());
            addToUpdatersList(updaters, MamdaTradeFields.UPDATE_AS_TRADE, new TradeUpdateAsTrade());

            return updaters;
        }

        private static void addToUpdatersList(
            TradeUpdate[] updaters,
            MamaFieldDescriptor fieldDesc,
            TradeUpdate updater)
        {
            if (fieldDesc == null) return;
            int fieldId = fieldDesc.getFid();
            if (fieldId <= mMaxFid)
            {
                updaters[fieldId] = updater;
            }
        }

        #endregion Updaters

        /// <summary>
        /// Private class used for Field Iteration
        /// </summary>
        private class FieldIterator : MamaMsgFieldIterator
        {
            private MamdaTradeListener mListener;
            public FieldIterator(MamdaTradeListener listener)
            {
                mListener = listener;
            }
            public void onField(
                MamaMsg msg,
                MamaMsgField field,
                Object closure)
            {
                try
                {
                    int fieldId = field.getFid();
                    if (fieldId <= mMaxFid)
                    {
                        TradeUpdate updater = MamdaTradeListener.mUpdaters[fieldId];
                        if (updater != null)
                        {
                            updater.onUpdate(mListener, field);
                        }
                    }
                }
                catch (Exception ex)
                {
                    throw new MamdaDataException(ex.Message, ex);
                }
            }
        }

        #endregion Implementation helpers

        #region MamaTradeCache

        internal class MamdaTradeCache
        {
            // The following fields are used for caching the offical last
            // price and related fields.  These fields can be used by
            // applications for reference and will be passed for recaps.
            public string mSymbol = null;
            public string mIssueSymbol = null;
            public string mPartId = null;
            public DateTime mSrcTime = DateTime.MinValue;
            public DateTime mActTime = DateTime.MinValue;
            public DateTime mLineTime = DateTime.MinValue;
            public DateTime mSendTime = DateTime.MinValue;
            public string mPubId = null;
            public bool mIsIrregular = false;
            public bool mWasIrregular = false;
            public MamaPrice mLastPrice = null;
            public double mLastVolume = 0;
            public string mLastPartId = null;
            public string mTradeId = null;
            public string mOrigTradeId = null;
            public string mCorrTradeId = null;
            public DateTime mLastTime = DateTime.MinValue;
            public DateTime mTradeDate = DateTime.MinValue;
            public MamaPrice mIrregPrice = null;
            public double mIrregVolume = 0;
            public string mIrregPartId = null;
            public DateTime mIrregTime = DateTime.MinValue;
            public double mAccVolume = 0;
            public double mOffExAccVolume = 0;
            public double mOnExAccVolume = 0;
            public string mTradeDirection = null;
            public MamdaTradeSide mTmpSide = new MamdaTradeSide();
            public string mSide = null;
            public MamaPrice mNetChange = null;
            public double mPctChange = 0.0;
            public MamaPrice mOpenPrice = null;
            public MamaPrice mHighPrice = null;
            public MamaPrice mLowPrice = null;
            public MamaPrice mClosePrice = null;
            public MamaPrice mPrevClosePrice = null;
            public DateTime mPrevCloseDate = DateTime.MinValue;
            public MamaPrice mAdjPrevClose = null;
            public long mTradeCount = 0;
            public double mBlockVolume = 0;
            public long mBlockCount = 0;
            public double mVwap = 0.0;
            public double mOffExVwap = 0.0;
            public double mOnExVwap = 0.0;
            public double mTotalValue = 0.0;
            public double mOffExTotalValue = 0.0;
            public double mOnExTotalValue = 0.0;
            public double mStdDev = 0.0;
            public double mStdDevSum = 0.0;
            public double mStdDevSumSquares = 0.0;
            public long mOrderId = 0;
            public MamaPrice mSettlePrice = null;
            public DateTime mSettleDate = DateTime.MinValue;
            public char mShortSaleCircuitBreaker;
            public char mOrigShortSaleCircuitBreaker;
            public char mCorrShortSaleCircuitBreaker;
            public long mConflateCount = 1;

            // The following fields are used for caching the last reported
            // trade, which might have been out of order.  The reason for
            // caching these values is to allow a configuration that passes
            // the minimum amount of data  (unchanged fields not sent).
            public long			mEventSeqNum = 0;
            public DateTime		mEventTime = DateTime.MinValue;
            public MamaPrice	mTradePrice = null;
            public double		mTradeVolume = 0;
            public string		mTradePartId = null;
            public string		mTradeQualStr = null;
            public string		mTradeQualNativeStr = null;
            public long			mSellersSaleDays = 0;
            public char			mStopStockInd = '\0';

            public string		mTradeExecVenue = null;
            public MamaPrice	mOffExTradePrice = null;
            public MamaPrice	mOnExTradePrice = null;

            // Additional fields for cancels/error/corrections:
            public bool			mIsCancel = false;
            public long			mOrigSeqNum = 0;
            public MamaPrice	mOrigPrice = null;
            public double		mOrigVolume = 0;
            public string		mOrigPartId = null;
            public string		mOrigQualStr = null;
            public string		mOrigQualNativeStr = null;
            public long			mOrigSellersSaleDays = 0;
            public char			mOrigStopStockInd = '\0';
            public MamaPrice	mCorrPrice = null;
            public double		mCorrVolume = 0;
            public string		mCorrPartId = null;
            public string		mCorrQualStr = null;
            public string		mCorrQualNativeStr = null;
            public long			mCorrSellersSaleDays = 0;
            public char			mCorrStopStockInd = '\0';
            public DateTime		mCorrTime = DateTime.MinValue;
            public DateTime		mCancelTime = DateTime.MinValue;
            public string		mTradeUnits = null;
            public long			mLastSeqNum = 0;
            public long			mHighSeqNum = 0;
            public long			mLowSeqNum = 0;
            public long			mTotalVolumeSeqNum = 0;
            public string		mCurrencyCode = null;

            public string		mTradeCondStr = null;
            public string		mOrigCondStr = null;
            public string		mCorrCondStr = null;

            public bool			mGotPartId = false;
            public bool			mLastGenericMsgWasTrade = false;
            public bool			mGotTradeTime = false;
            public bool			mGotTradePrice = false;
            public bool			mGotTradeSize = false;
            public bool			mGotTradeCount = false;
            public bool			mGotIssueSymbol = false;

            //Field State
            public MamdaFieldState mSymbolFieldState = new MamdaFieldState();
            public MamdaFieldState mIssueSymbolFieldState = new MamdaFieldState();
            public MamdaFieldState mPartIdFieldState = new MamdaFieldState();
            public MamdaFieldState mSrcTimeFieldState = new MamdaFieldState();
            public MamdaFieldState mActTimeFieldState = new MamdaFieldState();
            public MamdaFieldState mLineTimeFieldState = new MamdaFieldState();
            public MamdaFieldState mSendTimeFieldState = new MamdaFieldState();
            public MamdaFieldState mPubIdFieldState = new MamdaFieldState();
            public MamdaFieldState mIsIrregularFieldState = new MamdaFieldState();
            public MamdaFieldState mLastPriceFieldState = new MamdaFieldState();
            public MamdaFieldState mLastVolumeFieldState = new MamdaFieldState();
            public MamdaFieldState mLastPartIdFieldState = new MamdaFieldState();
            public MamdaFieldState mTradeIdFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigTradeIdFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrTradeIdFieldState = new MamdaFieldState();
            public MamdaFieldState mLastTimeFieldState = new MamdaFieldState();
            public MamdaFieldState mTradeDateFieldState = new MamdaFieldState();
            public MamdaFieldState mIrregPriceFieldState = new MamdaFieldState();
            public MamdaFieldState mIrregVolumeFieldState = new MamdaFieldState();
            public MamdaFieldState mIrregPartIdFieldState = new MamdaFieldState();
            public MamdaFieldState mIrregTimeFieldState = new MamdaFieldState();
            public MamdaFieldState mAccVolumeFieldState = new MamdaFieldState();
            public MamdaFieldState mOffExAccVolumeFieldState = new MamdaFieldState();
            public MamdaFieldState mOnExAccVolumeFieldState = new MamdaFieldState();
            public MamdaFieldState mTradeDirectionFieldState = new MamdaFieldState();
            public MamdaFieldState mSideFieldState = new MamdaFieldState();
            public MamdaFieldState mNetChangeFieldState = new MamdaFieldState();
            public MamdaFieldState mPctChangeFieldState = new MamdaFieldState();
            public MamdaFieldState mOpenPriceFieldState = new MamdaFieldState();
            public MamdaFieldState mHighPriceFieldState = new MamdaFieldState();
            public MamdaFieldState mLowPriceFieldState = new MamdaFieldState();
            public MamdaFieldState mClosePriceFieldState = new MamdaFieldState();
            public MamdaFieldState mPrevClosePriceFieldState = new MamdaFieldState();
            public MamdaFieldState mPrevCloseDateFieldState = new MamdaFieldState();
            public MamdaFieldState mAdjPrevCloseFieldState = new MamdaFieldState();
            public MamdaFieldState mTradeCountFieldState = new MamdaFieldState();
            public MamdaFieldState mBlockVolumeFieldState = new MamdaFieldState();
            public MamdaFieldState mBlockCountFieldState = new MamdaFieldState();
            public MamdaFieldState mVwapFieldState = new MamdaFieldState();
            public MamdaFieldState mOffExVwapFieldState = new MamdaFieldState();
            public MamdaFieldState mOnExVwapFieldState = new MamdaFieldState();
            public MamdaFieldState mTotalValueFieldState = new MamdaFieldState();
            public MamdaFieldState mOffExTotalValueFieldState = new MamdaFieldState();
            public MamdaFieldState mOnExTotalValueFieldState = new MamdaFieldState();
            public MamdaFieldState mStdDevFieldState = new MamdaFieldState();
            public MamdaFieldState mStdDevSumFieldState = new MamdaFieldState();
            public MamdaFieldState mStdDevSumSquaresFieldState = new MamdaFieldState();
            public MamdaFieldState mOrderIdFieldState = new MamdaFieldState();
            public MamdaFieldState mSettlePriceFieldState = new MamdaFieldState();
            public MamdaFieldState mSettleDateFieldState = new MamdaFieldState();
            public MamdaFieldState mShortSaleCircuitBreakerFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigShortSaleCircuitBreakerFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrShortSaleCircuitBreakerFieldState = new MamdaFieldState();
            public MamdaFieldState mEventSeqNumFieldState = new MamdaFieldState();
            public MamdaFieldState mEventTimeFieldState = new MamdaFieldState();
            public MamdaFieldState mTradePriceFieldState = new MamdaFieldState();
            public MamdaFieldState mTradeVolumeFieldState = new MamdaFieldState();
            public MamdaFieldState mTradePartIdFieldState = new MamdaFieldState();
            public MamdaFieldState mTradeQualStrFieldState = new MamdaFieldState();
            public MamdaFieldState mTradeQualNativeStrFieldState = new MamdaFieldState();
            public MamdaFieldState mSellersSaleDaysFieldState = new MamdaFieldState();
            public MamdaFieldState mStopStockIndFieldState = new MamdaFieldState();
            public MamdaFieldState mTradeExecVenueFieldState = new MamdaFieldState();
            public MamdaFieldState mOffExTradePriceFieldState = new MamdaFieldState();
            public MamdaFieldState mOnExTradePriceFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigSeqNumFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigPriceFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigVolumeFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigPartIdFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigQualStrFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigQualNativeStrFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigSellersSaleDaysFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigStopStockIndFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrPriceFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrVolumeFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrPartIdFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrQualStrFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrQualNativeStrFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrSellersSaleDaysFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrStopStockIndFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrTimeFieldState = new MamdaFieldState();
            public MamdaFieldState mCancelTimeFieldState = new MamdaFieldState();
            public MamdaFieldState mTradeCondStrFieldState = new MamdaFieldState();
            public MamdaFieldState mOrigCondStrFieldState = new MamdaFieldState();
            public MamdaFieldState mCorrCondStrFieldState = new MamdaFieldState();
            public MamdaFieldState mTradeUnitsFieldState = new MamdaFieldState();
            public MamdaFieldState mLastSeqNumFieldState = new MamdaFieldState();
            public MamdaFieldState mHighSeqNumFieldState = new MamdaFieldState();
            public MamdaFieldState mLowSeqNumFieldState = new MamdaFieldState();
            public MamdaFieldState mTotalVolumeSeqNumFieldState = new MamdaFieldState();
            public MamdaFieldState mCurrencyCodeFieldState = new MamdaFieldState();
            public MamdaFieldState mConflateCountFieldState = new MamdaFieldState();
        }

        #endregion MamaTradeCache

        #region State

        // We only need a single instance for use accross all Listener instances.
        // Each updater instance maintains no state.
        internal static TradeUpdate[] mUpdaters = null;
        private static object mUpdatersGuard = new object();
        private static int mMaxFid = 0;
        private bool mIgnoreUpdate = false;

        private readonly ArrayList mHandlers = new ArrayList();

        // Used for all field iteration processing
        private FieldIterator mFieldIterator = null;

        // Whether we should try to identify updates as trades.
        // This is true by default. Any feed wishing to not have updates processed
        // as trades will send a flag in the initial indicating the fact.
        private static bool mProcessUpdateAsTrade = true;

        // The following fields are used for caching the offical last
        // price and related fields.  These fields can be used by
        // applications for reference and will be passed for recaps.
        internal readonly MamdaTradeCache mTradeCache = new MamdaTradeCache();

        // Additional fields for gaps:
        private long mGapBegin = 0;
        private long mGapEnd = 0;
        private MamdaFieldState mGapBeginFieldState = new MamdaFieldState();
        private MamdaFieldState mGapEndFieldState = new MamdaFieldState();

        // Fields for closing reports:
        internal NullableBool mIsIndicative = new NullableBool();
        internal NullableLong mTmpTradeCount = new NullableLong();

        private MamdaFieldState mIsIndicativeFieldState = new MamdaFieldState();

        #endregion State

        #endregion Implementation details
    }
}
