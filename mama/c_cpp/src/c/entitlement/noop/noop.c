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

#include <string.h>
#include <stdio.h>
/* mama internal */
#include "subscriptionimpl.h"
#include "mamainternal.h"
/* mama public */
#include "mama/mama.h"
/* implementation */
#include "noop.h"

mama_status
noopEntitlementBridge_registerSubjectContext(SubjectContext* ctx)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noopEntitlementBridge_registerSubjectContext():");
    return MAMA_STATUS_OK;
}


mama_status
noopEntitlementBridge_destroy(mamaEntitlementBridge bridge)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noopEntitlementBridge_destroy():");
    return MAMA_STATUS_OK;
}

mama_status
noopEntitlementBridge_init(entitlementBridge* bridge)
{
    mama_log (MAMA_LOG_LEVEL_WARN,
              "\n********************************************************************************\n"
              "Note: This build of the MAMA API is not enforcing entitlement checks.\n"
              "Please see the Licensing file for details\n"
              "**********************************************************************************");

    /* set mamaEntitlemententitlement bridge pointer to bridge implementation struct */
    *bridge = NULL;

    return MAMA_STATUS_OK;
}


mama_status
noopEntitlementBridge_createSubscription(mamaEntitlementBridge mamaEntBridge, SubjectContext* ctx)
{
    /* Although this is a no-op bridge, some mama functionality assumes that an entitlementSubscription
     * exists as long as an entitlementBridge has been loaded
     */
    mama_status                         status;
    mamaEntitlementSubscription         mamaEntSub    = NULL;

    mama_log(MAMA_LOG_LEVEL_FINEST, "noopEntitlementBridge_createSubscription():");

    /* Allocate mama_level entitlement subscription object and set implementation struct pointer. */
    status = mamaEntitlementBridge_createSubscription(&mamaEntSub);
    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    mamaEntSub->mImpl               = NULL;
    mamaEntSub->mEntitlementBridge  = mamaEntBridge;

    /* Add mama level struct to subscription SubjectContext. */
    ctx->mEntitlementBridge = mamaEntBridge;
    ctx->mEntitlementSubscription = mamaEntSub;

    return MAMA_STATUS_OK;
}

mama_status
noopEntitlementBridge_destroySubscription(entitlementSubscriptionHandle handle)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noopEntitlementBridge_destroySubscription():");
    return MAMA_STATUS_OK;
}

mama_status
noopEntitlementBridge_setIsSnapshot(entitlementSubscriptionHandle handle, int isSnapshot)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noopEntitlementBridge_setIsSnapShot():");
	return MAMA_STATUS_OK;
}

int
noopEntitlementBridge_isAllowed(entitlementSubscriptionHandle handle, char* subject)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noopEntitlementBridge_isAllowed(%s)",subject);
    return 1;
}
