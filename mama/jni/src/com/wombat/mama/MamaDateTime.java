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

package com.wombat.mama;

import java.util.Date;
import java.util.Calendar;
import java.util.TimeZone;
import java.math.BigDecimal;
import java.text.SimpleDateFormat;

/**
 * MamaDataTime 
 *
 * Posix date/time representation.
 *
 */
public class MamaDateTime implements Comparable
{
    static
    {
        initIDs();
    }
    
    /* A long value containing a pointer to the underlying C message structure*/
    private long                dateTimePointer_i   = 0;
    private long                timeZonePointer_i   = 0;
    protected long getPointerVal()
    {
        return dateTimePointer_i;
    }
   
    protected void setPointerVal(long pointerVal)
    {
        dateTimePointer_i = pointerVal;
    } 
    
    /* Reusable object for return value*/
    private Calendar    myCalendar   = null;
    
    public MamaDateTime ()
    {
        _create();
    }

    public MamaDateTime (MamaDateTime copy)
    {
        _create ();
        copy (copy);    
    }

    public MamaDateTime (MamaDateTime time, MamaDateTime date)
    {
        _create ();
        setEpochTimeMilliseconds(time.getEpochTimeMilliseconds());
        setPrecision (MamaDateTimePrecision.PREC_MILLISECONDS);
        copyDate (date);
    }
    
    public MamaDateTime (Calendar calendar)
    {
        _create ();
        setEpochTimeMilliseconds(calendar.getTimeInMillis());
        setPrecision (MamaDateTimePrecision.PREC_MILLISECONDS);
    }

    public MamaDateTime (String str)
    {
       _create ();
	   _setFromString (str, null);
    }

    public MamaDateTime (String       str,
                         MamaTimeZone tz)
    {
        _create ();
        _setFromString (str, (tz!=null ? tz.tz():null)); 
    }

    public String toString ()
    {
        return getAsString ();
    }

    public int compareTo (Object obj)
    {  
        // If comparison is not possible this will throw a ClassCastException
        // which is what the Comparable spec requires.
        MamaDateTime time = (MamaDateTime) obj;
        long thisTime = getEpochTimeMicroseconds ();
        long thatTime = time.getEpochTimeMicroseconds ();

        return thisTime < thatTime ? -1 :
               thisTime > thatTime ?  1 : 0;
    }


    public boolean equals (Object obj)
    {
        if (obj instanceof MamaDateTime)
        {
            MamaDateTime time = (MamaDateTime) obj;
            return (time.getEpochTimeMicroseconds () == getEpochTimeMicroseconds ());
        }
        return false;
    }

    public int hashCode ()
    {
        return (int) getEpochTimeMicroseconds();
    }

    public native boolean isEmpty ();

    public void setEpochTime (long                  secondsSinceEpoch,
                              long                  microseconds,
                              MamaDateTimePrecision precision)
    {
        _setEpochTime (secondsSinceEpoch, microseconds, precision.getShortValue());                                  
    }

    public void setEpochTime (long secondsSinceEpoch,
                              long microseconds)
    {
        _setEpochTime (secondsSinceEpoch, microseconds, MamaDateTimePrecision.PREC_UNKNOWN.getShortValue());
    }


    public native void setEpochTimeF64 (double secondsSinceEpoch);

    public native void setEpochTimeMilliseconds (long millisecondsSinceEpoch);

    public native void setEpochTimeMicroseconds (long microsecondsSinceEpoch);

    public void setWithHints (long secondsSinceEpoch,
                              long microseconds,
                              MamaDateTimePrecision precision,
                              MamaDateTimeHints hints)
    {
        /**
         * This is not actually implemented in C/C++. The hints field is ignored.
         */
        _setEpochTime (secondsSinceEpoch, microseconds, precision.getShortValue());
    }

    void setPrecision (MamaDateTimePrecision precision) 
    {
        _setPrecision (precision.getShortValue());
    }
    
    final void setFromString (String       str, 
                              MamaTimeZone tz)
    {
        _setFromString (str,(tz!=null ? tz.tz():null));  
    }

    public native void setToNow ();


