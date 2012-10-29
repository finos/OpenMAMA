/* $Id: MamdaOptionExpirationStrikes.cs,v 1.2.38.5 2012/08/24 16:12:12 clintonmcdowell Exp $
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
using Wombat.Containers;

namespace Wombat
{
	/// <summary>
	/// A class that represents a set of strike prices at a particlar
	/// expiration date.  Each strike price of which contains a set of
	/// option contracts, each of which contains exchange-specific
	/// contracts.  To access a contract set for a given strike price, use
	/// the get method (inherited from TreeMap).
	/// </summary>
	public class MamdaOptionExpirationStrikes : TreeMap
	{
		public MamdaOptionExpirationStrikes()
		{
		}

		public MamdaOptionExpirationStrikes(MamdaOptionExpirationStrikes copy) : base(copy)
		{
			// Shallow copy.
		}

		/// <summary>
		/// Trim the current set of strike prices to the given set.
		/// </summary>
		/// <param name="strikeSet"></param>
		public void trimStrikes(SortedSet strikeSet)
		{
			double lowStrike = (double)strikeSet.first();
			double highStrike = (double)strikeSet.last();
			highStrike = highStrike + 0.0001;
			TreeMap trimmedStrikes = new TreeMap(subMap(lowStrike, highStrike));
			clear();
			putAll(trimmedStrikes);
		}
	}
}
