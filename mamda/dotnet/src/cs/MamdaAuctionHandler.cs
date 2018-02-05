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
	/// MamdaAuctionHandler is an interface for applications that want to
	/// have an easy way to handle Auction updates.  The interface defines
	/// callback methods for different types of Auction-related events:
	/// </summary>
	public interface MamdaAuctionHandler
	{
		/// <summary>
		/// Method invoked when the current last-Auction information for the
		/// security is available.  The reason for the invocation may be
		/// any of the following:
		/// - Initial image.
		/// - Recap update (e.g., after server fault tolerant event or data
		///   quality event.)
		/// - After stale status removed.
		/// </summary>
		/// <param name="subscription">The MamdaSubscription reference.</param>
		/// <param name="listener">The MamdaAuctionListener reference.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="recap">Access to the current value of all fields.</param>
		void onAuctionRecap(
			MamdaSubscription       subscription,
			MamdaAuctionListener    listener,
			MamaMsg                 msg,
			MamdaAuctionRecap       recap);

		/// <summary>
		/// Method invoked when a Auction update arrives.
		/// </summary>
		/// <param name="subscription">The MamdaSubscription reference.</param>
		/// <param name="listener">The MamdaAuctionListener reference.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="update">The MamdaAuctionUpdate event.</param>
		/// <param name="recap">Access to the current value of all fields.</param>
		void onAuctionUpdate(
			MamdaSubscription       subscription,
			MamdaAuctionListener    listener,
			MamaMsg                 msg,
			MamdaAuctionUpdate      update,
			MamdaAuctionRecap       recap);

	}
}

