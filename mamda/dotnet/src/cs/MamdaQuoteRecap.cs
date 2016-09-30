/* $Id: MamdaQuoteRecap.cs,v 1.6.2.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
	/// MamdaQuoteRecap is an interface that provides access to quote
	/// related fields.
	/// </summary>
	public interface MamdaQuoteRecap : MamdaBasicRecap
	{
		/// <summary>
		/// Get the bid price.
		/// </summary>
		/// <returns>Bid price.  The highest price that the representative
		/// party/group is willing to pay to buy the security.
		/// For most feeds, this size is represented in round lots.</returns>
		MamaPrice getBidPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBidPriceFieldState();

		/// <summary>
		/// Get the bid size.
		/// </summary>
		/// <returns>Total share size available for the current bid price.</returns>
		long getBidSize();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBidSizeFieldState();

		/// <summary>
		/// Get the bid participant identifier.
		/// </summary>
		/// <returns>The identifier of the market participant (e.g.
		/// exchange or market maker) contributing the bid price field.</returns>
		string getBidPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBidPartIdFieldState();

		/// <summary>
		/// Get the ask price.
		/// </summary>
		/// <returns>Ask price.  The lowest price that the representative
		/// party/group is willing to take to sell the security.  For
		/// most feeds, this size is represented in round lots.</returns>
		MamaPrice getAskPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAskPriceFieldState();

		/// <summary>
		/// Get the ask size.
		/// </summary>
		/// <returns>Total share size available for the current ask price.</returns>
		/// <remarks>
		/// Note: many feeds provide this size in terms of round lots.
		/// Wombat may change the behavior of this in the future to
		/// represent the exact number of shares.
		/// </remarks>
		long getAskSize();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAskSizeFieldState();

		/// <summary>
		/// Get the ask participant identifier.
		/// </summary>
		/// <returns>The identifier of the market participant (e.g.
		/// exchange or market maker) contributing the ask price field.</returns>
		string getAskPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAskPartIdFieldState();

		/// <summary>
		/// Get the quote mid price.
		/// </summary>
		/// <returns>The mid price of the current quote.  Usually, this is
		/// the average of the bid and ask prices, but some exchanges
		/// provide this field explicitly.</returns>
		MamaPrice getQuoteMidPrice();

		/// <summary>
		/// Get the quote count.
		/// </summary>
		/// <returns>The number of quotes generated for this security during
		/// the current trading session.</returns>
		long getQuoteCount();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getQuoteCountFieldState();

		/// <summary>
		/// Get the normalized quote qualifier.
		/// </summary>
		/// <returns>A normalized set of qualifiers for the last quote for
		/// the security. This field may contain multiple string values,
		/// separated by the colon(:) character.
		/// <see cref="MamdaQuoteUpdate.getQuoteQual"/>
		/// </returns>
		string getQuoteQual();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getQuoteQualFieldState();

		/// <summary>
		/// <see cref="MamdaQuoteUpdate.getQuoteQualNative()"/>
		/// </summary>
		/// <returns></returns>
		string getQuoteQualNative ();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getQuoteQualNativeFieldState();

        /// <summary>
        /// <see cref="MamdaQuoteUpdate.getShortSaleCircuitBreaker()"/>
        /// </summary>
        /// <returns></returns>
        char getShortSaleCircuitBreaker ();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getShortSaleCircuitBreakerFieldState();

		/// <summary>
		/// <see cref="MamdaQuoteUpdate.getShortSaleBidTick()"/>
		/// </summary>
		/// <returns></returns>
		char getShortSaleBidTick ();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getShortSaleBidTickFieldState();

        DateTime    getAskTime();
        DateTime    getBidTime();
        DateTime    getQuoteDate();
        string      getAskIndicator();
        string      getBidIndicator();
        long        getAskUpdateCount();
        long        getBidUpdateCount();
        double      getAskYield();
        double      getBidYield();

        MamdaFieldState    getAskTimeFieldState();
        MamdaFieldState    getBidTimeFieldState();
        MamdaFieldState    getQuoteDateFieldState();
        MamdaFieldState    getAskIndicatorFieldState();
        MamdaFieldState    getBidIndicatorFieldState();
        MamdaFieldState    getAskUpdateCountFieldState();
        MamdaFieldState    getBidUpdateCountFieldState();
        MamdaFieldState    getAskYieldFieldState();
        MamdaFieldState    getBidYieldFieldState();
	}
}
