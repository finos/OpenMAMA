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
using Wombat.Containers;

namespace Wombat
{
	/// <summary>
	/// A class that represents a "view" of a subset of an option chain.
	/// The view can be restricted to a percentage or number of strike
	/// prices around "the money" as well as to a maximum number of days
	/// into the future.  The view will be adjusted to include strike
	/// prices within the range as the underlying price moves.  This means
	/// that the range of strike prices will change over time.  In order to
	/// avoid a "jitter" in the range of strike prices when the underlying
	/// price hovers right on the edge of a range boundary, the class also
	/// provides a "jitter margin" as some percentage of the underyling
	/// price (default is 0.5%).
	/// </summary>
	public class MamdaOptionChainView : MamdaOptionChainHandler
	{
		/// <summary>
		/// Create a view on the given option chain.  Multiple views are
		/// supported on any given option chain.
		/// </summary>
		/// <param name="chain"></param>
		public MamdaOptionChainView(MamdaOptionChain chain)
		{
			mChain = chain;
			resetRange();
		}

		/// <summary>
		/// Return the symbol for the option chain.
		/// </summary>
		/// <returns></returns>
		public string getSymbol()
		{
			return mChain.getSymbol();
		}

		/// <summary>
		/// Set how the underlying price ("at the money") is determined
		/// <see cref="MamdaOptionAtTheMoneyCompareType"/>
		/// </summary>
		/// <param name="atTheMoneyType"></param>
		public void setAtTheMoneyType(MamdaOptionAtTheMoneyCompareType atTheMoneyType)
		{
			if (mAtTheMoneyType != atTheMoneyType)
			{
				mAtTheMoneyType = atTheMoneyType;
				resetRange();
			}
		}

		/// <summary>
		/// Set the range of strike prices to be included in the view by
		/// percentage variation from the underlying price.  The range of
		/// strike prices in the view will vary as the underlying varies.
		/// The "jitter margin" avoids switching between ranges too often.
		/// </summary>
		/// <param name="percentMargin"></param>
		public void setStrikeRangePercent(double percentMargin)
		{
			if (mStrikeMargin != percentMargin)
			{
				mStrikeMargin = percentMargin;
				resetRange();
			}
		}

		/// <summary>
		/// Set the number of strike prices to be included in the view.
		/// The "jitter margin" avoids switching between ranges too often.
		/// </summary>
		/// <param name="number"></param>
		public void setStrikeRangeNumber(int number)
		{
			if (mNumStrikes != number)
			{
				mNumStrikes = number;
				resetRange();
			}
		}

		/// <summary>
		/// Set the range of expiration dates to be included in the view by
		/// the maximum number of days until expiration.  Note: a non-zero
		/// range overrides a specific number of expirations set by
		/// setNumberOfExpirations().
		/// </summary>
		/// <param name="expirationDays"></param>
		public void setExpirationRangeDays(int expirationDays)
		{
			mExpirationDays = expirationDays;
			resetRange();
		}

		/// <summary>
		/// Set the number of expiration dates to be included in the view.
		/// Note: a non-zero range (set by setExpirationRangeDays())
		/// overrides a specific number of expirations.
		/// </summary>
		/// <param name="numExpirations"></param>
		public void setNumberOfExpirations(int numExpirations)
		{
			mNumExpirations = numExpirations;
			resetRange();
		}

		/// <summary>
		/// Set a "jitter margin" to avoid having the range jump between
		/// different strike prices when the underlying price hovers right
		/// on the edge of a range boundary.  The underlying is allowed to
		/// fluctuate within the jitter margin without the range being
		/// reset.  When the underlying moves beyond the jitter margin,
		/// the range is reset.
		/// </summary>
		/// <param name="percentMargin"></param>
		public void setJitterMargin(double percentMargin)
		{
			mJitterMargin = percentMargin;
			resetRange();
		}

		/// <summary>
		/// Return whether an option contract falls within this view's
		/// parameters.
		/// </summary>
		/// <param name="contract"></param>
		/// <returns></returns>
		public bool isVisible(MamdaOptionContract contract)
		{
			double strikePrice = contract.getStrikePrice();
			DateTime expireDate  = contract.getExpireDate();
			return ((mLowExpireDate <= expireDate) &&
					(mHighExpireDate >= expireDate) &&
					(mLowStrike <= strikePrice) &&
					(strikePrice <= mHighStrike));
		}

