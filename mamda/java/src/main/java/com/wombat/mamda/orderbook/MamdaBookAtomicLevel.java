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

package com.wombat.mamda.orderbook;

import java.util.Iterator;
import com.wombat.mamda.MamdaBasicRecap;
import com.wombat.mama.*;

/**
 * MamdaBookAtomicLevel is an interface that provides access to trade
 * related fields.
 */

public interface MamdaBookAtomicLevel extends MamdaBasicRecap
{
    
    /**
     * Return the number of price levels in the order book update.
     *
     * @return The number of price levels.
     */
    long  getPriceLevelNumLevels();

    /**
     * Return at which position this level is within an update containing a
     * number of levels. (i.e. level m of n levels in the update)
     *
     * @return The position of this level in the update received.
     */
    long  getPriceLevelNum();

    /**
     * Return the price for this price level.
     *
     * @return The price level price.
     */
    double  getPriceLevelPrice();

    /**
     * Return the MamaPrice for this price level.
     *
     * @return The price level price.
     */
    MamaPrice  getPriceLevelMamaPrice();

    /**
     * Return the number of order entries comprising this price level.
     * <br>
     * Not supported for V5 entry book updates.
     *
     * @return The number of entries in this price level.
     */
    double getPriceLevelSize();

    /**
     * Aggregate size at current price level.
     * <br>
     * Not supported for V5 entry book updates.
     *
     * @return The aggregate size at the current price level.
     */
    long getPriceLevelSizeChange ();


    /**
     * The action to apply to the orderbook for this price level. Can have a
     * value of:
     * <ul>
     *   <li>A : Add a new price level</li>
     *   <li>U : Update an existing price level</li>
     *   <li>D : Delete an existing price level</li>
     *   <li>C : Closing information for price level (often treat the same as
     *   Update).</li>
     * </ul>
     * 
     * Not supported for V5 entry book updates.
     *
     * @return The price level action.
     */
    char getPriceLevelAction();

    /**
     * Side of book at current price level.
     *
     * <ul>
     *   <li>B : Bid side. Same as 'buy' side.</li>
     *   <li>A : Ask side. Same as 'sell' side.</li>
     * </ul>
     *
     * @return The price level side.
     */
    char getPriceLevelSide();

    /**
     * Time of order book price level.
     *
     * @return The time of the orderbook price level.
     */
    MamaDateTime  getPriceLevelTime();

    /**
     * Number of order book entries at current price level.
     * <br>
     * Not supported for V5 entry book updates.
     *
     * @return The number of entries at the current price level.
     */
    double getPriceLevelNumEntries();

    public Iterator entryIterator ();

}
