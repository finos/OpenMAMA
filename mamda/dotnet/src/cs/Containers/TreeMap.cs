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
	public class TreeMap : SortedMap, ICloneable
	{
		public TreeMap() : this(new DefaultComparator())
		{
		}

		public TreeMap(Comparator c) : this(new RedBlackTree(c))
		{
		}

		public TreeMap(Map m) : this()
		{
			putAll(m);
		}

		public TreeMap(SortedMap m)
		{
			mBackingStore = m;
		}

		#region SortedMap Members

		public Comparator comparator()
		{
			return mBackingStore.comparator();
		}

		public SortedMap subMap(object fromKey, object toKey)
		{
			return mBackingStore.subMap(fromKey, toKey);
		}

		public SortedMap headMap(object toKey)
		{
			return mBackingStore.headMap(toKey);
		}

		public SortedMap tailMap(object fromKey)
		{
			return mBackingStore.tailMap(fromKey);
		}

		public object firstKey()
		{
			return mBackingStore.firstKey();
		}

		public object lastKey()
		{
			return mBackingStore.lastKey();
		}

		#endregion

		#region Map Members

		public int size()
		{
			return mBackingStore.size();
		}

		public bool isEmpty()
		{
			return mBackingStore.isEmpty();
		}

		public bool containsKey(object key)
		{
			return mBackingStore.containsKey(key);
		}

		public bool containsValue(object value)
		{
			return mBackingStore.containsValue(value);
		}

		public object valueOf(object key)
		{
			return mBackingStore.valueOf(key);
		}

		public object put(object key, object value)
		{
			return mBackingStore.put(key, value);
		}

		public object remove(object key)
		{
			return mBackingStore.remove(key);
		}

		public void putAll(Map m)
		{
			mBackingStore.putAll(m);
		}

		public void clear()
		{
			mBackingStore.clear();
		}

		public Set keySet()
		{
			return mBackingStore.keySet();
		}

		public Collection values()
		{
			return mBackingStore.values();
		}

		public Set entrySet()
		{
			return mBackingStore.entrySet();
		}

		#endregion

		#region ICloneable Members

		public object Clone()
		{
			// the cast to Map is necessary, otherwise TreeMap will just
			// get backed by the original map, instead of copying it
			TreeMap copy = new TreeMap((Map)mBackingStore);
			return copy;
		}

		#endregion

		#region Implementation details

		private SortedMap mBackingStore;

		#endregion
	}
}
