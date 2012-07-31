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

/**
 * MamdaTradeRecap is an interface that provides access to trade
 * related fields.
 */

public interface MamdaTradeRecap extends MamdaBasicRecap
{
    /**
     * Monetary value of an individual share of the security at the time
     * of the trade.
     */
    public MamaPrice  getLastPrice();

    /**
     * @return the last price Field State
     */
    public short  getLastPriceFieldState();

    /**
     * Number of shares traded in a single transaction for an individual
     * security.
     */
    public double getLastVolume();

    /**
     * @return the last volume Field State
     */
    public short  getLastVolumeFieldState();

    /**
     * Trade participant ID.  This is typically an exchange ID,
     * sometimes a market maker ID.
     */
    public String getLastPartId();

    /**
     * @return the last part ID Field State
     */
    public short  getLastPartIdFieldState();

    /**
     * Date corresponding to the last trade, as reported by the feed.
     */
    public MamaDateTime getLastDate();

    /**
     * @return the last date Field State
     */
    public short  getLastDateFieldState();


    /**
     * getShortSaleCircuitBreaker Returns the ShortSaleCircuitBreaker
     * @return ShortSaleCircuitBreaker
     * @see MamdaTradeReport#getShortSaleCircuitBreaker()
     */
    public char getShortSaleCircuitBreaker();  

    /**
     * @return Returns the FieldState, always MODIFIED.
     */
    public short getShortSaleCircuitBreakerFieldState();  
    
    /**
     * Time corresponding to the last trade, as reported by the feed.
     * The exact time of the trade may not be available, since
     * rules governing trade reporting allow for a trades to be
     * reported within a specified time limit.
     */
    public MamaDateTime getLastTime();

    /**
     * @return the last time Field State
     */
    public short  getLastTimeFieldState();

    /**
     * Monetary value of an individual share of the security at the time
     * of the last irregular trade.
     */
    public MamaPrice  getIrregPrice();

    /**
     * @return the irreg price Field State
     */
    public short  getIrregPriceFieldState();

    /**
     * Number of shares traded in a single transaction for an individual
     * security.
     */
    public double getIrregVolume();

    /**
     * @return the irreg volume Field State
     */
    public short  getIrregVolumeFieldState();

    /**
     * Trade participant ID for the last irregular trade.  This is typically 
     * an exchange ID, sometimes a market maker ID.
     */
    public String getIrregPartId();

    /**
     * @return the irreg part ID Field State
     */
    public short  getIrregPartIdFieldState();

    /**
     * Time corresponding to the last irregular trade, as reported by the feed.
     * The exact time of the trade may not be available, since
     * rules governing trade reporting allow for a trades to be
     * reported within a specified time limit.
     */
    public MamaDateTime getIrregTime();

    /**
     * @return the irreg time Field State
     */
    public short  getIrregTimeFieldState();

    /**
     * The number of trades today.
     */
    public long   getTradeCount();

    /**
     * @return the trade count Field State
     */
    public short  getTradeCountFieldState();

    /**
     * Total volume of shares traded in a security at the time it is
     * disseminated.
     */
    public double  getAccVolume();

    /**
     * @return the accumulated volume Field State
     */
    public short  getAccVolumeFieldState();

    /**
     * Total volume of shares traded off exchange in a security at the time it is
     * disseminated.
     */
    public double  getOffExAccVolume();

    /**
     * @return the off exchange accumulated volume Field State
     */
    public short  getOffExAccVolumeFieldState();

    /**
     * Total volume of shares traded on exchange in a security at the time it is
     * disseminated.
     */
    public double  getOnExAccVolume();

    /**
     * @return the on exchange accumulated volume Field State
     */
    public short  getOnExAccVolumeFieldState();

    /**
     * Get the change in price compared with the previous closing price.
     * @return Change in price compared with the previous closing
     * price (i.e. previous closing price - trade price).
     */
    public MamaPrice getNetChange();

    /**
     * @return the net change Field State
     */
    public short  getNetChangeFieldState();

    /**
     * Change in price as a percentage.
     */
    public double getPctChange();

    /**
     * @return the percentage change Field State
     */
    public short  getPctChangeFieldState();

