/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.options;

import com.wombat.mamda.*;
import java.util.logging.*;
import java.util.Comparator;
import java.util.Date;
import java.util.Iterator;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.HashMap;
import java.util.SortedSet;
import java.util.Collection;

/**
 * MamdaOptionChain is a specialized class to represent market data
 * option chains.  The class has capabilities to store the current
 * state of an entire option chain, or a subset of the chain.
 */

public class MamdaOptionChain
{
    private String                 mSymbol             = null;
    private MamdaQuoteListener     mQuoteListener      = null;
    private MamdaTradeListener     mTradeListener      = null;

    private final HashMap          mOptions            = new HashMap (1000);
    private final TreeSet          mCallOptions        = new TreeSet (new ContractComparator());
    private final TreeSet          mPutOptions         = new TreeSet (new ContractComparator());
    private final TreeSet          mExchanges          = new TreeSet ();
    private final TreeSet          mStrikePrices       = new TreeSet ();
    private final MamdaOptionExpirationDateSet mExpirationSet =
        new MamdaOptionExpirationDateSet();

    private static Logger mLogger = Logger.getLogger (
        "com.wombat.mamda.options.MamdaOptionChain");

    /**
     * MamdaOptionChain Constructor. 
     *
     * @param symbol The underlying symbol.
     */
    public MamdaOptionChain (String symbol)
    {
        setSymbol (symbol);
    }

    /**
     * Set the underlying symbol for the option chain.
     *
     * @param symbol The underlying symbol.
     */
    public void setSymbol (String symbol)
    {
        mSymbol = symbol;
    }

    /**
     * Get the underlying symbol for the option chain.
     *
     * @return String The underlying symbol for the option chain.
     */
    public String getSymbol ()
    {
        return mSymbol;
    }

    /**
     * Set the underlying quote listener information.
     * MamdaOptionChain does not use this class itself, but related
     * classes might (e.g., MamdaOptionChainView).
     *
     * @param quoteListener The quote listener for the chain.
     */
    public void setUnderlyingQuoteListener (
        MamdaQuoteListener  quoteListener)
    {
        mQuoteListener = quoteListener;
    }

    /**
     * Set the underlying trade listener information.
     * MamdaOptionChain uses this class to implement the atTheMoney()
     * method.  Related classes might (e.g., MamdaOptionChainView) use
     * it for other reasons.
     *
     * @param tradeListener The trade listener for the chain.
     */
    public void setUnderlyingTradeListener (
        MamdaTradeListener  tradeListener)
    {
        mTradeListener = tradeListener;
    }

    /**
     * Make the underlying quote listener available externally.
     *
     * @return MamdaQuoteListener The quote listener for the chain.
     */
    public MamdaQuoteListener  getUnderlyingQuoteListener ()
    {
        return mQuoteListener;
    }

    /**
     * Make the underlying trade listener available externally.
     *
     * @return MamdaTradeListener The trade listener for the chain.
     */
    public MamdaTradeListener  getUnderlyingTradeListener ()
    {
        return mTradeListener;
    }

    /**
     * Find an option contract by OPRA symbol.
     *
     * @param contractSymbol The option symbol.
     *
     * @return MamdaOptionContract The contract object representing the
     * option.
     */
    public MamdaOptionContract getContract (String contractSymbol)
    {
        return (MamdaOptionContract) mOptions.get(contractSymbol);
    }

    /**
     * Return the superset of regional exchange identifiers for any
     * option in this chain.  Each element in the set is a String
     * object.
     *
     * @return SortedSet Set of regional exchange identifiers.
     */
    public SortedSet getExchanges()
    {
        return mExchanges;
    }

    /**
     * Return the superset of strike prices for this chain.  Each
     * element in the set is a Double object.
     *
     * @return SortedSet Set of strike prices for the chain.
     */
    public SortedSet getStrikePrices()
    {
        return mStrikePrices;
    }

