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
import com.wombat.mama.MamaPrice;

/**
 * MamdaQuoteClosing is an interface that provides access to quote
 * closing related fields.
 */

public interface MamdaQuoteClosing extends MamdaBasicEvent
{
    /**
     * Get the closing bid price.
     * @return Today's closing bid price, after the market has
     * closed and the stock has traded today.
     */
    MamaPrice  getBidPrice();

    /**
     * @return the bid price Field State
     */
    short  getBidPriceFieldState();
    
    /**
     * Get the closing bid size.
     * @return Today's closing bid size, after the market has
     * closed and the stock has traded today.
     */
    double  getBidSize();

    /**
     * @return the bid size Field State
     */
    short  getBidSizeFieldState();
    
    /**
     * Get the closing bid participant identifier.
     * @return Today's closing bid participant identifier, after
     * the market has closed and the stock has traded today.
     */
    String  getBidPartId();

    /**
     * @return the bid part ID Field State
     */
    short  getBidPartIdFieldState();
    
    /**
     * Get the closing ask price.
     * @return Today's closing ask price, after the market has
     * closed and the stock has traded today.
     */
    MamaPrice  getAskPrice();

    /**
     * @return the ask price Field State
     */
    short  getAskPriceFieldState();
    
    /**
     * Get the closing ask size.
     * @return Today's closing ask size, after the market has
     * closed and the stock has traded today.
     */
    double    getAskSize();

    /**
     * @return the ask size Field State
     */
    short  getAskSizeFieldState();
    
    /**
     * Get the closing ask participant identifier.
     * @return Today's closing ask participant identifier, after
     * the market has closed and the stock has traded today.
     */
    String  getAskPartId();

    /**
     * @return the ask part ID Field State
     */
    short  getAskPartIdFieldState();
        
}
