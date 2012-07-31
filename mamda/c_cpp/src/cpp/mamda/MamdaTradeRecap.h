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

#ifndef MamdaTradeRecapH
#define MamdaTradeRecapH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicRecap.h>
#include <mamda/MamdaTradeDirection.h>
#include <mamda/MamdaTradeExecVenue.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    /**
     * MamdaTradeRecap is an interface that provides access to trade
     * related fields.
     */
    class MAMDAExpDLL MamdaTradeRecap : public MamdaBasicRecap
    {
    public:

        /**
         * Monetary value of an individual share of the security at the time
         * of the trade.
         *
         * @return The last trade price.
         */
        virtual const MamaPrice&  getLastPrice() const = 0;

        /**
         * Number of shares traded in a single transaction for an individual
         * security.
         *
         * @return The last trade volume.
         */
        virtual mama_quantity_t  getLastVolume() const = 0;

        /**
         * Trade participant ID.  This is typically an exchange ID,
         * sometimes a market maker ID.
         *
         * @return The last trade participant identifier.
         */
        virtual const char*  getLastPartId() const = 0;

        /**
         * Time corresponding to the last trade, as reported by the feed.
         * The exact time of the trade may not be available, since rules
         * governing trade reporting allow for a trades to be reported
         * within a specified time limit.
         *
         * @return The last trade time.
         */
        virtual const MamaDateTime&  getLastTime() const = 0;

        /**
         * Monetary value of an individual share of the security at the time
         * of the last irregular trade.
         *
         * @return The last irregular trade price.
         */
        virtual const MamaPrice&  getIrregPrice() const = 0;

        /**
         * Number of shares traded in a single transaction for an individual
         * security.
         *
         * @return The last irregular trade volume.
         */
        virtual mama_quantity_t  getIrregVolume() const = 0;

        /**
         * Irregular trade participant ID.  This is typically an exchange ID,
         * sometimes a market maker ID.
         *
         * @return The last irregular trade participant identifier.
         */
        virtual const char*  getIrregPartId() const = 0;

        /**
         * Time corresponding to the last irregular trade, as reported by the feed.
         * The exact time of the trade may not be available, since rules
         * governing trade reporting allow for a trades to be reported
         * within a specified time limit.
         *
         * @return The last irregular trade time.
         */
        virtual const MamaDateTime&  getIrregTime() const = 0;

        /**
         * Time corresponding to the last trade, as reported by the feed.
         * The exact time of the trade may not be available, since rules
         * governing trade reporting allow for a trades to be reported
         * within a specified time limit.
         *
         * @return The last trade time.
         */
        virtual const MamaDateTime&  getTradeDate() const = 0;

        /**
         * The number of trades today.
         *
         * @return The number of trades so far today.
         */
        virtual mama_u32_t  getTradeCount() const = 0;

        /**
         * Total volume of shares traded in a security at the time it is
         * disseminated.
         *
         * @return Accumulated trade volume.
         */
        virtual mama_quantity_t  getAccVolume() const = 0;

        /**
         * Total volume of off-exchange shares traded in a security at 
         * the time it is disseminated.
         *
         * @return Accumulated off-exchange trade volume.
         */
        virtual mama_quantity_t  getOffExAccVolume() const = 0;

        /**
         * Total volume of on-exchange shares traded in a security at 
         * the time it is disseminated.
         *
         * @return Accumulated on-exhange trade volume.
         */
        virtual mama_quantity_t  getOnExAccVolume() const = 0;

        /**
         * Change in price compared with the previous closing price (i.e.
         * previous closing price - trade price).
         *
         * @return Price change compared to previous price.
         */
        virtual const MamaPrice&  getNetChange() const = 0;

        /**
         * Percentage change in price compared with the previous closing price (i.e.
         * previous closing price - trade price).
         *
         * @return Percentage price change compared to previous price.
         */
        virtual double  getPctChange() const = 0;

        /**
         * Trade tick direction. See MamdaTradeDirection.h for details.
         *
         * @return The tick direction.
         */
        virtual MamdaTradeDirection  getTradeDirection() const = 0;

        /**
         * The price of the first qualifying trade in the security during
         * the current trading day.
         *
         * @return The opening price.
         */
        virtual const MamaPrice&  getOpenPrice() const = 0;

        /**
         * Highest price paid for security during the trading day.
         *
         * @return The highest trade price for the day.
         */
        virtual const MamaPrice&  getHighPrice() const = 0;

        /**
         * Lowest price paid for security during the trading day.
         *
         * @return The lowest trade price for the day.
         */
        virtual const MamaPrice&  getLowPrice() const = 0;

        /**
         * Today's closing price. The closing price field is populated when
         * official closing prices are sent by the feed after the
         * session close.
         *
         * @return The closing price for the day.
         */
        virtual const MamaPrice&  getClosePrice() const = 0;

        /**
         * The last qualifying trade price on the previous trading day. This
         * field may be copied from the close price field during the
         * morning "roll" of records in the feedhandler, or it may be
         * obtained from a secondary source, or it may be explicitly sent by
         * the feed prior to the opening of trading for the current day.
         *
         * @return The last qualifying trade price on the previous trading day.
         */
        virtual const MamaPrice&  getPrevClosePrice() const = 0;

        /**
         * The previous close price adjusted by corporate actions, such as dividends and
         * stock splits on the ex-date.
         *
         * @return The adjusted previous closing price.
         * @see getPrevClosePrice()
         */
        virtual const MamaPrice&  getAdjPrevClosePrice() const = 0;

        /**
         * Date corresponding to wPrevClosePrice. 
         *
         * @return The closing price from the previous trading day.
         * @see getPrevClosePrice().
         */
        virtual const MamaDateTime&  getPrevCloseDate() const = 0;

        /**
         * The number of block trades (at least 10,000 shares) today.
         *
         * @return Number of block trades.
         */
        virtual mama_u32_t  getBlockCount() const = 0;

        /**
         * Total volume of block trades today.
         *
         * @return Total volumn of block trades.
         */
        virtual mama_quantity_t  getBlockVolume() const = 0;

        /**
         * Volume-weighted average price of a security at the time it is
         * disseminated. Equivalent to dividing total value by total
         * volume.
         *
         * @return The VWAP (Volume-weighted average price)
         */
        virtual double  getVwap() const = 0;

        /**
         * Volume-weighted average price of an off-exchange security at the time it is
         * disseminated. Equivalent to dividing the off-exchange total value by 
         * the off-exchange total volume.
         *
         * @return The off-exchange VWAP (Volume-weighted average price)
         */
        virtual double  getOffExVwap() const = 0;

        /**
         * Volume-weighted average price of an on-exchange security at the time it is
         * disseminated. Equivalent to dividing on-echange total value by 
         * the on-exchange total volume.
         *
         * @return The on-exchange VWAP (Volume-weighted average price)
         */
        virtual double  getOnExVwap() const = 0;

        /**
         * Total value of all shares traded in a security at the time it is
         * disseminated. Calculated by summing the result of
         * multiplying the trade price by trade volume for each qualifying trade.
         * 
         * @return Total value of all instruments traded.
         */
        virtual double  getTotalValue() const = 0;

        /**
         * Total value of all off-exchange shares traded in a security at the time it is
         * disseminated. Calculated by summing the result of
         * multiplying the trade price by trade volume for each qualifying trade.
         * 
         * @return Total value of all off-exhange instruments traded.
         */
        virtual double  getOffExTotalValue() const = 0;
        
        /**
         * Total value of all on-exchange shares traded in a security at the time it is
         * disseminated. Calculated by summing the result of
         * multiplying the trade price by trade volume for each qualifying trade.
         * 
         * @return Total value of all on-exchange instruments traded.
         */
        virtual double  getOnExTotalValue() const = 0;
        
        /**
         * Standard deviation of last trade price of a security at the time
         * it is disseminated.
         * 
         * @return The standard deviation of last trade price.
         */
        virtual double  getStdDev() const = 0;

        /**
         * Sum of the standard deviations.
         *
         * @return The sum of the standard deviations.
         */
        virtual double  getStdDevSum() const = 0;

        /**
         * Square of the sum of the standard deviations.
         *
         * @return The square of the sum of the standard deviations.
         */
        virtual double  getStdDevSumSquares() const = 0;

        /**
         * Reuters trade units.  
         *
         * @return Reuters trade units
         */
        virtual const char*  getTradeUnits() const = 0;  

        /**
         * Sequence number of the last trade.
         *
         * @return The sequence number of the last trade.
         */
        virtual mama_seqnum_t  getLastSeqNum() const = 0;

        /**
         * Sequence number of incoming message which gives high value.
         *
         * @return Sequence number of incoming message which gives high value.
         */
        virtual mama_seqnum_t  getHighSeqNum() const = 0;

        /**
         * Sequence number of incoming message which gives low value.
         *
         * @return Sequence number of incoming message which gives low value.
         */
        virtual mama_seqnum_t  getLowSeqNum() const = 0;

        /**
         * Sequence number of trade.
         *
         * @return Sequence number of trade. 
         */
         virtual mama_seqnum_t  getEventSeqNum() const = 0;

         virtual mama_seqnum_t  getTotalVolumeSeqNum() const = 0;

        /**
         * Currency of the trade (eg US$)
         *
         * @return Currency of the trade. 
         */
        virtual const char*  getCurrencyCode() const = 0;

        /**
         * Settle price of trade.
         *
         * @return Settle price of trade. 
         */
        virtual const MamaPrice&  getSettlePrice() const = 0;

        /**
         * Settle date of trade.
         *
         * @return Settle date of trade. 
         */
        virtual const MamaDateTime&  getSettleDate() const = 0;

        /**
         * Trade execution venue. See MamdaTradeExecVenue.h for details.
         *
         * @return The trade execution venue.
         */
        virtual MamdaTradeExecVenue  getTradeExecVenue() const = 0;

        /**
         * Monetary value of an individual share of the security off exchange at the time
         * of the trade.
         *
         * @return The last off exchange trade price.
         */
        virtual const MamaPrice&  getOffExchangeTradePrice() const = 0;

        /**
         * Monetary value of an individual share of the security on exchange at the time
         * of the trade.
         *
         * @return The last on exchange trade price.
         */
        virtual const MamaPrice&  getOnExchangeTradePrice() const = 0;

        /**
         * @return the Aggressor Side or TradeSide
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
        virtual const char*  getSide() const = 0;

        /**
         * get the ShortSaleCircuitBreaker
         * @return ShortSaleCircuitBreaker 
         * <ul>
         *   <li>return values:</li>
         *   <li>Blank: Short Sale Restriction Not in Effect.</li>
         *   <li>A: Short Sale Restriction Activiated.</li>
         *   <li>C: Short Sale Restriction Continued.</li>
         *   <li>D: Sale Restriction Deactivated.</li>
         *   <li>E: Sale Restriction in Effect.</li>
         * </ul>    
         */    
        virtual char  getShortSaleCircuitBreaker() const=0;


        /*  FieldState Accessors    */
        
        /**
         * The last trade price Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getLastPriceFieldState() const = 0;

        /**
         * The last volume Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getLastVolumeFieldState() const = 0;

        /**
         * The last part Id Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getLastPartIdFieldState() const = 0;

        /**
         * The last time Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getLastTimeFieldState() const = 0;

        /**
         * The irreg price Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getIrregPriceFieldState() const = 0;

        /**
         * The irreg volume Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getIrregVolumeFieldState() const = 0;

        /**
         * The irreg part Id Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getIrregPartIdFieldState() const = 0;

        /**
         * The irregular time Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getIrregTimeFieldState() const = 0;

        /**
         * The trade date Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradeDateFieldState() const = 0;

        /**
         * The trade count Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradeCountFieldState() const = 0;

        /**
         * The accumulated volume Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAccVolumeFieldState() const = 0;

        /**
         * The off exchange accumulated volume Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOffExAccVolumeFieldState() const = 0;

        /**
         * The on exchange accumulated volume Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOnExAccVolumeFieldState() const = 0;

        /**
         * The net change Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getNetChangeFieldState() const = 0;

        /**
         * The percentage change Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getPctChangeFieldState() const = 0;

        /**
         * The trade direction Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradeDirectionFieldState() const = 0;

        /**
         * The open price Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOpenPriceFieldState() const = 0;

        /**
         * The high price Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getHighPriceFieldState() const = 0;

        /**
         * The low price Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getLowPriceFieldState() const = 0;

        /**
         * The close price Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getClosePriceFieldState() const = 0;

        /**
         * The previous close price Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getPrevClosePriceFieldState() const = 0;

        /**
         * The adjusted previous close date Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAdjPrevClosePriceFieldState() const = 0;

        /**
         * The previous close date Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getPrevCloseDateFieldState() const = 0;

        /**
         * The block count Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBlockCountFieldState() const = 0;

        /**
         * The block volume Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBlockVolumeFieldState() const = 0;

        /**
         * The Vwap Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getVwapFieldState() const = 0;

        /**
         * The off exchange Vwap Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOffExVwapFieldState() const = 0;

        /**
         * The on exchange Vwap Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOnExVwapFieldState() const = 0;

        /**
         * The total value Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTotalValueFieldState() const = 0;

        /**
         * The Off Exchange Total Value Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOffExTotalValueFieldState() const = 0;
        
        /**
         * The On Exchange Total Value Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOnExTotalValueFieldState() const = 0;
        
        /**
         * The std deviation Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getStdDevFieldState() const = 0;

        /**
         * The std deviation sum Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getStdDevSumFieldState() const = 0;

        /**
         * The StdDevSumSquares Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getStdDevSumSquaresFieldState() const = 0;

        /**
         * The trade units Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradeUnitsFieldState() const = 0;  

        /**
         * The last SeqNum Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getLastSeqNumFieldState() const = 0;

        /**
         * The high seqNum Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getHighSeqNumFieldState() const = 0;

        /**
         * The low SeqNum Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getLowSeqNumFieldState() const = 0;

        /**
         * The event SeqNum Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getEventSeqNumFieldState() const = 0;
         
        /**
         * The total volume seqNum Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTotalVolumeSeqNumFieldState() const = 0;

        /**
         * The currency code Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getCurrencyCodeFieldState() const = 0;
         
         /**
         * The settle price Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getSettlePriceFieldState() const = 0;

        /**
         * The last trade price Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getSettleDateFieldState() const = 0;

        /**
         * The settle date Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOffExchangeTradePriceFieldState() const = 0;

        /**
         * The onExchange trade price Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOnExchangeTradePriceFieldState() const = 0;
        
        /**
         * The TradeSide or AggressorSide Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getSideFieldState() const = 0; 
        
        /**
         * @return The ShortSaleCircuitBreaker Field State.
         * An enumeration representing field state.    
         */
        virtual MamdaFieldState  getShortSaleCircuitBreakerFieldState() const = 0;  
        
        virtual ~MamdaTradeRecap() {};
    };

} // namespace

#endif // MamdaTradeRecapH