    /**
     * Trade tick direction.
     * <ul>
     *   <li>0 : No direction is currently known/available.</li>
     *   <li>+ : Up tick.</li>
     *   <li>- : Down tick.</li>
     *   <li>0+ : Unchanged; Previous move was up tick.</li>
     *   <li>0- : Unchanged; Previous move was down tick.</li>
     *   <li>NA : Not applicable. (If it is meaningful to have
     *   such a value for some exchanges.)</li>
     * <ul>
     */
    public String getTradeDirection();

    /**
     * @return the trade direction Field State
     */
    public short  getTradeDirectionFieldState();
  
    /**
     * Returns the Aggressor Side or TradeSide
     * TradeSide
     * <ul>
     *   <li>0 : No TradeSide is currently known/available.</li>
     *   <li>1 or B : Buy</li>
     *   <li>2 or S : Sell</li>
     * <ul>
     * AggressorSide
     * <ul>
     *   <li>0 : No AggressorSide is currently known/available.</li>
     *   <li>1 or B : Buy</li>
     *   <li>2 or S : Sell</li>
     * <ul>
     */
    public String getSide();

    /**
     * @return the aggressor side or trade side Field State
     */
    public short  getSideFieldState();

    /**
     * The price of the first qualifying trade in the security during
     * the current trading day.
     */
    public MamaPrice getOpenPrice();

    /**
     * @return the open price Field Stated
     */
    public short  getOpenPriceFieldState();

    /**
     * Highest price paid for security during the trading day.
     */
    public MamaPrice getHighPrice();

    /**
     * @return the high price Field State
     */
    public short  getHighPriceFieldState();

    /**
     * Lowest price paid for security during the trading day.
     */
    public MamaPrice getLowPrice();

    /**
     * @return the low price Field State
     */
    public short  getLowPriceFieldState();

    /**
     * Today's closing price. The closing price field is populated when
     * official closing prices are sent by the feed after the
     * session close.
     */
    public MamaPrice getClosePrice();

    /**
     * @return the close price Field State
     */
    public short  getClosePriceFieldState();

    /**
     * The last qualifying trade price on the previous trading day. This
     * field may be copied from the close price field during the
     * morning "roll" of records in the feedhandler, or it may be
     * obtained from a secondary source, or it may be explicitly sent by
     * the feed prior to the opening of trading for the current day.
     */
    public MamaPrice getPrevClosePrice();

    /**
     * @return the prev close price Field State
     */
    public short  getPrevClosePriceFieldState();

    /**
     * Date corresponding to PrevClosePrice.
     * @see #getPrevClosePrice()
     */
    public MamaDateTime getPrevCloseDate();

    /**
     * @return the prev close date Field State
     */
    public short  getPrevCloseDateFieldState();

    /**
     * The previous close price adjusted by corporate actions, such as
     * dividends and stock splits on the ex-date.
     *
     * @return The adjusted previous closing price.
     * @see #getPrevClosePrice()
     */
    public MamaPrice getAdjPrevClose();

    /**
     * @return the adjusted prev close Field State
     */
    public short  getAdjPrevCloseFieldState();

    /**
     * The number of block trades (at least 10,000 shares) today.
     */
    public long   getBlockCount();

    /**
     * @return the block count Field State

     */
    public short  getBlockCountFieldState();

    /**
     * Total volume of block trades today.
     */
    public double getBlockVolume();

    /**
     * @return the block volume Field State
     */
    public short  getBlockVolumeFieldState();

    /**
     * Volume-weighted average price of a security at the time it is
     * disseminated. Equivalent to dividing total value by total
     * volume.
     */
    public double getVwap();

    /**
     * @return the Volume-weighted average price Field State
     */
    public short  getVwapFieldState();

    /**
     * Volume-weighted average off exchange price of a security at the time it is
     * disseminated. Equivalent to dividing total value by total
     * volume.
     */
    public double getOffExVwap();

    /**
     * @return the Volume-weighted average off exchange price Field State
     */
    public short  getOffExVwapFieldState();

    /**
     * Volume-weighted average on exchange price of a security at the time it is
     * disseminated. Equivalent to dividing total value by total
     * volume.
     */
    public double getOnExVwap();

    /**
     * @return the Volume-weighted average on exchange price Field State
     */
    public short  getOnExVwapFieldState();

