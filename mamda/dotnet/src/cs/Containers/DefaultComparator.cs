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

namespace Wombat.Containers
{
	public class DefaultComparator : Comparator
	{
		#region Comparator Members

		public int compare(object o1, object o2)
		{
			if (Object.ReferenceEquals(o1, o2) || (o1 == null && o2 == null))
				return 0;

			Comparable c1 = o1 as Comparable;
			Comparable c2 = o2 as Comparable;
			if (c1 != null || c2 != null)
			{
				if (c1 != null)
			return c1.compareTo(c2);
				else if (c2 != null)
					return -c2.compareTo(c1);
			}

			IComparable ic1 = o1 as IComparable;
			IComparable ic2 = o2 as IComparable;
			if (ic1 != null || ic2 != null)
			{
				if (ic1 != null)
					return ic1.CompareTo(ic2);
				else if (ic2 != null)
					return -ic2.CompareTo(ic1);
			}

			throw new InvalidOperationException();
		}

		#endregion
	}
}
