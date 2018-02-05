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
	/// MamdaBasicEvent is a superclass interface that provides access to
	/// common event related fields.  Events types include trades, quotes,
	/// order book updates, closing summaries, etc.  See the individual
	/// subclasses for specific information about each event type.
	///
	/// Note: Different types of time stamps are available representing the
	/// time that the event (trade ,quote,etc) actually occurred, the time
	/// the data source/exchange reported it, and the time the Wombat feed
	/// handler applied an action to the given data item (record, order
	/// book, etc.).  Many feeds to not provide a distinction between the
	/// event time and the source time (they may be the same) and the
	/// granularity of time stamps also varies between data sources.
	/// </summary>
	public interface MamdaBasicEvent
	{
		/// <summary>
		/// Returns the source time.
		/// </summary>
		/// <returns>Source time.  Typically, the exchange generated feed
		/// * time stamp.  This is often the same as the "event time",
		/// * because many feeds do not distinguish between the actual event
		/// * time and when the exchange sent the message.</returns>
		DateTime getSrcTime();

        /// <summary>
        /// Returns the field state.
        /// </summary>
        /// <returns>Source time Field State</returns>
        MamdaFieldState getSrcTimeFieldState();

        /// <summary>
		/// </summary>
		/// <returns>Activity time.  A feed handler generated time stamp
		/// representing when the data item was last updated.</returns>
		DateTime getActivityTime();

        /// <summary>
        /// Returns the field state.
        /// </summary>
        /// <returns>Activity time Field State</returns>
        MamdaFieldState getActivityTimeFieldState();

		/// <summary>
		/// </summary>
		/// <returns>Source sequence number.  The exchange generated
		/// sequence number.</returns>
		long getEventSeqNum();

        /// <summary>
        /// Returns the field state.
        /// </summary>
        /// <returns>Source sequence number Field State</returns>
        MamdaFieldState getEventSeqNumFieldState();

		/// <summary>
		/// </summary>
		/// <returns>Event time.  Typically, when the event actually
		/// occurred.  This is often the same as the "source time", because
		/// many feeds do not distinguish between the actual event time and
		/// when the exchange sent the message.</returns>
		DateTime getEventTime();

        /// <summary>
        /// Returns the field state.
        /// </summary>
        /// <returns>Event Time Field State</returns>
        MamdaFieldState getEventTimeFieldState();

	}
}
