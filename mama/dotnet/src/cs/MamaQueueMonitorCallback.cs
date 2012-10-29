/* $Id: MamaQueueMonitorCallback.cs,v 1.3.42.5 2012/08/24 16:12:01 clintonmcdowell Exp $
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
    /// Callback interface to receive notifications regarding 
    /// <see cref="MamaQueue"/> events
	public interface MamaQueueMonitorCallback
	{
		/// <summary>
		/// Callback invoked if an upper size limit has been specified for a queue and
		/// that limit has been exceeded.
		/// </summary>
		void HighWatermarkExceeded(MamaQueue mamaQueue,
			int size,
			object closure);

		/// <summary>
		/// Callback invoked when the queue size returns to the lower limit specified.
		/// Only Wombat TCP middleware supports low water mark callbacks.
		/// </summary>
		void LowWatermark(MamaQueue mamaQueue,
			int size,
			object closure);
	}
}
