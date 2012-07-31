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

import java.util.Vector;
import java.util.Iterator;
import java.util.ListIterator;

import com.wombat.mama.*;

public class MamdaOrderBookPriceLevel
{
    public static final char  ACTION_ADD              = 'A'; /** A new price level. */
    public static final char  ACTION_UPDATE           = 'U'; /** Updated price level. */
    public static final char  ACTION_DELETE           = 'D'; /** Deleted price level. */
    public static final char  ACTION_UNKNOWN          = 'Z'; /** Unknown action (error). */

    public static final char  SIDE_BID                = 'B'; /** Bid (buy) side. */
    public static final char  SIDE_ASK                = 'A'; /** Ask (sell) side. */
    public static final char  SIDE_UNKNOWN            = 'Z'; /** Unknown side (error). */
    
    public static final char  LEVEL_LIMIT             = 'L'; /** LIMIT order */
    public static final char  LEVEL_MARKET            = 'M'; /** MARKET order */
    public static final char  LEVEL_UNKNOWN           = 'U'; /** Unknown order */

    private static boolean    theStrictChecking       = false;

    private MamaPrice         mPrice                  = null;
    private MamaPrice         mAtomicPrice            = null;
    private double            mSize                   = 0;
    private double            mSizeChange             = 0;
    private double            mNumEntries             = 0;
    private char              mSide                   = SIDE_BID;
    private char              mAction                 = ACTION_ADD;
    private MamaDateTime      mTime                   = null;
    private MamaDateTime      mAtomicLevelEntryTime   = null;
    private MamaDateTime      mAtomicLevelTime        = null;
    private Vector            mEntries                = new Vector ();
    private Vector            mLevelEntries           = new Vector ();
    private MamdaOrderBook    mBook                   = null;
    private int               mNumEntriesTotal        = 0;
    private char              mOrderType              = LEVEL_LIMIT;

    /**
     * Default constructor.
     */
    public MamdaOrderBookPriceLevel ()
    {
    }

    /**
     * Construct a price level object which is a shallow copy of
     * the original.
     *
     * @param copy The MamdaOrderBookPriceLevel to copy.
     */
    public MamdaOrderBookPriceLevel (MamdaOrderBookPriceLevel copy)
    {
        mPrice           = copy.mPrice;
        mSize            = copy.mSize;
        mSizeChange      = copy.mSizeChange;
        mNumEntries      = copy.mNumEntries;
        mSide            = copy.mSide;
        mAction          = copy.mAction;
        mTime            = copy.mTime;
        mBook            = copy.mBook;
        mOrderType       = copy.mOrderType;
        mNumEntriesTotal = copy.mNumEntriesTotal;

        for (int i = 0; i < copy.mEntries.size(); i++)
        {
            MamdaOrderBookEntry entry = new MamdaOrderBookEntry(
                    (MamdaOrderBookEntry)copy.mEntries.get(i));
            mEntries.addElement(entry);
        }
    }

    public MamdaOrderBookPriceLevel (double price, char side)
    {
        mPrice = new MamaPrice (price);
        mSide  = side;
    }
 
    /**
     * Clear the pricelevel
     **/
    public void clear ()
    {
        if(mPrice != null)
        {
            mPrice.clear();
        }

        mSize            = 0.0;
        mSizeChange      = 0.0;
        mNumEntries      = 0;
        mNumEntriesTotal = 0;
        mSide            = MamdaOrderBookPriceLevel.SIDE_BID;
        mAction          = MamdaOrderBookPriceLevel.ACTION_ADD;
        mOrderType       = MamdaOrderBookPriceLevel.LEVEL_LIMIT;

        if(mTime != null)
        {
            mTime.clear();
        }

        if (this.empty())
        {
            mEntries.clear();
        }

        mBook = null;
    }
    
