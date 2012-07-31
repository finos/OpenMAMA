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

import com.wombat.mama.*;

import java.util.Iterator;

/**
 * MamdaOrderBookEntry is a class that provides implements order book
 * functionality.
 */

public class MamdaOrderBookEntry
{
    /**
     * An enumeration for book entry actions.  Price level actions
     * differ from entry actions because, for example, a price level
     * message with ACTION_UPDATE may consist of entries with
     * ACTION_ADD, ACTION_UPDATE or ACTION_DELETE.
     */
    public static final char ACTION_ADD     = 'A'; /** A new entry. */
    public static final char ACTION_UPDATE  = 'U'; /** An updated entry. */
    public static final char ACTION_DELETE  = 'D'; /** A deleted entry. */
    public static final char ACTION_UNKNOWN = 'Z'; /** Unknown action (error). */

    private static boolean theStrictChecking = false;

    private String                     mId          = null;
    private String                     mUniqueId    = null;
    private double                     mSize        = 0;
    private MamdaOrderBookPriceLevel   mPriceLevel  = null;
    private MamdaOrderBookEntryManager mManager     = null;
    private MamaSourceDerivative       mSourceDeriv = null;
    private Object                     mClosure     = null;
    private short                      mQuality     = MamaQuality.QUALITY_OK;
    private char                       mAction      = 'A';
    private char                       mReason      = 'Z';
    private MamaDateTime               mTime        = null;
    private int                        mStatus      = 0;


    public MamdaOrderBookEntry ()
    {
    }

    /**
     * Copy an order book entry object which is a shallow copy of
     * the original.
     *
     * @param copy The MamdaOrderBookEntry to copy.
     */    
    public MamdaOrderBookEntry (MamdaOrderBookEntry copy)
    {
        mId          = copy.mId;
        mUniqueId    = copy.mUniqueId;
        mSize        = copy.mSize;
        mTime        = copy.mTime;
        mSourceDeriv = copy.mSourceDeriv;
        mClosure     = copy.mClosure;
        mQuality     = copy.mQuality;
        mAction      = copy.mAction;
        mReason      = copy.mReason;
        mStatus      = copy.mStatus;
    }

    public MamdaOrderBookEntry (String               entryId,
                                double               entrySize,
                                char                 action,
                                MamaDateTime         eventTime,
                                MamaSourceDerivative source)
    {
        mId          = entryId;
        mSize        = entrySize;
        mAction      = action;
        mTime        = eventTime;
        mSourceDeriv = source;
    }

    /**
     * Clear the entry.
     */
    void clear ()
    {
        // First unhook ourselves from our manager using our "unique" ID:
        if (mManager != null)
        {
            mManager.removeEntry(getUniqueId());
        }

        // Now clear everything
        mId          = null;
        mUniqueId    = null;
        mSize        = 0;
        mTime.clear  ();
        mPriceLevel  = null;
        mManager     = null;
        mSourceDeriv = null;
        mClosure     = null;
        mQuality     = MamaQuality.QUALITY_OK;
        mAction      = ACTION_ADD;
        mReason      = MamdaOrderBookTypes.MAMDA_BOOK_REASON_UNKNOWN;
        mStatus      = 0;
    }


    /**
     * Copy an order book entry object which is an exact and deep copy of
     * the original.
     *
     * @param copy The MamdaOrderBookEntry to copy.
     */
    public void copy (MamdaOrderBookEntry copy)
    {
        mId          = copy.mId;
        mUniqueId    = copy.mUniqueId;
        mSize        = copy.mSize;
        mPriceLevel  = null;
        mManager     = null;
        mSourceDeriv = copy.mSourceDeriv;
        mClosure     = copy.mClosure;
        mQuality     = copy.mQuality;
        mAction      = copy.mAction;
        mReason      = copy.mReason;
        mStatus      = copy.mStatus;

        if (null!=copy.mTime)
        {
            if (null == mTime)
                mTime=new MamaDateTime();
            mTime.copy(copy.mTime);
        }
        else
        {
            mTime = null;
        }
    }

    public void setId (String id)
    {
        mId = id;
    }

    public void setSize (double size)
    {
        mSize  = size;
    }

    public void setAction (char action)
    {
        mAction = action;
    }

