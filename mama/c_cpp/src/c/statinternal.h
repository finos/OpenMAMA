/* $Id: statinternal.h,v 1.3.22.3 2011/08/10 14:53:26 nicholasmarriott Exp $
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

#ifndef MamaStatInternalH__
#define MamaStatInternalH__

#include "mama/status.h"
#include "mama/types.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef int (MAMACALLTYPE *pollStatCb) (void* closure);

MAMAExpDLL
extern mama_status
mamaStat_setIntervalValueFromTotal (mamaStat stat, mama_u32_t value);

MAMAExpDLL
extern mama_status
mamaStat_setPollCallback (mamaStat stat, pollStatCb callback, void* closure);

#if defined (__cplusplus)
}
#endif

#endif /* MamaStatInternalH__ */
