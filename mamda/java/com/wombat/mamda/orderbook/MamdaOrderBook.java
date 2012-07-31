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

import java.util.Comparator;
import java.util.Iterator;
import java.util.TreeMap;
import java.util.logging.Logger;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.NoSuchElementException;
import java.util.Collections;
import java.util.ArrayList;

/**
 * MamdaOrderBook is a class that provides order book functionality,
 * including iterators over price levels and entries within price
 * levels.
 *
 * An order book may be read only or writeable. Copying into a read only
 * book is faster than creating a writeable book. Calling methods that attempt
 * to edit the book structure on a read only book will throw a
 * MamdaOrderBookException.
 */

public class MamdaOrderBook
{        
    private static Logger mLogger = Logger.getLogger("com.wombat.mamda.MamdaOrderBook");
    
    private MamdaOrderBookImpl      mImpl              = null;

    /**
     * Construct a new order book object. Contains no price levels upon
     * construction. This book will be writeable.
     */
    public MamdaOrderBook ()
    {
           mImpl = new MamdaOrderBookFull(this);
    }

    /**
     * Construct a copy of the supplied orderbook.
     *
     * @param copy The book to copy.
     */
    public MamdaOrderBook (MamdaOrderBook copy)
    {
        mImpl = new MamdaOrderBookFull(this, copy);
    }
    
    /**
     * Construct a copy of the supplied orderbook.
     *
     * @param copy The book to copy.
     * @param writeable is this copy writable. false creates a read only copy
     */
    public MamdaOrderBook (MamdaOrderBook copy, boolean writeable)
    {
        if (writeable)
        {
            mImpl = new MamdaOrderBookFull(this, copy);
        }
        else
        {
            mImpl = new MamdaOrderBookCopy(copy);
        }
    }

    /**
     * If this book is read only.
     * @return true if this book is read only.
     */
    public boolean isReadOnly()
    {
        return mImpl.isReadOnly();
    }
    
    /**
     * Clear all levels from the order book. Clears both the bid and ask
     * side levels from the order book.
     */
    public void clear ()
    {
        mImpl.clear();
    }

    /**
     * The order book subscription symbol.
     * @param symbol the symbol.
     */
    public void setSymbol (String symbol)
    {
        mImpl.setSymbol(symbol);
    }
    
    /**
     * The order book subscription symbol.
     * @return the symbol.
     */
    public String getSymbol ()
    {
        return mImpl.getSymbol();
    }

    public void setPartId (String PartId)
    {
         mImpl.setPartId (PartId);
    }

    public String getPartId()
    {  
        return mImpl.getPartId();
    }

    public boolean hasPartId()
    {
        return mImpl.hasPartId();
    }     

    /**
     * Create a price level in the orderbook for the given price/size.
     * The price level is initially empty and marked as "not used".
     * The "not used" status changes automatically when entries are
     * added to the price level.
     *
     * @param price  The price of the price level to find/create.
     * @param side   The side of the book of the price level to find/create.

     * @return The found or newly create price level.
     *
     * @throws MamdaOrderBookException When an error is encountered during
     * book processing.
     */
    public MamdaOrderBookPriceLevel findOrCreateLevel (double price,
                                                       char   side)
    {
        return mImpl.findOrCreateLevel (price, side);
    }
    
    public MamdaOrderBookPriceLevel findLevel (double price,
                                               char   side)
    {
        return mImpl.findLevel (price, side);
    }
    
    public MamdaOrderBookPriceLevel getMarketOrdersSide (char side) 
    {
        return mImpl.getMarketOrdersSide (side);
    }

    public MamdaOrderBookPriceLevel getOrCreateMarketOrdersSide (char side) 
    {
        return mImpl.getOrCreateMarketOrdersSide (side);
    }

    /**
     * Apply a delta to this (presumably) full book.
     *
     * @param deltaBook The delta to apply to the order book.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void apply (MamdaOrderBook  deltaBook)
    {
        mImpl.apply(deltaBook);
    }
    
    /**
     * Apply a <code>MamdaOrderBookBasicDelta</code> to this book.
     *
     * @param delta The delta to apply.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void apply (MamdaOrderBookBasicDelta delta)
    {
        mImpl.apply(delta);
    }

    public void apply (MamdaBookAtomicLevel level)
    {
        mImpl.apply(level);
    }
    
    public void apply (MamdaBookAtomicLevelEntry levelEntry)
    {
        mImpl.apply(levelEntry);
    }
    
    /**
     * Apply a <code>MamdaOrderBookBasicDeltaList</code> to this book.
     *
     * @param deltaList The deltas to apply.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void apply (MamdaOrderBookBasicDeltaList deltaList)
    {
        Iterator i = deltaList.iterator ();
        while (i.hasNext ())
        {
            mImpl.apply ((MamdaOrderBookBasicDelta) i.next ());
        }
    }

    /**
     * Make a writeable deep copy of a book.
     *
     * @param book The order book to copy.
     */
    public void copy (MamdaOrderBook  book)
    {        
        mImpl.copy(book);
    }
    
    /**
     * Make a read only deep copy of a book. This method is
     * faster than the copy (MamdaOrderBook  book) method.
     *
     * @param book The order book to copy.
     */
    public void copyReadOnly (MamdaOrderBook  book)
    {
        mImpl = new MamdaOrderBookCopy();      
        mImpl.copy(book);
    }

    /**
     * Set this order book to be a delta that would, when applied,
     * delete all of the fields in the bookToDelete.
     *
     * @param bookToDelete The book to be deleted.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void setAsDeltaDeleted (MamdaOrderBook  bookToDelete)
    {
        mImpl.setAsDeltaDeleted (bookToDelete);
    }

    /**
     * Set this order book to be a delta that would, when applied, be
     * the difference between to other books.
     *
     * @param lhs An order book.
     * @param rhs An order book.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void setAsDeltaDifference (MamdaOrderBook  lhs,
                                      MamdaOrderBook  rhs)
    {
        mImpl.clear();
    }

    /**
     * Get the total number of price levels (both sides of order book).
     *
     * @return The total number of level in the order book.
     */
    public long getTotalNumLevels ()
    {
        return mImpl.getTotalNumLevels();
    }

    /**
     * Get the number of bid levels.
     *
     * @return the number of bid levels.
     */
    public int getNumBidLevels ()
    {
        return mImpl.getNumBidLevels();
    }

    /**
     * Get the number of ask levels.
     * @return the number of ask levels.
     */
    public int getNumAskLevels ()
    {
        return mImpl.getNumAskLevels ();
    }

    public MamdaOrderBookPriceLevel getBidMarketOrders ()
    {
        return mImpl.mBidMarketOrders;
    }

    public MamdaOrderBookPriceLevel getAskMarketOrders ()
    {
        return mImpl.mAskMarketOrders;
    }

    /**
     * Return the book time.
     * @return return the book time.
     */
    public MamaDateTime getBookTime()
    {
        return mImpl.getBookTime();
    }

    /**
     * Set The SourceDerivative for this book.
     * @param value The SourceDerivative.
     */
    public void setBookTime (MamaDateTime value)
    {
       mImpl.setBookTime(value);
    }
    
    /**
     * Set The SourceDerivative for this book.
     * @param value The SourceDerivative.
     */
    public void setSourceDerivative (MamaSourceDerivative value)
    {
       mImpl.setSourceDerivative(value);
    }

    /**
     * Return the SourceDerivative for this book.
     * @return the SourceDerivative.
     */
    public MamaSourceDerivative getSourceDerivative ()
    {
        return mImpl.getSourceDerivative();
    }

    /**
     * Get the MamaSource for this order book.
     *
     * @return The source.
     */
    public MamaSource getSource ()
    {
        return mImpl.getSourceDerivative();
    }

    /**
     * Set the mamaQuality for this order book.
     *
     * @param quality The new quality.
     */
    public void setQuality (short  quality)
    {
        mImpl.setQuality(quality);
    }

    /**
     * Get the mamaQuality for this order book.
     *
     * @return The quality.
     */
    public short  getQuality ()
    {
        return mImpl.getQuality();
    }

    /**
     * Set the order book closure handle.
     *
     * @param closure The closure.
     */
    public void setClosure (Object  closure)
    {
        mImpl.setClosure(closure);
    }

    /**
     * Get the order book closure handle.
     *
     * @return The orderbook closure.
     */
    public Object  getClosure ()
    {
        return mImpl.getClosure();
    }

    public int hashCode ()
    {
        /* From Effective Java */
        return mImpl.hashCode();
    }

    public boolean equals (Object obj)
    {
        return obj instanceof MamdaOrderBook && mImpl.equals (obj);
    }

    /**
     * Add an entry to the order book and (if "delta" is not NULL)
     * record information about the delta related to this action.
     *
     * @param entry the entry
     * @param price the price
     * @param side the side (bid or ask)
     * @param eventTime the event time.
     * @param delta the delta.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void addEntry (
        MamdaOrderBookEntry            entry,
        double                         price,
        char                           side,
        MamaDateTime                   eventTime,
        MamdaOrderBookBasicDelta       delta)
    {
        mImpl.addEntry( entry, price, side, eventTime, delta);
    }

    /**
     * Add an entry to the order book and (if "delta" is not NULL)
     * record information about the delta related to this action.  The
     * new entry is returned.
     * @param price the price
     * @param side the side (bid or ask)
     * @param eventTime the event time.
     * @param delta the delta.
     * @param entryId The id.
     * @param entrySize the size
     * @param source the MamaSourceDerivative.
     * @return the new entry.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public MamdaOrderBookEntry addEntry (
        String                   entryId,
        long                     entrySize,
        double                   price,
        char                     side,
        MamaDateTime             eventTime,
        MamaSourceDerivative     source,
        MamdaOrderBookBasicDelta delta)
    {
      MamdaOrderBookEntry  entry =
          new MamdaOrderBookEntry (entryId, entrySize,
                                   MamdaOrderBookEntry.ACTION_ADD,
                                   eventTime,
                                   source);
      addEntry (entry, price, side, eventTime, delta);
      return entry;
    }

    /**
     * Add a price level to the orderbook.
     * @param level The price level to add to the orderbook.
     *
     */
    public void addLevel(MamdaOrderBookPriceLevel  level)
    {
        mImpl.addLevel (level);
    } 
    
    /**
     * Update an existing level in the orderbook.
     *
     * @param level The details of the price level to update.
     *
     * @throws MamdaOrderBookException When an error is encountered during
     * book processing.
     */
    public void updateLevel (MamdaOrderBookPriceLevel  level)
    {
        mImpl.updateLevel (level);
    }
  
    /**
     * Populate a MamaMsg of the changes to this order book. 
     * This will include the changes from the last time this function was called
     * or all changes from the initial state. 
     *  @param msg A MamaMsg ref containing all changes to the current book.
     */   
    public boolean populateDelta(MamaMsg msg)
    {
        return mImpl.populateDelta(msg);
    }

    /**
     * Populate a MamaMsg with the current state of this order book.
     * @param msg A MamaMsg containing all book, price and entry 
     * (if applicable) details of the current book. 
     */  
    public void populateRecap(MamaMsg msg)
    {
        mImpl.populateRecap(msg, this);
    }

    /**
     * Delete a price level from the orderbook.
     *
     * @param level The price level to delete from the orderbook.
     *
     * @throws MamdaOrderBookException When an error is encountered during
     * book processing.
     */
    public void deleteLevel(MamdaOrderBookPriceLevel level)
    {
        mImpl.deleteLevel (level);
    }    
    
    /**
     * Re-evaluate the order book.  This would be performed after the
     * status of sources and/or subsources of an "aggregated order
     * book" (i.e., a book built from multiple sources) have changed.
     *
     * @return Whether the book info changed based on the re-evaluation.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public boolean reevaluate()
    {
        return mImpl.reevaluate();
    }

    /**
     * Set whether this book needs a re-evaluation.
     * @param need true if books requires re-evaluation.
     */
    public void setNeedsReevaluation (boolean  need)
    {
        mImpl.setNeedsReevaluation(need);
    }

    /**
     * Get whether this book needs a re-evaluation.
     * @return true if the book needs re-evaluation.
     */
    public boolean getNeedsReevaluation ()
    {
        return mImpl.getNeedsReevaluation();
    }

    /**
     * Set whether to check the MamaSourceState when
     * adding/deleting/re-evaluating entries in the book.
     *
     * @param check true to check the MamaSourceState.
     */
    public void setCheckSourceState (boolean  check)
    {
        mImpl.setCheckSourceState(check);
    }

    /**
     * Get whether to check the MamaSourceState when
     * adding/deleting/re-evaluating entries in the book.
     * @return true if checking the MamaSourceState
     */
    public boolean getCheckSourceState ()
    {
        return mImpl.getCheckSourceState();
    }

    /**
     * Return the order book price level at "price" on "side" of the
     * order book.
     *
     * @param price  The price of the order book price level.
     * @param side   The side of the order book to search.
     * @return The order book price level or NULL if not found.
     *
     */
    public MamdaOrderBookPriceLevel getLevelAtPrice (double price, char side)
    {
        return mImpl.getLevelAtPrice (price, side);
    }

    /**
     * Return the order book price level at position "pos" in the
     * order book.
     *
     * @param pos   The position of the order book price level.
     * @param side  The side of the order book to search.
     * @return The order book price level or NULL if not found.
     *
     */
    public MamdaOrderBookPriceLevel getLevelAtPosition (long pos, char side)
    {
        return mImpl.getLevelAtPosition (pos,side);
    }

    /**
     * Return the order book entry at position "pos" in the order book.
     *
     * @param pos  The position of the order book entry.
     * @param side  The side of the order book to search.
     * @return The order book entry or NULL if not found.
     */
    public MamdaOrderBookEntry getEntryAtPosition (long pos, char side)
    {
        // Remember: pos may be zero, which would mean we want the first
        // entry in a non-empty price level.
        return mImpl.getEntryAtPosition (pos, side);
    }

    /**
     * Order book equality verification.  A MamdaOrderBookException is
     * thrown if the books are not equal, along with the reason for
     * the inequality.
     *
     * @param rhs The order book to compare to the current book.
     *
     * @throws MamdaOrderBookException The two books are not equal.
     */
    public void assertEqual (MamdaOrderBook  rhs)
    {
        mImpl.assertEqual(rhs);
    }
    
