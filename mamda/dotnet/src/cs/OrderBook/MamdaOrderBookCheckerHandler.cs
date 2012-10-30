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
	/// MamdaOrderBookCheckerHandler is an interface for applications 
	/// that want to handle the results of the MamdaOrderBookChecker. Callback
	/// interfaces are provided for correct and erroneous checks. 
	/// </summary>
	public interface MamdaOrderBookCheckerHandler
	{
		/// <summary>
		/// Method invoked when a successful check is completed.
		/// </summary>
		/// <param name="checkType">Types of MamdaOrderBookChecks.</param>
		void onSuccess(MamdaOrderBookCheckType checkType);

		/// <summary>
		/// Method invoked when check is completed inconclusively.  An
		/// attempt to check the order book may be inconclusive if the
		/// order book sequence numbers do not match up.
		/// </summary>
		/// <param name="checkType">Types of MamdaOrderBookChecks.</param>
		/// <param name="reason">Reason for the sequence numbers mismatch.</param>
		void onInconclusive(
			MamdaOrderBookCheckType checkType,
			string reason);

		/// <summary>
		/// Method invoked when a failed check is completed.  The message
		/// provided, if non-NULL, is the one received for the snapshot or
		/// delta, depending upon the value of checkType.
		/// </summary>
		/// <param name="checkType">Types of MamdaOrderBookChecks.</param>
		/// <param name="reason">Reason for the failure.</param>
		/// <param name="msg">The MamaMsg which caused failure.</param>
		/// <param name="realTimeBook">The Real Time Order Book which is compared against.</param>
		/// <param name="checkBook">The Aggregated Book which is compared.</param>
		void onFailure(
			MamdaOrderBookCheckType checkType,
			string reason,
			MamaMsg msg,
			MamdaOrderBook realTimeBook,
            MamdaOrderBook checkBook);
	}
}
