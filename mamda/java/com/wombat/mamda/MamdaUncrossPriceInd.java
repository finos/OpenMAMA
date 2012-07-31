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

public class MamdaUncrossPriceInd
{
    public static final short   UNCROSS_NONE              = 0;
    public static final short   UNCROSS_INDICATIVE        = 1;
    public static final short   UNCROSS_FIRM              = 2;
    public static final short   UNCROSS_INSUFFICIENT_VOL  = 3;

    private static final String UNCROSS_NONE_STR              = "None";
    private static final String UNCROSS_INDICATIVE_STR        = "I";
    private static final String UNCROSS_FIRM_STR              = "F";
    private static final String UNCROSS_INSUFFICIENT_VOL_STR  = "V";


    /**
     * Convert a MamdaUncrossPriceInd to an appropriate, displayable
     * string.
     *
     * @param ind The price Ind as a short.
     */

    public static String toString (short ind)
    {
        switch (ind)
        {
            case  UNCROSS_NONE:             return UNCROSS_NONE_STR;
            case  UNCROSS_INDICATIVE:       return UNCROSS_INDICATIVE_STR;
            case  UNCROSS_FIRM:             return UNCROSS_FIRM_STR;
            case  UNCROSS_INSUFFICIENT_VOL: return UNCROSS_INSUFFICIENT_VOL_STR;
            default:
            return "None";
        }
    }

    public void set(short ind)
    {
        mInd = ind;
    }

    public short get ()
    {
        return mInd;
    }

    private short mInd = 0;
    
    
    public static short mamdaUncrossPriceIndFromString(String uncrossPriceInd)
    {
        if (uncrossPriceInd == null)
        {
            return UNCROSS_NONE;
        }

        if (uncrossPriceInd.equals (UNCROSS_INDICATIVE_STR))
            return UNCROSS_INDICATIVE;
        if (uncrossPriceInd.equals (UNCROSS_FIRM_STR))
            return UNCROSS_FIRM;
        if (uncrossPriceInd.equals (UNCROSS_INSUFFICIENT_VOL_STR))
            return UNCROSS_INSUFFICIENT_VOL;
        
        return UNCROSS_NONE;
    }
}

