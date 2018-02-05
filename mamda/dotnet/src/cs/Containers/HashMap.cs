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
using System.Collections;

namespace Wombat.Containers
{
	public class HashMap : Map, ICloneable
	{
		public HashMap(int initialCapacity, float loadFactor)
		{
			mHashTable = new Hashtable(initialCapacity, loadFactor);
		}

		public HashMap(int initialCapacity)
		{
			mHashTable = new Hashtable(initialCapacity);
		}

		public HashMap()
		{
			mHashTable = new Hashtable();
		}

		public HashMap(HashMap m)
		{
			mHashTable = new Hashtable(m.mHashTable);
		}

		internal HashMap(Hashtable ht)
		{
			mHashTable = ht;
		}

		#region Map Members

		public int size()
		{
			return mHashTable.Count;
		}

		public bool isEmpty()
		{
			return size() == 0;
		}

		public bool containsKey(object key)
		{
			return mHashTable.ContainsKey(key);
		}

		public bool containsValue(object value)
		{
			return mHashTable.ContainsValue(value);
		}

		public object valueOf(object key)
		{
			return mHashTable[key];
		}

		public object put(object key, object value)
		{
			object previous = mHashTable[key];
			mHashTable[key] = value;
			return previous;
		}

		public object remove(object key)
		{
			object previous = mHashTable[key];
			mHashTable.Remove(key);
			return previous;
		}

		public void putAll(Map m)
		{
			Set s = m.entrySet();
			Iterator i = s.iterator();
			while (i.hasNext())
			{
				MapEntry e = (MapEntry)i.next();
				putFast(e.getKey(), e.getValue());
			}
		}

		public void clear()
		{
			mHashTable.Clear();
		}

		public Set keySet()
		{
			return new HashMapSetOverHashtableKeys(mHashTable);
		}

		public Collection values()
		{
			return new HashMapCollectionOverHashtableValues(mHashTable);
		}

		public Set entrySet()
		{
			return new HashMapSetOverHashtableEntries(this);
		}

		#endregion

		private void putFast(object key, object value)
		{
			mHashTable[key] = value;
		}

		private sealed class HashMapSetOverHashtableKeys : Set
		{
			public HashMapSetOverHashtableKeys(Hashtable ht)
			{
				mHashTable = ht;
			}

			#region Collection Members

			public int size()
			{
				return mHashTable.Count;
			}

			public bool isEmpty()
			{
				return size() == 0;
			}

			public bool contains(object o)
			{
				return mHashTable.ContainsKey(o);
			}

			public object[] toArray()
			{
				ICollection k = mHashTable.Keys;
				object[] keys = new object[k.Count];
				k.CopyTo(keys, 0);
				return keys;
			}

			public bool add(object o)
			{
				throw new NotSupportedException();
			}

			public bool remove(object o)
			{
				throw new NotSupportedException();
			}

			public bool containsAll(Collection c)
			{
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					object key = i.next();
					if (!mHashTable.ContainsKey(key))
						return false;
				}
				return true;
			}

			public bool addAll(Collection c)
			{
				throw new NotSupportedException();
			}

			public bool removeAll(Collection c)
			{
				throw new NotSupportedException();
			}

			public bool retainAll(Collection c)
			{
				throw new NotSupportedException();
			}

			public void clear()
			{
				throw new NotSupportedException();
			}

			#endregion

			#region Iterable Members

			public Iterator iterator()
			{
				return new ReadonlyIteratorOverEnumerator(mHashTable.Keys.GetEnumerator());
			}

			#endregion

			private Hashtable mHashTable;
		}

		private sealed class HashMapCollectionOverHashtableValues : Collection
		{
			public HashMapCollectionOverHashtableValues(Hashtable ht)
			{
				mHashTable = ht;
			}

			#region Collection Members

			public int size()
			{
				return mHashTable.Count;
			}

			public bool isEmpty()
			{
				return size() == 0;
			}

			public bool contains(object o)
			{
				return mHashTable.ContainsValue(o);
			}

			public object[] toArray()
			{
				ICollection values = mHashTable.Values;
				object[] vals = new object[values.Count];
				values.CopyTo(vals, 0);
				return vals;
			}

			public bool add(object o)
			{
				throw new NotSupportedException();
			}

			public bool remove(object o)
			{
				throw new NotSupportedException();
			}

			public bool containsAll(Collection c)
			{
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					object value = i.next();
					if (!mHashTable.ContainsValue(value))
						return false;
				}
				return true;
			}

			public bool addAll(Collection c)
			{
				throw new NotSupportedException();
			}

			public bool removeAll(Collection c)
			{
				throw new NotSupportedException();
			}

			public bool retainAll(Collection c)
			{
				throw new NotSupportedException();
			}

			public void clear()
			{
				throw new NotSupportedException();
			}

			#endregion

			#region Iterable Members