		/// <summary>
		/// Return an Iterator over the set of expiration dates within the
		/// view.  Each Iterator represents a MamdaOptionExpirationStrikes
		/// object.
		/// </summary>
		/// <returns></returns>
		public Iterator expirationIterator()
		{
			return mExpirationDateSet.values().iterator();
		}

		/// <summary>
		/// Handler option chain recaps and initial values.
		/// </summary>
		/// <param name="subscription"></param>
		/// <param name="listener"></param>
		/// <param name="msg"></param>
		/// <param name="chain"></param>
		public void onOptionChainRecap (
			MamdaSubscription         subscription,
			MamdaOptionChainListener  listener,
			MamaMsg                   msg,
			MamdaOptionChain          chain)
		{
			resetRange();
		}

		/// <summary>
		/// Handler for option chain structural updates.
		/// </summary>
		/// <param name="subscription"></param>
		/// <param name="listener"></param>
		/// <param name="msg"></param>
		/// <param name="contract"></param>
		/// <param name="chain"></param>
		public void onOptionContractCreate (
			MamdaSubscription         subscription,
			MamdaOptionChainListener  listener,
			MamaMsg                   msg,
			MamdaOptionContract       contract,
			MamdaOptionChain          chain)
		{
			resetRange();
		}

		/// <summary>
		/// Handler for option chain structural updates.
		/// </summary>
		/// <param name="subscription"></param>
		/// <param name="listener"></param>
		/// <param name="msg"></param>
		/// <param name="updateEvent"></param>
		/// <param name="chain"></param>
		public void onOptionSeriesUpdate (
			MamdaSubscription         subscription,
			MamdaOptionChainListener  listener,
			MamaMsg                   msg,
			MamdaOptionSeriesUpdate   updateEvent,
			MamdaOptionChain          chain)
		{
			resetRange();
		}

		private void resetRange()
		{
			mExpirationDateSet = filterExpirations(mChain.getAllExpirations());
			filterStrikes(mExpirationDateSet);

			if ((mExpirationDateSet == null) || (mExpirationDateSet.size() == 0))
			{
				mLowExpireDate = DateTime.Today;
				mHighExpireDate = DateTime.Today.AddYears(1); // new Date(System.currentTimeMillis() + 366*24*60*60*1000);
			}
			else
			{
				mLowExpireDate = (DateTime)mExpirationDateSet.firstKey();
				mHighExpireDate = (DateTime)mExpirationDateSet.lastKey();
			}
		}

		private MamdaOptionExpirationDateSet filterExpirations(MamdaOptionExpirationDateSet initialSet)
		{
			if (mExpirationDays > 0)
			{
				// Find the subset of expirations between now and some
				// number of days into the future.
				System.Globalization.Calendar c = new System.Globalization.GregorianCalendar(System.Globalization.GregorianCalendarTypes.USEnglish);
				DateTime futureDate = c.AddDays(DateTime.Today, mExpirationDays);
				return initialSet.getExpirationsBefore(futureDate);
			}
			else if (mNumExpirations > 0)
			{
				// Explicit number of expiration dates.
				return initialSet.getExpirations(mNumExpirations);
			}
			else
			{
				// All expiration months.  We *copy* the initial set
				// because we may reduce the set to a particular range of
				// strike prices.
				return new MamdaOptionExpirationDateSet(initialSet);
			}
		}

		private void filterStrikes(MamdaOptionExpirationDateSet dateSet)
		{
			mLowStrike  = Double.MinValue;
			mHighStrike = Double.MaxValue;

			// First calculate the high/low strikes
			SortedSet strikeSet = mChain.getStrikesWithinPercent(mStrikeMargin, mAtTheMoneyType);

			if ((strikeSet == null) || (strikeSet.size() == 0))
			{
				strikeSet = mChain.getStrikesWithinRangeSize(mNumStrikes, mAtTheMoneyType);
			}

			if ((strikeSet == null) || (strikeSet.size() == 0))
			{
				return;
			}

			mLowStrike  = (double)strikeSet.first();
			mHighStrike = (double)strikeSet.last();

			Iterator expireIter = dateSet.values().iterator();
			while (expireIter.hasNext())
			{
				// Filter the strike prices for one expiration date
				MamdaOptionExpirationStrikes expireStrikes = (MamdaOptionExpirationStrikes)expireIter.next();
				expireStrikes.trimStrikes(strikeSet);
			}
		}

