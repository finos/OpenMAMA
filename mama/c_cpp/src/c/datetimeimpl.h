/* $Id: datetimeimpl.h,v 1.1.2.2 2011/09/01 16:34:37 emmapollock Exp $
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

typedef uint64_t mama_time_t;


#define MAMA_TIME_IMPL_NULL               ((uint64_t)0x0ULL)
#define MAMA_TIME_IMPL_MASK_SECONDS       ((uint64_t)0xffffffff00000000ULL)
#define MAMA_TIME_IMPL_MASK_MICROSECONDS  ((uint64_t)0x00000000000fffffULL)
#define MAMA_TIME_IMPL_MASK_PRECISION     ((uint64_t)0x00000000f0000000ULL)
#define MAMA_TIME_IMPL_BIT_HAS_DATE       ((uint64_t)0x0000000001000000ULL)
#define MAMA_TIME_IMPL_BIT_HAS_TIME       ((uint64_t)0x0000000002000000ULL)
#define MAMA_TIME_IMPL_MASK_TIME_ONLY     ((uint64_t)0xffffffff000fffffULL)

#define mamaDateTimeImpl_clear(t)             ((t) =  MAMA_TIME_IMPL_NULL)
#define mamaDateTimeImpl_copy(d,s)            ((d) =  (s))
#define mamaDateTimeImpl_empty(t)             ((t) == MAMA_TIME_IMPL_NULL)
#define mamaDateTimeImpl_equal(l,r)           ((l) == (r))

#define mamaDateTimeImpl_clearSeconds(t) \
      ((t) &= ~MAMA_TIME_IMPL_MASK_SECONDS)
#define mamaDateTimeImpl_clearMicroSeconds(t) \
      ((t) &= ~MAMA_TIME_IMPL_MASK_MICROSECONDS)
#define mamaDateTimeImpl_clearPrecision(t) \
      ((t) &= ~MAMA_TIME_IMPL_MASK_PRECISION)
#define mamaDateTimeImpl_clearHasDate(t) \
      ((t) &= ~MAMA_TIME_IMPL_BIT_HAS_DATE)
#define mamaDateTimeImpl_clearHasTime(t) \
      ((t) &= ~MAMA_TIME_IMPL_BIT_HAS_TIME)

#define mamaDateTimeImpl_getSeconds(t) \
      (uint32_t) (((t) & MAMA_TIME_IMPL_MASK_SECONDS)   >> 32)
#define mamaDateTimeImpl_getMicroSeconds(t) \
      (uint32_t) ((t) & MAMA_TIME_IMPL_MASK_MICROSECONDS)
#define mamaDateTimeImpl_getPrecision(t) \
      (uint32_t) (((t) & MAMA_TIME_IMPL_MASK_PRECISION) >> 28)
#define mamaDateTimeImpl_getHasDate(t) \
      (uint8_t) (((t) & MAMA_TIME_IMPL_BIT_HAS_DATE) >> 24)
#define mamaDateTimeImpl_getHasTime(t) \
      (uint8_t) (((t) & MAMA_TIME_IMPL_BIT_HAS_TIME) >> 24)
#define mamaDateTimeImpl_getTimeOnly(t) \
      ((t) & MAMA_TIME_IMPL_MASK_TIME_ONLY)

#define mamaDateTimeImpl_setSeconds(t,s) \
      ((t) = ((t) & ~MAMA_TIME_IMPL_MASK_SECONDS)      | ((uint64_t)(s) << 32))
#define mamaDateTimeImpl_setMicroSeconds(t,us) \
      ((t) = ((t) & ~MAMA_TIME_IMPL_MASK_MICROSECONDS) | ((uint64_t)(us)))
#define mamaDateTimeImpl_setPrecision(t,p) \
      ((t) = ((t) & ~MAMA_TIME_IMPL_MASK_PRECISION)    | ((uint64_t)(p) << 28))
#define mamaDateTimeImpl_setHasDate(t) \
      ((t) |= MAMA_TIME_IMPL_BIT_HAS_DATE)
#define mamaDateTimeImpl_setHasTime(t) \
      ((t) |= MAMA_TIME_IMPL_BIT_HAS_TIME)

#define MAMA_TIME_IMPL_SECONDS_IN_DAY  (24*60*60)

#define mamaDateTimeImpl_clearDate(t) \
      (mamaDateTimeImpl_setSeconds(t,mamaDateTimeImpl_getSeconds(t)%MAMA_TIME_IMPL_SECONDS_IN_DAY), \
       mamaDateTimeImpl_clearHasDate(t))
#define mamaDateTimeImpl_clearTime(t) \
      (mamaDateTimeImpl_setSeconds(t,(mamaDateTimeImpl_getSeconds(t)/MAMA_TIME_IMPL_SECONDS_IN_DAY)*MAMA_TIME_IMPL_SECONDS_IN_DAY), \
       mamaDateTimeImpl_setMicroSeconds(t,0), \
       mamaDateTimeImpl_clearHasTime(t))

#if defined(__cplusplus)
}
#endif

#endif /* MamaDateTimeImplH__ */