    /**
     * Order book level equality verification.  A MamdaOrderBookException is
     * thrown if the books are not equal, along with the reason for
     * the inequality.
     *
     * @param rhs The order book to compare to the current book.
     *
     * @throws MamdaOrderBookException The two books are not equal.
     */
    public void assertEqual (MamdaOrderBookPriceLevel lhsLevel,
                             MamdaOrderBookPriceLevel rhsLevel)
    {
        mImpl.assertEqual(lhsLevel, rhsLevel);
    }

    /**
     * Set whether the order book is in a consistent or an an
     * inconsistent state.
     *
     * @param isConsistent Whether the book is consistent.
     */
    public void setIsConsistent (boolean  isConsistent)
    {
        mImpl.setIsConsistent(isConsistent);
    }

    /**
     * Get whether the order book is in a consistent or an an
     * inconsistent state. The orderbook will be marked as inconsistent if a
     * gap is detected by the Listener. The book will be marked consistent
     * again once a full recap for the book has been obtained from the feed.
     *
     * @return boolean Whether the book is in a consistent state.
     */
    public boolean getIsConsistent ()
    {
        return mImpl.getIsConsistent();
    }

    /**
     * Dump the order book to standard out.
     */
    public void dump()
    {
        dump (System.out);
    }

    /**
     * Dump the order book to an <code>OutputStream</code>.
     * @param outputStream the <code>OutputStream</code>
     */
    public void dump (OutputStream outputStream)
    {
        mImpl.dump( outputStream );
    }

    /**
     * Update an entry in the order book and (if "delta" is not NULL)
     * record information about the delta related to this action.  If
     * the entry is not internally "wired" to the order book, a
     * MamdaOrderBookInvalidEntry exception is thrown.
     * @param entry the entry
     * @param size the size
     * @param eventTime the event time
     * @param delta the delta.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void updateEntry (
        MamdaOrderBookEntry       entry,
        double                    size,
        MamaDateTime              eventTime,
        MamdaOrderBookBasicDelta  delta)
    {
        mImpl.updateEntry (entry, size, eventTime, delta);
    }

    /**
     * Delete an entry in the order book and (if "delta" is not NULL)
     * record information about the delta related to this action.  If
     * the entry is not internally "wired" to the order book, a
     * MamdaOrderBookInvalidEntry exception is thrown.
     * @param entry the entry to detete.
     * @param eventTime the event time.
     * @param delta the delta
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void deleteEntry (
        MamdaOrderBookEntry      entry,
        MamaDateTime             eventTime,
        MamdaOrderBookBasicDelta delta)
    {
        mImpl.deleteEntry (entry, eventTime, delta);
    }

    /**
     * Add all entries from another book into this book.
     *
     * @param book    The source book to add.
     * @param filter  If not NULL, a filter to apply to each entry.
     * @param delta   An optional delta to collect the added entries.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void addEntriesFromBook (
        MamdaOrderBook                 book,
        MamdaOrderBookEntryFilter      filter,
        MamdaOrderBookBasicDeltaList   delta)
    {
        mImpl.addEntriesFromBook( book, filter, delta);
    }

    /**
     * Add all price levels from another book as entries (one per
     * price level) into this book using "source" as the entryId for
     * each entry.
     *
     * @param book    The source book to add.
     * @param source  The name to use as the entry ID.
     * @param delta   An optional delta to collect the added entries.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void addPriceLevelsFromBookAsEntries (
        MamdaOrderBook                 book,
        String                         source,
        MamdaOrderBookBasicDeltaList   delta)
    {
        mImpl.addPriceLevelsFromBookAsEntries( book, source, delta );
    }

    /**
     * Delete all entries in this book that have "source" as its MamaSource.
     *
     * @param source  The source to match.
     * @param delta   An optional delta to collect the deleted entries.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void deleteEntriesFromSource (
        MamaSource                     source,
        MamdaOrderBookBasicDeltaList   delta)
    {
        mImpl.deleteEntriesFromSource( source, delta );
    }

    /**
     * Enable the generation of book deltas for this book. When delta generation is enabled
     * changes to the book are saved and can be popultaed to MamaMsgs. 
     * @param publish Whether book delta generation is enabled.
     */    
    public void generateDeltaMsgs(boolean publish)
    {
         mImpl.generateDeltaMsgs(publish);
    }

    /**
     * Get whether book delta generation is enabled
     * @return Whether book delta generation is enabled.
     */
    public boolean getGenerateDeltaMsgs()
    {
        return mImpl.mGenerateDeltas;
    }
                  
    /**
     * clear the delta list using for storing generated deltas
     */   
    public void clearDeltaList()
    {
        mImpl.mPublishComplexDelta.clear();
        mImpl.mPublishSimpleDelta.clear();
        mImpl.mCurrentDeltaCount = 0;    
    }

    /**
     * For book delta generation. 
     * Add a delta to the order book delta list for the publishing of
     * order book data
     * @param entry MamdaOrderBookEntry where change occurred.
     * @param level MamdaOrderBookPriceLevel where change occurred.
     * @param plDeltaSize Pricelevel size change.
     * @param plAction Pricelevel action.
     * @param entAction Entry action.     
     */ 
    public void addDelta( 
        MamdaOrderBookEntry      entry,
        MamdaOrderBookPriceLevel level,
        double                   plDeltaSize,
        char                     plAction,
        char                     entryAction)
    {
        mImpl.addDelta(entry, level, plDeltaSize, plAction, entryAction);
    }    

    /**
     * NOOP FOR JAVA
     * Adds the entry to the list of entries to be detached.
     * @param entry the entry to detach.
     */
    public void detach (MamdaOrderBookEntry entry)
    {
        // NOOP for java as it is reference counted.
    }

    /**
     * Detach a price level.
     * @param level the level to detach.
     *
     * @throws MamdaOrderBookException if called on a read only order book
     */
    public void detach (MamdaOrderBookPriceLevel level)
    {
        // Remove the level from the relevant side
        mImpl.detach(level);
    }

    
    /**
     * Iterator for all the bid side entries in the book.
     * @return the iterator.
     */
    public Iterator bidEntryIterator ()
    {
        return new EntryIterator (bidIterator());
    }
    
    /**
     * Allows an application to iterate over all the bid side price
     * levels in the order book.
     *
     * @return Iterator java.util.Iterator for the bid
     * side price levels in the order book.
     */
    public Iterator bidIterator()
    {
        return mImpl.bidIterator();
    }

    /**
     * Allows an application to iterate over all ask side price levels
     * in the order book.
     *
     * @return Iterator java.util.Iterator for the ask
     * side price levels in the order book.
     */
    public Iterator askIterator()
    {
        return mImpl.askIterator();
    }

    /**
     * Iterator for all the ask side entries in the book.
     * @return the iterator.
     */
    public Iterator askEntryIterator ()
    {
        return new EntryIterator (askIterator());
    }
    
    /**
     * Enforce strict checking of order book modifications (at the
     * expense of some performance).  This setting is passed on to the
     * MamdaOrderBookPriceLevel and MamdaOrderBookEntry classes.
     * @param strict the value  for strict checking.
     */
    public static void setStrictChecking (boolean strict)
    {
        MamdaOrderBookPriceLevel.setStrictChecking (strict);
        MamdaOrderBookEntry.setStrictChecking (strict);
    }

    public void cleanupDetached ()
    {
        // NOOP for Java for now.
    }

    private class EntryIterator implements Iterator
    {
        private Iterator mLevelIterator = null;
        private Iterator mEntryIterator = null;

        public EntryIterator (Iterator levels)
        {
            mLevelIterator = levels;
        }

        public void remove ()
        {
            if (mEntryIterator != null)
            {
                mEntryIterator.remove ();
            }
        }

        public boolean hasNext ()
        {
            // No more entires and no more levels.
            if ((mEntryIterator == null || !mEntryIterator.hasNext ()) &&
                !mLevelIterator.hasNext ())
            {
                return false;
            }

            if (mEntryIterator != null && mEntryIterator.hasNext ())
                return true;

            // Look for a level with entries.
            while (mLevelIterator.hasNext ())
            {
                MamdaOrderBookPriceLevel level =
                    (MamdaOrderBookPriceLevel) mLevelIterator.next ();
                mEntryIterator = level.entryIterator ();
                if (mEntryIterator.hasNext ())
                    return true;
            }
            return false;
        }

        public Object next ()
        {
            // This check also makes shure that we have the correct iterators.
            if (hasNext ())
            {
                return mEntryIterator.next ();
            }
            else
            {
                // See JavaDoc for Iterator.
                throw new NoSuchElementException ();
            }
        }
    }

    private abstract class MamdaOrderBookImpl
    {
        protected String                             mSymbol              = "";
        protected String                             mPartId              = "";
        protected MamdaOrderBook                     parent;
        protected boolean                            mIsConsistent        = true;
        protected MamaDateTime                       mBookTime            = null;
        protected MamaSourceDerivative               mSourceDeriv;
        protected short                              mQuality             = MamaQuality.QUALITY_UNKNOWN;
        protected Object                             mClosure             = null;
        protected boolean                            mCheckVisibility     = false;
        protected boolean                            mNeedsReevaluation   = false;
        protected boolean                            mHasPartId           = false;
        protected boolean                            mGenerateDeltas      = false;
        protected MamdaOrderBookPriceLevel           mBidMarketOrders     = null;
        protected MamdaOrderBookPriceLevel           mAskMarketOrders     = null;
        protected int                                mCurrentDeltaCount   = 0;
        protected MamdaOrderBookConcreteSimpleDelta  mPublishSimpleDelta  = null;
        protected MamdaOrderbookConcreteComplexDelta mPublishComplexDelta = null;
        protected MamdaOrderBookWriter               mWriter              = null;
        
        public abstract boolean isReadOnly();

        public MamaPrice mPrice = new MamaPrice ();
        
        protected MamaPrice tmpPrice = new MamaPrice();

        public void setSymbol (String symbol)
        {
            mSymbol = symbol;
        }

        public String getSymbol ()
        {
            return mSymbol;
        }

        public void setPartId (String PartId)
        {    
            if (PartId.equals(""))
            {
                mHasPartId = false;
            }
            else
            {
                mHasPartId = true;
                mPartId = PartId;
            }
        }

        public String getPartId()
        {
            return mPartId;
        }

        public boolean hasPartId()
        {
            return mHasPartId;
        }   

        public MamaDateTime getBookTime()
        {
            return mBookTime;
        }


        public void setBookTime (MamaDateTime value)
        {
            mBookTime =  value;
        }
        
        public void setSourceDerivative (MamaSourceDerivative value)
        {
            mSourceDeriv = value;
        }

        public MamaSourceDerivative getSourceDerivative ()
        {
            return mSourceDeriv;
        }


        public MamaSource getSource ()
        {
            return mSourceDeriv != null ? mSourceDeriv.getBaseSource () : null;
        }

        public void setQuality (short  quality)
        {
            mQuality = quality;
        }


        public short  getQuality ()
        {
            return mQuality;
        }


        public void setClosure (Object  closure)
        {
            mClosure = closure;
        }


        public Object  getClosure ()
        {
            return mClosure;
        }

        public void setIsConsistent (boolean  isConsistent)
        {
            mIsConsistent = isConsistent;
        }

        public boolean getIsConsistent ()
        {
            return mIsConsistent;
        }

        public abstract void clear ();

        public abstract MamdaOrderBookPriceLevel findOrCreateLevel (double price, 
                                                                    char   side);
                                                                    
        public abstract MamdaOrderBookPriceLevel findLevel (double  price, 
                                                            char    side);
                                                                    
        public abstract MamdaOrderBookPriceLevel getMarketOrdersSide (char side);

        public abstract MamdaOrderBookPriceLevel getOrCreateMarketOrdersSide (char side);
        
        public abstract void apply (MamdaOrderBook  deltaBook);

        public abstract void apply (MamdaOrderBookBasicDelta delta);
        
        public abstract void apply (MamdaBookAtomicLevel level);
        
        public abstract void apply (MamdaBookAtomicLevelEntry levelEntry);
        
        public abstract void copy (MamdaOrderBook  book);

        public abstract long getTotalNumLevels ();

        public abstract int getNumBidLevels ();

        public abstract int getNumAskLevels ();

        public abstract MamdaOrderBookPriceLevel getLevelAtPrice (double price, char side);

        public abstract MamdaOrderBookPriceLevel getLevelAtPosition (long pos, char side);

        public abstract MamdaOrderBookEntry getEntryAtPosition (long pos, char side);

        public abstract void addEntry (
            MamdaOrderBookEntry            entry,
            double                         price,
            char                           side,
            MamaDateTime                   eventTime,
            MamdaOrderBookBasicDelta       delta);

        public abstract void addLevel(MamdaOrderBookPriceLevel level);

        public abstract void updateLevel(MamdaOrderBookPriceLevel level);

        public abstract void deleteLevel(MamdaOrderBookPriceLevel level);

        public abstract void updateEntry (
            MamdaOrderBookEntry       entry,
            double                    size,
            MamaDateTime              eventTime,
            MamdaOrderBookBasicDelta  delta);

        public abstract void deleteEntry (
            MamdaOrderBookEntry      entry,
            MamaDateTime             eventTime,
            MamdaOrderBookBasicDelta delta);
        
        public abstract void addEntry (MamdaBookAtomicLevelEntry levelEntry);

        public abstract void updateEntry (MamdaBookAtomicLevelEntry levelEntry);

        public abstract void deleteEntry (MamdaBookAtomicLevelEntry levelEntry);

        public abstract void addEntriesFromBook (
            MamdaOrderBook                 book,
            MamdaOrderBookEntryFilter      filter,
            MamdaOrderBookBasicDeltaList   delta);

        public abstract void addPriceLevelsFromBookAsEntries (
            MamdaOrderBook                 book,
            String                         source,
            MamdaOrderBookBasicDeltaList   delta);

        public abstract void deleteEntriesFromSource (
            MamaSource                     source,
            MamdaOrderBookBasicDeltaList   delta);