    /**
     * Copy a price level object which is an exact and deep copy of
     * the original.
     *
     * @param copy The MamdaOrderBookPriceLevel to copy.
     */    
    public void copy (MamdaOrderBookPriceLevel copy)
    {
        if (null != copy.mPrice)
        {
            if (null == mPrice)
                mPrice = new MamaPrice();
            mPrice.copy(copy.mPrice);
        }
        else
        {
            mPrice = null;
        }

        mSize            = copy.mSize;
        mSizeChange      = copy.mSizeChange;
        mNumEntries      = copy.mNumEntries;
        mNumEntriesTotal = copy.mNumEntriesTotal;
        mSide            = copy.mSide;
        mAction          = copy.mAction;
        mBook            = null; /* consistent with C++ */
        mOrderType       = copy.mOrderType;
        
        if (null != copy.mTime)
        {
            if (null == mTime)
                mTime = new MamaDateTime();
            mTime.copy(copy.mTime);
        }
        else
        {
            mTime = null;
        }

        for (int i = 0; i < copy.mEntries.size(); i++)
        {
            MamdaOrderBookEntry entry = new MamdaOrderBookEntry();
            entry.copy ((MamdaOrderBookEntry)copy.mEntries.get(i));
            mEntries.addElement(entry);
        }
    }
    
    /**
     * Copy an atomic price level object which is an exact and deep copy of
     * the original.
     *
     * @param copy The MamdaOrderBookPriceLevel to copy.
     */    
    public void copy (MamdaBookAtomicLevel copy)
    {
        if (null != copy.getPriceLevelMamaPrice())
        {
            if (null == mAtomicPrice)
            {
                mAtomicPrice = new MamaPrice();
            }
            mAtomicPrice.copy(copy.getPriceLevelMamaPrice());
        }
        else
        {
            mAtomicPrice = null;
        }

        this.mSizeChange = copy.getPriceLevelSizeChange();
        this.mSize       = copy.getPriceLevelSize();       
        this.mNumEntries = copy.getPriceLevelNumEntries();
        this.mSide       = copy.getPriceLevelSide();
        this.mAction     = copy.getPriceLevelAction();
           
        if (null != copy.getPriceLevelTime())
        {
            if (null == mAtomicLevelTime)
            {
                mAtomicLevelTime = new MamaDateTime();
            }
            mAtomicLevelTime.copy(copy.getPriceLevelTime());
        }
        else
        {
            mAtomicLevelTime = null;
        }
    }
     
    /**
     * Copy a price level object which is an exact and deep copy of
     * the original.
     *
     * @param copy The MamdaOrderBookPriceLevel to copy.
     */    
    public void copy (MamdaBookAtomicLevelEntry copy)
    {
        this.mSize                     = copy.getPriceLevelSize();
        this.mNumEntries               = copy.getPriceLevelNumEntries();
        this.mAtomicLevelEntryTime     = copy.getPriceLevelEntryTime();
        this.mPrice                    = copy.getPriceLevelMamaPrice();
        this.mSide                     = copy.getPriceLevelSide();
        this.mAction                   = copy.getPriceLevelAction();
    }
      
    
    public void setPrice (MamaPrice price)
    {
        if (mPrice == null)
        {
            mPrice = new MamaPrice();
        }
        mPrice.copy(price);
    }

    public void setPrice(double price)
    {
        if (mPrice == null)
        {
            mPrice = new MamaPrice();
        }
        mPrice.setValue(price);  
    }

    public void setSize (double size)
    {
        mSize  = size;
    }

    public void setSizeChange (double size)
    {
        mSizeChange  = size;
    }

    public void setNumEntries (double numEntries)
    {
        mNumEntries = numEntries;
    }

    public void setSide (char side)
    {
        mSide  = side;
    }

    public void setAction (char action)
    {
        mAction = action;
    }

    public void setTime (MamaDateTime time)
    {
        if (mTime == null)
        {
            mTime = new MamaDateTime();
            mTime.copy(time);
        }
        mTime.copy(time);
    }

    public void setOrderType (char orderType)
    {
        mOrderType = orderType;
    }

    public char getOrderType ()
    {
        return mOrderType;
    }

    /**
     * Return whether there are no entries for this level.
     *
     * @return Whether there are no entries for this level.
     */
    public boolean empty ()
    {
        return  mEntries == null || mEntries.size() == 0;
    }

    /**
     * Take the details from <code>level<code> and apply them to this level.
     * This does not update entries within the level, only information
     * pertaining to the level itself.
     * It is intended that this method should not be used externally to the
     * API.
     *
     * @param level The price level object from which the details are being
     * obtained.
     */
    public void setDetails (MamdaOrderBookPriceLevel level)
    {
        this.mSizeChange      = level.mSize - this.mSize;
        this.mSize            = level.mSize;
        this.mNumEntries      = level.mNumEntries;
        this.mNumEntriesTotal = level.mNumEntriesTotal;
        this.mTime            = level.mTime;
        this.mOrderType       = level.mOrderType;
    }

