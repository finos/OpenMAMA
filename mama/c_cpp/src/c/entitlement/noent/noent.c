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
#include "noent.h"

mama_status
noentEntitlementBridge_registerSubjectContext(SubjectContext* ctx)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noentEntitlementBridge_registerSubjectContext():");
    return MAMA_STATUS_OK;
}


mama_status
noentEntitlementBridge_destroy(mamaEntitlementBridge bridge)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noentEntitlementBridge_destroy():");
    return MAMA_STATUS_OK;
}


mama_status
noentEntitlementBridge_init(entitlementBridge* bridge)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noentEntitlementBridge_init():");

    noentEntitlementBridge* noentBridge = calloc(1, sizeof(noentEntitlementBridge));

    /* set mamaEntitlemententitlement bridge pointer to bridge implementation struct */
    *bridge = noentBridge;

    return MAMA_STATUS_OK;
}


mama_status
noentEntitlementBridge_createSubscription(mamaEntitlementBridge mamaEntBridge, SubjectContext* ctx)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noentEntitlementBridge_createSubscription():");

    /* implementation object */
    noentEntitlementSubscriptionHandle* noentSubHandle;
    mamaEntitlementBridge_createSubscription(&noentSubHandle);

    /* mama level object */
    mamaEntitlementSubscription mamaEntSub = calloc (1, sizeof(mamaEntitlementSubscription));
    mamaEntSub->mImpl =  noentSubHandle;

    ctx->mEntitlementBridge = mamaEntBridge;
    ctx->mEntitlementSubscription = mamaEntSub;

    return MAMA_STATUS_OK;
}

mama_status
noentEntitlementBridge_destroySubscription(entitlementSubscriptionHandle* handle)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noentEntitlementBridge_destroySubscription():");

    return MAMA_STATUS_OK;
}

mama_status
noentEntitlementBridge_setIsSnapshot(entitlementSubscriptionHandle* handle, int isSnapshot)
{
    mama_log(MAMA_LOG_LEVEL_FINEST, "noentEntitlementBridge_setIsSnapShot():");
}

int
noentEntitlementBridge_isAllowed(entitlementSubscriptionHandle* handle, char* subject)
{
    char* src = strtok (subject,".");
    char* sym = strtok (NULL,".");

    mama_log(MAMA_LOG_LEVEL_FINEST, "noentEntitlementBridge_isAllowed(%s)",subject);

    if (strncmp(sym, "failent", strlen("failent")))
    {
        return 1;
    }

    return 0;
}