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

#ifndef MamaDateTimeHpp__
#define MamaDateTimeHpp__

#include <mama/datetime.h>
#include <mama/MamaTimeZone.h>



namespace Wombat 
{

/**
 * A date/time representation with additional hints for precision,
 * advanced output formatting and support for time zone conversion
 * (using the MamaTimeZone type).
 *
 * Hints include: 
 *  - Whether the time stamp contains a date part, a time part, or both.
 *  - The level of accuracy (if known) of the time part (e.g., minutes, 
 *    seconds, milliseconds, etc.).
 *
 * The output format strings are similar to that available for the
 * strftime() function with the addition of %; which adds optional
 * (non-zero) fractional second to the string, and %: which adds
 * fractional seconds including trailing zeros, but does not include the ".".
 *  The following
 * table provides examples of output.<br>
 * <table>
 *   <tr>
 *     <th>Actual Time</th>
 *     <th>Output of "%T%;"</th>
 *     <th>Output of "%T%.:"</th>
 *   </tr>  
 *   <tr>
 *     <td>01:23:45 and 678 millisecs</td>
 *     <td>01:23:45.678</td>
 *     <td>01:23:45.678</td>
 *   </tr>
 *   <tr>
 *      <td>01:23:45 and 0 millisecs</td>
        <td>01:23:45</td>
        <td>01:23:45.000</td>
 *   </tr>
 * </table>
 */
class MAMACPPExpDLL MamaDateTime
{
public:
    MamaDateTime ();
    MamaDateTime (const MamaDateTime& copy);

    /**
     * Constructor taking a string argument.  This is the same as
     * calling setFromString() immediately after construction.
     */
    explicit MamaDateTime (const char*          str,
                           const MamaTimeZone*  tz = NULL);

    ~MamaDateTime ();

    MamaDateTime& operator=  (const MamaDateTime& rhs);
    bool          operator== (const MamaDateTime& rhs) const;
    bool          operator!= (const MamaDateTime& rhs) const
    { return ! operator== (rhs); }
    bool          operator<  (const MamaDateTime& rhs) const;
    bool          operator>  (const MamaDateTime& rhs) const;
    int           compare    (const MamaDateTime& rhs) const;
    bool          empty      () const;

    void          setEpochTime    (mama_u32_t             secondsSinceEpoch,
                                   mama_u32_t             microseconds,
                                   mamaDateTimePrecision  precision =
                                                  MAMA_DATE_TIME_PREC_UNKNOWN);
    void          setEpochTimeF64 (double                 secondsSinceEpoch);
    void          setEpochTimeMilliseconds (
                              mama_u64_t             millisecondsSinceEpoch);
    void          setEpochTimeMicroseconds (
                              mama_u64_t             microsecondsSinceEpoch);
    void          setWithHints    (mama_u32_t             secondsSinceEpoch,
                                   mama_u32_t             microseconds,
                                   mamaDateTimePrecision  precision =
                                                  MAMA_DATE_TIME_PREC_UNKNOWN,
                                   mamaDateTimeHints      hints = 0);
    void          setPrecision    (mamaDateTimePrecision  precision);
    void          setFromString   (const char*            str,
                                   const MamaTimeZone*    tz = NULL);
    void          setFromString   (const char*            str,
                                   mama_size_t            strLen,
                                   const MamaTimeZone*    tz = NULL);
    void          setToNow        ();
    void          setToMidnightToday (const MamaTimeZone*    tz = NULL);

    /**
     * Set the entire date and time for the MamaDateTime.  The year,
     * month and day parameters must all be integers greater than
     * zero.
     *
     * @param year          The year (must be 1970 or later).
     * @param month         The month (1 - 12).
     * @param day           The day (1 - 31).
     * @param hour          The hour (0 - 23).
     * @param minute        The minute (0 - 59).
     * @param second        The second (0 - 59).
     * @param microsecond   The second (0 - 999999).
     * @param precision     An explicit precision, if known.
     * @param tz            A timezone to shift from.
     */
    void set (
        mama_u32_t             year,
        mama_u32_t             month,
        mama_u32_t             day,
        mama_u32_t             hour,
        mama_u32_t             minute,
        mama_u32_t             second,
        mama_u32_t             microsecond,
        mamaDateTimePrecision  precision = MAMA_DATE_TIME_PREC_UNKNOWN,
        const MamaTimeZone*    tz = NULL);

    /**
     * Set the time-of-day portion of the MamaDateTime.  The date
     * portion is not affected.
     *
     * @param hour          The hour (0 - 23).
     * @param minute        The minute (0 - 59).
     * @param second        The second (0 - 59).
     * @param microsecond   The second (0 - 999999).
     * @param precision     An explicit precision, if known.
     * @param tz            A timezone to shift from.
     */
    void setTime (
        mama_u32_t             hour,
        mama_u32_t             minute,
        mama_u32_t             second,
        mama_u32_t             microsecond,
        mamaDateTimePrecision  precision = MAMA_DATE_TIME_PREC_UNKNOWN,
        const MamaTimeZone*    tz = NULL);

