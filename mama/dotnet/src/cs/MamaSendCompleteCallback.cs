/* $Id: MamaSendCompleteCallback.cs,v 1.4.34.5 2012/08/24 16:12:01 clintonmcdowell Exp $
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
	/// <summary>
	/// Provides an object-oriented callback interface for MamaPublisher's
	/// sendWithThrottle and sendFromInboxWithThrottle methods
	/// </summary>
	public interface MamaSendCompleteCallback
	{
		/// <summary>
		/// Prototype for callback invoked when a message sent with either
		/// mamaPublisher_sendWithThrottle() or
		/// mamaPublisher_sendFromInboxWithThrottle() has been sent from
		/// the throttle queue.
		/// 
		/// Messages sent on the throttle queue are no longer destroyed by the API. It
		/// is the responsibility of the application developer to manage the lifecycle
		/// of any messages sent on the throttle.
		/// </summary>
		/// <param name="publisher">The publisher object used to send the message.</param>
		/// <param name="message">The mamaMsg which has been sent from the throttle queue.</param>
		/// <param name="status">Whether the message was successfully sent from the
		/// throttle. A value of MAMA_STATUS_OK indicates that the send was successful.</param>
		/// <param name="closure">User supplied context data.</param>
		void onSendComplete(
			MamaPublisher publisher,
			MamaMsg message,
			MamaStatus.mamaStatus status,
			object closure);
	}
}