    /**
     * Take the details from <code>atomic level<code> and apply them to this level.
     * This does not update entries within the level, only information
     * pertaining to the level itself.
     * It is intended that this method should not be used externally to the
     * API.
     *
     * @param level The atomic price level object from which the details are being
     * obtained.
     */
    public void setDetails (MamdaBookAtomicLevel level)
    {
        this.mSizeChange      = level.getPriceLevelSizeChange();
        this.mSize            = level.getPriceLevelSize();
        this.mNumEntries      = level.getPriceLevelNumEntries();
        this.mAtomicLevelTime = level.getPriceLevelTime();
    }
     
    /**
     * Take the details from <code>atomic levelEntry<code> and apply them to this levelEntry.
     * This update entries within the level.
     * It is intended that this method should not be used externally to the
     * API.
     *
     * @param levelEntry The atomic price levelEntry object from which the details are being
     * obtained.
     */
    public void setDetails (MamdaBookAtomicLevelEntry levelEntry)
    {
        this.mSize                   = levelEntry.getPriceLevelEntrySize();
        this.mAtomicLevelEntryTime   = levelEntry.getPriceLevelEntryTime();
    }
    
    /**
     * Add a new order book entry to the price level.
     *
     * @param entry The new entry to be added to the level.
     * 
     * @see MamdaOrderBookEntry
     */
    public void addEntry (MamdaOrderBookEntry  entry)
    {
        if (theStrictChecking)
        {
            checkNotExist(entry);
        }
        if (mEntries == null)
            mEntries = new Vector();
        boolean checkState = (mBook != null ) ? mBook.getCheckSourceState() : false;
        if (!checkState || entry.isVisible())
        {
            mNumEntries++;
            mSize += entry.getSize();           
        }        
        mNumEntriesTotal++;
        entry.setPriceLevel (this);
        mEntries.add (entry);
        if (mBook != null)
        {
            if (mBook.getGenerateDeltaMsgs())
            {
                //Need to set correct action based on numEntries in level
                char plAction;
                if (mNumEntriesTotal > 1) 
                {
                    plAction = MamdaOrderBookPriceLevel.ACTION_UPDATE;
                }
                else
                {
                    plAction = MamdaOrderBookPriceLevel.ACTION_ADD;
                }
                mBook.addDelta (entry, entry.getPriceLevel(), 
                                entry.getPriceLevel().getSizeChange(),
                                plAction,
                                MamdaOrderBookEntry.ACTION_ADD);
            }
        }
    }

    /**
     * Update the details of an existing entry in the level.
     * 
     * @param entry An instance of <code>MamdaOrderBookEntry</code> with the
     * new details for the entry in the level.
     *
     * @see MamdaOrderBookEntry
     */
    public void updateEntry (MamdaOrderBookEntry  entry)
    {
        for (int i = 0; i < mEntries.size(); i++)
        {
            MamdaOrderBookEntry existingEntry =
                (MamdaOrderBookEntry)mEntries.get(i);
            if (existingEntry.equalId(entry.getId()))
            {
                // found it
                existingEntry.setDetails (entry);
                if (mBook != null )
                {
                    if (mBook.getGenerateDeltaMsgs())
                    {
                        mBook.addDelta (entry, entry.getPriceLevel(), 
                                        entry.getPriceLevel().getSizeChange(),
                                        MamdaOrderBookPriceLevel.ACTION_UPDATE,
                                        MamdaOrderBookEntry.ACTION_UPDATE);
                    }           
                }
                return;
            }
        }
        if (theStrictChecking)
        {
            throw new MamdaOrderBookException (
                "attempted to update a non-existent entry: " + entry.getId());
        }
    }

