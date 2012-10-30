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
	/// MamdaOrderImbalanceHandler is an interface for applications that want to
	/// have an easy way to handle order imbalance updates.  The interface defines
	/// callback methods for different types of order-imbalance events:
	/// </summary>
	public interface MamdaOrderImbalanceHandler
	{
		/// <summary>
		/// Method invoked when a order imbalance is reported.
		/// </summary>
		/// <param name="subscription">The subscription which received this update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		void onOrderImbalance( 
			MamdaSubscription			subscription,
			MamdaOrderImbalanceListener	listener,
			MamaMsg						msg);

		/// <summary>
		/// Method invoked when a no order imbalance is reported.
		/// </summary>
		/// <param name="subscription">The subscription which received this update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		void onNoOrderImbalance(
			MamdaSubscription			subscription,
			MamdaOrderImbalanceListener	listener,
			MamaMsg						msg);

		/// <summary>
		/// Method invoked when the current order imbalance information 
		/// is available.  The reason for the invocation may be
		/// any of the following:
		/// - Initial image.
		/// - Recap update (e.g., after server fault tolerant event or data
		///   quality event.)
		/// - After stale status removed.
		/// </summary>
		/// <param name="subscription">The subscription which received this update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		void onOrderImbalanceRecap(
			MamdaSubscription			subscription,
			MamdaOrderImbalanceListener	listener,
			MamaMsg						msg);
    
	}
}
