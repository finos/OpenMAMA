/* $Id: MamdaOptionExpirationDateSet.cs,v 1.2.38.5 2012/08/24 16:12:12 clintonmcdowell Exp $
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
using System.Text;
using Wombat.Containers;

namespace Wombat
{
	/// <summary>
	/// A class that represents a set of expiration dates, each of which
	/// contains a set of strike prices (MamdaOptionStrikeSet), each of
	/// which contains a set of option contracts (MamdaOptionContractSet),
	/// each of which contains exchange-specific contracts.  To access a
	/// set of strike prices for a given expiration date, use the get
	/// method (inherited from TreeMap).
	/// </summary>
	public class MamdaOptionExpirationDateSet : TreeMap
	{
		public MamdaOptionExpirationDateSet()
		{
		}

		public MamdaOptionExpirationDateSet(MamdaOptionExpirationDateSet  copy)
		{
			copyStrikes(copy);
		}

		protected MamdaOptionExpirationDateSet(SortedMap map)
		{
			copyStrikes(map);
		}

		public MamdaOptionExpirationDateSet getExpirationsBefore(DateTime date)
		{
			SortedMap result = headMap(date.Date);
			return new MamdaOptionExpirationDateSet(result);
		}

		public MamdaOptionExpirationDateSet getExpirations(int numExpirations)
		{
			Iterator keyIter = keySet().iterator();
			DateTime date = DateTime.MinValue;
			while (keyIter.hasNext() && (numExpirations >= 0))
			{
				date = (DateTime)keyIter.next();
				--numExpirations;
			}
			if ((numExpirations == -1) && (date != DateTime.MinValue))
			{
				return getExpirationsBefore(date);
			}
			else
			{
				return new MamdaOptionExpirationDateSet(this);
			}
		}

		public String toString()
		{
			Set entries = entrySet();
			Iterator entryIter = entries.iterator();
			StringBuilder result = new StringBuilder();
			while (entryIter.hasNext())
			{
				MapEntry entry = (MapEntry)entryIter.next();
				DateTime expireDate = (DateTime)entry.getKey();
				MamdaOptionExpirationStrikes strikes = (MamdaOptionExpirationStrikes)entry.getValue();
				result.AppendFormat("\n {0}={1}", mDateFormat.Format(expireDate), strikes);
			}
			return result.ToString();
		}
		
		public override string ToString()
		{
			return toString();
		}

		private void copyStrikes(SortedMap copy)
		{
			// Shallow copy is not good enough.  We want a copy that deep
			// copies the MamdaOptionExpirationStrikes, but not the
			// contents of those strike sets.
			DateTime today = DateTime.Today;
			Set entries = copy.entrySet();
			Iterator entryIter = entries.iterator();
			while (entryIter.hasNext())
			{
				MapEntry entry = (MapEntry)entryIter.next();
				DateTime expireDate = (DateTime)entry.getKey();
				//  Need to uncomment the following when we have a newer playback file
				//  to properly test with!
				//          if (expireDate > today)
				{
					MamdaOptionExpirationStrikes strikes = (MamdaOptionExpirationStrikes)entry.getValue();
					put(expireDate, new MamdaOptionExpirationStrikes(strikes));
				}
			}
		}

		private static SimpleDateFormat mDateFormat = new SimpleDateFormat("MMMyy");
	}
}
