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
import com.wombat.mama.*;
import java.util.logging.*;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.Iterator;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

/**
 * A class that represents a "view" of a subset of an option chain.
 * The view can be restricted to a percentage or number of strike
 * prices around "the money" as well as to a maximum number of days
 * into the future.  The view will be adjusted to include strike
 * prices within the range as the underlying price moves.  This means
 * that the range of strike prices will change over time.  In order to
 * avoid a "jitter" in the range of strike prices when the underlying
 * price hovers right on the edge of a range boundary, the class also
 * provides a "jitter margin" as some percentage of the underlying
 * price (default is 0.5%).
 */

public class MamdaOptionChainView implements MamdaOptionChainHandler
{
    private final MamdaOptionChain            mChain;

    private MamdaOptionChainViewRangeHandler  mRangeHandler = null;

    private static final double DEFAULT_JITTER_MARGIN = 0.5; /* percent */

    private short     mAtTheMoneyType   = MamdaOptionAtTheMoneyCompareType.MID_QUOTE;
    private double    mStrikeMargin     = 0.0;
    private int       mNumStrikes       = 0;
    private int       mExpirationDays   = 0;
    private int       mNumExpirations   = 0;
    private double    mJitterMargin     = DEFAULT_JITTER_MARGIN;

    private MamdaOptionExpirationDateSet  mExpirationDateSet = null;
    private Date                          mLowExpireDate     = null;
    private Date                          mHighExpireDate    = null;
    private double                        mLowStrike         = 0.0;
    private double                        mHighStrike        = 0.0;

    // The following "underlying" handlers are used if/when we need to
    // check the strike range.  Which, if either, are active depends
    // on the value of mAtTheMoneyType.
    private final UnderlyingQuoteHandler  mQuoteHandler = new UnderlyingQuoteHandler();
    private final UnderlyingTradeHandler  mTradeHandler = new UnderlyingTradeHandler();

    private static Logger mLogger = Logger.getLogger (
        "com.wombat.mamda.options.MamdaOptionChainView");

    /**
     * Create a view on the given option chain.  Multiple views are
     * supported on any given option chain.
     */
    public MamdaOptionChainView (MamdaOptionChain chain)
    {
        mChain = chain;
        resetRange ();
    }

    /**
     * Return the symbol for the option chain.
     */
    public String getSymbol ()
    {
        return mChain.getSymbol();
    }

    /**
     * Set how the underlying price ("at the money") is determined
     * (@see MamdaOptionAtTheMoneyCompareType).
     */
    public void setAtTheMoneyType (short  atTheMoneyType)
    {
        if (mAtTheMoneyType != atTheMoneyType)
        {
            mAtTheMoneyType = atTheMoneyType;
            resetRange();
        }
    }

    /**
     * Set the range of strike prices to be included in the view by
     * percentage variation from the underlying price.  The range of
     * strike prices in the view will vary as the underlying varies.
     * The "jitter margin" avoids switching between ranges too often.
     */
    public void setStrikeRangePercent (double  percentMargin)
    {
        if (mStrikeMargin != percentMargin)
        {
            mStrikeMargin = percentMargin;
            resetRange();
        }
    }

    /**
     * Set the number of strike prices to be included in the view.
     * The "jitter margin" avoids switching between ranges too often.
     */
    public void setStrikeRangeNumber (int  number)
    {
        if (mNumStrikes != number)
        {
            mNumStrikes = number;
            resetRange();
        }
    }

    /**
     * Set the range of expiration dates to be included in the view by
     * the maximum number of days until expiration.  Note: a non-zero
     * range overrides a specific number of expirations set by
     * setNumberOfExpirations().
     */
    public void setExpirationRangeDays (int  expirationDays)
    {
        mExpirationDays = expirationDays;
        resetRange();
    }

    /**
     * Set the number of expiration dates to be included in the view.
     * Note: a non-zero range (set by setExpirationRangeDays())
     * overrides a specific number of expirations.
     */
    public void setNumberOfExpirations (int  numExpirations)
    {
        mNumExpirations = numExpirations;
        resetRange();
    }

    /**
     * Set a "jitter margin" to avoid having the range jump between
     * different strike prices when the underlying price hovers right
     * on the edge of a range boundary.  The underlying is allowed to
     * fluctuate within the jitter margin without the range being
     * reset.  When the underlying moves beyond the jitter margin,
     * the range is reset.
     */
    public void setJitterMargin (double  percentMargin)
    {
        mJitterMargin = percentMargin;
        resetRange();
    }

    /**
     * Return whether an option contract falls within this view's
     * parameters.
     */
    public boolean isVisible (MamdaOptionContract  contract)
    {
        double strikePrice = contract.getStrikePrice();
        Date   expireDate  = contract.getExpireDate();
        return ((mLowExpireDate.compareTo(expireDate) <= 0) &&
                (mHighExpireDate.compareTo(expireDate) >= 0) &&
                (mLowStrike <= strikePrice) &&
                (strikePrice <= mHighStrike));
    }

    /**
     * Return an Iterator over the set of expiration dates within the
     * view.  Each Iterator represents a MamdaOptionExpirationStrikes
     * object.
     */
    public Iterator expirationIterator()
    {
        return mExpirationDateSet.values().iterator();
    }

    /**
     * Handler option chain recaps and initial values.
     */
    public void onOptionChainRecap (
        MamdaSubscription         subscription,
        MamdaOptionChainListener  listener,
        MamaMsg                   msg,
        MamdaOptionChain          chain)
    {
        resetRange();
    }

