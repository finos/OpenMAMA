/* $Id: MamdaBookAtomicLevel.cs,v 1.5.30.5 2012/09/07 07:44:57 ianbell Exp $
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
    /**
     * MamdaBookAtomicLevel is an interface that provides access to trade
     * related fields.
     */
    public interface MamdaBookAtomicLevel : MamdaBasicRecap
    {

		/// <summary>Return the number of price levels in the order book update.</summary>
		/// <returns>The number of price levels.</returns>		
		long getPriceLevelNumLevels();

		/// <summary>Return at which position this level is within an update containing a
		/// number of levels. (i.e. level m of n levels in the update)</summary>
		///<returns>The position of this level in the update received.</returns>
		long getPriceLevelNum();

		/// <summary>Return the price for this price level.</summary>
		/// <returns>The price level price.</returns>	
		double getPriceLevelPrice();

        /// <summary>Return the Mamaprice for this price level.</summary>
		/// <returns>The price level Mama price.</returns>	
		MamaPrice getPriceLevelMamaPrice();

		/// <summary>Return the number of order entries comprising this price level.</summary>
		/// <returns>The number of entries in this price level.</returns>	
		double getPriceLevelSize();

		/// <summary>Aggregate size at current price level.</summary>
		/// <returns>The aggregate size at the current price level.</returns>	
		long getPriceLevelSizeChange ();

		/// <summary>The action to apply to the orderbook for this price level.
		/// Can have a value of:
		/// A : Add a new price level
		/// U : Update an existing price level
		/// D : Delete an existing price level
		/// C : Closing information for price level (often treat the same as Update).
		/// </summary>
		/// <returns>The price level action.</returns>	
		char getPriceLevelAction();

		/// <summary>Side of book at current price level.
		/// B : Bid side. Same as 'buy' side.
		/// A : Ask side. Same as 'sell' side.
		/// </summary>
		/// <returns>The price level side.</returns>	
		char getPriceLevelSide();

		/// <summary>Time of order book price level.</summary>
		/// <returns>The time of the orderbook price level.</returns>	
		DateTime getPriceLevelTime();

		/// <summary>Number of order book entries at current price level.</summary>
		/// <returns>The number of entries at the current price level.</returns>	
		double getPriceLevelNumEntries();
    }
}
