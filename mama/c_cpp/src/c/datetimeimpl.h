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

#ifndef MamaDateTimeImplH__
#define MamaDateTimeImplH__

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct mama_datetime_t_
{
#if defined(__i386__) && (defined(unix) || defined(__unix__) || defined(__unix))
    int64_t                 mSeconds;
#else
    time_t                  mSeconds;
#endif
    long                    mNanoseconds;
    mamaDateTimePrecision   mPrecision;
    mamaDateTimeHints       mHints;
} mama_datetime_t;


#define mamaDateTimeImpl_clear(t) \
    ((t)->mSeconds = 0, (t)->mNanoseconds = 0, (t)->mPrecision = MAMA_DATE_TIME_PREC_UNKNOWN, (t)->mHints = 0)

#define mamaDateTimeImpl_copy(d,s) \
    ((d)->mSeconds = (s)->mSeconds, (d)->mNanoseconds = (s)->mNanoseconds, (d)->mPrecision = (s)->mPrecision, (d)->mHints = (s)->mHints)

#define mamaDateTimeImpl_empty(t) \
    ((t)->mSeconds == 0, (t)->mNanoseconds == 0, (t)->mPrecision == MAMA_DATE_TIME_PREC_UNKNOWN, (t)->mHints == 0)

#define mamaDateTimeImpl_equal(l,r) \
    ((l)->mSeconds == (r)->mSeconds && (l)->mNanoseconds == (r)->mNanoseconds)


#define mamaDateTimeImpl_clearSeconds(t) \
    ((t)->mSeconds = 0)
#define mamaDateTimeImpl_clearMicroSeconds(t) \
    ((t)->mNanoseconds = 0)
#define mamaDateTimeImpl_clearPrecision(t) \
    ((t)->mPrecision = MAMA_DATE_TIME_PREC_UNKNOWN)
#define mamaDateTimeImpl_clearHint(t) \
    ((t)->mHints = 0)

#define mamaDateTimeImpl_clearHasDate(t) \
    ((t)->mHints &= ~MAMA_DATE_TIME_HAS_DATE)
#define mamaDateTimeImpl_clearHasTime(t) \
    ((t)->mHints &= ~MAMA_DATE_TIME_HAS_TIME)
#define mamaDateTimeImpl_clearNoTimezone(t) \
    ((t)->mHints &= ~MAMA_DATE_TIME_NO_TIMEZONE)


#define mamaDateTimeImpl_getSeconds(t)      ((t)->mSeconds)

#define mamaDateTimeImpl_getMicroSeconds(t) ((t)->mNanoseconds / 1000)

#define mamaDateTimeImpl_getNanoSeconds(t)  ((t)->mNanoseconds)

#define mamaDateTimeImpl_getPrecision(t)    ((t)->mPrecision)

#define mamaDateTimeImpl_getHint(t)         ((t)->mHints)

#define mamaDateTimeImpl_getHasDate(t) \
      (((t)->mHints & MAMA_DATE_TIME_HAS_DATE) == MAMA_DATE_TIME_HAS_DATE)
#define mamaDateTimeImpl_getHasTime(t) \
      (((t)->mHints & MAMA_DATE_TIME_HAS_TIME) == MAMA_DATE_TIME_HAS_TIME)
#define mamaDateTimeImpl_getNoTimezone(t) \
      (((t)->mHints & MAMA_DATE_TIME_NO_TIMEZONE) == MAMA_DATE_TIME_NO_TIMEZONE)


#define mamaDateTimeImpl_setSeconds(t,s) \
      ((t)->mSeconds = (mama_i64_t)(s))
#define mamaDateTimeImpl_setMicroSeconds(t,us) \
      ((t)->mNanoseconds = (long)(us) * 1000)
#define mamaDateTimeImpl_setNanoSeconds(t,ns) \
      ((t)->mNanoseconds = (long)(ns))
#define mamaDateTimeImpl_setPrecision(t,p) \
      ((t)->mPrecision = (p))
#define mamaDateTimeImpl_setHint(t,p) \
      ((t)->mHints = (p))


#define mamaDateTimeImpl_setHasDate(t) \
      ((t)->mHints |= MAMA_DATE_TIME_HAS_DATE)
#define mamaDateTimeImpl_setHasTime(t) \
      ((t)->mHints |= MAMA_DATE_TIME_HAS_TIME)
#define mamaDateTimeImpl_setNoTimezone(t) \
      ((t)->mHints |= MAMA_DATE_TIME_NO_TIMEZONE)


#define MAMA_TIME_IMPL_SECONDS_IN_DAY  (24*60*60)

#define mamaDateTimeImpl_clearDate(t) \
      (mamaDateTimeImpl_setSeconds(t,mamaDateTimeImpl_getSeconds(t)%MAMA_TIME_IMPL_SECONDS_IN_DAY), \
       mamaDateTimeImpl_clearHasDate(t))
#define mamaDateTimeImpl_clearTime(t) \
      (mamaDateTimeImpl_setSeconds(t,(mamaDateTimeImpl_getSeconds(t)/MAMA_TIME_IMPL_SECONDS_IN_DAY)*MAMA_TIME_IMPL_SECONDS_IN_DAY), \
       mamaDateTimeImpl_setMicroSeconds(t,0), \
       mamaDateTimeImpl_clearHasTime(t))

#define mamaDateTimeImpl_compare(l,r) \
    do { \
        if ((l)->mSeconds > (r)->mSeconds) \
            return 1; \
        else if (((l)->mSeconds == (r)->mSeconds) && ((l)->mNanoseconds == (r)->mNanoseconds)) \
            return 0; \
        else \
            return -1; \
    } while (0)

#if defined(__cplusplus)
}
#endif

#endif /* MamaDateTimeImplH__ */