    /**
     * Handler for option chain structural updates.
     */
    public void onOptionContractCreate (
        MamdaSubscription         subscription,
        MamdaOptionChainListener  listener,
        MamaMsg                   msg,
        MamdaOptionContract       contract,
        MamdaOptionChain          chain)
    {
        resetRange();
    }

    /**
     * Handler for option chain structural updates.
     */
    public void onOptionSeriesUpdate (
        MamdaSubscription         subscription,
        MamdaOptionChainListener  listener,
        MamaMsg                   msg,
        MamdaOptionSeriesUpdate   event,
        MamdaOptionChain          chain)
    {
        resetRange();
    }

    private void resetRange ()
    {
        mExpirationDateSet = filterExpirations (mChain.getAllExpirations());
        filterStrikes (mExpirationDateSet);
        mLogger.fine ("resetRange: dateSet=" + mExpirationDateSet);

        if ((mExpirationDateSet == null) ||
            (mExpirationDateSet.size() == 0))
        {
            mLowExpireDate = new Date(0);
            mHighExpireDate = new Date(System.currentTimeMillis() +
                                        366*24*60*60*1000);
        }
        else
        {
            mLowExpireDate = (Date)mExpirationDateSet.firstKey();
            mHighExpireDate = (Date)mExpirationDateSet.lastKey();
        }
    }

    private MamdaOptionExpirationDateSet filterExpirations (
        MamdaOptionExpirationDateSet  initialSet)
    {
        if (mExpirationDays > 0)
        {
            // Find the subset of expirations between now and some
            // number of days into the future.
            GregorianCalendar futureDate = new GregorianCalendar();
            futureDate.add (Calendar.DATE, mExpirationDays);
            return initialSet.getExpirationsBefore (futureDate.getTime());
        }
        else if (mNumExpirations > 0)
        {
            // Explicit number of expiration dates.
            return initialSet.getExpirations (mNumExpirations);
        }
        else
        {
            // All expiration months.  We *copy* the initial set
            // because we may reduce the set to a particular range of
            // strike prices.
            return new MamdaOptionExpirationDateSet (initialSet);
        }
    }

    private void filterStrikes (MamdaOptionExpirationDateSet  dateSet)
    {
        mLowStrike  = Double.MIN_VALUE;
        mHighStrike = Double.MAX_VALUE;

        // First calculate the high/low strikes
        SortedSet strikeSet =
            mChain.getStrikesWithinPercent(mStrikeMargin, mAtTheMoneyType);

        if ((strikeSet == null) || (strikeSet.size() == 0))
        {
            strikeSet = mChain.getStrikesWithinRangeSize (mNumStrikes,
                                                          mAtTheMoneyType);
        }

        if ((strikeSet == null) || (strikeSet.size() == 0))
        {
            mLogger.fine ("filterStrikes: no strikes or underlying (yet?)");
            return;
        }
        else
        {
            mLogger.fine ("filterStrikes: got " + strikeSet.size() + " in range");
        }

        Double lowStrike  = (Double)strikeSet.first();
        Double highStrike = (Double)strikeSet.last();
        mLowStrike        = lowStrike.doubleValue();
        mHighStrike       = highStrike.doubleValue();

        Iterator expireIter = dateSet.values().iterator();
        while (expireIter.hasNext())
        {
            // Filter the strike prices for one expiration date
            MamdaOptionExpirationStrikes expireStrikes =
                (MamdaOptionExpirationStrikes) expireIter.next();
            expireStrikes.trimStrikes (strikeSet);
        }
    }

    private boolean strikeInRange (double strikePrice)
    {
        return true;
    }

    private boolean expirationInRange (String expirationDate)
    {
        return true;
    }

    private class UnderlyingQuoteHandler implements MamdaQuoteHandler
    {
        public void onQuoteRecap (
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
            MamdaQuoteUpdate    event,
            MamdaQuoteRecap     recap)
        {
        }

        public void onQuoteGap (
            MamdaSubscription   sub,
            MamdaQuoteListener  listener,
            MamaMsg             msg,
            MamdaQuoteGap       event,
            MamdaQuoteRecap     recap)
        {
        }

        public void onQuoteClosing (
            MamdaSubscription   sub,
            MamdaQuoteListener  listener,
            MamaMsg             msg,
            MamdaQuoteClosing   event,
            MamdaQuoteRecap     recap)
        {
        }
    }

    private class UnderlyingTradeHandler implements MamdaTradeHandler
    {
        public void onTradeRecap (
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
            MamdaTradeGap       event,
            MamdaTradeRecap     recap)
        {
        }

        public void onTradeCancelOrError (
            MamdaSubscription        sub,
            MamdaTradeListener       listener,
            MamaMsg                  msg,
            MamdaTradeCancelOrError  event,
            MamdaTradeRecap          recap)
        {
        }

        public void onTradeCorrection (
            MamdaSubscription        sub,
            MamdaTradeListener       listener,
            MamaMsg                  msg,
            MamdaTradeCorrection     event,
            MamdaTradeRecap          recap)
        {
        }

        public void onTradeClosing (
            MamdaSubscription        sub,
            MamdaTradeListener       listener,
            MamaMsg                  msg,
            MamdaTradeClosing        event,
            MamdaTradeRecap          recap)
        {
        }
    }
}
