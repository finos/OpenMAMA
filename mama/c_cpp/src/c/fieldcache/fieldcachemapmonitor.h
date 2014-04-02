/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachemapmonitor.h#1 $
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

#ifndef MamaFieldCacheMapMonitorH__
#define MamaFieldCacheMapMonitorH__

#include "fieldcachemap.h"

#if defined( __cplusplus )
extern "C" {
#endif /* defined( __cplusplus ) */

typedef struct mamaFieldCacheMapMonitorImpl_
{
    /* The base structure must appear first. */
    mamaFieldCacheMapImpl mBaseMap;
    mamaFieldCacheMap mOriginalMap;
} mamaFieldCacheMapMonitorImpl;

typedef struct mamaFieldCacheMapMonitorImpl_* mamaFieldCacheMapMonitor;

/* map will take ownership of originalMap; it will be destroyed when calling
 * mamaFieldCacheMapMonitor_destroy
 */
mama_status mamaFieldCacheMapMonitor_create(mamaFieldCacheMap* map,
                                            mamaFieldCacheMap originalMap);

/* This function will also destroy the original map */
mama_status mamaFieldCacheMapMonitor_destroy(mamaFieldCacheMap map);

mama_status mamaFieldCacheMapMonitor_clear(mamaFieldCacheMap map);

mama_status mamaFieldCacheMapMonitor_add(mamaFieldCacheMap map,
                                         mamaFieldCacheField field);

mama_status mamaFieldCacheMapMonitor_find(mamaFieldCacheMap map,
                                          mama_fid_t fid,
                                          mamaFieldType type,
                                          const char* name,
                                          mamaFieldCacheField* field);

#if defined( __cplusplus )
}
#endif /* defined( __cplusplus ) */

#endif /* MamaFieldCacheMapMonitorH__ */
