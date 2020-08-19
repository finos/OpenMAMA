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

#ifndef OPENMAMA_INTEGRATION
  #define OPENMAMA_INTEGRATION
#endif

#include <mama/integration/mama.h>

#if defined(__cplusplus)
extern "C"
{
#endif

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

    /* Indcates whether to keep the library loaded */
    mama_bool_t       keepLoaded;
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
