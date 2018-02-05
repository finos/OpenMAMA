/* $Id: RedBlackTree.cs,v 1.3.30.6 2012/08/24 16:12:10 clintonmcdowell Exp $
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
	// Red-black tree implementation, based on NGenerics, only it
	// implements SortedMap in order to back the TreeMap class
	public class RedBlackTree : SortedMap
	{
		public RedBlackTree() : this(new DefaultComparator())
		{
		}

		public RedBlackTree(Comparator comparator)
		{
			if (comparator == null)
				throw new ArgumentNullException("comparator");

			mComp = comparator;
		}

		#region SortedMap Members

		public Comparator comparator()
		{
			return mComp;
		}

		public SortedMap subMap(object fromKey, object toKey)
		{
			return new SubMap(this, false, fromKey, false, toKey);
		}

		public SortedMap headMap(object toKey)
		{
			return new SubMap(this, true, null, false, toKey);
		}

		public SortedMap tailMap(object fromKey)
		{
			return new SubMap(this, false, fromKey, true, null);
		}

		public object firstKey()
		{
			if (mCount == 0)
				throw new InvalidOperationException();

			return FindMinNode(mRoot).Key;
		}

		internal Node firstEntry()
		{
			if (mCount == 0)
				throw new InvalidOperationException();

			return FindMinNode(mRoot);
		}

		public object lastKey()
		{
			if (mCount == 0)
				throw new InvalidOperationException();

			return FindMaxNode(mRoot).Key;
		}

		internal Node lastEntry()
		{
			if (mCount == 0)
				throw new InvalidOperationException();

			return FindMaxNode(mRoot);
		}

		#endregion

		#region Map Members

		public int size()
		{
			return mCount;
		}

		public bool isEmpty()
		{
			return mCount == 0;
		}

		public bool containsKey(object key)
		{
			return FindNode(key) != null;
		}

		public bool containsValue(object value)
		{
			Collection vals = values();
			Iterator i = vals.iterator();
			while (i.hasNext())
			{
				object val = i.next();
				if (val.Equals(value))
				{
					return true;
				}
			}
			return false;
		}

		public object valueOf(object key)
		{
			Node node = FindNode(key);
			if (node == null)
				return null;
			return node.Value;
		}

		public object put(object key, object value)
		{
			mRoot = InsertNode(mRoot, key, value);
			mRoot.Color = NodeColor.Black;
			++mCount;
			return value;
		}

		public object remove(object key)
		{
			if (mRoot == null)
				return null;

			Node startNode = new Node(new object(), new object());

			Node childNode = startNode;
			startNode.Right = mRoot;

			Node parent = null;
			Node grandParent = null;
			Node foundNode = null;

			bool direction = true;

			while (childNode[direction] != null)
			{
				bool lastDirection = direction;

				grandParent = parent;
				parent = childNode;
				childNode = childNode[direction];

				int cmp = mComp.compare(childNode.Key, key);

				if (cmp == 0)
				{
					foundNode = childNode;
				}

				direction = cmp < 0;

				if ((Node.IsBlack(childNode)) && (Node.IsBlack(childNode[direction])))
				{
					if (Node.IsRed(childNode[!direction]))
					{
						parent = parent[lastDirection] = SingleRotate(childNode, direction);
					}
					else if (Node.IsBlack(childNode[direction]))
					{
						Node sibling = parent[!lastDirection];

						if (sibling != null)
						{
							if ((Node.IsBlack(sibling.Left)) && (Node.IsBlack(sibling.Right)))
							{
								parent.Color = NodeColor.Black;
								sibling.Color = NodeColor.Red;
								childNode.Color = NodeColor.Red;
							}
							else
							{
								bool parentDirection = grandParent.Right == parent;

								if (Node.IsRed(sibling[lastDirection]))
								{
									grandParent[parentDirection] = DoubleRotate(parent, lastDirection);
								}
								else if (Node.IsRed(sibling[!lastDirection]))
								{
									grandParent[parentDirection] = SingleRotate(parent, lastDirection);
								}

								childNode.Color = grandParent[parentDirection].Color = NodeColor.Red;
								grandParent[parentDirection].Left.Color = NodeColor.Black;
								grandParent[parentDirection].Right.Color = NodeColor.Black;
							}
						}
					}
				}
			}

			if (foundNode != null)
			{
				foundNode.Key = childNode.Key;
				parent[parent.Right == childNode] = childNode[childNode.Left == null];
			}

			mRoot = startNode.Right;

			if (mRoot != null)
			{
				mRoot.Color = NodeColor.Black;
			}

			if (foundNode != null)
			{
				--mCount;
				return foundNode.Value;
			}

			return null;
		}

		public void putAll(Map m)
		{
			Iterator i = m.entrySet().iterator();
			while (i.hasNext())
			{
				MapEntry e = (MapEntry)i.next();
				put(e.getKey(), e.getValue());
			}
		}

		public void clear()
		{
			mRoot = null;
			mCount = 0;
		}

		public Set keySet()
		{
			return KeySetOverRedBlackTree();
		}

		public Collection values()
		{
			return ValueCollectionOverRedBlackTree();
		}

		public Set entrySet()
		{
			return EntrySetOverRedBlackTree();
		}

		#endregion

		#region Implementation details

		private Set KeySetOverRedBlackTree()
		{
			return new KeySet(this);
		}

		private Collection ValueCollectionOverRedBlackTree()
		{
			return new ValueCollection(this);
		}

		private Set EntrySetOverRedBlackTree()
		{
			return new EntrySet(this);
		}

		internal Node FindNode(object key)
		{
			if (mRoot == null)
				return null;

			Node current = mRoot;
			while (current != null)
			{
				int cmp = mComp.compare(key, current.Key);
				if (cmp == 0)
					return current;
				else if (cmp < 0)
					current = current.Left;
				else
					current = current.Right;
			}

			return null;
		}

		private static Node FindMaxNode(Node startNode)
		{
			Node searchNode = startNode;
			while (searchNode.Right != null)
				searchNode = searchNode.Right;
			return searchNode;
		}

		private static Node FindMinNode(Node startNode)
		{
			Node searchNode = startNode;
			while (searchNode.Left != null)
				searchNode = searchNode.Left;
			return searchNode;
		}

		internal Node MaxNode()
		{
			return FindMaxNode(mRoot);
		}

		internal Node MinNode()
		{
			return FindMinNode(mRoot);
		}

		private Node InsertNode(Node node, object key, object value)
		{
			if (node == null)
			{
				node = new Node(key, value);
			}
			else if (mComp.compare(key, node.Key) != 0)
			{
				bool direction = mComp.compare(node.Key, key) < 0;
				node[direction] = InsertNode(node[direction], key, value);
				if (Node.IsRed(node[direction]))
				{
					if (Node.IsRed(node[!direction]))
					{
						node.Color = NodeColor.Red;
						node.Left.Color = NodeColor.Black;
						node.Right.Color = NodeColor.Black;
					}
					else
					{
						if (Node.IsRed(node[direction][direction]))
						{
							node = SingleRotate(node, !direction);
						}
						else if (Node.IsRed(node[direction][!direction]))
						{
							node = DoubleRotate(node, !direction);
						}
					}
				}
			}
			return node;
		}

		private static Node SingleRotate(Node node, bool direction)
		{
			Node childSibling = node[!direction];
			node[!direction] = childSibling[direction];
			childSibling[direction] = node;
			node.Color = NodeColor.Red;
			childSibling.Color = NodeColor.Black;
			return childSibling;
		}

		private static Node DoubleRotate(Node node, bool direction)
		{
			node[!direction] = SingleRotate(node[!direction], !direction);
			return SingleRotate(node, direction);
		}

		private Node GetSuccessor(Node node)
		{
			if (node == null)
			{
				return null;
			}
			else if (node.Right != null)
			{
				Node p = node.Right;
				while (p.Left != null)
				{
					p = p.Left;
				}
				return p;
			}
			else
			{
				Node p = GetParent(node);
				Node temp = node;
				while (p != null && temp == p.Right)
				{
					temp = p;
					p = GetParent(p);
				}
				return p;
			}
		}

		internal bool GetSuccessorKey(object key, ref object successorKey)
		{
			Node succNode = GetSuccessor(FindNode(key));
			if (succNode == null)
			{
				successorKey = null;
				return false;
			}
			successorKey = succNode.Key;
			return true;
		}

		internal bool GetSuccessorEntry(MapEntry entry, ref MapEntry successorEntry)
		{
			successorEntry = GetSuccessor((Node)entry);
			return successorEntry != null;
		}

		private Node GetPredecessor(Node node)
		{
			if (node == null)
			{
				return null;
			}
			else if (node.Left != null)
			{
				Node p = node.Left;
				while (p.Right != null)
				{
					p = p.Right;
				}
				return p;
			}
			else
			{
				Node p = GetParent(node);
				Node temp = node;
				while (p != null && temp == p.Left)
				{
					temp = p;
					p = GetParent(p);
				}
				return p;
			}
		}

		internal bool GetPredecessorKey(object key, ref object predecessorKey)
		{
			Node predNode = GetPredecessor(FindNode(key));
			if (predNode == null)
			{
				predecessorKey = null;
				return false;
			}
			predecessorKey = predNode.Key;
			return true;
		}

		internal bool GetPredecessorEntry(MapEntry entry, ref MapEntry predecessorEntry)
		{
			predecessorEntry = GetPredecessor((Node)entry);
			return predecessorEntry != null;
		}

		private Node GetParent(Node node)
		{
			if (mRoot == null || node == mRoot)
			{
				return null;
			}

			object key = node.Key;

			Node current = mRoot;
			while (current != null)
			{
				object left, right;
				if (((left = current.Left) != null && left == node) ||
					((right = current.Right) != null && right == node))
				{
					return current;
				}

				int cmp = mComp.compare(key, current.Key);
				System.Diagnostics.Debug.Assert(cmp != 0);
				if (cmp < 0)
					current = current.Left;
				else
					current = current.Right;
			}

			System.Diagnostics.Debug.Assert(false);
			return null;
		}

		internal bool ContainsEntry(MapEntry entry)
		{
			Node node = FindNode(entry.getKey());
			return node != null && node.Value.Equals(entry.getValue());
		}

		#region SubMap

		private class SubMap : SortedMap
		{
			public SubMap(SortedMap source, bool fromStart, object fromKey, bool toEnd, object toKey)
			{
				mSource = source;
				mFromStart = fromStart;
				mFromKey = fromKey;
				mToEnd = toEnd;
				mToKey = toKey;
			}

			#region SortedMap Members

			public Comparator comparator()
			{
				return mSource.comparator();
			}

			public SortedMap subMap(object from, object to)
			{
				return new SubMap(this, false, from, false, to);
			}

			public SortedMap headMap(object toKey)
			{
				return new SubMap(this, true, null, false, toKey);
			}

			public SortedMap tailMap(object fromKey)
			{
				return new SubMap(this, false, fromKey, true, null);
			}

			public object firstKey()
			{
				return mFromStart ? mSource.firstKey() : mFromKey;
			}

			public object lastKey()
			{
				return mToEnd ? mSource.lastKey() : mToKey;
			}

			#endregion

			#region Map Members

			public int size()
			{
				int count = 0;
				RedBlackTree tree = mSource as RedBlackTree;
				if (tree != null)
				{
					if (tree.size() == 0)
						return 0;

					Node startNode;
					if (mFromStart)
					{
						startNode = tree.MinNode();
					}
					else
					{
						startNode = tree.FindNode(mFromKey);
						if (startNode == null) // no exact match, find closest
						{
							startNode = tree.MinNode();
							while (startNode != null && !isKeyInRange(startNode.getKey()))
								startNode = tree.GetSuccessor((Node)startNode);
						}
					}

					Node current = startNode;
					do
					{
						++count;
						current = tree.GetSuccessor(current);
					}
					while (current != null && isKeyInRange(current.Key));
					return count;
				}
				// any other SortedMap
				Iterator i = mSource.keySet().iterator();
				object key = null;
				bool inRange = false;
				while (i.hasNext() && (!(inRange = isKeyInRange((key = i.next())))))
					;
				if (inRange)
				{
					do
					{
						++count;
					}
					while (i.hasNext() && (isKeyInRange((key = i.next()))));
				}
				return count;
			}

			public bool isEmpty()
			{
				return size() == 0;
			}

			public bool containsKey(object key)
			{
				ensureKeyInRange(key);
				return mSource.containsKey(key);
			}

			public bool containsValue(object value)
			{
				RedBlackTree tree = mSource as RedBlackTree;
				if (tree != null)
				{
					if (tree.size() == 0)
						return false;

					Node startNode;
					if (mFromStart)
					{
						startNode = tree.MinNode();
					}
					else
					{
						startNode = tree.FindNode(mFromKey);
						if (startNode == null) // no exact match, find closest
						{
							startNode = tree.MinNode();
							while (startNode != null && !isKeyInRange(startNode.getKey()))
								startNode = tree.GetSuccessor((Node)startNode);
						}
					}

					Node current = startNode;
					do
					{
						if (current.Value.Equals(value))
						{
							return true;
						}
						current = tree.GetSuccessor(current);
					}
					while (current != null && isKeyInRange(current.Key));
					return false;
				}
				// any other SortedMap
				Iterator i = mSource.entrySet().iterator();
				while (i.hasNext())
				{
					MapEntry e = (MapEntry)i.next();
					if (isKeyInRange(e.getKey()) && e.getValue().Equals(value))
						return true;
				}
				return false;
			}

			public object valueOf(object key)
			{
				ensureKeyInRange(key);
				return mSource.valueOf(key);
			}

			public object put(object key, object value)
			{
				ensureKeyInRange(key);
				return mSource.put(key, value);
			}

			public object remove(object key)
			{
				ensureKeyInRange(key);
				return mSource.remove(key);
			}

			public void putAll(Map m)
			{
				Iterator i = m.entrySet().iterator();
				while (i.hasNext())
				{
					MapEntry e = (MapEntry)i.next();
					ensureKeyInRange(e.getKey());
					mSource.put(e.getKey(), e.getValue());
				}
			}

			public void clear()
			{
				Iterator i = mSource.keySet().iterator();
				while (i.hasNext())
				{
					object key = i.next();
					if (isKeyInRange(key))
					{
						i.remove();
					}
				}
			}

			public Set keySet()
			{
				return new SubMapKeySet(this);
			}

			public Collection values()
			{
				ArrayListCollection result = new ArrayListCollection();
				RedBlackTree tree = mSource as RedBlackTree;
				if (tree != null)
				{
					if (tree.size() == 0)
						return result;

					Node startNode;
					if (mFromStart)
					{
						startNode = tree.MinNode();
					}
					else
					{
						startNode = tree.FindNode(mFromKey);
						if (startNode == null) // no exact match, find closest
						{
							startNode = tree.MinNode();
							while (startNode != null && !isKeyInRange(startNode.getKey()))
								startNode = tree.GetSuccessor((Node)startNode);
						}
					}

					Node current = startNode;
					do
					{
						result.add(current.Value);
						current = tree.GetSuccessor(current);
					}
					while (current != null && isKeyInRange(current.Key));
					return result;
				}
				// any other SortedMap
				Iterator i = mSource.entrySet().iterator();
				bool inRange = false;
				MapEntry e = null;
				while (i.hasNext() && (!(inRange = isKeyInRange((e = (MapEntry)i.next()).getKey()))))
					;
				if (inRange)
				{
					do
					{
						result.add(e.getValue());
					} while (i.hasNext() && (inRange = isKeyInRange((e = (MapEntry)i.next()).getKey())));
				}
				return result;
			}

			public Set entrySet()
			{
				return new SubMapEntrySet(this);
			}

			#endregion

			internal void ensureKeyInRange(object key)
			{
				if (!isKeyInRange(key))
				{
					throw new InvalidCastException();
				}
			}

			internal bool isKeyInRange(object key)
			{
				Comparator c = mSource.comparator();
				if (!mFromStart && c.compare(key, mFromKey) < 0)
					return false;
				if (!mToEnd && c.compare(key, mToKey) >= 0)
					return false;
				return true;
			}

			private class SubMapEntrySet : Set
			{
				public SubMapEntrySet(SubMap source)
				{
					mSource = source;
				}

				#region Collection Members

				public int size()
				{
					return mSource.size();
				}

				public bool isEmpty()
				{
					return mSource.isEmpty();
				}

				public bool contains(object o)
				{
					MapEntry e = (MapEntry)o;
					return mSource.containsKey(e.getKey());
				}

				public object[] toArray()
				{
					ArrayList list = new ArrayList();
					Iterator i = iterator();
					while (i.hasNext())
					{
						MapEntry e = (MapEntry)i.next();
						list.Add(e);
					}
					return list.ToArray();
				}

				public bool add(object o)
				{
					MapEntry e = (MapEntry)o;
					int count = mSource.size();
					mSource.put(e.getKey(), e.getValue());
					return count != mSource.size();
				}

				public bool remove(object o)
				{
					MapEntry e = (MapEntry)o;
					int count = mSource.size();
					mSource.remove(e.getKey());
					return count != mSource.size();
				}

				public bool containsAll(Collection c)
				{
					Iterator i = c.iterator();
					while (i.hasNext())
					{
						MapEntry e = (MapEntry)i.next();
						if (!mSource.containsKey(e.getKey()))
							return false;
					}
					return true;
				}

				public bool addAll(Collection c)
				{
					int count = mSource.size();
					Iterator i = c.iterator();
					while (i.hasNext())
					{
						MapEntry e = (MapEntry)i.next();
						mSource.put(e.getKey(), e.getValue());
					}
					return count != mSource.size();
				}

				public bool removeAll(Collection c)
				{
					int count = mSource.size();
					Iterator i = c.iterator();
					while (i.hasNext())
					{
						MapEntry e = (MapEntry)i.next();
						mSource.remove(e.getKey());
					}
					return count != mSource.size();
				}

				public bool retainAll(Collection c)
				{
					throw new NotSupportedException();
				}

				public void clear()
				{
					mSource.clear();
				}

				#endregion

				#region Iterable Members

				public Iterator iterator()
				{
					return new SubMapEntrySetIterator(this);
				}

				#endregion

				private class SubMapEntrySetIterator : Iterator
				{
					public SubMapEntrySetIterator(SubMapEntrySet entrySet)
					{
						mMapEntrySet = entrySet;
					}

					#region Iterator Members

					public bool hasNext()
					{
						moveNext();
						return mMoveSucceeded;
					}

					public object next()
					{
						moveNext();
						mMoved = false;
						if (mMoveSucceeded)
						{
							mMoveSucceeded = false;
							return mNextEntry;
						}
						return null;
					}

					public void remove()
					{
						mMapEntrySet.remove(mNextEntry);
						mMoved = false;
						moveNext();
					}

					#endregion

					private void moveNext()
					{
						if (mMoved)
							return;
						mMoved = true;
						if (!mInitialized)
						{
							mInitialized = true;
							RedBlackTree tree = mMapEntrySet.mSource.mSource as RedBlackTree;
							if (tree != null)
							{
								if (tree.size() > 0)
								{
									MapEntry e;
									if (mMapEntrySet.mSource.mFromStart)
									{
										e = tree.MinNode();
									}
									else
									{
										e = tree.FindNode(mMapEntrySet.mSource.mFromKey);
										if (e == null) // no exact match, find closest
										{
											e = tree.MinNode();
											while (e != null && !mMapEntrySet.mSource.isKeyInRange(e.getKey()))
												e = tree.GetSuccessor((Node)e);
										}
									}
									if (mMapEntrySet.mSource.isKeyInRange(e.getKey()))
									{
										mNextEntry = e;
										mMoveSucceeded = true;
									}
								}
							}
							else
							{
								// any other SortedMap
								if (!mMapEntrySet.isEmpty())
								{
									mIt = mMapEntrySet.mSource.mSource.entrySet().iterator();
									MapEntry e = null;
									while (mMoveSucceeded = mIt.hasNext() && (!mMapEntrySet.mSource.isKeyInRange((e = (MapEntry)mIt.next()).getKey())))
										;
									bool inRange = e != null && mMapEntrySet.mSource.isKeyInRange(e.getKey());
									mMoveSucceeded = inRange;
									if (mMoveSucceeded)
										mNextEntry = e;
								}
							}
						}
						else
						{
							RedBlackTree tree = mMapEntrySet.mSource.mSource as RedBlackTree;
							if (tree != null)
							{
								MapEntry e = tree.GetSuccessor((Node)mNextEntry);
								mMoveSucceeded = e != null && mMapEntrySet.mSource.isKeyInRange(e.getKey());
								if (mMoveSucceeded)
									mNextEntry = e;
							}
							else
							{
								// any other SortedMap
								MapEntry e = null;
								while (mMoveSucceeded = mIt.hasNext() && (!mMapEntrySet.mSource.isKeyInRange((e = (MapEntry)mIt.next()).getKey())))
									;
								bool inRange = e != null && mMapEntrySet.mSource.isKeyInRange(e.getKey());
								mMoveSucceeded = inRange;
								if (mMoveSucceeded)
									mNextEntry = e;
							}
						}
					}

					private bool mInitialized;
					private SubMapEntrySet mMapEntrySet;
					private Iterator mIt;
					private MapEntry mNextEntry;
					private bool mMoved;
					private bool mMoveSucceeded;
				}

				internal SubMap mSource;
			}

			private class SubMapKeySet : Set
			{
				public SubMapKeySet(SubMap source)
				{
					mSource = source;
				}

				#region Collection Members

				public int size()
				{
					return mSource.size();
				}

				public bool isEmpty()
				{
					return mSource.isEmpty();
				}

				public bool contains(object o)
				{
					return mSource.containsKey(o);
				}

				public object[] toArray()
				{
					ArrayList list = new ArrayList();
					Iterator i = iterator();
					while (i.hasNext())
					{
						object key = i.next();
						list.Add(key);
					}
					return list.ToArray();
				}

				public bool add(object o)
				{
					int count = mSource.size();
					mSource.put(o, null);
					return count != mSource.size();
				}

				public bool remove(object o)
				{
					int count = mSource.size();
					mSource.remove(o);
					return count != mSource.size();
				}

				public bool containsAll(Collection c)
				{
					Iterator i = c.iterator();
					while (i.hasNext())
					{
						object key = i.next();
						if (!mSource.containsKey(key))
							return false;
					}
					return true;
				}

				public bool addAll(Collection c)
				{
					int count = mSource.size();
					Iterator i = c.iterator();
					while (i.hasNext())
					{
						object key = i.next();
						object val = mSource.put(key, null);
						if (val != null)
						{
							mSource.put(key, val);
						}
					}
					return count != mSource.size();
				}

				public bool removeAll(Collection c)
				{
					int count = mSource.size();
					Iterator i = c.iterator();
					while (i.hasNext())
					{
						object key = i.next();
						mSource.remove(key);
					}
					return count != mSource.size();
				}

				public bool retainAll(Collection c)
				{
					throw new NotSupportedException();
				}

				public void clear()
				{
					mSource.clear();
				}

				#endregion

				#region Iterable Members

				public Iterator iterator()
				{
					return new SubMapKeySetIterator(this);
				}

				#endregion

				private class SubMapKeySetIterator : Iterator
				{
					public SubMapKeySetIterator(SubMapKeySet keySet)
					{
						mMapEntrySet = keySet;
					}

					#region Iterator Members

					public bool hasNext()
					{
						moveNext();
						return mMoveSucceeded;
					}

					public object next()
					{
						moveNext();
						mMoved = false;
						if (mMoveSucceeded)
						{
							mMoveSucceeded = false;
							return mNextKey;
						}
						return null;
					}

					public void remove()
					{
						mMapEntrySet.remove(mNextKey);
						mMoved = false;
						moveNext();
					}

					#endregion

					private void moveNext()
					{
						if (mMoved)
							return;
						mMoved = true;
						if (!mInitialized)
						{
							mInitialized = true;
							if (!mMapEntrySet.isEmpty())
							{
								RedBlackTree tree = mMapEntrySet.mSource.mSource as RedBlackTree;
								if (tree != null)
								{
									if (tree.size() > 0)
									{
										object key = null;
										if (mMapEntrySet.mSource.mFromStart)
										{
											key = tree.MinNode().Key;
										}
										else
										{
											MapEntry e = tree.FindNode(mMapEntrySet.mSource.mFromKey);
											if (e == null) // no exact match, find closest
											{
												e = tree.MinNode();
												while (e != null && !mMapEntrySet.mSource.isKeyInRange(e.getKey()))
													e = tree.GetSuccessor((Node)e);
											}
											if (e != null)
												key = e.getKey();
										}
										if (key != null && mMapEntrySet.mSource.isKeyInRange(key))
										{
											mNextKey= key;
											mMoveSucceeded = true;
										}
									}
								}
								else
								{
									// any other SortedMap
									mIt = mMapEntrySet.mSource.mSource.keySet().iterator();
									object key = null;
									while (mMoveSucceeded = mIt.hasNext() && (!mMapEntrySet.mSource.isKeyInRange((key = mIt.next()))))
										;
									bool inRange = key != null && mMapEntrySet.mSource.isKeyInRange(key);
									mMoveSucceeded = inRange;
									if (mMoveSucceeded)
										mNextKey = key;
								}
							}
						}
						else
						{
							RedBlackTree tree = mMapEntrySet.mSource.mSource as RedBlackTree;
							if (tree != null)
							{
								object key = null;
								if (tree.GetSuccessorKey(mNextKey, ref key))
								{
									mMoveSucceeded = mMapEntrySet.mSource.isKeyInRange(key);
									if (mMoveSucceeded)
										mNextKey = key;
								}
							}
							else
							{
								// any other SortedMap
								object key = null;
								while (mMoveSucceeded = mIt.hasNext() && (!mMapEntrySet.mSource.isKeyInRange((key = mIt.next()))))
									;
								bool inRange = key != null && mMapEntrySet.mSource.isKeyInRange(key);
								mMoveSucceeded = inRange;
								if (mMoveSucceeded)
									mNextKey = key;
							}
						}
					}

					private bool mInitialized;
					private SubMapKeySet mMapEntrySet;
					private Iterator mIt;
					private object mNextKey;
					private bool mMoved;
					private bool mMoveSucceeded;
				}

				private SubMap mSource;
			}

			private SortedMap mSource;
			internal bool mFromStart;
			internal object mFromKey;
			private bool mToEnd;
			private object mToKey;
		}

		#endregion

		#region KeySet class

		private class KeySet : Set
		{
			public KeySet(RedBlackTree source)
			{
				mSource = source;
			}

			#region Collection Members

			public int size()
			{
				return mSource.size();
			}

			public bool isEmpty()
			{
				return mSource.isEmpty();
			}

			public bool contains(object o)
			{
				return mSource.containsKey(o);
			}

			public object[] toArray()
			{
				ArrayList list = new ArrayList();
				Iterator i = iterator();
				while (i.hasNext())
				{
					object key = i.next();
					list.Add(key);
				}
				return list.ToArray();
			}

			public bool add(object o)
			{
				int count = mSource.size();
				mSource.put(o, null);
				return mSource.size() != count;
			}

			public bool remove(object o)
			{
				int count = mSource.size();
				mSource.remove(o);
				return mSource.size() != count;
			}

			public bool containsAll(Collection c)
			{
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					object key = i.next();
					if (!mSource.containsKey(key))
						return false;
				}
				return true;
			}

			public bool addAll(Collection c)
			{
				bool modified = false;
				int count = mSource.size();
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					object key = i.next();
					object val = mSource.put(key, null);
					if (val != null)
					{
						modified = true;
					}
				}
				return modified || (mSource.size() != count);
			}

			public bool removeAll(Collection c)
			{
				int count = mSource.size();
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					object key = i.next();
					mSource.remove(key);
				}
				return mSource.size() != count;
			}

			public bool retainAll(Collection c)
			{
				throw new NotSupportedException();
			}

			public void clear()
			{
				mSource.clear();
			}

			#endregion

			#region Iterable Members

			public Iterator iterator()
			{
				return new IteratorOverKeySet(this);
			}

			#endregion

			internal RedBlackTree Source
			{
				get
				{
					return mSource;
				}
			}

			private class IteratorOverKeySet : Iterator
			{
				public IteratorOverKeySet(KeySet keySet)
				{
					mMapEntrySet = keySet;
				}

				#region Iterator Members

				public bool hasNext()
				{
					moveNext();
					return mMoveSucceeded;
				}

				public object next()
				{
					moveNext();
					mMoved = false;
					if (mMoveSucceeded)
					{
						mMoveSucceeded = false;
						return mNextKey;
					}
					return null;
				}

				public void remove()
				{
					mMapEntrySet.remove(mNextKey);
					mMoved = false;
					moveNext();
				}

				private void moveNext()
				{
					if (mMoved)
						return;
					mMoved = true;
					if (!mInitialized)
					{
						mInitialized = true;
						if (!mMapEntrySet.isEmpty())
						{
							mNextKey = mMapEntrySet.Source.firstKey();
							mMoveSucceeded = true;
						}
					}
					else
					{
						mMoveSucceeded = mMapEntrySet.Source.GetSuccessorKey(mNextKey, ref mNextKey);
					}
				}

				#endregion

				private KeySet mMapEntrySet;
				private bool mInitialized, mMoved, mMoveSucceeded;
				private object mNextKey;
			}

			private RedBlackTree mSource;
		}

		#endregion KeySet class

		#region ValueCollection class

		private class ValueCollection : Collection
		{
			public ValueCollection(RedBlackTree source)
			{
				mSource = source;
			}

			#region Collection Members

			public int size()
			{
				return mSource.size();
			}

			public bool isEmpty()
			{
				return mSource.isEmpty();
			}

			public bool contains(object o)
			{
				return mSource.containsValue(o);
			}

			public object[] toArray()
			{
				ArrayList list = new ArrayList();

				Set entrySet = mSource.entrySet();
				Iterator i = entrySet.iterator();
				while (i.hasNext())
				{
					MapEntry e = (MapEntry)i.next();
					list.Add(e.getValue());
				}

				return list.ToArray();
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
					if (!mSource.containsValue(value))
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
				mSource.clear();
			}

			#endregion

			#region Iterable Members

			public Iterator iterator()
			{
				return new ValueIteratorOverEntrySetIterator(mSource.entrySet().iterator());
			}

			#endregion

			private class ValueIteratorOverEntrySetIterator : Iterator
			{
				public ValueIteratorOverEntrySetIterator(Iterator source)
				{
					mSource = source;
				}

				#region Iterator Members

				public bool hasNext()
				{
					return mSource.hasNext();
				}

				public object next()
				{
					MapEntry entry = (MapEntry)mSource.next();
					return entry.getValue();
				}

				public void remove()
				{
					mSource.remove();
				}

				#endregion

				private Iterator mSource;
			}

			private RedBlackTree mSource;
		}

		#endregion ValueCollection class

		#region EntrySet class

		private class EntrySet : Set
		{
			public EntrySet(RedBlackTree source)
			{
				mSource = source;
			}

			#region Collection Members

			public int size()
			{
				return mSource.size();
			}

			public bool isEmpty()
			{
				return mSource.isEmpty();
			}

			public bool contains(object o)
			{
				MapEntry entry = (MapEntry)o;
				return mSource.ContainsEntry(entry);
			}

			public object[] toArray()
			{
				ArrayList list = new ArrayList();
				Iterator i = iterator();
				while (i.hasNext())
				{
					object entry = i.next();
					list.Add(entry);
				}
				return list.ToArray();
			}

			public bool add(object o)
			{
				int count = mSource.size();
				MapEntry entry = (MapEntry)o;
				mSource.put(entry.getKey(), entry.getValue());
				return count != mSource.size();
			}

			public bool remove(object o)
			{
				int count = mSource.size();
				MapEntry entry = (MapEntry)o;
				mSource.remove(entry.getKey());
				return count != mSource.size();
			}

			public bool containsAll(Collection c)
			{
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					MapEntry entry = (MapEntry)i.next();
					if (!mSource.containsKey(entry.getKey()))
						return false;
				}
				return true;
			}

			public bool addAll(Collection c)
			{
				int count = mSource.size();
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					MapEntry entry = (MapEntry)i.next();
					mSource.put(entry.getKey(), entry.getValue());
				}
				return count != mSource.size();
			}

			public bool removeAll(Collection c)
			{
				int count = mSource.size();
				Iterator i = c.iterator();
				while (i.hasNext())
				{
					MapEntry entry = (MapEntry)i.next();
					mSource.remove(entry.getKey());
				}
				return count != mSource.size();
			}

			public bool retainAll(Collection c)
			{
				throw new NotSupportedException();
			}

			public void clear()
			{
				mSource.clear();
			}

			#endregion

			#region Iterable Members

			public Iterator iterator()
			{
				return new IteratorOverEntrySet(this);
			}

			#endregion

			internal RedBlackTree Source
			{
				get
				{
					return mSource;
				}
			}

			private class IteratorOverEntrySet : Iterator
			{
				public IteratorOverEntrySet(EntrySet entrySet)
				{
					mMapEntrySet = entrySet;
				}

				#region Iterator Members

				public bool hasNext()
				{
					moveNext();
					return mMoveSucceeded;
				}

				public object next()
				{
					moveNext();
					mMoved = false;
					if (mMoveSucceeded)
					{
						mMoveSucceeded = false;
						return mNextEntry;
					}
					return null;
				}

				public void remove()
				{
					mMapEntrySet.remove(mNextEntry.getKey());
					mMoved = false;
					moveNext();
				}

				private void moveNext()
				{
					if (mMoved)
						return;
					mMoved = true;
					if (!mInitialized)
					{
						mInitialized = true;
						if (!mMapEntrySet.isEmpty())
						{
							mNextEntry = mMapEntrySet.Source.firstEntry();
							mMoveSucceeded = true;
						}
					}
					else
					{
						mMoveSucceeded = mMapEntrySet.Source.GetSuccessorEntry(mNextEntry, ref mNextEntry);
					}
				}

				#endregion

				private EntrySet mMapEntrySet;
				private bool mInitialized, mMoved, mMoveSucceeded;
				private MapEntry mNextEntry;
			}

			private RedBlackTree mSource;
		}

		#endregion EntrySet class

		#region Tree node class

		internal class Node : MapEntry
		{
			public Node(object key, object value)
			{
				mKey = key;
				mVal = value;
				mNodeColor = NodeColor.Red;
				mLeft = mRight = null;
			}

			#region MapEntry Members

			public object getKey()
			{
				return mKey;
			}

			public object getValue()
			{
				return Value;
			}

			public object setValue(object value)
			{
				object old = Value;
				Value = value;
				return old;
			}

			#endregion

			public object Key
			{
				get { return mKey; }
				set { mKey = value; }
			}

			public object Value
			{
				get { return mVal; }
				set { mVal = value; }
			}

			public Node Left
			{
				get { return mLeft; }
				set { mLeft = value; }
			}

			public Node Right
			{
				get { return mRight; }
				set { mRight = value; }
			}

			public NodeColor Color
			{
				get { return mNodeColor; }
				set { mNodeColor = value; }
			}

			public Node this[bool direction]
			{
				get
				{
					return direction ? mRight : mLeft;
				}
				set
				{
					if (direction)
						mRight = value;
					else
						mLeft = value;
				}
			}

			public static bool IsRed(Node node)
			{
				return node != null && node.Color == NodeColor.Red;
			}

			public static bool IsBlack(Node node)
			{
				return node == null || node.Color == NodeColor.Black;
			}

			private object mKey;
			private object mVal;
			private NodeColor mNodeColor;
			private Node mLeft;
			private Node mRight;
		}

		#endregion Tree node class

		private Node mRoot;
		private int mCount;
		private Comparator mComp;

		#endregion
	}

	internal enum NodeColor
	{
		Red,
		Black
	}

}
