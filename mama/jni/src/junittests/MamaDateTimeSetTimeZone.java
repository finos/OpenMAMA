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
        mTimeZone = new MamaTimeZone();
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
}