    public void setReason (char reason)
    {
        mReason = reason;
    }

    public void setTime (MamaDateTime time)
    {
        if (null == mTime)
        {
            mTime = new MamaDateTime();
        }
        mTime.copy (time);
    }

    public void setDetails (MamdaOrderBookEntry copy)
    {
        mSize = copy.mSize;
        setTime (copy.mTime);
    }

    public void setDetails (MamdaBookAtomicLevelEntry  copy)
    {
      mSize = copy.getPriceLevelEntrySize();
      setTime (copy.getPriceLevelEntryTime());
    }
    
    /**
     * If supported, Order book entry ID (order ID, participant ID,
     * etc.)
     * @return the id.
     */
    public String getId ()
    {
        return mId;
    }

    /**
     * If supported, Order book entry unique ID (order ID, participant ID,
     * etc.).  The unique ID should be unique throughout the order book.  If
     * no explicit unique ID has been set, then it assumed that the basic ID
     * is unique and that is returned.
     *
     * @return The unique entry id
     */
    public String getUniqueId ()
    {
        return mUniqueId != null ? mUniqueId : mId;
    }


    /**
     * The size of the order entry.
     * @return the size.
     */
    public double getSize ()
    {
        return mSize;
    }

    /**
     * Whether to ADD, UPDATE or DELETE the entry.
     * 
     * @return char The action.
     */
    public char getAction ()
    {
        return mAction;
    }

    /**
     * The reason for the order entry.
     * @return the reason
     */
    public char getReason ()
    {
        return mReason;
    } 

    /**
     * Time of order book entry update.
     * @return the time.
     */
    public MamaDateTime getTime ()
    {
        return mTime;
    }

    /**
     * Set the status for the entry.
     *
     * @value the new status value.
     */
    public void setStatus (int value)
    {
        mStatus = value;
    }

    /**
     * Return the status for the entry.
     *
     * @return the status.
     */
    public int getStatus ()
    {
        return mStatus;
    }

    /**
     * Get the price for this entry.  This method will throw a
     * MamdaOrderBookInvalidEntry if no MamdaPriceLevel is associated
     * with it because order book price information is only stored in
     * MamdaOrderBookPriceLevel objects.
     *
     * @return  The price for this entry.
     */
    public double getPrice ()
    {
        if (mPriceLevel != null)
            return mPriceLevel.getPrice ().getValue ();
        else
            throw new MamdaOrderBookInvalidEntryException (
                this,
                "MamdaOrderBookEntry.getPrice()");
    }

    /**
     * Get the price for this entry.  This method will throw a
     * MamdaOrderBookInvalidEntry if no MamdaPriceLevel is associated
     * with it because order book price information is only stored in
     * MamdaOrderBookPriceLevel objects.
     *
     * @return  The side for this entry.
     */
    public char getSide ()
    {
        if (mPriceLevel != null)
            return mPriceLevel.getSide ();
        else
            throw new MamdaOrderBookInvalidEntryException (
                this,
                "MamdaOrderBookEntry.getSide()");
    }
    
