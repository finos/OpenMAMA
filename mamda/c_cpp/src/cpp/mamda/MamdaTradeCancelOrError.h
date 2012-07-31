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

#ifndef MamdaTradeCancelOrErrorH
#define MamdaTradeCancelOrErrorH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    /**
     * MamdaTradeCancelOrError is an interface that provides access to trade
     * cancellation related fields.
     */

    class MAMDAExpDLL MamdaTradeCancelOrError : public MamdaBasicEvent
    {
    public:
        /**
         * Return whether this event is a trade cancel.  If false, the
         * event is a trade error.
         *
         * @return Whether this is a trade cancel.
         */
        virtual bool  getIsCancel() const = 0;

        /**
         * Original feed-generated sequence for a correction/cancel/error.
         * 
         * @return The original sequence number.
         */
        virtual mama_seqnum_t  getOrigSeqNum() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOrigSeqNumFieldState() const = 0;

        /**
         * Original trade price in a correction/cancel/error.
         *
         * @return The original trade price.
         */
        virtual const MamaPrice&  getOrigPrice() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOrigPriceFieldState() const = 0;

        /**
         * Original trade size in a correction/cancel/error.
         * 
         * @return The original trade volume.
         */
        virtual mama_quantity_t  getOrigVolume() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOrigVolumeFieldState() const = 0;

        /**
         * Original trade participant identifier in a correction/cancel/error.
         *
         * @return The original trade participant identifier.
         */
        virtual const char*  getOrigPartId() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOrigPartIdFieldState() const = 0;

        /**
         * A normalized set of qualifiers for the original trade for the
         * security in a correction/cancel/error.
         *
         * @return The original trade qualifier.
         */
        virtual const char*  getOrigQual() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOrigQualFieldState() const = 0;

        /**
         * Feed-specific trade qualifier code(s) for original trade.
         * This field is provided primarily for completeness and/or
         * troubleshooting.
         *
         * @return The original trade condition.
         */
        virtual const char*  getOrigQualNative() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOrigQualNativeFieldState() const = 0;

        /**
         * Seller's sale days for original trade. Used when the trade
         * qualifier is "Seller". Specifies the number of days that may elapse
         * before delivery of the security.
         *
         * @return The original seller's sale days.
         */
        virtual mama_u32_t  getOrigSellersSaleDays() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOrigSellersSaleDaysFieldState() const = 0;

        /**
         * Stopped stock indicator for original trade.
         * Condition related to certain NYSE trading rules.
         * This is not related to a halted security
         * status.  (0 == N/A; 1 == Applicable)
         *
         * @return The original stopped stock indicator.
         */
        virtual char  getOrigStopStock() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOrigStopStockFieldState() const = 0;

        /**
         * Get the wombat normalized trade qualifier.
         * @return Trade qualifier.  A normalized set of qualifiers for
         * the current trade for the security. This field may contain
         * multiple string values, separated by the colon(:) character.
         *
         * <table width="100%" border="1">
         *   <tr><td><b>Value</b></td><td><b>Meaning</b></td></tr>
         *   <tr>
         *     <td>Normal</td>
         *     <td>Regular trade. A trade made without stated conditions
         *     is deemed regular way for settlement on the third *
         *     business day following the transaction * date.</td>
         *   </tr>
         *   <tr>
         *     <td>Acquisition</td>
         *     <td>A transaction made on the Exchange as a result of an
         *     Exchange acquisition.</td>
         *   </tr>
         *   <tr>
         *     <td>Bunched</td>
         *     <td>A trade representing an aggregate of two or more
         *     regular trades in a security occurring at the same price
         *     either simultaneously or within the same 60 second period,
         *     with no individual trade exceeding 10,000 shares.</td>
         *   </tr>
         *   <tr>
         *     <td>Cash</td>
         *     <td>A transaction which calls for the delivery of
         *     securities and payment on the same day the trade takes
         *     place.</td>
         *   </tr>
         *   <tr>
         *     <td>Distribution</td>
         *     <td>Sale of a large block of stock in such a manner that
         *     the price is not adversely affected.</td>
         *   </tr>
         *   <tr>
         *     <td>BunchedSold</td>
         *     <td>A bunched trade which is reported late</td>
         *   </tr>
         *   <tr>
         *     <td>Rule155</td>
         *     <td>To qualify as a 155 print, a specialist arranges for
         *     the sale of the block at one &quot;clean-up&quot; price or
         *     at the different price limits on his book. If the block is
         *     sold at a "clean-up" price, the specialist should execute
         *     at the same price all the executable buy orders on his
         *     book. The sale qualifier is only applicable for AMEX
         *     trades.</td>
         *   </tr>
         *   <tr>
         *     <td>SoldLast</td>
         *     <td>Sold Last is used when a trade prints in sequence but
         *     is reported late or printed in conformance to the One or
         *     Two Point Rule.</td>
         *   </tr>
         *   <tr>
         *     <td>NextDay</td>
         *     <td>A transaction which calls for delivery of securities on
         *     the first business day after the trade date.</td>
         *   </tr>
         *   <tr>
         *     <td>Opened</td>
         *     <td>Indicates an opening transaction that is printed out of
         *     sequence or reported late or printed in conformance to the
         *     One or Two Point Rule.</td>
         *   </tr>
         *   <tr>
         *     <td>PriorRef</td>
         *     <td>An executed trade that relates to an obligation to
         *     trade at an earlier point in the trading day or that refer
         *     to a prior reference price. This may be the result of an
         *     order that was lost or misplaced or a SelectNet order that
         *     was not executed on a timely basis.</td>
         *   </tr>
         *   <tr>
         *     <td>Seller</td>
         *     <td>A Seller's option transaction is a special transaction
         *     which gives the seller the right to deliver the stock at
         *     any time within a specific period, ranging from not less
         *     than four calendar days to no more than 60 calendar
         *     days.</td>
         *   </tr>
         *   <tr>
         *     <td>SplitTrade</td>
         *     <td>An execution in two markets when the specialist or
         *     Market Maker in the market first receiving the order agrees
         *     to execute a portion of it at whatever price is realized in
         *     another market to which the balance of the order is
         *     forwarded for execution.</td>
         *   </tr>
         *   <tr>
         *     <td>FormT</td>
         *     <td>See PrePostMkt.  Currently, all feed handlers that post
         *     Form-T trades - except CTA - send this qualifier for Form-T
         *     trades.  In the next major release, all fields will use
         *     PrePostMkt and FormT will be obsolete.</td>
         *   </tr>
         *   <tr>
         *     <td>PrePostMkt</td>
         *     <td>A trade reported before or after the normal trade
         *     reporting day. This is also known as a Form-T trade. The
         *     volume of Form-T trades will be included in the calculation
         *     of total volume. The price information in Form-T trades
         *     will not be used to update high, low and last sale data for
         *     individual securities or Indices since they occur outside
         *     of normal trade reporting hours.  Currently, all feed
         *     handlers that post Form-T trades - except CTA - send the
         *     "FormT" qualifier for Fot-T trades.  In the next major
         *     release, all feed handlers will use PrePostMkt and FormT
         *     will be obsolete.</td>
         *   </tr>
         *   <tr>
         *     <td>AvPrice</td>
         *     <td>A trade where the price reported is based upon an
         *     average of the prices for transactions in a security during
         *     all or any portion of the trading day.</td>
         *   </tr>
         *   <tr>
         *     <td>Sold</td>
         *     <td>Sold is used when a trade is printed (reported) out of
         *     sequence and at a time different from the actual
         *     transaction time.</td>
         *   </tr>
         *   <tr>
         *     <td>Adjusted</tr>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>Auto</td>
         *     <td>A sale condition code that identifies a NYSE trade that
         *     has been automatically executed without the potential
         *     benefit of price improvement. </td>
         *   </tr>
         *   <tr>
         *     <td>Basket</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>CashOnly</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>NextDayOnly</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>SpecTerms</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>Stopped</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>CATS</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>VCT</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>Rule127</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>BurstBasket</td>
         *     <td>A burst basket execution signifies a trade wherein the
         *     equity Specialists, acting in the aggregate as a market
         *     maker, purchase or sell the component stocks required for
         *     execution of a specific basket trade.</td>
         *   </tr>
         *   <tr>
         *     <td>OpenDetail</td>
         *     <td>Opening trade detail message.  Sent by CTS only and is
         *     a duplicate report of an earlier trade.  Note: since feed
         *     handler version 2.14.32, it is configurable whether these
         *     detail messages are published.</td>
         *   </tr>
         *   <tr>
         *     <td>Detail</td>
         *     <td>Trade detail message.  Sent by CTS only and is a
         *     duplicate report of an earlier trade.  Note: since feed
         *     handler version 2.14.32, it is configurable whether these
         *     detail messages are published.</td>
         *   </tr>
         *   <tr>
         *     <td>Reserved</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>BasketCross</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>BasketIndexOnClose</td>
         *     <td>A basket index on close transaction signifies a trade involving
         *     paired basket orders,the execution of which is based on the closing
         *     value of the index. These trades are reported after the close when
         *     the index closing value is determined.</td>
         *   </tr>
         *   <tr>
         *     <td>IntermarketSweep</td>
         *     <td>Indicates to CTS data recipients that the execution price
         *     reflects the order instruction not to send the order to another
         *     market that may have a superior price.</td>
         *   </tr>
         *   <tr>
         *     <td>YellowFlag</td>
         *     <td>Regular trades reported during specific events as out of the
         *     ordinary.</td>
         *   </tr>
         *   <tr>
         *     <td>MarketCenterOpen</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>MarketCenterClose</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *   <tr>
         *     <td>Unknown</td>
         *     <td>&nbsp;</td>
         *   </tr>
         *  </table>
         */
        virtual const char* getTradeQual() const = 0;

        /**
         * The trade qual Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradeQualFieldState() const = 0;

        /**
         * Get whether the trade is irregular.
         * @return Whether or not the trade qualifies as an irregular
         * trade.  In general, only "regular" trades qualify to update the
         * official last price and high/low prices.
         */
        virtual bool  getIsIrregular() const = 0;

        /**
         * The isIrregular Field State.
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getIsIrregularFieldState() const = 0;

        // Used by some feeds to indicate that a cancel or correction was for an unknown trade
        virtual bool            getGenericFlag()           const = 0;
        virtual MamdaFieldState getGenericFlagFieldState() const = 0;

        /**
         * Get the original trade id
         * @return the original trade id
         */
        virtual const char*     getOrigTradeId()           const = 0;
        virtual MamdaFieldState getOrigTradeIdFieldState() const = 0;

        /**
         *get the OrigShortSaleCircuitBreaker
         *@return OrigShortSaleCircuitBreaker 
         * <ul>
         *   <li>return values:</li>
         *   <li>Blank: Short Sale Restriction Not in Effect.</li>
         *   <li>A: Short Sale Restriction Activiated.</li>
         *   <li>C: Short Sale Restriction Continued.</li>
         *   <li>D: Sale Restriction Deactivated.</li>
         *   <li>E: Sale Restriction in Effect.</li>
         * </ul>    
         */    
        virtual char  getOrigShortSaleCircuitBreaker() const = 0;
        
        /**
         * @return The OrigShortSaleCircuitBreaker Field State.
         */
        virtual MamdaFieldState  getOrigShortSaleCircuitBreakerFieldState() const = 0;   
        
        virtual ~MamdaTradeCancelOrError() {};
    };


} // namespace

#endif // MamdaTradeCancelOrErrorH
