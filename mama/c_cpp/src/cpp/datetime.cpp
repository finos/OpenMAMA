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

#include "wombat/port.h"
#include "mama/mamacpp.h"
#include <math.h>

// YYYY-mm-dd HH:MM:SS.mmm
static const int MAX_DATE_TIME_STR_LEN = 10 + 1 + 15 + 1;

namespace Wombat
{

    MamaDateTime::MamaDateTime()
        : mStrRep (NULL)
    {
        mamaDateTime_clear (&mDateTime);
    }

    MamaDateTime::MamaDateTime (const MamaDateTime& copy)
        : mStrRep (NULL)
    {
        mamaDateTime_copy (&mDateTime,
                           const_cast<const mamaDateTime>(&copy.mDateTime));
    }

    MamaDateTime::MamaDateTime (const char*          str,
                                const MamaTimeZone*  tz)
        : mStrRep (NULL)
    {
        mamaDateTime_clear (&mDateTime);
        if (tz)
        {
            mamaDateTime_setFromStringWithTz (&mDateTime, str, tz->getCValue());
        }
        else
        {
            mamaDateTime_setFromString (&mDateTime, str);
        }
    }

    MamaDateTime::~MamaDateTime()
    {
        if (mStrRep)
        {
            delete[] (mStrRep);
        }
    }

    MamaDateTime& MamaDateTime::operator= (const MamaDateTime& rhs)
    {
        if (this != &rhs)
        {
            mamaDateTime_copy (&mDateTime,
                               const_cast<const mamaDateTime>(&rhs.mDateTime));
        }
        return *this;
    }

    int MamaDateTime::compare (const MamaDateTime&  rhs) const
    {
        return mamaDateTime_compare (
            const_cast<const mamaDateTime>(&mDateTime),
            const_cast<const mamaDateTime>(&rhs.mDateTime));
    }

    void MamaDateTime::clear ()
    {
        mamaDateTime_clear (&mDateTime);
    }

    void MamaDateTime::clearDate ()
    {
        mamaDateTime_clearDate (&mDateTime);
    }

    void MamaDateTime::clearTime ()
    {
        mamaDateTime_clearTime (&mDateTime);
    }

    void MamaDateTime::setEpochTimeF64 (double  seconds)
    {
        mamaDateTime_setEpochTimeF64 (&mDateTime, seconds);
    }

    void MamaDateTime::setEpochTimeMilliseconds (mama_u64_t  milliseconds)
    {
        mamaDateTime_setEpochTimeMilliseconds (&mDateTime, milliseconds);
    }

    void MamaDateTime::setEpochTimeMicroseconds (mama_u64_t  microseconds)
    {
        mamaDateTime_setEpochTimeMicroseconds (&mDateTime, microseconds);
    }

    void MamaDateTime::setEpochTime (mama_u32_t             seconds,
                                     mama_u32_t             microseconds,
                                     mamaDateTimePrecision  precision)
    {
        mamaDateTime_setEpochTime (&mDateTime, seconds, microseconds, precision);
    }

    void MamaDateTime::setWithHints (mama_u32_t             seconds,
                                     mama_u32_t             microseconds,
                                     mamaDateTimePrecision  precision,
                                     mamaDateTimeHints      hints)
    {
        mamaDateTime_setWithHints (&mDateTime, seconds, microseconds, precision,
                                   hints);
    }

    void MamaDateTime::setPrecision (mamaDateTimePrecision  precision)
    {
        mamaDateTime_setPrecision (&mDateTime, precision);
    }

    void MamaDateTime::setFromString (const char*           str,
                                      const MamaTimeZone*   tz)
    {
        if (tz)
        {
            mamaDateTime_setFromStringWithTz (&mDateTime, str, tz->getCValue());
        }
        else
        {
            mamaDateTime_setFromString (&mDateTime, str);
        }
    }

    void MamaDateTime::setFromString (const char*          str,
                                      mama_size_t          strLen,
                                      const MamaTimeZone*  tz)
    {
        if (tz)
        {
            mamaDateTime_setFromStringBufferWithTz (&mDateTime, str, strLen,
                                                    tz->getCValue());
        }
        else
        {
            mamaDateTime_setFromStringBuffer (&mDateTime, str, strLen);
        }
    }

