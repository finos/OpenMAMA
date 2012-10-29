/* $Id: MamdaStaleListener.cs,v 1.1.40.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
	/// MamdaStaleListener defines an interface for handling changes in
	/// quality of the subscription through notifications for a MamdaSubscription.
	/// </summary>
	public interface MamdaStaleListener
	{
		/// <summary>
		/// Invoked in response to the onQuality() callback for the
		/// underlying MamaSubscription.
		/// Typically invoked in response to an underlying symbol
		/// level sequence number gap being detected.
		/// </summary>
		/// <param name="subscription">The <c>MamdaSubscription</c> on which the data
		/// quality event fired.</param>
		/// <param name="quality">The quality of the subscription - see <c>MamaQuality</c></param>
		void onStale(
			MamdaSubscription subscription,
			mamaQuality quality);
	}
}
