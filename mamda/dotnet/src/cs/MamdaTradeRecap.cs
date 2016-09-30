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
    /// MamdaTradeRecap is an interface that provides access to trade
    /// related fields.
    /// </summary>
    public interface MamdaTradeRecap : MamdaBasicEvent
    {
        /// <summary>
        /// Monetary value of an individual share of the security at the time
        /// of the trade.
        /// </summary>
        /// <returns></returns>
        MamaPrice getLastPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getLastPriceFieldState();

        /// <summary>
        /// Number of shares traded in a single transaction for an individual
        /// security.
        /// </summary>
        /// <returns></returns>
        long getLastVolume();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getLastVolumeFieldState();

        /// <summary>
        /// Trade participant ID.  This is typically an exchange ID,
        /// sometimes a market maker ID.
        /// </summary>
        /// <returns></returns>
        string getLastPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getLastPartIdFieldState();

        /// <summary>
        /// Time corresponding to the last trade, as reported by the feed
        /// The exact time of the trade may not be available, since
        /// rules governing trade reporting allow for a trades to be
        /// reported within a specified time limit.
        /// </summary>
        /// <returns></returns>
        DateTime getLastTime();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getLastTimeFieldState();

        /// <summary>
        /// Monetary value of an individual share of the security at the time
        /// of the last irregular trade.
        /// </summary>
        /// <returns></returns>
        MamaPrice getIrregPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getIrregPriceFieldState();

        /// <summary>
        /// Number of shares traded in a single transaction for an individual
        /// security.
        /// </summary>
        /// <returns></returns>
        long getIrregVolume();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getIrregVolumeFieldState();

        /// <summary>
        /// Trade participant ID for the last irregular trade.  This is typically an
        /// exchange ID, sometimes a market maker ID.
        /// </summary>
        /// <returns></returns>
        string getIrregPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getIrregPartIdFieldState();

        /// <summary>
        /// Time corresponding to the last irregular trade, as reported by the feed
        /// The exact time of the trade may not be available, since
        /// rules governing trade reporting allow for a trades to be
        /// reported within a specified time limit.
        /// </summary>
        /// <returns></returns>
        DateTime getIrregTime();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getIrregTimeFieldState();

        DateTime getTradeDate();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeDateFieldState();

        /// <summary>
        /// The number of trades today.
        /// </summary>
        /// <returns></returns>
        long getTradeCount();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeCountFieldState();

        /// <summary>
        /// Total volume of shares traded in a security at the time it is
        /// disseminated.
        /// </summary>
        /// <returns></returns>
        long getAccVolume();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAccVolumeFieldState();

        /// <summary>
        /// Total volume of shares traded off exchange in a security at the time it is
        /// disseminated.
        /// </summary>
        /// <returns></returns>
        long getOffExAccVolume();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOffExAccVolumeFieldState();

        /// <summary>
        /// Total volume of shares traded on exchange in a security at the time it is
        /// disseminated.
        /// </summary>
        /// <returns></returns>
        long getOnExAccVolume();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOnExAccVolumeFieldState();

        /// <summary>
        /// Get the change in price compared with the previous closing price
        /// </summary>
        /// <returns>Change in price compared with the previous closing
        /// price (i.e. previous closing price - trade price).</returns>
        MamaPrice getNetChange();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getNetChangeFieldState();

        /// <summary>
        /// Change in price as a percentage.
        /// </summary>
        /// <returns></returns>
        double getPctChange();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getPctChangeFieldState();

        /// <summary>
        /// Trade tick direction.
        /// 0 : No direction is currently known/available.
        ///   + : Up tick.
        ///   - : Down tick.
        ///   0+ : Unchanged; Previous move was up tick.
        ///   0- : Unchanged; Previous move was down tick.
        ///   NA : Not applicable. (If it is meaningful to have
        ///   such a value for some exchanges.)
        /// </summary>
        /// <returns></returns>
        string getTradeDirection();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeDirectionFieldState();

		/// <summary>
		/// Returns the Aggressor side or TradeSide
		///
		/// AggressorSide:
		/// 0 : No AggressorSide is currently known/available.
		/// 1 or B : Buy
		/// 2 or S : Sell
		///
		/// TradeSide:
		/// TRADE_SIDE_UNKNOWN
		/// TRADE_SIDE_BUY
		/// TRADE_SIDE_SELL
		/// </summary>
		/// <returns></returns>
        string getSide();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSideFieldState();

        /// <summary>
        /// The price of the first qualifying trade in the security during
        /// the current trading day.
        /// </summary>
        /// <returns></returns>
        MamaPrice getOpenPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOpenPriceFieldState();

        /// <summary>
        /// Highest price paid for security during the trading day.
        /// </summary>
        /// <returns></returns>
        MamaPrice getHighPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getHighPriceFieldState();

        /// <summary>
        /// Lowest price paid for security during the trading day.
        /// </summary>
        /// <returns></returns>
        MamaPrice getLowPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getLowPriceFieldState();

        /// <summary>
        /// Today's closing price. The closing price field is populated when
        /// official closing prices are sent by the feed after the
        /// session close.
        /// </summary>
        /// <returns></returns>
        MamaPrice getClosePrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getClosePriceFieldState();

        /// <summary>
        /// The last qualifying trade price on the previous trading day. This
        /// field may be copied from the close price field during the
        /// morning "roll" of records in the feedhandler, or it may be
        /// obtained from a secondary source, or it may be explicitly sent by
        /// the feed prior to the opening of trading for the current day.
        /// </summary>
        /// <returns></returns>
        MamaPrice getPrevClosePrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getPrevClosePriceFieldState();

        /// <summary>
        /// Date corresponding to PrevClosePrice.
        /// <see>getPrevClosePrice()</see>
        /// </summary>
        /// <returns></returns>
        DateTime getPrevCloseDate();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getPrevCloseDateFieldState();

        /// <summary>
        /// The previous close price adjusted by corporate actions, such as
        /// dividends and stock splits on the ex-date. <see cref="getPrevClosePrice()"/>
        /// </summary>
        /// <returns>The adjusted previous closing price.</returns>
        MamaPrice getAdjPrevClose();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAdjPrevCloseFieldState();

        /// <summary>
        /// The number of block trades (at least 10,000 shares) today.
        /// </summary>
        /// <returns></returns>
        long getBlockCount();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBlockCountFieldState();

        /// <summary>
        /// Total volume of block trades today.
        /// </summary>
        /// <returns></returns>
        long getBlockVolume();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBlockVolumeFieldState();

        /// <summary>
        /// Volume-weighted average price of a security at the time it is
        /// disseminated. Equivalent to dividing total value by total
        /// volume.
        /// </summary>
        /// <returns></returns>
        double getVwap();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getVwapFieldState();

        /// <summary>
        /// Volume-weighted average off exchange price of a security at the time it is
        /// disseminated. Equivalent to dividing total value by total
        /// volume.
        /// </summary>
        /// <returns></returns>
        double getOffExVwap();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOffExVwapFieldState();

        /// <summary>
        /// Volume-weighted average on exchange price of a security at the time it is
        /// disseminated. Equivalent to dividing total value by total
        /// volume.
        /// </summary>
        /// <returns></returns>
        double getOnExVwap();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOnExVwapFieldState();

        /// <summary>
        /// Total value of all shares traded in a security at the time it is
        /// disseminated. Calculated by the sum of multiplying the trade price by trade volume
        /// for each qualifying trade.
        /// </summary>
        /// <returns></returns>
        double getTotalValue();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTotalValueFieldState();

        /// <summary>
        /// Total value of all shares traded off exchange in a security at the time it is
        /// disseminated. Calculated by the sum of multiplying the trade price by trade volume
        /// for each qualifying trade.
        /// </summary>
        /// <returns></returns>
        double getOffExTotalValue();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOffExTotalValueFieldState();

        /// <summary>
        /// Total value of all shares traded on exchange in a security at the time it is
        /// disseminated. Calculated by the sum of multiplying the trade price by trade volume
        /// for each qualifying trade.
        /// </summary>
        /// <returns></returns>
        double getOnExTotalValue();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOnExTotalValueFieldState();

        /// <summary>
        /// Standard deviation of last trade price of a security at the time
        /// it is disseminated.
        /// </summary>
        /// <returns></returns>
        double getStdDev();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getStdDevFieldState();

        double getStdDevSum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getStdDevSumFieldState();

        double getStdDevSumSquares();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getStdDevSumSquaresFieldState();

        /// <summary>
        /// Get the order id, if available.
        /// </summary>
        /// <returns>The trade message unique order id number (if available).</returns>
        long getOrderId ();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrderIdFieldState();

        /// <summary>
        /// Future's / Options settlement price.
        /// </summary>
        /// <returns></returns>
        MamaPrice getSettlePrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSettlePriceFieldState();

        /// <summary>
        /// Date corresponding to SettlePrice.
        /// <see>getSettlePrice()</see>
        /// </summary>
        /// <returns></returns>
        DateTime getSettleDate();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSettleDateFieldState();

        /// <summary>
        /// Trade execution venue.
        /// Unknown
        /// OnExchange
        /// OnExchangeOffBook
        /// OffExchange
        /// </summary>
        /// <returns></returns>
        string getTradeExecVenue();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeExecVenueFieldState();

        /// <summary>
        /// Monetary value of an individual off exchange share of the security at the time
        /// of the trade.
        /// </summary>
        /// <returns></returns>
        MamaPrice getOffExchangeTradePrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOffExchangeTradePriceFieldState();

        /// <summary>
        /// Monetary value of an individual off exchange share of the security at the time
        /// of the trade.
        /// </summary>
        /// <returns></returns>
        MamaPrice getOnExchangeTradePrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOnExchangeTradePriceFieldState();

        string getTradeUnits();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeUnitsFieldState();

        long getLastSeqNum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getLastSeqNumFieldState();

        long getHighSeqNum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getHighSeqNumFieldState();

        long getLowSeqNum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getLowSeqNumFieldState();

        long getTotalVolumeSeqNum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTotalVolumeSeqNumFieldState();

        string getCurrencyCode();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getCurrencyCodeFieldState();

		/// <summary>
		/// Returns the ShortSaleCircuitBreaker
		/// </summary>
		/// <returns></returns>
        char getShortSaleCircuitBreaker();

		/// <summary>
		/// Returns the FieldState, always MODIFIED
		/// </summary>
		/// <returns></returns>
        MamdaFieldState getShortSaleCircuitBreakerFieldState();
    }
}
