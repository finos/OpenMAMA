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
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * A class that represents a single option contract.  Instances of
 * this object are typically created by the MamdaOptionChainListener.
 * Applications may attach a custom object to each instance of
 * MamdaOptionContract.
 *
 * Note: User applications can be notified of creation of
 * MamdaOptionContract instances via the
 * MamdaOptionChainListener.onOptionContractCreate() method.  
 *
 * Note: It is possible to provide individual MamdaTradeHandler and
 * MamdaQuoteHandler handlers for trades and quotes, even though the
 * MamdaOptionChainHandler also provides a general callback interface
 * for updates to individual contracts.  
 */

public class MamdaOptionContract
{
    public static final char PC_UNKNOWN              = ' ';
    public static final char PC_CALL                 = 'C';
    public static final char PC_PUT                  = 'P';

    public static final char EXERCISE_STYLE_AMERICAN = 'A';
    public static final char EXERCISE_STYLE_EUROPEAN = 'E';
    public static final char EXERCISE_STYLE_CAPPED   = 'C';
    public static final char EXERCISE_STYLE_UNKNOWN  = 'Z';

    private String                   mSymbol                = null;
    private String                   mExchange              = null;
    private Date                     mExpireDate            = null;
    private double                   mStrikePrice           = 0.0;
    private char                     mPutCall               = PC_UNKNOWN;
    private long                     mOpenInterest          = 0;
    private char                     mExerciseStyle         = EXERCISE_STYLE_UNKNOWN;
    private boolean                  mInScope               = false;
    private MamdaTradeListener       mTradeListener         = new MamdaTradeListener();
    private MamdaQuoteListener       mQuoteListener         = new MamdaQuoteListener();
    private MamdaFundamentalListener mFundamentalListener   = new MamdaFundamentalListener();
    private Object                   mCustomObject          = null;
    private boolean                  mInView                = false;
    private SimpleDateFormat         mExpireFormat          = new SimpleDateFormat("MMMyy");

    private MamdaOptionContract()
    {
        // Hide the default constructor.
    }

    /**
     * Constructor from expiration date, strike price, and put/call
     * indicator.
     */
    public MamdaOptionContract (String  symbol,
                                String  exchange,
                                Date    expireDate,
                                double  strikePrice,
                                char    putCall)
    {
        mSymbol        = symbol;
        mExchange      = exchange;
        mExpireDate    = expireDate;
        mStrikePrice   = strikePrice;
        mPutCall       = putCall;
    }

    /**
     * Set the open interest size.
     */
    public void setOpenInterest (long  openInterest)
    {
        mOpenInterest = openInterest;
    }

    /**
     * Set the exercise style.
     */
    public void setExerciseStyle (char  exerciseStyle)
    {
        mExerciseStyle = exerciseStyle;
    }

    /**
     * Return the OPRA contract symbol.
     */
    public String getSymbol ()
    {
        return mSymbol;
    }

    /**
     * Return the exchange.
     */
    public String getExchange ()
    {
        return mExchange;
    }

    /**
     * Return the expiration date.
     */
    public Date getExpireDate ()
    {
        return mExpireDate;
    }

    /**
     * Return the expiration date as a commonly formatted string (MMMyy).
     */
    public String getExpireDateStr ()
    {
        return mExpireFormat.format(mExpireDate);
    }

    /**
     * Return the strike price.
     */
    public double getStrikePrice ()
    {
        return mStrikePrice;
    }

    /**
     * Return the put/call indicator.
     */
    public char getPutCall ()
    {
        return mPutCall;
    }

    /**
     * Return the open interest.
     */
    public long getOpenInterest ()
    {
        return mOpenInterest;
    }

    /**
     * Return the exercise style.
     */
    public char getExerciseStyle ()
    {
        return mExerciseStyle;
    }

    /**
     * Add a MamdaTradeHandler for handling trade updates to this
     * option contract.
     */
    public void addTradeHandler (MamdaTradeHandler handler)
    {
        mTradeListener.addHandler(handler);
    }

    /**
     * Add a MamdaQuoteHandler for handling quote updates to this
     * option contract.
     */
    public void addQuoteHandler (MamdaQuoteHandler handler)
    {
        mQuoteListener.addHandler(handler);
    }

    /**
     * Add a MamdaFundamentalHandler for handling fundamental updates
     * to this option contract.
     */
    public void addFundamentalHandler (MamdaFundamentalHandler handler)
    {
        mFundamentalListener.addHandler(handler);
    }

    /**
     * Add a custom object to this option contract.  Such an object
     * might contain customer per-contract data.
     */
    public void setCustomObject (Object object)
    {
        mCustomObject = object;
    }

    /**
     * Return the current trade fields.
     */
    public MamdaTradeRecap getTradeInfo ()
    {
        return mTradeListener;
    }

    /**
     * Return the current quote fields.
     */
    public MamdaQuoteRecap getQuoteInfo ()
    {
        return mQuoteListener;
    }

    /**
     * Return the current fundamental fields.
     */
    public MamdaFundamentals getFundamentalsInfo ()
    {
        return mFundamentalListener;
    }

    /**
     * Return the custom object.
     */
    public Object getCustomObject ()
    {
        return mCustomObject;
    }

    /**
     * Return the trade listener.
     */
    public MamdaTradeListener getTradeListener ()
    {
        return mTradeListener;
    }

    /**
     * Return the current quote listener.
     */
    public MamdaQuoteListener getQuoteListener ()
    {
        return mQuoteListener;
    }

    /**
     * Return the current fundamental listener.
     */
    public MamdaFundamentalListener getFundamentalListener ()
    {
        return mFundamentalListener;
    }

    /**
     * Set whether this contract is in the "view" within the option
     * chain.  
     * @see MamdaOptionChain
     */
    public void setInView (boolean inView)
    {
        mInView = inView;
    }

    /**
     * Return whether this contract is in the "view" within the option
     * chain.  
     * @see MamdaOptionChain
     */
    public boolean getInView ()
    {
        return mInView;
    }
}
