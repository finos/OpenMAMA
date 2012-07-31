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

/**
 * A class representing trade Side.
 */

public class MamdaTradeSide
{
    /**
     * The Values of TradeSide:
     * The value 0 maps to Unknown
     * The value 1 maps to Buy
     * The value 2 maps to Sell
     */
 
    public static final short   TRADE_SIDE_UNKNOWN        = 0;
    public static final short   TRADE_SIDE_BUY            = 1;
    public static final short   TRADE_SIDE_SELL           = 2;


    private static final String TRADE_SIDE_UNKNOWN_STR   = "Unknown";
    private static final String TRADE_SIDE_BUY_STR       = "Buy";
    private static final String TRADE_SIDE_SELL_STR      = "Sell";
  

    /**
     * Convert a MamdaTradeSide to an appropriate, displayable
     * string.
     *
     * @param tradeSide The tradeSide as a short.
     */

    public static String toString (short tradeSide)
    {
        switch (tradeSide)
        {
            case  TRADE_SIDE_UNKNOWN:  return TRADE_SIDE_UNKNOWN_STR;
            case  TRADE_SIDE_BUY:      return TRADE_SIDE_BUY_STR;
            case  TRADE_SIDE_SELL:     return TRADE_SIDE_SELL_STR;

            default:
                return "Unknown";
        }
    }

    public static short mamdaTradeSideFromString (String tradeSide)
    {
        // Older FH configurations sent strings:
        if (tradeSide.equals(TRADE_SIDE_BUY_STR))
            return TRADE_SIDE_BUY;
        if (tradeSide.equals(TRADE_SIDE_SELL_STR))
            return TRADE_SIDE_SELL;    
        if (tradeSide.equals(TRADE_SIDE_UNKNOWN_STR))
            return TRADE_SIDE_UNKNOWN;    
                
        // A misconfigured FH might send numbers as strings:
        if (tradeSide.equals("0"))
            return TRADE_SIDE_UNKNOWN;
        if (tradeSide.equals("1"))
            return TRADE_SIDE_BUY;
        if (tradeSide.equals("2"))
            return TRADE_SIDE_SELL;  
        return TRADE_SIDE_UNKNOWN;
    }

    public void setState(short side)
    {
        mTradeSide = side;
    }

    public short getState()
    {
        return mTradeSide;
    }

    private short mTradeSide = 0;

}
