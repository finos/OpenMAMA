/* $Id: MamaInboxCallback.cs,v 1.4.24.5 2012/08/24 16:12:01 clintonmcdowell Exp $
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
    #region MamaInboxCallback Interface

    /// <summary>
	/// Provides an object-oriented callback interface for a MamaInbox
	/// response to a p2p message being received or when an error is
	/// encountered during p2p messaging
	/// </summary>
	public interface MamaInboxCallback
	{
		/// <summary>
		/// Invoked in response to a p2p message being received.
		/// Note: you can obtain a reference to the user-supplied data passed to
		/// the create method by calling getClosure on the MamaInbox instance
		/// </summary>
		/// <param name="inbox">
        /// The MamaInbox instance which received the message.
        /// </param>
		/// <param name="message">
        /// The mamaMsg received in the p2p response.
        /// </param>
		void onMsg(MamaInbox inbox, MamaMsg message);

		/// <summary>
		/// NB. Not currently used.
		/// Invoked when an error is encountered during p2p messaging.
		/// Note: you can obtain a reference to the user-supplied data passed to
		/// the create method by calling getClosure on the MamaInbox instance
		/// </summary>
		/// <param name="inbox">
        /// The MamaInbox instance for which the error occurred.
        /// </param>
		/// <param name="status">
        /// The mama_status describing the error condition.
        /// </param>
		void onError(MamaInbox inbox, MamaStatus.mamaStatus status);

        /// <summary>
        /// This method is invoked when an inbox has been completely destroyed,
        /// the client can have confidence that no further events will be placed 
        /// on the queue for this inbox.
        /// </summary>
        /// <param name="inbox">
        /// The inbox.
        /// </param>
        /// <param name="closure">
        /// The closure passed to the create function.
        /// </param>
        void onDestroy(MamaInbox inbox, object closure);
    }

    #endregion
}