    /**
     * Get the position in the order book for this entry.  If maxPos
     * is not zero, then the method will return a result no greater
     * than maxPos.  This is to prevent searching the entire book when
     * only a limited search is necessary.  Note: the logic used in
     * the positional search is to use the number of entries that
     * MamdaOrderBookPriceLevel::getNumEntries() returns for price
     * levels above the entry's price level.  -1 is return if the
     * entry is in the book but not currently "visible" (i.e., it is
     * being omitted because the MAMA source is turned off).  A
     * MamdaOrderBookInvalidEntry is thrown if the entry is not found
     * in the book.
     *
     * @param maxPos  The maximum position to return;
     *
     * @return  The position of this entry in the order book.
     */
    public int  getPosition (int  maxPos)
    {
        if (mPriceLevel == null)
            throw new MamdaOrderBookInvalidEntryException (this,
                                "MamdaOrderBookEntry.getPosition() (no level)");
        MamdaOrderBook  book = getOrderBook();
        assert (book != null);
        int  pos = 0;

        if (MamdaOrderBookPriceLevel.SIDE_BID == mPriceLevel.getSide())
        {
            Iterator i = book.bidIterator ();
            while (i.hasNext ())
            {
                MamdaOrderBookPriceLevel level =
                    (MamdaOrderBookPriceLevel) i.next ();
                if (level != mPriceLevel)
                {
                    // Not the level containing this entry
                    if (level.getPrice().getValue () <
                        mPriceLevel.getPrice().getValue ())
                    {
                        throw new MamdaOrderBookInvalidEntryException (this,
                                 "MamdaOrderBookEntry.getPosition() (bid)");
                    }
                    pos += level.getNumEntries();
                }
                else
                {
                    // Count the entries before this entry within the level
                    Iterator plIter = level.entryIterator ();
                    while (plIter.hasNext ())
                    {
                        if (plIter.next () == this)
                            return pos;
                        pos++;
                    }
                }
                if ((maxPos > 0) && (pos > maxPos))
                    return maxPos;
            }
        }
        else
        {
            Iterator i = book.askIterator ();
            while (i.hasNext ())
            {
                MamdaOrderBookPriceLevel level =
                    (MamdaOrderBookPriceLevel) i.next ();
                if (level != mPriceLevel)
                {
                    // Not the level containing this entry
                    if (level.getPrice().getValue () >
                        mPriceLevel.getPrice().getValue ())
                    {
                        throw new MamdaOrderBookInvalidEntryException (this,
                                 "MamdaOrderBookEntry.getPosition() (ask)");
                    }
                    pos += level.getNumEntries();
                }
                else
                {
                    // Count the entries before this entry within the level
                    Iterator plIter = level.entryIterator ();
                    while (plIter.hasNext ())
                    {
                        if (plIter.next () == this)
                            return pos;
                        pos++;
                    }
                }
                if ((maxPos > 0) && (pos > maxPos))
                    return maxPos;
            }
        }
        throw new MamdaOrderBookInvalidEntryException (this,
            "MamdaOrderBookEntry.getPosition() (not found)");
    }


    public int hashCode ()
    {
        /* From Effective Java */
        int result = 17 + mId.hashCode ();
        result = 37*result + (int)mSize;
        result = 37*result + mAction;
        result = 37*result + mAction;
        return 37*result + mTime.hashCode ();
    }

    public boolean equals (Object obj)
    {
        if (obj instanceof MamdaOrderBookEntry)
        {
            MamdaOrderBookEntry entry = (MamdaOrderBookEntry) obj;
            return equalId (entry.mId) && (mSize == entry.mSize) &&
                   (mAction == entry.mAction) &&
                   (mTime.equals (entry.mTime));
        }
        return false;
    }

    /**
     * Set the MamdaOrderBookPriceLevel object to which this entry belongs.
     * This method is invoked automatically internally, by the MAMDA API, when
     * an entry is added to a price level.
     *
     * @param priceLevel  The price level to be associated with.
     */
    public void setPriceLevel (MamdaOrderBookPriceLevel priceLevel)
    {
        mPriceLevel = priceLevel;
    }

    /**
     * Get the MamdaOrderBookPriceLevel object to which this entry
     * belongs.
     *
     * @return  The price level currently associated with this entry.
     */
    public MamdaOrderBookPriceLevel getPriceLevel ()
    {
        return mPriceLevel;
    }

    /**
     * Get the order book for this entry, if possible.  This can only
     * be done if the entry is part of a price level and the price
     * level is part of an order book.  null is returned if no order
     * book can be found.
     *
     * @return  The order book or null.
     */
    public MamdaOrderBook getOrderBook()
    {
        return mPriceLevel == null ? null : mPriceLevel.getOrderBook ();
    }

    /**
     * Set the MamdaOrderBookEntryManager object to which this entry belongs.
     * This method is invoked automatically internally, by the MAMDA API, when
     * an entry is added to an entry manager.
     *
     * @param manager  The manager.
     */
    public void setManager (MamdaOrderBookEntryManager  manager)
    {
        mManager = manager;
    }

    /**
     * Get the MamdaOrderBookEntryManager object to which this entry
     * belongs.
     *
     * @return  The manager currently associated with this entry.
     */
    public MamdaOrderBookEntryManager getManager()
    {
        return mManager;
    }

