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
	/// <summary>
	/// MamdaMsgListener defines an interface for handling normal messages
	/// for a MamdaSubscription.
	/// </summary>
	public interface MamdaMsgListener
	{
		/// <summary>
		/// Invoked for each message received for the subscription to which
		/// the Listener is registered.
		/// </summary>
		/// <param name="subscription">The MamdaSubscription to which this listener was
		/// registered.</param>
		/// <param name="msg">The MamaMsg received by the underlying MAMA API and which
		/// resulted in this callback being invoked.</param>
		/// <param name="msgType">The message type. e.g. INITIAL, RECAP, UPDATE etc.</param>
		void onMsg(
			MamdaSubscription subscription,
			MamaMsg msg,
			mamaMsgType msgType);
	}
}