    /**
     * Total value of all shares traded in a security at the time it is
     * disseminated. Calculated by summing the result of
     * multiplying the trade price by trade volume for each qualifying trade.
     */
    public double getTotalValue();

    /**
     * @return the total value Field State
     */
    public short  getTotalValueFieldState();

    /**
     * Total value of all shares traded off exchange in a security at the time it is
     * disseminated. Calculated by summing the result of
     * multiplying the trade price by trade volume for each qualifying trade.
     */
    public double getOffExTotalValue();

    /**
     * @return the off exchange total value Field State
     */
    public short  getOffExTotalValueFieldState();

    /**
     * Total value of all shares traded on exchange in a security at the time it is
     * disseminated. Calculated by summing the result of
     * multiplying the trade price by trade volume for each qualifying trade.
     */
    public double getOnExTotalValue();

    /**
     * @return the on exchange total value Field State
     */
    public short  getOnExTotalValueFieldState();

    /**
     * Standard deviation of last trade price of a security at the time
     * it is disseminated.
     */
    public double getStdDev();

    /**
     * @return the stddev Field State
     */
    public short  getStdDevFieldState();

    public double getStdDevSum();

    /**
     * @return the stddev sum Field State
     */
    public short  getStdDevSumFieldState();

    public double getStdDevSumSquares();

    /**
     * @return the stddev sum squares Field State
     */
    public short  getStdDevSumSquaresFieldState();

    /**
     * Get the order id, if available.
     * @return The trade message unique order id number (if
     * available).
     */
    public long getOrderId ();

    /**
     * @return the order id Field State
     */
    public short  getOrderIdFieldState();

    /**
     * Trade execution venue.
     * <ul>
     *   <li>Unknown</li>
     *   <li>OnExchange</li>
     *   <li>OnExchangeOffBook</li>
     *   <li>OffExchange</li>
     * <ul>
     */
    public String getTradeExecVenue();

    /**
     * @return the trade execution venue Field State
     */
    public short  getTradeExecVenueFieldState();

    /**
     * Monetary value of an individual off exchange share of the security at the time
     * of the trade.
     */
    public MamaPrice  getOffExchangeTradePrice();

    /**
     * @return the off exchange trade price Field State
     */
    public short  getOffExchangeTradePriceFieldState();

    /**
     * Monetary value of an individual on exchange share of the security at the time
     * of the trade.
     */
    public MamaPrice  getOnExchangeTradePrice();

    /**
     * @return the on exchange trade price Field State
     */
    public short  getOnExchangeTradePriceFieldState();

    /**
     * Reuters trade units. 
     */
    public String getTradeUnits ();

    /**
     * @return the trade units Field State
     */
    public short  getTradeUnitsFieldState();
    
    /**
     *   Sequence number of trade.  
     */
    public long getEventSeqNum ();

    /**
     * @return the event seq num Field State
     */
    public short  getEventSeqNumFieldState();
    
    /**
     *   Sequence number of last trade.  
     */
    public long getLastSeqNum ();

    /**
     * @return the last seq num Field State
     */
    public short  getLastSeqNumFieldState();

    /**
     *   Sequence number of trade.  
     */
    public long getHighSeqNum ();

    /**
     * @return the high seq num Field State
     */
    public short  getHighSeqNumFieldState();

    /**
     *   Sequence number of trade.  
     */
    public long getLowSeqNum ();

    /**
     * @return the low seq num Field State
     */
    public short  getLowSeqNumFieldState();

    /**
     *   Sequence number of trade.  
     */
    public long getTotalVolumeSeqNum ();

    /**
     * @return the total volume seq num Field State
     */
    public short  getTotalVolumeSeqNumFieldState();

    /**
     *   Sequence number of trade.  
     */
    public String getCurrencyCode ();

    /**
     * @return the currency code Field State
     */
    public short  getCurrencyCodeFieldState();

    /**
     *   Settle  of trade.  
     */
    public MamaPrice getSettlePrice();

    /**
     * @return the settle price Field State
     */
    public short  getSettlePriceFieldState();

    /**
     *   Settle date of trade.  
     */
    public MamaDateTime getSettleDate();
    
    /**
     * @return the settle date Field State
     */
    public short  getSettleDateFieldState();
    
}