    /**
     * Update the details of an existing entry in the level, where the update details
     * are provied by an atomic levelEntry.
     * @param levelEntry An instance of <code>MamdaBookAtomicLevelEntry</code> with the
     * new details for the entry in the level.
     *
     * @see MamdaOrderBookEntry
     */
    public void updateEntry (MamdaBookAtomicLevelEntry  levelEntry)
    {
        for (int i = 0; i < mEntries.size(); i++)
        {
            MamdaOrderBookEntry existingEntry =(MamdaOrderBookEntry)mEntries.get(i);
            if (existingEntry.equalId(levelEntry.getPriceLevelEntryId()))
            {
                    // found it
                existingEntry.setDetails(levelEntry);
                return;
            }
        }
        if (theStrictChecking)
        {
            throw new MamdaOrderBookException (
                "attempted to update a non-existent entry: " + 
                levelEntry.getPriceLevelEntryId());
        }
    }

    /**
     * Remove an order book entry from the price level.
     * 
     * @param entry The entry which is to be removed from the price level.
     * 
     * @see MamdaOrderBookEntry
     */
    public void removeEntry (MamdaOrderBookEntry  entry)
    {
        for (int i = 0; i < mEntries.size(); i++)
        {
            MamdaOrderBookEntry existingEntry =
                (MamdaOrderBookEntry)mEntries.get(i);
            if (existingEntry.equalId(entry.getId()))
            {
                // found it
                mEntries.removeElementAt(i);
                boolean checkState = mBook.getCheckSourceState();                       
                if (!checkState || entry.isVisible())
                {
                    mSize -= entry.getSize();     
                    mNumEntries--;
                }
                mNumEntriesTotal--;

                if (mBook.getGenerateDeltaMsgs())
                {
                    char plAction;
                    if (0 == entry.getPriceLevel().getNumEntriesTotal()) 
                    {
                        plAction = MamdaOrderBookPriceLevel.ACTION_DELETE;
                    }
                    else
                    {
                        plAction = MamdaOrderBookPriceLevel.ACTION_UPDATE;
                    }
                    mBook.addDelta (entry, entry.getPriceLevel(), 
                                    entry.getPriceLevel().getSizeChange(), 
                                    plAction, 
                                    MamdaOrderBookEntry.ACTION_DELETE);
                } 
                return;
            }
        }
        if (theStrictChecking)
        {
            throw new MamdaOrderBookException (
                "attempted to delete a non-existent entry: " + entry.getId());
        }
    }

    /**
     * Remove an order book entry from the price level, where the delete details
     * are provied by an atomic levelEntry.
     * 
     * @param levelEntry The entry which is to be removed from the price level.
     * 
     * @see MamdaOrderBookEntry
     */
    public void removeEntry (MamdaBookAtomicLevelEntry  levelEntry)
    {
        for (int i = 0; i < mEntries.size(); i++)
        {
            MamdaOrderBookEntry existingEntry =
                (MamdaOrderBookEntry)mEntries.get(i);
            if (existingEntry.equalId(levelEntry.getPriceLevelEntryId()))
            {
                // found it
                mEntries.removeElementAt(i);
                boolean checkState = mBook.getCheckSourceState();
                if (!checkState || levelEntry.isVisible())
                {
                    mSize -= levelEntry.getPriceLevelEntrySize();
                    mNumEntries--;
                }
                mNumEntriesTotal--;
                return;
            }
        }
        if (theStrictChecking)
        {
            throw new MamdaOrderBookException (
                "attempted to delete a non-existent entry: " + 
                levelEntry.getPriceLevelEntryId());
        }
    }
      
    /**
     * Returns a <code>java.util.Iterator</code> for all entries within this
     * level.
     * Price Level entries are represented by the
     * <code>MamdaOrderBookEntry</code> class.
     *
     * @return Iterator The iterator for the price level entries
     */
    public Iterator entryIterator ()
    {
        return mEntries.iterator();
    }

    /**
     * If the provided order book entry exists in the price level a
     * <code>MamdaOrderBookException</code> exception is thrown. Otherwise the
     * method simply returns.
     * 
     * @param entry The entry whose presence in the level is being determined.
     * 
     * @throws MamdaOrderBookException If the entry is found in the price
     * level.
     */
    public void checkNotExist (MamdaOrderBookEntry  entry)
    {
        if (mEntries.size () == 0)
            return;

        for (int i = 0; i < mEntries.size(); i++)
        {
            MamdaOrderBookEntry existingEntry =
                (MamdaOrderBookEntry)mEntries.get(i);
            if (existingEntry.equalId (entry.getId()))
            {
                // found it
                throw new MamdaOrderBookException (
                    "attempted to add am existent entry: " + entry.getId());
            }
        }
    }