    /**
     * Add an option contract.  This method would not normally be
     * invoked by a user application.  Rather,
     * MamdaOptionChainListener would be most likely to call this
     * method.
     *
     * @param contractSymbol The option instrument symbol.
     * @param contract       The Mamda option contract representation.
     */
    public void addContract (String               contractSymbol,
                             MamdaOptionContract  contract)
    {
        Date   expireDate  = contract.getExpireDate();
        Double strikePrice = new Double(contract.getStrikePrice());
        String exchange    = contract.getExchange();
        char   putCall     = contract.getPutCall();

        mLogger.fine ("adding contract: " + contractSymbol + " " + 
                       exchange + " " + strikePrice);
        mOptions.put (contractSymbol, contract);

        if (contract.getPutCall() == MamdaOptionContract.PC_CALL)
        {
            mCallOptions.add (contract);
        }
        else
        {
            mPutOptions.add (contract);
        }

        // Add the contract to the expiration-by-strike set.
        MamdaOptionExpirationStrikes expireStrikes =
            (MamdaOptionExpirationStrikes) mExpirationSet.get (expireDate);

        if (expireStrikes == null)
        {
            expireStrikes = new MamdaOptionExpirationStrikes();
            mExpirationSet.put (expireDate, expireStrikes);
        }

        MamdaOptionStrikeSet strikeSet =
            (MamdaOptionStrikeSet) expireStrikes.get (strikePrice);

        if (strikeSet == null)
        {
            strikeSet = new MamdaOptionStrikeSet (expireDate,
                                                  strikePrice.doubleValue());
            expireStrikes.put (strikePrice, strikeSet);
        }

        MamdaOptionContractSet contractSet =
            (putCall == MamdaOptionContract.PC_CALL) ?
                strikeSet.getCallSet() : strikeSet.getPutSet();

        if (MamdaOptionExchangeUtils.isBbo(exchange))
        {
            contractSet.setBboContract (contract);
        }
        else if (MamdaOptionExchangeUtils.isWombatBbo(exchange))
        {
            contractSet.setWombatBboContract (contract);
        }
        else
        {
            contractSet.setExchangeContract (exchange, contract);
            mExchanges.add (exchange);
        }

        mStrikePrices.add (strikePrice);
        mLogger.fine ("number of expirations: " + mExpirationSet.size());
    }

    /**
     * Remove an option contract.  This method would not normally be
     * invoked by a user application.  Rather,
     * MamdaOptionChainListener would be most likely to call this
     * method.
     */
    public void removeContract (String contractSymbol) 
    {
    }

