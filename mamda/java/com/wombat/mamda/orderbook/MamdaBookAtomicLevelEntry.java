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
 * MamdaBookAtomicLevel is an interface that provides access to 
 * Price Level and Price Level Entry fields
 */

public interface MamdaBookAtomicLevelEntry extends MamdaBasicRecap
{

    /**
     * @see MamdaBookAtomicLevel#getPriceLevelPrice()
     */
    double getPriceLevelPrice();
    
    /**
     * @see MamdaBookAtomicLevel#getPriceLevelMamaPrice()
     */
    MamaPrice getPriceLevelMamaPrice();

    /**
     * @see MamdaBookAtomicLevel#getPriceLevelSize()
     */
    double getPriceLevelSize();
    
    /**
     * @see MamdaBookAtomicLevel#getPriceLevelAction()
     */
    char getPriceLevelAction();

    /**
     * @see MamdaBookAtomicLevel#getPriceLevelSide()
     */
    char getPriceLevelSide();

    /**
     * @see MamdaBookAtomicLevel#getPriceLevelTime()
     */
    MamaDateTime getPriceLevelTime();

    /**
     * @see MamdaBookAtomicLevel#getPriceLevelNumEntries()
     */
    double getPriceLevelNumEntries();

    /**
     *
     */
    long getPriceLevelActNumEntries();

    /**
     * Order book entry action to apply to the full order book.
     *
     *
     * A : Add entry to the price level.
     * U : Update existing entry in the price level.
     * D : Delete existing entry from  the price level.
     *
     * @return The orderbook entry action.
     */
    char getPriceLevelEntryAction();

    /**
     * Reason for order book entry.
     *
     * @return The orderbook entry reason.
     */
    char getPriceLevelEntryReason(); 

    /**
     * Order book entry Id. (order ID, participant ID, etc.)
     *
     * @return The entry id
     */
    String getPriceLevelEntryId();

    /**
     * Return the order book entry size
     *
     * @return Order book entry size
     */
    long getPriceLevelEntrySize();

    /**
     * Return the time of order book entry update.
     *
     * @return Time of order book entry update.
     */
    MamaDateTime getPriceLevelEntryTime();
    
    public Iterator entryIterator ();
    
    public boolean isVisible ();
 
}
