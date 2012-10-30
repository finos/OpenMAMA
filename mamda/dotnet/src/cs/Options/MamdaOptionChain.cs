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
	/// MamdaOptionChain is a specialized class to represent market data
	/// option chains.  The class has capabilities to store the current
	/// state of an entire option chain, or a subset of the chain.
	/// </summary>
	public class MamdaOptionChain
	{
		/// <summary>
		/// MamdaOptionChain Constructor. 
		/// </summary>
		/// <param name="symbol">The underlying symbol.</param>
		public MamdaOptionChain(string symbol)
		{
			setSymbol(symbol);
		}

		/// <summary>
		/// Set the underlying symbol for the option chain.
		/// </summary>
		/// <param name="symbol">The underlying symbol.</param>
		public void setSymbol(string symbol)
		{
			mSymbol = symbol;
		}

		/// <summary>
		/// Get the underlying symbol for the option chain.
		/// </summary>
		/// <returns>The underlying symbol for the option chain.</returns>
		public string getSymbol()
		{
			return mSymbol;
		}

		/// <summary>
		/// Set the underlying quote listener information.
		/// MamdaOptionChain does not use this class itself, but related
		/// classes might (e.g., MamdaOptionChainView).
		/// </summary>
		/// <param name="quoteListener">The quote listener for the chain.</param>
		public void setUnderlyingQuoteListener(MamdaQuoteListener quoteListener)
		{
			mQuoteListener = quoteListener;
		}

		/// <summary>
		/// Set the underlying trade listener information.
		/// MamdaOptionChain uses this class to implement the atTheMoney()
		/// method.  Related classes might (e.g., MamdaOptionChainView) use
		/// it for other reasons.
		/// </summary>
		/// <param name="tradeListener">The trade listener for the chain.</param>
		public void setUnderlyingTradeListener(MamdaTradeListener tradeListener)
		{
			mTradeListener = tradeListener;
		}

		/// <summary>
		/// Make the underlying quote listener available externally.
		/// </summary>
		/// <returns>The quote listener for the chain.</returns>
		public MamdaQuoteListener getUnderlyingQuoteListener()
		{
			return mQuoteListener;
		}

		/// <summary>
		/// Make the underlying trade listener available externally.
		/// </summary>
		/// <returns>The trade listener for the chain.</returns>
		public MamdaTradeListener getUnderlyingTradeListener()
		{
			return mTradeListener;
		}

		/// <summary>
		/// Find an option contract by OPRA symbol.
		/// </summary>
		/// <param name="contractSymbol"></param>
		/// <returns>MamdaOptionContract The contract object representing the 
		/// option.</returns>
		public MamdaOptionContract getContract(String contractSymbol)
		{
			return (MamdaOptionContract) mOptions.valueOf(contractSymbol);
		}

		/// <summary>
		/// Return the superset of regional exchange identifiers for any
		/// option in this chain.  Each element in the set is a String
		/// object.
		/// </summary>
		/// <returns>Set of regional exchange identifiers.</returns>
		public SortedSet getExchanges()
		{
			return mExchanges;
		}

		/// <summary>
		/// Return the superset of strike prices for this chain.  Each
		/// element in the set is a Double object.
		/// </summary>
		/// <returns>Set of strike prices for the chain.</returns>
		public SortedSet getStrikePrices()
		{
			return mStrikePrices;
		}

		/// <summary>
		/// Add an option contract.  This method would not normally be
		/// invoked by a user application.  Rather,
		/// MamdaOptionChainListener would be most likely to call this
		/// method.
		/// </summary>
		/// <param name="contractSymbol">The option instrument symbol.</param>
		/// <param name="contract">The Mamda option contract representation.</param>
		public void addContract(
			string               contractSymbol,
			MamdaOptionContract  contract)
		{
			DateTime expireDate = contract.getExpireDate();
			double strikePrice  = contract.getStrikePrice();
			string exchange     = contract.getExchange();
			MamdaOptionContract.PutOrCall putCall = contract.getPutCall();

			mOptions.put(contractSymbol, contract);
			if (putCall == MamdaOptionContract.PutOrCall.Call)
			{
				mCallOptions.add(contract);
			}
			else
			{
				mPutOptions.add(contract);
			}

			// Add the contract to the expiration-by-strike set.
			MamdaOptionExpirationStrikes expireStrikes = (MamdaOptionExpirationStrikes)mExpirationSet.valueOf(expireDate);
			if (expireStrikes == null)
			{
				expireStrikes = new MamdaOptionExpirationStrikes();
				mExpirationSet.put(expireDate, expireStrikes);
			}
			MamdaOptionStrikeSet strikeSet = (MamdaOptionStrikeSet)expireStrikes.valueOf(strikePrice);
			if (strikeSet == null)
			{
				strikeSet = new MamdaOptionStrikeSet(expireDate, strikePrice);
				expireStrikes.put(strikePrice, strikeSet);
			}
			MamdaOptionContractSet contractSet = (putCall == MamdaOptionContract.PutOrCall.Call) ?
				strikeSet.getCallSet() : strikeSet.getPutSet();
			if (MamdaOptionExchangeUtils.isBbo(exchange))
			{
				contractSet.setBboContract(contract);
			}
			else if (MamdaOptionExchangeUtils.isWombatBbo(exchange))
			{
				contractSet.setWombatBboContract(contract);
			}
			else
			{
				contractSet.setExchangeContract(exchange, contract);
				mExchanges.add(exchange);
			}
			mStrikePrices.add(strikePrice);
		}

		/// <summary>
		/// Remove an option contract.  This method would not normally be
		/// invoked by a user application.  Rather,
		/// MamdaOptionChainListener would be most likely to call this
		/// method.
		/// </summary>
		/// <param name="contractSymbol"></param>
		public void removeContract(string contractSymbol)
		{
			/*TODO*/
		}

		/// <summary>
		/// Get the price of the option underlying.
		/// Determine the underlying price ("at the money"), based on the
		/// mode of calculation.
		/// </summary>
		/// <param name="compareType">The mode of calculation
		/// <see cref="MamdaOptionAtTheMoneyCompareType">(MamdaOptionAtTheMoneyCompareType)</see></param>
		/// <returns>The price of the underlying.</returns>
		public double getAtTheMoney(MamdaOptionAtTheMoneyCompareType compareType)
		{
			double atTheMoney = 0.0;
			switch (compareType)
			{
				case MamdaOptionAtTheMoneyCompareType.MID_QUOTE:
					if (mQuoteListener == null) return 0.0;
					atTheMoney = (mQuoteListener.getBidPrice().getValue() + mQuoteListener.getAskPrice().getValue()) / 2.0;
					break;
				case MamdaOptionAtTheMoneyCompareType.BID:
					if (mQuoteListener == null) return 0.0;
					atTheMoney = mQuoteListener.getBidPrice().getValue();
					break;
				case MamdaOptionAtTheMoneyCompareType.ASK:
					if (mQuoteListener == null) return 0.0;
					atTheMoney = mQuoteListener.getAskPrice().getValue();
					break;
				case MamdaOptionAtTheMoneyCompareType.LAST_TRADE:
					if (mTradeListener == null) return 0.0;
					atTheMoney = mTradeListener.getLastPrice().getValue();
					break;
			}
			return atTheMoney;
		}

		/// <summary>
		/// Return whether the price is within a % of the money.
		/// Determine whether some price (e.g. a strike price) is within a
		/// given percentage range of the underlying (at the money) price.
		/// </summary>
		/// <param name="price">The strike price to check.</param>
		/// <param name="percentage">What % to check the strike price against.</param>
		/// <param name="compareType">What price we are checking against <see cref="MamdaOptionAtTheMoneyCompareType">(see MamdaOptionAtTheMoneyCompareType)</see></param>
		/// <returns>Whether the price is within % of the money.</returns>
		public bool getIsPriceWithinPercentOfMoney(
			double								price,
			double								percentage,
			MamdaOptionAtTheMoneyCompareType	compareType)
		{
			double atTheMoney = getAtTheMoney(compareType);
			if (atTheMoney == 0.0)
				return false;

			return (((1.0 - percentage) <= price) || (price <= (1.0 + percentage)));
		}

		/// <summary>
		/// Determine the set of strike prices that are included in a given
		/// percentage range of the underlying price.  If there are no
		/// strikes within the percentage range, then both strike prices
		/// are set to zero.
		/// <see cref="MamdaOptionAtTheMoneyCompareType"/>
		/// </summary>
		/// <param name="percentage">The percentage range of the underlying price.</param>
		/// <param name="compareType">Which underlying price to compare to.</param>
		/// <returns>Set of strike prices.</returns>
		public SortedSet getStrikesWithinPercent(
			double	percentage,
			MamdaOptionAtTheMoneyCompareType compareType)
		{
			percentage /= 100.0;
			if (percentage <= 0.0)
				return null;

			double atTheMoney = getAtTheMoney(compareType);
			if (atTheMoney == 0.0)
				return null;

			double    lowPercent   = atTheMoney * (1.0 - percentage);
			double    highPercent  = atTheMoney * (1.0 + percentage);
			SortedSet strikeSubSet = mStrikePrices.subSet(lowPercent, highPercent);
			return strikeSubSet;
		}

		/// <summary>
		/// Determine the set of strike prices that are included in a given
		/// fixed size range of strikes surrounding the underlying price.
		/// If rangeLen is odd, then the strike price nearest to the
		/// underlying price is treated as a higher strike price.  If
		/// rangeLen is even and the underlying price is exactly equal to a
		/// strike price, then that strike price is treated as a higher
		/// strike price.
		/// <see cref="MamdaOptionAtTheMoneyCompareType"/>
		/// </summary>
		/// <param name="rangeLength">Number of strike prices to include in result. </param>
		/// <param name="compareType">What underlying price to use as a comparator.</param>
		/// <returns>Resulting set of strike prices.</returns>
		public SortedSet getStrikesWithinRangeSize (
			int									rangeLength,
			MamdaOptionAtTheMoneyCompareType	compareType)
		{
			if (rangeLength <= 0)
				return null;

			double atTheMoney  = getAtTheMoney(compareType);
			if (atTheMoney == 0.0)
				return null;

			int    countToMoney = 0;
			int    countFromMoney = 0;
			double halfRangeLength = rangeLength / 2.0;
			double lowerBound = Double.NaN;
			double upperBound = Double.NaN;

			// First loop determines the upper bound and count the number
			// of strikes that are less than the "money".
			Iterator strikeIter = mStrikePrices.iterator();
			while (strikeIter.hasNext())
			{
				double strikePrice = (double)strikeIter.next();
				if (strikePrice < atTheMoney)
				{
					// We're still less than the "money", so keep counting up.
					countToMoney++;
				}
				else
				{
					// Check the upper bound. Take a price that is
					// slightly higher than this strike price because we
					// want to include it (Set.subSet excludes the
					// toElement).
					countFromMoney++;
					upperBound = strikePrice + 0.00001;
					if (countFromMoney >= halfRangeLength)
					{
						// We've passed the upper end of the range
						break;
					}
				}
			}

			// Second loop determines the lower bound.
			int targetCount = countToMoney - (rangeLength - countFromMoney);
			strikeIter = mStrikePrices.iterator();
			while (strikeIter.hasNext())
			{
				double strikePrice = (double)strikeIter.next();
				if (targetCount > 0)
				{
					// We have not yet hit the lowerBound.
					targetCount--;
				}
				else
				{
					lowerBound = strikePrice;
					break;
				}
			}
			if (Double.IsNaN(lowerBound))
				lowerBound = 0.0;
			if (Double.IsNaN(upperBound))
				upperBound = Double.MaxValue;

			SortedSet result = mStrikePrices.subSet(lowerBound, upperBound);
			lowerBound = (double)result.first();
			upperBound = (double)result.last();

			return result;
		}

		/// <summary>
		/// Return an iterator over all call option contracts.  The type of
		/// object that Iterator.next() returns is <code>MamdaOptionContract</code>.
		/// </summary>
		/// <returns>The iterator of call options.</returns>
		public Iterator callIterator()
		{
			return mCallOptions.iterator();
		}

		/// <summary>
		/// Return an iterator over all put option contracts.  The type of
		/// object that Iterator.next() returns is MamdaOptionContract.
		/// </summary>
		/// <returns></returns>
		public Iterator putIterator()
		{
			return mPutOptions.iterator();
		}

		/// <summary>
		/// Return a set of MamdaOptionExpirationDateSet.
		/// </summary>
		/// <returns>Set of expiration dates.</returns>
		public MamdaOptionExpirationDateSet getAllExpirations()
		{
			return mExpirationSet;
		}

		/// <summary>
		/// Print the contents of the chain to standard out.
		/// </summary>
		public void dump ()
		{
			Console.WriteLine("Dump chain: {0}", mSymbol);

			int i = 0;
			Iterator callIter = mCallOptions.iterator();
			Iterator putIter = mPutOptions.iterator();

			while (callIter.hasNext() || putIter.hasNext())
			{
				Console.WriteLine("{0} | ", i);
				if (callIter.hasNext())
				{
					MamdaOptionContract callContract = (MamdaOptionContract)callIter.next();
					dump(callContract);
				}
				else
				{
					Console.Write("          ");
				}

				Console.Write(" | ");

				if (putIter.hasNext())
				{
					MamdaOptionContract putContract = (MamdaOptionContract)putIter.next();
					dump(putContract);
				}
				else
				{
				}
				Console.WriteLine();
				++i;
			}
		}

		private void dump(MamdaOptionContract contract)
		{
			Console.Write("{0} {1}   {2}   {3}",
				contract.getSymbol(),
				contract.getExchange(),
				contract.getExpireDate(),
				contract.getStrikePrice());
		}

		private class ContractComparator : Comparator
		{
			/* Ascending expiration month, strike price, and exchange. */
			public int compare(object o1, object o2)
			{
				MamdaOptionContract contract1 = (MamdaOptionContract)o1;
				MamdaOptionContract contract2 = (MamdaOptionContract)o2;

				// Expiration month
				DateTime expiration1 = contract1.getExpireDate();
				DateTime expiration2 = contract2.getExpireDate();
				int comp = expiration1.CompareTo(expiration2);
				if (comp == 0)
				{
					// Strike price
					double strike1 = contract1.getStrikePrice();
					double strike2 = contract2.getStrikePrice();
					comp = strike1.CompareTo(strike2);
				}
				if (comp == 0)
				{
					// Exchange
					string exchange1 = contract1.getExchange();
					string exchange2 = contract2.getExchange();
					comp = exchange1.CompareTo(exchange2);
				}
				return comp;
			}

			public bool equals(object o1, object o2)
			{ 
				return (compare(o1, o2) == 0);
			}
		}

		private string                 mSymbol;
		private MamdaQuoteListener     mQuoteListener;
		private MamdaTradeListener     mTradeListener;

		private readonly HashMap mOptions    = new HashMap(1000);
		private readonly TreeSet mCallOptions= new TreeSet(new ContractComparator());
		private readonly TreeSet mPutOptions = new TreeSet(new ContractComparator());
		private readonly TreeSet mExchanges  = new TreeSet();
		private readonly TreeSet mStrikePrices = new TreeSet();
		private readonly MamdaOptionExpirationDateSet mExpirationSet = new MamdaOptionExpirationDateSet();
	}
}
