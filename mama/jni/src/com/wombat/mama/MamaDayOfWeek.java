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

import java.util.Calendar;

public class MamaDayOfWeek
{
    public static final MamaDayOfWeek SUNDAY =
        new MamaDayOfWeek ((short) 0, "Sunday");
    public static final MamaDayOfWeek MONDAY =
        new MamaDayOfWeek ((short) 1, "Monday");
    public static final MamaDayOfWeek TUESDAY =
        new MamaDayOfWeek ((short) 2, "Tuesday");
    public static final MamaDayOfWeek WEDNESDAY =
        new MamaDayOfWeek ((short) 3, "Wednesday");
    public static final MamaDayOfWeek THURSDAY =
        new MamaDayOfWeek ((short) 4, "Thursday");
    public static final MamaDayOfWeek FRIDAY =
        new MamaDayOfWeek ((short) 5, "Friday");
    public static final MamaDayOfWeek SATURDAY =
        new MamaDayOfWeek ((short) 6, "Saturday");

    private short myDay = 0;
    private String myName = "Sunday";

    public MamaDayOfWeek (short day, String name)
    {
        myDay = day;
        myName = name;
    }

    public String toString ()
    {
        return myName;
    }

    public short getValue ()
    {
        return myDay;
    }

    public static MamaDayOfWeek getDayForValue (short value)
    {
        switch (value)
        {
        case 0:
            return SUNDAY;
        case 1:
            return MONDAY;
        case 2:
            return TUESDAY;
        case 3:
            return WEDNESDAY;
        case 4:
            return THURSDAY;
        case 5:
            return FRIDAY;
        case 6:
            return SATURDAY;
        default:
            return null;
        }
    }

    public static MamaDayOfWeek getDayForJavaDay (int javaDay)
    {
        switch (javaDay)
        {
        case Calendar.MONDAY:
            return MONDAY;
        case Calendar.TUESDAY:
            return TUESDAY;
        case Calendar.WEDNESDAY:
            return WEDNESDAY;
        case Calendar.THURSDAY:
            return THURSDAY;
        case Calendar.FRIDAY:
            return FRIDAY;
        case Calendar.SATURDAY:
            return SATURDAY;
        case Calendar.SUNDAY:
            return SUNDAY;
        default:
            throw new MamaException ("Invalid Day");
        }
    }
}
