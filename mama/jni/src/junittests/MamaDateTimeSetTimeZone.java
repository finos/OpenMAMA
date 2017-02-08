/* $Id$
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

package com.wombat.mama.junittests;

import junit.framework.*;
import com.wombat.mama.*;

/**
 *
 * This class will test setting a timezone on MamaDateTime and the setLogCallback function.
 */
public class MamaDateTimeSetTimeZone extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    // The date time under test
    protected MamaDateTime mDateTime;

    // A time zone
    protected MamaTimeZone mTimeZone;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        // Create the date time
        mDateTime = new MamaDateTime();

        // Create a new mama time zone
        mTimeZone = null;            // set to UTC for these tests
    }

    @Override
    protected void tearDown()
    {
        // Reset member variables
        mDateTime = null;
        mTimeZone = null;
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    public void testSetToMidnightToday()
    {
        // Call the function
        mDateTime.setToMidnightToday(mTimeZone);

        // If we get here then the test has passed
        // Check that the time is midnight
        String time = mDateTime.getTimeAsString();
        assertEquals(time, "00:00:00");
    }

    public void testSetTime()
    {
        // Set the time
        mDateTime.setTime(3, 16, 42, 99, MamaDateTimePrecision.PREC_MICROSECONDS, mTimeZone);

        // Get the time as a string
        String time = mDateTime.getTimeAsString();
        assertEquals(time, "03:16:42.000099");
    }

    public void testSetFromString()
    {
        // Recreate the date time using a string
        mDateTime = new MamaDateTime("03:16:42.000099", mTimeZone);

        // Get the time as a string
        String time = mDateTime.getTimeAsString();
        assertEquals(time, "03:16:42.000099");
    }

    public void testSetEpochTimeLargePositive()
    {
        mDateTime.setEpochTime (4211753600L, 0, MamaDateTimePrecision.PREC_UNKNOWN);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (4211753600.0, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (4211753600000000L, timeus);
        long timems = mDateTime.getEpochTimeMilliseconds();
        assertEquals (4211753600000L, timems);
    }


    public void testSetEpochTimeLargePositiveWithMSec()
    {
        mDateTime.setEpochTime (4211753600L, 999999, MamaDateTimePrecision.PREC_UNKNOWN);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (4211753600.999999, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (4211753600999999L, timeus);
        long timems = mDateTime.getEpochTimeMilliseconds();
        assertEquals (4211753600999L, timems);
    }

    public void testSetEpochTimeLargeNegative()
    {
        mDateTime.setEpochTime (-4211753600L, 0, MamaDateTimePrecision.PREC_UNKNOWN);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (-4211753600.0, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (-4211753600000000L, timeus);
        long timems = mDateTime.getEpochTimeMilliseconds();
        assertEquals (-4211753600000L, timems);
    }


    public void testSetEpochTimeLargeNegativeWithMSec()
    {
        mDateTime.setEpochTime (-4211753600L, 999999, MamaDateTimePrecision.PREC_UNKNOWN);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (-4211753599.000001, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (-4211753599000001L, timeus);
    }

    public void testSetEpochTimeNegativeExtreme()
    {
        // This test will use the largest number that can be represented as microseconds
        mDateTime.setEpochTime (-9223372036854L, 0, MamaDateTimePrecision.PREC_UNKNOWN);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (-9223372036854.0, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (-9223372036854000000L, timeus);
        long timems = mDateTime.getEpochTimeMilliseconds();
        assertEquals (-9223372036854000L, timems);
    }


    public void testSetEpochTimeNegativeExtremeWithMSec()
    {
        // This test will use the largest number that can be represented as microseconds
        mDateTime.setEpochTime (-9223372036854L, 999999, MamaDateTimePrecision.PREC_UNKNOWN);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (-9223372036853.000001, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (-9223372036853000001L, timeus);
        long timems = mDateTime.getEpochTimeMilliseconds();
        assertEquals (-9223372036853001L, timems);
    }

    public void testSetEpochTime()
    {
        mDateTime.setEpochTime (123456, 12, MamaDateTimePrecision.PREC_UNKNOWN);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (123456.000012, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (123456000012L, timeus);
    }


    public void testSetEpochTimeBigMicroSecond()
    {
        mDateTime.setEpochTime (123456, 999999, MamaDateTimePrecision.PREC_UNKNOWN);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (123456.999999, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (123456999999L, timeus);
    }

    public void testSetEpochTimeNegativeMicroSecond()
    {
        mDateTime.setEpochTime (123456, -123456, MamaDateTimePrecision.PREC_UNKNOWN);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (123455.876544, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (123455876544L, timeus);
    }

    public void testSetWithHints()
    {
        // The hints are never used.
        mDateTime.setWithHints (123456, 12, MamaDateTimePrecision.PREC_UNKNOWN, null);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (123456.000012, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (123456000012L, timeus);
    }

    public void testSetWithHintsWithMSec()
    {
        // The hints are never used.
        mDateTime.setWithHints (123456, 999999, MamaDateTimePrecision.PREC_UNKNOWN, null);

        double timeDouble = mDateTime.getEpochTimeSeconds();
        assertEquals (123456.999999, timeDouble);

        long timeus = mDateTime.getEpochTimeMicroseconds();
        assertEquals (123456999999L, timeus);
    }
}

