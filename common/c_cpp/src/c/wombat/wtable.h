/* $Id: wtable.h,v 1.5.2.1.14.3 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#ifndef _WOMBAT_WTABLE_H
#define _WOMBAT_WTABLE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "wincompat.h"
#include <sys/types.h>

typedef void* wtable_t;


/**
 * Callback typedef function for wtable_for_each() and
 * wtable_clear_for_each().
 */
typedef void (*wTableCallback) (
    wtable_t  t, void* data, const char* key, void* closure);

/**
 * Create an empty table.  
 */
COMMONExpDLL
wtable_t wtable_create (const char* name,    unsigned long size);

/**
 * Destroy a table.  Warning: this function does *not* clear the table
 * first because there are several ways to do this: wtable_free_all(),
 * wtable_clear_for_each(), and wtable_clear().
 */
COMMONExpDLL
void wtable_destroy (wtable_t table);

/**
 * Insert an item into the table.  The key is copied internally to the
 * table.
 */
COMMONExpDLL
int wtable_insert (wtable_t table,const char* key, void* data);

/**
 * Find an item in the table.
 */
COMMONExpDLL
void* wtable_lookup (wtable_t table,const char* key);

/**
 * Remove an item from the table.  The item is returned.
 */
COMMONExpDLL
void* wtable_remove (wtable_t table,const char* key);

/**
 * Iterates over all data in all buckets in the table and invokes
 * the callback function passed in for each one
 */
COMMONExpDLL
void wtable_for_each (wtable_t table, wTableCallback cb, void* closure);

/**
 * Clear the table, including a call to free() for every value in the
 * table.  Required to avoid memory leaks.  Do not use this on wtables
 * containing C++ objects!
 */
COMMONExpDLL
void wtable_free_all (wtable_t table);

/**
 * This function will free all the memory in a table except for the data
 * pointers. This is used whenever tables are merged by copying the pointers
 * instead of the memory.
 */
COMMONExpDLL
void wtable_free_all_xdata (wtable_t table);

/**
 * Clear the table, excluding data values. This function has the potential to
 * cause a memory leak.  Alternatively, use wtable_free_all() to automatically
 * free data items that were inserted after being allocated from the heap
 * (malloc/calloc/realloc) -- not from C++. Use wtable_clear_for_each() to
 * invoke a callback that iterates over each value (can be used from C++ to
 * delete objects).
 */
COMMONExpDLL
void wtable_clear (wtable_t table);

/**
 * Clear the table, invoking a callback for each data value.  Can be
 * used from C++ to delete objects.
 */
COMMONExpDLL
void wtable_clear_for_each (wtable_t table, wTableCallback cb, void* closure);

/**
 * Dump a table for debugging purposes.
 */
void dumptable (wtable_t table);

#if defined(__cplusplus)
}
#endif

#endif /* _WOMBAT_WTABLE_H */
