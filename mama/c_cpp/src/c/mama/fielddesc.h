/* $Id: fielddesc.h,v 1.18.32.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaFieldDescriptorH__
#define MamaFieldDescriptorH__

/**
 * The mamaFieldDescriptor class represents a field in a mamaMsg.
 */

#include <mama/status.h>
#include <mama/types.h>
#include <stdlib.h>

#include "wombat/port.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum mamaFieldType_
{
    /** Sub message*/
    MAMA_FIELD_TYPE_MSG           =    1, 

    /** Opaque binary */
    MAMA_FIELD_TYPE_OPAQUE        =    7,
    
    /** String */
    MAMA_FIELD_TYPE_STRING        =    8,
    
    /** Boolean */
    MAMA_FIELD_TYPE_BOOL          =    9, 

    /** Character */
    MAMA_FIELD_TYPE_CHAR          =   10, 

    /** Signed 8 bit integer */
    MAMA_FIELD_TYPE_I8            =   14, 

    /** Unsigned byte */
    MAMA_FIELD_TYPE_U8            =   15,
    
    /** Signed 16 bit integer */
    MAMA_FIELD_TYPE_I16           =   16,
   
    /** Unsigned 16 bit integer */
    MAMA_FIELD_TYPE_U16           =   17,
    
    /** Signed 32 bit integer */
    MAMA_FIELD_TYPE_I32           =   18,
    
    /** Unsigned 32 bit integer */
    MAMA_FIELD_TYPE_U32           =   19,

    /** Signed 64 bit integer */
    MAMA_FIELD_TYPE_I64           =   20,

    /** Unsigned 64 bit integer */
    MAMA_FIELD_TYPE_U64           =   21,
   
    /** 32 bit float */
    MAMA_FIELD_TYPE_F32           =   24, 

    /** 64 bit float */
    MAMA_FIELD_TYPE_F64           =   25,

    /** 64 bit MAMA time */
    MAMA_FIELD_TYPE_TIME          =   26,

    /** MAMA price */
    MAMA_FIELD_TYPE_PRICE         =   27,

    /** Array type support */
    MAMA_FIELD_TYPE_VECTOR_I8     =   34, 
    MAMA_FIELD_TYPE_VECTOR_U8     =   35, 
    MAMA_FIELD_TYPE_VECTOR_I16    =   36, 
    MAMA_FIELD_TYPE_VECTOR_U16    =   37, 
    MAMA_FIELD_TYPE_VECTOR_I32    =   38, 
    MAMA_FIELD_TYPE_VECTOR_U32    =   39,
    MAMA_FIELD_TYPE_VECTOR_I64    =   40, 
    MAMA_FIELD_TYPE_VECTOR_U64    =   41,
    MAMA_FIELD_TYPE_VECTOR_F32    =   44, 
    MAMA_FIELD_TYPE_VECTOR_F64    =   45,
    MAMA_FIELD_TYPE_VECTOR_STRING =   46,
    MAMA_FIELD_TYPE_VECTOR_MSG    =   47,
    MAMA_FIELD_TYPE_VECTOR_TIME   =   48,
    MAMA_FIELD_TYPE_VECTOR_PRICE  =   49,
    MAMA_FIELD_TYPE_QUANTITY      =   50,
    
    /** Collection */
    MAMA_FIELD_TYPE_COLLECTION    =   99,

    /** Unknown */
    MAMA_FIELD_TYPE_UNKNOWN       =  100

} mamaFieldType;

/**
 * Return the field type as a string.
 *
 * @param type The type.
 *
 * @return The type as a string.
 */
MAMAExpDLL
extern const char*
mamaFieldTypeToString (mamaFieldType type);

/**
 * Return the string as a field type.
 *
 * @param str The string to transform.
 *
 * @return The field type.
 */
MAMAExpDLL
extern mamaFieldType
stringToMamaFieldType (const char*  str);

/**
 * Create a mamaFieldDescriptor
 *
 * @param descriptor The descriptor.
 * @param fid  The field identifier.
 * @param type The type.
 * @param name The field name.
 *
 * @return mama_status code
 */
MAMAExpDLL
extern mama_status
mamaFieldDescriptor_create(
    mamaFieldDescriptor*  descriptor, 
    mama_fid_t            fid, 
    mamaFieldType         type, 
    const char*           name);

/**
 * Destroy a mamaFieldDescriptor
 *
 * @param descriptor The descriptor.
 * @return mama_status code
 */
MAMAExpDLL
extern mama_status
mamaFieldDescriptor_destroy(
    mamaFieldDescriptor  descriptor);

/**
 * Return the field identifier. 0 indicates no fid.
 *
 * @param descriptor The descriptor.
 * @return The fid.
 */
MAMAExpDLL
mama_fid_t
mamaFieldDescriptor_getFid(
    const mamaFieldDescriptor  descriptor);

/**
 * Return the data type.
 *
 * @return The type.
 */
MAMAExpDLL
mamaFieldType
mamaFieldDescriptor_getType(
    const mamaFieldDescriptor  descriptor);

/**
 * Return the human readable name of the field.
 *
 * @param descriptor The descriptor.
 * @return The name.
 */
MAMAExpDLL
const char*
mamaFieldDescriptor_getName(
    const mamaFieldDescriptor  descriptor);

/**
 * return a human readable representation of the type name. 
 *
 * @param descriptor The descriptor.
 */
MAMAExpDLL
const char*
mamaFieldDescriptor_getTypeName(
    const mamaFieldDescriptor  descriptor);

/**
 * Associate some user supplied data with this field descriptor.
 *
 * @param descriptor The field descriptor on which the closure is being
 * set.
 * @param closure The arbitrary user supplied data.
 *
 * @return mama_status MAMA_STATUS_OK if the call was successful.
 */
MAMAExpDLL
mama_status
mamaFieldDescriptor_setClosure(
            mamaFieldDescriptor  descriptor,
            void*                closure);

/**
 * Return the user supplied data with this field descriptor. Returns NULL
 * if no data was associated with this field descriptor.
 *
 * @param descriptor The field descriptor from which the closure is being
 * obtained.
 * @param closure The location to where the user supplied data will be written.
 *
 * @return mama_status MAMA_STATUS_OK if the call was successful.
 */
MAMAExpDLL
mama_status
mamaFieldDescriptor_getClosure(
        mamaFieldDescriptor  descriptor,
        void**               closure);

#if defined(__cplusplus)
}
#endif

#endif /* MamaFieldDescriptorH__ */
