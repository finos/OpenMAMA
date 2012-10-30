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

﻿using System;

namespace Wombat
{
    /* ************************************************************** */
    #region MamaBasicSubscriptionCallback Interface

    /// <summary>
    /// This interface should be implemented by clients wish to create a MAMA Basic subscription, it
    /// contains callback functions that will be invoked whenever specific events occur.
    /// </summary>
    public interface MamaBasicSubscriptionCallback
    {
        /// <summary>
        /// Called when subscription creation is complete, and before
        /// the onMsg() callback is invoked
        /// </summary>
        /// <param name="subscription">
        /// The subscription that has been created.
        /// </param>
        void onCreate(MamaBasicSubscription subscription);

        /// <summary>
        /// Called when an error has occurred with the subscription
        /// </summary>
        /// <param name="subscription">
        /// The subscription where the error has been detected.
        /// </param>
        /// <param name="status">
        /// The status code associated with the error.
        /// </param>
        /// <param name="subject">
        /// The subject for entitlement errors.
        /// </param>
        void onError(MamaBasicSubscription subscription, MamaStatus.mamaStatus status, string subject);

        /// <summary>
        /// Invoked when a message arrives
        /// </summary>
        /// <param name="subscription">
        /// The subscription where the message has arrived.
        /// </param>
        /// <param name="message">
        /// The message.
        /// </param>
        void onMsg(MamaBasicSubscription subscription, MamaMsg message);
    
        /// <summary>
        /// Invoked whenever the basic subscription has been destroyed. The client can have confidence that 
        /// no further messages will be placed on the queue for this subscription.
        /// </summary>
        /// <param name="subscription">
        /// The basic subscription.
        /// </param>
        /// <param name="closure">
        /// The closure provided to the createBasic function.
        /// </param>
        void onDestroy(MamaBasicSubscription subscription, object closure);
    }

#endregion
}
