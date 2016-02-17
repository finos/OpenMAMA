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

#ifndef MamaEntNoopH__
#define MamaEntNoopH__

#if defined(__cplusplus)
extern "C" {
#endif


typedef struct noopEntitlementBridge_
{
} noopEntitlementBridge;


typedef struct noopEntitlementSubscriptionHandle_
{
    int mIsSnapshot;
} noopEntitlementSubscriptionHandle;

/*=========================================================================
  =                   Public implementation prototypes                    =
  =========================================================================*/

/**
 * This function will populate a subscription context with entitlements
 * implementation information.
 *
 * @param ctx Pointer to the SubscriptionContext to be populated.
 */
MAMAExpDLL
mama_status
noopEntitlementBridge_registerSubjectContext(SubjectContext* ctx);

/**
 * This function will create an entitlementSubscription object and associate
 * it with an entitlement bridge implementation.
 *
 * @param entBridge The entitlement bridge to associate with the subscription.
 * @param ctx The subscription SubjectContext to be populated with a pointer to
 *            the entitlement subscription object.
 */
MAMAExpDLL
mama_status
noopEntitlementBridge_createSubscription(mamaEntitlementBridge entBridge, SubjectContext* ctx);

/**
 * Destroys the entitlement bridge level subscription object.
 *
 * @param handle The subscription object to free.
 */
MAMAExpDLL
mama_status
noopEntitlementBridge_destroySubscription(entitlementSubscriptionHandle handle);

/**
 * Sets internal flag for snapshot subscriptions.
 *
 * @param handle Entitlement subscription object to update.
 * @param isSnapshot Value to update object to.
 */
MAMAExpDLL
mama_status
noopEntitlementBridge_setIsSnapshot(entitlementSubscriptionHandle* handle, int isSnapshot);

/**
 * Checks with entitlements implementation if user is entitled to subscribe to topic.
 * Returns non zero if entitled.
 *
 * @param handle The entitlement subscription objec tto check against
 * @param subject The topic to check.
 */
MAMAExpDLL
int
noopEntitlementBridge_isAllowed(entitlementSubscriptionHandle handle, char* subject);

/**
 * Destroys the entitlement bridge level object.
 * This function will be detected and called at the same time that the parent
 * MAMA level object is destroyed.
 *
 * @param bridge The bridge object to destroy.
 */
MAMAExpDLL
mama_status
noopEntitlementBridge_destroy(mamaEntitlementBridge bridge);

/**
 * This will initialize the entitlements bridge level object. Use this function
 * to create any connections or objects used in the entitlements implementation.
 *
 * This will be automatically called when MAMA loads the entitlements bridge.
 * @param bridge The (preallocated by mama) bridge level object to be
 *               initilized.
 */
MAMAExpDLL
mama_status
noopEntitlementBridge_init(entitlementBridge* bridge);

#if defined(__cplusplus)
}
#endif

#endif /* MamaEntNoopH__ */
