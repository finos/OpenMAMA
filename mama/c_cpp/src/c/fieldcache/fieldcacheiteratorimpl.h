/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcacheiteratorimpl.h#1 $
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

#ifndef MamaFieldCacheIteratorImplH__
#define MamaFieldCacheIteratorImplH__

#include <mama/config.h>
#include <mama/status.h>
#include <mama/fieldcache/fieldcacheiterator.h>

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

/* The following defines are function pointers for use in the FieldCacheIterator
 * structure.
 */
typedef mama_status (*fieldCacheIterator_destroy)(mamaFieldCacheIterator);

typedef mamaFieldCacheField (*fieldCacheIterator_next)(mamaFieldCacheIterator);

typedef mama_bool_t (*fieldCacheIterator_hasNext)(mamaFieldCacheIterator);

typedef mamaFieldCacheField (*fieldCacheIterator_begin)(mamaFieldCacheIterator);

/* Iterator */
typedef struct mamaFieldCacheIteratorImpl_
{
    fieldCacheIterator_destroy mDestroyFunction;
    fieldCacheIterator_next mFunctionNext;
    fieldCacheIterator_hasNext mFunctionHasNext;
    fieldCacheIterator_begin mFunctionBegin;
} mamaFieldCacheIteratorImpl;

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* MamaFieldCacheIteratorImplH__ */
