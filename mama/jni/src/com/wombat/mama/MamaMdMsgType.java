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
* MAMA types related to market data applications.
*
* It is intended that the types in this class will ultimately replace the
* MamaMsgType as an indicator of the type of market data message received by a
* subscribing application.
*/
public final class MamaMdMsgType
{
    private final String myStringValue;
    private final int    myValue;

    /** Trade Cancellation*/
    public static final int           CANCEL_VALUE = 2;
    public static final MamaMdMsgType CANCEL   = new MamaMdMsgType
                         (valueToString (CANCEL_VALUE), 
                          CANCEL_VALUE);

    /** Trade Error*/
    public static final int           ERROR_VALUE = 3;
    public static final MamaMdMsgType ERROR = new MamaMdMsgType
                         (valueToString (ERROR_VALUE), 
                          ERROR_VALUE);

    /** Trade correction*/
    public static final int           CORRECTION_VALUE = 4;
    public static final MamaMdMsgType CORRECTION = new MamaMdMsgType
                         (valueToString (CORRECTION_VALUE), 
                          CORRECTION_VALUE);

    /** Closing summary*/
    public static final int           CLOSING_VALUE = 5;
    public static final MamaMdMsgType CLOSING = new MamaMdMsgType
                         (valueToString (CLOSING_VALUE), 
                          CLOSING_VALUE);

    /** Action related to this symbol, such as a name change or symbol
        deletion due to option/future expiration, etc.*/
    public static final int           SYMBOL_ACTION_VALUE = 8;
    public static final MamaMdMsgType SYMBOL_ACTION = new MamaMdMsgType
                         (valueToString (SYMBOL_ACTION_VALUE), 
                          SYMBOL_ACTION_VALUE);

    /** Pre-opening summary (e.g. morning roll)*/
    public static final int           PREOPENING_VALUE = 12;
    public static final MamaMdMsgType PREOPENING = new MamaMdMsgType
                         (valueToString (PREOPENING_VALUE), 
                          PREOPENING_VALUE);

    /** Quote update. */
    public static final int           QUOTE_VALUE = 13;
    public static final MamaMdMsgType QUOTE = new MamaMdMsgType
                         (valueToString (QUOTE_VALUE), 
                          QUOTE_VALUE);

    /** Trade Update*/
    public static final int           TRADE_VALUE = 14;
    public static final MamaMdMsgType TRADE = new MamaMdMsgType
                         (valueToString (TRADE_VALUE), 
                          TRADE_VALUE);

    /** Order book initial value*/
    public static final int           BOOK_INITIAL_VALUE = 16;
    public static final MamaMdMsgType BOOK_INITIAL = new MamaMdMsgType
                         (valueToString (BOOK_INITIAL_VALUE),
                          BOOK_INITIAL_VALUE);

    /** Order imbalance or noimbalance update*/
    public static final int           IMBALANCE_VALUE = 17;
    public static final MamaMdMsgType IMBALANCE = new MamaMdMsgType
                         (valueToString (IMBALANCE_VALUE),
                          IMBALANCE_VALUE);
    
    /** Security status update*/
    public static final int           SECURITY_STATUS_VALUE = 18;
    public static final MamaMdMsgType SECURITY_STATUS = new MamaMdMsgType
                         (valueToString (SECURITY_STATUS_VALUE),
                          SECURITY_STATUS_VALUE);

    /** Miscellaneous.*/
    public static final int           MISC_VALUE = 100;
    public static final MamaMdMsgType MISC = new MamaMdMsgType
                         (valueToString (MISC_VALUE), 
                          MISC_VALUE);

    /* No publicly created instances allowed */
    private MamaMdMsgType (String name, int value) 
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
     * @param appDataType The object to check equality against.
     * @return Whether the two objects are equal.
     */
    public boolean equals (MamaMdMsgType appDataType)
    {
        if (myValue==appDataType.myValue) return true;
        return false;
    }

    /** 
     * Get the md msg type from the specified MamaMsg.
     * Returns null if no type was found in the message.
     * 
     * @param msg The MamaMsg from which the msg type will be extracted.
     * @return An instance of a MamaMdMsgType
     */
    public static MamaMdMsgType typeForMsg (final MamaMsg msg)
    {
        short msgType = -1;
        
        msgType = msg.getU8 (MamaReservedFields.AppMsgType.getName(),
                             MamaReservedFields.AppMsgType.getId(),
                             (short)-1/*Default return value*/);

        return enumObjectForValue (msgType);
    }

