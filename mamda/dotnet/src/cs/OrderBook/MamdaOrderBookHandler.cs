/* $Id: MamdaOrderBookHandler.cs,v 1.1.40.5 2012/08/24 16:12:13 clintonmcdowell Exp $
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
	/// Process updates on the order book.
	/// Concrete instances of this interface are passed to the
	/// <code>MamdaOrderBookListener</code> in order for an application to receive
	/// callbacks on order book update events.
	/// </summary>
	public interface MamdaOrderBookHandler
	{
		/// <summary>
		/// Method invoked when an updated full order book image is
		/// available.  The reason for the invocation may be any of the
		/// following:
		/// - Initial image.
		/// - Recap update (e.g., after server fault tolerant event or data
		///   quality event.)
		/// - After stale status removed.
		/// </summary>
		/// <param name="subscription">The subscription which received the update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="delta">The order book delata generated from the update.</param>
		/// <param name="fullBook">The full order book</param>
		void onBookRecap (
			MamdaSubscription        subscription,
			MamdaOrderBookListener   listener,
			MamaMsg                  msg,
			MamdaOrderBookDelta      delta,
			MamdaOrderBookRecap      fullBook);

		/// <summary>
		/// Method invoked when a regular update for an order book is
		/// available.
		/// </summary>
		/// <param name="subscription">The subscription which received the update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="delta">The order book delata generated from the update.</param>
		/// <param name="fullBook">The full order book</param>
		void onBookDelta (
			MamdaSubscription        subscription,
			MamdaOrderBookListener   listener,
			MamaMsg                  msg,
			MamdaOrderBookDelta      delta,
			MamdaOrderBookRecap      fullBook);

		/// <summary>
		/// Method invoked when an order book is cleared.
		/// </summary>
		/// <param name="subscription">The subscription which received the update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="delta">The order book delata generated from the update.</param>
		/// <param name="fullBook">The full order book</param>
		void onBookClear (
			MamdaSubscription        subscription,
			MamdaOrderBookListener   listener,
			MamaMsg                  msg,
			MamdaOrderBookDelta      delta,
			MamdaOrderBookRecap      fullBook);

		/// <summary>
		/// Method invoked when a gap in order book updates is discovered.
		/// </summary>
		/// <param name="subscription">The subscription which received the update.</param>
		/// <param name="listener">The listener which invoked this callback.</param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="gapEvent">Access to details on the order book gap event.</param>
		/// <param name="fullBook">The full order book</param>
		void onBookGap (
			MamdaSubscription        subscription,
			MamdaOrderBookListener   listener,
			MamaMsg                  msg,
			MamdaOrderBookGap        gapEvent,
			MamdaOrderBookRecap      fullBook);

	}
}