		private bool strikeInRange(double strikePrice)
		{
			// To be completed.
			return true;
		}

		private bool expirationInRange(string expirationDate)
		{
			// To be completed.
			return true;
		}

		private class UnderlyingQuoteHandler : MamdaQuoteHandler
		{
			public void onQuoteRecap(
				MamdaSubscription   sub,
				MamdaQuoteListener  listener,
				MamaMsg             msg,
				MamdaQuoteRecap     recap)
			{
			}

			public void onQuoteUpdate (
				MamdaSubscription   sub,
				MamdaQuoteListener  listener,
				MamaMsg             msg,
				MamdaQuoteUpdate    updateEvent,
				MamdaQuoteRecap     recap)
			{
			}

			public void onQuoteGap (
				MamdaSubscription   sub,
				MamdaQuoteListener  listener,
				MamaMsg             msg,
				MamdaQuoteGap       gapEvent,
				MamdaQuoteRecap     recap)
			{
			}

			public void onQuoteClosing (
				MamdaSubscription   sub,
				MamdaQuoteListener  listener,
				MamaMsg             msg,
				MamdaQuoteClosing   closingEvent,
				MamdaQuoteRecap     recap)
			{
			}
		}

		private class UnderlyingTradeHandler : MamdaTradeHandler
		{
			public void onTradeRecap(
				MamdaSubscription   sub,
				MamdaTradeListener  listener,
				MamaMsg             msg,
				MamdaTradeRecap     recap)
			{
			}

			public void onTradeReport (
				MamdaSubscription   sub,
				MamdaTradeListener  listener,
				MamaMsg             msg,
				MamdaTradeReport    trade,
				MamdaTradeRecap     recap)
			{
			}

			public void onTradeGap (
				MamdaSubscription   sub,
				MamdaTradeListener  listener,
				MamaMsg             msg,
				MamdaTradeGap       gapEvent,
				MamdaTradeRecap     recap)
			{
			}

			public void onTradeCancelOrError (
				MamdaSubscription        sub,
				MamdaTradeListener       listener,
				MamaMsg                  msg,
				MamdaTradeCancelOrError  cancelEvent,
				MamdaTradeRecap          recap)
			{
			}

			public void onTradeCorrection (
				MamdaSubscription        sub,
				MamdaTradeListener       listener,
				MamaMsg                  msg,
				MamdaTradeCorrection     correctionEvent,
				MamdaTradeRecap          recap)
			{
			}

			public void onTradeClosing (
				MamdaSubscription        sub,
				MamdaTradeListener       listener,
				MamaMsg                  msg,
				MamdaTradeClosing        closingEvent,
				MamdaTradeRecap          recap)
			{
			}
		}

		private readonly MamdaOptionChain			mChain;
		private MamdaOptionChainViewRangeHandler	mRangeHandler;

		private const double DEFAULT_JITTER_MARGIN = 0.5; /* percent */

		private MamdaOptionAtTheMoneyCompareType mAtTheMoneyType = MamdaOptionAtTheMoneyCompareType.MID_QUOTE;
		private double    mStrikeMargin     = 0.0;
		private int       mNumStrikes       = 0;
		private int       mExpirationDays   = 0;
		private int       mNumExpirations   = 0;
		private double    mJitterMargin     = DEFAULT_JITTER_MARGIN;

		private MamdaOptionExpirationDateSet  mExpirationDateSet = null;
		private DateTime                      mLowExpireDate     = DateTime.MinValue;
		private DateTime                      mHighExpireDate    = DateTime.MinValue;
		private double                        mLowStrike         = 0.0;
		private double                        mHighStrike        = 0.0;

		// The following "underlying" handlers are used if/when we need to
		// check the strike range.  Which, if either, are active depends
		// on the value of mAtTheMoneyType.
		private readonly UnderlyingQuoteHandler mQuoteHandler = new UnderlyingQuoteHandler();
		private readonly UnderlyingTradeHandler mTradeHandler = new UnderlyingTradeHandler();
	}
}