        public abstract void generateDeltaMsgs (boolean publish);

        public abstract void addDelta (
                        MamdaOrderBookEntry              entry,
                        MamdaOrderBookPriceLevel         level,
                        double                           plDeltaSize,
                        char                             plAction,
                        char                             entryAction);            

        public abstract void detach (MamdaOrderBookPriceLevel level);

        public abstract Iterator bidIterator();

        public abstract Iterator askIterator();

        public void assertEqual (MamdaOrderBook  rhs)
        {
            if (!mSymbol.equals (rhs.getSymbol()))
            {
                throw new MamdaOrderBookException(
                    "different symbols: " + mSymbol + "/" + rhs.getSymbol());
            }
            if (!mPartId.equals (rhs.getPartId()))
            {
                throw new MamdaOrderBookException(
                    "different participants: " + mPartId + "/" + rhs.getPartId());
            }

            if (getNumBidLevels() != rhs.getNumBidLevels() || getNumAskLevels() != rhs.getNumAskLevels())
            {
                System.out.println(getNumBidLevels()+"  "+ rhs.getNumBidLevels()+"  "+getNumAskLevels()+"  "+ rhs.getNumAskLevels());
                throw new MamdaOrderBookException(
                    "number of price levels do not add up");
            }
            assertEqual (bidIterator(), rhs.bidIterator());
            assertEqual (askIterator(), rhs.askIterator());
            assertEqual (mBidMarketOrders, rhs.getBidMarketOrders());
            assertEqual (mAskMarketOrders, rhs.getAskMarketOrders());

        }

        private void assertEqual (MamdaOrderBookPriceLevel lhsLevel,
                                  MamdaOrderBookPriceLevel rhsLevel)
        {
            if (null == lhsLevel)
            {
                if (null == rhsLevel) return;
                
                StringBuffer errMsg = new StringBuffer (1000);
                errMsg.append (" assertEqual():").
                    append("market order levels not equal ([Empty] ").append (rhsLevel.getSize());
                throw new MamdaOrderBookException (errMsg.toString ());
            }

            if (null == rhsLevel)
            {
                StringBuffer errMsg = new StringBuffer (1000);
                errMsg.append (" assertEqual():").
                    append("market order levels not equal ([Empty] ").append (lhsLevel.getSize());
                throw new MamdaOrderBookException (errMsg.toString ());
            }
            lhsLevel.assertEqual (rhsLevel);
        }

        public boolean populateDelta(MamaMsg msg)
        {                    
            if (mGenerateDeltas)
            {
                if (0 == mCurrentDeltaCount)
                {
                    mLogger.finest("MamdaOrderBook::populateDelta() Trying to create publisher msg from empty state");

                    return false;
                }
                if (1 == mCurrentDeltaCount)
                {     
                    mWriter.populateMsg(msg, mPublishSimpleDelta); 
                    mPublishSimpleDelta.clear();
                    mCurrentDeltaCount = 0;
                    return true;
                }
                else
                { 
                    mWriter.populateMsg(msg, mPublishComplexDelta);  
                    mPublishComplexDelta.clear();
                    mPublishSimpleDelta.clear();
                    mCurrentDeltaCount = 0;
                    return true;
                }
            }
            else
            {
                mLogger.warning("MamdaOrderBook::populateDelta() Order Book publishing not enabled");
                return false;
            }
        }
        
        public void populateRecap (MamaMsg msg, MamdaOrderBook mBook)
        {  
            if (mGenerateDeltas)
            {       
                mWriter.populateMsg(msg, mBook);
            }
            else
            {                       
                mLogger.warning( "MamdaOrderBook::populateRecap() Order Book publishing not enabled");       
            }
        }  

        private void assertEqual (Iterator  lhsIter,
                                  Iterator  rhsIter)
        {
            while (lhsIter.hasNext() && rhsIter.hasNext())
            {
                MamdaOrderBookPriceLevel lhsLevel =
                        (MamdaOrderBookPriceLevel)lhsIter.next();
                MamdaOrderBookPriceLevel rhsLevel =
                        (MamdaOrderBookPriceLevel)rhsIter.next();
                lhsLevel.assertEqual (rhsLevel);
            }
        }

        public void dump ()
        {
            dump (System.out);
        }

        public void dump (OutputStream outputStream)
        {

            PrintWriter out = new PrintWriter (outputStream, true);
            out.println ("Dump book: " + mSymbol);

            int      i       = 0;
            Iterator bidIter = bidIterator();
            Iterator askIter = askIterator();

            while (bidIter.hasNext() || askIter.hasNext())
            {

                if (bidIter.hasNext())
                {
                    MamdaOrderBookPriceLevel bidLevel =
                        (MamdaOrderBookPriceLevel)bidIter.next();
                    out.print ("Bid " + i + " | ");
                    out.print ("price="    + bidLevel.getPrice()
                             + " size="    + bidLevel.getSize()
                             + " action="  + bidLevel.getAction()
                             + " entries=" + bidLevel.getNumEntries()
                             + " time=");
                    out.println ((bidLevel.getTime() != null) ? (bidLevel.getTime()).toString() : "null");

                    Iterator bidEntryIterator = bidLevel.entryIterator ();

                    while (bidEntryIterator.hasNext ())
                    {
                        MamdaOrderBookEntry bidEntry = (MamdaOrderBookEntry) bidEntryIterator.next ();

                        out.print ("      |    id="  + bidEntry.getId()
                                        + " size="   + bidEntry.getSize()
                                        + " action=" + bidEntry.getAction()
                                        + " time=");
                        out.println ((bidEntry.getTime() != null) ? (bidEntry.getTime()).toString() : "null");
                    }
                }

                else
                {
                    out.print ("          ");
                }

                if (askIter.hasNext())
                {
                    MamdaOrderBookPriceLevel askLevel =
                        (MamdaOrderBookPriceLevel)askIter.next();
                    System.out.print ("Ask " + i + " | ");
                    out.print ("price="    + askLevel.getPrice()
                             + " size="    + askLevel.getSize()
                             + " action="  + askLevel.getAction()
                             + " entries=" + askLevel.getNumEntries()
                             + " time=");
                    out.println ((askLevel.getTime() != null) ? (askLevel.getTime()).toString() : "null");
                    Iterator askEntryIterator = askLevel.entryIterator ();

                    while (askEntryIterator.hasNext ())
                    {
                        MamdaOrderBookEntry askEntry = (MamdaOrderBookEntry) askEntryIterator.next ();

                        out.print ("      |    id="  + askEntry.getId()
                                        + " size="   + askEntry.getSize()
                                        + " action=" + askEntry.getAction()
                                        + " time=");
                        out.println ((askEntry.getTime() != null) ? (askEntry.getTime()).toString() : "null");
                    }
                }
                out.println();
                ++i;
            }
            
            if (mAskMarketOrders != null)
            {
                System.out.print ("Ask Market Orders");
                    out.print ( " size="    + mAskMarketOrders.getSize()
                              + " action="  + mAskMarketOrders.getAction()
                              + " entries=" + mAskMarketOrders.getNumEntries()
                              + " time="    + mAskMarketOrders.getTime().getTimeAsString());
                if (((mAskMarketOrders.getTime()).hasDate()) == true)
                    System.out.println (" date=" + (mAskMarketOrders.getTime()).getDateAsString()); 

                Iterator iter = mAskMarketOrders.entryIterator ();
                while (iter.hasNext ())
                {
                    MamdaOrderBookEntry entry =
                        new MamdaOrderBookEntry((MamdaOrderBookEntry) iter.next ());
                    System.out.print ( "      |    id=" + entry.getId()
                                           + " size="   + entry.getSize() 
                                           + " action=" + entry.getAction()
                                           + " time="   + (entry.getTime()).getTimeAsString());
                    if ((entry.getTime()).hasDate())
                        System.out.println (" date="    + (entry.getTime()).getDateAsString());
                }
            }

            if (mBidMarketOrders != null)
            {
                System.out.print ("Bid Market Orders");
                out.print ( " size="    + mBidMarketOrders.getSize()
                          + " action="  + mBidMarketOrders.getAction()
                          + " entries=" + mBidMarketOrders.getNumEntries()
                          + " time="    + mBidMarketOrders.getTime().getTimeAsString());
                if ((mBidMarketOrders.getTime()).hasDate())
                    System.out.println (" date=" + (mBidMarketOrders.getTime()).getDateAsString());

                Iterator iter = mAskMarketOrders.entryIterator ();
                while (iter.hasNext ())
                {
                    MamdaOrderBookEntry entry =
                    new MamdaOrderBookEntry((MamdaOrderBookEntry) iter.next ());
                    System.out.print ("      |    id=" + entry.getId()
                                          + " size="   + entry.getSize() 
                                          + " action=" + entry.getAction()
                                          + " time="   + (entry.getTime()).getTimeAsString());
                    if ((entry.getTime()).hasDate())
                        System.out.println ( " date="   + (entry.getTime()).getDateAsString());

                }
            }
        }

        public boolean equals (MamdaOrderBook obj)
        {
            return (mSymbol.equals (obj.getSymbol())           &&
                    mPartId.equals(obj.getPartId())            &&
                    equals (bidIterator(), obj.bidIterator())  &&
                    equals (askIterator(), obj.askIterator()));
        }

        private boolean equals (Iterator  lhsIter,
                                Iterator  rhsIter)
        {
            try
            {

                while (lhsIter.hasNext() && rhsIter.hasNext())
                {
                    MamdaOrderBookPriceLevel lhsLevel =
                            (MamdaOrderBookPriceLevel)lhsIter.next();
                    MamdaOrderBookPriceLevel rhsLevel =
                            (MamdaOrderBookPriceLevel)rhsIter.next();
                    lhsLevel.assertEqual (rhsLevel);
                }
            }
            catch (MamdaOrderBookException ex)
            {
                return false;
            }
            return true;
        }

        public abstract void setAsDeltaDifference (MamdaOrderBook  lhs,
                                                   MamdaOrderBook  rhs);

        public abstract void setAsDeltaDeleted (MamdaOrderBook  bookToDelete);

        public abstract boolean reevaluate ();

        public void setNeedsReevaluation (boolean  need)
        {
            mNeedsReevaluation = need;
        }

        public boolean getNeedsReevaluation ()
        {
            return mNeedsReevaluation;
        }

        public void setCheckSourceState (boolean  check)
        {
            mCheckVisibility = check;
        }

        public boolean getCheckSourceState ()
        {
            return mCheckVisibility;
        }

        public Iterator bidEntryIterator ()
        {
            return new EntryIterator (bidIterator(), !isReadOnly());
        }

        public Iterator askEntryIterator ()
        {
            return new EntryIterator (askIterator(), !isReadOnly());
        }

        private class EntryIterator implements Iterator
        {
            private Iterator mLevelIterator = null;
            private Iterator mEntryIterator = null;
            private boolean  editable       = true;

            public EntryIterator (Iterator levels, boolean editable)
            {
                mLevelIterator = levels;
            }

            public void remove ()
            {
                if (!editable)
                {
                    throw new MamdaOrderBookException (
                        "MamdaOrderBookImpl::remove cannot be applied to read only book");
                }
                if (mEntryIterator != null)
                {
                    mEntryIterator.remove ();
                }
            }

            public boolean hasNext ()
            {
                // No more entires and no more levels.
                if ((mEntryIterator == null || !mEntryIterator.hasNext ()) &&
                    !mLevelIterator.hasNext ())
                {
                    return false;
                }

                if (mEntryIterator != null && mEntryIterator.hasNext ())
                    return true;

                // Look for a level with entries.
                while (mLevelIterator.hasNext ())
                {
                    MamdaOrderBookPriceLevel level =
                        (MamdaOrderBookPriceLevel) mLevelIterator.next ();
                    mEntryIterator = level.entryIterator ();
                    if (mEntryIterator.hasNext ())
                        return true;
                }
                return false;
            }

            public Object next ()
            {
                // This check also makes shure that we have the correct iterators.
                if (hasNext ())
                {
                    return mEntryIterator.next ();
                }
                else
                {
                    // See JavaDoc for Iterator.
                    throw new NoSuchElementException ();
                }
            }
        }
    }
 
    private class MamdaOrderBookFull extends MamdaOrderBookImpl
    {
        private TreeMap              mBidLevels       = null;
        private TreeMap              mAskLevels       = null;
    
        MamdaOrderBookFull (MamdaOrderBook parent)
        {
            this.parent = parent;
            mBidLevels = new TreeMap (new BidCompare());
            mAskLevels = new TreeMap (new AskCompare());
        }
        
        MamdaOrderBookFull (MamdaOrderBook parent, MamdaOrderBook copy)
        {
            this.parent = parent;
            copy (copy);
        }
    
        public boolean isReadOnly()
        {
            return false;
        }
            
        public void clear ()
        {
            mIsConsistent = true;
            mNeedsReevaluation = false;
            mBidLevels.clear();
            mAskLevels.clear();

            if (mBidMarketOrders != null)
            {
                mBidMarketOrders.clear();
                mBidMarketOrders = null;
            }
            if (mAskMarketOrders != null)
            {
                mAskMarketOrders.clear();
                mAskMarketOrders = null;
            }

        }
        public MamdaOrderBookPriceLevel findOrCreateLevel (
            double price,
            char   side)
        {
            MamaChar ignored = new MamaChar ();
            if (side == MamdaOrderBookPriceLevel.SIDE_BID)
                return findOrCreateLevel (mBidLevels, price, side, ignored);
            else if (side == MamdaOrderBookPriceLevel.SIDE_ASK)
                return findOrCreateLevel (mAskLevels, price, side, ignored);
            else
            {
                StringBuffer msg = new StringBuffer (255);
                msg.append ("MamdaOrderBookCopy::findOrCreateLevel(): ").
                    append ("invalid side provided: ").
                    append ("side");
                throw new MamdaOrderBookException(msg.toString ());
            }
        }
        
