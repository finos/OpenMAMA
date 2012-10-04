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
 * The <code>MamaFieldDescriptor</code> class represents a field in a
 * <code>MamaMsg</code>
 *
 * @see MamaDictionary
 * @see MamaMsg
 */
public class MamaFieldDescriptor
{
    // These type constants are borrowed directly from the Tibco Rendezvous API
    // be careful when changing them. The currently unused ones are commented out

    public static final short MSG           =   1;
    public static final short DATETIME      =   3;
    public static final short OPAQUE        =   7;
    public static final short STRING        =   8;
    public static final short BOOL          =   9;
    public static final short CHAR          =   10;
    public static final short I8            =   14;
    public static final short U8            =   15;
    public static final short I16           =   16;
    public static final short U16           =   17;
    public static final short I32           =   18;
    public static final short U32           =   19;
    public static final short I64           =   20;
    public static final short U64           =   21;
    public static final short F32           =   24;
    public static final short F64           =   25;
    public static final short TIME          =   26;
    public static final short PRICE         =   27;
    public static final short I8ARRAY       =   34;
    public static final short U8ARRAY       =   35;
    public static final short I16ARRAY      =   36;
    public static final short U16ARRAY      =   37;
    public static final short I32ARRAY      =   38;
    public static final short U32ARRAY      =   39;
    public static final short I64ARRAY      =   40;
    public static final short U64ARRAY      =   41;
    public static final short F32ARRAY      =   44;
    public static final short F64ARRAY      =   45;
    
    public static final short VECTOR_STRING  = 46;
    public static final short VECTOR_MSG     = 47;
    public static final short VECTOR_TIME    = 48;
    public static final short VECTOR_PRICE   = 49;
    public static final short QUANTITY       = 50;

    public static final short VECTOR         = 98;
    public static final short COLLECTION     = 99;
    public static final short UNKNOWN        = 100;

    private final int    myFid;
    private final short  myType;
    private final String myName;
    private final Object myData;
    private Object       myClosure;
    private boolean      myTrackModState;

    /**
     * Create a <code>MamaFieldDescriptor</code>
     *
     * @param fid  The field identifier.
     * @param type The type.
     * @param name The field name.
     */
    public MamaFieldDescriptor( int fid, short type, String name, Object data )
    {
        this.myFid = fid;
        this.myType = type;
        this.myName = name;
        this.myData = data;
        this.myClosure = null;
    }

    /**
     * Return the field identifier.
     *
     * @return The fid.
     */
    public int getFid()
    {
        return myFid;
    }

    /**
     * Return the data type.
     *
     * @return The type.
     */
    public short getType()
    {
        return myType;
    }

    /**
     * Return the human readable name of the field.
     *
     * @return The name.
     */
    public String getName()
    {
        return myName;
    }

    /**
     * Returns a string suitable for output to a terminal containing the type,
     * id, and name.
     *
     * @return  A human readable string for the field.
     */
    public String toString()
    {
        return myData != null ? "" + myData : getTypeName (myType);
    }

    /**
     * Set user specified data to associate with the field descriptor.
     *
     * @param closure The data to associate with the descriptor.
     */
    public void setClosure (Object closure)
    {
        myClosure = closure;
    }

    /**
     * Return the user specified data associated with the field descriptor.
     * The closure data has a default value of <code>null</code>
     *
     * @return The user specified data associated with the field descriptor.
     */
    public Object getClosure ()
    {
        return myClosure;
    }

    public static String getTypeName( short type )
    {
        switch( type )
        {
        case OPAQUE :
            return "OPAQUE";
        case STRING :
            return "STRING";
        case I8:
            return "I8";
        case U8 :
            return "U8";
        case I16 :
            return "I16";
        case U16 :
            return "U16";
        case I32 :
            return "I32";
        case U32 :
            return "U32";
        case I64 :
            return "I64";
        case U64 :
            return "U64";
        case F32 :
            return "F32";
        case F64 :
            return "F64";
        case U16ARRAY  :
        case U32ARRAY  :
            return "INT_ARRAY";
        case VECTOR:
            return "VECTOR";
        case COLLECTION:
            return "COLLECTION";
        case BOOL:
            return "BOOL";
        case CHAR:
            return "CHAR";
        case MSG:
            return "MSG";
        case TIME:
            return "TIME";
        case PRICE:
            return "PRICE";
        case DATETIME:
            return "DATETIME";
        case I8ARRAY:
            return "BYTE_ARRAY";
        case U8ARRAY:
        case I16ARRAY:
            return "SHORT_ARRAY";
        case I32ARRAY:
        case I64ARRAY:
        case U64ARRAY:
            return "LONG_ARRAY";
        case F32ARRAY:
            return "FLOAT_ARRAY";
        case F64ARRAY:
            return "DOUBLE_ARRAY";
        case VECTOR_MSG:
            return "VECTOR_MSG";
        case VECTOR_TIME:
            return "VECTOR_TIME";
        case VECTOR_PRICE:
            return "VECTOR_PRICE";
        default :
            return "UNKNOWN";
        }
    }
    
    public void setTrackModState (boolean on)
    {
        myTrackModState = on;
    }
   
    public boolean getTrackModState ()
    {
        return myTrackModState;
    }
}
