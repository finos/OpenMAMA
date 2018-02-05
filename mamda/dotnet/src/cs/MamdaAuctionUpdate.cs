/* $Id: MamdaAuctionUpdate.cs,v 1.1.4.5 2012/09/07 07:44:57 ianbell Exp $
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
	/// MamdaAuctionUpdate is an interface that provides access to fields
	/// related to quote updates.
	/// </summary>
	public interface MamdaAuctionUpdate : MamdaBasicEvent
	{
		/// <summary>
		/// Get the uncross price.
		/// </summary>
		/// <returns>Indicative or firm auction price. </returns>
		MamaPrice getUncrossPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getUncrossPriceFieldState();

		/// <summary>
		/// Get the uncross Volume.
		/// </summary>
		/// <returns>Indicative Volume or the volume turned over in the auction</returns>
		double getUncrossVolume();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getUncrossVolumeFieldState();

		/// <summary>
		/// Get the uncross price indicator.
		/// </summary>
		/// <returns>The uncross price ind - indicates whether the price and volume
        /// is indicative of the current state of the auction or whether its the firm
        /// auction price and volume </returns>
		long getUncrossPriceInd();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getUncrossPriceIndFieldState();
	}
}
