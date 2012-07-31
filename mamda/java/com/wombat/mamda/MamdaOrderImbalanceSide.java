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
import java.util.*;
import java.util.logging.*;
import java.io.*;

/***************************************************************
 *class Imbalance Side
 ***************************************************************/
public final class MamdaOrderImbalanceSide
{
    /* The imbalance side could be either the buy or sell side
       i.e. bid/ask. 0 and 1 representation will be used for
       bid/ask respectively.
    */
    private final int    mValue ;
    private final String mDescription ;

    /** BID_SIDE_IMBALANCE */
    public final static int BID_SIDE_VALUE = 0;
    public final static MamdaOrderImbalanceSide BID_SIDE = new 
    MamdaOrderImbalanceSide (valueToString (BID_SIDE_VALUE), BID_SIDE_VALUE);
    
    /** ASK_SIDE_IMBALANCE */
    public final static int ASK_SIDE_VALUE = 1; 
    public final static MamdaOrderImbalanceSide ASK_SIDE = new
    MamdaOrderImbalanceSide (valueToString (ASK_SIDE_VALUE), ASK_SIDE_VALUE);

    /** NO_IMBALANCE */
    public final static int NO_IMBALANCE_VALUE = 2;
    public final static MamdaOrderImbalanceSide NO_IMBALANCE_SIDE = new
    MamdaOrderImbalanceSide (valueToString (NO_IMBALANCE_VALUE), NO_IMBALANCE_VALUE);

    /**
     * Returns the stringified name for the enumerated type.
     *
     * @return Name for the type.
     */
    public String toString ()
    {
        return mDescription;
    }

    /**
     * Returns the integer value for the type.
     * This value can be used in switch statements against the public
     * XXX_VALUE static members of the class.
     *
     * @return The integer type.
     */
    public int getValue ()
    {
        return mValue;
    }

    /**
     * Compare the two types for equality.
     * Returns true if the integer value of both types is equal. Otherwise
     * returns false.
     *
     * @param  imbalanceSide The object to check equality against.
     * @return Whether the two objects are equal.
     */
    public boolean equals (MamdaOrderImbalanceSide imbalanceSide)
    {
        if (mValue == imbalanceSide.mValue) 
            return true;
        return false;
    }

    private MamdaOrderImbalanceSide (String desc, int value)
    {
        mDescription = desc;
        mValue       = value;
    }

    /**
     * Utility method for mapping type integer values to corresponding string
     * values.
     *
     * Returns "UNKNOWN" is the int type value is not recognised.
     *
     * @param value The int value for a MamdaOrderImbalanceSide
     * @return The string name value of the specified MamdaOrderImbalanceSide
     * integer
     * value.
     */
    public static String valueToString (int value)
    {
        switch (value)
        {
            case ASK_SIDE_VALUE:        return "ASK_SIDE";
            case BID_SIDE_VALUE:        return "BID_SIDE";
            case NO_IMBALANCE_VALUE:    return "NO_IMBALANCE";
            default:                    return "UNKNOWN";
        }
    }

    /**
     * Return an instance of a MamdaOrderImbalanceSide  corresponding to 
     * the specified integer value.
     * Returns null if the integer value is not recognised.
     *
     * @param value Int value for a MamdaOrderImbalanceSide
     * @return Instance of a MamdaOrderImbalanceSide if a mapping exists.
     */
    public static MamdaOrderImbalanceSide enumObjectForValue (int value)
    {
        switch (value)
        {
            case ASK_SIDE_VALUE:        return ASK_SIDE;
            case BID_SIDE_VALUE:        return BID_SIDE;
            case NO_IMBALANCE_VALUE:    return NO_IMBALANCE_SIDE;
            default:                    return null;
        }
    }
}
