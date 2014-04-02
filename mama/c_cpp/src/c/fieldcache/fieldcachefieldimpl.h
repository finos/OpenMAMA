/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachefieldimpl.h#1 $
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

#ifndef MamaFieldCacheFieldImplH__
#define MamaFieldCacheFieldImplH__

#include <mama/config.h>
#include <mama/fielddesc.h>
#include <mama/types.h>
#include <mama/fieldcache/fieldcachetypes.h>
#include "fieldcachevector.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

    /* This structure describes a field that is held in the field cache, it is used
     * when invoking ultraFast calls on the cache to provide direct read only access
     * to field data.
     */

typedef union cacheData_
{
    mama_i8_t      i8;
    mama_u8_t      u8;
    mama_i16_t     i16;
    mama_u16_t     u16;
    mama_i32_t     i32;
    mama_u32_t     u32;
    mama_i64_t     i64;
    mama_u64_t     u64;
    mama_f32_t     f32;
    mama_f64_t     f64;
    mama_bool_t    abool;
    char           chr;
    char*          str;
    void*          data;
} cacheData;

typedef struct mamaFieldCacheFieldImpl_
{
    /* DO NOT CHANGE THE ORDER - IT CAN BREAK THE MEMORY ALIGNMENT */

    /* Field identification */
    mamaFieldType mType;  /* 4 bytes */
    mama_fid_t mFid;      /* 2 bytes */

    /* This flag specifies if the field must never be published (0) or can be
     * published (1)
     */    
    mama_bool_t mPublish; /* 1 byte */
    
    /* This flag specifies if the field should be checked for modifications (1)
     * to decide if it must be published
     */
    mama_bool_t mCheckModified; /* 1 byte */

    /* This is a pointer to the actual field data */
    cacheData  mData;

    /* This is the size of the memory allocated for the data */
    mama_size_t mDataSize;

    /* This is only used for a vector data type and will contain the number of
     * elements in the vector. For any other type this value will be 0.
     * If the type is an opaque then it will contain the number of bytes.
     */
    mama_size_t mVectorSize;
    
    /* Pointer to a field descriptor - NO COPY */
    mamaFieldDescriptor mDescriptor;

    /* Explicit field name - overrides the field descriptor name */
    const char* mName;
    
    /* The current modification state of the field. */
    mama_bool_t mIsModified; /* 1 byte */
} mamaFieldCacheFieldImpl;

/* Get the pointer to the internal data and the size of the memory allocated */
/*
mama_status
mamaFieldCacheField_getData(
    const mamaFieldCacheField field,
    void** data,
    mama_u32_t* size);
*/
/* Copy size byte starting at pointer data to field to the internal field data.
 * New memory is allocated with malloc and old one is freed with free (if needed).
 */
mama_status
mamaFieldCacheField_setDataValue(
    mamaFieldCacheField field,
    const void* data,
    mama_size_t size);

/* Copy pointer data to internal data pointer in field.
 * Returns a pointer to the old data pointer so that old data can be destroyed
 * and freed.
 */
mama_status
mamaFieldCacheField_setDataPointer(mamaFieldCacheField field,
                                   void* data,
                                   void** old);

/* Price vector type requires a special function to be destroyed and freed.
 */
mama_status
mamaFieldCacheField_destroyPriceVector(mamaFieldCacheVector priceVector);

/* DateTime vector type requires a special function to be destroyed and freed.
 */
mama_status
mamaFieldCacheField_destroyDateTimeVector(mamaFieldCacheVector dateTimeVector);

#if defined(__cplusplus)
}
#endif

#endif	/* MamaFieldCacheFieldImplH__ */
