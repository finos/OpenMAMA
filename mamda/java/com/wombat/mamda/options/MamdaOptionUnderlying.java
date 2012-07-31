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

/**
 * A class that represents the underlying for an option chain.
 * Instances of this object are typically created by the
 * MamdaOptionChain.  Applications may attach a custom object to each
 * instance of MamdaOptionUnderlying.
 *
 * Note: It is possible to provide individual MamdaTradeHandler and
 * MamdaQuoteHandler handlers for trades and quotes, even though the
 * MamdaOptionChainHandler also provides a general callback interface
 * for updates to the underlying.  
 */

public class MamdaOptionUnderlying
{
    private MamdaTradeListener  mTradeListener = new MamdaTradeListener();
    private MamdaQuoteListener  mQuoteListener = new MamdaQuoteListener();
    private Object              mCustomObject  = null;

    /**
     * Constructor from expiration date, strike price, and put/call
     * indicator.
     */
    public MamdaOptionUnderlying ()
    {
    }

    /**
     * Add a MamdaTradeHandler for handling trade updates to this
     * underlying.
     */
    public void addTradeHandler (MamdaTradeHandler handler)
    {
        mTradeListener.addHandler(handler);
    }

    /**
     * Add a MamdaQuoteHandler for handling quote updates to this
     * underlying.
     */
    public void addQuoteHandler (MamdaQuoteHandler handler)
    {
        mQuoteListener.addHandler(handler);
    }

    /**
     * Add a custom object to this option underlying.  Such an object
     * might contain customer data for the underlying.
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
     * Return the custom object.
     */
    public Object getCustomObject ()
    {
        return mCustomObject;
    }
}
