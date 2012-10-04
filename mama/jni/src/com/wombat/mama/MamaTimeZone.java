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

import java.util.TimeZone;
import java.util.Calendar;

/**
 * A time zone representation to make conversion of timestamps to and
 * from particular time zones more convenient.
 *
 * Note: The addition of instance monitoring to the MamaTimeZone
 * implementation has resulted in the following limitation in its
 * usage.  Do not create short lived objects of this type on the
 * method stack or delete long lived objects before program
 * termination.  Pointers to all instances are maintained in a global
 * vector.  At the moment there is no mechanism by which we can detect
 * deleted objects or those which are popped off the method stack. An
 * internal thread will always iterate over all objects ever
 * created. A call to an object removed from the stack will result in
 * nondeterminable behaviour.  Pointers could be stored in a map
 * against a unique object id; however, addition and removal from the
 * map would have to be synchronized which would impact on
 * performance.
 */
public class MamaTimeZone
{
    private static MamaTimeZone myLocalTz     = new MamaTimeZone (TimeZone.getDefault ());
    private static MamaTimeZone myUtcTz       = new MamaTimeZone (TimeZone.getTimeZone ("UTC"));
    private static MamaTimeZone myUsEastern   = new MamaTimeZone (TimeZone.getTimeZone ("US/Eastern"));

    private TimeZone myTimeZone;

    /**
     * Internal constructor for creating timezones from java.util.TimeZone objects.
     * @param timeZone
     */
    private MamaTimeZone (TimeZone timeZone)
    {
        myTimeZone = timeZone;
    }

    /** Return a reference to a MamaTimeZone corresponding to the local
     * time zone. */
    public static MamaTimeZone local ()
    {
        return myLocalTz;
    }

    /** Return a reference to a MamaTimeZone corresponding to UTC time zone. */
    public static MamaTimeZone utc ()
    {
        return myUtcTz;
    }

    /** Return a reference to a MamaTimeZone corresponding to the US
     * Eastern time zone. */
    public static MamaTimeZone usEastern ()
    {
        return myUsEastern;
    }

    /** Constructor. */
    public MamaTimeZone ()
    {
        /**
         * The default for C/C++ sets TZ="TZ" which appears to be no timezone (date in bash reports the correct local
         * time with no timezone.  For now, we will just use the local timezone. I am not sure if this is correct.
         */
        myTimeZone = TimeZone.getDefault ();
    }


    /** Constructor.  NULL argument is equivalent to local timezone. */
    public MamaTimeZone (String tz)
    {
        myTimeZone = TimeZone.getTimeZone (tz);
    }

    /** Copy constructor. */
    public MamaTimeZone (MamaTimeZone  copy)
    {
        myTimeZone = copy.myTimeZone;
    }

    /**
     * Equals override. Two timezones are equal if they represent the same 
     * timezone
     */
    public boolean equals (Object obj)
    {
        if (obj instanceof MamaTimeZone)
        {
            return tz().equals ( ( (MamaTimeZone)obj).tz ());
        }
        return false;
    }

    /**
     * Hashcode override.
     */
    public int hashCode ()
    {
        /* Consistent with equals() */
        return tz ().hashCode ();
    }

    /** Assign new timezones to this object. */
    public void set (String tz)
    {
        myTimeZone = TimeZone.getTimeZone (tz);
    }

    /** Return the time zone string. */
    public String tz ()
    {
        return myTimeZone.getID ();
    }

    /** Return the offset from UTC (in seconds).  Can be positive or
     * negative, depending upon the direction. */
    public int offset ()
    {
        /**
         * This might be inefficient if this method gets called alot. In the C/C++ implementation, the offset is cached
         * and a spearate thread updates them at regular intervals. We may not need this for Java clients.
         */
        return myTimeZone.getOffset (System.currentTimeMillis ())/1000;
    }

    /**
     * Return a java.util.TimeZone object for this MamaTimeZone.
     */
    public TimeZone getJavaTimzone ()
    {
        return myTimeZone;
    }
}
