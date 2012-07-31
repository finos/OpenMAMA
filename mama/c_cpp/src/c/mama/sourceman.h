/* $Id: sourceman.h,v 1.8.22.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaSourceManagerH__
#define MamaSourceManagerH__

#include "mama/config.h"
#include "mama/status.h"
#include "mama/types.h"
#include "mama/quality.h"
#include "mama/log.h"
#include "mama/source.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef void (MAMACALLTYPE *mamaSourceManager_sourcesIteratorCb) (
                                  mamaSourceManager sourceManager, 
                                  mamaSource        source,
                                  void*             closure);

/**
 * Create a mamaSourceManager object.
 *
 * @param sourceManager The location of a mamaSourceManager to store the result.
 */
MAMAExpDLL
extern mama_status
mamaSourceManager_create (mamaSourceManager* sourceManager);

/**
 * Destroy a mamaSourceManager object.
 *
 * @param sourceManager  The sourceManager object to destroy.
 */
MAMAExpDLL
extern mama_status
mamaSourceManager_destroy (mamaSourceManager sourceManager);

/**
 * Create a new mamaSource and add it to the manager.
 *
 * @param sourceManager The sourceManager to use for creating the mamaSource.
 * @param name The string identifier for the mamaSource.
 * @param source The address to which the new source will be * written.
 *
 * @return MAMA_STATUS_OK if execution is successful.
 */
MAMAExpDLL
extern mama_status
mamaSourceManager_createSource (mamaSourceManager sourceManager,
                                const char*       name,
                                mamaSource*       source);

/**
 * Locates an existing mamaSource for the given name. If none exists creates
 * a new mamaSource and adds to the sourceManager.
 *
 * @param sourceManager The sourceManager to use for locating the mamaSource.
 * @param name The string identifier for the mamaSource
 * @param source The location to which the address for the source will be
 * written.
 *
 * @return MAMA_STATUS_OK if execution is successful.
 */
MAMAExpDLL
extern mama_status
mamaSourceManager_findOrCreateSource (mamaSourceManager  sourceManager,
                                      const char*        name,
                                      mamaSource*        source);

/**
 * Locates an existing mamaSource in the specified sourceManager with the
 * specified string 'name' identifier.
 * The value of the source argument will be set to NULL if no source was
 * located in the sourceManager provided.
 *
 * @param sourceManager The mamaSourceManager to use to locate the specified
 * mamaSource.
 * @param name The string identifier for the required mamaSource.
 * @param source The location to which the address for the source will be
 * written. NULL if none is found.
 *
 * @return MAMA_STATUS_OK if creation is successful.
 */
MAMAExpDLL
extern mama_status
mamaSourceManager_findSource (mamaSourceManager  sourceManager,
                              const char*        name,
                              mamaSource*        source);

/**
 * Add an existing mamaSource to the specified mamaSourceManager.
 * The id of the source will be used instead of the name to uniquely identify
 * the source within the manager.
 *
 * @param sourceManager The mamaSourceManager to which an existing mamaSource
 * is being added.
 * @param source The mamaSource being added to the specified
 * mamaSourceManager.
 * 
 * @return MAMA_STATUS_OK if execution is successful.
 */
MAMAExpDLL
extern mama_status
mamaSourceManager_addSource (mamaSourceManager  sourceManager,
                             mamaSource         source);

/**
 * Add an existing mamaSource to the specified mamaSourceManager using the
 * specified name as a unique identifier.
 *
 * @param sourceManager The mamaSourceManager to which an existing mamaSource
 * is being added.
 * @param name The string identifier for the mamaSource
 * @param source The mamaSource being added to the specified
 * mamaSourceManager.
 * 
 * @return MAMA_STATUS_OK if execution is successful.
 */
MAMAExpDLL
extern mama_status
mamaSourceManager_addSourceWithName (mamaSourceManager  sourceManager,
                                     mamaSource         source,
                                     const char*        name);

/**
 * Iterate over all the sources in this mamaSourceManager.
 *
 * @param sourceManager The mamaSourceManager to iterate over.
 * @param callback    The callback function pointer to invoke for each source
 * in the group.
 * @param closure     User supplied arbitrary data. Passed back on each
 * invocation of the callback function.
 * 
 * @return MAMA_STATUS_OK if the function executes successfully.
 */
MAMAExpDLL
extern mama_status
mamaSourceManager_iterateSources (mamaSourceManager                 sourceGroup,
                                  mamaSourceManager_sourcesIteratorCb callback,
                                  void*                               closure);
                                
#if defined (__cplusplus)
}
#endif

#endif