    /**
     * If the provided levelEntry entry exists in the price level, 
     * as a OrderBookEntry, a
     * <code>MamdaOrderBookException</code> exception is thrown. Otherwise the
     * method simply returns.
     * 
     * @param entry The entry whose presence in the level is being determined.
     * 
     * @throws MamdaOrderBookException If the entry is found in the price
     * level.
     */
     public void checkNotExist (MamdaBookAtomicLevelEntry  levelEntry)
     {
        if (mEntries.size () == 0)
            return;

        for (int i = 0; i < mEntries.size(); i++)
        {
            MamdaOrderBookEntry existingEntry =
                (MamdaOrderBookEntry)mEntries.get(i);
            if (existingEntry.equalId(levelEntry.getPriceLevelEntryId()))
            {
                // found it
                throw new MamdaOrderBookException (
                    "attempted to add am existent entry: " + 
                    levelEntry.getPriceLevelEntryId());
            }
        }
    }
     
    /**
     * Mark everything in this price level as deleted, including
     * entries.
     */
    public void markAllDeleted ()
    {
        setSizeChange (-getSize());
        setSize       (0);
        setNumEntries (0);
        setAction     (MamdaOrderBookPriceLevel.ACTION_DELETE);

        for (int i = 0; i < mEntries.size(); i++)
        {
            MamdaOrderBookEntry entry = (MamdaOrderBookEntry)mEntries.get(i);
            entry.setSize (0);
            entry.setAction (MamdaOrderBookEntry.ACTION_DELETE);
        }
    }

    public void setAsDifference (MamdaOrderBookPriceLevel  lhs,
                                 MamdaOrderBookPriceLevel  rhs)
    {
        int lhsBookSize = lhs.mEntries.size();
        int rhsBookSize = rhs.mEntries.size();
        int lhsIndex    = 0;
        int rhsIndex    = 0;

        while ((lhsIndex < lhsBookSize) && (rhsIndex < rhsBookSize))
        {
            String  lhsId                = null;
            String  rhsId                = null;
            double lhsSize               = 0;
            double rhsSize               = 0;
            MamdaOrderBookEntry lhsEntry;
            MamdaOrderBookEntry rhsEntry = null;

            if (lhsIndex < lhsBookSize)
            {
                lhsEntry = (MamdaOrderBookEntry)lhs.mEntries.get(lhsIndex);
                lhsId    = lhsEntry.getId();
                lhsSize  = lhsEntry.getSize();
            }

            if (rhsIndex < rhsBookSize)
            {
                rhsEntry = (MamdaOrderBookEntry)rhs.mEntries.get(rhsIndex);
                rhsId    = rhsEntry.getId();
                rhsSize  = rhsEntry.getSize();
            }

            if ((lhsId != null) && (rhsId != null))
            {
                if (lhsId.equals(rhsId))
                {
                    // Same ID, maybe different size.
                    if (lhsSize != rhsSize)
                    {
                        MamdaOrderBookEntry  updateEntry =
                            new MamdaOrderBookEntry (rhsEntry);
                        updateEntry.setAction(MamdaOrderBookEntry.ACTION_UPDATE);
                        addEntry (updateEntry);
                    }
                    lhsIndex++;
                    rhsIndex++;
                    continue;
                }
                else
                {
                    // Different ID (either something exists on the LHS
                    // and not on RHS or vice versa).
                    int rhsFound = findEntryAfter (rhs.mEntries, rhsIndex,
                                                   lhsId);
                    if (rhsFound != rhsSize)
                    {
                        // The ID from the LHS was found on the RHS, so
                        // there must have been additional entries on the
                        // RHS, which we now need to add.
                        do
                        {
                            addEntry ((MamdaOrderBookEntry)rhs.mEntries.get(rhsIndex));
                            rhsIndex++;
                        }
                        while (rhsIndex < rhsFound);
                    }
                    else
                    {
                        // The ID from the LHS was not present on the RHS,
                        // so add the LHS entry.  Note: it would probably
                        // be faster to iterate over the LHS side rather
                        // than begin the loop again.
                        addEntry ((MamdaOrderBookEntry)lhs.mEntries.get(lhsIndex));
                        lhsIndex++;
                    }
                }
            }
        }

        mPrice          = rhs.getPrice();
        setSizeChange   (rhs.getSize() - lhs.getSize());
        setSize         (rhs.getSize());
        setNumEntries   (rhs.getNumEntries());
        setAction       (MamdaOrderBookPriceLevel.ACTION_UPDATE);
        mTime           = rhs.getTime();
        setSide         (rhs.getSide());
    }

