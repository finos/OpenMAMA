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

#ifndef MamdaTradeOutOfSequenceH
#define MamdaTradeOutOfSequenceH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicEvent.h>

namespace Wombat
{

    /**
     * MamdaTradeOutOfSequence is an interface that provides access to fields
     * related to trade updates which have been identified as being out of
     * sequence with previous update (e.g. retransmissions from exchange or a
     * fault tolerant take over event on the feedhandlers.).
     */
    class MAMDAExpDLL MamdaTradeOutOfSequence : public MamdaBasicEvent
    {
    public:
        /**
         * Get the trade price.
         *
         * @return The monetary value of an individual share of the
         * security at the time of the trade.
         */
        virtual const MamaPrice&  getTradePrice() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradePriceFieldState() const = 0;

        /**
         * Get the trade volume.
         * @return The number of shares traded in a single transaction for
         * an individual security.
         */
        virtual mama_quantity_t  getTradeVolume() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradeVolumeFieldState() const = 0;

        /**
         * Get the participant identifier for the trade. This is typically an 
         * exchange ID or a market maker ID.  I
         *
         * @return Trade participant ID.  
         */
        virtual const char*  getTradePartId() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradePartIdFieldState() const = 0;

        /**
         * Get the trade qualifier.  A normalized set of qualifiers for
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
         *     <td>Adjusted</td>
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
         *     <td>Unknown</td>
         *     <td>&nbsp;</td>
         *   </tr>
         * </table>
         *
         * @return Trade qualifier.
         */
        virtual const char*  getTradeQual() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradeQualFieldState() const = 0;

        /**
         * Get the Trade condition ("sale condition").
         * Feed-specific trade qualifier code(s).  This field is provided
         * primarily for completeness and/or troubleshooting.  
         *
         * @return The Trade Condition.
         * @see getTradeQual()
         */
        virtual const char*  getTradeQualNative() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradeQualNativeFieldState() const = 0;

        /**
         * Get the seller's sale days. Used when the trade qualifier is "Seller".
         * Specifies the number of days that may elapse before delivery of the
         * security.
         * 
         * @return The Seller's sale days.
         */
        virtual mama_u32_t  getTradeSellersSaleDays() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradeSellersSaleDaysFieldState() const = 0;

        /**
         * Get the Stopped stock indicator.  Condition related to certain
         * NYSE trading rules.  This is not related to a halted security
         * status.  (0 == N/A; 1 == Applicable)
         *
         * @return Stopped stock indicator.
         */
        virtual char  getTradeStopStock() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getTradeStopStockFieldState() const = 0;

        /**
         * Get whether or not the trade qualifies as an irregular
         * trade.  In general, only "regular" trades qualify to update the
         * official last price and high/low prices.
         *
         * @return Whether the trade is irregular or not.
         */
        virtual bool  getIsIrregular() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getIsIrregularFieldState() const = 0;

        /**
         * Get the trade's order id, if available.
         * @return The trade message unique order id number (if available).
         */
        virtual mama_u64_t  getOrderId() const = 0;

        /**
         * Get the field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getOrderIdFieldState() const = 0;

        virtual ~MamdaTradeOutOfSequence() {};
    };

} // namespace

#endif // MamdaTradeOutOfSequenceH
