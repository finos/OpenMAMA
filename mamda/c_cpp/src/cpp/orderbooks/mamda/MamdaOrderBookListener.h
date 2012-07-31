/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
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

#ifndef MamdaOrderBookListenerH
#define MamdaOrderBookListenerH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    class MamdaOrderBookHandler;

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
     * provide full-book related callbacks, even when the publisher
     * (e.g., feed handler) is only publishing deltas containing modified
     * fields.
     */

    class MAMDAOPTExpDLL MamdaOrderBookListener : public MamdaMsgListener
    {
    public:
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
        MamdaOrderBookListener (MamdaOrderBook*  fullBook = NULL);

        virtual ~MamdaOrderBookListener ();

        /**
         * Add a specialized order book handler.  Currently, only one
         * handler can (and must) be registered.
         *
         * @param handler The handler registered to receive order book update
         * callbacks.
         */
        virtual void addHandler (MamdaOrderBookHandler* handler);

        /**
         * Set whether we are interested in "entry level" information at
         * all.  Many applications only care about price level
         * information.  (Default is to process entry level information.)
         *
         * @param process Whether to process entries in books.
         */
        virtual void setProcessEntries (bool  process);


        /**
         * Get whether we are interested in "entry level" information
         *
         @return true if we are set to process entries
         */
        virtual bool getProcessEntries () const;
        
        /**
         * Set whether we are interested in market order information, if
         * available. 
         *
         * @param process Whether to process market order data. 
         */
        virtual void setProcessMarketOrders (bool  process);

         /**
         * Get whether we are interested in market order information.
         *
         @return true if we are set to process market orders
         */
        virtual bool getProcessMarketOrders () const;
        
        /**
         * Add an entry ID to ignore.  This only makes sense when the
         * entry type is participant ID (as opposed to order ID).  All
         * order book updates for this entry ID will be ignored.
         *
         * @param id The id of the participant to ignore when process book
         * updates.
         */
        virtual void  addIgnoreEntryId (const char*  id);

        /**
         * Remove an entry ID to ignore.
         *
         * @param id The id of the participant whose update will be subsequently
         * processed as part of the book.
         *
         * @see addIgnoreEntryId(const char*)
         */
        virtual void  removeIgnoreEntryId (const char*  id);

        /**
         * Clear all cached data fields.
         */
        virtual void  clear ();

        /**
         * Remove the reference of handlers from the internal list.
         * Memory is not freed.
         */
        virtual void  removeHandlers ();

        // Inherited from MamdaBasicRecap and MamdaBasicEvent
        virtual const char*          getSymbol         () const;
        virtual const char*          getPartId         () const;
        virtual const MamaDateTime&  getSrcTime        () const;
        virtual const MamaDateTime&  getActivityTime   () const;
        virtual const MamaDateTime&  getLineTime       () const;
        virtual const MamaDateTime&  getSendTime       () const;
        virtual const MamaMsgQual&   getMsgQual        () const;
        virtual const MamaDateTime&  getEventTime      () const;
        virtual mama_seqnum_t        getEventSeqNum    () const;

        virtual MamdaFieldState      getSymbolFieldState()       const;
        virtual MamdaFieldState      getPartIdFieldState()       const;
        virtual MamdaFieldState      getEventSeqNumFieldState()  const;
        virtual MamdaFieldState      getEventTimeFieldState()    const;
        virtual MamdaFieldState      getSrcTimeFieldState()      const;
        virtual MamdaFieldState      getActivityTimeFieldState() const;
        virtual MamdaFieldState      getLineTimeFieldState()     const;
        virtual MamdaFieldState      getSendTimeFieldState()     const;
        virtual MamdaFieldState      getMsgQualFieldState()      const;
        
        // Inherited from MamdaOrderBookRecap
        virtual const MamdaOrderBook*  getOrderBook    () const;
        virtual MamdaOrderBook*        getOrderBook    ();
        // Inherited from MamdaOrderBookGap
        virtual mama_seqnum_t        getBeginGapSeqNum () const;
        virtual mama_seqnum_t        getEndGapSeqNum   () const;

        /**
         * Set whether to use an "entry manager" for finding entries in a
         * book.
         */
        virtual void  setUseEntryManager (bool  useManager);

        /**
         * Set whether the entry IDs are unique across the entire book.
         * This is only relevant if using an "entry manager".
         */
        virtual void  setEntryIdsAreUnique (bool  uniqueIds);

        /**
         * Set whether to gather individual basic deltas when processing
         * a complex delta.  Many applications don't need the basic
         * deltas and will iterate over part or all of the full book (with
         * the deltas already applied).
         */
        virtual void  setKeepBasicDeltas (bool  keep);

        /**
         * Whether to handle or ignore updates sent for an inconsistent
         * book.  A book may be in an inconsistent state if there has been
         * a gap on the sequence of update (delta) messages.  Default is
         * to not update (and wait for a recap).
         *
         * @param update Whether to apply updates to an inconsistent book.
         */
        virtual void  setUpdateInconsistentBook (bool  update);

        /**
         * Whether to handle or ignore updates sent for a stale order
         * book.  A book is in a stale state if its mamaQuality is not
         * MAMA_QUALITY_OK..
         *
         * @param update Whether to apply updates to a stale book.
         */
        virtual void  setUpdateStaleBook (bool  update);

        /**
         * Whether to clear the order book upon detection of a stale book
         * condition.  If this is set to true, then setUpdateStaleBook()
         * should probably have been set to false.
         */
        virtual void  setClearStaleBook (bool  clear);

        /**
         * Change the order book quality and invoke appropriate callbacks.
         */
        virtual void  setQuality (MamdaSubscription*  sub,
                                  mamaQuality         quality);
        /**
         * Whether to conflate the order book deltas
         *
         * @param conflate Whether to conflate the order book deltas
         */
        virtual void  setConflateDeltas  (bool conflate);

        /**
         * Whether or not order book deltas are conflated
         *
         * @return true if conflation is set to true
         */
        virtual bool  getConflateDeltas  ();
        
        /**
         * The conflation interval
         *
         * @param interval The conflation interval in seconds. Default is 0.5
         * seconds. 
         */
        virtual void  setConflationInterval (double interval);

        /**
         * Invoke delta handlers immediately if there is a conflated delta
         * pending 
         */
        virtual void  forceInvokeDeltaHandlers ();

         /**
         * Clear any pending conflated deltas
         */
        virtual void  clearConflatedDeltas ();
      
        /**
         * Implementation of MamdaListener interface.
         */
        virtual void onMsg (MamdaSubscription*  subscription,
                            const MamaMsg&      msg,
                            short               msgType);

        /**
         * Acquire a read lock on the full book, so that an alternative
         * running thread can view the book without it being changed
         * from within the MamdaBookListener. releaseReadLock should be called
         * to release the lock.
         */
        virtual void acquireReadLock ();
        
        /**
         * Release the read lock on the full book. A companion to 
         * the acquireReadLock method.
         */
        virtual void releaseReadLock ();

        virtual void acquireWriteLock ();
        virtual void releaseWriteLock ();
        
        /**
         * A thread safe function that can be called to populate a
         * snapshot of the full book for the given symbol providing there
         * already exists a MamdaBookListener for the symbol within the
         * process.
         *
         * This routine will throw a MamdaOrderBookException in the following
         * three circumstances:
         * - If the Mamda API hasn't been built with multi-threading enabled, or
         * - If there is no listener within the process for the given symbol, or
         * - The listener hasn't as yet received an initial image.
         *
         * @throw <MamdaOrderBookException> See above for details.
         */
        virtual void getBookSnapShot (MamdaOrderBook&  result);

        /**
         * Set the FIDs for any additional fields to cached in the
         * book properties
         * for the book.
         * 
         * @param fids the array of FIDs to cache
         * @param numFids the size of the array
         */
        virtual void setBookPropertyFids (mama_fid_t* fids, mama_size_t numFids);

        /**
         * Set the FIDs for any additional fields to cached in the
         * price level properties
         * for the book.
         * 
         * @param fids the array of FIDs to cache
         * @param numFids the size of the array
         */
        virtual void setLevelPropertyFids (mama_fid_t* fids, mama_size_t numFids);

        /**
         * Set the FIDs for any additional fields to cached in the
         * entry properties
         * for the book.
         * 
         * @param fids the array of FIDs to cache
         * @param numFids the size of the array
         */
        virtual void setEntryPropertyFids (mama_fid_t* fids, mama_size_t numFids);
        
        
        // NOTE: must be public for builds on Solaris to work with changes
        // required for gcc 2.96.
        struct MamdaOrderBookListenerImpl;
        MamdaOrderBookListenerImpl& mImpl;
    };

} // namespace

#endif // MamdaOrderBookListenerH

