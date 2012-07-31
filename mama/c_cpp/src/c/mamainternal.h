/* $Id: mamainternal.h,v 1.16.22.6 2011/08/18 10:54:08 ianbell Exp $
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

#ifndef MamaInternalH__
#define MamaInternalH__

#include <property.h>
#include "mama/types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define MAMA_PAYLOAD_MAX	CHAR_MAX
/**
* Check whether Callbacks are run in 'debug' catch exceptions mode
*/
MAMAExpDLL
extern int
mamaInternal_getCatchCallbackExceptions (void);

/**
 * Exposes the property object from mama
 */
MAMAExpDLL
extern wproperty_t
mamaInternal_getProperties (void);

/* Used by the bridges to register themselves with MAMA. Should not
   be called from anywhere else */
MAMAExpDLL
extern void
mamaInternal_registerBridge (mamaBridge     bridge,
                             const char*    middleware);

/**
 * getVersion for use when mama is wrapped
 */
const char*
mama_wrapperGetVersion(mamaBridge     bridge);

MAMAExpDLL
extern mamaStatsGenerator
mamaInternal_getStatsGenerator (void);

MAMAExpDLL
extern mamaStatsCollector*
mamaInternal_getGlobalStatsCollector (void);

MAMAExpDLL
extern void
cleanupReservedFields (void);

MAMAExpBridgeDLL
extern int
mamaInternal_generateLbmStats (void);

MAMAExpBridgeDLL
mamaBridge
mamaInternal_findBridge (void);

mamaPayloadBridge
mamaInternal_findPayload (char id);

mamaPayloadBridge
mamaInternal_getDefaultPayload (void);

#if defined(__cplusplus)
}
#endif

#endif /*MamaInternalH__ */
