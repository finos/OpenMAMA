/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachemapmonitor.c#1 $
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

#include "fieldcachemapmonitor.h"

mama_status mamaFieldCacheMapMonitor_create(mamaFieldCacheMap* map,
                                            mamaFieldCacheMap originalMap)
{
    mamaFieldCacheMapMonitor mapMonitor =
             (mamaFieldCacheMapMonitor)calloc(1, sizeof(mamaFieldCacheMapMonitorImpl));

	if(map == NULL)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
	
    if(mapMonitor == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }

    mapMonitor->mOriginalMap = originalMap;
    *map = (mamaFieldCacheMap)mapMonitor;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheMapMonitor_destroy(mamaFieldCacheMap map)
{
    mamaFieldCacheMapMonitor mapMonitor = (mamaFieldCacheMapMonitor)map;
    mamaFieldCacheMap_destroy(mapMonitor->mOriginalMap);
    free(mapMonitor);
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheMapMonitor_clear(mamaFieldCacheMap map)
{
    mamaFieldCacheMapMonitor mapMonitor = (mamaFieldCacheMapMonitor)map;
    return mamaFieldCacheMap_clear(mapMonitor->mOriginalMap);
}

mama_status mamaFieldCacheMapMonitor_add(mamaFieldCacheMap map,
                                         mamaFieldCacheField field)
{
    mamaFieldCacheMapMonitor mapMonitor = (mamaFieldCacheMapMonitor)map;
    return mamaFieldCacheMap_add(mapMonitor->mOriginalMap, field);
}

mama_status mamaFieldCacheMapMonitor_find(mamaFieldCacheMap map,
                                          mama_fid_t fid,
                                          mamaFieldType type,
                                          const char* name,
                                          mamaFieldCacheField* field)
{
    mamaFieldCacheMapMonitor mapMonitor = (mamaFieldCacheMapMonitor)map;
    return mamaFieldCacheMap_find(mapMonitor->mOriginalMap, fid, type, name, field);
}
