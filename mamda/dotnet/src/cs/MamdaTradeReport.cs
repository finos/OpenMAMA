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
	/// MamdaTradeReport is an interface that provides access to fields
	/// related to a trade report.  This class is used for all trade
	/// reports, whether those trades qualify as regular or irregular
	/// trades.  (A regular trade generally qualifies to update the
	/// official last price and intraday high/low prices.)
	/// </summary>
	public interface MamdaTradeReport : MamdaBasicEvent
	{
		/// <summary>
		/// Return the trade price.
		/// </summary>
		/// <returns>The monetary value of an individual share of the
		/// security at the time of the trade.</returns>
		MamaPrice getTradePrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradePriceFieldState();

        /// <summary>
        /// Return the trade id.
        /// </summary>
        string getTradeId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeIdFieldState();

		/// <summary>
		/// Return the trade volume.
		/// </summary>
		/// <returns>The number of shares traded in a single transaction for
		/// an individual security.</returns>
		long getTradeVolume();

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
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeVolumeFieldState();

		/// <summary>
		/// Return the participant identifier.
		/// </summary>
		/// <returns>Trade participant ID.  This is typically an exchange ID
		/// or a market maker ID.  In the future, we will make this a
		/// configurable enumeration.</returns>
		string getTradePartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradePartIdFieldState();

		/// <summary>
		/// Return the normalized trade qualifier.
		/// </summary>
		/// <returns>Trade qualifier.  A normalized set of qualifiers for
		/// the current trade for the security.</returns>
		/// <remarks>
		/// This field may contain multiple string values, separated by
		/// the colon(:) character. In the future, we will make this
		/// a configurable enumeration.
		///
		/// <table width="100%" border="1">
		///   <tr><td><b>Value</b></td><td><b>Meaning</b></td></tr>
		///   <tr>
		///     <td>Normal</td>
		///     <td>Regular trade. A trade made without stated conditions
		///     is deemed regular way for settlement on the third *
		///     business day following the transaction * date.</td>
		///   </tr>
		///   <tr>
		///     <td>Acquisition</td>
		///     <td>A transaction made on the Exchange as a result of an
		///     Exchange acquisition.</td>
		///   </tr>
		///   <tr>
		///     <td>Bunched</td>
		///     <td>A trade representing an aggregate of two or more
		///     regular trades in a security occurring at the same price
		///     either simultaneously or within the same 60 second period,
		///     with no individual trade exceeding 10,000 shares.</td>
		///   </tr>
		///   <tr>
		///     <td>Cash</td>
		///     <td>A transaction which calls for the delivery of
		///     securities and payment on the same day the trade takes
		///     place.</td>
		///   </tr>
		///   <tr>
		///     <td>Distribution</td>
		///     <td>Sale of a large block of stock in such a manner that
		///     the price is not adversely affected.</td>
		///   </tr>
		///   <tr>
		///     <td>BunchedSold</td>
		///     <td>A bunched trade which is reported late</td>
		///   </tr>
		///   <tr>
		///     <td>Rule155</td>
		///     <td>To qualify as a 155 print, a specialist arranges for
		///     the sale of the block at one &quot;clean-up&quot; price or
		///     at the different price limits on his book. If the block is
		///     sold at a "clean-up" price, the specialist should execute
		///     at the same price all the executable buy orders on his
		///     book. The sale qualifier is only applicable for AMEX
		///     trades.</td>
		///   </tr>
		///   <tr>
		///     <td>SoldLast</td>
		///     <td>Sold Last is used when a trade prints in sequence but
		///     is reported late or printed in conformance to the One or
		///     Two Point Rule.</td>
		///   </tr>
		///   <tr>
		///     <td>NextDay</td>
		///     <td>A transaction which calls for delivery of securities on
		///     the first business day after the trade date.</td>
		///   </tr>
		///   <tr>
		///     <td>Opened</td>
		///     <td>Indicates an opening transaction that is printed out of
		///     sequence or reported late or printed in conformance to the
		///     One or Two Point Rule.</td>
		///   </tr>
		///   <tr>
		///     <td>PriorRef</td>
		///     <td>An executed trade that relates to an obligation to
		///     trade at an earlier point in the trading day or that refer
		///     to a prior reference price. This may be the result of an
		///     order that was lost or misplaced or a SelectNet order that
		///     was not executed on a timely basis.</td>
		///   </tr>
		///   <tr>
		///     <td>Seller</td>
		///     <td>A Seller's option transaction is a special transaction
		///     which gives the seller the right to deliver the stock at
		///     any time within a specific period, ranging from not less
		///     than four calendar days to no more than 60 calendar
		///     days.</td>
		///   </tr>
		///   <tr>
		///     <td>SplitTrade</td>
		///     <td>An execution in two markets when the specialist or
		///     Market Maker in the market first receiving the order agrees
		///     to execute a portion of it at whatever price is realized in
		///     another market to which the balance of the order is
		///     forwarded for execution.</td>
		///   </tr>
		///   <tr>
		///     <td>FormT</td>
		///     <td>See PrePostMkt.  Currently, all feed handlers that post
		///     Form-T trades - except CTA - send this qualifier for Form-T
		///     trades.  In the next major release, all fields will use
		///     PrePostMkt and FormT will be obsolete.</td>
		///   </tr>
		///   <tr>
		///     <td>PrePostMkt</td>
		///     <td>A trade reported before or after the normal trade
		///     reporting day. This is also known as a Form-T trade. The
		///     volume of Form-T trades will be included in the calculation
		///     of total volume. The price information in Form-T trades
		///     will not be used to update high, low and last sale data for
		///     individual securities or Indices since they occur outside
		///     of normal trade reporting hours.  Currently, all feed
		///     handlers that post Form-T trades - except CTA - send the
		///     "FormT" qualifier for Fot-T trades.  In the next major
		///     release, all feed handlers will use PrePostMkt and FormT
		///     will be obsolete.</td>
		///   </tr>
		///   <tr>
		///     <td>AvPrice</td>
		///     <td>A trade where the price reported is based upon an
		///     average of the prices for transactions in a security during
		///     all or any portion of the trading day.</td>
		///   </tr>
		///   <tr>
		///     <td>Sold</td>
		///     <td>Sold is used when a trade is printed (reported) out of
		///     sequence and at a time different from the actual
		///     transaction time.</td>
		///   </tr>
		///   <tr>
		///     <td>Adjusted</tr>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>Auto</td>
		///     <td>A sale condition code that identifies a NYSE trade that
		///     has been automatically executed without the potential
		///     benefit of price improvement. </td>
		///   </tr>
		///   <tr>
		///     <td>Basket</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>CashOnly</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>NextDayOnly</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>SpecTerms</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>Stopped</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>CATS</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>VCT</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>Rule127</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>BurstBasket</td>
		///     <td>A burst basket execution signifies a trade wherein the
		///     equity Specialists, acting in the aggregate as a market
		///     maker, purchase or sell the component stocks required for
		///     execution of a specific basket trade.</td>
		///   </tr>
		///   <tr>
		///     <td>OpenDetail</td>
		///     <td>Opening trade detail message.  Sent by CTS only and is
		///     a duplicate report of an earlier trade.  Note: since feed
		///     handler version 2.14.32, it is configurable whether these
		///     detail messages are published.</td>
		///   </tr>
		///   <tr>
		///     <td>Detail</td>
		///     <td>Trade detail message.  Sent by CTS only and is a
		///     duplicate report of an earlier trade.  Note: since feed
		///     handler version 2.14.32, it is configurable whether these
		///     detail messages are published.</td>
		///   </tr>
		///   <tr>
		///     <td>Reserved</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>BasketCross</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>BasketIndexOnClose</td>
		///     <td>A basket index on close transaction signifies a trade involving
		///     paired basket orders,the execution of which is based on the closing
		///     value of the index. These trades are reported after the close when
		///     the index closing value is determined.</td>
		///   </tr>
		///   <tr>
		///     <td>IntermarketSweep</td>
		///     <td>Indicates to CTS data recipients that the execution price
		///     reflects the order instruction not to send the order to another
		///     market that may have a superior price.</td>
		///   </tr>
		///   <tr>
		///     <td>YellowFlag</td>
		///     <td>Regular trades reported during specific events as out of the
		///     ordinary.</td>
		///   </tr>
		///   <tr>
		///     <td>MarketCenterOpen</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>MarketCenterClose</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///   <tr>
		///     <td>Unknown</td>
		///     <td>&nbsp;</td>
		///   </tr>
		///  </table>
		/// <remarks>
		string getTradeQual();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeQualFieldState();

		/// <summary>
		/// The native, non normalized, trade qualifier. <see cref="getTradeQual"/>
		/// </summary>
		/// <returns></returns>
		string getTradeQualNativeStr();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeQualNativeStrFieldState();

		/// <summary>
		/// Return the exchange provided trade condition.
		/// </summary>
		/// <returns>Trade condition (a.k.a. "sale condition").
		/// Feed-specific trade qualifier code(s).
		/// <see cref="getTradeQual"/></returns>
		string getTradeCondition();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeConditionFieldState();

		/// <summary>
		/// Return the seller trade days.
		/// </summary>
		/// <returns>Seller's sale days. Used when the trade qualifier is
		/// "Seller".  Specifies the number of days that may elapse before
		/// delivery of the security.</returns>
		long getTradeSellersSaleDays();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeSellersSaleDaysFieldState();

		/// <summary>
		/// Return the stopped stock indicator.
		/// </summary>
		/// <returns>Stopped stock indicator.  Condition related to certain
		/// NYSE trading rules.  This is not related to a halted security
		/// status.  (0 == N/A; 1 == Applicable)</returns>
		char getTradeStopStock();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getTradeStopStockFieldState();

		/// <summary>
		/// Return whether this is an irregular trade.
		/// </summary>
		/// <returns>Whether or not the trade qualifies as an irregular
		/// trade.  In general, only "regular" trades qualify to update the
		/// official last price and high/low prices.
		/// </returns>
		bool getIsIrregular();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getIsIrregularFieldState();

		/// <summary>
		/// Get the ShortSaleCircuitBreaker
		/// </summary>
		/// <returns>Blank: Short Sale Restriction Not in Effect.
		/// A: Short Sale Restriction Activiated.
		/// C: Short Sale Restriction Continued.
		/// D: Sale Restriction Deactivated.
		/// E: Sale Restriction in Effect.</returns>
        char getShortSaleCircuitBreaker();

		/// <summary>
		/// Get the field state
		/// </summary>
		/// <returns>Field State</returns>
        MamdaFieldState getShortSaleCircuitBreakerFieldState();
	}
}
