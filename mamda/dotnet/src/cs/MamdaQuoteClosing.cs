/* $Id: MamdaQuoteClosing.cs,v 1.3.30.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
	/// MamdaQuoteClosing is an interface that provides access to quote
	/// closing related fields.
	/// </summary>
	public interface MamdaQuoteClosing : MamdaBasicEvent
	{
		/// <summary>
		/// Get the closing bid price.
		/// </summary>
		/// <returns>Today's closing bid price, after the market has
		/// closed and the stock has traded today.</returns>
		MamaPrice getBidPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBidPriceFieldState();

		/// <summary>
		/// Get the closing bid size.
		/// </summary>
		/// <returns>Today's closing bid size, after the market has
		/// closed and the stock has traded today.</returns>
		long getBidSize();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBidSizeFieldState();

		/// <summary>
		/// Get the closing bid participant identifier.
		/// </summary>
		/// <returns>Today's closing bid participant identifier, after
		/// the market has closed and the stock has traded today.</returns>
		string getBidPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBidPartIdFieldState();

		/// <summary>
		/// Get the closing ask price.
		/// </summary>
		/// <returns>Today's closing ask price, after the market has
		/// closed and the stock has traded today.</returns>
		MamaPrice getAskPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAskPriceFieldState();

		/// <summary>
		/// Get the closing ask size.
		/// </summary>
		/// <returns>Today's closing ask size, after the market has
		/// closed and the stock has traded today.</returns>
		long getAskSize();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAskSizeFieldState();

		/// <summary>
		/// Get the closing ask participant identifier.
		/// </summary>
		/// <returns>Today's closing ask participant identifier, after
		/// the market has closed and the stock has traded today.</returns>
		string getAskPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getAskPartIdFieldState();

	}
}

