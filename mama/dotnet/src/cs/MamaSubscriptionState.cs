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

﻿
namespace Wombat
{
    /**
     * The enumeration describes the state of the subsubscription at any time.
     * The state can be obtained by calling the mamaSubscription_getState function.
     * A string representation of this state can be obtained by calling the
     * mamaSubscription_stringForState function.
     */
    public enum mamaSubscriptionState
    {
        /* The state of the subscription is unknown. */
        MAMA_SUBSCRIPTION_UNKNOWN = 0,

        /* The subscription has been allocated in memory. */
        MAMA_SUBSCRIPTION_ALLOCATED = 1,

        /* Inital setup work has been done, mamaSubscription_activate must still be called. Note that this state is only valid
         * for market data subscriptions.
         */
        MAMA_SUBSCRIPTION_SETUP = 2,

        /* The subscription is now on the throttle queue waiting to be fully activated. */
        MAMA_SUBSCRIPTION_ACTIVATING = 3,

        /* The subscription is now fully activated and is processing messages. */
        MAMA_SUBSCRIPTION_ACTIVATED = 4,

        /* The subscription is being de-activated, it will not be fully deactivated until the onDestroy callback is received. */
        MAMA_SUBSCRIPTION_DEACTIVATING = 5,

        /* The subscription has been de-activated, messages are no longer being processed. */
        MAMA_SUBSCRIPTION_DEACTIVATED = 6,

        /* The subscription is being destroyed, it will not be fully destroyed until the onDestroy callback is received. */
        MAMA_SUBSCRIPTION_DESTROYING = 7,

        /* The subscription has been fully destroyed. */
        MAMA_SUBSCRIPTION_DESTROYED = 8,

        /* The subscription is in the process of being de-allocated, this state is only valid if the mamaSubscription_deallocate
         * function is called while the subscription is being destroyed.
         */
        MAMA_SUBSCRIPTION_DEALLOCATING = 9,

        /* The subscription has been de-allocated, this state is only supported so that the log entry will whenever the subscription
         * has finally been freed.
         */
        MAMA_SUBSCRIPTION_DEALLOCATED = 10
    }
}
