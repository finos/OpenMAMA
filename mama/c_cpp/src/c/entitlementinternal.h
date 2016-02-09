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
#include "imagerequest.h"
#include <mama/entitlement.h>
#include <mama/status.h>

#ifndef EntitlementInternalH__
#define EntitlementInternalH__

#if defined(__cplusplus)
extern "C"
{
#endif

typedef void* entitlementBridge;
typedef void* entitlementSubscriptionHandle;


typedef mama_status
(*entitlementBridge_destroy) (mamaEntitlementBridge bridge);
typedef mama_status
(*entitlementBridge_init) (entitlementBridge bridge);
typedef mama_status
(*entitlementBridge_registerSubjectContext) (SubjectContext* ctx);
typedef mama_status
(*entitlementBridge_createSubscription) (mamaEntitlementBridge mamaEntBridge, SubjectContext* ctx);
typedef mama_status
(*entitlementBridge_destroySubscription) (entitlementSubscriptionHandle handle);
typedef mama_status
(*entitlementBridge_setIsSnapshot) (mamaEntitlementSubscription handle, int isSnapshot);
typedef int
(*entitlementBridge_isAllowed) (mamaEntitlementSubscription handle, char* subject);

typedef struct mamaEntitlementBridge_
{ 
    entitlementBridge mImpl; /* pointer to implementation bridge struct */

    /* implementation functions*/
    entitlementBridge_destroy                   destroy;
    entitlementBridge_registerSubjectContext    registerSubjectContext;
    entitlementBridge_createSubscription        createSubscription;
    entitlementBridge_destroySubscription       destroySubscription;
    entitlementBridge_setIsSnapshot             setIsSnapshot;
    entitlementBridge_isAllowed                 isAllowed;
}mamaEntitlementBridge_;


typedef struct mamaEntitlementSubscription_
{
    mamaEntitlementBridge         mEntitlementBridge;
    entitlementSubscriptionHandle mImpl;
}mamaEntitlementSubscription_;

/*Called when loading/creating a bridge */


#if defined(__cplusplus)
}
#endif

#endif /* EntitlementInternalH__ */
