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
 * The MamaPricePrecision class determines how decimals are represented for
 * the price. PRECISION_10 - PRECISION_10000000000 represent decimal places.
 * PRECISION_DIV_2, _4, etc, represent fraction 1/2's ,1/4's etc.
 *
 * TICK_32, HALF_32 are for feeds that send prices in 1/32's and half 1/32's.
 */
public class MamaPricePrecision
{
    public static final MamaPricePrecision PRECISION_UNKNOWN =
        new MamaPricePrecision ((byte) 0x00, "UNKNOWN");
    public static final MamaPricePrecision PRECISION_10 =
        new MamaPricePrecision ((byte) 0x01, "10");
    public static final MamaPricePrecision PRECISION_100 =
        new MamaPricePrecision ((byte) 0x02, "100");
    public static final MamaPricePrecision PRECISION_1000 =
        new MamaPricePrecision ((byte) 0x03, "1000");
    public static final MamaPricePrecision PRECISION_10000 =
        new MamaPricePrecision ((byte) 0x04, "10000");
    public static final MamaPricePrecision PRECISION_100000 =
        new MamaPricePrecision ((byte) 0x05, "100000");
    public static final MamaPricePrecision PRECISION_1000000 =
        new MamaPricePrecision ((byte) 0x06, "1000000");
    public static final MamaPricePrecision PRECISION_10000000 =
        new MamaPricePrecision ((byte) 0x07, "10000000");
    public static final MamaPricePrecision PRECISION_100000000 =
        new MamaPricePrecision ((byte) 0x08, "100000000");
    public static final MamaPricePrecision PRECISION_1000000000 =
        new MamaPricePrecision ((byte) 0x09, "1000000000");
    public static final MamaPricePrecision PRECISION_10000000000 =
        new MamaPricePrecision ((byte) 0x0a, "10000000000");
    public static final MamaPricePrecision PRECISION_INT =
        new MamaPricePrecision ((byte) 0x10, "INT");
    public static final MamaPricePrecision PRECISION_DIV_2 =
        new MamaPricePrecision ((byte) 0x11, "2");
    public static final MamaPricePrecision PRECISION_DIV_4 =
        new MamaPricePrecision ((byte) 0x12, "4");
    public static final MamaPricePrecision PRECISION_DIV_8 =
        new MamaPricePrecision ((byte) 0x13, "8");
    public static final MamaPricePrecision PRECISION_DIV_16 =
        new MamaPricePrecision ((byte) 0x14, "16");
    public static final MamaPricePrecision PRECISION_DIV_32 =
        new MamaPricePrecision ((byte) 0x15, "32");
    public static final MamaPricePrecision PRECISION_DIV_64 =
        new MamaPricePrecision ((byte) 0x16, "64");
    public static final MamaPricePrecision PRECISION_DIV_128 =
        new MamaPricePrecision ((byte) 0x17, "128");
    public static final MamaPricePrecision PRECISION_DIV_256 =
        new MamaPricePrecision ((byte) 0x18, "256");
    public static final MamaPricePrecision PRECISION_DIV_512 =
        new MamaPricePrecision ((byte) 0x19, "512");
    public static final MamaPricePrecision PRECISION_TICK_32 =
        new MamaPricePrecision ((byte) 0x1a, "TICK_32");
    public static final MamaPricePrecision PRECISION_HALF_32 =
        new MamaPricePrecision ((byte) 0x1b, "HALF_32");
    public static final MamaPricePrecision PRECISION_QUARTER_32 =
        new MamaPricePrecision ((byte) 0x1c, "QUARTER_32");
    public static final MamaPricePrecision PRECISION_TICK_64 =
        new MamaPricePrecision ((byte) 0x1d, "TICK_64");
    public static final MamaPricePrecision PRECISION_HALF_64 =
        new MamaPricePrecision ((byte) 0x1e, "HALF_64");


    private static final byte BYTE_UNKNOWN     = 0x00;
    private static final byte BYTE_10          = 0x01;
    private static final byte BYTE_100         = 0x02;
    private static final byte BYTE_1000        = 0x03;
    private static final byte BYTE_10000       = 0x04;
    private static final byte BYTE_100000      = 0x05;
    private static final byte BYTE_1000000     = 0x06;
    private static final byte BYTE_10000000    = 0x07;
    private static final byte BYTE_100000000   = 0x08;
    private static final byte BYTE_1000000000  = 0x09;
    private static final byte BYTE_10000000000 = 0x0a;
    private static final byte BYTE_INT         = 0x10;
    private static final byte BYTE_DIV_2       = 0x11;
    private static final byte BYTE_DIV_8       = 0x13;
    private static final byte BYTE_DIV_4       = 0x12;
    private static final byte BYTE_DIV_16      = 0x14;
    private static final byte BYTE_DIV_32      = 0x15;
    private static final byte BYTE_DIV_64      = 0x16;
    private static final byte BYTE_DIV_128     = 0x17;
    private static final byte BYTE_DIV_256     = 0x18;
    private static final byte BYTE_DIV_512     = 0x19;
    private static final byte BYTE_TICK_32     = 0x1a;
    private static final byte BYTE_HALF_32     = 0x1b;
    private static final byte BYTE_QUARTER_32  = 0x1c;
    private static final byte BYTE_TICK_64     = 0x1d;
    private static final byte BYTE_HALF_64     = 0x1e;

