/* $Id: datetime.h,v 1.15.4.2.10.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaDateTimeH__
#define MamaDateTimeH__

#include <mama/config.h>
#include <mama/status.h>
#include <mama/types.h>
#include <mama/timezone.h>
#include "wombat/port.h"

#include <time.h>


#if defined(__cplusplus)
extern "C" {
#endif


typedef enum mamaDateTimePrecision_
{
    MAMA_DATE_TIME_PREC_SECONDS      = 0,
    MAMA_DATE_TIME_PREC_DECISECONDS  = 1,
    MAMA_DATE_TIME_PREC_CENTISECONDS = 2,
    MAMA_DATE_TIME_PREC_MILLISECONDS = 3,
    MAMA_DATE_TIME_PREC_MICROSECONDS = 6,
    MAMA_DATE_TIME_PREC_DAYS         = 10,
    MAMA_DATE_TIME_PREC_MINUTES      = 12,
    MAMA_DATE_TIME_PREC_UNKNOWN      = 15
} mamaDateTimePrecision;

typedef enum mamaDayOfWeek_
{
    Sunday = 0, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
} mamaDayOfWeek;


typedef mama_u8_t  mamaDateTimeHints;
#define MAMA_DATE_TIME_HAS_DATE    ((mamaDateTimeHints) 0x01)
#define MAMA_DATE_TIME_HAS_TIME    ((mamaDateTimeHints) 0x02)
#define MAMA_DATE_TIME_NO_TIMEZONE ((mamaDateTimeHints) 0x04)


/**
 * Create a date/time object.
 *
 * @param dateTime The location of a mamaDateTime to store the result
 */
MAMAExpDLL
extern mama_status
mamaDateTime_create (mamaDateTime* dateTime);

/**
 * Destroy a mamaDateTime object.
 *
 * @param dateTime  The date/time object to destroy.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_destroy (mamaDateTime dateTime);

/**
 * Clear a mamaDateTime object.
 *
 * @param dateTime  The date/time object to clear.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_clear (mamaDateTime dateTime);

/**
 * Clear the date part of a mamaDateTime object.
 *
 * @param dateTime  The date/time object to clear (preserving the time of day).
 */
MAMAExpDLL
extern mama_status
mamaDateTime_clearDate (mamaDateTime dateTime);

/**
 * Clear the time of day part of a mamaDateTime object (preserving the date).
 *
 * @param dateTime  The date/time object to clear.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_clearTime (mamaDateTime dateTime);

/*
 * Copy a date/time object.  The destination object must have already
 * been allocated using mamaDateTime_create().
 */
MAMAExpDLL
extern mama_status
mamaDateTime_copy (mamaDateTime       dest,
                   const mamaDateTime src);

/*
 * Check whether the date/time object has been set to anything
 */
MAMAExpDLL
extern int
mamaDateTime_empty (const mamaDateTime dateTime);

/*
 * Check for equality between two date/time objects.
 */
MAMAExpDLL
extern int
mamaDateTime_equal (const mamaDateTime lhs,
                    const mamaDateTime rhs);

/*
 * Compare two date/time objects.  The return value is negative if lhs
 * is earlier than rhs, positive if lhs is greater than rhs and zero
 * if the two are equal.
 */
MAMAExpDLL
extern int
mamaDateTime_compare (const mamaDateTime lhs,
                      const mamaDateTime rhs);