    void MamaDateTime::setToNow ()
    {
        mamaDateTime_setToNow (&mDateTime);
    }

    void MamaDateTime::setToMidnightToday (const MamaTimeZone*  tz)
    {
        mamaDateTime_setToMidnightToday (&mDateTime, 
                                         tz == NULL ? NULL : tz->getCValue());
    }

    void MamaDateTime::set (mama_u32_t             year,
                            mama_u32_t             month,
                            mama_u32_t             day,
                            mama_u32_t             hours,
                            mama_u32_t             minutes,
                            mama_u32_t             seconds,
                            mama_u32_t             microseconds,
                            mamaDateTimePrecision  precision,
                            const MamaTimeZone*    tz)
    {
        if (tz)
        {
            mamaDateTime_setWithPrecisionAndTz (
                &mDateTime, year, month, day, hours, minutes, seconds,
                microseconds, precision, tz->getCValue());
        }
        else
        {
            mamaDateTime_setWithPrecisionAndTz (
                &mDateTime, year, month, day, hours, minutes, seconds,
                microseconds, precision, NULL);
        }
    }

    void MamaDateTime::setTime (mama_u32_t             hours,
                                mama_u32_t             minutes,
                                mama_u32_t             seconds,
                                mama_u32_t             microseconds,
                                mamaDateTimePrecision  precision,
                                const MamaTimeZone*    tz)
    {
        if (tz)
        {
            mamaDateTime_setTimeWithPrecisionAndTz (
                &mDateTime, hours, minutes, seconds, microseconds, precision,
                tz->getCValue());
        }
        else
        {
            mamaDateTime_setTimeWithPrecisionAndTz (
                &mDateTime, hours, minutes, seconds, microseconds, precision,
                NULL);
        }
    }

    void MamaDateTime::setDate (mama_u32_t   year,
                                mama_u32_t   month,
                                mama_u32_t   day)
    {
        mamaDateTime_setDate (&mDateTime, year, month, day);
    }

    void MamaDateTime::copyTime (const MamaDateTime&  copy)
    {
        mamaDateTime_copyTime (&mDateTime,
                               const_cast<const mamaDateTime>(&copy.mDateTime));
    }

    void MamaDateTime::copyDate (const MamaDateTime&  copy)
    {
        mamaDateTime_copyDate (&mDateTime,
                               const_cast<const mamaDateTime>(&copy.mDateTime));
    }

    void MamaDateTime::addSeconds (mama_f64_t  seconds)
    {
        mamaDateTime_addSeconds (&mDateTime, seconds);
    }

    void MamaDateTime::addSeconds (mama_i32_t  seconds)
    {
        mamaDateTime_addWholeSeconds (&mDateTime, seconds);
    }

    void MamaDateTime::addMicroseconds (mama_i64_t  microseconds)
    {
        mamaDateTime_addMicroseconds (&mDateTime, microseconds);
    }

    mama_u64_t MamaDateTime::getEpochTimeMicroseconds () const
    {
        mama_u64_t  result = 0;
        mamaDateTime_getEpochTimeMicroseconds (
            const_cast<const mamaDateTime>(&mDateTime), &result);
        return result;
    }

    mama_u64_t MamaDateTime::getEpochTimeMicroseconds (const MamaTimeZone& tz) const
    {
        mama_u64_t  result = 0;
        mamaDateTime_getEpochTimeMicrosecondsWithTz (
            const_cast<const mamaDateTime>(&mDateTime), &result, tz.getCValue());
        return result;
    }

    mama_u64_t MamaDateTime::getEpochTimeMilliseconds () const
    {
        mama_u64_t  result = 0;
        mamaDateTime_getEpochTimeMilliseconds (
            const_cast<const mamaDateTime>(&mDateTime), &result);
        return result;
    }

    mama_u64_t MamaDateTime::getEpochTimeMilliseconds (const MamaTimeZone& tz) const
    {
        mama_u64_t  result = 0;
        mamaDateTime_getEpochTimeMillisecondsWithTz (
            const_cast<const mamaDateTime>(&mDateTime), &result, tz.getCValue());
        return result;
    }

