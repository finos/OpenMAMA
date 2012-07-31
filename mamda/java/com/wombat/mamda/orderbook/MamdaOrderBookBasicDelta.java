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

package com.wombat.mamda.orderbook;

import java.io.OutputStream;
import java.io.PrintStream;

/**
 * MamdaOrderBookBasicDelta is a class that saves information about a
 * basic order book delta.  A basic delta is one that affects a
 * single order book entry.
 */

public class MamdaOrderBookBasicDelta
{

    private MamdaOrderBookPriceLevel mPriceLevel;
    private MamdaOrderBookEntry      mEntry;
    private double                   mPlDeltaSize;
    private char                     mPlAction;
    private char                     mEntryAction;

    public MamdaOrderBookBasicDelta ()
    {
        clear ();
    }

    public MamdaOrderBookBasicDelta (MamdaOrderBookBasicDelta source)
    {
        mPriceLevel  = source.mPriceLevel;
        mEntry       = source.mEntry;
        mPlDeltaSize = source.mPlDeltaSize;
        mPlAction    = source.mPlAction;
        mEntryAction = source.mEntryAction;                   
    }

    /**
     * Clear the delta.
     */
    public void clear ()
    {
        mPriceLevel  = null;
        mEntry       = null;
        mPlDeltaSize = 0.0;
        mPlAction    = MamdaOrderBookEntry.ACTION_UNKNOWN;
        mEntryAction = MamdaOrderBookEntry.ACTION_UNKNOWN;
    }

    /**
     * Set the delta info.
     * @param entry The entry
     * @param level The level
     * @param plDeltaSize  The delta size
     * @param plAction  The price level action
     * @param entryAction The entry action
     */
    public void set (MamdaOrderBookEntry      entry,
                     MamdaOrderBookPriceLevel level,
                     double                   plDeltaSize,
                     char                     plAction,
                     char                     entryAction)
    {
        mPriceLevel  = level;
        mEntry       = entry;
        mPlDeltaSize = plDeltaSize;
        mPlAction    = plAction;
        mEntryAction = entryAction;
    }

    /**
     * Set the MamdaOrderBookPriceLevel object to which this entry
     * belongs.  This method is invoked internally, by the MAMDA API,
     * when an entry is added to a price level.
     *
     * @param level The price level to be associated with.
     */
    public void setPriceLevel (MamdaOrderBookPriceLevel level)
    {
        mPriceLevel = level;
    }

    /**
     * Set the delta action with respect to the price level.
     *
     * @param action The price level action.
     */
    public void setPlDeltaAction (char action)
    {
        mPlAction = action;
    }

    /**
     * Get the MamdaOrderBookPriceLevel object related to this basic delta.
     *
     * @return The price level.
     */
    public MamdaOrderBookPriceLevel getPriceLevel ()
    {
        return mPriceLevel;
    }

    /**
     * Get the MamdaOrderBookEntry object related to this basic delta.
     *
     * @return The entry.
     */
    public MamdaOrderBookEntry getEntry ()
    {
        return mEntry;
    }

    /**
     * Get the difference in size for the price level.
     *
     * @return The price level size delta.
     */
    public double getPlDeltaSize ()
    {
        return mPlDeltaSize;
    }

    /**
     * Get the delta action with respect to the price level.
     *
     * @return The price level action.
     */
    public char getPlDeltaAction ()
    {
        return mPlAction;
    }

    /**
     * Get the delta action with respect to the entry.
     *
     * @return The entry action.
     */
    public char getEntryDeltaAction ()
    {
        return mEntryAction;
    }

    /**
     * Get the MamdaOrderBook object to which this delta belongs.
     *
     * @return The order book related to this delta.
     */
    public MamdaOrderBook getOrderBook ()
    {
        if (mPriceLevel != null)
            return mPriceLevel.getOrderBook();
        else
            return null;
    }

    /**
     * Dump the simple update to the output stream.
     *
     * @param output The <code>OutputStream</code> to write the update to.
     */
    public void dump (OutputStream output)
    {
        PrintStream out = new PrintStream (output, true);
        out.print ("Price=" + mPriceLevel.getPrice ());
        out.print (", Side=" + mPriceLevel.getSide ());
        out.print (", PlDeltaAction=" + mPlAction);
        out.print (", EntryId=" + ((mEntry == null) ? "null" : mEntry.getId ()));
        out.print (", EntryAction=" + mEntryAction + "\n");
    }

    /**
     * Dump the simple update to the output stream.
     *
     */
    public void dump ()
    {
        dump (System.out);
    }
}
