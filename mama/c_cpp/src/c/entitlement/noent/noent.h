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

#ifndef NoEntH__
#define NoEntH__

#if defined(__cplusplus)
extern "C" {
#endif


typedef struct noentEntitlementBridge_
{
} noentEntitlementBridge;


typedef struct noentEntitlementSubscriptionHandle_
{
    int mIsSnapshot;
} noentEntitlementSubscriptionHandle;


mama_status
noentEntitlementBridge_registerSubjectContext(SubjectContext* ctx);

mama_status
noentEntitlementBridge_createSubscription(mamaEntitlementBridge entBridge, SubjectContext* ctx);

mama_status
noentEntitlementBridge_destroySubscription(entitlementSubscriptionHandle* handle);

mama_status
noentEntitlementBridge_setIsSnapshot(entitlementSubscriptionHandle* handle, int isSnapshot);

int
noentEntitlementBridge_isAllowed(entitlementSubscriptionHandle* handle, char* subject);

mama_status
noentEntitlementBridge_destroy(mamaEntitlementBridge bridge);

mama_status
noentEntitlementBridge_init(entitlementBridge* bridge);

#if defined(__cplusplus)
}
#endif

#endif /* NoEntH__ */