    mama_f64_t MamaDateTime::getEpochTimeSeconds () const
    {
        mama_f64_t  result = 0;
        mamaDateTime_getEpochTimeSeconds (
            const_cast<const mamaDateTime>(&mDateTime), &result);
        return result;
    }

    mama_f64_t MamaDateTime::getEpochTimeSeconds (const MamaTimeZone& tz) const
    {
        mama_f64_t  result = 0;
        mamaDateTime_getEpochTimeSecondsWithTz (
            const_cast<const mamaDateTime>(&mDateTime), &result, tz.getCValue());
        return result;
    }

    mama_f64_t MamaDateTime::getEpochTimeSecondsWithCheck () const
    {
        mama_f64_t  result = 0;
        mamaDateTime_getEpochTimeSecondsWithCheck (
            const_cast<const mamaDateTime>(&mDateTime), &result);
        return result;
    }

    void MamaDateTime::getAsStructTimeVal (struct timeval&  result) const
    {
        mamaDateTime_getStructTimeVal (const_cast<const mamaDateTime>(&mDateTime),
                                       &result);
    }

    void MamaDateTime::getAsStructTimeVal (struct timeval&      result,
                                           const MamaTimeZone&  tz) const
    {
        mamaDateTime_getStructTimeValWithTz (const_cast<const mamaDateTime>(&mDateTime),
                                             &result, tz.getCValue());
    }

    void MamaDateTime::getAsStructTm (struct tm&  result) const
    {
        mamaDateTime_getStructTm (const_cast<const mamaDateTime>(&mDateTime),
                                  &result);
    }

    void MamaDateTime::getAsStructTm (struct tm&            result,
                                      const MamaTimeZone&  tz) const
    {
        mamaDateTime_getStructTmWithTz (const_cast<const mamaDateTime>(&mDateTime),
                                        &result, tz.getCValue());
    }

    void MamaDateTime::getAsString (char*        result,
                                    mama_size_t  maxLen) const
    {
        mamaDateTime_getAsString (const_cast<const mamaDateTime>(&mDateTime),
                                  result, maxLen);
    }

    void MamaDateTime::getTimeAsString (char*        result,
                                        mama_size_t  maxLen) const
    {
        mamaDateTime_getTimeAsString (const_cast<const mamaDateTime>(&mDateTime),
                                      result, maxLen);
    }

    void MamaDateTime::getDateAsString (char*        result,
                                        mama_size_t  maxLen) const
    {
        mamaDateTime_getDateAsString (const_cast<const mamaDateTime>(&mDateTime),
                                      result, maxLen);
    }

    const char* MamaDateTime::getAsString () const
    {
        if (!mStrRep)
        {
            mStrRep = new char [MAX_DATE_TIME_STR_LEN + 1];
        }
        getAsString (mStrRep, MAX_DATE_TIME_STR_LEN);
        return mStrRep;
    }

    const char* MamaDateTime::getTimeAsString () const
    {
        if (!mStrRep)
        {
            mStrRep = new char [MAX_DATE_TIME_STR_LEN + 1];
        }
        getTimeAsString (mStrRep, MAX_DATE_TIME_STR_LEN);
        return mStrRep;
    }

    const char* MamaDateTime::getDateAsString () const
    {
        if (!mStrRep)
        {
            mStrRep = new char [MAX_DATE_TIME_STR_LEN + 1];
        }
        getDateAsString (mStrRep, MAX_DATE_TIME_STR_LEN);
        return mStrRep;
    }

    void MamaDateTime::getAsFormattedString (char*        result,
                                             mama_size_t  maxLen,
                                             const char*  format) const
    {
        mamaDateTime_getAsFormattedString (
            const_cast<const mamaDateTime>(&mDateTime), result, maxLen, format);
    }

    void MamaDateTime::getAsFormattedString (char*                result,
                                             mama_size_t          maxLen,
                                             const char*          format,
                                             const MamaTimeZone&  tz) const
    {
        mamaDateTime_getAsFormattedStringWithTz (
            const_cast<const mamaDateTime>(&mDateTime),
            result, maxLen, format, tz.getCValue());
    }

    mama_u32_t MamaDateTime::getYear() const
    {
        mama_u32_t  result = 0;
        mamaDateTime_getYear (const_cast<const mamaDateTime>(&mDateTime),&result);
        return result;
    }