    /**
     * Set the date portion of the MamaDateTime.  The time-of-day
     * portion is not affected.  The year, month and day parameters
     * must all be integers greater than zero.
     *
     * @param year          The year (must be 1970 or later).
     * @param month         The month (1 - 12).
     * @param day           The day (1 - 31).
     */
    void setDate (
        mama_u32_t          year,
        mama_u32_t          month,
        mama_u32_t          day);

    /**
     * Copy the time-of-day portion of the MamaDateTime.  The date
     * portion is not affected.
     *
     * @param copy          The object to copy from
     */
    void copyTime (
        const MamaDateTime&  copy);

    /**
     * Copy the date portion of the MamaDateTime.  The time-of-day
     * portion is not affected.
     *
     * @param copy          The object to copy from
     */
    void copyDate (
        const MamaDateTime&  copy);

    /**
     * Clear the entire MamaDateTime.
     */
    void clear ();

    /**
     * Clear the time-of-day portion of the MamaDateTime.  The date
     * portion is not affected.
     */
    void clearTime ();

    /**
     * Clear the date portion of the MamaDateTime.  The time-of-day
     * portion is not affected.
     */
    void clearDate ();

    void          addSeconds      (mama_f64_t             seconds);
    void          addSeconds      (mama_i32_t             seconds);
    void          addMicroseconds (mama_i64_t             microSeconds);

	/**
	 * Get the date and time as microseconds since the Epoch, (using the UTC timezone).
	 *
	 * @return The number of microseconds since the Epoch.
	 */
    mama_u64_t    getEpochTimeMicroseconds () const;
	
	/**
	 * Get the date and time as microseconds since the Epoch in the supplied
	 * time zone.
	 *
	 * @param[int] tz The timezone.
	 * @return The number of microseconds since the Epoch.
	 */
    mama_u64_t    getEpochTimeMicroseconds (const MamaTimeZone&  tz) const;
    
    mama_u64_t    getEpochTimeMilliseconds () const;
    mama_u64_t    getEpochTimeMilliseconds (const MamaTimeZone&  tz) const;
    
    mama_f64_t    getEpochTimeSeconds      () const;
    mama_f64_t    getEpochTimeSeconds      (const MamaTimeZone&  tz) const;

	/**
	 * Get the date and time as seconds since the Epoch, (using the UTC timezone). 
	 * If no date value is contained in the dateTime then it will be set to today's date
	 * and the calculation made.
	 *	 
	 * @return The number of seconds, (including partial seconds), since the Epoch.
	 */
	mama_f64_t    getEpochTimeSecondsWithCheck() const;	
    
    void          getAsString     (char*        result,
                                   mama_size_t  maxLen) const;
    void          getTimeAsString (char*        result,
                                   mama_size_t  maxLen) const;
    void          getDateAsString (char*        result,
                                   mama_size_t  maxLen) const;

    /**
     * Return a string representation of the date/time.  Note that the
     * alternative getAsString() methods are more efficient because
     * these method must allocate a temporary buffer (automatically
     * destroyed upon object destruction).
     */
    const char*   getAsString     () const;
    const char*   getTimeAsString () const;
    const char*   getDateAsString () const;

    void          getAsFormattedString (char*                result,
                                        mama_size_t          maxLen,
                                        const char*          format) const;
    void          getAsFormattedString (char*                result,
                                        mama_size_t          maxLen,
                                        const char*          format,
                                        const MamaTimeZone&  tz) const;

    void          getAsStructTimeVal (struct timeval&       result) const;
    void          getAsStructTimeVal (struct timeval&       result,
                                      const MamaTimeZone&   tz) const;
                                      
    void          getAsStructTm      (struct tm&            result) const;
    void          getAsStructTm      (struct tm&            result,
                                      const MamaTimeZone&   tz) const;
                                      
    mama_u32_t    getYear        () const;
    mama_u32_t    getMonth       () const;
    mama_u32_t    getDay         () const;
    mama_u32_t    getHour        () const;
    mama_u32_t    getMinute      () const;
    mama_u32_t    getSecond      () const;
    mama_u32_t    getMicrosecond () const;
    mamaDayOfWeek getDayOfWeek   () const;

    /**
     * Return whether the object has a time component.
     */
    bool hasTime() const;

    /**
     * Return whether the object has a date component.
     */
    bool hasDate() const;

    mamaDateTime        getCValue();
    const mamaDateTime  getCValue() const;

private:
    mama_u64_t      mDateTime;
    mutable char*   mStrRep;
};

} // namespace Wombat


mama_f64_t  diffSeconds        (const Wombat::MamaDateTime&  t1,
                                const Wombat::MamaDateTime&  t0);

mama_f64_t  diffSecondsSameDay (const Wombat::MamaDateTime&  t1,
                                const Wombat::MamaDateTime&  t0);

mama_i64_t  diffMicroseconds   (const Wombat::MamaDateTime&  t1,
                                const Wombat::MamaDateTime&  t0);

#endif // MamaDateTimeHpp__