/**
 * Set the date and time as seconds and microseconds since the Epoch
 * (UTC time zone) with an option to designate the accuracy of the
 * time.
 *
 * @param dateTime      The dateTime to set.
 * @param seconds       The number of seconds since the Epoch.
 * @param microseconds  The number of microseconds.
 * @param precision     The precision of the time stamp.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setEpochTime(mamaDateTime           dateTime,
                          mama_u32_t             seconds,
                          mama_u32_t             microseconds,
                          mamaDateTimePrecision  precision);

/**
 * Set the date and time as seconds (plus, possibly fractional
 * seconds) since the Epoch (UTC time zone).  Fractional seconds will
 * be rounded to microseconds.
 *
 * @param dateTime The dateTime to set.
 * @param seconds  The number of seconds since the Epoch.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setEpochTimeF64(mamaDateTime dateTime,
                             mama_f64_t   seconds);

/**
 * Set the date and time as milliseconds.
 *
 * @param dateTime The dateTime to set.
 * @param milliseconds  The number of milliseconds since the Epoch.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setEpochTimeMilliseconds(mamaDateTime dateTime,
                                      mama_u64_t   milliseconds);

/**
 * Set the date and time as microseconds.
 *
 * @param dateTime The dateTime to set.
 * @param milliseconds  The number of microseconds since the Epoch.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setEpochTimeMicroseconds(mamaDateTime dateTime,
                                      mama_u64_t   milliseconds);

/**
 * Set the date and/or time with special, optional hints to indicate
 * whether the date/time includes date information, time information
 * and/or whether a user of the date/time should consider it in the
 * context of a time zone.
 *
 * @param dateTime      The dateTime to set.
 * @param seconds       The number of seconds (since the Epoch or start-of-day if no date).
 * @param microseconds  The number of microseconds.
 * @param precision     The precision of the date/time stamp.
 * @param hints         Additional hints
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setWithHints(mamaDateTime           dateTime,
                          mama_u32_t             seconds,
                          mama_u32_t             microseconds,
                          mamaDateTimePrecision  precision,
                          mamaDateTimeHints      hints);

/**
 * Set the precision hint.
 *
 * @param dateTime      The dateTime to set.
 * @param precision     The precision of the date/time stamp.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setPrecision(mamaDateTime           dateTime,
                          mamaDateTimePrecision  precision);

/**
 * Set the date and time to the current UTC time.  Precision and hints
 * will be set appropriately.
 *
 * @param dateTime      The dateTime to set.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setToNow(mamaDateTime dateTime);

/**
 * Set the dateTime object to 12am of the current date in the timezone
 * provided (or UTC if NULL).
 *
 * @param dateTime  The dateTime to set.
 * @param tz        The timezone in which the date will be set.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setToMidnightToday(mamaDateTime       dateTime,
                                const mamaTimeZone tz);

/**
 * Set the entire date and time for the MamaDateTime.  The year,
 * month and day parameters must all be integers greater than
 * zero.
 *
 * @param dateTime      The dateTime to set.
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
MAMAExpDLL
extern mama_status
mamaDateTime_setWithPrecisionAndTz(mamaDateTime           dateTime,
                                   mama_u32_t             year,
                                   mama_u32_t             month,
                                   mama_u32_t             day,
                                   mama_u32_t             hour,
                                   mama_u32_t             minute,
                                   mama_u32_t             second,
                                   mama_u32_t             microsecond,
                                   mamaDateTimePrecision  precision,
                                   const mamaTimeZone     tz);

/**
 * Set the time-of-day portion of the MamaDateTime.  The date
 * portion is not affected.
 *
 * @param dateTime      The dateTime to set.
 * @param hour          The hour (0 - 23).
 * @param minute        The minute (0 - 59).
 * @param second        The second (0 - 59).
 * @param microsecond   The microsecond (0 - 999999).
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setTime(mamaDateTime dateTime,
                     mama_u32_t   hour,
                     mama_u32_t   minute,
                     mama_u32_t   second,
                     mama_u32_t   microsecond);

/**
 * Set the time-of-day portion of the MamaDateTime with explicit
 * precision.  The date portion is not affected.
 *
 * @param dateTime      The dateTime to set.
 * @param hour          The hour (0 - 23).
 * @param minute        The minute (0 - 59).
 * @param second        The second (0 - 59).
 * @param microsecond   The microsecond (0 - 999999).
 * @param precision     An explicit precision, if known.
 * @param tz            A timezone to shift from.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setTimeWithPrecisionAndTz(mamaDateTime           dateTime,
                                       mama_u32_t             hour,
                                       mama_u32_t             minute,
                                       mama_u32_t             second,
                                       mama_u32_t             microsecond,
                                       mamaDateTimePrecision  precision,
                                       const mamaTimeZone     tz);

/**
 * Set the date portion of the MamaDateTime.  The time-of-day
 * portion is not affected.
 *
 * @param dateTime      The dateTime to set.
 * @param year          The year (must be 1970 or later).
 * @param month         The month (1 - 12).
 * @param day           The day (1 - 31).
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setDate(mamaDateTime dateTime,
                     mama_u32_t   year,
                     mama_u32_t   month,
                     mama_u32_t   day);

/**
 * Copy the time-of-day portion of the mamaDateTime.  The date
 * portion is not affected.
 *
 * @param dest      The dateTime to copy to.
 * @param src       The dateTime to copy from.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_copyTime (mamaDateTime       dest,
                       const mamaDateTime src);

/**
 * Copy the date portion of the mamaDateTime.  The time-of-day
 * portion is not affected.
 *
 * @param dest      The dateTime to copy to.
 * @param src       The dateTime to copy from.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_copyDate (mamaDateTime       dest,
                       const mamaDateTime src);

/**
 * Determine whether the time-of-day portion of the MamaDateTime is set.
 *
 * @param dateTime  The dateTime to check.
 * @param result    Address of a bool to store the result. 0 false.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_hasTime(const mamaDateTime dateTime,
                     mama_bool_t*       result);

/**
 * Determine whether the date portion of the MamaDateTime is set.
 *
 * @param dateTime  The dateTime to check.
 * @param result    Address of mama_bool_t to store the result. 0 false.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_hasDate(const mamaDateTime dateTime,
                     mama_bool_t*       result);

/**
 * Add a number of seconds, including fractional seconds (may be negative).
 *
 * @param dateTime      The dateTime to set.
 * @param seconds       The number of seconds to add (or subtract).
 */