    /**
     * Get the symbol for this entry, if possible.  This can only be
     * done if the entry is part of a price level and the price level
     * is part of an order book.  NULL is returned if no symbol can be
     * found.
     *
     * @return  The symbol or NULL.
     */
    public String getSymbol ()
    {
        return mPriceLevel.getSymbol ();
    }

    /**
     * Set the MamaSourceDerivative for this book entry.  The source
     * derivative is used to help determine what the quality of order book
     * entry is and to efficiently identify all of the entries for a given
     * source (e.g., for aggregated order books).
     *
     * @param  source The MAMA source derivative.
     */
    public void setSourceDerivative (MamaSourceDerivative    source)
    {
        mSourceDeriv = source;
    }

    /**
     * Return the MamaSourceDerivative for this book entry.
     * @return the MamaSourceDerivative
     */
    public MamaSourceDerivative  getSourceDerivative ()
    {
        return mSourceDeriv;
    }

    /**
     * Return the MamaSource for this book entry.
     * @return the source.
     */
    public MamaSource getSource ()
    {
        return mSourceDeriv != null ? mSourceDeriv.getBaseSource() : null;
    }

    /**
     * Get whether this order book wants to check the source state.
     *
     * @return  Whether to check source state.
     */
    public boolean getCheckSourceState ()
    {
        MamdaOrderBook  book = getOrderBook();
        return book != null && book.getCheckSourceState ();
    }

    /**
     * Set the entry-level quality factor.  This level, if not
     * MAMA_QUALITY_OK, overrides the source-level level.
     *
     * @param quality  The new entry-level MAMA quality level.
     */
    public void setQuality (short  quality)
    {
        mQuality = quality;
    }

    /**
     * Get the entry-level quality factor.  If the entry-level quality is
     * MAMA_QUALITY_OK, then this method returns the source-level quality.
     *
     * @return  The current MAMA quality level.
     */
    public short getQuality ()
    {
        return mQuality;
    }

    public boolean isVisible ()
    {
        return getSourceState() == MamaSourceState.OK;
    }

    /**
     * Set the order book entry closure handle.
     *
     * @param closure The closure.
     */
    public void setClosure (Object  closure)
    {
        mClosure = closure;
    }

    /**
     * Get the order book entry closure handle.
     *
     * @return The entry closure.
     */
    public Object getClosure ()
    {
        return mClosure;
    }


    /**
     * Get whether this entry is "visible" in this book.  Visibilty is
     * controlled by the status of the MamaSourceDerivative for the entry.
     *
     * @return  Whether the entry is visible.
     */
    private MamaSourceState getSourceState ()
    {
        return mSourceDeriv != null ? mSourceDeriv.getState()
                                     : MamaSourceState.OK;
    }

    /**
     * Order book entry equality verification.  A
     * MamdaOrderBookException is thrown if the entries within a price
     * level are not equal, along with the reason for the inequality.
     * \throw<MamdaOrderBookException> \n
     */
    public void assertEqual (MamdaOrderBookEntry  rhs)
    {
        if (!(mId.equals (rhs.mId)))
        {
            throw new MamdaOrderBookException("entry IDs not equal");
        }
        if (mSize != rhs.mSize)
            throw new MamdaOrderBookException("entry size not equal");
        if (mAction != rhs.mAction)
            throw new MamdaOrderBookException("entry action not equal");
        if (mTime != null)
        {
            if ((rhs.mTime == null) || !mTime.equals (rhs.mTime))
                throw new MamdaOrderBookException("entry time not equal");
        } 
        else
        {
            if (rhs.mTime != null)
                throw new MamdaOrderBookException("entry time not equal");
        }            
    }

    /**
     * Whether the id of the entry is equal to the id passed to the method.
     *
     * @param id The id to compare to the order entry id.
     *
     * @return boolean Whether the id is the same as the order id.
     */
    public boolean equalId (String id)
    {
        if (mId != null)
            return mId.equals(id);
        else
            return id == null;
    }

    /**
     * Enforce strict checking of order book modifications (at the
     * expense of some performance).  This setting is automatically
     * updated by MamdaOrderBook::setStrictChecking().
     */
    public static void setStrictChecking (boolean strict)
    {
        theStrictChecking = strict;
    }

    public void setUniqueId (String uniqueId)
    {
        mUniqueId = uniqueId;
    }
}
