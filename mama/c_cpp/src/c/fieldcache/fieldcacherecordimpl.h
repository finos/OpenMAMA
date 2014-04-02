/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcacherecordimpl.h#1 $
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

#ifndef MamaFieldCacheRecordImplH__
#define MamaFieldCacheRecordImplH__

#include <mama/config.h>
#include <mama/fielddesc.h>
#include <mama/types.h>
#include <mama/fieldcache/fieldcachetypes.h>
#include "fieldcachevector.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

    /* This structure describes a record of fields
     */
typedef struct mamaFieldCacheRecordImpl_
{
    /* dynamic resizable vector containing mamaFieldCacheField pointers */
    mamaFieldCacheVector mFields;
    /* number of fields stored */
    mama_size_t mSize;
} mamaFieldCacheRecordImpl;

#if defined(__cplusplus)
}
#endif

#endif	/* MamaFieldCacheRecordImplH__ */
