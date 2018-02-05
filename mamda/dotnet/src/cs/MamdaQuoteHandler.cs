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
	/// MamdaQuoteHandler is an interface for applications that want to
	/// have an easy way to handle quote updates.  The interface defines
	/// callback methods for different types of quote-related events:
	/// quotes and closing-quote updates.
	/// </summary>
	public interface MamdaQuoteHandler
	{
		/// <summary>
		/// Method invoked when the current last-quote information for the
		/// security is available.  The reason for the invocation may be
		/// any of the following:
		/// - Initial image.
		/// - Recap update (e.g., after server fault tolerant event or data
		///   quality event.)
		/// - After stale status removed.
		/// </summary>
		/// <param name="subscription">The MamdaSubscription reference.</param>
		/// <param name="listener">The MamdaQuoteListener reference.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="recap">Access to the current value of all fields.</param>
		void onQuoteRecap(
			MamdaSubscription   subscription,
			MamdaQuoteListener  listener,
			MamaMsg             msg,
			MamdaQuoteRecap     recap);

		/// <summary>
		/// Method invoked when a quote update arrives.
		/// </summary>
		/// <param name="subscription">The MamdaSubscription reference.</param>
		/// <param name="listener">The MamdaQuoteListener reference.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="update">The MamdaQuoteUpdate event.</param>
		/// <param name="recap">Access to the current value of all fields.</param>
		void onQuoteUpdate(
			MamdaSubscription   subscription,
			MamdaQuoteListener  listener,
			MamaMsg             msg,
			MamdaQuoteUpdate    update,
			MamdaQuoteRecap     recap);

		/// <summary>
		/// Method invoked when a gap in quote updates is discovered.
		/// </summary>
		/// <param name="subscription">The MamdaSubscription reference.</param>
		/// <param name="listener">The MamdaQuoteListener reference.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="gap">The MamdaQuoteGap event.</param>
		/// <param name="recap">Access to the current value of all fields.</param>
		void onQuoteGap (
			MamdaSubscription   subscription,
			MamdaQuoteListener  listener,
			MamaMsg             msg,
			MamdaQuoteGap       gap,
			MamdaQuoteRecap     recap);

		/// <summary>
		/// Method invoked for a quote closing summary.
		/// </summary>
		/// <param name="subscription">The MamdaSubscription reference.</param>
		/// <param name="listener">The MamdaQuoteListener reference.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="closingEvent">The MamdaQuoteClosing event.</param>
		/// <param name="recap">Access to the current value of all fields.</param>
		void onQuoteClosing (
			MamdaSubscription   subscription,
			MamdaQuoteListener  listener,
			MamaMsg             msg,
			MamdaQuoteClosing   closingEvent,
			MamdaQuoteRecap     recap);

	}
}
