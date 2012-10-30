/* $Id: MamdaSecurityStatusHandler.cs,v 1.2.38.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
	/// MamdaSecurityStatusHandler is an interface for applications that want to
	/// have an easy way to handle security status updates.  The interface defines
	/// callback methods for status update and recap events.
	/// </summary>
	public interface MamdaSecurityStatusHandler
	{
		/// <summary>
		/// Method invoked when the current security status information for the
		/// security is available.  The reason for the invocation may be
		/// any of the following:
		/// - Initial image.
		/// - Recap update (e.g., after server fault tolerant event or data
		///   quality event.)
		/// </summary>
		/// <param name="subscription"></param>
		/// <param name="listener"></param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="recap"></param>
		void onSecurityStatusRecap(
			MamdaSubscription            subscription,
			MamdaSecurityStatusListener  listener,
			MamaMsg                      msg,
			MamdaSecurityStatusRecap     recap);

		/// <summary>
		/// Method invoked when the current security status information has changed.
		/// </summary>
		/// <param name="subscription"></param>
		/// <param name="listener"></param>
		/// <param name="msg">The MamaMsg that triggered this invocation.</param>
		/// <param name="update"></param>
		/// <param name="recap"></param>
		void onSecurityStatusUpdate(
			MamdaSubscription            subscription,
			MamdaSecurityStatusListener  listener,
			MamaMsg                      msg,
			MamdaSecurityStatusUpdate    update,
			MamdaSecurityStatusRecap     recap);
	}
}
