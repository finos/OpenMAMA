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

import com.wombat.mamda.*;
import com.wombat.mama.*;
import com.wombat.mamda.locks.*;

import java.util.*;
import java.util.logging.Logger;
import java.util.logging.Level;

/**
 * MamdaOrderBookListener is a class that specializes in handling
 * order book updates.  Developers provide their own implementation of
 * the MamdaOrderBookHandler interface and will be delivered
 * notifications for order book recaps and deltas.  Notifications for
 * order book deltas include the delta itself as well as the full
 * order book with deltas applied.  An obvious application for this
 * MAMDA class is any kind of program trading application that looks
 * at depth of book.
 *
 * Note: The MamdaOrderBookListener class caches the order book.
 * Among other reasons, caching of these fields makes it possible to
 * provide complete trade-related callbacks, even when the publisher
 * (e.g., feed handler) is only publishing deltas containing modified
 * fields.
 */

public class MamdaOrderBookListener
    implements MamdaMsgListener,
               MamdaOrderBookClear,
               MamdaOrderBookRecap,
               MamdaOrderBookGap
{
    private static Logger mLogger =
                   Logger.getLogger("com.wombat.mamda.MamdaOrderBookListener");

    // It might be better if the following were actually "final":
    private final MamdaOrderBook   mFullBook;
    private       MamdaLock        mFullBookLock              = new MamdaLock();
    private final LinkedList       mHandlers                  = new LinkedList();
    private final MamaLong         mEventSeqNum               = new MamaLong(0);
    private long                   mGapBegin                  = 0;
    private long                   mPrevSenderId              = 0;
    private long                   mGapEnd                    = 0;
    private boolean                mGotInitial                = false;
    private boolean                mHaveSanityCheckedBookDict = false;
    private boolean                mUpdateInconsistentBook    = false;
    private boolean                mUpdateStaleBook           = false;
    private boolean                mClearStaleBook            = true;
    private boolean                mProcessEntries            = true;
    private boolean                mProcessMarketOrders       = false;
    private boolean                mHaveEntries               = false;
    private boolean                mCalcDeltaForRecap         = false;
    long                           mCurrentDeltaCount         = 0;
    boolean                        mUniqueEntryIds            = false;
    boolean                        mMappedForSnapshot         = false;
    boolean                        mIgnoreUpdate              = false;
    private static boolean         mUpdatersComplete          = false;

    MamdaFieldState    mSymbolFieldState        = new MamdaFieldState();
    MamdaFieldState    mPartIdFieldState        = new MamdaFieldState();
    MamdaFieldState    mSrcTimeFieldState       = new MamdaFieldState ();
    MamdaFieldState    mActivityTimeFieldState  = new MamdaFieldState ();
    MamdaFieldState    mLineTimeFieldState      = new MamdaFieldState ();
    MamdaFieldState    mSendTimeFieldState      = new MamdaFieldState ();
    MamdaFieldState    mEventTimeFieldState     = new MamdaFieldState ();
    MamdaFieldState    mEventSeqNumFieldState   = new MamdaFieldState ();

    private MamdaOrderBookEntryManager  mEntryManager       = null;
    private TreeMap                     mIgnoredEntries     = new TreeMap ();
    private BookMsgFields               mBookMsgFields      = new BookMsgFields ();
    private static final HashMap        mSnapshotMap        = new HashMap (10);
    private static final MamdaLock      mSnapshotMapLock    = new MamdaLock ();
    private static BookMsgUpdate[]      mBookUpdaters       = null;
    private static BookMsgUpdate[]      mPriceLevelUpdaters = null;
    private static BookMsgUpdate[]      mEntryUpdaters      = null;
    private static final MamdaLock      mBookUpdatersLock   = new MamdaLock ();
    private StringBuffer                mUniqueIdBuffer     = new StringBuffer (64);
    
    // Can't use multiple inheritance. 
    private MamdaOrderBookSimpleDelta  mSimpleDelta             = new SimpleDeltaImpl ();
    private MamdaOrderBookComplexDelta mComplexDelta            = new ComplexDeltaImpl ();
    
    private MamdaOrderBookSimpleDelta  mSimpleMarketOrderDelta  = new SimpleDeltaImpl ();
    private MamdaOrderBookComplexDelta mComplexMarketOrderDelta = new ComplexDeltaImpl ();

    private MamaMsgField  tmpField   = new MamaMsgField();
    private MamaString    tmpString  = new MamaString();
    private MamaMessage   tmpMsg     = new MamaMessage();

    /**
     * Create an order book listener using internally created data
     * structures for the full and delta order books.
     */
    public MamdaOrderBookListener ()
    {
        this (new MamdaOrderBook());
    }

    /**
     * Create an order book listener using an optional user-provided
     * object for the full order book.  If "fullBook" is NULL, an
     * object will be allocated internally.  If this listener is
     * destroyed then the full order book object will only be
     * destroyed if it was created by the listener (i.e., if fullBook
     * was passed as NULL in this constructor).
     *
     * @param fullBook The MamdaOrderBook used to maintain the full book.
     */
    public MamdaOrderBookListener (MamdaOrderBook  fullBook)
    {
        if (fullBook == null)
        {
            mFullBook = new MamdaOrderBook ();
        }
        else
        {
            mFullBook = fullBook;
        }
    }

    /**
     * Add a specialized order book handler.  Currently, only one
     * handler can (and must) be registered.
     *
     * @param handler The hadler registered to receive order book update
     * callbacks.
     */
    public void addHandler (MamdaOrderBookHandler handler)
    {
        mHandlers.addLast(handler);
    }

    /**
     * Add an entry ID to ignore.  This only makes sense when the
     * entry type is participant ID (as opposed to order ID).  All
     * order book updates for this entry ID will be ignored.
     *
     * @param id The id of the participant to ignore when process book
     * updates.
     */
    public void addIgnoreEntryId (String  id)
    {
        mIgnoredEntries.put (id, null);
    }

    /**
     * Remove an entry ID to ignore.
     *
     * @param id The id of the participant whose update will be subsequently
     * processed as part of the book.
     *
     * @see #addIgnoreEntryId(String)
     */
    public void removeIgnoreEntryId (String  id)
    {
        mIgnoredEntries.remove (id);
    }

    /**
     * Clear all cached data fields.
     */
    public void clear ()
    {
        mBookMsgFields.clear    ();
        mEventSeqNum.setValue   (0);
        mGapBegin               = 0;
        mGapEnd                 = 0;
        mGotInitial             = false;

        mFullBookLock.acquireWriteLock();
        mFullBook.clear();
        mFullBookLock.releaseWriteLock();

        if (mEntryManager != null)
            mEntryManager.clear();
    }

    /**
     * Remove the reference of handlers from the internal list.
     * Memory is not freed.
     */
    void removeHandlers ()
    {
        mHandlers.clear ();
    }

    public String getSymbol ()
    {
        if (mFullBook != null)
            return mFullBook.getSymbol();
        else
            return "unknown-symbol";
    }

    public String getPartId ()
    {
        return "";
    }

    public MamaDateTime getSrcTime()
    {
        return mBookMsgFields.mSrcTime;
    }

    public MamaDateTime getActivityTime()
    {
        return mBookMsgFields.mActivityTime;
    }

    public long   getEventSeqNum()
    {
        return mEventSeqNum.getValue ();
    }

    public MamaDateTime getEventTime()
    {
        return mBookMsgFields.mBookTime;
    }

    public MamaDateTime getLineTime()
    {
        return mBookMsgFields.mLineTime;
    }

    public MamaDateTime getSendTime()
    {
        return mBookMsgFields.mSendTime;
    }

    public short getMsgQual ()
    {
        return mBookMsgFields.mMsgQual.getValue();
    }

    public MamdaOrderBook getOrderBook ()
    {
        if (mFullBook != null)
        {
            return mFullBook;
        }
        else
        {
            throw new MamdaOrderBookException (
                "Attempt to access a NULL full order book");
        }
    }

    public long getBeginGapSeqNum()
    {
        return mGapBegin;
    }

    public long getEndGapSeqNum()
    {
        return mGapEnd;
    }

    public void setProcessMarketOrders (boolean  process)
    {
        mProcessMarketOrders = process;
        if (mSimpleMarketOrderDelta == null)
        {
            mSimpleMarketOrderDelta = new MamdaOrderBookConcreteSimpleDelta();
        }
        if (mComplexMarketOrderDelta == null)
        {
            mComplexMarketOrderDelta = new MamdaOrderbookConcreteComplexDelta();    
        }
    }

    public boolean getProcessMarketOrders () 
    {
        return mProcessMarketOrders;
    }


    // FieldState Accessors
    
    public short getSymbolFieldState ()
    {
        return mSymbolFieldState.getState();
    }

    public short getPartIdFieldState ()
    {
        return mPartIdFieldState.getState();
    }

    public short getSrcTimeFieldState()
    {
        return mSrcTimeFieldState.getState();
    }

    public short getActivityTimeFieldState()
    {
        return mActivityTimeFieldState.getState();
    }

    public short getLineTimeFieldState()
    {
        return mLineTimeFieldState.getState();
    }

    public short getSendTimeFieldState()
    {
        return mSendTimeFieldState.getState();
    }
    public short getEventTimeFieldState()
    {
        return mEventTimeFieldState.getState();
    }
    public short getEventSeqNumFieldState()
    {
        return mEventSeqNumFieldState.getState();
    }
    // End FieldState Accessors

    /**
     * Set whether to use an "entry manager" for finding entries in a
     * book.
     * @param useManager True if using an entry manager.
     */
    void setUseEntryManager (boolean  useManager)
    {
        if (useManager && mEntryManager == null)
        {
            mEntryManager = new MamdaOrderBookEntryManager (100);
        }
        else if (!useManager && mEntryManager != null)
        {
            mEntryManager = null;
        }
    }

    /**
     * Set whether the entry IDs are unique across the entire book.
     * This is only relevant if using an "entry manager".
     *
     * @param uniqueIds True if using unique id's.
     */
    void setEntryIdsAreUnique (boolean uniqueIds)
    {
        mUniqueEntryIds = uniqueIds;
    }

    /**
     * Set whether to gather individual basic deltas when processing
     * a complex delta.  Many applications don't need the basic
     * deltas and will iterate over part or all of the full book (with
     * the deltas already applied).
     *
     * @param keep True to keep basic deltas
     */
    void  setKeepBasicDeltas (boolean  keep)
    {
        // Not implemented in C++
    }

    /**
     * Whether to handle or ignore updates sent for an inconsistent
     * book.  A book may be in an inconsistent state if there has been
     * a gap on the sequence of update (delta) messages.  Default is
     * to not update (and wait for a recap).
     *
     * @param update Whether to apply updates to an inconsistent book.
     */
    void  setUpdateInconsistentBook (boolean  update)
    {
        mUpdateInconsistentBook = update;
    }

    /**
     * Whether to handle or ignore updates sent for a stale order
     * book.  A book is in a stale state if its mamaQuality is not
     * MAMA_QUALITY_OK..
     *
     * @param update Whether to apply updates to a stale book.
     */
    void  setUpdateStaleBook (boolean  update)
    {
        mUpdateStaleBook = update;
    }

    /**
     * Whether to clear the order book upon detection of a stale book
     * condition.  If this is set to true, then setUpdateStaleBook()
     * should probably have been set to false.
     *
     * @param clear True to clear stale books.
     */
    void  setClearStaleBook (boolean  clear)
    {
        mClearStaleBook = clear;
    }

    /**
     * Change the order book quality and invoke appropriate callbacks.
     * @param sub the subscription
     * @param quality the quality.
     */
    void  setQuality (MamdaSubscription  sub,
                      short              quality)
    {
        if (mFullBook.getQuality() == quality)
            return;  // no change

        mFullBook.setQuality (quality);
        switch (quality)
        {
            case MamaQuality.QUALITY_OK:
                break;
            case MamaQuality.QUALITY_STALE:
            case MamaQuality.QUALITY_MAYBE_STALE:
            case MamaQuality.QUALITY_PARTIAL_STALE:
            case MamaQuality.QUALITY_FORCED_STALE:
            case MamaQuality.QUALITY_UNKNOWN:
                if (mClearStaleBook)
                {
                    clear();
                    invokeClearHandlers (sub, null);
                }
        }
    }

    /**
     * Set whether we are interested in "entry level" information at
     * all.  Many applications only care about price level
     * information.  (Default is to process entry level information.)
     *
     * @param process Whether to process entries in books.
     */
    public void setProcessEntries (boolean  process)
    {
        mProcessEntries = process;
    }

    /**
     * Returns a deep, writeable copy of the underlying order book.
     *
     * @return      A snapshot of the current Order Book
     */
    public MamdaOrderBook getBookSnapshot()
    {
        MamdaOrderBook mSnapshot;
        mSnapshot = new MamdaOrderBook();
        // aquire lock
        mFullBookLock.acquireWriteLock();
        mSnapshot.copy (mFullBook);
        mFullBookLock.releaseWriteLock();
        return mSnapshot;
    }

    public static MamdaOrderBook getBookSnapShot (String symbol)
    {
        mSnapshotMapLock.acquireWriteLock();
        MamdaOrderBookListener listener = 
            (MamdaOrderBookListener) mSnapshotMap.get (symbol);
        if (listener == null)
        {
            mSnapshotMapLock.releaseWriteLock();
            throw new MamdaOrderBookException (
                "Attempted to get a NULL/empty full order book snapshot");
        }
        else
        mSnapshotMapLock.releaseWriteLock();

        listener.mFullBookLock.acquireWriteLock();
        MamdaOrderBook newBook = new MamdaOrderBook (listener.mFullBook);
        listener.mFullBookLock.releaseWriteLock();
        return newBook;
    }

   /**
    * Returns a deep, read only, copy of the underlying order book.
    * This method is faster than the writeable getBookSnapshot.
    *
    * @return      A deep snapshot of the current Order Book
    */    
    public MamdaOrderBook getReadOnlyBookSnapshot()
    {
        MamdaOrderBook mSnapshot = null;
        if (null != mFullBook)
        {
            mSnapshot = new MamdaOrderBook();
            // aquire lock
            mFullBookLock.acquireWriteLock();
            mSnapshot.copyReadOnly (mFullBook);
            mFullBookLock.releaseWriteLock();
        }
        return mSnapshot;
    }

    /**
     * Implementation of MamdaListener interface.
     */
    public void onMsg (MamdaSubscription subscription,
                       MamaMsg           msg,
                       short             msgType)
    {
        if (mLogger.isLoggable (Level.FINEST))
            mLogger.finest ("MamdaOrderBookListener: got message (type=" +
                             msgType + ")");

        // Listeners may be created on multiple threads and we only
        // want a single list of updaters.
        if (!mUpdatersComplete)
        {
            mBookUpdatersLock.acquireWriteLock();
            if (!MamdaOrderBookFields.isSet())
            {
                mBookUpdatersLock.releaseWriteLock();
                mLogger.warning ("MamdaOrderBookFields::setDictionary() has not been called.");
                return;
            }
            if (!mUpdatersComplete)
            {
                initFieldUpdaters ();
                mUpdatersComplete = true;
            }
            mBookUpdatersLock.releaseWriteLock();
        }

        try
        {
            switch (msgType)
            {
                case MamaMsgType.TYPE_BOOK_UPDATE:
                    if ((mFullBook.getQuality() != MamaQuality.QUALITY_OK) &&
                            !mUpdateStaleBook)
                    {
                        mLogger.fine("MamdaOrderBookListener: " +
                            "ignoring update during stale status for" +
                            (subscription != null ? subscription.getSymbol() : "(nil)"));
                        return;
                    }
                    handleUpdate (subscription, msg);
                    break;
                case MamaMsgType.TYPE_BOOK_CLEAR:
                    handleClear (subscription, msg);
                    break;
                case MamaMsgType.TYPE_BOOK_INITIAL:
                case MamaMsgType.TYPE_BOOK_RECAP:
                case MamaMsgType.TYPE_BOOK_SNAPSHOT:
                    handleRecap (subscription, msg);
                    break;
                default:
                    mLogger.info("MamdaOrderBookListener: ignoring invalid book message type " +
                        msgType + " for symbol " +
                        (subscription != null ? subscription.getSymbol() : "(nil)"));
            }

            // Added listener to global map
            if (!mMappedForSnapshot)
            {
                mSnapshotMapLock.acquireWriteLock();
                mSnapshotMap.put (subscription.getSymbol(), this);
                mMappedForSnapshot = true;
                mSnapshotMapLock.releaseWriteLock();
            }
            if (mFullBook != null)
                mFullBook.cleanupDetached();
        }
        catch (MamdaOrderBookException e)
        {
            mLogger.info ("MamdaOrderBookListener: caught exception for " +
                          (subscription != null ? subscription.getSymbol () :
                          "(nil)") + ": " + e);
            mLogger.info ("MamdaOrderBookListener: message was: " +
                          msg.toString());
        }
        catch (MamaException e)
        {
            mLogger.info ("MamdaOrderBookListener: caught MamaStatus exception: " +
                          e.toString());
        }
        if (mLogger.isLoggable (Level.FINEST))
            mLogger.finest ("MamdaOrderBookListener: done with message");
    }

    private void handleClear (MamdaSubscription  subscription,
                              MamaMsg            msg)
    {
        if (mLogger.isLoggable (Level.FINE))
        mLogger.fine (
                  "MamdaOrderBookListener: handling CLEAR for order book " +
                  subscription.getSymbol());

        mFullBookLock.acquireWriteLock();
        processBookMessage (subscription, msg, true);
        mFullBook.clear();
        mFullBook.setIsConsistent (true);
        if (mEntryManager != null)
            mEntryManager.clear();
        mFullBookLock.releaseWriteLock();

        invokeClearHandlers (subscription, msg);
        mGotInitial = true;
    }
    
    private void handleRecap (MamdaSubscription  subscription,
                              MamaMsg            msg)
    {
        // Synchronize the current Order Book in case a snapshot is being created
        if (mLogger.isLoggable (Level.FINE))
            mLogger.fine (
                "MamdaOrderBookListener: handling INITIAL/RECAP for order book "+
                subscription.getSymbol());

        boolean complete = false;

        mFullBookLock.acquireWriteLock();
        if (!mFullBook.getIsConsistent ())
        {
            mLogger.info (
                "Received Recap: Book now consistent for [" +
                (subscription != null? subscription.getSymbol() : "no symbol")
                + "]");
        }

        processBookMessage (subscription, msg, true);

        try
        {
            complete = createDelta (true, msg);
            mFullBook.setIsConsistent (true);
            mPrevSenderId = mBookMsgFields.mSenderId.getValue(); 
        }
        catch (MamdaOrderBookException e)
        {
            mLogger.info (
                "MamdaOrderBookListener: caught exception for " +
                subscription != null ? subscription.getSymbol() : "(nil)" +
                ": " + e);
        }
        catch (MamaException e)
        {
            mLogger.info (
                "MamdaOrderBookListener: caught MamaStatus exception: %s" +
                e.toString());
        }
        finally
        {
            mFullBookLock.releaseWriteLock();
        }

        if (complete)
        {
            // Always call the recap handler even in exceptional or error
            // situations
            setQuality (subscription, MamaQuality.QUALITY_OK);
            invokeRecapHandlers (subscription, msg);
        }

        mGotInitial = true;
    }

    private void handleUpdate (MamdaSubscription  subscription,
                               MamaMsg            msg)
    {
        if (mLogger.isLoggable (Level.FINEST))
            mLogger.finest (
                  "MamdaOrderBookListener: handling update for order book " +
                  subscription.getSymbol());

        if (!mGotInitial)
        {
            throw new MamdaOrderBookException ("got update before initial/recap");
        }

        mFullBookLock.acquireWriteLock();
        
        processBookMessage (subscription, msg, false);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            mFullBookLock.releaseWriteLock();
            return;
        }

        try
        {
            createDelta (false, msg);
        }
        catch (MamdaOrderBookException e)
        {
            mLogger.info (
                "MamdaOrderBookListener: caught exception for " +
                subscription != null ? subscription.getSymbol() : "(nil)" +
                ": " + e);
        }
        catch (MamaException e)
        {
            mLogger.info (
                "MamdaOrderBookListener: caught MamaStatus exception: " +
                e.toString());
        }
        finally
        {
            mFullBookLock.releaseWriteLock();
        }

        if (!mHandlers.isEmpty())
        {
            if (mUpdateInconsistentBook || mFullBook.getIsConsistent())
            {
                invokeDeltaHandlers (subscription, msg);
            }
            else
            {
                if (mLogger.isLoggable (Level.FINE))
                    mLogger.fine (
                        "MamdaOrderBookListener: not forwarding update for " +
                        "inconsistent order book "+
                        subscription.getSymbol());
            }
        }
    }

    private void getBookMsgFields(MamaMsg msg)
    {
        int i =0;
        while (mBookUpdaters[i] != null)
        {
            mBookUpdaters[i++].onUpdate (msg, MamdaOrderBookListener.this);
        }
    }

    private void getPriceLevelMsgFields (MamaMsg msg)
    {
        int i =0;
        while (mPriceLevelUpdaters[i] != null)
            mPriceLevelUpdaters[i++].onUpdate(msg, MamdaOrderBookListener.this);
    }

    private void getEntryMsgFields (MamaMsg msg)
    {
        int i =0;
        while (mEntryUpdaters[i] != null)
            mEntryUpdaters[i++].onUpdate (msg, MamdaOrderBookListener.this);
    }
    
    private void processBookMessage (MamdaSubscription subscription,
                                     MamaMsg           msg,
                                     boolean           isRecap)
    {
        mBookMsgFields.clear();

        mBookMsgFields.mBookType.setValue(0); 
        if(MamdaOrderBookFields.BOOK_TYPE != null)
        {
            msg.tryU16 (null, MamdaOrderBookFields.BOOK_TYPE.getFid(), mBookMsgFields.mBookType);        
        }

        getBookMsgFields (msg);

        if (mBookMsgFields.mBookTime.isEmpty ())
        {
            mBookMsgFields.mBookTime.copy (mBookMsgFields.mSrcTime);
        }
        mFullBook.setBookTime (mBookMsgFields.mBookTime);

        if (mBookMsgFields.mMsgNum.getValue() == 1)
        {
            // Only clear the book/delta if this is the first message in the
            // set of updates.
            mCurrentDeltaCount = 0;
            if (isRecap)
            {
                mFullBook.clear();
                if (mEntryManager != null)
                {
                    mEntryManager.clear();
                }
            }
        }

        long seqNum      = mBookMsgFields.mMsgSeqNum.getValue();
        long eventSeqNum = mEventSeqNum.getValue ();
        long senderId    = mBookMsgFields.mSenderId.getValue();

        mEventSeqNum.setValue (seqNum);
    
        if (!isRecap && (seqNum == eventSeqNum))
        {
            mIgnoreUpdate = true;
        }
        else if (!isRecap && (seqNum != 0) && (seqNum != (eventSeqNum + 1)))
        {
            mGapBegin    = eventSeqNum + 1;
            mGapEnd      = seqNum - 1;
            invokeGapHandlers (subscription, msg);
            if (senderId == mPrevSenderId)
            {
                mFullBook.setIsConsistent (false);
            }
        }
        mPrevSenderId  =  senderId;
    }

    private void processLevelMessage (MamaMsg msg)
    {
        mBookMsgFields.mEntryVector.setValue(null);
        mBookMsgFields.mEntries.clear();

        getPriceLevelMsgFields(msg);

        if ((!mBookMsgFields.mHasPlTime) && (!mBookMsgFields.mBookTime.isEmpty()))
        {
            mBookMsgFields.mPlTime.copy (mBookMsgFields.mBookTime);      
        }
    }

    private void processEntryMessage (MamaMsg msg)
    {
        getEntryMsgFields(msg);

        if (mBookMsgFields.mEntryTime.isEmpty ())
        {
            mBookMsgFields.mEntryTime.copy (mBookMsgFields.mPlTime);
        }
    }

    private boolean createDelta (boolean isRecap, MamaMsg msg)
    {
        if ((mBookMsgFields.mNumLevels.getValue() == 0) &&
            (mBookMsgFields.mHasMarketOrders == false))
        {
            // No price levels in update
            return true;
        }

        if (mProcessMarketOrders)
        {
            if (mBookMsgFields.mBidMarketOrders.getNumFields() != 0) 
            {
                mBookMsgFields.clearPriceLevel();
                processLevelMessage (mBookMsgFields.mBidMarketOrders);
                MamdaOrderBookPriceLevel level =
                    processLevelPart1 (mBookMsgFields.mPlPrice.getValue(),
                                       mBookMsgFields.mPlSide.getValue(),
                                       mBookMsgFields.mPlAction.getValue(),
                                       MamdaOrderBookPriceLevel.LEVEL_MARKET,
                                       mBookMsgFields.mPlTime);
                processEntries    (level, mBookMsgFields.mPlAction.getValue(), msg);
                processLevelPart2 (level,
                                   mBookMsgFields.mPlAction.getValue(),
                                   mBookMsgFields.mPlSize.getValue(),
                                   mBookMsgFields.mPlSizeChange.getValue(),
                                   mBookMsgFields.mPlNumEntries.getValue());
            }
            if (mBookMsgFields.mAskMarketOrders.getNumFields() != 0)
            {
                mBookMsgFields.clearPriceLevel();
                processLevelMessage (mBookMsgFields.mAskMarketOrders);
                MamdaOrderBookPriceLevel level =
                processLevelPart1 (mBookMsgFields.mPlPrice.getValue(),
                                   mBookMsgFields.mPlSide.getValue(),
                                   mBookMsgFields.mPlAction.getValue(),
                                   MamdaOrderBookPriceLevel.LEVEL_MARKET,
                                   mBookMsgFields.mPlTime);
                processEntries    (level, mBookMsgFields.mPlAction.getValue(), msg);
                processLevelPart2 (level,
                                   mBookMsgFields.mPlAction.getValue(),
                                   mBookMsgFields.mPlSize.getValue(),
                                   mBookMsgFields.mPlSizeChange.getValue(),
                                   mBookMsgFields.mPlNumEntries.getValue());
            }
            //Single Market Order Update
            if ((mBookMsgFields.mNumLevels.getValue() == 0) && (mBookMsgFields.mHasMarketOrders == true))
            {
                return true;
            }
        }

        if (mBookMsgFields.mPriceLevelVector.getValue() == null &&
            mBookMsgFields.mPriceLevels.isEmpty ())
        {
            // Single, flat price level
            if (!mBookMsgFields.mHasPlTime)
            {
                mBookMsgFields.mPlTime.copy(mBookMsgFields.mBookTime);
            }
            MamdaOrderBookPriceLevel level =
                processLevelPart1 (mBookMsgFields.mPlPrice.getValue(),
                                   mBookMsgFields.mPlSide.getValue(),
                                   mBookMsgFields.mPlAction.getValue(),
                                   MamdaOrderBookPriceLevel.LEVEL_LIMIT,
                                   mBookMsgFields.mPlTime);
                                
            if (null == level)
            {
                if (mLogger.isLoggable (Level.FINEST))
                {
                    mLogger.finest (
                            "  received delete for unknown price level ("
                            + mBookMsgFields.mPlPrice.getValue() + ") ");
                }
            }
            else
            {
                processEntries  (level, mBookMsgFields.mPlAction.getValue(), msg);
                processLevelPart2 (level,
                                   mBookMsgFields.mPlAction.getValue(),
                                   mBookMsgFields.mPlSize.getValue(),
                                   mBookMsgFields.mPlSizeChange.getValue(),
                                   mBookMsgFields.mPlNumEntries.getValue());
            }
        }
        else
        {
            if (mBookMsgFields.mPriceLevelVector.getValue() != null)
            {
                for (int i=0; i < mBookMsgFields.mPriceLevelVector.getValue().length; i++)
                {
                    MamaMsg levelMsg = mBookMsgFields.mPriceLevelVector.getValue()[i];
                    if (levelMsg != null)
                    {
                        mBookMsgFields.clearPriceLevel();
                        processLevelMessage (levelMsg);
                        MamdaOrderBookPriceLevel level =
                            processLevelPart1 (mBookMsgFields.mPlPrice.getValue(),
                                               mBookMsgFields.mPlSide.getValue(),
                                               mBookMsgFields.mPlAction.getValue(),
                                               MamdaOrderBookPriceLevel.LEVEL_LIMIT,
                                               mBookMsgFields.mPlTime);
                        if (null == level)
                        {
                            if (mLogger.isLoggable (Level.FINEST))
                            {
                                mLogger.finest (
                                        "  received delete for unknown price level ("
                                        + mBookMsgFields.mPlPrice.getValue() + ") ");
                            }
                        }  
                        else
                        {
                            processEntries    (level, mBookMsgFields.mPlAction.getValue(), msg);
                            processLevelPart2 (level,
                                               mBookMsgFields.mPlAction.getValue(),
                                               mBookMsgFields.mPlSize.getValue(),
                                               mBookMsgFields.mPlSizeChange.getValue(),
                                               mBookMsgFields.mPlNumEntries.getValue());
                        }
                    }
                }
            }
            else
            {
                for (int i = 1; i <= MamdaOrderBookFields.getNumLevelFields (); i++)
                {
                    if  (msg.tryMsg (MamdaOrderBookFields.PRICE_LEVEL[i], tmpMsg))
                    {
                        MamaMsg levelMsg = tmpMsg.getValue();
                        if (levelMsg != null)
                        {
                            processLevelMessage (levelMsg);
                            MamdaOrderBookPriceLevel level =
                                processLevelPart1 (mBookMsgFields.mPlPrice.getValue(),
                                                   mBookMsgFields.mPlSide.getValue(),
                                                   mBookMsgFields.mPlAction.getValue(),
                                                   MamdaOrderBookPriceLevel.LEVEL_LIMIT,
                                                   mBookMsgFields.mPlTime);
                            if (null == level)
                            {
                                if (mLogger.isLoggable (Level.FINEST))
                                        {
                                            mLogger.finest (
                                                    "  received delete for unknown price level ("
                                                    + mBookMsgFields.mPlPrice.getValue() + ") ");
                                        }
                            }          
                            else
                            {
                                processEntries    (level, mBookMsgFields.mPlAction.getValue(), msg);
                                processLevelPart2 (level,
                                                mBookMsgFields.mPlAction.getValue(),
                                                mBookMsgFields.mPlSize.getValue(),
                                                mBookMsgFields.mPlSizeChange.getValue(),
                                                mBookMsgFields.mPlNumEntries.getValue());
                            }
                        }
                    }
                    else
                        break;
                }
            }
        }
        return mBookMsgFields.mMsgNum.getValue() == mBookMsgFields.mMsgTotal.getValue();
    }

    private void processLevel (MamaMsg msg)
    {
        MamdaOrderBookPriceLevel level =
            processLevelPart1 (mBookMsgFields.mPlPrice.getValue(),
                               mBookMsgFields.mPlSide.getValue(),
                               mBookMsgFields.mPlAction.getValue(),
                               MamdaOrderBookPriceLevel.LEVEL_LIMIT,
                               mBookMsgFields.mPlTime);

        processLevelPart2 (level,
                           mBookMsgFields.mPlAction.getValue(),
                           mBookMsgFields.mPlSize.getValue(),
                           mBookMsgFields.mPlSizeChange.getValue(),
                           mBookMsgFields.mPlNumEntries.getValue());
    }

    private void processEntries (MamdaOrderBookPriceLevel level, 
                                 char                     plAction, 
                                 MamaMsg                  msg)
    {
        if ((!mProcessEntries && (mBookMsgFields.mBookType.getValue() != 1)) 
            || (mBookMsgFields.mBookType.getValue() == 2))
        {
            return;
        }

        if (mBookMsgFields.mPlNumAttach.getValue() == 0)
        {
            if (MamdaOrderBookPriceLevel.ACTION_DELETE != plAction)
            {
               // No entry delta; just this price level delta.
              addDelta (null, level, mBookMsgFields.mPlSizeChange.getValue(), plAction,
                        MamdaOrderBookEntry.ACTION_UNKNOWN);
              return;
            }
            
            Iterator i = level.entryIterator ();

            // Received PriceLevel Delete for priceLevel with no entries
            if (false == i.hasNext())
            {

              // No entry delta; just this price level delta.
              addDelta (null, level, mBookMsgFields.mPlSizeChange.getValue(), plAction,
                        MamdaOrderBookEntry.ACTION_UNKNOWN);
              return;
            }
            
            // Special case where we receive a delete, we are processing
            // entries but the message has no entries.
            // Assume that all entries in the book are to be deleted and
            // build up the delta using the actual entries in the cached level.
            while (i.hasNext ())
            {
                MamdaOrderBookEntry entry     = (MamdaOrderBookEntry) i.next ();
                // Don't call processEntry as we already have the entry we want
                // so we can skip stright to removing the entry and building up
                // the delta
                entry.setAction (MamdaOrderBookEntry.ACTION_DELETE);
                if (mEntryManager != null)
                {
                    mEntryManager.removeEntry (entry);
                }

                // remove the entry from the level via the iterator
                i.remove ();
                addDelta (entry, level, level.getSizeChange(),
                          MamdaOrderBookPriceLevel.ACTION_DELETE,
                          MamdaOrderBookEntry.ACTION_DELETE);
            }
            return;
        }

        if (mBookMsgFields.mEntryVector.getValue() == null &&
            mBookMsgFields.mEntries.isEmpty ()  &&
            mBookMsgFields.mEntryId.getValue() != null)
        {
            // Single, flat entry
            if (mBookMsgFields.mEntryTime.isEmpty ())
            {
                mBookMsgFields.mEntryTime.copy(mBookMsgFields.mPlTime);

            }

            processEntry (level,
                          plAction,
                          mBookMsgFields.mEntryAction.getValue(),
                          mBookMsgFields.mEntryReason.getValue(),
                          mBookMsgFields.mEntryId.getValue(),
                          mBookMsgFields.mEntrySize.getValue(),
                          mBookMsgFields.mEntryTime,
                          mBookMsgFields.mEntryStatus.getValue());
            mBookMsgFields.clearEntry();
        }
        else
        {
            // wombatMsg
            if (mBookMsgFields.mEntryVector.getValue() != null)
            {
                for (int i = 0; i < mBookMsgFields.mPlNumAttach.getValue(); i++)
                {
                    MamaMsg entryMsg = mBookMsgFields.mEntryVector.getValue()[i];
                    if (entryMsg != null)
                    {
                        processEntryMessage (entryMsg);
                        processEntry (level,
                                      plAction,
                                      mBookMsgFields.mEntryAction.getValue(),
                                      mBookMsgFields.mEntryReason.getValue(),
                                      mBookMsgFields.mEntryId.getValue(),
                                      mBookMsgFields.mEntrySize.getValue(),
                                      mBookMsgFields.mEntryTime,
                                      mBookMsgFields.mEntryStatus.getValue());
                    }
                    mBookMsgFields.clearEntry();                    
                }
            }
            else 
            {
                for (int i = 1; i <= MamdaOrderBookFields.getNumEntryFields (); i++)
                {
                    if  (msg.tryMsg (MamdaOrderBookFields.PL_ENTRY[i], tmpMsg))
                    {

                        MamaMsg entryMsg= tmpMsg.getValue();
                        if (entryMsg != null)
                        {
                            processEntryMessage (entryMsg);
                            processEntry (level,
                                          plAction,
                                          mBookMsgFields.mEntryAction.getValue(),
                                          mBookMsgFields.mEntryReason.getValue(),
                                          mBookMsgFields.mEntryId.getValue(),
                                          mBookMsgFields.mEntrySize.getValue(),
                                          mBookMsgFields.mEntryTime,
                                          mBookMsgFields.mEntryStatus.getValue());
                            mBookMsgFields.clearEntry();
                        }
                    }
                }
            }
        }
    }

    private MamdaOrderBookPriceLevel processLevelPart1 (double       plPrice,
                                                        char         plSide,
                                                        char         plAction,
                                                        char         orderType,
                                                        MamaDateTime plTime)
    {
        MamdaOrderBookPriceLevel level = null;
        boolean newLevel = false;

        if (MamdaOrderBookPriceLevel.LEVEL_MARKET == orderType)
        {
            level = mFullBook.getOrCreateMarketOrdersSide (plSide);
            MamaPrice levelPrice = new MamaPrice (plPrice);
            level.setPrice (levelPrice);
        }
        else
        {
            if (plAction == MamdaOrderBookPriceLevel.ACTION_DELETE)
            {
                level = mFullBook.findLevel (plPrice, plSide);
                if (null == level)
                    return level;
                mFullBook.detach (level);
            }
            else
            {
                //Check if level exists
                level = mFullBook.findLevel(plPrice, plSide);
                if (null == level)
                {                    
                    newLevel = true;
                    level = mFullBook.findOrCreateLevel (plPrice, plSide);
                }
                
                if (newLevel)
                {
                    //Fix the level action for the delta                   
                    plAction = MamdaOrderBookPriceLevel.ACTION_ADD;
                    mBookMsgFields.mPlAction.setValue (plAction);
                }
                else
                {
                    //Fix the level action for the delta                   
                    plAction = MamdaOrderBookPriceLevel.ACTION_UPDATE;
                    mBookMsgFields.mPlAction.setValue (plAction);
                }
            }
        }

        level.setOrderType (orderType);
        level.setSide (plSide);
        if (!plTime.isEmpty())
        {
            level.setTime (plTime);           
        }
        else if (!mBookMsgFields.mBookTime.isEmpty())
        {
            level.setTime (mBookMsgFields.mBookTime);
        }
        
        return level;
    }

    private void processLevelPart2 (MamdaOrderBookPriceLevel level,
                                    char                     plAction,
                                    double                   plSize,
                                    double                   plSizeChange,
                                    double                   plNumEntries)
    {
        // Fix up explicit numbers provided by the message.
        level.setSize (plSize);
        if (!mHaveEntries)
            level.setNumEntries (plNumEntries);

        if ((!mProcessEntries && (mBookMsgFields.mBookType.getValue() != 1)) 
            || (mBookMsgFields.mBookType.getValue() == 2))
        {
            // No entry delta; just this price level delta.
            addDelta (null, level, plSizeChange, plAction,
                      MamdaOrderBookEntry.ACTION_UNKNOWN);

            if (mFullBook.getGenerateDeltaMsgs())
            {
                mFullBook.addDelta (null, level, plSizeChange, plAction,
                                    MamdaOrderBookEntry.ACTION_UNKNOWN);
            }
        }
        
        //V5 Entry Book
        if (mBookMsgFields.mBookType.getValue() == 1)
        {
            if(!mHaveEntries)
            {
                //Remove a plAction Add/Update level which got inserted by a delete entry delta
                mFullBook.detach (level);
            }
        }                   
    }

    private void processEntry (MamdaOrderBookPriceLevel level,
                               char                     plAction,
                               char                     entryAction,
                               char                     entryReason,
                               String                   id,
                               double                   size,
                               MamaDateTime             entryTime, 
                               int                      status)
    {
        // We have to connect this entry to the price level, even if this
        // is a deletion and even if this is a deletion of an entry that
        // did not even previously exist.  The delta levels and entries
        // always need to have been connected to the book because
        // otherwise calls like MamdaOrderBookEntry::getOrderBook() will
        // fail.
        boolean              newEntry = false;
        MamdaOrderBookEntry  entry    = null;

        if (id == null || id.equals(""))
        {
            if (mLogger.isLoggable (Level.FINEST))
            {
                mLogger.finest (
                        " Invalid value for Entry Id - unable to process entry");
            }
            return;
        }

        if (mEntryManager != null)
        {
            if (mUniqueEntryIds)
            {
                entry = mEntryManager.findEntry (id, false);
                
                if (entryAction == MamdaOrderBookEntry.ACTION_DELETE)
                {
                    if (entry != null)
                    {
                        mEntryManager.removeEntry (id);
                    }
                    else
                    {
                        if (1 == mBookMsgFields.mBookType.getValue())
                        {
                            //size will be unchanged
                            if (!mBookMsgFields.mHasPlSize)
                            {
                                mBookMsgFields.mPlSize.setValue(level.getSize());
                            }
                        }
                        if (mLogger.isLoggable (Level.FINEST))
                        {
                            mLogger.finest (
                                    "  received delete for unknown entry ("+ id +
                                    ") for price level (" + level.getPrice() + ") ");
                        }
                        if (level.getNumEntriesTotal() == 0)
                        {
                            mFullBook.detach (level);
                        }
                        return;
                    }
                }
                
                if (entry == null)
                {
                    entry = new MamdaOrderBookEntry();
                    entry.setId               (id);
                    entry.setAction           (MamdaOrderBookEntry.ACTION_ADD);
                    entry.setReason           (entryReason);
                    entry.setSourceDerivative (mFullBook.getSourceDerivative());
                    entry.setManager          (mEntryManager);
                    level.addEntry            (entry);
                    mEntryManager.addEntry    (entry);
                    newEntry = true;
                }
            }
            else
            {
                // uniqueId is for the entry manager
                //snprintf (uniqueId, 32, "%s%g", id, level.getPrice());

                mUniqueIdBuffer.delete (0, mUniqueIdBuffer.length ());
                mUniqueIdBuffer.append (id)
                               .append (level.getPrice ().getValue ())
                               .append (level.getSide ());
                entry = mEntryManager.findEntry (mUniqueIdBuffer.toString (),
                                                 false);
                                                  
                if (entryAction == MamdaOrderBookEntry.ACTION_DELETE)
                {
                    if (entry != null)
                    {
                        mEntryManager.removeEntry (mUniqueIdBuffer.toString ());
                    }
                    else
                    {
                        if (1 == mBookMsgFields.mBookType.getValue())
                        {
                            //size will be unchanged
                            if (!mBookMsgFields.mHasPlSize)
                            {
                                mBookMsgFields.mPlSize.setValue(level.getSize());
                            }
                        }
                        if (mLogger.isLoggable (Level.FINEST))
                        {
                            mLogger.finest (
                                    "  received delete for unknown entry ("+ id  +
                                    ") for price level (" + level.getPrice () + ") ");
                        }
                        return;
                    }
                }
                
                if (entry == null)
                {
                    entry = new MamdaOrderBookEntry();
                    entry.setId               (id);
                    entry.setUniqueId         (mUniqueIdBuffer.toString ());
                    entry.setAction           (MamdaOrderBookEntry.ACTION_ADD);
                    entry.setReason           (entryReason);
                    entry.setSourceDerivative (mFullBook.getSourceDerivative());
                    entry.setManager          (mEntryManager);
                    level.addEntry            (entry);
                    mEntryManager.addEntry (entry,
                                            mUniqueIdBuffer.toString ());
                    newEntry = true;
                   
                }
            }
        }
        else
        {
            if (entryAction == MamdaOrderBookEntry.ACTION_DELETE)
            {
                entry = level.findEntry (id);
                if (null == entry)
                {
                    //V5 Entry Book
                    if (1 == mBookMsgFields.mBookType.getValue())
                    {
                        //size will be unchanged
                        if (!mBookMsgFields.mHasPlSize)
                        {
                            mBookMsgFields.mPlSize.setValue(level.getSize ());
                        }
                    }
                    
                    if (mLogger.isLoggable (Level.FINEST))
                    {
                        mLogger.finest (
                                "  received delete for unknown entry ("+ id +
                                ") for price level (" + level.getPrice() + ") ");
                    }
                    return;
                }
            }
            else
            {
                entry = level.findEntry(id);
                if (null == entry)
                {
                    entry = level.findOrCreateEntry (id);
                    newEntry = true;
                }

                if ((newEntry) && (MamdaOrderBookEntry.ACTION_UPDATE == entryAction))
                {
                    //fix the entry action
                    entryAction = MamdaOrderBookEntry.ACTION_ADD;
                }
            }
        }
                
        if (!mBookMsgFields.mHasPlSizeChange)
        {
            if (newEntry)
            {
                //Add
                mBookMsgFields.mPlSizeChange.setValue (mBookMsgFields.mPlSizeChange.getValue() + size);
            }
            else
            {
                //Update
                mBookMsgFields.mPlSizeChange.setValue (size - entry.getSize());
            }
        }

        if (!mBookMsgFields.mHasPlSize)
        {
            if (newEntry)
            {
                //Add
                mBookMsgFields.mPlSize.setValue (level.getSize() + size);
            }
            else
            {
                //Update
                mBookMsgFields.mPlSize.setValue (
                level.getSize() + size - entry.getSize());
            }
        }                
                
        entry.setSize   (size);
        entry.setTime   (entryTime);
        entry.setStatus (status);
        entry.setId     (id);

        if (!mBookMsgFields.mHasPlTime)
        {
            if (level.getTime() == null)
            {
                level.setTime (entryTime);
            }
            else if (entryTime.compareTo (level.getTime()) == 1)
            {
                level.setTime (entryTime);
            }
        }
        
        if (entryAction == MamdaOrderBookEntry.ACTION_DELETE)              
        {
            // should this be in the "else" part, above?
            level.removeEntry (entry);
            // We will delete it later
            if (mBookMsgFields.mBookType.getValue() == 1)
            {
                if (0 == level.getNumEntriesTotal())
                {
                    //Fix the level action for the delta - if there are no entries
                    //set the action to delete
                    plAction = MamdaOrderBookPriceLevel.ACTION_DELETE;
                    mBookMsgFields.mPlAction.setValue (MamdaOrderBookPriceLevel.ACTION_DELETE);
                    mFullBook.detach (level);
                }
            }
        }    
                
        if (mLogger.isLoggable (Level.FINEST))
        {
            mLogger.finest (
                      "  found delta entry ("+ entry.getId () +
                      ") for price level (" + level.getPrice () + ") " +
                      "side=" + level.getSide () + ", plAct=" +
                      plAction + ", plSize=" + level.getSize () +
                      ", entAct=" +entryAction + ", entSize=" +
                      entry.getSize ());
        }
   
        addDelta (entry, level, level.getSizeChange(), plAction, entryAction);
        mHaveEntries = true; 
        if (mFullBook.getGenerateDeltaMsgs())
        {
            mFullBook.addDelta (entry, level, 
                                mBookMsgFields.mPlSizeChange.getValue(), 
                                plAction, entryAction);
        }
    }

    private boolean ignoreEntryId (String id)
    {
        return mIgnoredEntries.containsKey (id); 
    }

    private void invokeClearHandlers (MamdaSubscription subscription,
                                      MamaMsg           msg)
    {
        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaOrderBookHandler handler = (MamdaOrderBookHandler) i.next();
            handler.onBookClear (subscription, this, msg, this, getOrderBook ());
        }
    }

    private void invokeRecapHandlers (MamdaSubscription subscription,
                                      MamaMsg           msg)
    {
        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaOrderBookHandler handler = (MamdaOrderBookHandler) i.next();
            handler.onBookRecap (subscription, this, msg, null, this,
                                 mFullBook);
        }
    }

    private void invokeDeltaHandlers (MamdaSubscription subscription,
                                      MamaMsg           msg)
    {
        if (mCurrentDeltaCount == 0)
        {
            mLogger.info (
                        "MamdaOrderBookListener: got message with no delta: "+
                        msg.toString());
            return;
        }

        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaOrderBookHandler handler = (MamdaOrderBookHandler) i.next();

            if (mCurrentDeltaCount == 1)
            {
                handler.onBookDelta (subscription, this, msg,
                                     mSimpleDelta, mFullBook);
            }
            else if (mCurrentDeltaCount > 1)
            {
                handler.onBookComplexDelta (subscription, this, msg,
                                            mComplexDelta, mFullBook);
            }
        }      
    }

    private void invokeComplexDeltaHandlers (MamdaSubscription subscription,
                                             MamaMsg           msg)
    {
        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaOrderBookHandler handler = (MamdaOrderBookHandler) i.next();
            handler.onBookComplexDelta (subscription, this, msg,
                                        mComplexDelta, mFullBook);
        }
    }

    private void invokeGapHandlers (MamdaSubscription subscription,
                                    MamaMsg           msg)
    {
        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaOrderBookHandler handler = (MamdaOrderBookHandler) i.next();
            handler.onBookGap (subscription, this, msg, this, mFullBook);
        }
    }

    private void addDelta (MamdaOrderBookEntry      entry,
                           MamdaOrderBookPriceLevel level,
                           double                   plDeltaSize,
                           char                     plAction,
                           char                     entryAction)
    {
        mCurrentDeltaCount++;
        if (mCurrentDeltaCount == 1)
        {
            /* This is number one, so save the "simple" delta. */
            mSimpleDelta.set(entry, level, plDeltaSize, plAction, entryAction);
        }
        else if (mCurrentDeltaCount == 2)
        {
            /* This is number two, so copy the saved "simple" delta to the
             * "complex" delta and add the current one. */
            mComplexDelta.clear        ();
            mComplexDelta.setOrderBook (mFullBook);
            mComplexDelta.add          (mSimpleDelta);
            mComplexDelta.add          (entry, level, plDeltaSize, plAction,
            entryAction);
        }
        else
        {
            /* This is number greater than two, so add the current delta. */
            mComplexDelta.add (
                entry, level, plDeltaSize, plAction, entryAction);
        }
    }

    private static void initFieldUpdaters ()
    {
        int i, j, k;
        synchronized (MamdaOrderBookListener.class)
        {
            if (mBookUpdaters == null)
            {
                mBookUpdaters       = new BookMsgUpdate[50];
                mPriceLevelUpdaters = new BookMsgUpdate[50];
                mEntryUpdaters      = new BookMsgUpdate[50];
            }
            else
            {
                return;
            }

            for (i=0;i<50;i++)
            {
                mBookUpdaters       [i] = null;
                mPriceLevelUpdaters [i] = null;
                mEntryUpdaters      [i] = null;
            } 
                
            i=j=k=0;
            
            if (MamdaCommonFields.MSG_SEQ_NUM != null)
                mBookUpdaters[i++] = FieldUpdateMsgSeqNum;
            else
                mLogger.info ("MamdaOrderBookListener: MSG_SEQ_NUM missing from dictionary");
                
            if (MamdaCommonFields.SENDER_ID != null)
                mBookUpdaters[i++] = FieldUpdateSenderId;
            if (MamdaCommonFields.MSG_NUM != null)
                mBookUpdaters[i++] = FieldUpdateMsgNum;    
            if (MamdaCommonFields.MSG_TOTAL != null)
                mBookUpdaters[i++] = FieldUpdateMsgTotal;   
            if (MamdaCommonFields.SYMBOL != null)
                mBookUpdaters[i++] = FieldUpdateSymbol;       
            if (MamdaCommonFields.PART_ID != null)
                mBookUpdaters[i++] = FieldUpdatePartId;       
            if (MamdaCommonFields.SRC_TIME != null)
                mBookUpdaters[i++] = FieldUpdateSrcTime;       
            if (MamdaCommonFields.LINE_TIME != null)
                mBookUpdaters[i++] = FieldUpdateLineTime;       
            if (MamdaOrderBookFields.BOOK_TIME != null)
                mBookUpdaters[i++] = FieldUpdateBookTime;       
            if (MamdaCommonFields.SEND_TIME != null)
                mBookUpdaters[i++] = FieldUpdateSendTime;       
            if (MamdaCommonFields.ACTIVITY_TIME != null)
                mBookUpdaters[i++] = FieldUpdateActivityTime;
            // Ask and Bid mkt order updaters need to before PRICE_LEVELS
            // else the underlying message used in tryArrayMsg() used in the PRICE_LEVELS will be overwritten
            // by the tryArrayMsg() in the ASK or BID updaters
            if (MamdaOrderBookFields.ASK_MARKET_ORDERS != null)
            {
                mBookUpdaters[i++] = FieldUpdateAskMarketOrders;
                mPriceLevelUpdaters[j++] = FieldUpdateAskMarketOrders;
            }
            if (MamdaOrderBookFields.BID_MARKET_ORDERS != null)
            {
                mBookUpdaters[i++] = FieldUpdateBidMarketOrders;
                mPriceLevelUpdaters[j++] = FieldUpdateBidMarketOrders;
            }
            if (MamdaOrderBookFields.NUM_LEVELS != null)
            {
                mBookUpdaters[i++] = FieldUpdateNumLevels;
                mPriceLevelUpdaters[j++] = FieldUpdateNumLevels;
            }                
            if (MamdaOrderBookFields.PRICE_LEVELS != null)
            {
                mBookUpdaters[i++] = FieldUpdatePriceLevels;
                mPriceLevelUpdaters[j++] = FieldUpdatePriceLevels;
            }
            if (MamdaOrderBookFields.PL_PRICE != null)
            {
                mBookUpdaters[i++] = FieldUpdatePlPrice;
                mPriceLevelUpdaters[j++] = FieldUpdatePlPrice;
            }
            if (MamdaOrderBookFields.PL_SIDE != null)
            {
                mBookUpdaters[i++] = FieldUpdatePlSide;
                mPriceLevelUpdaters[j++] = FieldUpdatePlSide;
            }   
            if (MamdaOrderBookFields.PL_ACTION != null)
            {
                mBookUpdaters[i++] = FieldUpdatePlAction;
                mPriceLevelUpdaters[j++] = FieldUpdatePlAction;
            }  
            if (MamdaOrderBookFields.PL_SIZE != null)
            {
                mBookUpdaters[i++] = FieldUpdatePlSize;
                mPriceLevelUpdaters[j++] = FieldUpdatePlSize;
            }   
            if (MamdaOrderBookFields.PL_SIZE_CHANGE != null)
            {
                mBookUpdaters[i++] = FieldUpdatePlSizeChange;
                mPriceLevelUpdaters[j++] = FieldUpdatePlSizeChange;
            }  
            if (MamdaOrderBookFields.PL_TIME != null)
            {
                mBookUpdaters[i++] = FieldUpdatePlTime;
                mPriceLevelUpdaters[j++] = FieldUpdatePlTime;
            }  
            if (MamdaOrderBookFields.PL_NUM_ENTRIES != null)
            {
                mBookUpdaters[i++] = FieldUpdatePlNumEntries;
                mPriceLevelUpdaters[j++] = FieldUpdatePlNumEntries;
            }    
            if (MamdaOrderBookFields.PL_NUM_ATTACH != null)
            {
                mBookUpdaters[i++] = FieldUpdatePlNumAttach;
                mPriceLevelUpdaters[j++] = FieldUpdatePlNumAttach;
            }   
            if (MamdaOrderBookFields.PL_ENTRIES != null)
            {
                mBookUpdaters[i++] = FieldUpdateEntries;
                mPriceLevelUpdaters[j++] = FieldUpdateEntries;
            } 
            
            if (MamdaOrderBookFields.ENTRY_ID != null)
            {
                mBookUpdaters[i++] = FieldUpdateEntryId;
                mPriceLevelUpdaters[j++] = FieldUpdateEntryId;
                mEntryUpdaters[k++] = FieldUpdateEntryId;
            }
            if (MamdaOrderBookFields.ENTRY_SIZE != null)
            {
                mBookUpdaters[i++] = FieldUpdateEntrySize;
                mPriceLevelUpdaters[j++] = FieldUpdateEntrySize;
                mEntryUpdaters[k++] = FieldUpdateEntrySize;
            }
            if (MamdaOrderBookFields.ENTRY_TIME != null)
            {
                mBookUpdaters[i++] = FieldUpdateEntryTime;
                mPriceLevelUpdaters[j++] = FieldUpdateEntryTime;
                mEntryUpdaters[k++] = FieldUpdateEntryTime;
            }         
            if (MamdaOrderBookFields.ENTRY_ACTION != null)
            {
                mBookUpdaters[i++] = FieldUpdateEntryAction;
                mPriceLevelUpdaters[j++] = FieldUpdateEntryAction;
                mEntryUpdaters[k++] = FieldUpdateEntryAction;
            }                           
            if (MamdaOrderBookFields.ENTRY_REASON != null)
            {
                mBookUpdaters[i++] = FieldUpdateEntryReason;
                mPriceLevelUpdaters[j++] = FieldUpdateEntryReason;
                mEntryUpdaters[k++] = FieldUpdateEntryReason;
            }       
            if (MamdaOrderBookFields.BOOK_PROPERTIES != null)
            {
                mBookUpdaters[i++] = FieldUpdateBookProperties;
                mPriceLevelUpdaters[j++] = FieldUpdateBookProperties;
                mEntryUpdaters[k++] = FieldUpdateBookProperties;
            }        
            if (MamdaOrderBookFields.PL_PROPERTIES != null)
            {
                mBookUpdaters[i++] = FieldUpdatePlProperties;
                mPriceLevelUpdaters[j++] = FieldUpdatePlProperties;
                mEntryUpdaters[k++] = FieldUpdatePlProperties;
            }        
            if (MamdaOrderBookFields.ENTRY_PROPERTIES != null)
            {
                mBookUpdaters[i++] = FieldUpdateEntryProperties;
                mPriceLevelUpdaters[j++] = FieldUpdateEntryProperties;
                mEntryUpdaters[k++] = FieldUpdateEntryProperties;
            }       
            if (MamdaOrderBookFields.BOOK_PROP_MSG_TYPE != null)
            {
                mBookUpdaters[i++] = FieldUpdateBookPropMsgType;
                mPriceLevelUpdaters[j++] = FieldUpdateBookPropMsgType;
                mEntryUpdaters[k++] = FieldUpdateBookPropMsgType;
            }                         
            if (MamdaOrderBookFields.ENTRY_PROP_MSG_TYPE != null)
            {
                mBookUpdaters[i++] = FieldUpdateEntryPropMsgType;
                mPriceLevelUpdaters[j++] = FieldUpdateEntryPropMsgType;
                mEntryUpdaters[k++] = FieldUpdateEntryPropMsgType;
            }       
            if (MamdaOrderBookFields.ENTRY_STATUS != null)
            {
                mBookUpdaters[i++] = FieldUpdateEntryStatus;
                mPriceLevelUpdaters[j++] = FieldUpdateEntryStatus;
                mEntryUpdaters[k++] = FieldUpdateEntryStatus;
            } 
            if (MamdaOrderBookFields.getNumLevelFields ()> 0)
            {
                mBookUpdaters[i++] = FieldUpdateLevel;
            }             
            if (MamdaOrderBookFields.getNumEntryFields() > 0)
            {
                mPriceLevelUpdaters[j++] = FieldUpdateEntry;
            }   
        }
    }
    
    private static BookMsgUpdate FieldUpdateSenderId = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryU64 (null, MamdaCommonFields.SENDER_ID.getFid(), impl.mBookMsgFields.mSenderId);
        }
    };

    private static BookMsgUpdate FieldUpdateEntry = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
                if  (msg.tryMsg (MamdaOrderBookFields.PL_ENTRY[1], impl.tmpMsg))
                    impl.mBookMsgFields.mEntries.add(impl.tmpMsg.getValue());
        }
    };

    private static BookMsgUpdate FieldUpdateLevel = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {           
                if  (msg.tryMsg (MamdaOrderBookFields.PRICE_LEVEL[1], impl.tmpMsg))
                    impl.mBookMsgFields.mPriceLevels.add(impl.tmpMsg.getValue());
        }
    };

    private static BookMsgUpdate FieldUpdateMsgSeqNum = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryU32 (null, MamdaCommonFields.MSG_SEQ_NUM.getFid(), impl.mBookMsgFields.mMsgSeqNum);
        }
    };

    private static BookMsgUpdate FieldUpdateMsgNum = new BookMsgUpdate ()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryI16 (null, MamdaCommonFields.MSG_NUM.getFid(), impl.mBookMsgFields.mMsgNum);
        }
    };

    private static BookMsgUpdate FieldUpdateMsgTotal = new BookMsgUpdate ()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryI16 (null, MamdaCommonFields.MSG_TOTAL.getFid(), impl.mBookMsgFields.mMsgTotal);
        }
    };

    private static BookMsgUpdate FieldUpdateSymbol = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            /* We only need to set this value once for a book.
               Annoyingly the stl string will return a 0 length char* instead of
               a NULL pointer when the string has no value*/
            MamdaOrderBook book = impl.mFullBook;
            if (book.getSymbol().length () == 0)
            {
                if (msg.tryString (null, MamdaCommonFields.SYMBOL.getFid(), impl.tmpString))
                {
                    book.setSymbol (impl.tmpString.getValue());
                }
            }
        }
    };

    private static BookMsgUpdate FieldUpdatePartId = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            // Not in C++
        }
    };

    private static BookMsgUpdate FieldUpdateSrcTime = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryDateTime (null, MamdaCommonFields.SRC_TIME.getFid(), impl.mBookMsgFields.mSrcTime);
        }
    };

    private static BookMsgUpdate FieldUpdateLineTime = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryDateTime (null, MamdaCommonFields.LINE_TIME.getFid(), impl.mBookMsgFields.mLineTime);
        }
    };

    private static BookMsgUpdate FieldUpdateBookTime = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryDateTime (null, MamdaOrderBookFields.BOOK_TIME.getFid(), impl.mBookMsgFields.mBookTime);
        }
    };

    private static BookMsgUpdate FieldUpdateSendTime = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
           msg.tryDateTime (null, MamdaCommonFields.SEND_TIME.getFid(), impl.mBookMsgFields.mSendTime);
        }
    };
    
    private static BookMsgUpdate FieldUpdateActivityTime = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryDateTime (null, MamdaCommonFields.ACTIVITY_TIME.getFid(), impl.mBookMsgFields.mActivityTime);  
        }
    };
    
    private static BookMsgUpdate FieldUpdateNumLevels = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryU32 (null, MamdaOrderBookFields.NUM_LEVELS.getFid(), impl.mBookMsgFields.mNumLevels);
        }
    };

    private static BookMsgUpdate FieldUpdatePriceLevels = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryArrayMsg (null, MamdaOrderBookFields.PRICE_LEVELS.getFid(), 
                             impl.mBookMsgFields.mPriceLevelVector);
        }
    };
    
    private static BookMsgUpdate FieldUpdateBidMarketOrders = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (impl.mProcessMarketOrders)
            {
                MamaArrayMsg  arrayMsg = new MamaArrayMsg();
                MamaMsg       msgCopy  = new MamaMsg();

                if (msg.tryArrayMsg (MamdaOrderBookFields.BID_MARKET_ORDERS, arrayMsg))
                {
                    msgCopy = arrayMsg.getValue()[0];
                    impl.mBookMsgFields.mBidMarketOrders.copy (msgCopy);
                    impl.mBookMsgFields.mHasMarketOrders = true;
                }
            }
        }
    };
    
    private static BookMsgUpdate FieldUpdateAskMarketOrders = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (impl.mProcessMarketOrders)
            {
                MamaArrayMsg  arrayMsg = new MamaArrayMsg();
                MamaMsg       msgCopy  = new MamaMsg();

                if (msg.tryArrayMsg (MamdaOrderBookFields.ASK_MARKET_ORDERS, arrayMsg))
                {
                    msgCopy = arrayMsg.getValue()[0];
                    impl.mBookMsgFields.mAskMarketOrders.copy(msgCopy); 
                    impl.mBookMsgFields.mHasMarketOrders = true;
                }
            }
        }
    };

    private static BookMsgUpdate FieldUpdatePlPrice = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryF64 (null, MamdaOrderBookFields.PL_PRICE.getFid(), impl.mBookMsgFields.mPlPrice);
            double roundPrice = Math.floor(
                    (100000000*impl.mBookMsgFields.mPlPrice.getValue()) + 0.5) / 100000000;
                    impl.mBookMsgFields.mPlPrice.setValue (roundPrice);
        }
    };

    private static BookMsgUpdate FieldUpdatePlSide = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            impl.tmpField = msg.getField(null, MamdaOrderBookFields.PL_SIDE.getFid(), null);
            if (impl.tmpField != null)
            {
                switch (impl.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8 :
                    case MamaFieldDescriptor.CHAR :
                    case MamaFieldDescriptor.U8 :
                        impl.mBookMsgFields.mPlSide.setValue(impl.tmpField.getChar ());
                        break;
                    case MamaFieldDescriptor.STRING :
                        if (impl.tmpField.getString().length() > 0)
                        {
                            impl.mBookMsgFields.mPlSide.setValue(impl.tmpField.getString().charAt(0));
                        }
                        else
                        {
                            impl.mBookMsgFields.mPlSide.setValue( ' ');
                        }
                        break;
                    default: 
                        break;
                }
            }
        }
    };

    private static BookMsgUpdate FieldUpdatePlAction = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            impl.tmpField = msg.getField(null, MamdaOrderBookFields.PL_ACTION.getFid(), null);
            if (impl.tmpField != null)
            {
                impl.mBookMsgFields.mHasPlAction = true;
                switch (impl.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8 :
                    case MamaFieldDescriptor.CHAR :
                    case MamaFieldDescriptor.U8 :
                        impl.mBookMsgFields.mPlAction.setValue(impl.tmpField.getChar ());
                        break;
                    case MamaFieldDescriptor.STRING :
                        if (impl.tmpField.getString().length() > 0)
                        {
                            impl.mBookMsgFields.mPlAction.setValue(impl.tmpField.getString().charAt(0));
                        }
                        else
                        {
                            impl.mBookMsgFields.mPlAction.setValue( ' ');
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    };

    private static BookMsgUpdate FieldUpdatePlSize = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (msg.tryF64 (null, MamdaOrderBookFields.PL_SIZE.getFid(), impl.mBookMsgFields.mPlSize))
                impl.mBookMsgFields.mHasPlSize = true;
        }
    };

    private static BookMsgUpdate FieldUpdatePlSizeChange = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (msg.tryF64 (null, MamdaOrderBookFields.PL_SIZE_CHANGE.getFid(), 
                            impl.mBookMsgFields.mPlSizeChange))
                impl.mBookMsgFields.mHasPlSizeChange = true;
        }
    };

    private static BookMsgUpdate FieldUpdatePlTime = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (msg.tryDateTime (null, MamdaOrderBookFields.PL_TIME.getFid(), impl.mBookMsgFields.mPlTime))
                impl.mBookMsgFields.mHasPlTime = true;
        }
    };

    private static BookMsgUpdate FieldUpdatePlNumEntries = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            msg.tryF32 (null, MamdaOrderBookFields.PL_NUM_ENTRIES.getFid(), impl.mBookMsgFields.mPlNumEntries);
        }
    };

    private static BookMsgUpdate FieldUpdatePlNumAttach = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (impl.mProcessEntries || (impl.mBookMsgFields.mBookType.getValue() == 1))
            {
                msg.tryU32 (null, MamdaOrderBookFields.PL_NUM_ATTACH.getFid(), 
                            impl.mBookMsgFields.mPlNumAttach);
            }
        }
    };

    private static BookMsgUpdate FieldUpdateEntries = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (impl.mProcessEntries || (impl.mBookMsgFields.mBookType.getValue() == 1))
            {   
                msg.tryArrayMsg (null, MamdaOrderBookFields.PL_ENTRIES.getFid(), 
                                 impl.mBookMsgFields.mEntryVector);
            }
        }
    };

    private static BookMsgUpdate FieldUpdateEntryId = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (impl.mProcessEntries || (impl.mBookMsgFields.mBookType.getValue() == 1))
            {
                msg.tryString (null, MamdaOrderBookFields.ENTRY_ID.getFid(), 
                               impl.mBookMsgFields.mEntryId);
            }
        }
    };

    private static BookMsgUpdate FieldUpdateEntrySize = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (impl.mProcessEntries || (impl.mBookMsgFields.mBookType.getValue() == 1))
            {
                msg.tryF64 (null, MamdaOrderBookFields.ENTRY_SIZE.getFid(), 
                            impl.mBookMsgFields.mEntrySize);
            }
        }
    };

    private static BookMsgUpdate FieldUpdateEntryTime = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (impl.mProcessEntries || (impl.mBookMsgFields.mBookType.getValue() == 1))
            {
                msg.tryDateTime (null, MamdaOrderBookFields.ENTRY_TIME.getFid(), 
                                 impl.mBookMsgFields.mEntryTime);
            }
        }
    };

    private static BookMsgUpdate FieldUpdateEntryAction = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (impl.mProcessEntries || (impl.mBookMsgFields.mBookType.getValue() == 1))
            {
                impl.tmpField = msg.getField(null, MamdaOrderBookFields.ENTRY_ACTION.getFid(), null);
                if (impl.tmpField != null)
                {
                    switch (impl.tmpField.getType())
                    {
                        case MamaFieldDescriptor.I8 :
                        case MamaFieldDescriptor.CHAR :
                        case MamaFieldDescriptor.U8 :
                            impl.mBookMsgFields.mEntryAction.setValue(impl.tmpField.getChar ());
                            break;
                        case MamaFieldDescriptor.STRING :
                            if (impl.tmpField.getString().length() > 0)
                            {
                                impl.mBookMsgFields.mEntryAction.setValue(
                                                            impl.tmpField.getString().charAt(0));
                            }
                            else
                            {
                                impl.mBookMsgFields.mEntryAction.setValue( ' ');
                            }
                            break;
                        default: 
                            break;
                    }
                }
            }
        }
    };

    private static BookMsgUpdate FieldUpdateEntryStatus = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (impl.mProcessEntries || (impl.mBookMsgFields.mBookType.getValue() == 1))
            {
                msg.tryU16 (null, MamdaOrderBookFields.ENTRY_STATUS.getFid(), 
                            impl.mBookMsgFields.mEntryStatus);
            }
        }
    };

    private static BookMsgUpdate FieldUpdateEntryReason = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            if (impl.mProcessEntries || (impl.mBookMsgFields.mBookType.getValue() == 1))
            {
                impl.tmpField = msg.getField(null, MamdaOrderBookFields.ENTRY_REASON.getFid(), null);
                if (impl.tmpField != null)
                {
                    switch (impl.tmpField.getType())
                    {
                        case MamaFieldDescriptor.I8 :
                        case MamaFieldDescriptor.CHAR :
                        case MamaFieldDescriptor.U8 :
                            impl.mBookMsgFields.mEntryReason.setValue(impl.tmpField.getChar ());
                            break;
                        case MamaFieldDescriptor.STRING :
                            if (impl.tmpField.getString().length() > 0)
                            {
                                impl.mBookMsgFields.mEntryReason.setValue(impl.tmpField.getString().charAt(0));
                            }
                            else
                            {
                                impl.mBookMsgFields.mEntryReason.setValue( ' ');
                            }
                            break;
                        default: 
                            break;
                    }
                }
            }
        }
    };

    private static BookMsgUpdate FieldUpdateBookProperties = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            impl.mBookMsgFields.mBookProptyFidsChanged.setValue(
                msg.tryArrayU16 (null, MamdaOrderBookFields.BOOK_PROPERTIES.getFid(), impl.mBookMsgFields.mBookPropertyFids)
                );
        }
    };

    private static BookMsgUpdate FieldUpdatePlProperties = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            impl.mBookMsgFields.mPlProptyFidsChanged.setValue(
                msg.tryArrayU16 (null, MamdaOrderBookFields.PL_PROPERTIES.getFid(), impl.mBookMsgFields.mPlPropertyFids)
                );
        }
    };

    private static BookMsgUpdate FieldUpdateEntryProperties = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            impl.mBookMsgFields.mEntryProptyFidsChanged.setValue(
                msg.tryArrayU16 (null, MamdaOrderBookFields.ENTRY_PROPERTIES.getFid(), impl.mBookMsgFields.mEntryPropertyFids)
                );
        }
    };

    private static BookMsgUpdate FieldUpdateBookPropMsgType = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            impl.tmpField = msg.getField (MamdaOrderBookFields.BOOK_PROP_MSG_TYPE, null);
             if (impl.tmpField != null)
             {
                switch (impl.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8 :
                    case MamaFieldDescriptor.CHAR :
                    case MamaFieldDescriptor.U8 :
                        impl.mBookMsgFields.mBookPropMsgType.setValue(impl.tmpField.getChar ());
                        break;
                    case MamaFieldDescriptor.STRING :
                        if (impl.tmpField.getString().length() > 0)
                        {
                            impl.mBookMsgFields.mBookPropMsgType.setValue(impl.tmpField.getString().charAt(0));
                        }
                        else
                        {
                            impl.mBookMsgFields.mBookPropMsgType.setValue( ' ');
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    };

    private static BookMsgUpdate FieldUpdatePlPropMsgType = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            impl.tmpField = msg.getField (MamdaOrderBookFields.PL_PROP_MSG_TYPE, null);
            if (impl.tmpField != null)
            {
                switch (impl.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8 :
                    case MamaFieldDescriptor.CHAR :
                    case MamaFieldDescriptor.U8 :
                        impl.mBookMsgFields.mPlPropMsgType.setValue(impl.tmpField.getChar ());
                        break;
                    case MamaFieldDescriptor.STRING :
                        if (impl.tmpField.getString().length() > 0)
                        {
                            impl.mBookMsgFields.mPlPropMsgType.setValue(impl.tmpField.getString().charAt(0));
                        }
                        else
                        {
                            impl.mBookMsgFields.mPlPropMsgType.setValue( ' ');
                        }
                        break;
                    default: 
                        break;
                }
            }
        }
    };

    private static BookMsgUpdate FieldUpdateEntryPropMsgType = new BookMsgUpdate()
    {
        public void onUpdate (MamaMsg msg, MamdaOrderBookListener impl)
        {
            impl.tmpField = msg.getField (MamdaOrderBookFields.ENTRY_PROP_MSG_TYPE, null);
            if (impl.tmpField != null)
            {           
                switch (impl.tmpField.getType())
                {
                    case MamaFieldDescriptor.I8 :
                    case MamaFieldDescriptor.CHAR :
                    case MamaFieldDescriptor.U8 :
                        impl.mBookMsgFields.mEntryPropMsgType.setValue(impl.tmpField.getChar ());
                        break;
                    case MamaFieldDescriptor.STRING :
                        if (impl.tmpField.getString().length() > 0)
                        {
                            impl.mBookMsgFields.mEntryPropMsgType.setValue(impl.tmpField.getString().charAt(0));
                        }
                        else
                        {
                            impl.mBookMsgFields.mEntryPropMsgType.setValue( ' ');
                        }
                        break;
                    default: 
                        break;
                }
            }
        }
    };

    private static class BookMsgFields
    {
        MamaShort           mMsgNum           = new MamaShort((short)1);
        MamaShort           mMsgTotal         = new MamaShort((short)1);

        // The following fields are used for caching the order book and
        // related fields.  These fields can be used by applications for
        // reference and will be passed for recaps.
        MamaDateTime        mSrcTime          = new MamaDateTime ();
        MamaDateTime        mLineTime         = new MamaDateTime ();
        MamaDateTime        mSendTime         = new MamaDateTime ();
        MamaDateTime        mActivityTime     = new MamaDateTime ();
        MamaShort           mMsgQual          = new MamaShort    (MamaQuality.QUALITY_UNKNOWN);
        MamaDateTime        mBookTime         = new MamaDateTime ();
        MamaLong            mMsgSeqNum        = new MamaLong     (0);
        MamaLong            mSenderId         = new MamaLong     (0);

        // PriceLevels:
        MamaArrayMsg        mPriceLevelVector = new MamaArrayMsg ();
        ArrayList           mPriceLevels      = null; // For RV messages
        MamaLong            mNumLevels        = new MamaLong     (1);
        MamaDouble          mPlPrice          = new MamaDouble   (0.0);
        MamaChar            mPlSide           = new MamaChar     (MamdaOrderBookPriceLevel.SIDE_BID);
        MamaDateTime        mPlTime           = new MamaDateTime ();
        MamaChar            mPlAction         = new MamaChar     (MamdaOrderBookPriceLevel.ACTION_ADD);
        MamaDouble          mPlSize           = new MamaDouble   (0.0);
        MamaDouble          mPlSizeChange     = new MamaDouble   (0.0);
        MamaFloat           mPlNumEntries     = new MamaFloat    (1);
        MamaMsg             mBidMarketOrders  = new MamaMsg      ();
        MamaMsg             mAskMarketOrders  = new MamaMsg      ();
        boolean             mHasMarketOrders  = false;

        // Entries:
        MamaArrayMsg        mEntryVector      = new MamaArrayMsg ();
        ArrayList           mEntries          = null; // For RV messages
        MamaLong            mPlNumAttach      = new MamaLong     (1);
        MamaString          mEntryId          = new MamaString   ();
        MamaDouble          mEntrySize        = new MamaDouble   (0.0);
        MamaDateTime        mEntryTime        = new MamaDateTime ();
        MamaInteger         mEntryStatus      = new MamaInteger  (0);
        MamaChar            mEntryAction      = 
            new MamaChar(MamdaOrderBookEntry.ACTION_DELETE);
        MamaChar            mEntryReason      = 
            new MamaChar(MamdaOrderBookTypes.MAMDA_BOOK_REASON_UNKNOWN);

        MamaArrayInt        mPlPropertyFids         = new MamaArrayInt ();
        MamaBoolean         mPlProptyFidsChanged    = new MamaBoolean  (false);
        MamaChar            mPlPropMsgType          = 
            new MamaChar(MamdaOrderBookTypes.MAMDA_PROP_MSG_TYPE_UPDATE);

        MamaArrayInt        mEntryPropertyFids      = new MamaArrayInt ();
        MamaBoolean         mEntryProptyFidsChanged = new MamaBoolean  (false);
        MamaChar            mEntryPropMsgType       = new MamaChar(
            MamdaOrderBookTypes.MAMDA_PROP_MSG_TYPE_UPDATE);

        MamaArrayInt        mBookPropertyFids       = new MamaArrayInt ();
        MamaBoolean         mBookProptyFidsChanged  = new MamaBoolean  (false);
        MamaChar            mBookPropMsgType        = 
            new MamaChar(MamdaOrderBookTypes.MAMDA_PROP_MSG_TYPE_UPDATE);

        MamaInteger         mBookType          = new MamaInteger(0);
        boolean             mHasPlSize         = false; 
        boolean             mHasPlSizeChange   = false;
        boolean             mHasPlTime         = false;
        boolean             mHasPlAction       = false;
        
        BookMsgFields()
        {
            mEntries           =
                new ArrayList (MamdaOrderBookFields.getNumEntryFields ());
            mPriceLevels       =
                new ArrayList (MamdaOrderBookFields.getNumLevelFields ());
            clear();
        }

        void printFields()
        {
            System.out.println ("mMsgNum = "        + mMsgNum.getValue      ());   
            System.out.println ("mMsgTotal = "      + mMsgTotal.getValue    ());
            System.out.println ("mMsgQual = "       + mMsgQual.getValue     ());
            System.out.println ("mMsgSeqNum = "     + mMsgSeqNum.getValue   ());
            System.out.println ("mSenderId = "      + mSenderId.getValue    ());
            System.out.println ("mNumLevels = "     + mNumLevels.getValue   ());
            System.out.println ("mPlPrice = "       + mPlPrice.getValue     ());
            System.out.println ("mPlSide = "        + mPlSide.getValue      ());
            System.out.println ("mPlAction = "      + mPlAction.getValue    ());
            System.out.println ("mPlSize = "        + mPlSize.getValue      ());
            System.out.println ("mPlSizeChange = "  + mPlSizeChange.getValue());
            System.out.println ("mPlNumEntries = "  + mPlNumEntries.getValue());
            System.out.println ("mPlNumAttach = "   + mPlNumAttach.getValue ());
            System.out.println ("mNumLevels = "     + mNumLevels.getValue   ());

        }
        
        void clearEntry()
        {
            mEntryId.setValue       (null);
            mEntrySize.setValue     (0.0);
            mEntryStatus.setValue   (0);
            mEntryAction.setValue   (MamdaOrderBookEntry.ACTION_DELETE);
            mEntryReason.setValue   (MamdaOrderBookTypes.MAMDA_BOOK_REASON_UNKNOWN);
            mEntryTime.clear        ();
        }
        
        void clearPriceLevel()
        {
            mPlPrice.setValue       (0.0);
            mPlSize.setValue        (0.0);
            mPlSizeChange.setValue  (0.0);
            mPlSide.setValue        (MamdaOrderBookPriceLevel.SIDE_BID);
            mPlAction.setValue      (MamdaOrderBookPriceLevel.ACTION_ADD);
            mPlNumEntries.setValue  (1);
            mPlNumAttach.setValue   (1);
            mEntryVector.setValue   (null);
            mPlTime.clear           ();
            mHasPlSize              = false;
            mHasPlSizeChange        = false;
            mHasPlAction            = false;      
            mHasPlTime              = false;
        }
        
        void clear()
        {                           
            mMsgSeqNum.setValue     (0);
            mSenderId.setValue      (0);
            mMsgNum.setValue        ((short)1);
            mMsgTotal.setValue      ((short)1);
            mPlPrice.setValue       (0.0);
            mPlSize.setValue        (0.0);
            mPlSizeChange.setValue  (0.0);
            mPlSide.setValue        (MamdaOrderBookPriceLevel.SIDE_BID);
            mPlAction.setValue      (MamdaOrderBookPriceLevel.ACTION_ADD);
            mNumLevels.setValue     (1);
            mPlNumEntries.setValue  (1);
            mPlNumAttach.setValue   (1);
            mEntryId.setValue       (null);
            mEntrySize.setValue     (0.0);
            mEntryStatus.setValue   (0);
            mEntryAction.setValue   (MamdaOrderBookEntry.ACTION_DELETE);
            mEntryReason.setValue   (MamdaOrderBookTypes.MAMDA_BOOK_REASON_UNKNOWN);
            mLineTime.clear         ();
            /* Must be initialized to NULL. Used to determine whether the message is
             * flattened or not*/
            mPriceLevelVector.setValue       (null);
            mEntryVector.setValue            (null);
            mPriceLevels.clear               ();
            mEntries.clear                   ();
            mPlProptyFidsChanged.setValue    (false);
            mEntryProptyFidsChanged.setValue (false);
            mBookProptyFidsChanged.setValue  (false);
            mPlPropMsgType.setValue          (MamdaOrderBookTypes.MAMDA_PROP_MSG_TYPE_UPDATE);
            mEntryPropMsgType.setValue       (MamdaOrderBookTypes.MAMDA_PROP_MSG_TYPE_UPDATE);
            mBookPropMsgType.setValue        (MamdaOrderBookTypes.MAMDA_PROP_MSG_TYPE_UPDATE);
            mSrcTime.clear                   ();
            mBookTime.clear                  ();
            mPlTime.clear                    ();
            mEntryTime.clear                 ();
            mActivityTime.clear              ();
            mBidMarketOrders.clear           ();
            mAskMarketOrders.clear           ();
            mHasMarketOrders =               false;
            mBookType.setValue               (0);
            mHasPlSize =                     false;
            mHasPlSizeChange                 = false;
            mHasPlAction                     = false;      
            mHasPlTime                       = false;
        }
    }

    private static interface BookMsgUpdate
    {
        abstract void onUpdate (MamaMsg msg, MamdaOrderBookListener impl);
    }

    private class SimpleDeltaImpl extends MamdaOrderBookSimpleDelta
    {
        public MamaDateTime getSrcTime ()
        {
            return MamdaOrderBookListener.this.getSrcTime ();
        }

        public MamaDateTime getActivityTime ()
        {
            return MamdaOrderBookListener.this.getActivityTime ();
        }

        public long getEventSeqNum ()
        {
            return MamdaOrderBookListener.this.getEventSeqNum ();
        }

        public MamaDateTime getEventTime ()
        {
            return MamdaOrderBookListener.this.getEventTime ();
        }
        public short getSrcTimeFieldState ()
        {
            return MamdaOrderBookListener.this.getSrcTimeFieldState ();
        }

        public short getActivityTimeFieldState ()
        {
            return MamdaOrderBookListener.this.getActivityTimeFieldState ();
        }

        public short getEventSeqNumFieldState ()
        {
            return MamdaOrderBookListener.this.getEventSeqNumFieldState ();
        }

        public short getEventTimeFieldState ()
        {
            return MamdaOrderBookListener.this.getEventTimeFieldState ();
        }
    }

    private class ComplexDeltaImpl extends MamdaOrderBookComplexDelta
    {
        public MamaDateTime getSrcTime ()
        {
            return MamdaOrderBookListener.this.getSrcTime ();
        }

        public MamaDateTime getActivityTime ()
        {
            return MamdaOrderBookListener.this.getActivityTime ();
        }

        public long getEventSeqNum ()
        {
            return MamdaOrderBookListener.this.getEventSeqNum ();
        }

        public MamaDateTime getEventTime ()
        {
            return MamdaOrderBookListener.this.getEventTime ();
        }
        public short getSrcTimeFieldState ()
        {
          return MamdaOrderBookListener.this.getSrcTimeFieldState ();
        }

        public short getActivityTimeFieldState ()
        {
          return MamdaOrderBookListener.this.getActivityTimeFieldState ();
        }

        public short getEventSeqNumFieldState ()
        {
          return MamdaOrderBookListener.this.getEventSeqNumFieldState ();
        }

        public short getEventTimeFieldState ()
        {
          return MamdaOrderBookListener.this.getEventTimeFieldState ();
        }
    }
}