    /**
     * Get the price of the option underlying.
     * Determine the underlying price ("at the money"), based on the
     * mode of calculation.
     *
     * @param compareType The mode of calculation
     * (<code>MamdaOptionAtTheMoneyCompareType</code>)
     *
     * @return double The price of the underlying.
     * 
     * @see MamdaOptionAtTheMoneyCompareType
     */
    public double getAtTheMoney (
        short  compareType)
    {
        double atTheMoney = 0.0;
        switch (compareType)
        {
            case MamdaOptionAtTheMoneyCompareType.MID_QUOTE:
                if (mQuoteListener == null) return 0.0;
                atTheMoney = (mQuoteListener.getBidPrice().getValue() +
                              mQuoteListener.getAskPrice().getValue()) / 2.0;
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

    /**
     * Return whether the price within a % of the money.
     * Determine whether some price (e.g. a strike price) is within a
     * given percentage range of the underlying (at the money) price.
     *
     * @param price       The strike price to check.
     * @param percentage  What % to check the strike price against.
     * @param compareType What price we are checking against (see
     * <code>MamdaOptionAtTheMoneyCompareType</code>)
     *
     * @return boolean Whether the proce is within % of the money.
     *
     * @see MamdaOptionAtTheMoneyCompareType
     */
    public boolean getIsPriceWithinPercentOfMoney (
        double    price,
        double    percentage,
        short     compareType)
    {
        double atTheMoney  = getAtTheMoney(compareType);
        if (atTheMoney == 0.0)
            return false;

        return (((1.0 - percentage) <= price) ||
                (price <= (1.0 + percentage)));
    }

    /**
     * Determine the set of strike prices that are included in a given
     * percentage range of the underlying price.  If there are no
     * strikes within the percentage range, then both strike prices
     * are set to zero.
     *
     * @param percentage  The percentage range of the underlying price.
     * @param compareType Which underlying price to compare to.
     *
     * @return SortedSet Set of strike prices.
     *
     * @see MamdaOptionAtTheMoneyCompareType
     */
    public SortedSet getStrikesWithinPercent (
        double    percentage,
        short     compareType)
    {
        percentage /= 100.0;
        if (percentage <= 0.0)
            return null;

        double atTheMoney  = getAtTheMoney(compareType);
        if (atTheMoney == 0.0)
            return null;

        Double    lowPercent   = new Double (atTheMoney * (1.0 - percentage));
        Double    highPercent  = new Double (atTheMoney * (1.0 + percentage));
        SortedSet strikeSubSet = mStrikePrices.subSet (lowPercent,
                                                       highPercent);
        return strikeSubSet;
    }

    /**
     * Determine the set of strike prices that are included in a given
     * fixed size range of strikes surrounding the underlying price.
     * If rangeLen is odd, then the strike price nearest to the
     * underlying price is treated as a higher strike price.  If
     * rangeLen is even and the underlying price is exactly equal to a
     * strike price, then that strike price is treated as a higher
     * strike price.
     *
     * @param rangeLength Number of strike prices to include in result. 
     * @param compareType What underlying price to use as a comparator.
     *
     * @return SortedSet Resulting set of strike prices.
     *
     * @see MamdaOptionAtTheMoneyCompareType
     */
    public SortedSet getStrikesWithinRangeSize (
        int       rangeLength,
        short     compareType)
    {
        if (rangeLength <= 0)
            return null;

        double atTheMoney  = getAtTheMoney(compareType);
        if (atTheMoney == 0.0)
            return null;

        int     countToMoney    = 0;
        int     countFromMoney  = 0;
        double  halfRangeLength = rangeLength / 2.0;
        Double  lowerBound      = null;
        Double  upperBound      = null;

        // First loop determines the upper bound and count the number
        // of strikes that are less than the "money".
        Iterator strikeIter = mStrikePrices.iterator();
        while (strikeIter.hasNext())
        {
            Double strikePrice = (Double) strikeIter.next();
            if (strikePrice.doubleValue() < atTheMoney)
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
                upperBound = new Double(strikePrice.doubleValue()+0.00001);
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
            Double strikePrice = (Double) strikeIter.next();
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
        if (lowerBound == null)
            lowerBound = new Double(0.0);
        if (upperBound == null)
            upperBound = new Double(Double.MAX_VALUE);

        SortedSet result = mStrikePrices.subSet(lowerBound,upperBound);
        lowerBound = (Double) result.first();
        upperBound = (Double) result.last();
        mLogger.finer ("getStrikesWithinRangeSize: lowerBound=" + lowerBound +
                       " atTheMoney=" + atTheMoney +
                       " upperBound=" + upperBound);
        return result;
    }

    /**
     * Return an iterator over all call option contracts.  The type of
     * object that Iterator.next() returns is <code>MamdaOptionContract</code>.
     *
     * @return Iterator The iterator of call options.
     */
    public Iterator callIterator()
    {
        return mCallOptions.iterator();
    }

    /**
     * Return an iterator over all put option contracts.  The type of
     * object that Iterator.next() returns is MamdaOptionContract.
     *
     * @return Iterator The iterator of put options.
     */
    public Iterator putIterator()
    {
        return mPutOptions.iterator();
    }

    /**
     * Return a set of MamdaOptionExpirationDateSet.
     *
     * @return MamdaOptionExpirationDateSet Set of expiration dates.
     */
    public MamdaOptionExpirationDateSet getAllExpirations ()
    {
        return mExpirationSet;
    }

    /**
     * Print the contents of the chain to standard out.
     */
    public void dump ()
    {
        System.out.println ("Dump chain: " + mSymbol);

        int      i        = 0;
        Iterator callIter = mCallOptions.iterator();
        Iterator putIter  = mPutOptions.iterator();

        while (callIter.hasNext() || putIter.hasNext())
        {
            System.out.print ("" + i + " | ");
            if (callIter.hasNext())
            {
                MamdaOptionContract callContract =
                    (MamdaOptionContract)callIter.next();
                System.out.print ("" + callContract.getSymbol() + " " +
                                  callContract.getExchange() + "   " +
                                  callContract.getExpireDate() + "   " +
                                  callContract.getStrikePrice());
            }
            else
            {
                System.out.print ("          ");
            }

            System.out.print (" | ");

            if (putIter.hasNext())
            {
                MamdaOptionContract putContract =
                    (MamdaOptionContract)putIter.next();
                System.out.print ("" + putContract.getSymbol() + " " +
                                  putContract.getExchange() + "   " +
                                  putContract.getExpireDate() + "   " +
                                  putContract.getStrikePrice());
            }
            else
            {
            }
            System.out.println();
            ++i;
        }
    }

    private class ContractComparator implements Comparator
    {
        /* Ascending expiration month, strike price, and exchange. */
        public int compare (Object o1, Object o2)
        {
            MamdaOptionContract contract1 = (MamdaOptionContract)o1;
            MamdaOptionContract contract2 = (MamdaOptionContract)o2;

            // Expiration month
            Date expiration1 = contract1.getExpireDate();
            Date expiration2 = contract2.getExpireDate();
            int comp = expiration1.compareTo(expiration2);
            if (comp == 0)
            {
                // Strike price
                Double strike1 = new Double(contract1.getStrikePrice());
                Double strike2 = new Double(contract2.getStrikePrice());
                comp = strike1.compareTo(strike2);
            }
            if (comp == 0)
            {
                // Exchange
                String exchange1 = contract1.getExchange();
                String exchange2 = contract2.getExchange();
                comp = exchange1.compareTo(exchange2);
            }
            return comp;
        }
        public boolean equals (Object o1, Object o2)
        { 
            return (compare(o1,o2) == 0);
        }
    }
}
