/* $Id$
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
 
#include <wombat/wConfig.h>
#include <wombat/port.h>
#include <stdarg.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define     INVALID_PROPERTIES              NULL
#define     PROPERTY_NAME_MAX_LENGTH        1024L

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
 * Iterate over all keys in the properties table matching the given prefix
 */
COMMONExpDLL
void
properties_ForEachWithPrefix (wproperty_t handle,
                              const char* prefix,
                              propertiesCallback cb,
                              void* closure);

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

COMMONExpDLL
const char*
properties_GetPropertyValueUsingFormatString (wproperty_t handle,
                                              const char* defaultVal,
                                              const char* format,
                                              ...);

/**
 * This is a local function for parsing string configuration parameters from the
 * properties object, and supports default values. This function should
 * be used where the configuration parameter itself can be variable.
 * @param handle     The property file handle to use
 * @param defaultVal This is the default value to use if the parameter does not
 *                   exist in the configuration file
 * @param paramName  The format and variable list combine to form the real
 *                   configuration parameter used. This configuration parameter
 *                   will be stored at this location so the calling function
 *                   can log this.
 * @param format     This is the format string which is used to build the
 *                   name of the configuration parameter which is to be parsed.
 * @param arguments  This is the variable list of arguments to be used along
 *                   with the format string.
 *
 * @return const char* containing the parameter value or the default or NULL if
 *         it is not found.
 */
COMMONExpDLL
const char*
properties_GetPropertyValueUsingVaList (
    wproperty_t handle,
    const char* defaultVal,
    char* paramName,
    const char* format,
    va_list arguments);

/**
 * Will escape the chars with a \ found to match in chars array. Returns a 
 * malloc'd string which will need freed by the caller
 */
COMMONExpDLL
char* 
properties_AddEscapes (const char* src, const char chars[], int num);

/**
* Returns the number of properties that have been processed so far
*/
COMMONExpDLL
uint32_t
properties_Count (wproperty_t handle);

#if defined(__cplusplus)
}
#endif

#endif /* PROPERTY_H__ */
