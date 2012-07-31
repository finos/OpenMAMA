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
 * An enumeration representing field state.
 * MODIFIED (0) value indicates the field was updated in last tick
 * NOT MODIFIED (1) value indicate that there was no change in the last tick
 * NOT_INITIALISED (2) value indicates that the field has never been updated
 * 
 */

public class MamdaFieldState
{
    public static final short   NOT_INITIALISED           = 0;
    public static final short   NOT_MODIFIED              = 1;
    public static final short   MODIFIED                  = 2;

    private static final String NOT_INITIALISED_STR   = "Not Initialised";
    private static final String NOT_MODIFIED_STR      = "Not Modified";
    private static final String MODIFIED_STR          = "Modified";


    /**
     * Convert a MamdaFieldState to an appropriate, displayable
     * string.
     *
     * @param fieldState The Field State as a short.
     */

    public static String toString (short fieldState)
    {
        switch (fieldState)
        {
            case  NOT_INITIALISED:   return NOT_INITIALISED_STR;
            case  NOT_MODIFIED:      return NOT_MODIFIED_STR;
            case  MODIFIED:          return MODIFIED_STR;

            default:
                return "Not Initialised";
        }
    }

    public void setState(short state)
    {
        mState = state;
    }

    public short getState ()
    {
        return mState;
    }

    private short mState = 0;
}