    byte myPrecision; /* Package level visibility */
    private String myText;

    public MamaPricePrecision (byte value, String text)
    {
        myPrecision = value;
        myText = text;
    }

    public String toString ()
    {
        return myText;
    }

    /**
     * Return the appropriate precision for a given number of
     * decimal places. If places > 10 return Unknown.
     */
    public static MamaPricePrecision decimals2Precision (int places)
    {
        if (places > 0 && places <= 10)
        {
            return precisionFromByte ((byte)places);
        }
        return PRECISION_UNKNOWN;
    }

    /**
     * Return the appropriate precision code for a given fractional
     * denominator.
     */
    public static MamaPricePrecision denom2Precision (int denominator)
    {
        switch (denominator)
        {
        case 1:   return PRECISION_INT;
        case 2:   return PRECISION_DIV_2;
        case 4:   return PRECISION_DIV_4;
        case 8:   return PRECISION_DIV_8;
        case 16:  return PRECISION_DIV_16;
        case 32:  return PRECISION_DIV_32;
        case 64:  return PRECISION_DIV_64;
        case 128: return PRECISION_DIV_128;
        case 256: return PRECISION_DIV_256;
        case 512: return PRECISION_DIV_512;
        default:  return PRECISION_UNKNOWN;
        }
    }

    /**
     * Return the number of decimal places for this precision.
     * Returns 10 for fractional precision for consistency with C API.
     */
    public int precision2Decimals ()
    {
        if (myPrecision < 0 || myPrecision > 10)
            return 10;
        return myPrecision;
    }

    /**
     * Return the fractional denominator or 0 if the precision is not
     * fractional.
     */
    public int precision2Denom ()
    {
        switch (myPrecision)
        {
        case BYTE_INT:     return 1;
        case BYTE_DIV_2:   return 2;
        case BYTE_DIV_4:   return 4;
        case BYTE_DIV_8:   return 8;
        case BYTE_DIV_16:  return 16;
        case BYTE_DIV_32:  return 32;
        case BYTE_DIV_64:  return 64;
        case BYTE_DIV_128: return 128;
        case BYTE_DIV_256: return 256;
        case BYTE_DIV_512: return 512;
        default:           return 0;
        }
    }

    static MamaPricePrecision precisionFromByte (
        byte precision)
    {
        switch (precision)
        {
        case BYTE_UNKNOWN:
        default:
            return PRECISION_UNKNOWN;
        case BYTE_10:
            return PRECISION_10;
        case BYTE_100:
            return PRECISION_100;
        case BYTE_1000:
            return PRECISION_1000;
        case BYTE_10000:
            return PRECISION_10000;
        case BYTE_100000:
            return PRECISION_100000;
        case BYTE_1000000:
            return PRECISION_1000000;
        case BYTE_10000000:
            return PRECISION_10000000;
        case BYTE_100000000:
            return PRECISION_100000000;
        case BYTE_1000000000:
            return PRECISION_1000000000;
        case BYTE_10000000000:
            return PRECISION_10000000000;
        case BYTE_INT:
            return PRECISION_INT;
        case BYTE_DIV_2:
            return PRECISION_DIV_2;
        case BYTE_DIV_4:
            return PRECISION_DIV_4;
        case BYTE_DIV_8:
            return PRECISION_DIV_8;
        case BYTE_DIV_16:
            return PRECISION_DIV_16;
        case BYTE_DIV_32:
            return PRECISION_DIV_32;
        case BYTE_DIV_64:
            return PRECISION_DIV_64;
        case BYTE_DIV_128:
            return PRECISION_DIV_128;
        case BYTE_DIV_256:
            return PRECISION_DIV_256;
        case BYTE_DIV_512:
            return PRECISION_DIV_512;
        case BYTE_TICK_32:
            return PRECISION_TICK_32;
        case BYTE_HALF_32:
            return PRECISION_HALF_32;
        case BYTE_QUARTER_32:
            return PRECISION_QUARTER_32;
        case BYTE_TICK_64:
            return PRECISION_TICK_64;
        case BYTE_HALF_64:
            return PRECISION_HALF_64;
        }
    }
}
