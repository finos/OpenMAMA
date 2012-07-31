/* $Id: property.h,v 1.10.2.1.14.3 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#ifndef PROPERTY_H__
#define PROPERTY_H__
 
#include "wombat/wConfig.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define INVALID_PROPERTIES NULL
typedef void* wproperty_t;

typedef void( *propertiesCallback) (const char* name, const char* value,
				    void* closure );

/**
 * Load the properties file. 
 */
COMMONExpDLL
wproperty_t 
properties_Load( const char* path, const char* fileName );


/**
 * Retrieve the property for the specified name. Returns NULL if the property
 * does not exist.
 */
COMMONExpDLL
const char* 
properties_Get( wproperty_t handle, const char* name );

COMMONExpDLL
int
properties_setProperty( wproperty_t handle, 
                        const char* name,
                        const char* value );

/**
 * Create an empty properties object.
 */
COMMONExpDLL
wproperty_t
properties_Create( void );

/**
 * Add (through parsing) an individual property string.
 */
COMMONExpDLL
int
properties_AddString( wproperty_t handle, const char* propString );

/**
 * Merge one properties object into another one.
 */
COMMONExpDLL
void
properties_Merge( wproperty_t to, wproperty_t from );

/**
 * Iterate over all keys in the properties table.
 */
COMMONExpDLL
void
properties_ForEach( wproperty_t handle, propertiesCallback cb, void* closure);

/**
 * Free the resources associated with the properties.
 */
COMMONExpDLL
void
properties_Free( wproperty_t handle );

COMMONExpDLL
void
properties_FreeEx( wproperty_t handle );

/**
 * This function will free all the property memory except for the data
 * pointers. This is used whenever properties are merged by copying the pointers
 * instead of the memory.
 */
COMMONExpDLL
void
properties_FreeEx2( wproperty_t handle );

COMMONExpDLL
int 
properties_GetPropertyValueAsBoolean( const char* propertyValue );

/**
 * Will escape the chars with a \ found to match in chars array. Returns a 
 * malloc'd string which will need freed by the caller
 */
COMMONExpDLL
char* 
properties_AddEscapes (const char* src, const char chars[], int num);


#if defined(__cplusplus)
}
#endif

#endif /* PROPERTY_H__ */
