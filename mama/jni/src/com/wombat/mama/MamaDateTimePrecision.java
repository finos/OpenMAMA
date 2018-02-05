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


public class MamaDateTimePrecision
{
    public static final MamaDateTimePrecision PREC_SECONDS =
        new MamaDateTimePrecision ((short) 0, "seconds");
    public static final MamaDateTimePrecision PREC_DECISECONDS =
        new MamaDateTimePrecision ((short) 1, "deciseconds");
    public static final MamaDateTimePrecision PREC_CENTISECONDS =
        new MamaDateTimePrecision ((short) 2, "centiseconds");
    public static final MamaDateTimePrecision PREC_MILLISECONDS =
        new MamaDateTimePrecision ((short) 3, "milliseconds");
    public static final MamaDateTimePrecision PREC_MICROSECONDS =
        new MamaDateTimePrecision ((short) 6, "microseconds");
    public static final MamaDateTimePrecision PREC_DAYS =
        new MamaDateTimePrecision ((short) 10, "days");
    public static final MamaDateTimePrecision PREC_MINUTES =
        new MamaDateTimePrecision ((short) 12, "minutes");
    public static final MamaDateTimePrecision PREC_UNKNOWN =
        new MamaDateTimePrecision ((short) 15, "unknown");

    private short myPrecision = 15;
    private String myString = "unknown";

    private MamaDateTimePrecision (short precision, String stringValue)
    {
        myString    = stringValue;
        myPrecision = precision;
    }

    /**
     * Return the precision from the int value.
     */
    public static MamaDateTimePrecision getForInt (int value)
    {
        switch (value)
        {
        case 0:
            return PREC_SECONDS;
        case 1:
            return PREC_DECISECONDS;
        case 2:
            return PREC_CENTISECONDS;
        case 3:
            return PREC_MILLISECONDS;
        case 6:
            return PREC_MICROSECONDS;
        case 10:
            return PREC_DAYS;
        case 12:
            return PREC_MINUTES;
        default:
        case 15:
            return PREC_UNKNOWN;
        }
    }

    public short getShortValue ()
    {
        return myPrecision;
    }

    public String toString ()
    {
        return myString;
    }
}
