/* $Id: MamdaOptionSeriesUpdate.cs,v 1.2.38.5 2012/08/24 16:12:12 clintonmcdowell Exp $
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
	public enum MamdaOptionAction : byte
	{
		Unknown	= (byte)' ',
		Add		= (byte)'A',
		Delete	= (byte)'D'
	}

	/// <summary>
	/// MamdaOptionSeriesUpdate is an interface that provides access to
	/// fields related to option series update events.  Update events
	/// include adds/removes of contracts to the chain.
	/// </summary>
	public interface MamdaOptionSeriesUpdate : MamdaBasicEvent
	{
		/// <summary>
		/// Get the option contract to which the most recent event applies.
		/// </summary>
		/// <returns>The option contract to which the most recent event
		/// applies.  If the contract is new, it will have already been
		/// added to the chain.  If it is being removed, it will have
		/// already been removed from the chain.
		/// </returns>
		MamdaOptionContract getOptionContract();

		/// <summary>
		/// Get the most recent action.
		/// </summary>
		/// <returns>The action related to the last series update message.</returns>
		MamdaOptionAction getOptionAction();
	}
}
