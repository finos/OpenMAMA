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

using System;

namespace Wombat
{
    /* ************************************************************** */
    #region mamaQuality Enumeration

    /// <summary>
    /// Represents the data quality of the subscription and is passed by the MamaBasicSubscription.onQuality
    /// callback function.
    /// </summary>
    public enum mamaQuality
    {
        MAMA_QUALITY_OK,
        MAMA_QUALITY_MAYBE_STALE,
        MAMA_QUALITY_STALE,
        MAMA_QUALITY_PARTIAL_STALE,
        MAMA_QUALITY_FORCED_STALE,
        MAMA_QUALITY_DUPLICATE,
        MAMA_QUALITY_UNKNOWN
    }

    #endregion

    /* ************************************************************** */
    #region MamaSubscriptionCallback Interface

    /// <summary>
    /// This interface should be implemented by clients wish to create a MAMA subscription, it
    /// contains callback functions that will be invoked whenever specific events occur.
    /// </summary>
    public interface MamaSubscriptionCallback
    {
        /// <summary>
        /// Called when subscription creation is complete, and before
        /// the onMsg() callback is invoked
        /// </summary>
        /// <param name="subscription">
        /// The subscription that has been created.
        /// </param>
        void onCreate(MamaSubscription subscription);

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
        void onError(MamaSubscription subscription, MamaStatus.mamaStatus status, string subject);

        /// <summary>
        /// Function invoked when a sequence number gap is detected. At this point the topic is
        /// considered stale and the subscription will not receive further messages until the
        /// feed handler satisfies a recap request.
        /// </summary>
        /// <param name="subscription">
        /// The subscription where the gap has been detected.
        /// </param>
        void onGap(MamaSubscription subscription);

        /// <summary>
        /// Invoked when a message arrives
        /// </summary>
        /// <param name="subscription">
        /// The subscription where the message has arrived.
        /// </param>
        /// <param name="message">
        /// The message.
        /// </param>
        void onMsg(MamaSubscription subscription, MamaMsg message);

        /// <summary>
        /// Invoked to indicate a data quality event.
        /// </summary>
        /// <param name="subscription">
        /// The subscription where the quality has changed.
        /// </param>
        /// <param name="quality">
        /// The new quality.
        /// </param>
        /// <param name="symbol">
        /// The symbol.
        /// </param>
        void onQuality(MamaSubscription subscription, mamaQuality quality, string symbol);

        /// <summary>
        /// Invoked when a recap is requested upon detecting a sequence number gap.
        /// </summary>
        /// <param name="subscription">
        /// The subscription where the recap has been requested.
        /// </param>
        void onRecapRequest(MamaSubscription subscription);

        /// <summary>
        /// Function invoked when a subscription has been destroyed or deactivated.
        /// </summary>
        /// <param name="subscription">
        /// The subscription that has been destroyed.
        /// </param>
        void onDestroy(MamaSubscription subscription);
    }

    #endregion
}
