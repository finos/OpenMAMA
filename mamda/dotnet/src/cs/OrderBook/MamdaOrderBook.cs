/* $Id: MamdaOrderBook.cs,v 1.4.32.6 2012/08/24 16:12:13 clintonmcdowell Exp $
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
using System.IO;
using System.Collections;
using TreeMap = System.Collections.SortedList;

namespace Wombat
{
	/// <summary>
	/// MamdaOrderBook is a class that provides order book functionality,
	/// including iterators over price levels and entries within price
	/// levels.
	/// </summary>
	public class MamdaOrderBook
	{
		/// <summary>
		/// Construct a new order book object. Contains no price levels upon
		/// construction.
		/// </summary>
		public MamdaOrderBook()
		{
			mBidLevels = new TreeMap(new BidComparer());
			mAskLevels = new TreeMap(new AskCompare());
		}

		/// <summary>
		/// Clear all levels from the order book. Clears both the bid and ask
		/// side levels from the order book.
		/// </summary>
		public void clear ()
		{
			mBidLevels.Clear();
			mAskLevels.Clear();
		}

		/// <summary>
		/// </summary>
		/// <param name="symbol">The order book subscription symbol.</param>
		public void setSymbol(string symbol)
		{
			mSymbol = symbol;
		}

		/// <summary>
		/// </summary>
		/// <returns>The order book subscription symbol.</returns>
		public String getSymbol ()
		{
			return mSymbol;
		}
        
   		/// <summary>
		/// </summary>
		/// <param name="symbol">The order book subscription symbol.</param>
		public void setBookTime(DateTime time)
		{
			mBookTime = time;
		}

		/// <summary>
		/// </summary>
		/// <returns>The order book subscription symbol.</returns>
		public DateTime getBookTime ()
		{
			return mBookTime;
		}

		/// <summary>
		/// Add a price level to the order book.
		/// </summary>
		/// <param name="level">The price level to add to the book.</param>
		public void addLevel(MamdaOrderBookPriceLevel level)
		{
			switch (level.getSide())
			{
				case MamdaOrderBookPriceLevel.Sides.Bid:
					addLevelSide(level, mBidLevels);
					break;
				case MamdaOrderBookPriceLevel.Sides.Ask:
					addLevelSide(level, mAskLevels);
					break;
			}
		}

		/// <summary>
		/// Update an existing level in the order book.
		/// </summary>
		/// <param name="level">The price level to update.</param>
		public void updateLevel(MamdaOrderBookPriceLevel level)
		{
			switch (level.getSide())
			{
				case MamdaOrderBookPriceLevel.Sides.Bid:
					updateLevelSide(level, mBidLevels);
					break;
				case MamdaOrderBookPriceLevel.Sides.Ask:
					updateLevelSide(level, mAskLevels);
					break;
			}
		}

		/// <summary>
		/// Delete an existing price level from the book.
		/// </summary>
		/// <param name="level">The price level in the book to delete.</param>
		public void deleteLevel(MamdaOrderBookPriceLevel level)
		{
			switch (level.getSide())
			{
				case MamdaOrderBookPriceLevel.Sides.Bid:
					deleteLevelSide(level, mBidLevels);
					break;
				case MamdaOrderBookPriceLevel.Sides.Ask:
					deleteLevelSide(level, mAskLevels);
					break;
			}
		}

		/// <summary>
		/// Apply a delta to this (presumably) full book.
		/// </summary>
		/// <param name="deltaBook">The delta to apply to the order book.</param>
		public void apply(MamdaOrderBook deltaBook)
		{
			apply(deltaBook.getBidLevels());
			apply(deltaBook.getAskLevels());
		}

		/// <summary>
		/// Make a shallow copy of a book.
		/// </summary>
		/// <param name="book">The order book to copy.</param>
		public void copy(MamdaOrderBook book)
		{
			clear();
			apply(book);
		}

		/// <summary>
		/// Make a deep copy of a book.
		/// </summary>
		/// <param name="book">The order book to copy.</param>
		public void deepCopy(MamdaOrderBook book)
		{
			clear();
			deepCopy(book.getBidLevels());
			deepCopy(book.getAskLevels());
		}

		/// <summary>
		/// Set this order book to be a delta that would, when applied,
		/// delete all of the fields in the bookToDelete.
		/// </summary>
		/// <param name="bookToDelete">The book to be deleted.</param>
		public void setAsDeltaDeleted(MamdaOrderBook bookToDelete)
		{
			copy(bookToDelete);
			markAllDeleted(mBidLevels);
			markAllDeleted(mAskLevels);
		}

		/// <summary>
		/// Set this order book to be a delta that would, when applied, be
		/// the difference between to other books.
		/// </summary>
		/// <param name="lhs">An order book.</param>
		/// <param name="rhs">An order book.</param>
		public void setAsDeltaDifference(
			MamdaOrderBook lhs,
			MamdaOrderBook rhs)
		{
			clear();
			determineDiffs(mBidLevels, lhs.mBidLevels, rhs.mBidLevels, false);
			determineDiffs(mAskLevels, lhs.mAskLevels, rhs.mAskLevels, true); 
		}

		/// <summary>
		/// Get the total number of price levels (both sides of order book).
		/// </summary>
		/// <returns>The total number of level in the order book.</returns>
		public long getTotalNumLevels ()
		{
			return mBidLevels.Count + mAskLevels.Count;
		}

		/// <summary>
		/// Order book equality verification.  A MamdaOrderBookException is
		/// thrown if the books are not equal, along with the reason for
		/// the inequality.
		/// </summary>
		/// <param name="rhs">The order book to compare to the current book.</param>
		/// <exception cref="MamdaOrderBookException">Throws MamdaOrderBookException
		/// if the two books are not equal.</exception>
		public void assertEqual (MamdaOrderBook  rhs)
		{
			if (mSymbol != rhs.mSymbol)
			{
				throw new MamdaOrderBookException(
					"different symbols: " + mSymbol + "/" + rhs.mSymbol);
			}
			assertEqual(mBidLevels, rhs.mBidLevels);
			assertEqual(mAskLevels, rhs.mAskLevels);
		}

		/// <summary>
		/// Allows an application to iterate over all the bid side price
		/// levels in the order book.
		/// </summary>
		/// <returns>IEnumerable for the bid side price levels in the
		/// order book - the elements are of type MamdaOrderBookPriceLevel</returns>
		public IEnumerable getBidLevels()
		{
			return mBidLevels.Values;
		}

		/// <summary>
		/// Allows an application to iterate over all the ask side price
		/// levels in the order book.
		/// </summary>
		/// <returns>IEnumerable for the ask side price levels in the
		/// order book - the elements are of type MamdaOrderBookPriceLevel</returns>
		public IEnumerable getAskLevels()
		{
			return mAskLevels.Values;
		}

		/// <summary>
		/// Set whether the order book is in a consistent or an an
		/// inconsistent state.
		/// </summary>
		/// <param name="isConsistent">Whether the book is consistent.</param>
		public void setIsConsistent (bool isConsistent)
		{
			mIsConsistent = isConsistent;
		}

		/// <summary>
		/// Get whether the order book is in a consistent or an an
		/// inconsistent state. The orderbook will be marked as inconsistent if a
		/// gap is detected by the Listener. The book will be marked consistent
		/// again once a full recap for the book has been obtained from the feed.
		/// </summary>
		/// <returns>Whether the book is in a consistent state.</returns>
		public bool getIsConsistent()
		{
			return mIsConsistent;
		}

		/// <summary>
		/// Dump the order book to standard output.
		/// </summary>
		public void dump()
		{
			dump(Console.Out);
		}

		/// <summary>
		/// Dumps the order book to the specified stream
		/// </summary>
		/// <param name="stream">The stream on which the order book will be dumped</param>
		public void dump(Stream stream)
		{
			using (StreamWriter writer = new StreamWriter(stream))
			{
				dump(writer);
			}
		}

		/// <summary>
		/// Dumps the order book using the specified TextWriter instance
		/// </summary>
		/// <param name="writer"></param>
		public void dump(TextWriter writer)
		{
			writer.WriteLine("Dump book: {0}", mSymbol);
			int i = 0;
			IEnumerator bidEnum = mBidLevels.Values.GetEnumerator();
			IEnumerator askEnum = mAskLevels.Values.GetEnumerator();

			bool bidHasNext = bidEnum.MoveNext();
			bool askHasNext = askEnum.MoveNext();

			while (bidHasNext || askHasNext)
			{
				if (bidHasNext)
				{
					dump(writer, (MamdaOrderBookPriceLevel)bidEnum.Current, i);
					bidHasNext = bidEnum.MoveNext();
				}
				else
				{
					writer.Write("          ");
				}
				if (askHasNext)
				{
					dump(writer, (MamdaOrderBookPriceLevel)askEnum.Current, i);
					askHasNext = askEnum.MoveNext();
				}
				writer.WriteLine();
				++i;
			}
		}

		/// <summary>
		/// Enforce strict checking of order book modifications (at the
		/// expense of some performance).  This setting is passed on to the
		/// MamdaOrderBookPriceLevel and MamdaOrderBookEntry classes.
		/// </summary>
		/// <param name="strict"></param>
		public static void setStrictChecking(bool strict)
		{   
			MamdaOrderBookPriceLevel.setStrictChecking(strict);
			MamdaOrderBookEntry.setStrictChecking(strict);
		}


		#region Implementation details

		private void apply(IEnumerable deltaBookLevels)
		{
			foreach (MamdaOrderBookPriceLevel deltaLevel in deltaBookLevels)
			{
				switch (deltaLevel.getAction())
				{
					case MamdaOrderBookPriceLevel.Actions.Add:
						addLevel(deltaLevel);
						break;
					case MamdaOrderBookPriceLevel.Actions.Update: 
						updateLevel(deltaLevel);
						break;
					case MamdaOrderBookPriceLevel.Actions.Delete:
						deleteLevel(deltaLevel);
						break;
				}
			}
		}

		private void deepCopy(IEnumerable bookLevels)
		{
			foreach (MamdaOrderBookPriceLevel bookLevel in bookLevels)
			{
				MamdaOrderBookPriceLevel level = new MamdaOrderBookPriceLevel();
				level.copy(bookLevel);
				switch (level.getAction())
				{
					case MamdaOrderBookPriceLevel.Actions.Add:
						addLevel(level);
						break;
					case MamdaOrderBookPriceLevel.Actions.Update: 
						updateLevel(level);
						break;
					case MamdaOrderBookPriceLevel.Actions.Delete:
						deleteLevel(level);
						break;
				}
			}
		}

		private void assertEqual(
			TreeMap lhsLevels,
			TreeMap rhsLevels)
		{
			if (lhsLevels.Count != rhsLevels.Count)
			{
				throw new MamdaOrderBookException(
					"number of price levels do not add up (" + lhsLevels.Count +
					"/" + rhsLevels.Count);
			}

			IEnumerator lhsEnum = lhsLevels.Values.GetEnumerator();
			IEnumerator rhsEnum = rhsLevels.Values.GetEnumerator();

			while (lhsEnum.MoveNext() && rhsEnum.MoveNext())
			{
				MamdaOrderBookPriceLevel lhsLevel = 
					(MamdaOrderBookPriceLevel)lhsEnum.Current;
				MamdaOrderBookPriceLevel rhsLevel =
					(MamdaOrderBookPriceLevel)rhsEnum.Current;
				lhsLevel.assertEqual(rhsLevel);
			}
		}

		private void dump(TextWriter writer, MamdaOrderBookPriceLevel level, int index)
		{
			string title = level.getSide() == MamdaOrderBookPriceLevel.Sides.Ask ? "Ask" : "Bid";
			writer.WriteLine("{0} {1} | price={2} size={3} action={4} entries={5} time={6}",
				title,
				index,
				level.getPrice(),
				level.getSize(),
				level.getAction(),
				level.getNumEntries(),
				level.hasTime() ? level.getTime().ToString() : "null");
	                
			foreach (MamdaOrderBookEntry entry in level)
			{
				writer.WriteLine("      |    id={0} size={1} action={2} time={3}",
					entry.getId(),
					entry.getSize(),
					entry.getAction(),
					entry.hasTime() ? entry.getTime().ToString() : "null");
			}
		}

		private void addLevelSide(
			MamdaOrderBookPriceLevel level,
			TreeMap bookSide)
		{
			MamaPrice price = level.getPrice();
			if (!bookSide.ContainsKey(price))
			{
				bookSide.Add(price, level);
			}
			else
			{
				bookSide[price] = level; // Overwrite it anyway
			}
		}

		private void updateLevelSide(
			MamdaOrderBookPriceLevel level,
			TreeMap bookSide)
		{
			MamaPrice price = level.getPrice();
			if (bookSide.ContainsKey(price))
			{
				MamdaOrderBookPriceLevel fullBookLevel =
					bookSide[price] as MamdaOrderBookPriceLevel;

				/*Iterate over the entries in the update and apply them to the
				 * full book level according to action*/
				foreach (MamdaOrderBookEntry deltaEntry in level)
				{
					switch (deltaEntry.getAction())
					{
						case MamdaOrderBookEntry.Actions.Add:
							fullBookLevel.addEntry(deltaEntry);
							break;
						case MamdaOrderBookEntry.Actions.Update:
							fullBookLevel.updateEntry(deltaEntry);
							break;
						case MamdaOrderBookEntry.Actions.Delete:
							fullBookLevel.removeEntry(deltaEntry);
							break;
						case MamdaOrderBookEntry.Actions.Unknown:
							/*Do nothing*/
							break;
						default:
							/*Do nothing*/
							break;
					}
				}
				/*Update the details for the level itself*/
				fullBookLevel.setDetails(level);
			}
			else
			{
				bookSide.Add(price, level); // Add it anyway
			}
		}

		private void deleteLevelSide(
			MamdaOrderBookPriceLevel level,
			TreeMap bookSide)
		{
			MamaPrice price = level.getPrice();
			if (bookSide.ContainsKey(price))
			{
				/* We actually need to process this properly because the
				 * update may not contain all entries, just updated
				 * ones. */
				bookSide.Remove(price);
			}
		}

		private void markAllDeleted(TreeMap bookSide)
		{
			foreach (MamdaOrderBookPriceLevel level in bookSide.Values)
			{
				level.markAllDeleted();
			}
		}

		private bool doubleEquals(
			double lhs,
			double rhs)
		{
			return (Math.Abs(lhs-rhs) < Math.Abs(lhs * 0.00000000001D));
		}

		private TreeMap determineDiffs(
			TreeMap resultSide,
			TreeMap lhs,
			TreeMap rhs,
			bool ascending)
		{
			IEnumerator lhsEnum = lhs.Values.GetEnumerator();
			IEnumerator rhsEnum = rhs.Values.GetEnumerator();
			bool lhsHasNext = lhsEnum.MoveNext();
			bool rhsHasNext = rhsEnum.MoveNext();

			while (lhsHasNext || rhsHasNext)
			{
				MamdaOrderBookPriceLevel lhsLevel = null;
				MamdaOrderBookPriceLevel rhsLevel = null;
				double    lhsPrice = double.MinValue;
                double    rhsPrice = double.MinValue;
				long      lhsSize  = 0;
				long      rhsSize  = 0;

				if (lhsHasNext)
				{
					lhsLevel = (MamdaOrderBookPriceLevel)lhsEnum.Current;
					lhsPrice = lhsLevel.getPrice().getValue();
					lhsSize  = lhsLevel.getSize();
				}
				if (rhsHasNext)
				{
					rhsLevel = (MamdaOrderBookPriceLevel)rhsEnum.Current;
					rhsPrice = rhsLevel.getPrice().getValue();
					rhsSize  = rhsLevel.getSize();
				}

				// Compare two doubles using an epsilon
				if ((doubleEquals(lhsPrice, rhsPrice)) && (lhsSize == rhsSize))
				{
					// Usual scenario: both levels are the same
					lhsHasNext = lhsEnum.MoveNext();
					rhsHasNext = rhsEnum.MoveNext();
					continue;
				}

				if (doubleEquals(lhsPrice, rhsPrice))
				{
					// Same price, different size.  Need to determine the
					// different entries.
					MamdaOrderBookPriceLevel diffLevel = new MamdaOrderBookPriceLevel();
					diffLevel.setAsDifference(lhsLevel, rhsLevel);
					resultSide.Add(lhsLevel.getPrice(), diffLevel);
					lhsHasNext = lhsEnum.MoveNext();
					rhsHasNext = rhsEnum.MoveNext();
					continue;
				}

				if (ascending)
				{
                    if (((lhsPrice > rhsPrice) && (rhsPrice != double.MinValue)) || (lhsPrice == double.MinValue))
					{
						// RHS has an additional price level
						MamdaOrderBookPriceLevel diffLevel = new MamdaOrderBookPriceLevel(rhsLevel);
						resultSide.Add(rhsLevel.getPrice(), diffLevel);
						rhsHasNext = rhsEnum.MoveNext();
						continue;
					}
					else
					{
						// RHS does not have a price level that is on the LHS.
						// Copy the LHS level and mark all as deleted.
						MamdaOrderBookPriceLevel diffLevel = new MamdaOrderBookPriceLevel(lhsLevel);
						resultSide.Add(lhsLevel.getPrice(), diffLevel);
						lhsHasNext = lhsEnum.MoveNext();
						continue;
					}
				}
				else
				{
                    if (((lhsPrice > rhsPrice) && (lhsPrice != double.MinValue)) || (rhsPrice == double.MinValue))
					{
						// LHS has an additional price level
						MamdaOrderBookPriceLevel diffLevel = new MamdaOrderBookPriceLevel(lhsLevel);
						resultSide.Add(lhsLevel.getPrice(), diffLevel); // CHECK: use indexer?
						lhsHasNext = lhsEnum.MoveNext();
						continue;
					}
					else
					{
						// LHS does not have a price level that is on the RHS.
						// Copy the RHS level and mark all as deleted.
						MamdaOrderBookPriceLevel diffLevel = new MamdaOrderBookPriceLevel(rhsLevel);
						resultSide.Add(rhsLevel.getPrice(), diffLevel); // CHECK: use indexer?
						rhsHasNext = rhsEnum.MoveNext();
						continue;
					}
				}
			}
			return resultSide;
		}

		private class BidComparer : IComparer
		{
			/* Descending order prices (MamaPrice) */
			public int Compare(object o1, object o2)
			{
				return ((MamaPrice)o2).CompareTo((MamaPrice)o1);
			}
		}

		private class AskCompare : IComparer
		{
			/* Ascending order prices (MamaPrice) */
			public int Compare(object o1, object o2)
			{
				return ((MamaPrice)o1).CompareTo((MamaPrice)o2);
			}
		}

		private String   mSymbol       = null;
		private TreeMap  mBidLevels    = null;
		private TreeMap  mAskLevels    = null;
		private bool	 mIsConsistent  = true;
        private DateTime mBookTime     = DateTime.MinValue;

		#endregion Implementation details

	}
}
