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

#ifndef MamaInternalH__
#define MamaInternalH__

#include <property.h>
#include "wombat/wtable.h"
#include "mama/types.h"
#include "mama/status.h"
#include "entitlementinternal.h"
#include "mama/mama.h"
#include "wombat/strutils.h"
#include <mama/version.h>
#include <mama/integration/mama.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#define MAMA_PAYLOAD_MAX        CHAR_MAX
#define MAMA_MAX_MIDDLEWARES    CHAR_MAX
#define MAMA_MAX_ENTITLEMENTS   CHAR_MAX
#define MAX_ENTITLEMENT_BRIDGES CHAR_MAX

/* Maximum internal property length */
#define MAX_INTERNAL_PROP_LEN   1024

/* These are the bare parameters which may be reused in other properties */
#define MAMA_PROP_BARE_ENT_DEFERRED        "entitlements.deferred"
#define MAMA_PROP_BARE_COMPILE_TIME_VER    "compile_version"

/* These properties will be set by MAMA */
#define MAMA_PROP_MAMA_RUNTIME_VER         "mama.runtime_version"

/* %s = Bridge Name - these properties may be populated by bridge */
#define MAMA_PROP_BRIDGE_ENT_DEFERRED      "mama.%s." MAMA_PROP_BARE_ENT_DEFERRED
#define MAMA_PROP_BRIDGE_COMPILE_TIME_VER  "mama.%s." MAMA_PROP_BARE_COMPILE_TIME_VER

#define MAMA_SET_BRIDGE_COMPILE_TIME_VERSION(bridgeName)                       \
do                                                                             \
{                                                                              \
    char valString[MAX_INTERNAL_PROP_LEN];                                     \
    char propString[MAX_INTERNAL_PROP_LEN];                                    \
                                                                               \
    /* Advise MAMA which version of MAMA the bridge was compiled against */    \
    snprintf (propString,                                                      \
              sizeof(propString),                                              \
              MAMA_PROP_BRIDGE_COMPILE_TIME_VER,                               \
              bridgeName);                                                     \
    /* Advise MAMA which version of MAMA the bridge was compiled against */    \
    snprintf (valString,                                                       \
              sizeof(valString),                                               \
              "%d.%d.%s",                                                      \
              MAMA_VERSION_MAJOR,                                              \
              MAMA_VERSION_MINOR,                                              \
              MAMA_VERSION_RELEASE);                                           \
    mamaInternal_setMetaProperty (                                             \
              propString,                                                      \
              valString);                                                      \
}                                                                              \
while(0)

/**
 * @brief Structure for storing combined mamaPayloadBridge and LIB_HANDLE data.
 */
typedef struct mamaPayloadLib_
{
    mamaPayloadBridge bridge;
    LIB_HANDLE        library;
    char              id;

    /* Indcates if the bridge was allocated by MAMA or the bridge */
    mama_bool_t       mamaAllocated;
} mamaPayloadLib;


/*
 * @brief Structure for storing combined mamaEntitlementBridge and LIB_HANDLE data.
 */
typedef struct mamaEntitlementLib_
{
    mamaEntitlementBridge bridge;
    LIB_HANDLE            library;
} mamaEntitlementLib;

#if defined(__cplusplus)
}
#endif

#endif /*MamaInternalH__ */
