/* $Id: source.h,v 1.13.32.4 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaSourceH__
#define MamaSourceH__

#include "mama/config.h"
#include "mama/status.h"
#include "mama/sourceman.h"
#include "mama/types.h"
#include "mama/quality.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum mamaSourceState_
{
    MAMA_SOURCE_STATE_OFF     = 0,
    MAMA_SOURCE_STATE_OK      = 1,
    MAMA_SOURCE_STATE_UNKNOWN = 99
} mamaSourceState;

/**
 * Create a mamaSource object.
 *
 * @param source The location of a mamaSource to store the result.
 */
MAMAExpDLL
extern mama_status
mamaSource_create (mamaSource*  source);

/**
 * Destroy a mamaSource object.
 *
 * @param source  The source object to destroy.
 */
MAMAExpDLL
extern mama_status
mamaSource_destroy (mamaSource  source);

/**
 * Clear a mamaSource object.
 *
 * @param source  The source object to clear.
 */
MAMAExpDLL
extern mama_status
mamaSource_clear (mamaSource  source);

/**
 * Set the ID of a mamaSource object.
 *
 * @param source   The source object to update.
 * @param id       The new ID for the source object.
 */
MAMAExpDLL
extern mama_status
mamaSource_setId (mamaSource  source,
                  const char* id);

/**
 * Set the mapped ID of a mamaSource object.  The mapped ID is the ID
 * that the parent source manager has mapped this source as.
 *
 * @param source   The source object to update.
 * @param id       The new mapped ID for the source object.
 */
MAMAExpDLL
extern mama_status
mamaSource_setMappedId (mamaSource  source,
                        const char* id);

/**
 * Set the display ID of a mamaSource object.
 *
 * @param source   The source object to update.
 * @param id       The new display ID for the source object.
 */
MAMAExpDLL
extern mama_status
mamaSource_setDisplayId (mamaSource  source,
                         const char* id);

/**
 * Set the quality of a mamaSource object.
 *
 * @param source   The source object to update.
 * @param quality  The new quality for the source object.
 */
MAMAExpDLL
extern mama_status
mamaSource_setQuality (mamaSource  source,
                       mamaQuality quality);

/**
 * Set the state of a mamaSource object.
 *
 * @param source   The source object to update.
 * @param quality  The new state for the source object.
 */
MAMAExpDLL
extern mama_status
mamaSource_setState (mamaSource      source,
                     mamaSourceState quality);

/**
 * Set the parent for a mamaSource object. The current mamaSource is added to
 * the parents sub source manager.
 *
 * @see mamaSource_getSubSourceManager()
 *
 * @param source   The source object to update.
 * @param parent   The parent source for this source object.
 */
MAMAExpDLL
extern mama_status
mamaSource_setParent (mamaSource source,
                      mamaSource parent);

/**
 * Set the publisher specific source name for this source. e.g. This could be
 * "NASDAQ" for a UTP PAPA publisher.
 *
 * @param source The source object to update.
 * @param symbolNamespace The namespace for the publisher this
 * mamaSource object describes.
 */
MAMAExpDLL
extern mama_status
mamaSource_setSymbolNamespace (mamaSource  source,
                               const char* symbolNamespace);

/**
 * Set the name of the mamaTransport on which this describes a valid source of
 * data.
 *
 * @param source The source object to update.
 * @param transportName The name of the mamaTransport for which this source is
 * valid.
 */
MAMAExpDLL
extern mama_status
mamaSource_setTransportName (mamaSource  source,
                             const char* transportName);

/**
 * Associate a mamaTransport object with the source.
 *
 * @param source The source to update.
 * @param transport The mamaTransport to associate with the source.
 */
MAMAExpDLL
extern mama_status
mamaSource_setTransport (mamaSource    source,
                         mamaTransport transport);

                         
/**
 * Associate a mamaSymbology object with the source.
 *
 * @param source The source to update.
 * @param symbology The mamaSymbology to associate with the source.
 */
MAMAExpDLL
extern mama_status
mamaSource_setSymbology (mamaSource    source,
                         mamaSymbology symbology);                        
/**
 * Get the ID of a mamaSource object.
 *
 * @param source   The source object to check.
 * @param id       Location of the result for the ID of the source.
 */
MAMAExpDLL
extern mama_status
mamaSource_getId (const mamaSource source,
                  const char**     id);

/**
 * Get the mapped ID of a mamaSource object.
 *
 * @param source   The source object to check.
 * @param id       Location of the result for the mapped ID of the source.
 */
MAMAExpDLL
extern mama_status
mamaSource_getMappedId (const mamaSource source,
                        const char**     id);

/**
 * Get the display ID of a mamaSource object.
 *
 * @param source   The source object to check.
 * @param id       Location of the result for the display ID of the source.
 */
MAMAExpDLL
extern mama_status
mamaSource_getDisplayId (const mamaSource source,
                         const char**     id);

/**
 * Get the quality of a mamaSource object.
 *
 * @param source   The source object to check.
 * @param quality  Location of the result for the quality of the source.
 */
MAMAExpDLL
extern mama_status
mamaSource_getQuality (const mamaSource source,
                       mamaQuality*     quality);

/**
 * Get the state of a mamaSource object.
 *
 * @param source   The source object to check.
 * @param state    Location of the result for the state of the source.
 */
MAMAExpDLL
extern mama_status
mamaSource_getState (const mamaSource source,
                     mamaSourceState* state);

/*
   Get the stringified representation of the source state.
*/
MAMAExpDLL
extern const char*
mamaSource_getStateAsString (const mamaSource source);

/**
 * Get the parent source for a mamaSource object.
 *
 * @param source   The source object to check.
 * @param parent   Location to store the address of the parent for this source.
 */
MAMAExpDLL
extern mama_status
mamaSource_getParent (const mamaSource source,
                      mamaSource*      parent);

/**
 * Get the source name for the publisher which this mamaSource represents.
 *
 * @param source The source object to check.
 * @param symbolNamespace The source name for the publisher.
 */
MAMAExpDLL
extern mama_status
mamaSource_getSymbolNamespace (const mamaSource source,
                               const char**     symbolNamespace);

/**
 * Get the name of the mamaTransport on which this source is valid.
 * 
 * @param source The source object to check.
 * @param transportName The location to store the pointer to the
 * transportName.
 */
MAMAExpDLL
extern mama_status
mamaSource_getTransportName (const mamaSource source,
                             const char**     transportName);

/**
 * Get the mamaTransport associated with this source.
 *
 * @param source The source object to check.
 * @param transport The mamaTransport associated with this source.
 */
MAMAExpDLL
extern mama_status
mamaSource_getTransport (const mamaSource source,
                         mamaTransport*   transport);


/**
 * Get the mamaSymbology associated with this source.
 *
 * @param source The source object to check.
 * @param symbology The mamaSymbology associated with this source.
 */
MAMAExpDLL
extern mama_status
mamaSource_getSymbology (const mamaSource source,
                         mamaSymbology*   symbology);
                         
/**
 * A mamaSource can have sub-sources. These sub sources are maintained by a
 * mamaSourceManager within the mamaSource.
 *
 * @param source The mamaSource for which the sub source manager is being
 * obtained.
 * @param subSourceManager The location top store the address of the sub
 * source manager object pointer.
 */
MAMAExpDLL
extern mama_status
mamaSource_getSubSourceManager (const mamaSource   source,
                                mamaSourceManager* subSourceManager);

#if defined(__cplusplus)
}
#endif

#endif
