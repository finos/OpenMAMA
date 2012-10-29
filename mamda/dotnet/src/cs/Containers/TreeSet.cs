/* $Id: TreeSet.cs,v 1.3.32.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
	// implemented on top of TreeMap
	public class TreeSet : SortedSet, ICloneable
	{
		private TreeSet(SortedMap m)
		{
			mSortedMap = m;
			mKeySet = m.keySet();
		}
		
		public TreeSet() : this(new TreeMap())
		{
		}
		
		public TreeSet(Comparator c) : this(new TreeMap(c))
		{
		}
		
		public TreeSet(Collection c) : this()
		{
			addAll(c);
		}
		
		public TreeSet(SortedSet s) : this(s.comparator())
		{
			addAll(s);
		}
		
		#region SortedSet Members

		public Comparator comparator()
		{
			return mSortedMap.comparator();
		}

		public SortedSet subSet(object from, object to)
		{
			return new TreeSet(mSortedMap.subMap(from, to));
		}

		public SortedSet headSet(object to)
		{
			return new TreeSet(mSortedMap.headMap(to));
		}

		public SortedSet tailSet(object from)
		{
			return new TreeSet(mSortedMap.tailMap(from));
		}

		public object first()
		{
			return mSortedMap.firstKey();
		}

		public object last()
		{
			return mSortedMap.lastKey();
		}

		#endregion

		#region Collection Members

		public int size()
		{
			return mSortedMap.size();
		}

		public bool isEmpty()
		{
			return mSortedMap.isEmpty();
		}

		public bool contains(object o)
		{
			return mSortedMap.containsKey(o);
		}

		public object[] toArray()
		{
			return mKeySet.toArray();
		}

		public bool add(object o)
		{
			return mSortedMap.put(o, PRESENT) == null;
		}

		public bool remove(object o)
		{
			return mSortedMap.remove(o) == PRESENT;
		}

		public bool containsAll(Collection c)
		{
			Iterator i = c.iterator();
			while (i.hasNext())
			{
				object o = i.next();
				if (!contains(o))
					return false;
			}
			return true;
		}

		public bool addAll(Collection c)
		{
			int count = size();
			Iterator i = c.iterator();
			while (i.hasNext())
			{
				object o = i.next();
				add(o);
			}
			return count != size();
		}

		public bool removeAll(Collection c)
		{
			int count = size();
			Iterator i = c.iterator();
			while (i.hasNext())
			{
				object o = i.next();
				remove(o);
			}
			return count != size();
		}

		public bool retainAll(Collection c)
		{
			int count = size();
			Iterator i = c.iterator();
			while (i.hasNext())
			{
				object o = i.next();
				if (!contains(o))
					add(o);
				else
					remove(o);
			}
			return count != size();
		}

		public void clear()
		{
			mSortedMap.clear();
		}

		#endregion

		#region Iterable Members

		public Iterator iterator()
		{
			return mKeySet.iterator();
		}

		#endregion
	
		#region ICloneable Members

		/// <summary>
		/// Creates a deep clone of this TreeSet
		/// </summary>
		/// <returns></returns>
		public object Clone()
		{
			// the cast to Map is necessary, otherwise TreeMap will just 
			// get backed by the original map, instead of copying it
			TreeSet copy = new TreeSet(new TreeMap((Map)mSortedMap));
			return copy;
		}

		#endregion

		private SortedMap mSortedMap;
		private Set mKeySet;
		private static readonly object PRESENT = new object();
	}
}