    /**
     * Return the price for this level.
     * @return The price for this level.
     */
    public MamaPrice getPrice ()
    {
        return mPrice;
    }

    /**
     * Return the total size (across all entries) for this level.
     * @return The total size for this level.
     */
    public double getSize ()
    {
        return mSize;
    }

    /**
     * Return the size change for this (presumably delta) level.  This
     * attribute is only of interest for delta order books.  For full
     * order books, this field will be equal to the size of the price
     * level.
     *
     * @return The changed size for this level.
     */
    public double getSizeChange ()
    {
        return mSizeChange;
    }

    /**
     * Return the actual number of entries for this level.  The actual
     * number of entries may not equate to the number of entries that
     * can be iterated over if:
     * (a) the feed does not provide the actual entries, or
     * (b) the price level is just a delta.
     *
     * @return The actual number of entries for this level.
     */
    public double getNumEntries ()
    {
        return mNumEntries;
    }

    public int getNumEntriesTotal()
    {  
        return mNumEntriesTotal;
    }

    /**
     * Return the side (bid/ask) of the book for this level.
     * @return The side of the book for this level.
     */
    public char getSide ()
    {
        return mSide;
    }

    /**
     * Return the action for this price level.  All price levels for a
     * full book are marked with ACTION_ADD.
     *
     * @return The action for this level.
     */
    public char getAction ()
    {
        return mAction;
    }

    /**
     * Return the time stamp for when the price level was last updated.
     * @return The time stamp for when the price level was last updated.
     */
    public MamaDateTime getTime ()
    {
        return mTime;
    }

    /**
     * Order book price level equality verification.  A
     * MamdaOrderBookException is thrown if the price levels are
     * not equal, along with the reason for the inequality.
     *
     * @throws MamdaOrderBookException
     */
    public void assertEqual (MamdaOrderBookPriceLevel  rhs)
    {
        if (!mPrice.equals(rhs.mPrice))
            throwError ("price not equal");
        if (mSize != rhs.mSize)
            throwError ("size not equal");
        if (mNumEntries != rhs.mNumEntries)
            throwError ("number of entries not equal");
        if (mNumEntriesTotal != rhs.mNumEntriesTotal)
            throwError ("number of total entries not equal");
        if (mSide != rhs.mSide)
            throwError ("side not equal");
        if (mAction != rhs.mAction)
            throwError ("action not equal");
        if (mOrderType != rhs.mOrderType)
          throwError ("orderType not equal");
        if (mTime != null)
        {
            if ((rhs.mTime == null) || !mTime.equals (rhs.mTime))
                throwError ("time not equal");
        } 
        else
        {
            if (rhs.mTime != null)
                throwError ("time not equal");
        }
          
        if (((mEntries.size() != 0) && (rhs.mEntries.size() == 0)) ||
            ((mEntries.size() == 0) && (rhs.mEntries.size() != 0)))
            throwError ("entries mismatch");
        if ((mEntries.size() != 0) && (rhs.mEntries.size() != 0))
        {
            if (mEntries.size() != rhs.mEntries.size())
            {
                throwError ("entries size mismatch (" + mEntries.size() +
                            "!=" + rhs.mEntries.size());
            }
            for (int i = 0; i < mEntries.size(); i++)
            {
                MamdaOrderBookEntry  lhsEntry =
                    (MamdaOrderBookEntry)mEntries.get(i);
                MamdaOrderBookEntry  rhsEntry =
                    (MamdaOrderBookEntry)rhs.mEntries.get(i);
                lhsEntry.assertEqual (rhsEntry);
            }
        }
    }