			public Iterator iterator()
			{
				return new ReadonlyIteratorOverEnumerator(mHashTable.Values.GetEnumerator());
			}

			#endregion

			private Hashtable mHashTable;
		}

		private sealed class HashMapSetOverHashtableEntries : Set
		{
			public HashMapSetOverHashtableEntries(HashMap hm)
			{
				mHashMap = hm;
			}

			#region Collection Members

			public int size()
			{
				return mHashMap.mHashTable.Count;
			}

			public bool isEmpty()
			{
				return size() == 0;
			}

			public bool contains(object o)
			{
				MapEntry me = (MapEntry)o;
				object val = mHashMap.mHashTable[me.getKey()];
				return val == me.getValue();
			}

			public object[] toArray()
			{
				object[] vals = new object[mHashMap.mHashTable.Count];
				int i = 0;
				foreach (DictionaryEntry e in mHashMap.mHashTable)
				{
					vals[i++] = new MapEntryOverDictionaryEntry(e);
				}
				return vals;
			}

			public bool add(object o)
			{
				MapEntry me = (MapEntry)o;
				int count = mHashMap.size();
				mHashMap.put(me.getKey(), me.getValue());
				return count != mHashMap.size();
			}

			public bool remove(object o)
			{
				MapEntry me = (MapEntry)o;
				int count = mHashMap.size();
				mHashMap.mHashTable.Remove(me.getKey());
				return count != mHashMap.size();
			}

			public bool containsAll(Collection c)
			{
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					MapEntry me = (MapEntry)i.next();
					if (!mHashMap.mHashTable.ContainsKey(me.getKey()))
						return false;
				}
				return true;
			}

			public bool addAll(Collection c)
			{
				int count = mHashMap.mHashTable.Count;
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					MapEntry me = (MapEntry)i.next();
					mHashMap.mHashTable[me.getKey()] = me.getValue();
				}
				return mHashMap.mHashTable.Count != count;
			}

			public bool removeAll(Collection c)
			{
				int count = mHashMap.mHashTable.Count;
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					MapEntry me = (MapEntry)i.next();
					mHashMap.mHashTable.Remove(me.getKey());
				}
				return mHashMap.mHashTable.Count != count;
			}

			public bool retainAll(Collection c)
			{
				int count = mHashMap.mHashTable.Count;
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					MapEntry me = (MapEntry)i.next();
					object val = mHashMap.mHashTable[me.getKey()];
					if (val != me.getValue())
					{
						if (!mHashMap.mHashTable.ContainsKey(me.getKey()))
							mHashMap.mHashTable.Add(me.getKey(), me.getValue());
						else
							mHashMap.mHashTable.Remove(me.getKey());
					}
				}
				return mHashMap.mHashTable.Count != count;
			}

			public void clear()
			{
				mHashMap.mHashTable.Clear();
			}

			#endregion

			#region Iterable Members

			public Iterator iterator()
			{
				return new MapEntryIteratorOverDictionaryEnumerator(mHashMap.mHashTable);
			}

			private sealed class MapEntryOverDictionaryEntry : MapEntry
			{
				public MapEntryOverDictionaryEntry(DictionaryEntry e)
				{
					mDictEntry = e;
				}

				#region MapEntry Members

				public object getKey()
				{
					return mDictEntry.Key;
				}

				public object getValue()
				{
					return mDictEntry.Value;
				}

				public object setValue(object value)
				{
					object previous = mDictEntry.Value;
					mDictEntry.Value = value;
					return previous;
				}

				#endregion

				private DictionaryEntry mDictEntry;
			}

			private sealed class MapEntryIteratorOverDictionaryEnumerator : Iterator
			{
				public MapEntryIteratorOverDictionaryEnumerator(Hashtable ht)
				{
					mHashTable = ht;
					mDictEntry = mHashTable.GetEnumerator();
				}

				#region Iterator Members

				public bool hasNext()
				{
					if (!mMoved)
					{
						mMoveSucceeded = mDictEntry.MoveNext();
						mMoved = true;
					}
					return mMoveSucceeded;
				}

				public object next()
				{
					mMoved = false;
					return new MapEntryOverDictionaryEntry((DictionaryEntry)mDictEntry.Current);
				}

				public void remove()
				{
					DictionaryEntry de = (DictionaryEntry)mDictEntry.Current;
					mHashTable.Remove(de.Key);
				}

				#endregion

				private Hashtable mHashTable;
				private IDictionaryEnumerator mDictEntry;
				private bool mMoved;
				private bool mMoveSucceeded;
			}

			private HashMap mHashMap;

			#endregion

		}

		private Hashtable mHashTable;

		#region ICloneable Members

		/// <summary>
		/// Creates a *deep* copy of this HashMap instance
		/// </summary>
		/// <returns></returns>
		public object Clone()
		{
			Hashtable copy = new Hashtable(mHashTable);
			return new HashMap(copy);
		}

		#endregion
	}
}
