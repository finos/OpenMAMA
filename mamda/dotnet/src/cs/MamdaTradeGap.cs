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
	/// MamdaTradeGap is an interface that provides access to fields
	/// related to gap in trade reporting.
	/// </summary>
	public interface MamdaTradeGap : MamdaBasicEvent
	{
		/// <summary>
		/// The starting sequence number of detected missing trades based on
		/// the trade count.
		/// </summary>
		/// <returns></returns>
		long getBeginGapSeqNum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getBeginGapSeqNumFieldState();

		/// <summary>
		/// The end sequence number of detected missing trades based on the
		/// trade count.
		/// </summary>
		/// <returns></returns>
		long getEndGapSeqNum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getEndGapSeqNumFieldState();
	}
}

