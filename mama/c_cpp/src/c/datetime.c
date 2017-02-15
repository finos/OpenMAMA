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

#include <mama/log.h>
#include <mama/datetime.h>
#include <mama/timezone.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include "datetimeimpl.h"
#include "mamaStrUtils.h"

#include "wombat/wincompat.h"

                                 /* YYYY-mm-dd HH:MM:SS.mmmmmm */
#define MAX_DATE_TIME_STR_LEN (10 + 1 + 15 + 1)
#define SECONDS_IN_A_DAY      (24 * 60 * 60)

static void
utcTm (struct tm*   result,
       time_t       secSinceEpoch);

static unsigned long
makeTime (int           year,
          int           mon,
          int           day,
          int           hour,
          int           min,
          int           sec);

static void
printSubseconds (char*                  result,
                 mama_size_t*           resLen,
                 mama_size_t            maxLen,
                 mamaDateTimePrecision  precision,
                 long                   microsecs);

static mama_status
mamaDateTime_addTodayToDateTime (mamaDateTime destination);


mama_status
mamaDateTime_create (mamaDateTime* dateTime)
{
    *dateTime = (mamaDateTime) malloc (sizeof(mama_time_t));

    if (*dateTime == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }
    else
    {
        mamaDateTimeImpl_clear((mama_time_t*)*dateTime);
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaDateTime_destroy (mamaDateTime dateTime)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    free ((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_clear (mamaDateTime dateTime)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_clear((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_clearDate (mamaDateTime dateTime)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_clearDate((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_clearTime (mamaDateTime dateTime)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_clearTime((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_copy (mamaDateTime       dest,
                   const mamaDateTime src)
{
    if (!src || !dest)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_copy((mama_time_t*)dest,(const mama_time_t*)src);
    return MAMA_STATUS_OK;
}

int mamaDateTime_empty (const mamaDateTime dateTime)
{
    if (!dateTime)
        return 1;
    return mamaDateTimeImpl_empty((mama_time_t*)dateTime);
}

int mamaDateTime_equal (const mamaDateTime lhs,
                        const mamaDateTime rhs)
{
    if (lhs == rhs)
        return 1;
    if (!lhs || !rhs)
        return 0;
    return mamaDateTimeImpl_equal ((mama_time_t*)lhs,(mama_time_t*)rhs);
}

int mamaDateTime_compare (const mamaDateTime lhs,
                          const mamaDateTime rhs)
{
    if (NULL == lhs || NULL == rhs)
        return 0;

    mamaDateTimeImpl_compare((mama_time_t*)lhs, (mama_time_t*)rhs);
}

mama_status
mamaDateTime_setEpochTime(mamaDateTime           dateTime,
                          mama_u32_t             seconds,
                          mama_u32_t             microseconds,
                          mamaDateTimePrecision  precision)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_clear           ((mama_time_t*)dateTime);
    mamaDateTimeImpl_setSeconds      ((mama_time_t*)dateTime, seconds);
    mamaDateTimeImpl_setMicroSeconds ((mama_time_t*)dateTime, microseconds);
    mamaDateTimeImpl_setPrecision    ((mama_time_t*)dateTime, precision);
    mamaDateTimeImpl_setHasTime      ((mama_time_t*)dateTime);
    if (seconds > SECONDS_IN_A_DAY)
        mamaDateTimeImpl_setHasDate ((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_setEpochTimeF64(mamaDateTime dateTime,
                             mama_f64_t   seconds)
{
    if (!dateTime)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        double  wholeSeconds = floor(seconds+0.0000001);
        long    microseconds =
            (long) (1000000.0 * (seconds - wholeSeconds));
        mamaDateTimeImpl_clear           ((mama_time_t*)dateTime);
        mamaDateTimeImpl_setSeconds      ((mama_time_t*)dateTime, seconds);
        mamaDateTimeImpl_setMicroSeconds ((mama_time_t*)dateTime, microseconds);
        mamaDateTimeImpl_setHasTime      ((mama_time_t*)dateTime);
        if (seconds > SECONDS_IN_A_DAY)
            mamaDateTimeImpl_setHasDate ((mama_time_t*)dateTime);
        if (microseconds > 0)
        {
            mamaDateTimeImpl_setPrecision ((mama_time_t*)dateTime, 6);
        }
        else
        {
            mamaDateTimeImpl_setPrecision ((mama_time_t*)dateTime, 0);
        }
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaDateTime_setEpochTimeMilliseconds(mamaDateTime dateTime,
                                      mama_u64_t   milliseconds)
{
    mama_u64_t seconds = milliseconds / 1000;
    milliseconds %= 1000;
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_clear           ((mama_time_t*)dateTime);
    mamaDateTimeImpl_setSeconds      ((mama_time_t*)dateTime, seconds);
    mamaDateTimeImpl_setMicroSeconds ((mama_time_t*)dateTime, milliseconds * 1000);
    mamaDateTimeImpl_setHasTime      ((mama_time_t*)dateTime);
    if (seconds > SECONDS_IN_A_DAY)
        mamaDateTimeImpl_setHasDate ((mama_time_t*)dateTime);
    if (milliseconds > 0)
    {
        mamaDateTimeImpl_setPrecision ((mama_time_t*)dateTime, 3);
    }
    else
    {
        mamaDateTimeImpl_setPrecision ((mama_time_t*)dateTime, 0);
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_setEpochTimeMicroseconds(mamaDateTime dateTime,
                                      mama_u64_t   microseconds)
{
    mama_u64_t seconds = microseconds / 1000000;
    microseconds %= 1000000;

    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_clear           ((mama_time_t*)dateTime);
    mamaDateTimeImpl_setSeconds      ((mama_time_t*)dateTime, seconds);
    mamaDateTimeImpl_setMicroSeconds ((mama_time_t*)dateTime, microseconds);
    mamaDateTimeImpl_setHasTime      ((mama_time_t*)dateTime);
    if (seconds > SECONDS_IN_A_DAY)
        mamaDateTimeImpl_setHasDate ((mama_time_t*)dateTime);
    if (microseconds > 0)
    {
        mamaDateTimeImpl_setPrecision ((mama_time_t*)dateTime, 6);
    }
    else
    {
        mamaDateTimeImpl_setPrecision ((mama_time_t*)dateTime, 0);
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_setWithHints(mamaDateTime           dateTime,
                          mama_u32_t             seconds,
                          mama_u32_t             microseconds,
                          mamaDateTimePrecision  precision,
                          mamaDateTimeHints      hints)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_clear           ((mama_time_t*)dateTime);
    mamaDateTimeImpl_setSeconds      ((mama_time_t*)dateTime, seconds);
    mamaDateTimeImpl_setMicroSeconds ((mama_time_t*)dateTime, microseconds);
    mamaDateTimeImpl_setPrecision    ((mama_time_t*)dateTime, precision);
    mamaDateTimeImpl_setHasTime      ((mama_time_t*)dateTime);
    mamaDateTimeImpl_setHint         ((mama_time_t*)dateTime, hints);
    if (seconds > SECONDS_IN_A_DAY)
        mamaDateTimeImpl_setHasDate ((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_setPrecision(mamaDateTime           dateTime,
                          mamaDateTimePrecision  precision)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;
    mamaDateTimeImpl_setPrecision ((mama_time_t*)dateTime, precision);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_setFromString(mamaDateTime    dateTime,
                           const char*     str)
{
    return mamaDateTime_setFromStringWithTz (dateTime, str, NULL);
}

mama_status
mamaDateTime_setFromStringWithTz(mamaDateTime        dateTime,
                                 const char*         str,
                                 const mamaTimeZone  tz)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;
    if (!str)
        return mamaDateTime_clear (dateTime);
    else
    {
        const char* space   = strchr (str, ' ');
        const char* slash1  = strchr (str, '/');
        const char* slash2  = NULL;
        const char* strtime = space  ? space+1 : str;
        const char* colon1  = strchr (strtime, ':');

        unsigned long year      = 0;
        unsigned long month     = 0;
        unsigned long day       = 0;
        unsigned long hour      = 0;
        unsigned long minute    = 0;
        unsigned long second    = 0;
        unsigned long microsecs = 0;
        int           precision = 0;

        unsigned long epochSeconds = 0;

        if (slash1)
        {
            slash2 = strchr (slash1+1, '/');
        }
        else
        {
            slash1 = strchr (str, '-');
            slash2 = slash1 ? strchr (slash1+1, '-') : NULL;
        }

        mamaDateTimeImpl_clear ((mama_time_t*)dateTime);

        if (slash1)
        {
            if (slash2)
            {
                /* year/month/day is present */
                year  = strtoul (str,  NULL, 10);
                month = strtoul (slash1+1, NULL, 10);
                day   = strtoul (slash2+1, NULL, 10);
            }
            else
            {
                /* month/day is present */
                month = strtoul (str,  NULL, 10);
                day   = strtoul (slash1+1, NULL, 10);
            }
            mamaDateTimeImpl_setHasDate ((mama_time_t*)dateTime);
        }

        if (colon1)
        {
            /* hour:minute is present */
            const char* colon2 = strchr (colon1+1,  ':');
            hour   = strtoul (strtime,  NULL, 10);
            minute = strtoul (colon1+1, NULL, 10);
            if (colon2)
            {
                /* :second is present */
                const char* dot = strchr (colon2+1, '.');
                second = strtoul (colon2+1, NULL, 10);
                if (dot)
                {
                    /* .subsecond is present */
                    const char* ch = dot+1;
                    int i = 0;
                    for (i = 0; isdigit(*ch); i++,ch++)
                    {
                        microsecs = 10 * microsecs + (*ch - '0');
                    }
                    precision = i;
                    while (i++ < 6)
                    {
                        microsecs *= 10;
                    }
                }
            }
            mamaDateTimeImpl_setHasTime ((mama_time_t*)dateTime);
        }

        epochSeconds = makeTime (year, month, day, hour, minute, second);
        if (tz)
        {
            /* adjust to UTC */
            mama_i32_t  offset = 0;
            mama_status tzStatus = mamaTimeZone_getOffset (tz, &offset);
            if (MAMA_STATUS_OK != tzStatus)
                return tzStatus;
            epochSeconds -= offset;
        }

        mamaDateTimeImpl_setPrecision ((mama_time_t*)dateTime, precision);
        mamaDateTimeImpl_setSeconds ((mama_time_t*)dateTime, epochSeconds);
        mamaDateTimeImpl_setMicroSeconds ((mama_time_t*)dateTime, microsecs);

        return MAMA_STATUS_OK;
    }
}

mama_status
mamaDateTime_setFromStringBuffer(mamaDateTime dateTime,
                                 const char*  str,
                                 mama_size_t  strLen)
{
    return mamaDateTime_setFromStringBufferWithTz (dateTime, str, strLen, NULL);
}

mama_status
mamaDateTime_setFromStringBufferWithTz(mamaDateTime       dateTime,
                                       const char*        str,
                                       mama_size_t        strLen,
                                       const mamaTimeZone tz)
{
    mama_status status;
    char* tmpStr = (char*) malloc (strLen +1);
    if (!tmpStr)
        return MAMA_STATUS_NOMEM;

    strncpy (tmpStr, str, strLen);
    status = mamaDateTime_setFromStringWithTz (dateTime, tmpStr, tz);
    free (tmpStr);
    return status;
}

mama_status
mamaDateTime_setToNow(mamaDateTime dateTime)
{
    struct timeval now;
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_clear ((mama_time_t*)dateTime);
    if (gettimeofday (&now, NULL) != 0)
    {
        return MAMA_STATUS_SYSTEM_ERROR;
    }
    mamaDateTimeImpl_setSeconds      ((mama_time_t*)dateTime, now.tv_sec);
    mamaDateTimeImpl_setMicroSeconds ((mama_time_t*)dateTime, now.tv_usec);
    mamaDateTimeImpl_setPrecision    ((mama_time_t*)dateTime, 6);
    mamaDateTimeImpl_setHasDate      ((mama_time_t*)dateTime);
    mamaDateTimeImpl_setHasTime      ((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_setToMidnightToday(mamaDateTime       dateTime,
                                const mamaTimeZone tz)
{
    mama_u32_t secondsSinceEpoch = 0;
    mama_u32_t tmpSeconds = 0;
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_clear ((mama_time_t*)dateTime);
    secondsSinceEpoch = time(NULL);

    if (tz)
    {
        mama_i32_t  offset = 0;
        mamaTimeZone_getOffset (tz, &offset);
        secondsSinceEpoch += offset;
    }

    tmpSeconds = (secondsSinceEpoch / SECONDS_IN_A_DAY) * SECONDS_IN_A_DAY;
    mamaDateTimeImpl_setSeconds      ((mama_time_t*)dateTime, tmpSeconds);
    mamaDateTimeImpl_setMicroSeconds ((mama_time_t*)dateTime, 0);
    mamaDateTimeImpl_setPrecision    ((mama_time_t*)dateTime, 0);
    mamaDateTimeImpl_setHasDate      ((mama_time_t*)dateTime);
    mamaDateTimeImpl_setHasTime      ((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_setWithPrecisionAndTz(mamaDateTime           dateTime,
                                   mama_u32_t             year,
                                   mama_u32_t             month,
                                   mama_u32_t             day,
                                   mama_u32_t             hour,
                                   mama_u32_t             minute,
                                   mama_u32_t             second,
                                   mama_u32_t             microsecond,
                                   mamaDateTimePrecision  precision,
                                   const mamaTimeZone     tz)
{
    mama_status status = MAMA_STATUS_OK;
    if (MAMA_STATUS_OK !=
        (status = mamaDateTime_clear (dateTime)))
        return status;
    if (MAMA_STATUS_OK !=
        (status = mamaDateTime_setDate (dateTime, year, month, day)))
        return status;
    if (MAMA_STATUS_OK !=
        (status = mamaDateTime_setTimeWithPrecisionAndTz (
            dateTime, hour, minute, second, microsecond, precision, tz)))
        return status;
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_setTime(mamaDateTime  dateTime,
                     mama_u32_t    hour,
                     mama_u32_t    minute,
                     mama_u32_t    second,
                     mama_u32_t    microsecond)
{
    return mamaDateTime_setTimeWithPrecisionAndTz (
        dateTime, hour, minute, second, microsecond,
        MAMA_DATE_TIME_PREC_UNKNOWN, NULL);
}

mama_status
mamaDateTime_setTimeWithPrecisionAndTz(mamaDateTime           dateTime,
                                       mama_u32_t             hour,
                                       mama_u32_t             minute,
                                       mama_u32_t             second,
                                       mama_u32_t             microsecond,
                                       mamaDateTimePrecision  precision,
                                       const mamaTimeZone     tz)
{
    mama_i32_t  tmpSeconds = 0;
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    /* Get existing number of seconds and remove any intraday-seconds. */
    tmpSeconds = (mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) / SECONDS_IN_A_DAY) *
                                                  SECONDS_IN_A_DAY;
    tmpSeconds += second + (60 * (minute + (60 * hour)));
    if (tz)
    {
        /* Adjust for time zone offset. */
        mama_i32_t  offset = 0;
        mamaTimeZone_getOffset (tz, &offset);
        tmpSeconds -= offset;
    }
    mamaDateTimeImpl_setSeconds      ((mama_time_t*)dateTime, tmpSeconds);
    mamaDateTimeImpl_setMicroSeconds ((mama_time_t*)dateTime, microsecond);
    mamaDateTimeImpl_setPrecision    ((mama_time_t*)dateTime, precision);
    mamaDateTimeImpl_setHasTime      ((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_setDate(mamaDateTime dateTime,
                     mama_u32_t   year,
                     mama_u32_t   month,
                     mama_u32_t   day)
{
    mama_i64_t  tmpSeconds = 0;
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    /* Get existing number of seconds and remove any full-day seconds. */
    tmpSeconds = mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) % SECONDS_IN_A_DAY;
    tmpSeconds += makeTime (year, month, day, 0, 0, 0);
    mamaDateTimeImpl_setSeconds      ((mama_time_t*)dateTime, tmpSeconds);
    mamaDateTimeImpl_setHasDate      ((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_copyTime(mamaDateTime       dest,
                      const mamaDateTime src)
{
    mama_i64_t  tmpSeconds      = 0;
    long        tmpMicroseconds = 0;
    if (!dest || !src)
        return MAMA_STATUS_INVALID_ARG;

    /* Get existing number of seconds and remove any intraday-seconds. */
    tmpSeconds =  (mamaDateTimeImpl_getSeconds ((mama_time_t*)dest) / SECONDS_IN_A_DAY) *
                                                         SECONDS_IN_A_DAY;
    tmpSeconds += mamaDateTimeImpl_getSeconds ((const mama_time_t*)src)  % SECONDS_IN_A_DAY;
    tmpMicroseconds = mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)src);
    mamaDateTimeImpl_setSeconds      ((mama_time_t*)dest, tmpSeconds);
    mamaDateTimeImpl_setMicroSeconds ((mama_time_t*)dest, tmpMicroseconds);
    mamaDateTimeImpl_setHasTime      ((mama_time_t*)dest);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_copyDate(mamaDateTime       dest,
                      const mamaDateTime src)
{
    mama_i64_t  tmpSeconds = 0;
    if (!dest || !src)
        return MAMA_STATUS_INVALID_ARG;

    /* Get existing number of seconds and remove any full-day seconds. */
    tmpSeconds =  mamaDateTimeImpl_getSeconds ((mama_time_t*)dest) % SECONDS_IN_A_DAY;
    tmpSeconds += (mamaDateTimeImpl_getSeconds ((mama_time_t*)src)  / SECONDS_IN_A_DAY) *
                                                         SECONDS_IN_A_DAY;
    mamaDateTimeImpl_setSeconds      ((mama_time_t*)dest, tmpSeconds);
    mamaDateTimeImpl_setHasDate      ((mama_time_t*)dest);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_hasTime(const mamaDateTime dateTime,
                     mama_bool_t*       result)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;
    *result = mamaDateTimeImpl_getHasTime ((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_hasDate(const mamaDateTime dateTime,
                     mama_bool_t*       result)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;
    *result = mamaDateTimeImpl_getHasDate ((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_addSeconds(mamaDateTime dateTime,
                        mama_f64_t   addSeconds)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;
    if (addSeconds != 0)
    {
        double         wholeSeconds = floor(addSeconds+0.0000001);
        unsigned long  microseconds =
            (unsigned long) (1000000.0 * (addSeconds - wholeSeconds));

        mamaDateTime_addWholeSeconds (dateTime, wholeSeconds);
        mamaDateTime_addMicroseconds (dateTime, microseconds);
        mamaDateTimeImpl_setHasTime  ((mama_time_t*)dateTime);
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_addWholeSeconds(mamaDateTime dateTime,
                             mama_i32_t   addSeconds)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;
    if (addSeconds != 0)
    {
        mama_i64_t seconds = mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime);
        mamaDateTimeImpl_clearSeconds ((mama_time_t*)dateTime);
        mamaDateTimeImpl_setSeconds   ((mama_time_t*)dateTime, seconds + addSeconds);
        mamaDateTimeImpl_setHasTime   ((mama_time_t*)dateTime);
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_addMicroseconds(mamaDateTime dateTime,
                             mama_i64_t   addMicroseconds)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;
    if (addMicroseconds != 0)
    {
        /*Add to account for current microSeconds*/
        addMicroseconds += mamaDateTimeImpl_getMicroSeconds((mama_time_t*)dateTime);
        if (addMicroseconds >= 1000000)
        {
            mamaDateTime_addWholeSeconds (dateTime, addMicroseconds/1000000);
            addMicroseconds %= 1000000;
        }
        else if (addMicroseconds < 0)
        {
            if(addMicroseconds >= -1000000)
            {
                mamaDateTime_addWholeSeconds (dateTime, -1);
            }
            else
            {
                mamaDateTime_addWholeSeconds (dateTime, addMicroseconds/1000000 - 1);
            }
            addMicroseconds %= 1000000;
            if(addMicroseconds < 0) addMicroseconds += 1000000;
        }
        mamaDateTimeImpl_clearMicroSeconds ((mama_time_t*)dateTime);
        mamaDateTimeImpl_setMicroSeconds   ((mama_time_t*)dateTime, addMicroseconds);
        mamaDateTimeImpl_setHasTime        ((mama_time_t*)dateTime);
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_getEpochTime(const mamaDateTime     dateTime,
                          mama_u32_t*            seconds,
                          mama_u32_t*            microseconds,
                          mamaDateTimePrecision* precision)
{
    return mamaDateTime_getEpochTimeWithTz (dateTime, seconds, microseconds,
                                            precision, NULL);
}

mama_status
mamaDateTime_getEpochTimeWithTz(const mamaDateTime     dateTime,
                                mama_u32_t*            seconds,
                                mama_u32_t*            microseconds,
                                mamaDateTimePrecision* precision,
                                const mamaTimeZone     tz)
{
    if (!dateTime || !seconds || !microseconds)
        return MAMA_STATUS_INVALID_ARG;

    if (tz)
    {
        mama_i32_t  offset   = 0;
        mamaTimeZone_getOffset (tz, &offset);

        if (((mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) + offset) > UINT32_MAX) ||
            ((mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) + offset) < 0))
        {
            return MAMA_STATUS_INVALID_ARG;
        }

        *seconds = (mama_u32_t)mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) + offset;
    }
    else
    {
        if ((mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) > UINT32_MAX) ||
            (mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) < 0))
        {
            return MAMA_STATUS_INVALID_ARG;
        }

        *seconds = (mama_u32_t)mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime);
    }

    if ((mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime) > UINT32_MAX) ||
        (mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime) < 0))
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    *microseconds = (mama_u32_t)mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime);

    if (precision)
        *precision = mamaDateTimeImpl_getPrecision ((mama_time_t*)dateTime);

    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_getEpochTimeMicroseconds(const mamaDateTime dateTime,
                                      mama_u64_t*        microseconds)
{
    return mamaDateTime_getEpochTimeMicrosecondsWithTz (dateTime,
                                                        microseconds, NULL);
}

mama_status
mamaDateTime_getEpochTimeMicrosecondsWithTz(const mamaDateTime dateTime,
                                            mama_u64_t*        microseconds,
                                            const mamaTimeZone tz)
{
    if (!dateTime || !microseconds)
        return MAMA_STATUS_INVALID_ARG;

    if (tz)
    {
        mama_i32_t  offset   = 0;
        mamaTimeZone_getOffset (tz, &offset);

        *microseconds = 1000000 * ((mama_u64_t)mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) + offset) +
                            mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime);
    }
    else
    {
        *microseconds = 1000000 * (mama_u64_t)mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) +
                            mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_getEpochTimeMilliseconds(const mamaDateTime dateTime,
                                      mama_u64_t*        milliseconds)
{
    return mamaDateTime_getEpochTimeMillisecondsWithTz (dateTime, milliseconds,
                                                        NULL);
}

mama_status
mamaDateTime_getEpochTimeMillisecondsWithTz(const mamaDateTime dateTime,
                                            mama_u64_t*        milliseconds,
                                            const mamaTimeZone tz)
{
    if (!dateTime || !milliseconds)
        return MAMA_STATUS_INVALID_ARG;

    if (tz)
    {
        mama_i32_t  offset   = 0;
        mamaTimeZone_getOffset (tz, &offset);

        *milliseconds = 1000 * ((mama_u64_t)mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) + offset) +
                         mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime) / 1000;
    }
    else
    {
        *milliseconds = 1000 * (mama_u64_t)mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) +
                           mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime) / 1000;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_getEpochTimeSeconds(const mamaDateTime dateTime,
                                 mama_f64_t*        seconds)
{
    return mamaDateTime_getEpochTimeSecondsWithTz (dateTime, seconds, NULL);
}

mama_status mamaDateTime_addTodayToDateTime(mamaDateTime destination)
{
    mama_status ret = MAMA_STATUS_SYSTEM_ERROR;

    /* Create a new date time. */
    mama_time_t time;
    mamaDateTimeImpl_clear(&time);
    {
        /* Get the current time of day. */
        struct timeval now;
        memset(&now, 0, sizeof(now));
        if(gettimeofday(&now, NULL) == 0)
        {
            /* Initialise the time structure with the date. */
            mamaDateTimeImpl_setSeconds      (&time, now.tv_sec);
            mamaDateTimeImpl_setMicroSeconds (&time, now.tv_usec);
            mamaDateTimeImpl_setPrecision    (&time, 6);
            mamaDateTimeImpl_setHasDate      (&time);
            mamaDateTimeImpl_setHasTime      (&time);
            {
                /* Get existing number of seconds and remove any full-day seconds
                 * from the destination.
                 */
                mama_i64_t tmpSeconds =  mamaDateTimeImpl_getSeconds((mama_time_t*)destination) % SECONDS_IN_A_DAY;

                /* Append the date from today. */
                tmpSeconds += (mamaDateTimeImpl_getSeconds(&time) / SECONDS_IN_A_DAY) * SECONDS_IN_A_DAY;

                /* Set the result in the destination. */
                mamaDateTimeImpl_setSeconds((mama_time_t*)destination, tmpSeconds);
                mamaDateTimeImpl_setHasDate((mama_time_t*)destination);

                /* Function succeeded. */
                ret = MAMA_STATUS_OK;
            }
        }
    }

    return ret;
}

mama_status
mamaDateTime_getEpochTimeSecondsWithCheck(const mamaDateTime dateTime,
                                          mama_f64_t*        seconds)
{
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((dateTime != NULL) && (seconds != NULL))
    {
        /* Check to see if the date time has got a date value set. */
        uint8_t hasDate = mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime);

        /* No date value present, add today's date to the time value. */
        if(hasDate == 0)
        {
            ret = mamaDateTime_addTodayToDateTime(dateTime);
            if(ret == MAMA_STATUS_OK)
            {
                /* Get the seconds as normal. */
                ret = mamaDateTime_getEpochTimeSecondsWithTz(dateTime, seconds, NULL);
            }
        }

        /* Date value present, get the seconds as normal. */
        else
        {
            ret = mamaDateTime_getEpochTimeSecondsWithTz(dateTime, seconds, NULL);
        }
    }

    return ret;
}

mama_status
mamaDateTime_getEpochTimeSecondsWithTz(const mamaDateTime dateTime,
                                       mama_f64_t*        seconds,
                                       const mamaTimeZone tz)
{
    if (!dateTime || !seconds)
        return MAMA_STATUS_INVALID_ARG;

    if (tz)
    {
        mama_i32_t  offset   = 0;
        mamaTimeZone_getOffset (tz, &offset);

        *seconds = mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) + offset +
            ((double)mamaDateTimeImpl_getMicroSeconds((mama_time_t*)dateTime)) / 1000000.0;
    }
    else
    {
        *seconds = mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) +
        ((double)mamaDateTimeImpl_getMicroSeconds((mama_time_t*)dateTime)) / 1000000.0;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_getWithHints(const mamaDateTime     dateTime,
                          mama_u32_t*            seconds,
                          mama_u32_t*            microseconds,
                          mamaDateTimePrecision* precision,
                          mamaDateTimeHints*     hints)
{
    if (!dateTime || !seconds || !microseconds)
        return MAMA_STATUS_INVALID_ARG;

    if ((mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) > UINT32_MAX) ||
        (mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime) < 0))
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    *seconds = mamaDateTimeImpl_getSeconds ((mama_time_t*)dateTime);

    if ((mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime) > UINT32_MAX) ||
        (mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime) < 0))
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    *microseconds = mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime);

    if (precision)
        *precision = mamaDateTimeImpl_getPrecision   ((mama_time_t*)dateTime);
    if (hints)
        *hints = mamaDateTimeImpl_getHint            ((mama_time_t*)dateTime);
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_getStructTimeVal(const mamaDateTime dateTime,
                              struct timeval*    result)
{
    return mamaDateTime_getStructTimeValWithTz (dateTime, result, NULL);
}

mama_status
mamaDateTime_getStructTimeValWithTz(const mamaDateTime dateTime,
                                    struct timeval*    result,
                                    const mamaTimeZone tz)
{
    if (!dateTime || !result)
        return MAMA_STATUS_INVALID_ARG;

    result->tv_sec  = mamaDateTimeImpl_getSeconds      ((mama_time_t*)dateTime);
    result->tv_usec = mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime);

    if (tz)
    {
        mama_i32_t  offset   = 0;
        mamaTimeZone_getOffset (tz, &offset);
        result->tv_usec += offset;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_setFromStructTimeVal(const mamaDateTime dateTime,
                                  struct timeval*    inputTimeVal)
{
    if (!dateTime || !inputTimeVal)
        return MAMA_STATUS_INVALID_ARG;

    mamaDateTimeImpl_setSeconds         ((mama_time_t*)dateTime, inputTimeVal->tv_sec);
    mamaDateTimeImpl_setMicroSeconds    ((mama_time_t*)dateTime, inputTimeVal->tv_usec);
    mamaDateTimeImpl_setHasTime         ((mama_time_t*)dateTime);
    mamaDateTimeImpl_setHasDate         ((mama_time_t*)dateTime);

    if (inputTimeVal->tv_usec > 0)
    {
        mamaDateTimeImpl_setPrecision ((mama_time_t*)dateTime, 6);
    }
    else
    {
        mamaDateTimeImpl_setPrecision ((mama_time_t*)dateTime, 0);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_getStructTm(const mamaDateTime dateTime,
                         struct tm*         result)
{
    return mamaDateTime_getStructTmWithTz (dateTime, result, NULL);
}

mama_status
mamaDateTime_getStructTmWithTz(const mamaDateTime dateTime,
                               struct tm*         result,
                               const mamaTimeZone tz)
{
    if (!dateTime || !result)
        return MAMA_STATUS_INVALID_ARG;

    if (tz)
    {
        mama_i32_t  offset   = 0;
        mamaTimeZone_getOffset (tz, &offset);
        utcTm (result, (time_t)mamaDateTimeImpl_getSeconds((mama_time_t*)dateTime) + offset);
    }
    else
    {
         utcTm (result, (time_t)mamaDateTimeImpl_getSeconds((mama_time_t*)dateTime));
    }

    return MAMA_STATUS_OK;
}

mama_status mamaDateTime_getAsString (const mamaDateTime dateTime,
                                      char*              buf,
                                      mama_size_t        bufMaxLen)
{
    time_t          seconds;
    struct tm       tmValue;
    mama_time_t*    aDateTime   = (mama_time_t*)dateTime;

    if (!aDateTime || !buf)
        return MAMA_STATUS_INVALID_ARG;

    seconds = (time_t) mamaDateTimeImpl_getSeconds(aDateTime);
    utcTm (&tmValue, seconds);
    buf[0] = '\0';
    if (mamaDateTimeImpl_getHasTime (aDateTime))
    {
        size_t    bytesUsed = 0;
        size_t    precision = 0;
        if (mamaDateTimeImpl_getHasDate(aDateTime))
        {
            bytesUsed = strftime (buf, bufMaxLen, "%Y-%m-%d %H:%M:%S", &tmValue);
        }
        else
        {
            bytesUsed = strftime (buf, bufMaxLen, "%H:%M:%S", &tmValue);
        }
        if (bytesUsed > 0)
        {
            buf       += bytesUsed;
            bufMaxLen -= bytesUsed;
        }
        precision = mamaDateTimeImpl_getPrecision(aDateTime);
        /* for unknown we print all the digits */
        precision = precision == MAMA_DATE_TIME_PREC_UNKNOWN ? 3 : precision;
        if (precision > 0)
        {
            size_t i;
            long digits = mamaDateTimeImpl_getMicroSeconds(aDateTime);
            for (i = 6; i > precision; i--)
            {
                digits /= 10;
            }
            if (precision >= bufMaxLen)
            {
                precision = bufMaxLen-1;
                mama_log (MAMA_LOG_LEVEL_WARN,
                "mamaDateTime_getAsString: percision truncated by buffer");
            }
            snprintf (buf, bufMaxLen, ".%0*d", (int)precision, digits);
        }
    }
    else if (mamaDateTimeImpl_getHasDate(aDateTime))
    {
        strftime (buf, bufMaxLen, "%Y-%m-%d", &tmValue);
    }
    return MAMA_STATUS_OK;
}

mama_status mamaDateTime_getTimeAsString (const mamaDateTime dateTime,
                                          char*              buf,
                                          mama_size_t        bufMaxLen)
{
    time_t    seconds;
    struct tm tmValue;
    size_t    bytesUsed;
    size_t    precision;

    if (!dateTime || !buf)
        return MAMA_STATUS_INVALID_ARG;

    seconds = (time_t) mamaDateTimeImpl_getSeconds((mama_time_t*)dateTime);
    utcTm (&tmValue, seconds);
    buf[0] = '\0';
    if (mamaDateTimeImpl_getHasTime((mama_time_t*)dateTime))
    {
        bytesUsed = strftime (buf, bufMaxLen, "%H:%M:%S", &tmValue);
        if (bytesUsed > 0)
        {
            buf       += bytesUsed;
            bufMaxLen -= bytesUsed;
        }
        precision = mamaDateTimeImpl_getPrecision((mama_time_t*)dateTime);
        /* for unknown we print all the digits */
        precision = precision == MAMA_DATE_TIME_PREC_UNKNOWN ? 3 : precision;
        if (precision > 0)
        {
            size_t i;
            uint32_t digits = mamaDateTimeImpl_getMicroSeconds((mama_time_t*)dateTime);
            for (i = 6; i > precision; i--)
            {
                digits /= 10;
            }
            snprintf (buf, bufMaxLen, ".%0*d", (int)precision, digits);
        }
    }
    return MAMA_STATUS_OK;
}

mama_status mamaDateTime_getDateAsString (const mamaDateTime dateTime,
                                          char*              buf,
                                          mama_size_t        bufMaxLen)
{
    time_t    seconds;
    struct tm tmValue;

    if (!dateTime || !buf)
        return MAMA_STATUS_INVALID_ARG;

    seconds = (time_t) mamaDateTimeImpl_getSeconds((mama_time_t*)dateTime);
    utcTm (&tmValue, seconds);
    buf[0] = '\0';
    if (mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime))
    {
        strftime (buf, bufMaxLen, "%Y-%m-%d", &tmValue);
    }
    return MAMA_STATUS_OK;
}


/* There may be a way to get the month names from a locale. */
static const char* gMonthsAbbrev[] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char* gMonthsAbbrevAllCap[] =
{
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

static const char* gMonthsFull[] =
{
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};


mama_status
mamaDateTime_getAsFormattedString (const mamaDateTime dateTime,
                                   char*              result,
                                   mama_size_t        maxLen,
                                   const char*        fmt)
{
    return mamaDateTime_getAsFormattedStringWithTz (
        dateTime, result, maxLen, fmt, mamaTimeZone_utc());
}

mama_status
mamaDateTime_getAsFormattedStringWithTz (const mamaDateTime dateTime,
                                         char*              result,
                                         mama_size_t        maxLen,
                                         const char*        fmt,
                                         const mamaTimeZone tz)
{
    struct tm    tmTime;
    const char*  fmtChar  = NULL;
    mama_size_t  resLen   = 0;
    mama_i32_t   offset   = 0;

    if (!dateTime || !result)
        return MAMA_STATUS_INVALID_ARG;

    if (tz)
    {
        mamaTimeZone_getOffset (tz, &offset);
    }

    if (offset != 0)
    {
        mamaDateTime  tmpDateTime = (mama_time_t*)dateTime;
        mamaDateTime_addWholeSeconds (tmpDateTime, offset);
        utcTm (&tmTime, (time_t)mamaDateTimeImpl_getSeconds((mama_time_t*)tmpDateTime));
    }
    else
    {
        utcTm (&tmTime, (time_t)mamaDateTimeImpl_getSeconds((mama_time_t*)dateTime));
    }

    for (fmtChar = fmt;
         *fmtChar && (resLen < maxLen);
         ++fmtChar)
    {
        if (*fmtChar != '%')
        {
            /* Copy the character directly from the format string to
             * the result */
            result[resLen++] = *fmtChar;
            continue;
        }
        /* Handle a % (possible special format string) */
        switch (fmtChar[1])
        {
        case 'Y':
        {
            /* %Y = 4-digit year */
            if (!mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "    ");
            }
            else
            {
                mama_u32_t year = tmTime.tm_year+1900;
                printDigit (result, &resLen, maxLen, year/1000); year %= 1000;
                printDigit (result, &resLen, maxLen, year/100);  year %= 100;
                printDigit (result, &resLen, maxLen, year/10);   year %= 10;
                printDigit (result, &resLen, maxLen, year);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'y':
        {
            /* %y = 2-digit year */
            if (!mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "  ");
            }
            else
            {
                mama_u32_t year = tmTime.tm_year % 100;
                printDigit (result, &resLen, maxLen, year/10);   year %= 10;
                printDigit (result, &resLen, maxLen, year);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'm':
        {
            /* %m = 2-digit month */
            if (!mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "  ");
            }
            else
            {
                mama_u32_t month = tmTime.tm_mon+1;
                printDigit (result, &resLen, maxLen, month/10);  month %= 10;
                printDigit (result, &resLen, maxLen, month);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'd':
        {
            /* %m = 2-digit day of month */
            if (!mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "  ");
            }
            else
            {
                mama_u32_t mday = tmTime.tm_mday;
                printDigit (result, &resLen, maxLen, mday/10);   mday %= 10;
                printDigit (result, &resLen, maxLen, mday);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'F':
        {
            /* %F = %Y-%m-%d */
            if (!mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "    -  -  ");
            }
            else
            {
                mama_u32_t year   = tmTime.tm_year + 1900;
                mama_u32_t month  = tmTime.tm_mon  + 1;
                mama_u32_t mday   = tmTime.tm_mday;
                printDigit (result, &resLen, maxLen, year/1000); year %= 1000;
                printDigit (result, &resLen, maxLen, year/100);  year %= 100;
                printDigit (result, &resLen, maxLen, year/10);   year %= 10;
                printDigit (result, &resLen, maxLen, year);
                printChar  (result, &resLen, maxLen, '-');
                printDigit (result, &resLen, maxLen, month/10);  month %= 10;
                printDigit (result, &resLen, maxLen, month);
                printChar  (result, &resLen, maxLen, '-');
                printDigit (result, &resLen, maxLen, mday/10);   mday %= 10;
                printDigit (result, &resLen, maxLen, mday);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'T':
        {
            /* %T = %H:%M:%S */
            if (!mamaDateTimeImpl_getHasTime((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "  :  :  ");
            }
            else
            {
                mama_u32_t hour   = tmTime.tm_hour;
                mama_u32_t minute = tmTime.tm_min;
                mama_u32_t second = tmTime.tm_sec;
                printDigit (result, &resLen, maxLen, hour/10);   hour %= 10;
                printDigit (result, &resLen, maxLen, hour);
                printChar  (result, &resLen, maxLen, ':');
                printDigit (result, &resLen, maxLen, minute/10); minute %= 10;
                printDigit (result, &resLen, maxLen, minute);
                printChar  (result, &resLen, maxLen, ':');
                printDigit (result, &resLen, maxLen, second/10); second %= 10;
                printDigit (result, &resLen, maxLen, second);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'D':
        {
            /* %D = %m/%d/%y */
            if (!mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "    -  -  ");
            }
            else
            {
                mama_u32_t year   = tmTime.tm_year;
                mama_u32_t month  = tmTime.tm_mon  + 1;
                mama_u32_t mday   = tmTime.tm_mday;
                printDigit (result, &resLen, maxLen, month/10);  month %= 10;
                printDigit (result, &resLen, maxLen, month);
                printChar  (result, &resLen, maxLen, '/');
                printDigit (result, &resLen, maxLen, mday/10);   mday %= 10;
                printDigit (result, &resLen, maxLen, mday);
                printChar  (result, &resLen, maxLen, '/');
                printDigit (result, &resLen, maxLen, year/10);   year %= 10;
                printDigit (result, &resLen, maxLen, year);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'H':
        {
            /* %H = 2-digit hour (24-hour clock) */
            if (!mamaDateTimeImpl_getHasTime((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "  ");
            }
            else
            {
                mama_u32_t hour = tmTime.tm_hour;
                printDigit (result, &resLen, maxLen, hour/10);   hour %= 10;
                printDigit (result, &resLen, maxLen, hour);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'M':
        {
            /* %M = 2-digit minute */
            if (!mamaDateTimeImpl_getHasTime((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "  ");
            }
            else
            {
                mama_u32_t minute = tmTime.tm_min;
                printDigit (result, &resLen, maxLen, minute/10); minute %= 10;
                printDigit (result, &resLen, maxLen, minute);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'S':
        {
            /* %H = 2-digit second */
            if (!mamaDateTimeImpl_getHasTime((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "  ");
            }
            else
            {
                mama_u32_t second = tmTime.tm_sec;
                printDigit (result, &resLen, maxLen, second/10); second %= 10;
                printDigit (result, &resLen, maxLen, second);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case ':':
        {
            /* %: = subsecond */
            if (!mamaDateTimeImpl_getHasTime((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "   ");
            }
            else
            {
                long us = mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime);
                printSubseconds (result, &resLen, maxLen,
                                 MAMA_DATE_TIME_PREC_MILLISECONDS, us);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case ';':
        {
            /* %; = (if subsec > 0) add dot and subsecond */
            long us = mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime);
            if (mamaDateTimeImpl_getHasTime((mama_time_t*)dateTime) && (us > 0))
            {
                if (resLen < maxLen)
                {
                    result[resLen++] = '.';
                }
                printSubseconds (result, &resLen, maxLen,
                                 MAMA_DATE_TIME_PREC_MILLISECONDS, us);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case '.':
        {
            /* %: = subsecond */
            if (!mamaDateTimeImpl_getHasTime((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "   ");
            }
            else
            {
                long us = mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime);
                printSubseconds (result, &resLen, maxLen,
                                 mamaDateTimeImpl_getPrecision ((mama_time_t*)dateTime), us);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case ',':
        {
            /* %; = (if subsec > 0) add dot and subsecond */
            long us = mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime);
            if (mamaDateTimeImpl_getHasTime((mama_time_t*)dateTime) && (us > 0))
            {
                if (resLen < maxLen)
                {
                    result[resLen++] = '.';
                }
                printSubseconds (result, &resLen, maxLen,
                                 mamaDateTimeImpl_getPrecision ((mama_time_t*)dateTime), us);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'B':
        {
            /* %B = full month (first letter capitalized) */
            if (!mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "     ");
            }
            else
            {
                const char* month = gMonthsFull[tmTime.tm_mon];
                printString (result, &resLen, maxLen, month);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'b':
        {
            /* %b = 3-char abbreviated month (first letter capitalized) */
            if (!mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "   ");
            }
            else
            {
                const char* month = gMonthsAbbrev[tmTime.tm_mon];
                printString (result, &resLen, maxLen, month);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case 'h':
        {
            /* %h = 3-char abbreviated month (all letters capitalized) */
            if (!mamaDateTimeImpl_getHasDate((mama_time_t*)dateTime))
            {
                printString (result, &resLen, maxLen, "   ");
            }
            else
            {
                const char* month = gMonthsAbbrevAllCap[tmTime.tm_mon];
                printString (result, &resLen, maxLen, month);
            }
            ++fmtChar; /* skip the extra character in the fmt */
            break;
        }
        case '\0':
            /* Format string ended with a single % */
            result[resLen++] = '%';
        default:
            /* % char followed by a non-special format; copy the % */
            result[resLen++] = '%';
            break;
        }
    }

    /* Only add a string terminator if we haven't already hit the end
     * of string */
    if (resLen < maxLen)
        result[resLen] = '\0';
    else
        result[maxLen-1] = '\0';

    return MAMA_STATUS_OK;
}


mama_status
mamaDateTime_getYear(const mamaDateTime dateTime,
                     mama_u32_t*        result)
{
    struct tm  stm;
    mama_status status = mamaDateTime_getStructTm(dateTime,&stm);
    if (MAMA_STATUS_OK == status)
    {
        *result = stm.tm_year + 1900;
    }
    return status;
}

mama_status
mamaDateTime_getMonth(const mamaDateTime dateTime,
                      mama_u32_t*        result)
{
    struct tm  stm;
    mama_status status = mamaDateTime_getStructTm(dateTime,&stm);
    if (MAMA_STATUS_OK == status)
    {
        *result = stm.tm_mon + 1;
    }
    return status;
}

mama_status
mamaDateTime_getDay(const mamaDateTime dateTime,
                    mama_u32_t*        result)
{
    struct tm  stm;
    mama_status status = mamaDateTime_getStructTm(dateTime,&stm);
    if (MAMA_STATUS_OK == status)
    {
        *result = stm.tm_mday;
    }
    return status;
}

mama_status
mamaDateTime_getHour(const mamaDateTime dateTime,
                     mama_u32_t*        result)
{
    struct tm  stm;
    mama_status status = mamaDateTime_getStructTm(dateTime,&stm);
    if (MAMA_STATUS_OK == status)
    {
        *result = stm.tm_hour;
    }
    return status;
}

mama_status
mamaDateTime_getMinute(const mamaDateTime dateTime,
                       mama_u32_t*        result)
{
    struct tm  stm;
    mama_status status = mamaDateTime_getStructTm(dateTime,&stm);
    if (MAMA_STATUS_OK == status)
    {
        *result = stm.tm_min;
    }
    return status;
}

mama_status
mamaDateTime_getSecond(const mamaDateTime dateTime,
                       mama_u32_t*        result)
{
    struct tm  stm;
    mama_status status = mamaDateTime_getStructTm(dateTime,&stm);
    if (MAMA_STATUS_OK == status)
    {
        *result = stm.tm_sec;
    }
    return status;
}

mama_status
mamaDateTime_getMicrosecond(const mamaDateTime dateTime,
                            mama_u32_t*        result)
{
    if (!dateTime)
        return MAMA_STATUS_INVALID_ARG;

    if (((mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime)) > UINT32_MAX) ||
        ((mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime)) < 0))
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    *result = mamaDateTimeImpl_getMicroSeconds ((mama_time_t*)dateTime);

    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_getDayOfWeek(const mamaDateTime dateTime,
                          mamaDayOfWeek*     result)
{
    struct tm  stm;
    mama_status status = mamaDateTime_getStructTm(dateTime,&stm);
    if (MAMA_STATUS_OK == status)
    {
        *result = stm.tm_wday;
    }
    return status;
}


mama_status
mamaDateTime_diffSeconds(const mamaDateTime t1,
                         const mamaDateTime t0,
                         mama_f64_t*        result)
{
    if (!t1 || !t0 || !result)
        return MAMA_STATUS_INVALID_ARG;

    *result = difftime (mamaDateTimeImpl_getSeconds((mama_time_t*)t1), mamaDateTimeImpl_getSeconds((mama_time_t*)t0)) +
              ((double)mamaDateTimeImpl_getMicroSeconds((mama_time_t*)t1) -
               (double)mamaDateTimeImpl_getMicroSeconds((mama_time_t*)t0)) / 1000000.0;
    return MAMA_STATUS_OK;
}

#define T24_HOURS   86400

mama_status
mamaDateTime_diffSecondsSameDay(const mamaDateTime t1,
                                const mamaDateTime t0,
                                mama_f64_t*        result)
{
    if (!t1 || !t0 || !result)
        return MAMA_STATUS_INVALID_ARG;

    *result = difftime ((mamaDateTimeImpl_getSeconds((mama_time_t*)t1) % T24_HOURS),
                        mamaDateTimeImpl_getSeconds((mama_time_t*)t0) % T24_HOURS) +
              ((double)mamaDateTimeImpl_getMicroSeconds((mama_time_t*)t1) -
               (double)mamaDateTimeImpl_getMicroSeconds((mama_time_t*)t0)) / 1000000.0;
    return MAMA_STATUS_OK;
}

mama_status
mamaDateTime_diffMicroseconds(const mamaDateTime t1,
                              const mamaDateTime t0,
                              mama_i64_t*        result)
{
    mama_u64_t  t0_msecs = 0;
    mama_u64_t  t1_msecs = 0;
    mama_status status   = MAMA_STATUS_OK;

    if (!t1 || !t0 || !result)
        return MAMA_STATUS_INVALID_ARG;

    if (MAMA_STATUS_OK !=
        (status = mamaDateTime_getEpochTimeMicroseconds(t0,&t0_msecs)))
    {
        return status;
    }
    if (MAMA_STATUS_OK !=
        (status = mamaDateTime_getEpochTimeMicroseconds(t1,&t1_msecs)))
    {
        return status;
    }

    *result = (mama_i64_t)t1_msecs - (mama_i64_t)t0_msecs;
    return MAMA_STATUS_OK;
}


unsigned long  makeTime (
    int year,
    int mon,
    int day,
    int hour,
    int min,
    int sec)
{
    struct tm timeInfo;

    /* tm_year stores the years since 1900
     * must not be less than 70 */
    if (year > 1970)
        timeInfo.tm_year  = year - 1900;
    else
        timeInfo.tm_year  = 70;

    /* tm_mon stores the months since Jan (0 to 11) */
    if (mon > 0)
        timeInfo.tm_mon   = mon - 1;
    else
        timeInfo.tm_mon   = 0;

    /* day of the month (1 to 31) */
    if (day > 0)
        timeInfo.tm_mday = day;
    else
        timeInfo.tm_mday = 1;

    timeInfo.tm_hour  = hour;
    timeInfo.tm_min   = min;
    timeInfo.tm_sec   = sec;
    timeInfo.tm_isdst = 0;

    return wtimegm (&timeInfo);
}

void utcTm (
    struct tm*     result,
    time_t         secSinceEpoch)
{
    const time_t tmp = secSinceEpoch;
    gmtime_r (&tmp, result);
}

static void
printSubseconds (char*                  result,
                 mama_size_t*           resLen,
                 mama_size_t            maxLen,
                 mamaDateTimePrecision  precision,
                 long                   microsecs)
{
    if (precision == MAMA_DATE_TIME_PREC_UNKNOWN)
        precision = MAMA_DATE_TIME_PREC_MILLISECONDS;

    switch (precision)
    {
    case MAMA_DATE_TIME_PREC_MICROSECONDS:
        printDigit (result, resLen, maxLen, microsecs/100000);  microsecs %= 100000;
        printDigit (result, resLen, maxLen, microsecs/10000);   microsecs %= 10000;
        printDigit (result, resLen, maxLen, microsecs/1000);    microsecs %= 1000;
        printDigit (result, resLen, maxLen, microsecs/100);     microsecs %= 100;
        printDigit (result, resLen, maxLen, microsecs/10);      microsecs %= 10;
        printDigit (result, resLen, maxLen, microsecs);
        break;
    case MAMA_DATE_TIME_PREC_MILLISECONDS:
        microsecs /= 1000;
        printDigit (result, resLen, maxLen, microsecs/100); microsecs %= 100;
        printDigit (result, resLen, maxLen, microsecs/10);  microsecs %= 10;
        printDigit (result, resLen, maxLen, microsecs);
        break;
    case MAMA_DATE_TIME_PREC_CENTISECONDS:
        microsecs /= 100;
        printDigit (result, resLen, maxLen, microsecs/10);  microsecs %= 10;
        printDigit (result, resLen, maxLen, microsecs);
        break;
    case MAMA_DATE_TIME_PREC_DECISECONDS:
        microsecs /= 10;
        printDigit (result, resLen, maxLen, microsecs);
        break;
    case MAMA_DATE_TIME_PREC_SECONDS:
    case MAMA_DATE_TIME_PREC_MINUTES:
    case MAMA_DATE_TIME_PREC_DAYS:
    case MAMA_DATE_TIME_PREC_UNKNOWN:
        break;
    }
}
