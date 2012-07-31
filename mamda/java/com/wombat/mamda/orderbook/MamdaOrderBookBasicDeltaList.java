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
import java.io.PrintWriter;
import java.util.*;

/**
 * MamdaOrderBookBasicDeltaList is a class that saves information
 * about an order book delta that involves multiple entries and/or
 * price levels.  For example, a modified order may involve a price
 * change that means moving an entry from one price level to another.
 * A delta list is made up of several basic deltas, which can be
 * iterated over by methods provided in the class.
 */

public class MamdaOrderBookBasicDeltaList
{
    public static final int MOD_SIDES_NONE        = 0;
    public static final int MOD_SIDES_BID         = 1;
    public static final int MOD_SIDES_ASK         = 2;
    public static final int MOD_SIDES_BID_AND_ASK = 3;

    volatile LinkedList mDeltas;
    MamdaOrderBook      mBook;
    int                 mModSides;
    boolean             mKeepDeltas;

    public MamdaOrderBookBasicDeltaList ()
    {
        mDeltas     = new LinkedList ();
        mBook       = null;
        mModSides   = MOD_SIDES_NONE;
        mKeepDeltas = true;
    }

    /**
     * Clear the delta.
     */
    public void clear ()
    {
        mDeltas.clear ();
        mBook     = null;
        mModSides = MOD_SIDES_NONE;
    }

    /**
     * Set whether to actually keep the basic deltas.  Many
     * applications don't need the basic deltas and will iterate over
     * part or all of the full book (with the deltas already applied).
     * If this is set to true and an attempt is made to iterate over
     * the basic deltas (by calling begin() or end()) then a
     * MamdaOrderBookException will be thrown.
     * @param keep true if to keep the basic deltas.
     */
    public void setKeepBasicDeltas (boolean keep)
    {
        mKeepDeltas = keep;
    }

    /**
     * Get which side(s) of the book have been modified by this
     * complex update.  This information may prevent the need for
     * receivers of complex updates to iterate over one or other side
     * of the book.
     *
     * @return The modified side(s).
     */
    public int getModifiedSides ()
    {
        return mModSides;
    }

    /**
     * Add a basic delta.  This method adds a MamdaOrderBookBasicDelta
     * to the list.
     * @param entry the entry.
     * @param level the level.
     * @param plDeltaSize the delta size.
     * @param plAction the price level action.
     * @param entryAction the entry action.
     */
    public void add (MamdaOrderBookEntry      entry,
                     MamdaOrderBookPriceLevel level,
                     double                   plDeltaSize,
                     char                     plAction,
                     char                     entryAction)
    {
        if (mKeepDeltas)
        {
            MamdaOrderBookBasicDelta basicDelta =
                new MamdaOrderBookBasicDelta ();
            basicDelta.set (entry, level, plDeltaSize, plAction, entryAction);
            mDeltas.add (basicDelta);
        }
        checkSide (level);
    }

    /**
     * Add a basic delta.  This method adds a copy of the
     * MamdaOrderBookBasicDelta to the list.
     * @param delta the delta.
     */
    void add (MamdaOrderBookBasicDelta delta)
    {
        if (mKeepDeltas)
        {
            MamdaOrderBookBasicDelta  basicDelta =
                new MamdaOrderBookBasicDelta (delta);
            mDeltas.add (basicDelta);
        }
        checkSide (delta.getPriceLevel());
    }

    /**
     * Set the MamdaOrderBook object to which this delta belongs.
     *
     * @param book The order book related to this delta.
     */
    void setOrderBook (MamdaOrderBook book)
    {
        mBook = book;
    }

    /**
     * Get the MamdaOrderBook object to which this delta belongs.
     *
     * @return The order book related to this delta.
     */
    public MamdaOrderBook getOrderBook ()
    {
        return mBook;
    }

    /**
     * Return the number of simple deltas in this complex delta.
     * @return the number of deltas.
     */
    public long getSize ()
    {
       return mDeltas.size ();
    }

    /**
     * Fix up price level actions (temporary workaround for problem).
     * This method ensures that all basic deltas for the same price
     * level end up with the same price level action.
     */
    void fixPriceLevelActions ()
    {
        if (mDeltas.size () > 0)
        {
            // First change all actions where first is an add.
            MamdaOrderBookBasicDelta firstSamePlDelta = null;
            ListIterator i = mDeltas.listIterator (0);
            while (i.hasNext ())
            {
                MamdaOrderBookBasicDelta delta =
                    (MamdaOrderBookBasicDelta) i.next ();
                MamdaOrderBookPriceLevel pl = delta.getPriceLevel ();
                if (firstSamePlDelta != null &&
                    (pl == firstSamePlDelta.getPriceLevel ()))
                {
                    if (firstSamePlDelta.getPlDeltaAction () ==
                        MamdaOrderBookPriceLevel.ACTION_ADD)
                    {
                        delta.setPlDeltaAction (
                            MamdaOrderBookPriceLevel.ACTION_ADD);
                    }
                }
                else
                {
                    firstSamePlDelta = delta;
                }
            }
            // Next change all actions where last is aa delete.
            firstSamePlDelta = null;
            i = mDeltas.listIterator (mDeltas.size ());
            while (i.hasPrevious ())
            {
                MamdaOrderBookBasicDelta delta =
                    (MamdaOrderBookBasicDelta) i.previous ();
                MamdaOrderBookPriceLevel pl = delta.getPriceLevel ();
                if (firstSamePlDelta != null &&
                    (pl == firstSamePlDelta.getPriceLevel ()))
                {
                    if (firstSamePlDelta.getPlDeltaAction () ==
                        MamdaOrderBookPriceLevel.ACTION_DELETE)
                    {
                        delta.setPlDeltaAction (
                            MamdaOrderBookPriceLevel.ACTION_DELETE);
                    }
                }
                else
                {
                    firstSamePlDelta = delta;
                }
            }
        }
    }

    /**
     * Dump the complex update to the output stream.
     *
     * @param output The <code>OutputStream</code> to write the update to.
     */
    public void dump (OutputStream output)
    {
        PrintWriter out = new PrintWriter (output, true);
        out.println("ComplexUpdate:\n");
        if (mDeltas.size () > 0)
        {
            Iterator i = mDeltas.iterator ();
            while (i.hasNext ())
            {
                MamdaOrderBookBasicDelta delta =
                    (MamdaOrderBookBasicDelta) i.next ();
                out.print ( "  ");
                delta.dump(output);
            }
        }
    }

    /**
     * Dump the complex update to the output stream.
     *
     */
    public void dump ()
    {
        dump (System.out);
    }

    public Iterator iterator ()
    {
        return mDeltas.iterator ();
    }

    private void checkSide (MamdaOrderBookPriceLevel level)
    {
        if (level == null)
            return;
        if (level.getSide() == MamdaOrderBookPriceLevel.SIDE_BID)
        {
            switch (mModSides)
            {
                case MOD_SIDES_NONE:
                    mModSides = MOD_SIDES_BID;
                    break;
                case MOD_SIDES_ASK:
                    mModSides = MOD_SIDES_BID_AND_ASK;
                    break;
                default:
                    break;
            }
        }
        else
        {
            switch (mModSides)
            {
                case MOD_SIDES_NONE:
                    mModSides = MOD_SIDES_ASK;
                    break;
                case MOD_SIDES_BID:
                    mModSides = MOD_SIDES_BID_AND_ASK;
                    break;
                default:
                    break;
            }
        }
    }
}
