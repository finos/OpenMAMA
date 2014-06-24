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

#ifndef MAMA_BRIDGE_QPID_COMMON_H__
#define MAMA_BRIDGE_QPID_COMMON_H__

/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mama/mama.h>
#include <proton/message.h>
#include <proton/error.h>

#include "payloadbridge.h"
#include "msgfieldimpl.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

#define MAMA_ROOT_MARKET_DATA      "_MD"
#define MAMA_ROOT_MARKET_DATA_DICT "_MDDD"


/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/


/**
 * This function will generate a string which is unique to the root, source
 * and topic provided. Centralization of this function means that it can be used
 * in both the publisher and the subscriber in order to generate a consistent
 * topic for use throughout the platform. It will ensure that if any provided
 * parameter is NULL, that it will be ignored in the expansion (e.g. providing
 * root=NULL, source=EXCHANGENAME.ISIN.CURRENCY and topic=NULL will result in
 * a new keyTarget="EXCHANGENAME.ISIN.CURRENCY" as opposed to
 * ".EXCHANGENAME.ISIN.CURRENCY."
 *
 * @param root      Prefix to associate with the subject (e.g. _MDDD).
 * @param source    Source to base the subject key on (e.g. EXCHANGENAME).
 * @param topic     Topic to base the subject key on (e.g. ISIN.CURRENCY).
 * @param keyTarget Pointer to populate with the generated subject key. The
 *                  caller is then responsible for freeing this new strdup'd
 *                  string.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeCommon_generateSubjectKey (const char*  root,
                                     const char*  source,
                                     const char*  topic,
                                     const char** keyTarget);

/**
 * This function will take the provided format string and use the root, source,
 * topic and uuid provided to expand the format string into a newly allocated
 * string which the caller is responsible for freeing. The format string uses
 * the following tokens:
 *
 * %r : Root (only for market data requests, otherwise blank). e.g. _MD
 * %S : MAMA Source / Symbol Namespace. e.g. OPENMAMA
 * %s : Symbol / Topic. e.g. MSFT
 * %u : uuid. e.g. 4542dc20-f1ae-11e3-ac10-0800200c9a66
 *
 * @param format    The format string to expand.
 * @param root      Prefix to associate with the subject (e.g. _MDDD).
 * @param source    Source to base the subject key on (e.g. EXCHANGENAME).
 * @param topic     Topic to base the subject key on (e.g. ISIN.CURRENCY).
 * @param keyTarget Pointer to populate with the generated subject key. The
 *                  caller is then responsible for freeing this new strdup'd
 *                  string.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeCommon_generateSubjectUri (const char*  format,
                                     const char*  root,
                                     const char*  source,
                                     const char*  topic,
                                     const char*  uuid,
                                     const char** uriTarget);

/**
 * This function will parse a standard period delimited string as generated via
 * qpidBridgeCommon_generateSubjectKey and split it up into its individual
 * components. This allows us to accurately split up these components, even
 * when the arguments supplied to qpidBridgeCommon_generateSubjectKey aren't
 * quite in the format expected (e.g. if qpidBridgeCommon_generateSubjectKey
 * is provided with root=NULL, source=NULL and
 * topic=_MD.EXCHANGENAME.ISIN.CURRENCY, this will then be collapsed in
 * qpidBridgeCommon_generateSubjectKey to create
 * subjectKey=_MD.EXCHANGENAME.ISIN.CURRENCY. If that string is then provided
 * to this function, it will parse the subjectKey and extract the *real* values
 * for root=_MD, source=EXCHANGENAME and topic=ISIN.CURRENCY).
 *
 * @param key       Subject key to parse (e.g. _MD.EXCHANGENAME.ISIN.CURRENCY).
 * @param root      Pointer to populate with the parsed root (e.g. _MD). Caller
 *                  is then responsible for the memory associated with this
 *                  string.
 * @param source    Pointer to populate with the parsed source (e.g. _MD).
 *                  Caller is then responsible for the memory associated with
 *                  this string.
 * @param topic     Pointer to populate with the parsed topic (e.g. _MD). Caller
 *                  is then responsible for the memory associated with this
 *                  string.
 * @param transport Transport bridge on which this topic is being created. This
 *                  is required to help determine whether or not the key
 *                  contains a known symbol namespace.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeCommon_parseSubjectKey (const char*       key,
                                  const char**      root,
                                  const char**      source,
                                  const char**      topic,
                                  transportBridge   transport);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_QPID_COMMON_H__ */
