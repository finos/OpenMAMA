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

/**
* This class provides an enumeration of types and methods for
* MAMA throttle instance. Used with setOutboundThrottle and 
* getOutboundThrottle in the JNI version of the API. 
*
* Enum to determine into which throttle a call applies. Currently the
* default throttle, used by the publisher, and the initial value
* request throttle are the same. Mama sends recap requests on a
* separate throttle.
*/
public final class MamaThrottleInstance
{
    private final String myStringValue;
    private final int    myValue;

    /** The default throttle queue*/
    public static final int           DEFAULT_THROTTLE_VALUE = 0;
    public static final MamaThrottleInstance DEFAULT_THROTTLE       = 
            new MamaThrottleInstance (valueToString (DEFAULT_THROTTLE_VALUE), 
                                      DEFAULT_THROTTLE_VALUE);
    
    /** The initial value throttle queue*/
    public static final int           INITIAL_THROTTLE_VALUE = 1;
    public static final MamaThrottleInstance INITIAL_THROTTLE       = 
            new MamaThrottleInstance (valueToString (INITIAL_THROTTLE_VALUE), 
                                      INITIAL_THROTTLE_VALUE);

    /** The recap throttle queue*/
    public static final int           RECAP_THROTTLE_VALUE = 2;
    public static final MamaThrottleInstance RECAP_THROTTLE       = 
            new MamaThrottleInstance (valueToString (RECAP_THROTTLE_VALUE), 
                                      RECAP_THROTTLE_VALUE);
    
    /* No publicly created instances allowed */
    private MamaThrottleInstance (String name, int value) 
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
     * This value can be used in switch statements against the public
     * XXX_VALUE static members of the class.
     * 
     * @return The integer type.
     */
    public int getValue ()
    {
        return myValue;
    }

    /**
     * Compare the two types for equality. 
     * Returns true if the integer value of both types is equal. Otherwise
     * returns false.
     *
     * @param throttleInstance The object to check equality against.
     * @return Whether the two objects are equal.
     */
    public boolean equals (MamaThrottleInstance throttleInstance)
    {
        if (myValue==throttleInstance.myValue) return true;
        return false;
    }

    /**
    * Utility method for mapping type integer values to corresponding string
    * values.
    *
    * Returns "UNKNOWN" is the int type value is not recognised.
    *
    * @param value The int value for a MamaThrottleInstance.
    * @return The string name value of the specified MamaThrottleInstance integer
    * value.
    */
    public static String valueToString (int value)
    {
        switch (value)
        {
            case DEFAULT_THROTTLE_VALUE:
                return "DEFAULT_THROTTLE";
            case INITIAL_THROTTLE_VALUE:
                return "INITIAL_THROTTLE";
            case RECAP_THROTTLE_VALUE:
                return "RECAP_THROTTLE";
            default:
                return "UNKNOWN";
        }
    }

    /**
     * Return an instance of a MamaMdMsgType corresponding to the specified
     * integer value. 
     * Returns null if the integer value is not recognised.
     *
     * @param value Int value for a MamaThrottleInstance.
     * @return Instance of a MamaThrottleInstance if a mapping exists.
     */
    public static MamaThrottleInstance enumObjectForValue (int value)
    {
        switch (value)
        {
            case DEFAULT_THROTTLE_VALUE:
                return DEFAULT_THROTTLE;
            case INITIAL_THROTTLE_VALUE:
                return INITIAL_THROTTLE;
            case RECAP_THROTTLE_VALUE:
                return RECAP_THROTTLE;
            default:
                return null;
        }
    }
}
