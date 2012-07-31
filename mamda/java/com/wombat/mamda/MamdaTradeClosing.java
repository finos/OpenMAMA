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

package com.wombat.mamda;
import com.wombat.mama.*;

/**
 * MamdaTradeClosing is an interface that provides access to fields
 * related to trade closing reports.  There is no need to provide
 * access to total volume, open/high/low, etc., since these fields are
 * also available in the recap interface passed along with closing
 * messages.
 */

public interface MamdaTradeClosing extends MamdaBasicEvent
{
    /**
     * Return the Close price Today's closing price. The close price is
     * populated when official closing prices are sent by the feed
     * after the session close.
     */
    public MamaPrice  getClosePrice();

    /**
     * @return Returns the ClosePrice Field State.
     */
    public short getClosePriceFieldState();

    /**
     * Return whether this closing price is indicative or official.
     */
    public boolean getIsIndicative();

    /**
     * @return Returns the IsIndicative Field State.
     */
    public short getIsIndicativeFieldState();
}
