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
	/// MamdaBasicRecap is an interface that provides access to common
	/// recap-related fields.
	/// </summary>
	public interface MamdaBasicRecap
	{
		/// <summary>
		/// Symbol.  This is the "well-known" symbol for the
		/// security, including any symbology mapping performed by the
		/// publisher.
		/// </summary>
		string getSymbol();

        /// <summary>
        /// Returns the field state.
        /// </summary>
        /// <returns>Symbol Field State</returns>
        MamdaFieldState getSymbolFieldState();

		/// <summary>
		/// Participant ID.  This may be an exchange identifier, a
		/// market maker ID, etc., or NULL (if this is not related to any
		/// specific participant).
		/// </summary>
		string getPartId();

        /// <summary>
        /// Returns the field state.
        /// </summary>
        /// <returns>Participant ID Field State</returns>
        MamdaFieldState getPartIdFieldState();

		/// <summary>
		/// Source time.  Typically, the exchange generated feed
		/// time stamp.  This is often the same as the "event time",
		/// because many feeds do not distinguish between the
		/// actual event time and when the exchange sent the message.
		/// </summary>
		DateTime getSrcTime();

        /// <summary>
        /// Returns the field state.
        /// </summary>
        /// <returns>Source time Field State</returns>
        MamdaFieldState getSrcTimeFieldState();

		/// <summary>
		/// Activity time.  A feed handler generated time stamp
		/// representing when the data item was last updated.
		/// </summary>
		/// <returns></returns>
		DateTime getActivityTime();

        /// <summary>
        /// Returns the field state.
        /// </summary>
        /// <returns>Activity Time Field State</returns>
        MamdaFieldState getActivityTimeFieldState();

		/// <summary>
		/// Line time.  A feed handler (or similar publisher) time
		/// stamp representing the time that such publisher received the
		/// update message pertaining to the event.  If clocks are properly
		/// synchronized and the source time (see above) is accurate
		/// enough, then the difference between the source time and line
		/// time is the latency between the data source and the feed
		/// handler.
		/// </summary>
		DateTime getLineTime();

        /// <summary>
        /// Returns the field state.
        /// </summary>
        /// <returns>Line Time Field State</returns>
        MamdaFieldState getLineTimeFieldState();

		/// <summary>
		/// Send time.  A feed handler (or similar publisher) time
		/// stamp representing the time that such publisher sent the
		/// current message.  The difference between the line time and send
		/// time is the latency within the feed handler itself.  Also, if
		/// clocks are properly synchronized then the difference between
		/// the send time and current time is the latency within the market
		/// data distribution framework (i.e. MAMA and the underlying
		/// middleware).  See MAMA API: MamaDateTime::currentTime()).
		/// </summary>
		DateTime getSendTime();

        /// <summary>
        /// Returns the field state.
        /// </summary>
        /// <returns>Send time Field State</returns>
        MamdaFieldState getSendTimeFieldState();
	}
}
