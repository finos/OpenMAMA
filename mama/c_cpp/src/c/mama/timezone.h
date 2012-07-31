/* $Id: timezone.h,v 1.4.32.2 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaTimeZoneH__
#define MamaTimeZoneH__

#include <mama/config.h>
#include <mama/status.h>
#include <mama/types.h>
#include "wombat/port.h"

#include <time.h>


#if defined(__cplusplus)
extern "C" {
#endif

/** Return a pointer to a mamaTimeZone corresponding to the local time zone. */
MAMAExpDLL
extern mamaTimeZone mamaTimeZone_local(void); 

/** Return a pointer to a mamaTimeZone corresponding to the UTC time zone. */
MAMAExpDLL
extern mamaTimeZone mamaTimeZone_utc(void); 

/** Return a pointer to a mamaTimeZone corresponding to the US Eastern
 * time zone. */
MAMAExpDLL
extern mamaTimeZone mamaTimeZone_usEastern(void); 

/** Create an instance of a mamaTimeZone. */
MAMAExpDLL
extern mama_status
mamaTimeZone_create (mamaTimeZone*  timeZone);

/** Create an instance of a mamaTimeZone with a standard TZ identifier
 * (NULL identifier is equivalent to the local time zone) */
MAMAExpDLL
extern mama_status
mamaTimeZone_createFromTz (mamaTimeZone*  timeZone,
                           const char*    tzId);

/** Create an instance of a mamaTimeZone by copying from another instance. */
MAMAExpDLL
extern mama_status
mamaTimeZone_createCopy (mamaTimeZone*        timeZone,
                         const mamaTimeZone   timeZoneCopy);

/** Destroy an instance of a mamaTimeZone. */
MAMAExpDLL
extern mama_status
mamaTimeZone_destroy (mamaTimeZone  timeZone);

/** Copy an instance of a mamaTimeZone (to a previously-created instance). */
MAMAExpDLL
extern mama_status
mamaTimeZone_copy (mamaTimeZone        timeZone,
                   const mamaTimeZone  timeZoneCopy);

/** Set an existing instance of a mamaTimeZone to a new time zone. */
MAMAExpDLL
extern mama_status
mamaTimeZone_set (mamaTimeZone  timeZone,
                  const char*   tzId);

/** Clear the time zone (but not destroyed). */
MAMAExpDLL
extern mama_status
mamaTimeZone_clear (mamaTimeZone  timeZone);

/** Get the time zone ID string. */
MAMAExpDLL
extern mama_status
mamaTimeZone_getTz (const mamaTimeZone  timeZone,
                    const char**        result);

/** Get the offset from UTC (in seconds).  Can be positive or
 * negative, depending upon the direction. */
MAMAExpDLL
extern mama_status
mamaTimeZone_getOffset (const mamaTimeZone  timeZone,
                        mama_i32_t*         result);

/** Check (recalculate) the UTC offset in case it has changed due
 * to daylight savings adjustments. */
MAMAExpDLL
extern mama_status
mamaTimeZone_check (mamaTimeZone  timeZone);

/**
 * Set the interval of the thread updating each MamaTimeZone instance
 * offset.
 */
MAMAExpDLL
extern void
mamaTimeZone_setScanningInterval (mama_f64_t  seconds);


#if defined(__cplusplus)
}
#endif

#endif
