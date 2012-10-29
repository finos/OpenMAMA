/* $Id: MamdaOrderBookEntry.cs,v 1.1.40.6 2012/08/24 16:12:13 clintonmcdowell Exp $
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
	/// MamdaOrderBookEntry is a class that provides/implements order book
	/// functionality.
	/// </summary>
	public class MamdaOrderBookEntry
	{
		/// <summary>
		/// </summary>
		public MamdaOrderBookEntry()
		{
		}

		/// <summary>
		/// Copy an order book entry object which is a deep copy of
		/// the original.
		/// </summary>
		/// <param name="copy">The MamdaOrderBookEntry to copy.</param>
		public MamdaOrderBookEntry(MamdaOrderBookEntry copy)
		{
			this.copy(copy);
		}

		/// <summary>
		/// Copy an order book entry object which is an exact and deep copy of
		/// the original.
		/// </summary>
		/// <param name="copy">The MamdaOrderBookEntry to deep copy.</param>
		public void copy(MamdaOrderBookEntry copy)
		{
			mId = copy.mId;
			mSize = copy.mSize;
			mAction = copy.mAction;
			mTime = copy.mTime;
		}

		/// <summary>
		/// </summary>
		/// <param name="id"></param>
		public void setId(string id)
		{
			mId = id;
		}

		/// <summary>
		/// </summary>
		/// <param name="size"></param>
		public void setSize(long size)
		{
			mSize = size;
		}

		/// <summary>
		/// </summary>
		/// <param name="action"></param>
		public void setAction(Actions action)
		{
			mAction = action;
		}

		/// <summary>
		/// </summary>
		/// <param name="time"></param>
		public void setTime(DateTime time)
		{
			mTime = time;
		}

		/// <summary>
		/// </summary>
		/// <param name="copy"></param>
		public void setDetails(MamdaOrderBookEntry copy)
		{
			mSize = copy.mSize;
			mTime = copy.mTime;
		}

		/// <summary>
		/// If supported, Order book entry ID (order ID, participant ID,
		/// etc.)
		/// </summary>
		/// <returns></returns>
		public string getId()
		{
			return mId;
		}

		/// <summary>
		/// The size of the order entry.
		/// </summary>
		/// <returns></returns>
		public long getSize()
		{
			return mSize;
		}

		/// <summary>
		/// Whether to ADD, UPDATE or DELETE the entry. 
		/// </summary>
		/// <returns>The action.</returns>
		public Actions getAction ()
		{
			return mAction;
		}

		/// <summary>
		/// Time of order book entry update.
		/// </summary>
		/// <returns></returns>
		public DateTime getTime ()
		{
			return mTime;
		}

		/// <summary>
		/// Returns whether the time part of this entry was set to a valid date
		/// </summary>
		/// <returns></returns>
		public bool hasTime()
		{
			return mTime != DateTime.MinValue;
		}

		/// <summary>
		/// Order book entry equality verification.  A
		/// MamdaOrderBookException is thrown if the entries within a price
		/// level are not equal, along with the reason for the inequality.
		/// </summary>
		/// <param name="rhs"></param>
		/// <exception cref="MamdaOrderBookException">MamdaOrderBookException</exception>
		public void assertEqual(MamdaOrderBookEntry rhs)
		{
			if (mId != rhs.mId)
				throw new MamdaOrderBookException("entry IDs not equal");
			if (mSize != rhs.mSize)
				throw new MamdaOrderBookException("entry size not equal");
			if (mAction != rhs.mAction)
				throw new MamdaOrderBookException("entry action not equal");
			if (mTime != rhs.mTime)
				throw new MamdaOrderBookException("entry time not equal");
		}

		/// <summary>
		/// Whether the id of the entry is equal to the id passed to the method.
		/// </summary>
		/// <param name="id">The id to compare to the order entry id.</param>
		/// <returns>Whether the id is the same as the order id.</returns>
		public bool equalId(string id)
		{
			return mId == id;
		}

		/// <summary>
		/// Enforce strict checking of order book modifications (at the
		/// expense of some performance).  This setting is automatically
		/// updated by MamdaOrderBook.setStrictChecking().
		/// </summary>
		/// <param name="strict"></param>
		public static void setStrictChecking(bool strict)
		{
			mStrictChecking = strict;
		}

		/// <summary>
		/// An enumeration for book entry actions.  Price level actions
		/// differ from entry actions because, for example, a price level
		/// message with Actions.Add may consist of entries with
		/// Actions.Add, Actions.Update or Actions.Delete.
		/// </summary>
		public enum Actions : sbyte
		{
			/// <summary>
			/// A new entry.
			/// </summary>
			Add		= (sbyte)'A',
			/// <summary>
			/// An updated entry.
			/// </summary>
			Update	= (sbyte)'U',
			/// <summary>
			/// A deleted entry.
			/// </summary>
			Delete	= (sbyte)'D',
			/// <summary>
			/// Unknown action (error).
			/// </summary>
			Unknown	= (sbyte)'Z'
		}


		#region Implementation details

		private string		mId         = "";
		private long		mSize       = 0;
		private Actions		mAction     = Actions.Add;
		private DateTime	mTime       = DateTime.MinValue;

		private static bool mStrictChecking = false;

		#endregion

	}
}
