/* $Id: MamdaOrderBookPriceLevel.cs,v 1.1.40.5 2012/08/24 16:12:13 clintonmcdowell Exp $
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

namespace Wombat
{
	/// <summary>
	/// </summary>
	public class MamdaOrderBookPriceLevel : IEnumerable // IEnumerable<MamdaOrderBookEntry> if C# 2.0
	{
		/// <summary>
		/// Default constructor.
		/// </summary>
		public MamdaOrderBookPriceLevel()
		{
		}

		/// <summary>
		/// Construct a price level object which is a shallow copy of
		/// the original.
		/// </summary>
		/// <param name="copy">The MamdaOrderBookPriceLevel to copy.</param>
		public MamdaOrderBookPriceLevel(MamdaOrderBookPriceLevel copy)
		{
			mPrice      = copy.mPrice;
			mSize       = copy.mSize;
			mSizeChange = copy.mSizeChange;
			mNumEntries = copy.mNumEntries;
			mSide       = copy.mSide;
			mAction     = copy.mAction;
			mTime       = copy.mTime;

			for (int i = 0; i < copy.mEntries.Count; i++)
			{
				MamdaOrderBookEntry entry = new MamdaOrderBookEntry(
					(MamdaOrderBookEntry)copy.mEntries[i]);
				mEntries.Add(entry);
			}
		}

		/// <summary>
		/// Copy a price level object which is an exact and deep copy of
		/// the original.
		/// </summary>
		/// <param name="copy">The MamdaOrderBookPriceLevel to copy.</param>
		public void copy(MamdaOrderBookPriceLevel copy)
		{
			setPrice(copy.mPrice);
			setSize(copy.mSize);
			setSizeChange(copy.mSizeChange);
			setNumEntries(copy.mNumEntries);
			setSide(copy.mSide);
			setAction(copy.mAction);
			setTime(copy.mTime); // always deep copied

			for (int i = 0; i < copy.mEntries.Count; i++)
			{
				MamdaOrderBookEntry entry = new MamdaOrderBookEntry();
				entry.copy((MamdaOrderBookEntry)copy.mEntries[i]);
				mEntries.Add(entry);
			}
		}

		/// <summary>
		/// </summary>
		/// <param name="price"></param>
		public void setPrice(MamaPrice price)
		{
			if (mPrice == null)
			{
				if (price != null)
				{
					mPrice = new MamaPrice(price); // copy
				}
			}
			else
			{
				if (price == null)
				{
					mPrice.destroy();
					mPrice = null;
				}
				else
				{
					mPrice.copy(price);
				}
			}
		}

		/// <summary>
		/// </summary>
		/// <param name="size"></param>
		public void setSize(long size)
		{
			mSize  = size;
		}

		/// <summary>
		/// </summary>
		/// <param name="size"></param>
		public void setSizeChange(long size)
		{
			mSizeChange  = size;
		}

		/// <summary>
		/// </summary>
		/// <param name="numEntries"></param>
		public void setNumEntries(int numEntries)
		{
			mNumEntries = numEntries;
		}

		/// <summary>
		/// </summary>
		/// <param name="side"></param>
		public void setSide(Sides side)
		{
			mSide  = side;
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
		/// Take the details from <code>level</code>code> and apply them to this level
		/// This does not update entries within the level, only information
		/// pertaining to the level itself.
		/// It is intended that this method should not be used externally to the
		/// API.
		/// </summary>
		/// <param name="level">The price level object from which the details are being
		/// obtained.</param>
		public void setDetails(MamdaOrderBookPriceLevel level)
		{
			this.mSizeChange = level.mSize - this.mSize;
			this.mSize       = level.mSize;
			this.mNumEntries = level.mNumEntries;
			this.mTime       = level.mTime;
		}

		/// <summary>
		/// Add a new order book entry to the price level.
		/// </summary>
		/// <param name="entry">The new entry to be added to the level.</param>
		public void addEntry(MamdaOrderBookEntry  entry)
		{
			if (mStrictChecking)
			{
				checkNotExist(entry);
			}
			mEntries.Add(entry);
		}

		/// <summary>
		/// Update the details of an existing entry in the level.
		/// </summary>
		/// <param name="entry">An instance of <code>MamdaOrderBookEntry</code> with the
		/// new details for the entry in the level.</param>
		public void updateEntry(MamdaOrderBookEntry  entry)
		{
			for (int i = 0; i < mEntries.Count; i++)
			{
				MamdaOrderBookEntry existingEntry =
					(MamdaOrderBookEntry)mEntries[i];
				if (existingEntry.equalId(entry.getId()))
				{
					existingEntry.setDetails(entry);
					return;
				}
			}
			if (mStrictChecking)
			{
				throw new MamdaOrderBookException(
					"attempted to update a non-existent entry: " + entry.getId());
			}
		}

		/// <summary>
		/// Remove an order book entry from the price level.
		/// <see cref="MamdaOrderBookEntry"/>
		/// </summary>
		/// <param name="entry">The entry which is to be removed from the price level.</param>
		public void removeEntry(MamdaOrderBookEntry  entry)
		{
			for (int i = 0; i < mEntries.Count; i++)
			{
				MamdaOrderBookEntry existingEntry =
					(MamdaOrderBookEntry)mEntries[i];
				if (existingEntry.equalId(entry.getId()))
				{
					mEntries.RemoveAt(i);
					return;
				}
			}
			if (mStrictChecking)
			{
				throw new MamdaOrderBookException (
					"attempted to delete a non-existent entry: " + entry.getId());
			}
		}

		/// <summary>
		/// Returns a <code>System.Collections.IEnumerator</code> for all entries within this
		/// level.
		/// Price Level entries are represented by the
		/// <code>MamdaOrderBookEntry</code> class.
		/// </summary>
		/// <returns>The enumerator for the price level entries</returns>
		public IEnumerator GetEnumerator()
		{
			return mEntries.GetEnumerator();
		}

		/// <summary>
		/// If the provided order book entry exists in the price level a
		/// <code>MamdaOrderBookException</code> exception is thrown. Otherwise the
		/// method simply returns.
		/// </summary>
		/// <param name="entry">The entry whose presence in the level is being determined.</param>
		/// <exception cref="MamdaOrderBookException">Throws MamdaOrderBookException if the entry is found in the price</exception>
		public void checkNotExist(MamdaOrderBookEntry  entry)
		{
			for (int i = 0; i < mEntries.Count; i++)
			{
				MamdaOrderBookEntry existingEntry =
					(MamdaOrderBookEntry)mEntries[i];
				if (existingEntry.equalId(entry.getId()))
				{
					throw new MamdaOrderBookException(
						"attempted to add am existent entry: " + entry.getId());
				}
			}
		}

		/// <summary>
		/// Mark everything in this price level as deleted, including
		/// entries.
		/// </summary>
		public void markAllDeleted()
		{
			setSizeChange(-getSize());
			setSize(0);
			setNumEntries(0);
			setAction(Actions.Delete);

			for (int i = 0; i < mEntries.Count; i++)
			{
				MamdaOrderBookEntry entry = (MamdaOrderBookEntry)mEntries[i];
				entry.setSize(0);
				entry.setAction(MamdaOrderBookEntry.Actions.Delete);
			}
		}

		/// <summary>
		/// </summary>
		/// <param name="lhs"></param>
		/// <param name="rhs"></param>
		public void setAsDifference(
			MamdaOrderBookPriceLevel lhs,
			MamdaOrderBookPriceLevel rhs)
		{
			int lhsBookSize = lhs.mEntries.Count;
			int rhsBookSize = rhs.mEntries.Count;
			int lhsIndex = 0;
			int rhsIndex = 0;

			while ((lhsIndex < lhsBookSize) && (rhsIndex < rhsBookSize))
			{
				string  lhsId                = null;
				string  rhsId                = null;
				long    lhsSize              = 0;
				long    rhsSize              = 0;
				MamdaOrderBookEntry lhsEntry = null;
				MamdaOrderBookEntry rhsEntry = null;

				if (lhsIndex < lhsBookSize)
				{
					lhsEntry = (MamdaOrderBookEntry)lhs.mEntries[lhsIndex];
					lhsId    = lhsEntry.getId();
					lhsSize  = lhsEntry.getSize();
				}
				if (rhsIndex < rhsBookSize)
				{
					rhsEntry = (MamdaOrderBookEntry)rhs.mEntries[rhsIndex];
					rhsId    = rhsEntry.getId();
					rhsSize  = rhsEntry.getSize();
				}

				if ((lhsId != null) && (rhsId != null))
				{
					if (lhsId == rhsId)
					{
						// Same ID, maybe different size.
						if (lhsSize != rhsSize)
						{
							MamdaOrderBookEntry updateEntry = new MamdaOrderBookEntry(rhsEntry);
							updateEntry.setAction(MamdaOrderBookEntry.Actions.Update);
							addEntry(updateEntry);
						}
						lhsIndex++;
						rhsIndex++;
						continue;
					}
					else
					{
						// Different ID (either something exists on the LHS
						// and not on RHS or vice versa).
						int rhsFound = findEntryAfter(rhs.mEntries, rhsIndex, lhsId);
						if (rhsFound != rhsSize)
						{
							// The ID from the LHS was found on the RHS, so
							// there must have been additional entries on the
							// RHS, which we now need to add.
							do
							{
								addEntry((MamdaOrderBookEntry)rhs.mEntries[rhsIndex]);
								rhsIndex++;
							}
							while (rhsIndex < rhsFound);
						}
						else
						{
							// The ID from the LHS was not present on the RHS,
							// so add the LHS entry.  Note: it would probably
							// be faster to iterate over the LHS side rather
							// than begin the loop again.
							addEntry((MamdaOrderBookEntry)lhs.mEntries[lhsIndex]);
							lhsIndex++;
						}
					}
				}
			}
			if (mPrice != null && rhs.getPrice() != null)
			{
				mPrice.destroy();
			}
			mPrice = rhs.getPrice();
			setSizeChange(rhs.getSize() - lhs.getSize());
			setSize(rhs.getSize());
			setNumEntries(rhs.getNumEntries());
			setAction(Actions.Update);
			mTime = rhs.getTime();
			setSide(rhs.getSide());
		}

		/// <summary>
		/// Return the price for this level.
		/// </summary>
		/// <returns>The price for this level.</returns>
		public MamaPrice getPrice()
		{
			return mPrice;
		}

		/// <summary>
		/// Return the total size (across all entries) for this level.
		/// </summary>
		/// <returns>The total size for this level.</returns>
		public long getSize()
		{
			return mSize;
		}

		/// <summary>
		/// Return the size change for this (presumably delta) level.  This
		/// attribute is only of interest for delta order books.  For full
		/// order books, this field will be equal to the size of the price
		/// level.
		/// </summary>
		/// <returns>The changed size for this level.</returns>
		public long getSizeChange()
		{
			return mSizeChange;
		}

		/// <summary>
		/// Return the actual number of entries for this level.  The actual
		/// number of entries may not equate to the number of entries that
		/// can be iterated over if:
		/// (a) the feed does not provide the actual entries, or
		/// (b) the price level is just a delta.
		/// </summary>
		/// <returns>The actual number of entries for this level.</returns>
		public int getNumEntries()
		{
			return mNumEntries;
		}

		/// <summary>
		/// Return the side (bid/ask) of the book for this level.
		/// </summary>
		/// <returns>The side of the book for this level.</returns>
		public Sides getSide()
		{
			return mSide;
		}

		/// <summary>
		/// Return the action for this price level.  All price levels for a
		/// full book are marked with mActionADD.
		/// </summary>
		/// <returns>The action for this level.</returns>
		public Actions getAction ()
		{
			return mAction;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public bool hasTime()
		{
			return mTime != DateTime.MinValue;
		}

		/// <summary>
		/// Return the time stamp for when the price level was last updated.
		/// </summary>
		/// <returns>The time stamp for when the price level was last updated.</returns>
		public DateTime getTime ()
		{
			return mTime;
		}

		/// <summary>
		/// Order book price level equality verification.  A
		///  is thrown if the price levels are
		/// not equal, along with the reason for the inequality.
		/// </summary>
		/// <param name="rhs"></param>
		/// <exception cref="MamdaOrderBookException">MamdaOrderBookException is thrown if the price levels are</exception>
		public void assertEqual(MamdaOrderBookPriceLevel  rhs)
		{
			if ((mPrice != null) != (rhs.mPrice != null))
				throwError("price not equal");
			if (!mPrice.equals(rhs.mPrice))
				throwError("price not equal");
			if (mSize != rhs.mSize)
				throwError("size not equal");
			if (mNumEntries != rhs.mNumEntries)
				throwError("number of entries not equal");
			if (mSide != rhs.mSide)
				throwError("side not equal");
			if (mAction != rhs.mAction)
				throwError("action not equal");
			if (mTime != rhs.mTime)
				throwError ("time not equal");
          
			if (mEntries.Count != rhs.mEntries.Count)
			{
				throwError ("entries size mismatch (" + mEntries.Count +
					"!=" + rhs.mEntries.Count);
			}
			for (int i = 0; i < mEntries.Count; i++)
			{
				MamdaOrderBookEntry  lhsEntry = (MamdaOrderBookEntry)mEntries[i];
				MamdaOrderBookEntry  rhsEntry = (MamdaOrderBookEntry)rhs.mEntries[i];
				lhsEntry.assertEqual (rhsEntry);
			}
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
		/// An enumeration for book price level actions.  Price level actions
		/// differ from entry actions because, for example, a price level
		/// message with Actions.Add may consist of entries with
		/// Actions.Add, Actions.Update or Actions.Delete.
		/// </summary>
		public enum Actions : sbyte
		{
			/// <summary>
			/// A new price level
			/// </summary>
			Add		= (sbyte)'A',
			/// <summary>
			/// Updated price level.
			/// </summary>
			Update	= (sbyte)'U',
			/// <summary>
			/// Deleted price level.
			/// </summary>
			Delete	= (sbyte)'D',
			/// <summary>
			/// Unknown action (error).
			/// </summary>
			Unknown	= (sbyte)'Z'
		}

		/// <summary>
		/// </summary>
		public enum Sides : sbyte
		{
			/// <summary>
			/// Bid (buy) side.
			/// </summary>
			Bid		= (sbyte)'B',
			/// <summary>
			/// Ask (sell) side.
			/// </summary>
			Ask		= (sbyte)'A',
			/// <summary>
			/// Unknown side (error).
			/// </summary>
			Unknown	= (sbyte)'U'
		}

		#region Implementation details

		private int findEntryAfter(
			ArrayList entries,
			int start,
			string id)
		{
			int i;
			int size = entries.Count;
			for (i = start; i < size; i++)
			{
				MamdaOrderBookEntry entry = (MamdaOrderBookEntry)entries[i];
				if (entry.equalId(id))
				{
					return i;
				}
			}
			return i;
		}

		private void throwError(string context)
		{
			throw new MamdaOrderBookException(context + "(price=" + mPrice +
				", size=" + mSide);
		}

		private MamaPrice    mPrice      = null;
		private long         mSize       = 0;
		private long         mSizeChange = 0;
		private int          mNumEntries = 0;
		private Sides        mSide       = Sides.Unknown;
		private Actions      mAction     = Actions.Unknown;
		private DateTime	 mTime       = DateTime.MinValue;
		private ArrayList    mEntries    = new ArrayList();

		private static bool mStrictChecking = false;

		#endregion Implementation details

	}
}