        public MamdaOrderBookPriceLevel findLevel (double   price,
                                                   char     side)
        {
            if (side == MamdaOrderBookPriceLevel.SIDE_BID)
                return findLevel (mBidLevels, price, side);
            else if (side == MamdaOrderBookPriceLevel.SIDE_ASK)
                return findLevel (mAskLevels, price, side);
            else
            {
                StringBuffer msg = new StringBuffer (255);
                msg.append ("MamdaOrderBookCopy::findLevel(): ").
                    append ("invalid side provided: ").
                    append ("side");
                throw new MamdaOrderBookException(msg.toString ());
            }
        }        

        public MamdaOrderBookPriceLevel getMarketOrdersSide (char side) 
        {
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
                return mBidMarketOrders;
            else if (MamdaOrderBookPriceLevel.SIDE_ASK== side)
                return mAskMarketOrders;
            else
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("getMarketOrdersSide(").append (mSymbol).
                       append ("): side=").append (side).
                       append (" invalid side provided");
                throw new MamdaOrderBookException (errMsg.toString ());
            } 
        }

        public MamdaOrderBookPriceLevel getOrCreateMarketOrdersSide (char side) 
        {
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
            {
                if (null == mBidMarketOrders)
                {
                    mBidMarketOrders = new MamdaOrderBookPriceLevel (
                        0.0,
                        MamdaOrderBookPriceLevel.SIDE_BID);

                    mBidMarketOrders.setOrderBook (parent);
                    mBidMarketOrders.setOrderType (
                        MamdaOrderBookPriceLevel.LEVEL_MARKET);
                }
                return mBidMarketOrders;
            }
            else if (MamdaOrderBookPriceLevel.SIDE_ASK == side)
            {
                if (null == mAskMarketOrders)
                {
                    mAskMarketOrders = new MamdaOrderBookPriceLevel (
                        0.0,
                        MamdaOrderBookPriceLevel.SIDE_ASK);

                    mAskMarketOrders.setOrderBook (parent);
                    mAskMarketOrders.setOrderType (
                        MamdaOrderBookPriceLevel.LEVEL_MARKET);
                }
                return mAskMarketOrders;
            }
            else
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("MamdaOrderBook::getOrCreateMarketOrdersSide()(").append (mSymbol).
                       append ("): side=").append (side).
                       append (" invalid side provided");
                throw new MamdaOrderBookException (errMsg.toString ());
            } 
        }

        public void apply (MamdaOrderBook  deltaBook)
        {
            applySide (mBidLevels, deltaBook.bidIterator());
            applySide (mAskLevels, deltaBook.askIterator());
            
            if (deltaBook.getBidMarketOrders() != null)
            {
                applyMarketOrderSide (MamdaOrderBookPriceLevel.SIDE_BID,
                                      deltaBook.getBidMarketOrders());
            }
            if (deltaBook.getAskMarketOrders() != null)
            {
                applyMarketOrderSide (MamdaOrderBookPriceLevel.SIDE_ASK,
                                      deltaBook.getAskMarketOrders());
            }
        }

        public void apply (MamdaOrderBookBasicDelta delta)
        {
            MamdaOrderBookPriceLevel level = delta.getPriceLevel();

            if (MamdaOrderBookPriceLevel.LEVEL_MARKET ==
                level.getOrderType())
            {
                applyMarketOrderSide (level.getSide(), level, delta);
                return;
            }
            switch (level.getSide())
            {
                case MamdaOrderBookPriceLevel.SIDE_BID:
                    applySide (mBidLevels, delta);
                    break;
                case MamdaOrderBookPriceLevel.SIDE_ASK:
                    applySide (mAskLevels, delta);
                    break;
                default:
                    throw new MamdaOrderBookException (
                        "MamdaOrderBookFull::apply (simple delta): bad side");
            }
        }
       
        
        public void apply (MamdaBookAtomicLevel level)
        {
            switch (level.getPriceLevelSide())
            {
                case MamdaOrderBookTypes.MAMDA_BOOK_SIDE_BID:
                    applySide (mBidLevels, level);
                    break;
                case MamdaOrderBookTypes.MAMDA_BOOK_SIDE_ASK:
                    applySide (mAskLevels, level);
                    break;    
                default:
                    throw new MamdaOrderBookException (
                        "MamdaOrderBookFull::apply (Atomic level): bad side");
            }
        }

        public void apply (MamdaBookAtomicLevelEntry levelEntry)
        {
            try
            {
                switch (levelEntry.getPriceLevelEntryAction())
                {
                    case MamdaOrderBookTypes.MAMDA_BOOK_ACTION_ADD:
                        addEntry (levelEntry);
                        break;
                    case MamdaOrderBookTypes.MAMDA_BOOK_ACTION_UPDATE:
                        updateEntry (levelEntry);
                        break;
                    case MamdaOrderBookTypes.MAMDA_BOOK_ACTION_DELETE:
                        deleteEntry (levelEntry);
                        break;
                    case MamdaOrderBookTypes.MAMDA_BOOK_ACTION_UNKNOWN:
                        updateEntry (levelEntry); //treated same as update
                        break;
                }
            }
            catch (MamdaOrderBookException e)
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append (e.getMessage ()).append (" (price=").
                       append (levelEntry.getPriceLevelMamaPrice ());
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }
            
        public MamdaOrderBookPriceLevel atomicUpdateToOrderBookPriceLevel (MamdaBookAtomicLevel level)
        {
            MamdaOrderBookPriceLevel priceLevel = new MamdaOrderBookPriceLevel();

            priceLevel.setAction     (level.getPriceLevelAction());
            priceLevel.setSide       (level.getPriceLevelSide());
            priceLevel.setPrice      (new MamaPrice (level.getPriceLevelPrice()) );
            priceLevel.setSize       (level.getPriceLevelSize());
            priceLevel.setNumEntries (level.getPriceLevelNumEntries());

            priceLevel.setSizeChange (level.getPriceLevelSizeChange());
            priceLevel.setTime       (level.getPriceLevelTime());

            return priceLevel;
        }
        
        public MamdaOrderBookEntry atomiclevelEntryToOrderBookEntry (MamdaBookAtomicLevelEntry levelEntry)
        {
          MamdaOrderBookEntry  entry = new MamdaOrderBookEntry ();
          
          entry.setId       (levelEntry.getPriceLevelEntryId());
          entry.setSize     ((double)levelEntry.getPriceLevelEntrySize());
          entry.setAction   (levelEntry.getPriceLevelEntryAction());
          entry.setTime     (levelEntry.getPriceLevelEntryTime());
          entry.setReason   (levelEntry.getPriceLevelEntryReason());

          return entry;
        }

        public void copy (MamdaOrderBook  book)
        {
            clear();
            mIsConsistent      = book.getIsConsistent();
            mCheckVisibility   = book.getCheckSourceState();
            mNeedsReevaluation = book.getNeedsReevaluation();
            mSymbol            = book.getSymbol();
    
            apply(book);
        }
        
        public long getTotalNumLevels ()
        {
            return mBidLevels.size() + mAskLevels.size();
        }
        
        public int getNumBidLevels ()
        {
            return mBidLevels.size ();
        }
        
        public int getNumAskLevels ()
        {
            return mAskLevels.size ();
        }
        MamdaOrderBookPriceLevel getBidMarketOrders ()
        {
            return mBidMarketOrders;
        }

        MamdaOrderBookPriceLevel getAskMarketOrders ()
        {
            return mAskMarketOrders;
        }
        
        public MamdaOrderBookPriceLevel getLevelAtPrice (double price, char side)
        {
            MamdaOrderBookPriceLevel found = null;
            tmpPrice.setValue (price);
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
            {
                found = (MamdaOrderBookPriceLevel)mBidLevels.get(tmpPrice);
    
            }
            else if (MamdaOrderBookPriceLevel.SIDE_ASK == side)
            {
                found = (MamdaOrderBookPriceLevel)mAskLevels.get(tmpPrice);
            }
            return found;
        }
        
        public MamdaOrderBookPriceLevel getLevelAtPosition (long pos, char side)
        {
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
            {
                long  i = 0;
                Iterator iter = bidIterator ();
                while (iter.hasNext ())
                {
                    if (i++ == pos)
                        return (MamdaOrderBookPriceLevel) iter.next ();
                    else
                        iter.next ();
                }
            }
            else
            {
                long  i = 0;
                Iterator iter = askIterator ();
                while (iter.hasNext ())
                {
                    if (i++ == pos)
                        return (MamdaOrderBookPriceLevel) iter.next ();
                    else
                        iter.next ();
                }
            }
            return null;
        }
    
        public MamdaOrderBookEntry getEntryAtPosition (long pos, char side)
        {
            // Remember: pos may be zero, which would mean we want the first
            // entry in a non-empty price level.
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
            {
                Iterator iter = bidIterator ();
                while (iter.hasNext ())
                {
                    MamdaOrderBookPriceLevel level =
                        (MamdaOrderBookPriceLevel) iter.next ();
                    double numEntries = level.getNumEntries();
                    if (pos >= numEntries)
                    {
                        pos -= numEntries;
                        // ... and continue to next level.
                    }
                    else
                    {
                        return level.getEntryAtPosition (pos);
                    }
                }
            }
            else
            {
                Iterator iter = askIterator ();
                while (iter.hasNext ())
                {
                    MamdaOrderBookPriceLevel level =
                        (MamdaOrderBookPriceLevel) iter.next ();
                    double numEntries = level.getNumEntries();
                    if (pos >= numEntries)
                    {
                        pos -= numEntries;
                        // ... and continue to next level.
                    }
                    else
                    {
                        return level.getEntryAtPosition (pos);
                    }
                }
            }
            return null;
        }
        
        private void applySide (TreeMap bookSide, Iterator i)
        {
            while (i.hasNext ())
            {
                MamdaOrderBookPriceLevel level =
                    (MamdaOrderBookPriceLevel) i.next ();
                try
                {
                    switch (level.getAction())
                    {
                        case MamdaOrderBookPriceLevel.ACTION_ADD:
                            addLevelSide (bookSide, level);
                            break;
                        case MamdaOrderBookPriceLevel.ACTION_UPDATE:
                            applyLevelSide (bookSide, level);
                            break;
                        case MamdaOrderBookPriceLevel.ACTION_DELETE:
                            deleteLevelSide (bookSide, level);
                            break;
                        case MamdaOrderBookPriceLevel.ACTION_UNKNOWN:
                            // explicitly not handled
                            break;
                    }
                }
                catch (MamdaOrderBookException e)
                {
                    StringBuffer errMsg = new StringBuffer (256);
                    errMsg.append (e.getMessage ()).append (" (price=").
                           append (level.getPrice ()).append (")");
    
                    throw new MamdaOrderBookException (errMsg.toString ());
                }
            }
        }

        private void applyMarketOrderSide (char side, MamdaOrderBookPriceLevel level)
        {
            try
            {
                switch (level.getAction())
                {
                    case MamdaOrderBookPriceLevel.ACTION_ADD:
                        addMarketOrderLevelSide (side, level);
                        break;
                    case MamdaOrderBookPriceLevel.ACTION_UPDATE:
                        applyMarketOrderLevelSide (side, level);
                        break;
                    case MamdaOrderBookPriceLevel.ACTION_DELETE:
                        deleteMarketOrderLevelSide (side, level);
                        break;
                    case MamdaOrderBookPriceLevel.ACTION_UNKNOWN:
                        // explicitly not handled
                        break;
                }
            }
            catch (MamdaOrderBookException e)
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append (e.getMessage ()).append (" Market Order (price=").
                       append (level.getPrice ()).append (")");
    
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }
          
        private void applyMarketOrderSide (char                     side, 
                                           MamdaOrderBookPriceLevel level,
                                           MamdaOrderBookBasicDelta delta)
        {
            try
            {
                switch (delta.getPlDeltaAction())
                {
                    case MamdaOrderBookPriceLevel.ACTION_ADD:
                        addMarketOrderLevelSide (side, level);
                        break;
                    case MamdaOrderBookPriceLevel.ACTION_UPDATE:
                        applyMarketOrderLevelSide (side, level, delta);
                        break;
                    case MamdaOrderBookPriceLevel.ACTION_DELETE:
                        deleteMarketOrderLevelSide (side, level);
                        break;
                    case MamdaOrderBookPriceLevel.ACTION_UNKNOWN:
                        // explicitly not handled
                        break;
                }
            }
            catch (MamdaOrderBookException e)
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append (e.getMessage ()).append (" Market Order (price=").
                       append (level.getPrice ()).append (")");

                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }
        
        private void addLevelSide (TreeMap bookSide, MamdaBookAtomicLevel level)
        {
            mPrice.setValue(level.getPriceLevelPrice());

            if (!bookSide.containsKey(mPrice))
            {
                MamdaOrderBookPriceLevel levelcopy;
                levelcopy = atomicUpdateToOrderBookPriceLevel(level);

                levelcopy.setOrderBook(parent);
                bookSide.put (levelcopy.getPrice(), levelcopy);
            }
            else
            {
                updateLevelSide (bookSide, level);
                mLogger.info ("addLevelSide: Atomic Level :warning: " + level.getPriceLevelPrice() +
                              " already exists in book\n");
                throw new MamdaOrderBookException ("addLevelSide: Atomic Level :warning: " +
                                                   level.getPriceLevelPrice() +
                                                   " already exists in book\n");
            }
        }
    
        
        private void addLevelSide (TreeMap bookSide, MamdaOrderBookPriceLevel level)
        {
            MamaPrice price = level.getPrice();
            if (!bookSide.containsKey(price))
            {
                MamdaOrderBookPriceLevel levelCopy;
    
                // Only copy if alread associated with a book.
                if (level.getOrderBook () != null)
                    levelCopy = new MamdaOrderBookPriceLevel (level);
                else
                    levelCopy = level;
    
                levelCopy.setOrderBook (parent);
                bookSide.put (price, levelCopy);

                if (mGenerateDeltas)
                {
                    addDelta (null, levelCopy, levelCopy.getSizeChange(), 
                              MamdaOrderBookPriceLevel.ACTION_ADD, 
                              MamdaOrderBookEntry.ACTION_UNKNOWN);
                }
            }
            else
            {
                updateLevelSide (bookSide, level);
                mLogger.info ("addLevelSide: warning: " + price +
                              " already exists in book\n");
                throw new MamdaOrderBookException ("addLevelSide: warning: " +
                                                   price +
                                                   " already exists in book\n");
            }
        }

        private void addMarketOrderLevelSide (char side, MamdaOrderBookPriceLevel level)
        {

            MamdaOrderBookPriceLevel bookLevel = getMarketOrdersSide(side);
            if (bookLevel == null)
            {
                bookLevel = new MamdaOrderBookPriceLevel (level);
                bookLevel.setOrderBook(parent);
                if (MamdaOrderBookPriceLevel.SIDE_BID == side)
                    mBidMarketOrders = bookLevel;
                else if (MamdaOrderBookPriceLevel.SIDE_ASK == side)
                    mAskMarketOrders = bookLevel;
                else
                {
                    StringBuffer errMsg = new StringBuffer (1000);
                    errMsg.append (" MamdaOrderBook::addMarketOrderSide():").
                           append("invalid side provided: ").append (side);

                    throw new MamdaOrderBookException (errMsg.toString ());
                }
            }
            else
            {
                updateMarketOrderLevelSide (side, level);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("addMarketOrderLevelSide(").append (mSymbol).append ("): side=").
                       append (side).append ("already exists in book (size=").
                       append (level.getSize ());
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        private void addLevelSide (TreeMap bookSide, MamdaOrderBookBasicDelta delta)
        {
            MamdaOrderBookPriceLevel level = delta.getPriceLevel();
            MamaPrice price = level.getPrice();
            MamdaOrderBookPriceLevel found =
                (MamdaOrderBookPriceLevel) bookSide.get(price);
            if (found == null)
            {
                // Only copy if associated with book.
                MamdaOrderBookPriceLevel levelCopy;
                if (level.getOrderBook () != null)
                    levelCopy = new MamdaOrderBookPriceLevel (level);
                else
                    levelCopy = level;
                bookSide.put (price, levelCopy);
                levelCopy.setOrderBook(parent);

                if (mGenerateDeltas)
                {
                    addDelta (delta.getEntry(), levelCopy, levelCopy.getSizeChange(), 
                              MamdaOrderBookPriceLevel.ACTION_ADD, 
                              MamdaOrderBookEntry.ACTION_UNKNOWN);
                }

            }
            else
            {
                updateLevelSide (bookSide, delta);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("addLevel(").append (mSymbol).append ("): price=").
                       append (price).append ("already exists in book (size=").
                       append (found.getSize ());
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        public void addLevelSideNoCopy (TreeMap                   bookSide,
                                        MamdaOrderBookPriceLevel  level)
        {
            MamaPrice price = level.getPrice();
            if (!bookSide.containsKey(price))
            {
                bookSide.put (level.getPrice(),level);            
                if (mGenerateDeltas)
                {
                    addDelta (null, level, level.getSizeChange(), 
                              MamdaOrderBookPriceLevel.ACTION_ADD, 
                              MamdaOrderBookEntry.ACTION_UNKNOWN);
                }  
            }
            else 
            {
                updateLevelSide (bookSide, level);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("addLevel(").append (mSymbol).
                       append ("): price=").append (price).
                       append (" already exists in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        private void updateLevelSide (TreeMap               bookSide,
                                      MamdaBookAtomicLevel  level)
        {
            mPrice.setValue (level.getPriceLevelPrice());

            MamdaOrderBookPriceLevel found =
                (MamdaOrderBookPriceLevel) bookSide.get(mPrice);

            if (found != null)
            {
                found.copy (level);
            }
            else
            {
                addLevelSide (bookSide, level);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("Atomic updateLevel(").append (mSymbol).
                       append ("): price=").append (level.getPriceLevelPrice()).
                       append (" does not exist in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        private void updateLevelSide (TreeMap                   bookSide,
                                      MamdaOrderBookPriceLevel  level)
        {
            MamaPrice price = level.getPrice();
            MamdaOrderBookPriceLevel found =
                (MamdaOrderBookPriceLevel) bookSide.get(price);
            if (found != null)
            {
                /* Note: If the update does not contain all of the entries,
                 * then we should have used applyLevelSide instead! */
                found.copy (level);
                if (mGenerateDeltas)
                {
                    addDelta (null, level, level.getSizeChange(), 
                              MamdaOrderBookPriceLevel.ACTION_UPDATE, 
                              MamdaOrderBookEntry.ACTION_UNKNOWN);
                }
            }
            else
            {
                addLevelSide (bookSide, level);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("updateLevel(").append (mSymbol).
                       append ("): price=").append (price).
                       append (" does not exist in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        private void updateLevelSide (TreeMap                   bookSide,
                                      MamdaOrderBookBasicDelta  delta)
        {
            MamdaOrderBookPriceLevel level = delta.getPriceLevel();
            MamaPrice price = level.getPrice();
            MamdaOrderBookPriceLevel found =
                (MamdaOrderBookPriceLevel) bookSide.get(price);
            if (found != null)
            {
                /* Note: If the update does not contain all of the entries,
                 * then we should have used applyLevelSide instead! */
                found.copy (level);
                found.setOrderBook (parent);
                if (mGenerateDeltas)
                {
                    addDelta (delta.getEntry(),delta.getPriceLevel(), level.getSizeChange(),
                              MamdaOrderBookPriceLevel.ACTION_UPDATE, 
                              MamdaOrderBookEntry.ACTION_UNKNOWN);
                }
            }
            else
            {
                addLevelSide (bookSide, delta);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("updateLevel(").append(mSymbol).append("): price=").
                       append (price);
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        private void updateMarketOrderLevelSide(char side, MamdaOrderBookPriceLevel  level)
        {
            MamdaOrderBookPriceLevel existingLevel = getMarketOrdersSide (side);
            if (existingLevel != null)
            {
                /* Note: If the update does not contain all of the entries,
                 * then we should have used applyLevelSide instead! */
                existingLevel.copy (level);
                existingLevel.setOrderBook (parent);
            }
            else
            {
                addMarketOrderLevelSide (side, level);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("updateMarketOrderLevelSidel()").append (mSymbol).
                       append ("): side=").append (side).
                       append (" does not exist in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        private void applyLevelSide (TreeMap               bookSide,
                                     MamdaBookAtomicLevel  level)
        {          
            MamaPrice price = level.getPriceLevelMamaPrice();

            MamdaOrderBookPriceLevel found =
                (MamdaOrderBookPriceLevel)bookSide.get(price);
            if (found != null)
            {
                Iterator i = level.entryIterator ();
                while (i.hasNext ())
                {
                    MamdaOrderBookEntry entry =
                    new MamdaOrderBookEntry((MamdaOrderBookEntry) i.next ());
                    switch (entry.getAction())
                    {
                        case MamdaOrderBookEntry.ACTION_ADD:
                            found.addEntry (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_UPDATE:
                            found.updateEntry (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_DELETE:
                            found.removeEntryById (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_UNKNOWN:
                            // explicitly not handled
                            break;
                    }
                }
                found.setDetails (level);
            }
            else
            {
                addLevelSide (bookSide, level);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("Atomic applyLevel()").append (mSymbol).
                       append ("): price=").append (price).
                       append (" does not exist in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }
        
        private void applyLevelSide (TreeMap                   bookSide,
                                     MamdaOrderBookPriceLevel  level)
        {
            MamaPrice price = level.getPrice();
            MamdaOrderBookPriceLevel found =
                (MamdaOrderBookPriceLevel) bookSide.get(price);
            if (found != null)
            {
                Iterator i = level.entryIterator ();
                while (i.hasNext ())
                {
                    MamdaOrderBookEntry entry =
                        new MamdaOrderBookEntry ((MamdaOrderBookEntry) i.next ());
                    switch (entry.getAction())
                    {
                        case MamdaOrderBookEntry.ACTION_ADD:
                            found.addEntry (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_UPDATE:
                            found.updateEntry (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_DELETE:
                            found.removeEntryById (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_UNKNOWN:
                            // explicitly not handled
                            break;
                    }
                }
                found.setDetails (level);
            }
            else
            {
                addLevelSide (bookSide, level);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("applyLevel(").append (mSymbol).
                       append ("): price=").append (price).
                       append (" does not exist in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        private void applyLevelSide (TreeMap                   bookSide,
                                     MamdaOrderBookBasicDelta  delta)
        {
            MamdaOrderBookPriceLevel level = delta.getPriceLevel();
            MamaPrice price = level.getPrice();
            MamdaOrderBookPriceLevel found =
                (MamdaOrderBookPriceLevel) bookSide.get(price);
            if (found != null)
            {
                MamdaOrderBookEntry entry = delta.getEntry();
                if (entry != null)
                {
                    switch (delta.getEntryDeltaAction())
                    {
                        case MamdaOrderBookEntry.ACTION_ADD:
                            found.addEntry (new MamdaOrderBookEntry(entry));
                            break;
                        case MamdaOrderBookEntry.ACTION_UPDATE:
                            found.updateEntry (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_DELETE:
                            found.removeEntryById (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_UNKNOWN:
                            // explicitly not handled
                            break;
                    }
                }
                found.setDetails (level);
            }
            else
            {
                addLevelSide (bookSide, delta);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("applyLevel(").append (mSymbol).append ("): price=").
                       append (price).append (" does not exist in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        private void applyMarketOrderLevelSide (char side, MamdaOrderBookPriceLevel level)
        {
            MamdaOrderBookPriceLevel existingLevel = getMarketOrdersSide (side);
            if (null != existingLevel)
            {
                Iterator i = level.entryIterator ();
                while (i.hasNext ())
                {
                    MamdaOrderBookEntry entry = 
                        new MamdaOrderBookEntry((MamdaOrderBookEntry) i.next ());
                    switch (entry.getAction())
                    {
                        case MamdaOrderBookEntry.ACTION_ADD:
                            existingLevel.addEntry (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_UPDATE:
                            existingLevel.updateEntry (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_DELETE:
                            existingLevel.removeEntryById (entry);
                            break;
                        case MamdaOrderBookEntry.ACTION_UNKNOWN:
                            // explicitly not handled
                            break;
                    }
                }
                existingLevel.setDetails (level);
            }
            else
            {
                addMarketOrderLevelSide (side, level);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("applyMarketOrderLevelSide(").append (mSymbol).
                       append ("): side=").append (side).
                       append (" does not exist in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }
            
        private void applyMarketOrderLevelSide (char                      side,
                                                MamdaOrderBookPriceLevel  level,
                                                MamdaOrderBookBasicDelta  delta)
        {
            MamdaOrderBookPriceLevel existingLevel = getMarketOrdersSide (side);
            if (null != existingLevel)
            {
                MamdaOrderBookEntry entry = delta.getEntry();

                switch (delta.getEntryDeltaAction())
                {
                    case MamdaOrderBookEntry.ACTION_ADD:
                        existingLevel.addEntry (new MamdaOrderBookEntry(entry));
                        break;
                    case MamdaOrderBookEntry.ACTION_UPDATE:
                        existingLevel.updateEntry (entry);
                        break;
                    case MamdaOrderBookEntry.ACTION_DELETE:
                        existingLevel.removeEntryById (entry);
                        break;
                    case MamdaOrderBookEntry.ACTION_UNKNOWN:
                        // explicitly not handled
                        break;
                }
                existingLevel.setDetails (level);
            }
            else
            {
                addMarketOrderLevelSide (side, level);
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("applyMarketOrderLevelSide(").append (mSymbol).
                       append ("): side=").append (side).
                       append (" does not exist in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }
            
        public void addEntry (MamdaBookAtomicLevelEntry  levelEntry)
        {
            MamdaOrderBookEntry  entry = atomiclevelEntryToOrderBookEntry(levelEntry);

            long                      entrySize   = levelEntry.getPriceLevelEntrySize();
            double                    price       = levelEntry.getPriceLevelPrice();
            char                      side        = levelEntry.getPriceLevelSide();
            MamaDateTime              eventTime   = levelEntry.getPriceLevelEntryTime();
            MamdaOrderBookBasicDelta  delta       = null;

            addEntry (entry, price, side, eventTime, delta);
        }

        
        public void addEntry (MamdaOrderBookEntry            entry,
                              double                         price,
                              char                           side,
                              MamaDateTime                   eventTime,
                              MamdaOrderBookBasicDelta       delta)
        {
            if (side == MamdaOrderBookPriceLevel.SIDE_BID)
                addEntry (mBidLevels, entry, price, side, eventTime, delta);
            else if (side == MamdaOrderBookPriceLevel.SIDE_ASK)
                addEntry (mAskLevels, entry, price, side, eventTime, delta);
        }
        
        private void deleteLevelSide (TreeMap              bookSide,
                                      MamdaBookAtomicLevel level)
        {
            if (bookSide.containsKey(level.getPriceLevelMamaPrice()))
            {
                bookSide.remove (level.getPriceLevelMamaPrice());
            }
            else
            {
                mLogger.info ("Atomic deleteLevelSide: warning: " + level.getPriceLevelMamaPrice() +
                              " does not exist in book\n");
            }
        }
        
        private void deleteLevelSide (TreeMap                  bookSide,
                                      MamdaOrderBookPriceLevel level)
        {
            MamaPrice price = level.getPrice();
            if (bookSide.containsKey(price))
            {
                /* We actually need to process this properly because the
                 * update may not contain all entries, just updated
                 * ones. */
                bookSide.remove (price);
                if (mGenerateDeltas)
                {
                    addDelta (null, level, level.getSizeChange(), 
                              MamdaOrderBookPriceLevel.ACTION_DELETE,
                              MamdaOrderBookEntry.ACTION_UNKNOWN);
                }
            }
            else
            {
                mLogger.info ("deleteLevelSide: warning: " + price +
                              " does not exist in book\n");
            }
        }
    
        private void deleteLevelSide (TreeMap                   bookSide,
                                      MamdaOrderBookBasicDelta  delta)
        {
            MamdaOrderBookPriceLevel level = delta.getPriceLevel();
            MamaPrice                price = level.getPrice();
            MamdaOrderBookPriceLevel found =
                (MamdaOrderBookPriceLevel) bookSide.get(price);

            if (found != null)
            {
                bookSide.remove(price);
                if (mGenerateDeltas)
                {
                    addDelta (delta.getEntry(), delta.getPriceLevel(), level.getSizeChange(),
                              MamdaOrderBookPriceLevel.ACTION_DELETE,
                              delta.getEntry().getAction());
                }
            }
            else
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("deleteLevel(").append (mSymbol).
                       append ("): price=").append (price).
                       append (" does not exist in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        private void deleteMarketOrderLevelSide (char side, MamdaOrderBookPriceLevel level)
        {
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
            {
                detach(level);
                return;
            }
            if (MamdaOrderBookPriceLevel.SIDE_ASK == side)
            {
                detach(level);
                return;
            }
            else
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("deleteMarketOrderLevelSide: warning:").append (mSymbol).
                       append ("): side=").append (side).
                       append (" does not exist in book");
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        private void markAllDeleted (TreeMap  bookSide)
        {
            Iterator iter = bookSide.values().iterator();
            while (iter.hasNext())
            {
                MamdaOrderBookPriceLevel level =
                    (MamdaOrderBookPriceLevel)iter.next();
                level.markAllDeleted();
            }
        }
        
        private TreeMap determineDiffs (TreeMap  resultSide,
                                        TreeMap  lhs,
                                        TreeMap  rhs)
        {
            Iterator lhsIter = lhs.values().iterator();
            Iterator rhsIter = rhs.values().iterator();
            while (lhsIter.hasNext() || rhsIter.hasNext())
            {
                MamdaOrderBookPriceLevel lhsLevel = null;
                MamdaOrderBookPriceLevel rhsLevel = null;
                double                   lhsPrice = Double.MIN_VALUE;
                double                   rhsPrice = Double.MIN_VALUE;
                double                   lhsSize  = 0;
                double                   rhsSize  = 0;
    
                if (lhsIter.hasNext())
                {
                    lhsLevel = (MamdaOrderBookPriceLevel)lhsIter.next();
                    lhsPrice = lhsLevel.getPrice().getValue();
                    lhsSize  = lhsLevel.getSize();
                }

                if (rhsIter.hasNext())
                {
                    rhsLevel = (MamdaOrderBookPriceLevel)rhsIter.next();
                    rhsPrice = rhsLevel.getPrice().getValue();
                    rhsSize  = rhsLevel.getSize();
                }
    
                if ((lhsPrice == rhsPrice) && (lhsSize == rhsSize))
                {
                    // Usual scenario: both levels are the same
                    continue;
                }
    
                if (lhsPrice == rhsPrice)
                {
                    // Same price, different size.  Need to determine the
                    // different entries.
                    assert lhsLevel != null;
                    assert rhsLevel != null;
                    MamdaOrderBookPriceLevel  diffLevel =
                        new MamdaOrderBookPriceLevel();
                    diffLevel.setAsDifference (lhsLevel, rhsLevel);
                    resultSide.put (lhsLevel.getPrice(), diffLevel);
                    continue;
                }
    
                if (((lhsPrice > rhsPrice) && (rhsPrice != Double.MIN_VALUE)) ||
                    (lhsPrice == Double.MIN_VALUE))
                {
                    // RHS has an additional price level
                    MamdaOrderBookPriceLevel  diffLevel =
                        new MamdaOrderBookPriceLevel(rhsLevel);
                    diffLevel.setOrderBook (parent);
                    resultSide.put (rhsLevel.getPrice(), diffLevel);
                }
                else
                {
                    // RHS does not have a price level that is on the LHS.
                    // Copy the LHS level and mark all as deleted.
                    MamdaOrderBookPriceLevel  diffLevel =
                        new MamdaOrderBookPriceLevel(lhsLevel);
                    diffLevel.setOrderBook (parent);
                    resultSide.put (lhsLevel.getPrice(), diffLevel);
                }
            }
            return resultSide;
        }
        
        private MamdaOrderBookPriceLevel findOrCreateLevel (TreeMap   bookSide,
                                                            double    price,
                                                            char      side,
                                                            MamaChar  plAction)
        {
            tmpPrice.setValue (price);

            MamdaOrderBookPriceLevel found =
                (MamdaOrderBookPriceLevel) bookSide.get (tmpPrice);
            if (found == null)
            {
                MamdaOrderBookPriceLevel  level =
                    new MamdaOrderBookPriceLevel (price, side);
                bookSide.put (level.getPrice(),level);
                level.setOrderBook (parent);
                plAction.setValue (MamdaOrderBookPriceLevel.ACTION_ADD);
                if (mGenerateDeltas)
                {
                    addDelta (null, level, 0.0, 
                              MamdaOrderBookPriceLevel.ACTION_ADD, 
                              MamdaOrderBookEntry.ACTION_UNKNOWN);
                }
                return level;
            }
            else
            {
                /* The level may exist but could be empty, so the plAction may
                 * be ADD or UPDATE. */
                plAction.setValue ((found.getNumEntries() == 0) ?
                    MamdaOrderBookPriceLevel.ACTION_ADD :
                    MamdaOrderBookPriceLevel.ACTION_UPDATE);
                return found;
            }
        }

        private MamdaOrderBookPriceLevel findLevel (TreeMap  bookSide,
                                                    double   price,
                                                    char     side)
        {
            tmpPrice.setValue (price);
            MamdaOrderBookPriceLevel found = null;
            found = (MamdaOrderBookPriceLevel) bookSide.get (tmpPrice);
            return found;
        }

        private void addEntry (TreeMap                   bookSide, 
                               MamdaOrderBookEntry       entry,
                               double                    price, 
                               char                      side,
                               MamaDateTime              eventTime,
                               MamdaOrderBookBasicDelta  delta)
        {
            MamaChar plAction = new MamaChar ();
            MamdaOrderBookPriceLevel level =
                findOrCreateLevel (bookSide, price, side, plAction);
            double plSizeDelta = 0.0;
            if (!mCheckVisibility || entry.isVisible())
            {
                plSizeDelta = entry.getSize();
                level.setTime (eventTime);
            }
            level.setOrderBook(parent);
            level.addEntry (entry);
    
            if (delta != null)
            {
                delta.set (entry, level, plSizeDelta, plAction.getValue (),
                           MamdaOrderBookEntry.ACTION_ADD);
            }
        }
      
        public void updateEntry (MamdaBookAtomicLevelEntry levelEntry)
        {
            double price = levelEntry.getPriceLevelPrice();
            char   side  = levelEntry.getPriceLevelSide();
          
            MamdaOrderBookPriceLevel level = getLevelAtPrice(price, side);

            if (level == null)
            {
                throw new MamdaOrderBookInvalidEntryException (levelEntry,
                        "MamdaOrderBook::updateEntry()");
            }
          
            if (level.getTime() != levelEntry.getPriceLevelEntryTime())
            {     
                // For some reason, we can get updates that do not change the
                // size and so we also don't want to change the time.
                if (!mCheckVisibility || levelEntry.isVisible())
                {
                    level.setTime (levelEntry.getPriceLevelEntryTime());
                }
            }
            level.updateEntry(levelEntry);
        }
        
        public void updateEntry (MamdaOrderBookEntry       entry,
                                 double                    size,
                                 MamaDateTime              eventTime,
                                 MamdaOrderBookBasicDelta  delta)
        {
            MamdaOrderBookPriceLevel level = entry.getPriceLevel();

            if (level == null)
            {
                throw new MamdaOrderBookInvalidEntryException (entry,
                        "MamdaOrderBook::updateEntry()");
            }

            double plSizeDelta = 0.0;
            if (size != entry.getSize())
            {
                // For some reason, we can get updates that do not change the
                // size and so we also don't want to change the time.
                if (!mCheckVisibility || entry.isVisible())
                {
                    plSizeDelta = size - entry.getSize();
                    
                    level.setSize ((long) (level.getSize() + plSizeDelta));
                    level.setTime (eventTime);
                }
                entry.setSize ((long) size);
                entry.setTime (eventTime);
            }
    
            if (delta != null)
            {
                delta.set (entry, level, plSizeDelta,
                           MamdaOrderBookPriceLevel.ACTION_UPDATE,
                           MamdaOrderBookEntry.ACTION_UPDATE);
            }
            if (mGenerateDeltas)
            {
                addDelta (entry, level, plSizeDelta, 
                          MamdaOrderBookPriceLevel.ACTION_UPDATE, 
                          MamdaOrderBookEntry.ACTION_UPDATE);
            }  

        }
        
        public void deleteEntry (MamdaBookAtomicLevelEntry levelEntry)
        {          
            double price = levelEntry.getPriceLevelPrice();
            char side = levelEntry.getPriceLevelSide();
           
            MamdaOrderBookPriceLevel level = getLevelAtPrice(price, side);
            if (level == null) /*this may occur when a previous Level update with action delete removes the level
                                and the levelEntry  */
            {
                return;
            }
            else
            {
                if (level.getTime() != levelEntry.getPriceLevelEntryTime())
                { 
                    if (!mCheckVisibility || levelEntry.isVisible())
                    {
                        level.setTime (levelEntry.getPriceLevelEntryTime());
                    }
                }
                level.removeEntry (levelEntry);
                
                if (level.empty())
                {
                    detach (level);
                }
            }
        }

        public void deleteEntry (MamdaOrderBookEntry       entry,
                                 MamaDateTime              eventTime,
                                 MamdaOrderBookBasicDelta  delta)
        {
            MamdaOrderBookPriceLevel level = entry.getPriceLevel();

            if (level == null)
                throw new MamdaOrderBookInvalidEntryException (entry,
                        "MamdaOrderBook::deleteEntry()");

            double plSizeDelta = 0.0;

            if (!mCheckVisibility || entry.isVisible())
            {
                plSizeDelta = -entry.getSize();
                level.setTime (eventTime);
            }
            level.removeEntry (entry);

            if (level.empty())
            {
                detach (level);
            }

            char plAction = MamdaOrderBookPriceLevel.ACTION_UPDATE;

            if (delta != null)
            {
                if (level.getSize() == 0)
                {
                    plAction = MamdaOrderBookPriceLevel.ACTION_DELETE;
                }
                delta.set (entry, level, plSizeDelta, plAction,
                           MamdaOrderBookEntry.ACTION_DELETE);
            }

            if ((!level.empty()) && (mGenerateDeltas))
            {
                addDelta (entry, level, plSizeDelta, plAction, 
                          MamdaOrderBookEntry.ACTION_DELETE);
            } 
        }
        
        
        public void addEntriesFromBook (MamdaOrderBook                 book,
                                        MamdaOrderBookEntryFilter      filter,
                                        MamdaOrderBookBasicDeltaList   delta)
        {
            MamaChar  ignored = new MamaChar ();

            Iterator bidIter = mBidLevels.values ().iterator ();
            while (bidIter.hasNext ())
            {
                MamdaOrderBookPriceLevel bookLevel =
                    (MamdaOrderBookPriceLevel) bidIter.next ();

                MamdaOrderBookPriceLevel level = findOrCreateLevel (
                    mBidLevels, bookLevel.getPrice().getValue (),
                    bookLevel.getSide(), ignored);

                level.addEntriesFromLevel (bookLevel, filter, delta);
            }
    
            Iterator askIter = mAskLevels.values ().iterator ();
            while (askIter.hasNext ())
            {
                MamdaOrderBookPriceLevel bookLevel =
                    (MamdaOrderBookPriceLevel) askIter.next ();

                MamdaOrderBookPriceLevel level = findOrCreateLevel (
                    mAskLevels, bookLevel.getPrice().getValue (),
                    bookLevel.getSide(), ignored);

                level.addEntriesFromLevel (bookLevel, filter, delta);
            }
    
        }
        
        public void addPriceLevelsFromBookAsEntries (MamdaOrderBook                 book,
                                                     String                         source,
                                                     MamdaOrderBookBasicDeltaList   delta)
        {
            Iterator bidIter = mBidLevels.values ().iterator ();
            while (bidIter.hasNext ())
            {
                MamdaOrderBookPriceLevel bookLevel =
                    (MamdaOrderBookPriceLevel) bidIter.next ();

                MamdaOrderBookEntry  entry =
                    new MamdaOrderBookEntry (source, bookLevel.getSize(),
                                             MamdaOrderBookEntry.ACTION_ADD,
                                             bookLevel.getTime(),
                                             book.getSourceDerivative());
                if (delta != null)
                {
                    MamdaOrderBookBasicDelta  basicDelta =
                        new MamdaOrderBookBasicDelta ();
                    addEntry (mBidLevels, entry, bookLevel.getPrice().getValue (),
                              bookLevel.getSide(), bookLevel.getTime(),
                              basicDelta);
                    delta.add (basicDelta);
                }
                else
                {
                    addEntry (mBidLevels, entry, bookLevel.getPrice().getValue (),
                              bookLevel.getSide(), bookLevel.getTime(), null);
                }
            }

            Iterator askIter = mAskLevels.values ().iterator ();
            while (askIter.hasNext ())
            {
                MamdaOrderBookPriceLevel bookLevel =
                    (MamdaOrderBookPriceLevel) askIter.next ();
                MamdaOrderBookEntry  entry =
                    new MamdaOrderBookEntry (source, bookLevel.getSize(),
                                             MamdaOrderBookEntry.ACTION_ADD,
                                             bookLevel.getTime(),
                                             book.getSourceDerivative());
                if (delta != null)
                {
                    MamdaOrderBookBasicDelta  basicDelta =
                        new MamdaOrderBookBasicDelta ();
                    addEntry (mAskLevels, entry, bookLevel.getPrice().getValue (),
                              bookLevel.getSide(), bookLevel.getTime(),
                              basicDelta);
                    delta.add (basicDelta);
                }
                else
                {
                    addEntry (mAskLevels, entry, bookLevel.getPrice().getValue (),
                              bookLevel.getSide(), bookLevel.getTime(), null);
                }
            }
        }
        
        public void deleteEntriesFromSource (
            MamaSource                     source,
            MamdaOrderBookBasicDeltaList   delta)
        {
            Iterator bidIter = mBidLevels.values ().iterator ();
            while (bidIter.hasNext ())
            {
                ((MamdaOrderBookPriceLevel)
                    bidIter.next()).deleteEntriesFromSource (source, delta);
            }

            Iterator askIter = mAskLevels.values ().iterator ();
            while (askIter.hasNext ())
            {
                ((MamdaOrderBookPriceLevel)
                    askIter.next ()).deleteEntriesFromSource (source, delta);
            }
        }
        
        public void detach (MamdaOrderBookPriceLevel level)
        {
            // Remove the level from the relevant side
            if (level.getOrderType() == MamdaOrderBookPriceLevel.LEVEL_MARKET)
            {
                switch (level.getSide())
                {
                    case MamdaOrderBookPriceLevel.SIDE_BID:
                    {
                        level  = null;
                        if (mBidMarketOrders != null)
                        {
                            mBidMarketOrders.clear();
                            mBidMarketOrders = null;
                        }
                        return;
                    }

                    case MamdaOrderBookPriceLevel.SIDE_ASK:
                    {
                        level  = null;
                        if (mAskMarketOrders != null)
                        {
                            mAskMarketOrders.clear();
                            mAskMarketOrders = null;
                        }
                        return;
                    }

                    case MamdaOrderBookPriceLevel.SIDE_UNKNOWN:
                    {
                        StringBuffer msg = new StringBuffer (1000);
                        String sourceId = "none";
                        if (mSourceDeriv != null)
                            sourceId = mSourceDeriv.getBaseSource().getDisplayId();

                        msg.append ("MamdaOrderBook.detach(").append (sourceId).
                            append (":").append (mSymbol).append (") attempted to ").
                            append ("detach MARKET level with unknown side!");
                        //throw MamdaOrderBookException(msg);
                        System.out.println (msg.toString ());
                    }
                }
            }

            MamaPrice price = level.getPrice();
            switch (level.getSide())
            {
                case MamdaOrderBookPriceLevel.SIDE_BID:
                {
                    Object found = mBidLevels.remove (price);
                    if (found == null)
                    {
                        StringBuffer msg = new StringBuffer (1000);
                        String sourceId = "none";
                        if (mSourceDeriv != null)
                            sourceId = mSourceDeriv.getBaseSource().getDisplayId();
        
                        msg.append ("MamdaOrderBook.detach(").append (sourceId).
                            append (":").append (mSymbol).append (") attempted to ").
                            append ("detach price level ").append (price).
                            append (" (bid) which does not exist in the book!");

                        System.out.println (msg.toString ());
                        return;
                    }
                    break;
                }
                case MamdaOrderBookPriceLevel.SIDE_ASK:
                {
                    Object found = mAskLevels.remove (price);
                    if (found == null)
                    {
                        StringBuffer msg = new StringBuffer (1000);
                        String sourceId = "none";
                        if (mSourceDeriv != null)
                            sourceId = mSourceDeriv.getBaseSource().getDisplayId();
        
                        msg.append ("MamdaOrderBook.detach(").append (sourceId).
                            append (":").append (mSymbol).append (") attempted to ").
                            append ("detach price level ").append (price).
                            append (" (ask) which does not exist in the book!");
                        //throw MamdaOrderBookException(msg);
                        System.out.println (msg.toString ());
                        return;
                    }
                    break;
                }
                case MamdaOrderBookPriceLevel.SIDE_UNKNOWN:
                {
                    StringBuffer msg = new StringBuffer (1000);
                    String sourceId = "none";
                    if (mSourceDeriv != null)
                        sourceId = mSourceDeriv.getBaseSource().getDisplayId();
        
                    msg.append ("MamdaOrderBook.detach(").append (sourceId).
                        append (":").append (mSymbol).append (") attempted to ").
                        append ("detach price level ").append (price).
                        append ("with unknown side!");
                    //throw MamdaOrderBookException(msg);
                    System.out.println (msg.toString ());
                }
            }
        }

        public void addLevel (
           MamdaOrderBookPriceLevel  level)
        {
            try
            {
                switch (level.getSide())
                {
                    case MamdaOrderBookPriceLevel.SIDE_BID:                       
                        addLevelSideNoCopy(mBidLevels, level);
                        break;
                    case MamdaOrderBookPriceLevel.SIDE_ASK:
                        addLevelSideNoCopy(mAskLevels, level);
                        break;
                    case MamdaOrderBookPriceLevel.SIDE_UNKNOWN:
                        // explicitly not handled
                        break;
                }
            }
            catch (MamdaOrderBookException e)
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append (e.getMessage ()).append (" (price=").
                       append (level.getPrice ());
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        public void updateLevel (
            MamdaOrderBookPriceLevel  level)
        {
            try
            {
                switch (level.getSide())
                {
                    case MamdaOrderBookPriceLevel.SIDE_BID:
                        updateLevelSide (mBidLevels, level);
                        break;
                    case MamdaOrderBookPriceLevel.SIDE_ASK:
                        updateLevelSide (mAskLevels, level);
                        break;
                    case MamdaOrderBookPriceLevel.SIDE_UNKNOWN:
                        // explicitly not handled
                        break;
                }
            }
            catch (MamdaOrderBookException e)
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append (e.getMessage ()).append (" (price=").
                       append (level.getPrice ());
                throw new MamdaOrderBookException (errMsg.toString ());
               
            }
        }

        public void deleteLevel (MamdaOrderBookPriceLevel  level)
        {
            /* needs a try catch*/
            try
            {
                switch (level.getSide())
                {
                    case MamdaOrderBookPriceLevel.SIDE_BID:
                        deleteLevelSide (mBidLevels, level);
                        break;
                    case MamdaOrderBookPriceLevel.SIDE_ASK:
                        deleteLevelSide (mAskLevels, level);
                        break;
                    case MamdaOrderBookPriceLevel.SIDE_UNKNOWN:
                        // explicitly not handled
                        break;
                }
            }
            catch( MamdaOrderBookException e)
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append (e.getMessage ()).append (" (price=").
                append (level.getPrice ());
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }

        public void generateDeltaMsgs(boolean publish)
        {
            mGenerateDeltas = publish;
            if (publish)
            {
                mPublishSimpleDelta  = new MamdaOrderBookConcreteSimpleDelta();
                mPublishComplexDelta = new MamdaOrderbookConcreteComplexDelta();
                mWriter              = new MamdaOrderBookWriter();
            }
        }
        
        public void addDelta (MamdaOrderBookEntry       entry,
                              MamdaOrderBookPriceLevel  level,
                              double                    plDeltaSize,
                              char                      plAction,
                              char                      entryAction)
            {
                ++mCurrentDeltaCount;
                if (1 == mCurrentDeltaCount)
                {        
                    /* This is number one, so save the "simple" delta. */
                    mPublishSimpleDelta.set (
                        entry, level, plDeltaSize, plAction, entryAction);     

                }
                else if (2 == mCurrentDeltaCount)
                {
                    /* This is number two, so copy the saved "simple" delta to the
                     * "complex" delta and add the current one. */
                    mPublishComplexDelta.clear();
                    mPublishComplexDelta.add (mPublishSimpleDelta);
                    mPublishComplexDelta.add (
                        entry, level, plDeltaSize, plAction, entryAction);   
                }
                else
                {
                    /* This is number greater than two, so add the current delta. */
                    mPublishComplexDelta.add (
                        entry, level, plDeltaSize, plAction, entryAction);
                }
            }

        private void applySide (TreeMap bookSide, MamdaOrderBookBasicDelta delta)
        {
            MamdaOrderBookPriceLevel level = delta.getPriceLevel();
            try
            {
                switch (delta.getPlDeltaAction())
                {
                    case MamdaOrderBookPriceLevel.ACTION_ADD:
                        addLevelSide (bookSide, delta);
                        break;
                    case MamdaOrderBookPriceLevel.ACTION_UPDATE:
                        applyLevelSide (bookSide, delta);
                        break;
                    case MamdaOrderBookPriceLevel.ACTION_DELETE:
                        deleteLevelSide (bookSide, delta);
                        break;
                    case MamdaOrderBookPriceLevel.ACTION_UNKNOWN:
                        // explicitly not handled
                        break;
                }
            }
            catch (MamdaOrderBookException e)
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append (e.getMessage ()).append (" (price=").
                       append (level.getPrice ());
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }
        
        private void applySide (TreeMap bookSide, MamdaBookAtomicLevel level)
        {
            try
            {
                switch (level.getPriceLevelAction())
                {
                    case MamdaOrderBookTypes.MAMDA_BOOK_ACTION_ADD:
                        addLevelSide (bookSide, level);
                        break;
                    case MamdaOrderBookTypes.MAMDA_BOOK_ACTION_UPDATE:
                        applyLevelSide (bookSide, level);
                        break;
                    case MamdaOrderBookTypes.MAMDA_BOOK_ACTION_DELETE:
                        deleteLevelSide (bookSide, level);
                        break;
                    case MamdaOrderBookTypes.MAMDA_BOOK_ACTION_UNKNOWN:
                        applyLevelSide (bookSide, level); //treated same as update
                        break;
                }
            }
            catch (MamdaOrderBookException e)
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append (e.getMessage ()).append (" (price=").
                       append (level.getPriceLevelMamaPrice ());
                throw new MamdaOrderBookException (errMsg.toString ());
            }
        }
        
        public Iterator bidIterator()
        {
            return mBidLevels.values().iterator();
        }
        
        
        public Iterator askIterator()
        {
            return mAskLevels.values().iterator();
        }
    
        
        public void setAsDeltaDifference (MamdaOrderBook  lhs,
                                          MamdaOrderBook  rhs)
        {
            clear();
            determineDiffs (mBidLevels, lhs.bidIterator(), rhs.bidIterator());
            determineDiffs (mAskLevels, lhs.askIterator(), rhs.askIterator());
        }
        
        
        private TreeMap determineDiffs (TreeMap   resultSide,
                                        Iterator  lhsIter,
                                        Iterator  rhsIter)
        {
            while (lhsIter.hasNext() || rhsIter.hasNext())
            {
                MamdaOrderBookPriceLevel lhsLevel = null;
                MamdaOrderBookPriceLevel rhsLevel = null;
                double                   lhsPrice = 0.0;
                double                   rhsPrice = 0.0;
                double                   lhsSize  = 0;
                double                   rhsSize  = 0;
    
                if (lhsIter.hasNext())
                {
                    lhsLevel = (MamdaOrderBookPriceLevel)lhsIter.next();
                    lhsPrice = lhsLevel.getPrice().getValue();
                    lhsSize  = lhsLevel.getSize();
                }

                if (rhsIter.hasNext())
                {
                    rhsLevel = (MamdaOrderBookPriceLevel)rhsIter.next();
                    rhsPrice = rhsLevel.getPrice().getValue();
                    rhsSize  = rhsLevel.getSize();
                }
    
                if ((lhsPrice == rhsPrice) && (lhsSize == rhsSize))
                {
                    // Usual scenario: both levels are the same
                    continue;
                }
    
                if (lhsPrice == rhsPrice)
                {
                    // Same price, different size.  Need to determine the
                    // different entries.
                    assert lhsLevel != null;
                    assert rhsLevel != null;
                    MamdaOrderBookPriceLevel  diffLevel =
                        new MamdaOrderBookPriceLevel();
                    diffLevel.setAsDifference (lhsLevel, rhsLevel);
                    resultSide.put (lhsLevel.getPrice(), diffLevel);
                    continue;
                }
    
                if (((lhsPrice > rhsPrice) && (rhsPrice != 0.0)) ||
                    (lhsPrice == 0.0))
                {
                    // RHS has an additional price level
                    MamdaOrderBookPriceLevel  diffLevel =
                        new MamdaOrderBookPriceLevel(rhsLevel);
                    diffLevel.setOrderBook (parent);
                    resultSide.put (rhsLevel.getPrice(), diffLevel);
                }
                else
                {
                    // RHS does not have a price level that is on the LHS.
                    // Copy the LHS level and mark all as deleted.
                    MamdaOrderBookPriceLevel  diffLevel =
                        new MamdaOrderBookPriceLevel(lhsLevel);
                    diffLevel.setOrderBook (parent);
                    resultSide.put (lhsLevel.getPrice(), diffLevel);
                }
            }
            return resultSide;
        }
        
        public void setAsDeltaDeleted (MamdaOrderBook  bookToDelete)
        {
            copy (bookToDelete);
            markAllDeleted (mBidLevels);
            markAllDeleted (mAskLevels);
        }
        
        public boolean reevaluate()
        {
            if (!mCheckVisibility)
                return false;
    
            boolean changed = false;

            Iterator bidIter = mBidLevels.values ().iterator ();
            while (bidIter.hasNext ())
            {
                MamdaOrderBookPriceLevel level =
                    ((MamdaOrderBookPriceLevel) bidIter.next ());
                changed = level.reevaluate() || changed;
            }

            Iterator askIter = mAskLevels.values ().iterator ();
            while (askIter.hasNext ())
            {
                MamdaOrderBookPriceLevel level =
                    ((MamdaOrderBookPriceLevel) askIter.next ());
                changed = level.reevaluate() || changed;
            }

            if (mBidMarketOrders != null)
                changed = mBidMarketOrders.reevaluate() || changed;

            if (mAskMarketOrders != null)
                changed = mAskMarketOrders.reevaluate() || changed;

            return changed;
        }

        public int hashCode ()
        {
            /* From Effective Java */
            int result = 17;
            result = result*37 + mSymbol.hashCode ();
            result = result*37 + mPartId.hashCode();
            result = result*37 + mBidLevels.hashCode ();
            result = result*37 + mAskLevels.hashCode ();
            return result;
        }
    
        
        private class BidCompare implements Comparator
        {
            /* Descending order prices (MamaPrice) */
            public int compare (Object o1, Object o2)
            {
                return ((MamaPrice)o2).compareTo(o1);
            }
            public boolean equals (Object o1, Object o2)
            {
                return o1.equals(o2);
            }
        }
    
        private class AskCompare implements Comparator
        {
            /* Ascending order prices (MamaPrice) */
            public int compare (Object o1, Object o2)
            {
                return ((MamaPrice)o1).compareTo(o2);
            }
            public boolean equals (Object o1, Object o2)
            {
                return o1.equals(o2);
            }
        }
    }
    
    private class MamdaOrderBookCopy extends MamdaOrderBookImpl
    {
        private ArrayList     mAskLevels;
        private ArrayList     mBidLevels;
        private BidCompare    bidCompare    = new BidCompare();
        private AskCompare    askCompare    = new AskCompare();
        
        public MamdaOrderBookCopy()
        {
            mAskLevels = new ArrayList();
            mBidLevels = new ArrayList();
        }
        
        public MamdaOrderBookCopy (MamdaOrderBook copy)
        {
            mAskLevels = new ArrayList();
            mBidLevels = new ArrayList();
            copy(copy);
        }
        
        public boolean isReadOnly()
        {
            return true;
        }
        
        public void copy (MamdaOrderBook book)
        {
            clear();
            mIsConsistent      = book.getIsConsistent();
            mCheckVisibility   = book.getCheckSourceState();
            mNeedsReevaluation = book.getNeedsReevaluation();

            addBidSide(book.bidIterator());
            addAskSide(book.askIterator());
        }
        
        public void clear ()
        {
            mIsConsistent      = true;
            mNeedsReevaluation = false;
            mBidLevels.clear();
            mAskLevels.clear();

            if (mBidMarketOrders != null)
            {
                mBidMarketOrders.clear();
                mBidMarketOrders = null;
            }

            if (mAskMarketOrders != null)
            {
            	mAskMarketOrders.clear();
            	mAskMarketOrders = null;
            }
        }
        
        
        public MamdaOrderBookPriceLevel findOrCreateLevel (double  price,
                                                           char    side)
        {
            MamaChar ignored = new MamaChar ();

            if (side == MamdaOrderBookPriceLevel.SIDE_BID)
                return findOrCreateLevel (mBidLevels, price, side, ignored);
            else if (side == MamdaOrderBookPriceLevel.SIDE_ASK)
                return findOrCreateLevel (mAskLevels, price, side, ignored);
            else
            {
                StringBuffer msg = new StringBuffer (255);
                msg.append ("MamdaOrderBookCopy::findOrCreateLevel(): ").
                    append ("invalid side provided: ").
                    append ("side");
                throw new MamdaOrderBookException(msg.toString ());
            }
        }

        public MamdaOrderBookPriceLevel findLevel (double   price,
                                                   char     side)
        {
            if (side == MamdaOrderBookPriceLevel.SIDE_BID)
                return findLevel (mBidLevels, price, side);
            else if (side == MamdaOrderBookPriceLevel.SIDE_ASK)
                return findLevel (mAskLevels, price, side);
            else
            {
                StringBuffer msg = new StringBuffer (255);
                msg.append ("MamdaOrderBookCopy::findLevel(): ").
                    append ("invalid side provided: ").
                    append ("side");
                throw new MamdaOrderBookException(msg.toString ());
            }
        }
        
        public MamdaOrderBookPriceLevel getMarketOrdersSide (char side) 
        {
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
                return mBidMarketOrders;
            else if (MamdaOrderBookPriceLevel.SIDE_ASK== side)
                return mAskMarketOrders;
            else
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("getMarketOrdersSide(").append (mSymbol).
                       append ("): side=").append (side).
                       append (" invalid side provided");
                throw new MamdaOrderBookException (errMsg.toString ());
            } 
        }

        public MamdaOrderBookPriceLevel getOrCreateMarketOrdersSide (char side) 
        {
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
            {
                if (null == mBidMarketOrders)
                {
                    mBidMarketOrders = new MamdaOrderBookPriceLevel (
                            0.0,
                            MamdaOrderBookPriceLevel.SIDE_BID);

                    mBidMarketOrders.setOrderBook (parent);
                    mBidMarketOrders.setOrderType (
                            MamdaOrderBookPriceLevel.LEVEL_MARKET);
                }
                return mBidMarketOrders;
            }
            else if (MamdaOrderBookPriceLevel.SIDE_ASK == side)
            {
                if (null == mAskMarketOrders)
                {
                    mAskMarketOrders = new MamdaOrderBookPriceLevel (
                            0.0,
                            MamdaOrderBookPriceLevel.SIDE_ASK);

                    mAskMarketOrders.setOrderBook (parent);
                    mAskMarketOrders.setOrderType (
                            MamdaOrderBookPriceLevel.LEVEL_MARKET);
                }
                return mAskMarketOrders;
            }
            else
            {
                StringBuffer errMsg = new StringBuffer (256);
                errMsg.append ("MamdaOrderBook::getOrCreateMarketOrdersSide()(").append (mSymbol).
                       append ("): side=").append (side).
                       append (" invalid side provided");
                throw new MamdaOrderBookException (errMsg.toString ());
            } 
        }

        private MamdaOrderBookPriceLevel find  (MamaPrice  price,
                                                char       side)
        {
            int pos;
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
            {
                pos = Collections.binarySearch (mBidLevels, price, bidCompare);
                if (pos >= 0)
                    return (MamdaOrderBookPriceLevel)mBidLevels.get(pos);
                return null;
            }
            else
            {
                pos = Collections.binarySearch (mAskLevels, price, askCompare);
                if (pos >= 0)
                    return (MamdaOrderBookPriceLevel)mAskLevels.get(pos);
                return null;
            }
        }
        
        private MamdaOrderBookPriceLevel findOrCreateLevel (ArrayList  bookSide,
                                                            double     price,
                                                            char       side,
                                                            MamaChar   plAction)           
        {
            tmpPrice.setValue (price);
            MamdaOrderBookPriceLevel found = find (tmpPrice, side);
            if (found == null)
            {
                return null;
            }
            else
            {
                /* The level may exist but could be empty, so the plAction may
                * be ADD or UPDATE. */
                plAction.setValue ((found.getNumEntries() == 0) ?
                    MamdaOrderBookPriceLevel.ACTION_ADD :
                    MamdaOrderBookPriceLevel.ACTION_UPDATE);
                return found;
            }
        }

        private MamdaOrderBookPriceLevel findLevel (ArrayList  bookSide,
                                                    double     price,
                                                    char       side)           
        {
            tmpPrice.setValue (price);
            MamdaOrderBookPriceLevel found = null;
            found = find (tmpPrice, side);
            return found;
        }
        
        public void apply (MamdaOrderBook  deltaBook)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::apply : cannot be applied to read only book");
        }
        
        public void apply (MamdaBookAtomicLevelEntry levelEntry)
        {
          throw new MamdaOrderBookException (
              "MamdaOrderBookCopy::Apply (AtomicLevelEntry) : cannot be applied to book");
        }
        
        public void apply (MamdaBookAtomicLevel level)
        {
          throw new MamdaOrderBookException (
              "MamdaOrderBookCopy::Apply (AtomicLevel) : cannot be applied to book");
        }
        public void apply (MamdaOrderBookBasicDelta delta)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::apply : cannot be applied to read only book");
        }
        
        public long getTotalNumLevels ()
        {
            return mBidLevels.size() + mAskLevels.size();
        }
        
        public int getNumBidLevels ()
        {
            return mBidLevels.size ();
        }
        
        public int getNumAskLevels ()
        {
            return mAskLevels.size ();
        }

        public MamdaOrderBookPriceLevel getBidMarketOrders ()
        {
            return mBidMarketOrders;
        }

        public MamdaOrderBookPriceLevel getAskMarketOrders ()
        {
            return mAskMarketOrders;
        }

        public MamdaOrderBookPriceLevel getLevelAtPrice (double price, char side)
        {
            MamdaOrderBookPriceLevel found = null;
            tmpPrice.setValue (price);
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
            {
                found = find (tmpPrice, side);
            }
            else if (MamdaOrderBookPriceLevel.SIDE_ASK == side)
            {
                found = find (tmpPrice, side);
            }
            return found;
        }
        
        public MamdaOrderBookPriceLevel getLevelAtPosition (long pos, char side)
        {
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
            {
                return (MamdaOrderBookPriceLevel) mBidLevels.get((int)pos);
            }
            else
            {
                return (MamdaOrderBookPriceLevel) mAskLevels.get((int)pos);
            }
        }
        
        public MamdaOrderBookEntry getEntryAtPosition (long pos, char side)
        {
            // Remember: pos may be zero, which would mean we want the first
            // entry in a non-empty price level.
            if (MamdaOrderBookPriceLevel.SIDE_BID == side)
            {
                Iterator iter = bidIterator ();
                while (iter.hasNext ())
                {
                    MamdaOrderBookPriceLevel level =
                        (MamdaOrderBookPriceLevel) iter.next ();
                    double numEntries = level.getNumEntries();
                    if (pos >= numEntries)
                    {
                        pos -= numEntries;
                        // ... and continue to next level.
                    }
                    else
                    {
                        return level.getEntryAtPosition (pos);
                    }
                }
            }
            else
            {
                Iterator iter = askIterator ();
                while (iter.hasNext ())
                {
                    MamdaOrderBookPriceLevel level =
                        (MamdaOrderBookPriceLevel) iter.next ();
                    double numEntries = level.getNumEntries();
                    if (pos >= numEntries)
                    {
                        pos -= numEntries;
                        // ... and continue to next level.
                    }
                    else
                    {
                        return level.getEntryAtPosition (pos);
                    }
                }
            }
            return null;
        }
        
        public void addEntry (MamdaBookAtomicLevelEntry  levelEntry)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::addLevelEntry cannot be applied to book");
        }

        public void addLevel (MamdaOrderBookPriceLevel  level)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::addLevel cannot be applied to book");
        }        

        public void updateLevel (MamdaOrderBookPriceLevel  level)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::updateLevel cannot be applied to book");
        }
        
        public void deleteLevel (MamdaOrderBookPriceLevel  level)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::deleteLevel cannot be applied to book");
        }    
        
        public void updateEntry (MamdaBookAtomicLevelEntry  levelEntry)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::updateLevelEntry cannot be applied to book");
        }
        
        public void deleteEntry (MamdaBookAtomicLevelEntry  levelEntry)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::deleteLevelEntry cannot be applied to book");
        }
        
        public void addEntry (MamdaOrderBookEntry       entry,
                              double                    price,
                              char                      side,
                              MamaDateTime              eventTime,
                              MamdaOrderBookBasicDelta  delta)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::addEntry : cannot be applied to read only book");
        }
                
        public void updateEntry (MamdaOrderBookEntry       entry,
                                 double                    size,
                                 MamaDateTime              eventTime,
                                 MamdaOrderBookBasicDelta  delta)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::updateEntry : cannot be applied to read only book");
        }
        
        
        public void deleteEntry (MamdaOrderBookEntry       entry,
                                 MamaDateTime              eventTime,
                                 MamdaOrderBookBasicDelta  delta)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::deleteEntry : cannot be applied to read only book");
        }
        
        
        public void addEntriesFromBook (MamdaOrderBook                book,
                                        MamdaOrderBookEntryFilter     filter,
                                        MamdaOrderBookBasicDeltaList  delta)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::reevaluate : cannot be applied to read only book");
        }
        
        public void addPriceLevelsFromBookAsEntries (MamdaOrderBook                book,
                                                     String                        source,
                                                     MamdaOrderBookBasicDeltaList  delta)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::addPriceLevelsFromBookAsEntries : cannot be applied to read only book");
        }
        
        public void generateDeltaMsgs (boolean publish)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy.generateDeltaMsgs : cannot be applied to read only book");
        }

        public void addDelta (MamdaOrderBookEntry       entry,
                              MamdaOrderBookPriceLevel  level,
                              double                    plDeltaSize,
                              char                      plAction,
                              char                      entryAction)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy.addDelta : cannot be applied to read only book");
        }
        
        public void deleteEntriesFromSource (MamaSource                    source,
                                             MamdaOrderBookBasicDeltaList  delta)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::deleteEntriesFromSource : cannot be applied to read only book");
        }
        
        public void detach (MamdaOrderBookPriceLevel level)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::detach : cannot be applied to read only book");
        }
        
    
        public void setAsDeltaDifference (MamdaOrderBook  lhs,
                                          MamdaOrderBook  rhs)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::setAsDeltaDifference : cannot be applied to read only book");
        }
        
        public void setAsDeltaDeleted (MamdaOrderBook  bookToDelete)
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::setAsDeltaDeleted : cannot be applied to read only book");
        }
        
        public boolean reevaluate ()
        {
            throw new MamdaOrderBookException (
                "MamdaOrderBookCopy::reevaluate : cannot be applied to read only book");
        }

        public int hashCode ()
        {
            /* From Effective Java */
            int result = 17;
            result = result*37 + mSymbol.hashCode ();
            result = result*37 + mBidLevels.hashCode ();
            result = result*37 + mAskLevels.hashCode ();
            return result;
        }
        
        private void addBidSide (Iterator iterator)
        {
            mBidLevels.clear();
            while (iterator.hasNext())
            {
                mBidLevels.add (new MamdaOrderBookPriceLevel ((MamdaOrderBookPriceLevel) iterator.next ()));
            }
        }
        
        private void addAskSide (Iterator iterator)
        {
            mAskLevels.clear();
            while (iterator.hasNext())
            {
                mAskLevels.add (new MamdaOrderBookPriceLevel ((MamdaOrderBookPriceLevel) iterator.next ()));
            }
        }
    
        public Iterator bidIterator()
        {
            return mBidLevels.iterator();
        }
        
        public Iterator askIterator()
        {
            return mAskLevels.iterator();
        }
        
        
        private class BidCompare implements Comparator
        {
            /* Descending order prices (MamaPrice) */
            public int compare (Object o1, Object o2)
            {
                return ((MamaPrice)o2).compareTo (((MamdaOrderBookPriceLevel)o1).getPrice());
            }
            public boolean equals (Object o1, Object o2)
            {
                return o2.equals (((MamdaOrderBookPriceLevel)o1).getPrice());
            }
        }
    
        private class AskCompare implements Comparator
        {
            /* Ascending order prices (MamaPrice) */
            public int compare (Object o1, Object o2)
            {
                return ((MamdaOrderBookPriceLevel)o1).getPrice().compareTo(o2);
            }
            public boolean equals (Object o1, Object o2)
            {
                return ((MamdaOrderBookPriceLevel)o1).getPrice().equals(o2);
            }
        }
    }
}
    