    mama_u32_t MamaDateTime::getMonth() const
    {
        mama_u32_t  result = 0;
        mamaDateTime_getMonth(const_cast<const mamaDateTime>(&mDateTime), &result);
        return result;
    }

    mama_u32_t MamaDateTime::getDay() const
    {
        mama_u32_t  result = 0;
        mamaDateTime_getDay (const_cast<const mamaDateTime>(&mDateTime), &result);
        return result;
    }

    mama_u32_t MamaDateTime::getHour() const
    {
        mama_u32_t  result = 0;
        mamaDateTime_getHour (const_cast<const mamaDateTime>(&mDateTime), &result);
        return result;
    }

    mama_u32_t MamaDateTime::getMinute() const
    {
        mama_u32_t  result = 0;
        mamaDateTime_getMinute (const_cast<const mamaDateTime>(&mDateTime),
                                &result);
        return result;
    }

    mama_u32_t MamaDateTime::getSecond() const
    {
        mama_u32_t  result = 0;
        mamaDateTime_getSecond (const_cast<const mamaDateTime>(&mDateTime),
                                &result);
        return result;
    }

    mama_u32_t MamaDateTime::getMicrosecond() const
    {
        mama_u32_t  result = 0;
        mamaDateTime_getMicrosecond (const_cast<const mamaDateTime>(&mDateTime),
                                     &result);
        return result;
    }

    mamaDayOfWeek MamaDateTime::getDayOfWeek() const
    {
        mamaDayOfWeek  result = Sunday;
        mamaDateTime_getDayOfWeek (const_cast<const mamaDateTime>(&mDateTime),
                                   &result);
        return result;
    }

    bool MamaDateTime::operator== (const MamaDateTime& rhs) const
    {
        return (this == &rhs) ||
            mamaDateTime_equal (const_cast<const mamaDateTime>(&mDateTime),
                                const_cast<const mamaDateTime>(&rhs.mDateTime));
    }

    bool MamaDateTime::operator> (const MamaDateTime& rhs) const
    {
        return mamaDateTime_compare (
            const_cast<const mamaDateTime>(&mDateTime),
            const_cast<const mamaDateTime>(&rhs.mDateTime)) > 0;
    }

    bool MamaDateTime::operator< (const MamaDateTime& rhs) const
    {
        return mamaDateTime_compare (
            const_cast<const mamaDateTime>(&mDateTime),
            const_cast<const mamaDateTime>(&rhs.mDateTime)) < 0;
    }

    bool MamaDateTime::empty() const
    {
        return mamaDateTime_empty (const_cast<const mamaDateTime>(&mDateTime));
    }

    bool MamaDateTime::hasTime() const
    {
        mama_bool_t  result = 0;
        mamaDateTime_hasTime (const_cast<const mamaDateTime>(&mDateTime),
                              &result);
        return (result);
    }

    bool MamaDateTime::hasDate() const
    {
        mama_bool_t  result = 0;
        mamaDateTime_hasDate (const_cast<const mamaDateTime>(&mDateTime),
                              &result);
        return (result);
    }

    mamaDateTime MamaDateTime::getCValue()
    {
        return &mDateTime;
    }

    const mamaDateTime MamaDateTime::getCValue() const
    {
        return const_cast<const mamaDateTime>(&mDateTime);
    }

} /* namespace  Wombat */


mama_f64_t diffSeconds (const Wombat::MamaDateTime&  t1,
                        const Wombat::MamaDateTime&  t0)
{
    mama_f64_t result = 0.0;
    mamaDateTime_diffSeconds (t1.getCValue(), t0.getCValue(), &result);
    return result;
}

mama_f64_t diffSecondsSameDay (const Wombat::MamaDateTime&  t1,
                               const Wombat::MamaDateTime&  t0)
{
    mama_f64_t result = 0.0;
    mamaDateTime_diffSecondsSameDay (t1.getCValue(), t0.getCValue(), &result);
    return result;
}

mama_i64_t diffMicroseconds (const Wombat::MamaDateTime&  t1,
                             const Wombat::MamaDateTime&  t0)
{
    mama_i64_t result = 0;
    mamaDateTime_diffMicroseconds (t1.getCValue(), t0.getCValue(), &result);
    return result;
}
