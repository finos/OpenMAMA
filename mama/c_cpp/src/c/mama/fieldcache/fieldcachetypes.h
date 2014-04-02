/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachetypes.h#1 $
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

#ifndef MamaFieldCacheTypesH__
#define MamaFieldCacheTypesH__

#include <wombat/wConfig.h>

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

/** This structure contains the information of a field cache.
 */
typedef struct mamaFieldCacheImpl_* mamaFieldCache;

/** This structure contains the information of a field that can be stored in a
 *  field cache.
 */
typedef struct mamaFieldCacheFieldImpl_* mamaFieldCacheField;

/** This structure contains the information to store a set of fields.
 */
typedef struct mamaFieldCacheRecordImpl_* mamaFieldCacheRecord;

/** This structure allows to iterate through a field cache.
 */
typedef struct mamaFieldCacheIteratorImpl_* mamaFieldCacheIterator;

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* MamaFieldCacheTypesH__ */