    /**
     * The stringified name of the type based on the int value for the type in
     * the specified message.
     * 
     * @param msg The MamaMsg from which the type is to be extracted.
     * @return The string name for the type.
     */
    public static String stringForMsg (final MamaMsg msg)
    {
        return typeForMsg (msg).toString();
    }

    /**
     * Return the corresponding MamaMsgType for the specified MamaMdMsgType.
     * Mainly for backwards compatibility.
     * 
     * @param mdMsgType The MamaMdMsgType for which a mapping is required.
     * @return The MamaMsgType corresponding to the specified MamaMdMsgType.
     */
    public static short compatMsgType (MamaMdMsgType mdMsgType)
    {
        int value  = mdMsgType.getValue();
        switch (value)
        {
            case CANCEL_VALUE:
                return MamaMsgType.TYPE_UPDATE;
            case ERROR_VALUE:
                return MamaMsgType.TYPE_ERROR;
            case CORRECTION_VALUE:
                return MamaMsgType.TYPE_CORRECTION;
            case CLOSING_VALUE:
                return MamaMsgType.TYPE_CLOSING;
            case PREOPENING_VALUE:
                return MamaMsgType.TYPE_PREOPENING;
            case QUOTE_VALUE:
                return MamaMsgType.TYPE_QUOTE;
            case TRADE_VALUE:
                return MamaMsgType.TYPE_TRADE;
            case BOOK_INITIAL_VALUE:
                return MamaMsgType.TYPE_BOOK_INITIAL;
            case MISC_VALUE:
                return MamaMsgType.TYPE_MISC;
            default:
                return MamaMsgType.TYPE_MISC;
        }
    }

    /**
    * Utility method for mapping type integer values to corresponding string
    * values.
    *
    * Returns "UNKNOWN" is the int type value is not recognised.
    *
    * @param value The int value for a MamaMdMsgType.
    * @return The string name value of the specified MamaMdMsgType integer
    * value.
    */
    public static String valueToString (int value)
    {
        switch (value)
        {
            case CANCEL_VALUE:
                return "CANCEL";
            case ERROR_VALUE:
                return "ERROR";
            case CORRECTION_VALUE:
                return "CORRECTION";
            case CLOSING_VALUE:
                return "CLOSING";
            case SYMBOL_ACTION_VALUE:
                return "SYMBOL_ACTION";
            case PREOPENING_VALUE:
                return "PREOPENING";
            case QUOTE_VALUE:
                return "QUOTE";
            case TRADE_VALUE:
                return "TRADE";
            case BOOK_INITIAL_VALUE:
                return "BOOK_INITIAL";
            case IMBALANCE_VALUE:
                return "IMBALANCE";
            case SECURITY_STATUS_VALUE:
                return "SECURITY_STATUS";
            case MISC_VALUE:
                return "MISC";
            default:
                return "UNKNOWN";
        }
    }

    /**
     * Return an instance of a MamaMdMsgType corresponding to the specified
     * integer value. 
     * Returns null if the integer value is not recognised.
     *
     * @param value Int value for a MamaMdMsgType.
     * @return Instance of a MamaMdMsgType if a mapping exists.
     */
    public static MamaMdMsgType enumObjectForValue (int value)
    {
        switch (value)
        {
            case CANCEL_VALUE:
                return CANCEL;
            case ERROR_VALUE:
                return ERROR;
            case CORRECTION_VALUE:
                return CORRECTION;
            case CLOSING_VALUE:
                return CLOSING;
            case SYMBOL_ACTION_VALUE:
                return SYMBOL_ACTION;
            case PREOPENING_VALUE:
                return PREOPENING;
            case QUOTE_VALUE:
                return QUOTE;
            case TRADE_VALUE:
                return TRADE;
            case BOOK_INITIAL_VALUE:
                return BOOK_INITIAL;
            case IMBALANCE_VALUE:
                return IMBALANCE;
            case SECURITY_STATUS_VALUE:
                return SECURITY_STATUS;
            case MISC_VALUE:
                return MISC;
            default:
                return null;
        }
    }
}
