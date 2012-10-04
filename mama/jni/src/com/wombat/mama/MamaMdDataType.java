/* $Id:
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

package com.wombat.mama;

public final class MamaMdDataType
{
    private final String myStringValue;
    private final int    myValue;

    public static final int            STANDARD_VALUE = 0;
    public static final MamaMdDataType STANDARD   = new MamaMdDataType
                             (valueToString (STANDARD_VALUE), STANDARD_VALUE);

    public static final int            ORDER_BOOK_VALUE = 1;
    public static final MamaMdDataType ORDER_BOOK = new MamaMdDataType
                             (valueToString (ORDER_BOOK_VALUE), ORDER_BOOK_VALUE);

    public static final int            NEWS_STORY_VALUE = 2;
    public static final MamaMdDataType NEWS_STORY = new MamaMdDataType
                             (valueToString (NEWS_STORY_VALUE), NEWS_STORY_VALUE);
   
    public static final int            WORLDVIEW_VALUE = 3;
    public static final MamaMdDataType WORLDVIEW = new MamaMdDataType
                             (valueToString (WORLDVIEW_VALUE), WORLDVIEW_VALUE);                         
    
    /* No publicly created instances allowed */
    private MamaMdDataType (String name, int value) 
    {
        myStringValue = name;
        myValue       = value;
    }
   
    /**
     * Returns the stringified name for the enumerated type.
     *
     * @return Name for the type.
     */
    public String toString ()
    {
        return myStringValue;
    }

    /**
     * Returns the integer value for the type.
     * This value can be used in switch statements against the
     * public  XXX_VALUE static members of the class.
     *
     * @return The integer type.
     */
    public int getValue ()
    {
        return myValue;
    }

    /**
     * Compare the two types for equality.
     * Returns true if the integer value of both types is
     * equal. Otherwise returns false.
     *
     * @param appDataType The object to check equality against.
     * @return Whether the two objects are equal.
     */
    public boolean equals (MamaMdDataType appDataType)
    {
        if (myValue==appDataType.myValue) return true;
        return false;
    }

    /**
     * Utility method for mapping type integer values to
     * corresponding string values.
     *
     * Returns "UNKNOWN" is the int type value is not
     * recognised.
     *
     * @param value The int value for a MamaMdDataType.
     * @return The string name value of the specified MamaMdDataType
     * integer value.
     */
    public static String valueToString (int value)
    {
        switch (value)
        {
            case STANDARD_VALUE:
                return "STANDARD";
            case ORDER_BOOK_VALUE:
                return "ORDER_BOOK";
            case NEWS_STORY_VALUE:
                return "NEWS_STORY";
            case WORLDVIEW_VALUE:
                return "WORLDVIEW";    
            default:
                return "UNKNOWN";
        }
    }

    /**
     * Return an instance of a MamaMdDataType corresponding to the
     * specified integer value.
     * Returns null if the integer value is not recognised.
     *
     * @param value Int value for a MamaMdDataType.
     * @return Instance of a MamaMdDataType if a mapping exists.
     */
    public static MamaMdDataType enumObjectForValue (int value)
    {
        switch (value)
        {
            case STANDARD_VALUE:
                return STANDARD;
            case ORDER_BOOK_VALUE:
                return ORDER_BOOK;
            case NEWS_STORY_VALUE:
                return NEWS_STORY;
            case WORLDVIEW_VALUE:
                return WORLDVIEW;    
            default:
                return null;
        }
    }
}
