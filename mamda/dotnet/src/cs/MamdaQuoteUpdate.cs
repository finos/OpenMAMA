/* $Id: MamdaQuoteUpdate.cs,v 1.6.2.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
	/// MamdaQuoteUpdate is an interface that provides access to fields
	/// related to quote updates.
	/// </summary>
	public interface MamdaQuoteUpdate : MamdaBasicEvent
	{
		/// <summary>
		/// </summary>
		/// <returns>Bid price.  The highest price that the representative
		/// party/group is willing to pay to buy the security.  For most
		/// feeds, this size is represented in round lots.</returns>
		MamaPrice getBidPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBidPriceFieldState();

		/// <summary>
		/// </summary>
		/// <returns>Total share size available for the current
		/// bid price. For most feeds, this size is represented in round
		/// lots.</returns>
		long getBidSize();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBidSizeFieldState();

		/// <summary>
		/// </summary>
		/// <returns>The identifier of the market participant (e.g. exchange
		/// or market maker) contributing the bid price field.</returns>
		string getBidPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBidPartIdFieldState();

		/// <summary>
		/// </summary>
		/// <returns>Ask price.  The lowest price that the representative
		/// party/group is willing to take to sell the security.  For most
		/// feeds, this size is represented in round lots.</returns>
		MamaPrice getAskPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAskPriceFieldState();

		/// <summary>
		/// </summary>
		/// <returns>Total share size available for the current
		/// ask price. For most feeds, this size is represented in round
		/// lots.</returns>
		long getAskSize();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAskSizeFieldState();

		/// <summary>
		/// </summary>
		/// <returns>The identifier of the market participant (e.g. exchange
		/// or market maker) contributing the ask price field.</returns>
		string getAskPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAskPartIdFieldState();

		/// <summary>
		/// A normalized set of qualifiers for the last quote for
		/// the security. This field may contain multiple string values,
		/// separated by the colon(:) character.
		/// </summary>
		/// <returns>
		/// A normalized set of qualifiers for the last quote for
		/// the security. This field may contain multiple string values,
		/// separated by the colon(:) character.  In the future, this will
		/// be made a configurable enumeration.
		/// 
		/// <table width="100%" border="1">
		///   <tr><td><b>Value</b></td><td><b>Meaning</b></td></tr>
		///   <tr>
		///     <td>Normal</td>
		///     <td>Regular quote; no special condition</td>
		///   </tr>
		///   <tr>
		///     <td>DepthAsk</td>
		///     <td>Depth on ask side</td>
		///   </tr>
		///   <tr>
		///     <td>DepthBid</td>
		///     <td>Depth on bid side</td>
		///   </tr>
		///   <tr>
		///     <td>SlowQuoteOnAskSide</td>
		///     <td>This indicates that a market participants Ask is in a slow
		///     (CTA) mode. While in this mode, automated
		///     execution is not eligible on the Ask side and can be
		///     traded through pursuant to Regulation NMS requirements.</td>
		///   </tr>
		///   <tr>
		///     <td>SlowQuoteOnBidSide</td>
		///     <td>This indicates that a market participants Bid is in a slow
		///     (CTA) mode. While in this mode, automated
		///     execution is not eligible on the Bid side and can be
		///     traded through pursuant to Regulation NMS
		///     requirements.</td>
		///   </tr>
		///   <tr>
		///     <td>Fast</td>
		///     <td>Fast trading</td>
		///   </tr>
		///   <tr>
		///     <td>NonFirm</td>
		///     <td>Non-firm quote</td>
		///   </tr>
		///   <tr>
		///     <td>Rotation</td>
		///     <td>OPRA only. Quote relates to a trading rotation (Where a participant rotates through various clients that they are trading for)</td>
		///   </tr>
		///   <tr>
		///     <td>Auto</td>
		///     <td>Automatic trade</td>
		///   </tr>
		///   <tr>
		///     <td>Inactive</td>
		///     <td> </td>
		///   </tr>
		///   <tr>
		///     <td>SpecBid</td>
		///     <td>Specialist bid</td>
		///   </tr>
		///   <tr>
		///     <td>SpecAsk</td>
		///     <td>Specialist ask</td>
		///   </tr>
		///   <tr>
		///     <td>OneSided</td>
		///     <td>One sided.  No orders, or only market orders, exist on one side
		///     of the book.</td>
		///   </tr>
		///   <tr>
		///     <td>PassiveMarketMaker</td>
		///     <td>Market Maker is both underwriter and buyer of security.</td>
		///   </tr>
		///   <tr>
		///     <td>LockedMarket</td>
		///     <td>Locked market - Bid is equal to Ask for OTCBB issues
		///     </td>
		///   </tr>
		///   <tr>
		///     <td>Crossed</td>
		///     <td>Crossed market - Bid is greater than Ask for OTCBB
		///     </td>
		///   </tr>
		///   <tr>
		///     <td>Synd</td>
		///     <td>Syndicate bid</td>
		///   </tr>
		///   <tr>
		///     <td>PreSynd</td>
		///     <td>Pre-syndicate bid</td>
		///   </tr>
		///   <tr>
		///     <td>Penalty</td>
		///     <td>Penalty bid</td>
		///   </tr>
		///   <tr>
		///     <td>UnsolBid</td>
		///     <td>Unsolicited bid</td>
		///   </tr>
		///   <tr>
		///     <td>UnsolAsk</td>
		///     <td>Unsolicited ask</td>
		///   </tr>
		///   <tr>
		///     <td>UnsolQuote</td>
		///     <td>Unsolicited quote</td>
		///   </tr>
		///   <tr>
		///     <td>Empty</td>
		///     <td>Empty quote (no quote)</td>
		///   </tr>
		///   <tr>
		///     <td>XpressBid</td>
		///     <td>NYSE LiquidityQuote Xpress bid indicator</td>
		///   </tr>
		///   <tr>
		///     <td>XpressAsk</td>
		///     <td>NYSE LiquidityQuote Xpress ask indicator</td>
		///   </tr>
		///   <tr>
		///     <td>BestOrder</td>
		///     <td> </td>
		///   </tr>
		///   <tr>
		///     <td>WillSell</td>
		///     <td> </td>
		///   </tr>
		///   <tr>
		///     <td>WillBuy</td>
		///     <td> </td>
		///   </tr>
		///   <tr>
		///     <td>AnyOrder</td>
		///     <td> </td>
		///   </tr>
		///   <tr>
		///     <td>MktOnly</td>
		///     <td>Market orders only.</td>
		///   </tr>
		///   <tr>
		///     <td>ManualAsk</td>
		///     <td>This indicates that a market participants Ask is in a manual
		///     (NASDAQ) mode. While in this mode, automated
		///     execution is not eligible on the Ask side and can be traded through
		///     pursuant to Regulation NMS requirements.</td>
		///   </tr>
		///   <tr>
		///     <td>ManualBid</td>
		///     <td>This indicates that a market participants Bid is in a manual
		///     (NASDAQ) mode. While in this mode, automated
		///     execution is not eligible on the Bid side and can be
		///     traded through pursuant to Regulation NMS requirements.</td>
		///   </tr>
		///   <tr>
		///     <td>AutomaticAsk</td>
		///     <td>This indicates that the marlet participant's Ask is in
		///     automatic mode - we generally send a combination of the last four
		///     (e.g. ManualAsk:AutomaticBid or ManualAsk;ManualBid)</td>
		///   </tr>
		///   <tr>
		///     <td>AutomaticBid</td>
		///     <td>This indicates that the marlet participant's Ask is in
		///     automatic mode - we generally send a combination of the last four
		///     (e.g. ManualAsk:AutomaticBid or ManualAsk;ManualBid)</td>
		///   </tr>
		///   <tr>
		///     <td>Closing</td>
		///     <td>Closing quote.</td>
		///   </tr>
		///   <tr>
		///     <td>Unknown</td>
		///     <td>Quote condition unknown.</td>
		///   </tr>
		///  </table>
		/// </returns>
		string getQuoteQual();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getQuoteQualFieldState();

		/// <summary>
		/// The exchange specific non normalized quote qualifier.
		/// </summary>
		/// <returns>The exchange specific quote qualifier. <see cref="getQuoteQual"/></returns>
		/// 
		string getQuoteQualNative();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getQuoteQualNativeFieldState();

		/// <summary>
		/// NASDAQ Bid Tick Indicator for Short Sale Rule Compliance.
		/// National Bid Tick Indicator based on changes to the bid price of the
		/// National Best Bid or Offer (National BBO).
		/// 
		/// <table width="100%" border="1">
		///   <tr><td><b>Value</b></td><td><b>Meaning</b></td></tr>
		///   <tr>
		///     <td>U</td>
		///     <td>Up Tick. The current Best Bid Price price is higher than the
		///     previous Best Pid Price for the given NNM security.</td>
		///   </tr>
		///   <tr>
		///     <td>D</td>
		///     <td>Down Tick.  The current Best Bid Price price is lower than the
		///     previous Best Pid Price for the given NNM security.</td>
		///   </tr>
		///   <tr>
		///     <td>N</td>
		///     <td>No Tick. The NASD Short Sale Rule does not apply to issue (i.e.
		///     NASDAQ SmallCap listed security).</td>
		///   </tr>
		///   <tr>
		///     <td>Z</td>
		///     <td>Unset - default value within the API</td>
		///   </tr>
		/// </table>
		/// </summary>
		/// <returns>The tick bid indicator.</returns>
		char getShortSaleBidTick();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getShortSaleBidTickFieldState();

		/// <summary>
		/// get the ShortSaleCircuitBreaker
		/// </summary>
		/// <returns>return values: 
		/// Blank: Short Sale Restriction Not in Effect. 
		/// A: Short Sale Restriction Activiated. 
		/// C: Short Sale Restriction Continued. 
		/// D: Sale Restriction Deactivated. 
		/// E: Sale Restriction in Effect.</returns>
        char getShortSaleCircuitBreaker();

        /**
        * return the reason Field State
        * MODIFIED (2) value indicates the fied was updated in last tick
        * NOT MODIFIED (1) value indicate that there was no change in the last tick
        * NOT_INITIALISED (0) value indicates that the field has never been updated
        */

		/// <summary>
		/// return the reason Field State
		/// </summary>
		/// <returns>MODIFIED (2) value indicates the fied was updated in last tick,
		/// NOT MODIFIED (1) value indicate that there was no change in the last tick,
		/// NOT_INITIALISED (0) value indicates that the field has never been updated</returns>
        MamdaFieldState getShortSaleCircuitBreakerFieldState();

        DateTime		getAskTime();
        DateTime		getBidTime();
        string          getAskIndicator();
        string          getBidIndicator();
        long            getAskUpdateCount();
        long            getBidUpdateCount();
        double          getAskYield();
        double          getBidYield();  
        
        MamdaFieldState    getAskTimeFieldState();
        MamdaFieldState    getBidTimeFieldState();
        MamdaFieldState    getAskIndicatorFieldState();
        MamdaFieldState    getBidIndicatorFieldState();
        MamdaFieldState    getAskUpdateCountFieldState();
        MamdaFieldState    getBidUpdateCountFieldState();
        MamdaFieldState    getAskYieldFieldState();
        MamdaFieldState    getBidYieldFieldState();  
	}
}