    public void setToMidnightToday (MamaTimeZone tz)
    {  
        _setToMidnightToday (tz!=null ? tz.tz():null);  
    }

    public void set (int year,
                     int month,
                     int day,
                     int hour,
                     int minute,
                     int second,
                     long microsecond,
                     MamaDateTimePrecision precision,
                     MamaTimeZone tz)
    {
        clear ();
        setDate (year, month, day);
        setTime (hour, minute, second, microsecond, precision, tz);
    }

    public void setTime (
        int hour,
        int minute,
        int second,
        long microsecond,
        MamaDateTimePrecision precision,
        MamaTimeZone tz)
    {
       _setTime (hour, minute, second, microsecond, precision.getShortValue(), (tz!=null ? tz.tz():null));
        
    }

    public native void setDate (
        int year,
        int month,
        int day);

    public native void copyTime (MamaDateTime src);

    public native void copyDate (MamaDateTime src);

    
    public native void clear ();

    public native void clearTime ();

    public native void clearDate ();

    public native boolean hasDate ();

    public native boolean hasTime ();

    public native void addSeconds (double seconds); 

    public native void addSeconds (long seconds);

    public native void addMicroSeconds (long microSeconds);

	/**
	 * Get the date and time as microseconds since the Epoch, (using the UTC timezone).	 
	 *	 
	 * @return The number of microseconds since the Epoch.
	 * @exception com.wombat.mama MamaException thrown if the underlying JNI object has been destroyed.
	 * @exception com.wombat.common.WombatException for other general MAMA errors.
	 */
    public native long getEpochTimeMicroseconds ();

    public native long getEpochTimeMilliseconds ();

    public native double getEpochTimeSeconds ();
	
	/**
	 * Get the date and time as seconds since the Epoch, (using the UTC timezone). 
	 * If no date value is contained in the dateTime then it will be set to today's date
	 * and the calculation made.	 
	 *	 
	 * @return The number of seconds, (including partial seconds), since the Epoch.
	 * @exception com.wombat.mama MamaException thrown if the underlying JNI object has been destroyed.
	 * @exception com.wombat.common.WombatException for other general MAMA errors.
	 */
	public native double getEpochTimeSecondsWithCheck ();

    public native String getAsString ();

    public native String getTimeAsString ();

    public native String getDateAsString ();

    public native long getYear ();
    
    public native long getMonth ();

    public native long getDay ();

    public native long getHour ();

    public native long getMinute ();

    public native long getSecond ();

    public native long getMicrosecond ();

    public MamaDayOfWeek getDayOfWeek ()
    {
        return  MamaDayOfWeek.getDayForValue(_getDayOfWeek());
    }

    public native static double diffSeconds (MamaDateTime t1,
                                             MamaDateTime t0);
                                      
    public native static double diffSecondsSameDay (MamaDateTime t1,
                                                    MamaDateTime t0);

    public native static long diffMicroseconds (MamaDateTime t1,
                                                MamaDateTime t0);

    public Calendar getCalendar ()
    {
        if (null==myCalendar) myCalendar = Calendar.getInstance ();
        /* Java's Calendar stores months starting from 0, i.e. January = 0, February = 1 etc */
        myCalendar.set ((int)getYear(), (int)getMonth() -1 , (int)getDay(), (int)getHour(), (int)getMinute(), (int)getSecond());
        return myCalendar;
    }

    public native void destroy ();
    
    private native void _create ();  
    
    public native void copy (MamaDateTime copy); 
    
    private native void _setEpochTime (long secondsSinceEpoch, 
                                       long microseconds, 
                                       short precision);
    
    private native void _setPrecision (short precision);
    
    private native int  _getPrecision ();
    
    private native void _setFromString (String str, 
                                       String tz);
    
    private native void _setToMidnightToday (String tz);
    
    private native void _setTime (int hour,
                                  int minute,
                                  int second,
                                  long microsecond,
                                  short precision,
                                  String tz);
                                  
  
    private native short _getDayOfWeek();
    
    /*Used to cache ids for callback methods/fields*/
    private static native void initIDs();
    
    protected void finalize()
    {
        if (dateTimePointer_i != 0)
            destroy();
    }
}