    private int findEntryAfter (Vector  entries,
                                int     start,
                                String  id)
    {
        int i;
        int size = entries.size();
        for (i = start; i < size; i++)
        {
            MamdaOrderBookEntry entry = (MamdaOrderBookEntry)entries.get(i);
            if (id.equals(entry.getId()))
            {
                return i;
            }
        }
        return i;
    }

    private void throwError (String context)
    {
        throw new MamdaOrderBookException (context + "(price=" + mPrice +
                                           ", size=" + mSide);
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

    protected MamdaOrderBook getOrderBook ()
    {
        return mBook;
    }

    protected void setOrderBook (MamdaOrderBook book)
    {
        mBook = book;
    }

    public String getSymbol ()
    {
        return mBook.getSymbol ();
    }

    public void removeEntryById (MamdaOrderBookEntry entry)
    {
        ListIterator i = mEntries.listIterator ();
        while (i.hasNext ())
        {
            MamdaOrderBookEntry existingEntry = (MamdaOrderBookEntry) i.next ();
            if (existingEntry.equalId(entry.getId()))
            {
                // found it
                i.remove ();
                boolean checkState = mBook !=null ? mBook.getCheckSourceState() : false;
                if (!checkState || entry.isVisible())
                {
                    mSize -= entry.getSize();                       
                    mNumEntries--;
                }
                mNumEntriesTotal--;

                if (mBook.getGenerateDeltaMsgs())
                {
                    char plAction;
                    if (0 == entry.getPriceLevel().getNumEntriesTotal()) 
                    {
                        plAction = MamdaOrderBookPriceLevel.ACTION_DELETE;
                    }
                    else
                    {
                        plAction = MamdaOrderBookPriceLevel.ACTION_UPDATE;
                    }
                        mBook.addDelta (entry, entry.getPriceLevel(), 
                                        entry.getPriceLevel().getSizeChange(), 
                                        plAction, 
                                        MamdaOrderBookEntry.ACTION_DELETE);
                } 
                return;
            }
        }
        if (theStrictChecking)
        {
            throw new MamdaOrderBookException(
                "attempted to delete a non-existent entry: " +
                entry.getId());
        }
    }
    
    public void removeEntryById (MamdaBookAtomicLevelEntry levelEntry)
    {
        ListIterator i = mEntries.listIterator ();
        while (i.hasNext ())
        {
            MamdaOrderBookEntry existingEntry = (MamdaOrderBookEntry) i.next ();
            if (existingEntry.equalId(levelEntry.getPriceLevelEntryId()))
            {
                // found it
                i.remove ();
                boolean checkState = mBook != null ? mBook.getCheckSourceState() : false;
                if (!checkState || levelEntry.isVisible())
                {
                    mSize -= levelEntry.getPriceLevelEntrySize();
                    mNumEntries--;
                }
                mNumEntriesTotal--;

                return;
            }
        }
        if (theStrictChecking)
        {
            throw new MamdaOrderBookException(
                "attempted to delete a non-existent entry: " +
                levelEntry.getPriceLevelEntryId());
        }
    }

    public void addEntriesFromLevel (MamdaOrderBookPriceLevel     bookLevel,
                                     MamdaOrderBookEntryFilter    filter,
                                     MamdaOrderBookBasicDeltaList delta)
    {
        if (mEntries.isEmpty ())
            return;

        Iterator i = mEntries.iterator ();
        while (i.hasNext ())
        {
            MamdaOrderBookEntry entry     = (MamdaOrderBookEntry) i.next ();
            if (filter == null || filter.checkEntry(entry))
            {
                // Must copy because entry is associated with bookLevel.
                MamdaOrderBookEntry copyEntry = new MamdaOrderBookEntry (entry);
                copyEntry.setAction (MamdaOrderBookEntry.ACTION_ADD);
                addEntry (copyEntry);
            }
        }
        mTime = bookLevel.mTime;

    }

    public void deleteEntriesFromSource (MamaSource                   source,
                                         MamdaOrderBookBasicDeltaList delta)
    {
        if (mEntries.isEmpty ())
            return;

        ListIterator i = mEntries.listIterator ();
        MamdaOrderBookEntry entry;
        while (i.hasNext ())
        {
            entry = (MamdaOrderBookEntry) i.next ();
            if (entry.getSource().isPartOf (source))
            {
                if (delta != null)
                {
                    System.out.print ("perhaps need to implement delta for deleteEntriesFromSource()\n");
                }
                eraseEntryByIterator (mEntries, i, entry);
            }
        }
    }

    private void eraseEntryByIterator (Vector              entries, 
                                       ListIterator        i,
                                       MamdaOrderBookEntry entry)
    {
        boolean checkState = mBook.getCheckSourceState();
        i.remove ();
        mBook.detach (entry);
        if (!checkState || entry.isVisible())
        {
            mSize -= entry.getSize();
            mNumEntries--;
        }
        mNumEntriesTotal--;
    }

    /**
     * Re-evaluate the price level.  This would be performed after the
     * status of sources and/or subsources of an "aggregated order
     * book" (i.e., a book built from multiple sources) have changed.
     *
     * @return Whether the book info changed based on the re-evaluation.
     */
    public boolean reevaluate ()
    {
        if (mEntries.isEmpty())
            return false;
        long             size       = 0;
        long             numEntries = 0;
        boolean          changed    = false;
        boolean          checkState = mBook.getCheckSourceState();

        Iterator i = mEntries.iterator ();
        MamdaOrderBookEntry entry;
        while (i.hasNext ())
        {
            entry = (MamdaOrderBookEntry) i.next ();
            if (!checkState || entry.isVisible())
            {
                size += entry.getSize();
                numEntries++;
            }
        }

        if (mSize != size)
        {
            mSize  = size;
            changed = true;
        }
        if (mNumEntries != numEntries)
        {
            mNumEntries = numEntries;
            changed = true;
        }
        return changed;
    }

    /**
     * Return the order book entry at position "pos" in the price level.
     *
     * @param pos  The position of the order book entry.
     * @return The order book entry or NULL if not found.
     */
    public MamdaOrderBookEntry getEntryAtPosition (long pos)
    {
        // Remember: pos may be zero, which would mean we want the first
        // entry.
        if (mEntries.isEmpty ())
            return null;

        boolean        checkState = mBook.getCheckSourceState();
        long  i = 0;
        Iterator iter = mEntries.iterator ();
        MamdaOrderBookEntry  entry;

        while (iter.hasNext ())
        {
            entry = (MamdaOrderBookEntry) iter.next();
            if (checkState && !entry.isVisible())
                continue;
            if (pos == i)
                return entry;
            i++;
        }
        return null;
    }

    public MamdaOrderBookEntry findOrCreateEntry (String id)
    {
        if (mEntries != null)
        {
            Iterator iter = mEntries.iterator ();
            while (iter.hasNext ())
            {
                MamdaOrderBookEntry  entry = (MamdaOrderBookEntry) iter.next ();
                if (id.equals (entry.getId ()))
                {
                    return entry;
                }
            }
        }
        // Not found
        MamdaOrderBookEntry  entry = new MamdaOrderBookEntry();
        entry.setId         (id);
        entry.setAction     (MamdaOrderBookEntry.ACTION_ADD);
        entry.setPriceLevel (this);
        mEntries.add        (entry);

        boolean checkState = (mBook != null) ? mBook.getCheckSourceState() : false;
        if (!checkState || entry.isVisible())
        {
            mNumEntries++;
            mSize += entry.getSize();
        }
        mNumEntriesTotal++;

        if (mBook.getGenerateDeltaMsgs())
        {
            //Need to set correct action based on numEntries in level
            char plAction;
            if (0 == mNumEntriesTotal) 
            {
                plAction = MamdaOrderBookPriceLevel.ACTION_ADD;
            }
            else
            {
                plAction = MamdaOrderBookPriceLevel.ACTION_UPDATE;
            }
            mBook.addDelta (entry, entry.getPriceLevel(), 
                            entry.getPriceLevel().getSizeChange(),
                            plAction,
                            MamdaOrderBookEntry.ACTION_ADD);
        }
        return entry;
    }

    public MamdaOrderBookEntry findEntry (String id)
    {
        MamdaOrderBookEntry entry = null;
        if (mEntries != null)
        {
            Iterator iter = mEntries.iterator ();
            while (iter.hasNext ())
            {
                entry = (MamdaOrderBookEntry) iter.next ();
                if (id.equals (entry.getId ()))
                {
                    return entry;
                }
            }
        }
        return null;
    }
}