MAMAExpDLL
extern mama_status
mamaDateTime_addSeconds(mamaDateTime dateTime,
                        mama_f64_t   seconds);

/**
 * Add a number of whole seconds (may be negative).
 *
 * @param dateTime      The dateTime to set.
 * @param seconds       The number of seconds to add (or subtract).
 */
MAMAExpDLL
extern mama_status
mamaDateTime_addWholeSeconds(mamaDateTime dateTime,
                             mama_i32_t   seconds);

/**
 * Add a number of microseconds (may be negative)
 * Add a number of microseconds (may be negative and greater than
 * 1000000 or less than -1000000).
 *
 * @param dateTime      The dateTime to set.
 * @param microseconds The number of microseconds to add. Can be positive
 * or negative. Note that there are no restrictions on the magnitude of
 * this value.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_addMicroseconds(mamaDateTime dateTime,
                             mama_i64_t   microseconds);

/**
 * Set the date and time from a string representation of format
 * YYYY-mm-dd HH:MM:SS.mmmmmm.  (YYYY/mm/dd also works.)  The
 * precision of subseconds is determined by the number of digits
 * after the decimal point.
 *
 * @param dateTime The dateTime to set.
 * @param str      The string representation of some date and/or time.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setFromString(mamaDateTime dateTime,
                           const char*  str);

/**
 * Set the date and time from a string representation of format
 * YYYY-mm-dd HH:MM:SS.mmmmmm.  (YYYY/mm/dd also works.)  The
 * precision of subseconds is determined by the number of digits after
 * the decimal point.  If tz is not NULL, the time string is assumed
 * to be set in a different time zone and will be adjusted to UTC
 * accordingly.
 *
 * @param dateTime The dateTime to set.
 * @param str      The string representation of some date and/or time.
 * @param tz       A timezone to shift from.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setFromStringWithTz(mamaDateTime        dateTime,
                                 const char*         str,
                                 const mamaTimeZone  tz);

/**
 * Set the date and time from an unterminated string representation of
 * format YYYY-mm-dd HH:MM:SS.mmmmmm.  (YYYY/mm/dd also works.)  The
 * precision of subseconds is determined by the number of digits after
 * the decimal point.
 *
 * @param dateTime  The dateTime to set.
 * @param str       The string representation of some date and/or time.
 * @param strLen    The length of the unterminated string date/time.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setFromStringBuffer(mamaDateTime dateTime,
                                 const char*  str,
                                 mama_size_t  strLen);

/**
 * Set the date and time from an unterminated string representation of
 * format YYYY-mm-dd HH:MM:SS.mmmmmm.  (YYYY/mm/dd also works.)  The
 * precision of subseconds is determined by the number of digits after
 * the decimal point.  If tz is not NULL, the time string is assumed
 * to be set in a different time zone and will be adjusted to UTC
 * accordingly.
 *
 * @param dateTime  The dateTime to set.
 * @param str       The string representation of some date and/or time.
 * @param strLen    The length of the unterminated string date/time.
 * @param tz        A timezone to shift from.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_setFromStringBufferWithTz(mamaDateTime       dateTime,
                                       const char*        str,
                                       mama_size_t        strLen,
                                       const mamaTimeZone tz);

/**
 * Get the date and time as seconds and microseconds since the Epoch
 * (UTC time zone).
 *
 * @param dateTime The dateTime to set.
 * @param seconds  The number of seconds since the Epoch.
 * @param microseconds  The number of additional microseconds.
 * @param precision The precision of the returned time value.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getEpochTime(const mamaDateTime     dateTime,
                          mama_u32_t*            seconds,
                          mama_u32_t*            microseconds,
                          mamaDateTimePrecision* precision);
    
/**
 * Get the date and time as seconds and microseconds since the Epoch
 * in the timezone supplied. 
 *
 * @param dateTime The dateTime to set.
 * @param seconds  The number of seconds since the Epoch.
 * @param microseconds  The number of additional microseconds.
 * @param precision The precision of the returned time value.
 * @param tz The timezone for the returned values.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getEpochTimeWithTz(const mamaDateTime     dateTime,
                                mama_u32_t*            seconds,
                                mama_u32_t*            microseconds,
                                mamaDateTimePrecision* precision,
                                const mamaTimeZone     tz);

/**
 * Get the date and time as microseconds since the Epoch, (using the UTC timezone).
 *
 * @param[in] 	dateTime 		The dateTime to obtain the number of microseconds from.
 * @param[out] 	microseconds  	The number of microseconds since the Epoch.
 * @return Indicates whether the function succeeded or failed and could be one of:
 *				- MAMA_STATUS_INVALID_ARG
 *				- MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getEpochTimeMicroseconds(const mamaDateTime dateTime,
                                      mama_u64_t*        microseconds);

/**
 * Get the date and time as microseconds since the Epoch in the supplied
 * time zone.
 *
 * @param[in] 	dateTime 		The dateTime to obtain the number of microseconds from.
 * @param[out] 	microseconds  	The number of microseconds since the Epoch.
 * @param[int] 	tz 			 	The timezone.
 * @return Indicates whether the function succeeded or failed and could be one of:
 *				- MAMA_STATUS_INVALID_ARG
 *				- MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getEpochTimeMicrosecondsWithTz(const mamaDateTime dateTime,
                                            mama_u64_t*        microseconds,
                                            const mamaTimeZone tz);
    
/**
 * Get the date and time as milliseconds since the Epoch (UTC time
 * zone).
 *
 * @param dateTime The dateTime to set.
 * @param milliseconds  The number of milliseconds since the Epoch.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getEpochTimeMilliseconds(const mamaDateTime dateTime,
                                      mama_u64_t*        milliseconds);

/**
 * Get the date and time as milliseconds since the Epoch in the 
 * timezone supplied.
 *
 * @param dateTime The dateTime to set.
 * @param milliseconds  The number of milliseconds since the Epoch.
 * @param tz The timezone for the returned values. 
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getEpochTimeMillisecondsWithTz(const mamaDateTime dateTime,
                                            mama_u64_t*        milliseconds,
                                            const mamaTimeZone tz);
    
/**
 * Get the date and time as seconds since the Epoch (UTC time
 * zone).
 *
 * @param dateTime The dateTime to set
 * @param seconds The number of seconds (including partial seconds) since the Epoch.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getEpochTimeSeconds(const mamaDateTime dateTime,
                                 mama_f64_t*        seconds);

/**
 * Get the date and time as seconds since the Epoch, (using the UTC timezone). 
 * If no date value is contained in the dateTime then it will be set to today's date
 * and the calculation made.
 *
 * @param[in] 	dateTime 	The dateTime to obtain the number of microseconds from.
 * @param[out] 	seconds 	The number of seconds, (including partial seconds), since the Epoch.
 * @return Indicates whether the function succeeded or failed and could be one of:
 *				- MAMA_STATUS_INVALID_ARG
 *				- MAMA_STATUS_SYSTEM_ERROR
 *				- MAMA_STATUS_NULL_ARG
 *				- MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getEpochTimeSecondsWithCheck(const mamaDateTime dateTime,
                                          mama_f64_t*        seconds);

/**
 * Get the date and time as seconds since the Epoch in the 
 * timezone supplied.
 *
 * @param dateTime The dateTime to set
 * @param seconds The number of seconds (including partial seconds) since the
 * Epoch.
 * @param tz The timezone for the returned values. 
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getEpochTimeSecondsWithTz(const mamaDateTime dateTime,
                                       mama_f64_t*        seconds,
                                       const mamaTimeZone tz);
    
/**
 * Get the date and/or time with special, optional hints to indicate
 * whether the date/time includes date information, time information
 * and/or whether a user of the date/time should consider it in the
 * context of a time zone.
 *
 * @param dateTime      The dateTime to set.
 * @param seconds       The number of seconds (since the Epoch or start-of-day if no date).
 * @param microseconds  The number of microseconds.
 * @param precision     The precision of the date/time stamp.
 * @param hints         Additional hints
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getWithHints(const mamaDateTime     dateTime,
                          mama_u32_t*            seconds,
                          mama_u32_t*            microseconds,
                          mamaDateTimePrecision* precision,
                          mama_u8_t*             hints);

/**
 * Get the date/time as a "struct timeval".
 *
 * @param dateTime The dateTime to set.
 * @param result   The struct timeval to fill in.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getStructTimeVal(const mamaDateTime dateTime,
                              struct timeval*    result);

/**
 * Get the date/time as a "struct timeval" in the timezone supplied.
 *
 * @param dateTime The dateTime to set.
 * @param result   The struct timeval to fill in.
 * @param tz The timezone for the returned values. 
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getStructTimeValWithTz(const mamaDateTime dateTime,
                                    struct timeval*    result,
                                    const mamaTimeZone tz);
    
/**
 * Get the date/time as a "struct tm".
 *
 * @param dateTime The dateTime to set.
 * @param result   The struct timeval to fill in.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getStructTm(const mamaDateTime dateTime,
                         struct tm*         result);

/**
 * Get the date/time as a "struct tm" in the timezone supplied.
 *
 * @param dateTime The dateTime to set.
 * @param result   The struct timeval to fill in.
 * @param tz The timezone for the returned values. 
 
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getStructTmWithTz(const mamaDateTime dateTime,
                               struct tm*         result,
                               const mamaTimeZone tz);
    
/**
 * Get the date and/or time as a string.  If no date information is
 * available, no date is printed in the resulting string.  The format
 * for dates is YYYY-mm-dd (the ISO 8601 date format) and the format
 * for times is HH:MM:SS.mmmmmmm (where the precision of the
 * subseconds may vary depending upon any precision hints available).
 *
 * @param dateTime      The dateTime to set.
 * @param str           The string buffer to update.
 * @param maxLen        The maximum size of the string buffer (including
 * trailing '\\0').
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getAsString(const mamaDateTime dateTime,
                         char*              str,
                         mama_size_t        maxLen);

/**
 * Get the time (no date) as a string.  The format for times is
 * HH:MM:SS.mmmmmmm (where the precision of the subseconds may vary
 * depending upon any precision hints available).
 *
 * @param dateTime      The dateTime to set.
 * @param str           The string buffer to update.
 * @param maxLen        The maximum size of the string buffer (including
 * trailing '\\0').
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getTimeAsString(const mamaDateTime dateTime,
                             char*              str,
                             mama_size_t        maxLen);

/**
 * Get the date (no time) as a string.  The format
 * for dates is YYYY-mm-dd (the ISO 8601 date format).
 *
 * @param dateTime      The dateTime to set.
 * @param str           The string buffer to update.
 * @param maxLen        The maximum size of the string buffer (including
 * trailing '\\0').
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getDateAsString(const mamaDateTime dateTime,
                             char*              str,
                             mama_size_t        maxLen);

/**
 * Get the date and/or time as a string using the format provided.
 * The format string can be the common ones supported by strftime()
 * (%Y, %m, %d, %F, %T, %Y, %H, %M, %S, %B, %b, %h, ), with the
 * additional format strings, "%:" and "%;", which represents the
 * number of subseconds in millis. "%;" includes the dot and only prints the
 * subseconds if they are non-zero. "%:" does not include the dot, and 
 * prints "000" for 0 milliseconds. "%." and "%," are identical to "%:" and "%;" 
 * except they use the internal precision field to determine how many decimal 
 * places to print.
 *
 *
 * @param dateTime      The dateTime to set.
 * @param str           The string buffer to update.
 * @param maxLen        The maximum size of the string buffer (including
 * trailing '\\0').
 * @param format        The output format.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getAsFormattedString(const mamaDateTime dateTime,
                                  char*              str,
                                  mama_size_t        maxLen,
                                  const char*        format);

/**
 * Get the date and/or time as a string using the format provided.
 * The format string can be the common ones supported by strftime()
 * (%Y, %m, %d, %F, %T, %Y, %H, %M, %S, %B, %b, %h, ), with the
 * additional format strings, "%:" and "%;", which represents the
 * number of subseconds in millis. "%;" includes the dot and only prints the
 * subseconds if they are non-zero. "%:" does not include the dot, and 
 * prints "000" for 0 milliseconds. "%." and "%," are identical to "%:" and "%;" 
 * except they use the internal precision field to determine how many decimal 
 * places to print.
 *
 * @param dateTime      The dateTime to set.
 * @param str           The string buffer to update.
 * @param maxLen        The maximum size of the string buffer (including
 * trailing '\\0').
 * @param format        The output format.
 * @param tz            The timezone for the returned date/time.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getAsFormattedStringWithTz(const mamaDateTime dateTime,
                                        char*              str,
                                        mama_size_t        maxLen,
                                        const char*        format,
                                        const mamaTimeZone tz);

/**
 * Get the year (1970 onwards).
 *
 * @param dateTime      The dateTime from which to get the result.
 * @param result        The result of the get method.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getYear(const mamaDateTime dateTime,
                     mama_u32_t*        result);

/**
 * Get the month (1-12).
 *
 * @param dateTime      The dateTime from which to get the result.
 * @param result        The result of the get method.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getMonth(const mamaDateTime dateTime,
                      mama_u32_t*        result);

/**
 * Get the day of month (1-31).
 *
 * @param dateTime      The dateTime from which to get the result.
 * @param result        The result of the get method.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getDay(const mamaDateTime dateTime,
                    mama_u32_t*        result);

/**
 * Get the hour (0-23).
 *
 * @param dateTime      The dateTime from which to get the result.
 * @param result        The result of the get method.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getHour(const mamaDateTime dateTime,
                     mama_u32_t*        result);

/**
 * Get the minute (0-59).
 *
 * @param dateTime      The dateTime from which to get the result.
 * @param result        The result of the get method.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getMinute(const mamaDateTime dateTime,
                       mama_u32_t*        result);

/**
 * Get the second (0-59).
 *
 * @param dateTime      The dateTime from which to get the result.
 * @param result        The result of the get method.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getSecond(const mamaDateTime dateTime,
                       mama_u32_t*        result);

/**
 * Get the microsecond (0-999999).
 *
 * @param dateTime      The dateTime from which to get the result.
 * @param result        The result of the get method.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getMicrosecond(const mamaDateTime dateTime,
                            mama_u32_t*        result);

/**
 * Get the day of week.
 *
 * @param dateTime      The dateTime from which to get the result.
 * @param result        The result of the get method.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_getDayOfWeek(const mamaDateTime dateTime,
                          mamaDayOfWeek*     result);

/**
 * Return the difference, in seconds (including fractions of seconds),
 * between the two times.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_diffSeconds(const mamaDateTime t1,
                         const mamaDateTime t0,
                         mama_f64_t*        result);

/**
 * Return the difference, in seconds (including fractions of seconds),
 * between the two times, ignoring any date information.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_diffSecondsSameDay(const mamaDateTime t1,
                                const mamaDateTime t0,
                                mama_f64_t*        result);

/**
 * Return the difference, in microseconds, between the two times.
 */
MAMAExpDLL
extern mama_status
mamaDateTime_diffMicroseconds(const mamaDateTime t1,
                              const mamaDateTime t0,
                              mama_i64_t*        result);


#if defined(__cplusplus)
}
#endif

#endif
