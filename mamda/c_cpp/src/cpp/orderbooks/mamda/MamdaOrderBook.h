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

#ifndef MamdaOrderBookH
#define MamdaOrderBookH

#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookExceptions.h>
#include <mamda/MamdaOrderBookEntryFilter.h>
#include <mama/mamacpp.h>
#include <mama/MamaSourceDerivative.h>
#include <iosfwd>

using std::ostream;

namespace Wombat
{

    class MamdaOrderBookBasicDelta;
    class MamdaOrderBookBasicDeltaList;

    /**
     * MamdaOrderBook is a class that provides order book functionality,
     * including iterators over price levels and entries within price
     * levels.
     */
    class MAMDAOPTExpDLL MamdaOrderBook
    {
    public:
        MamdaOrderBook ();
        ~MamdaOrderBook ();

        // Copying and assignment
        MamdaOrderBook (const MamdaOrderBook&);
        MamdaOrderBook& operator= (const MamdaOrderBook&);

        /**
         * Clear the order book entirely.
         *
         * @param deleteLevels If true then the level objects will be deleted
         */
        void clear (bool deleteLevels = true);

        /**
         * The order book subscription symbol.
         *
         * @param symbol The subscription symbol.
         */
        void setSymbol (const char*  symbol);

        /**
         * The orderbook subscription symbol.
         * 
         * @return The orderbook subscription symbol.
         */
        const char*  getSymbol () const;

        /**
         * The orderbook participant id
         * @param partId The participant id
         */
        void setPartId (const char* partId);

        /**
         * The orderbook participant id
         * @return The orderbook participant id
         **/
        const char*  getPartId () const;

        /** 
         * The orderbook participant id
         * @return Whether this orderbook has a participant id
         **/
        bool hasPartId () const;

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
         * @throw <MamdaOrderBookException> When an error is encountered during
         * book processing.
         */
        MamdaOrderBookPriceLevel* findOrCreateLevel (
            double                              price,
            MamdaOrderBookPriceLevel::Side      side);

        MamdaOrderBookPriceLevel* findOrCreateLevel (
            MamaPrice&                          price,
            MamdaOrderBookPriceLevel::Side      side);

        MamdaOrderBookPriceLevel* findOrCreateLevel (
            double                              price,
            MamdaOrderBookPriceLevel::Side      side,
            MamdaOrderBookPriceLevel::Action&   action);

        MamdaOrderBookPriceLevel* findOrCreateLevel (
            MamaPrice&                          price,
            MamdaOrderBookPriceLevel::Side      side,
            MamdaOrderBookPriceLevel::Action&   action);

        MamdaOrderBookPriceLevel* findLevel (
            double                              price,
            MamdaOrderBookPriceLevel::Side      side);

        MamdaOrderBookPriceLevel* findLevel (
            MamaPrice&                          price,
            MamdaOrderBookPriceLevel::Side      side);
            
        /**
         * Add a price level to the orderbook.
         *
         * @param level The price level to add to the orderbook.
         *
         * @throw <MamdaOrderBookException> When an error is encountered during
         * book processing.
         */
        void addLevel    (const MamdaOrderBookPriceLevel&  level);

        /**
         * Update an existing level in the orderbook.
         *
         * @param level The details of the price level to update.
         *
         * @throw <MamdaOrderBookException> When an error is encountered during
         * book processing.
         */
        void updateLevel (const MamdaOrderBookPriceLevel&  level);

        /**
         * Delete a price level from the orderbook.
         *
         * @param level The price level to delete from the orderbook.
         *
         * @throw <MamdaOrderBookException> When an error is encountered during
         * book processing.
         */
        void deleteLevel (const MamdaOrderBookPriceLevel&  level);

        /**
         * Apply a delta to this (presumably) full book.
         *
         * @param deltaBook The delta to apply to the orderbook.
         *
         * @throw <MamdaOrderBookException> When an error is encountered during
         * book processing.
         */
        void apply (const MamdaOrderBook&  deltaBook);

        /**
         * Apply a delta to this book.
         *
         * @param delta The simple delta to apply to the orderbook.
         *
         * @throw <MamdaOrderBookException> When an error is encountered during
         * book processing.
         */
        void apply (const MamdaOrderBookBasicDelta&  delta);

        /**
         * Apply a delta to this book.
         *
         * @param delta The complex delta to apply to the orderbook.
         *
         * @throw <MamdaOrderBookException> When an error is encountered during
         * book processing.
         */
        void apply (const MamdaOrderBookBasicDeltaList&  delta);

        /**
         * Apply a market order delta to this book.
         *
         * @param delta The simple market order delta to apply to the orderbook.
         *
         * @throw <MamdaOrderBookException> When an error is encountered during
         * book processing.
         */
        void applyMarketOrder (const MamdaOrderBookBasicDelta&  delta);

        /**
         * Apply a market order delta to this book.
         *
         * @param delta The market ordercomplex delta to apply to the orderbook.
         *
         * @throw <MamdaOrderBookException> When an error is encountered during
         * book processing.
         */
        void applyMarketOrder (const MamdaOrderBookBasicDeltaList&  delta);
        
        /**
         * Copy a book.
         *
         * @param rhs The orderbook to copy.
         *
         * @throw <MamdaOrderBookException> When an error is encountered during
         * book processing.
         */
        void copy (const MamdaOrderBook&  rhs);

        /**
         * Set this order book to be a delta that would, when applied,
         * delete all of the fields in the bookToDelete.
         */
        void setAsDeltaDeleted (const MamdaOrderBook&  bookToDelete);

        /**
         * Set this order book to be a delta that would, when applied, be
         * the difference between two other books.
         */
        void setAsDeltaDifference (const MamdaOrderBook&  lhs,
                                   const MamdaOrderBook&  rhs);

        /**
         * Get the total number of price levels (both sides of order book).
         *
         * @return The total number of levels in the book.
         */
        size_t getTotalNumLevels() const;

        /**
         * Get the number of bid price levels.
         *
         * @return The total number of bid levels in the book.
         */
        size_t getNumBidLevels() const;

        /**
         * Get the number of ask price levels.
         *
         * @return The total number of ask levels in the book.
         */
        size_t getNumAskLevels() const;

         /**
         * Get the bid market orders.
         *
         * @return The market order bid level.
         */
        MamdaOrderBookPriceLevel* getBidMarketOrders() const;

        /**
         * Get the ask market orders.
         *
         * @return The market order ask level.
         */
        MamdaOrderBookPriceLevel* getAskMarketOrders() const;

        /**
         * Get the market orders for the specified side. Will return NULL if no
         * market orders exist in the book.
         *
         * @param side The market order side the price level to get.
         * @return The market order ask level.
         */
        MamdaOrderBookPriceLevel* getMarketOrdersSide (
            MamdaOrderBookPriceLevel::Side side);
            
        /**
         * Get the market orders for the specified side. Will create an empty level
         * if none exist.
         *
         * @param side The market order side the price level to get.
         * @return The market order ask level.
         */
        MamdaOrderBookPriceLevel* getOrCreateMarketOrdersSide (
            MamdaOrderBookPriceLevel::Side  side);
            
        /**
         * Detach the given level from the book
         *
         * @param level The level to detach
         */
        void detach (MamdaOrderBookPriceLevel* level);

        /**
         * Add the given entry to the detach list to be cleaned up
         *
         * @param entry The entry to detach
         */
        void detach (MamdaOrderBookEntry* entry);

        /**
         * Free resources associated with any detached price levels or entries, 
         * detached either through explicit calls to detach() or detached as a 
         * result or having no remaining entries.
         */
        void cleanupDetached ();

        /**
         * Get the "book time" (or "event time") of the last update.  The
         * book time is related to market data feeds: the time that the
         * market data feed suggests that the update actually happened.
         *
         * @return The book time of the last update.
         */
        const MamaDateTime& getBookTime() const;

            /**
         * Set the BookTime for this order book.
         *
         * @param bookTime The book time
         */
        void setBookTime(const MamaDateTime&  bookTime) const;
        
        /**
         * Set the MamaSourceDerivative for this order book.
         *
         * @param sourceDeriv The source derivative
         */
        void setSourceDerivative (const MamaSourceDerivative*  sourceDeriv);

        /**
         * Get the MamaSourceDerivative for this order book.
         * 
         * @return The source derivative.
         */
        const MamaSourceDerivative*  getSourceDerivative () const;

        /**
         * Get the MamaSource for this order book.
         * 
         * @return The source.
         */
        const MamaSource*  getSource () const;

        /**
         * Set the mamaQuality for this order book.
         *
         * @param quality The new quality.
         */
        void setQuality (mamaQuality  quality);

        /**
         * Get the mamaQuality for this order book.
         * 
         * @return The quality.
         */
        mamaQuality  getQuality () const;

        /**
         * Set the order book closure handle.
         *
         * @param closure The closure.
         */
        void setClosure (void*  closure);

        /**
         * Get the order book closure handle.
         * 
         * @return The orderbook closure.
         */
        void*  getClosure () const;

        /**
         * Equality operator.  Two books are equal if their symbols, price
         * levels and price level entries are identical.
         *
         * @param rhs The book to compare this book to.
         *
         * @return Whether the two books are equal.
         */
        bool operator== (const MamdaOrderBook&  rhs) const;

        /**
         * Non-equality operator.  Two books are equal if their symbols,
         * price levels and price level entries are identical.
         *
         * @param rhs The book to compare this book to.
         *
         * @return Whether the two books are not equal.
         */
        bool operator!= (const MamdaOrderBook&  rhs) const
        { return ! operator== (rhs); }

        /**
         * Add an entry to the order book and (if "delta" is not NULL)
         * record information about the delta related to this action.
         */
        void addEntry (
            MamdaOrderBookEntry*            entry,
            double                          price,
            MamdaOrderBookPriceLevel::Side  side,
            const MamaDateTime&             eventTime,
            MamdaOrderBookBasicDelta*       delta);

        /**
         * Add an entry to the order book and (if "delta" is not NULL)
         * record information about the delta related to this action.
         */
        void addEntry (
            MamdaOrderBookEntry*            entry,
            MamaPrice&                      price,
            MamdaOrderBookPriceLevel::Side  side,
            const MamaDateTime&             eventTime,
            MamdaOrderBookBasicDelta*       delta);
            
        /**
         * Add an entry to the order book and (if "delta" is not NULL)
         * record information about the delta related to this action.  The
         * new entry is returned.
         */
        MamdaOrderBookEntry* addEntry (
            const char*                     entryId,
            mama_quantity_t                 entrySize,
            double                          price,
            MamdaOrderBookPriceLevel::Side  side,
            const MamaDateTime&             eventTime,
            const MamaSourceDerivative*     sourceDeriv,
            MamdaOrderBookBasicDelta*       delta);

        /**
         * Add an entry to the order book and (if "delta" is not NULL)
         * record information about the delta related to this action.  The
         * new entry is returned.
         */
        MamdaOrderBookEntry* addEntry (
            const char*                     entryId,
            mama_quantity_t                 entrySize,
            MamaPrice&                      price,
            MamdaOrderBookPriceLevel::Side  side,
            const MamaDateTime&             eventTime,
            const MamaSourceDerivative*     sourceDeriv,
            MamdaOrderBookBasicDelta*       delta);
            
        /**
         * Update an entry in the order book and (if "delta" is not NULL)
         * record information about the delta related to this action.  If
         * the entry is not internally "wired" to the order book, a
         * MamdaOrderBookInvalidEntry exception is thrown.
         */
        void updateEntry (
            MamdaOrderBookEntry*            entry,
            mama_quantity_t                 size,
            const MamaDateTime&             eventTime,
            MamdaOrderBookBasicDelta*       delta);

        /**
         * Delete an entry in the order book and (if "delta" is not NULL)
         * record information about the delta related to this action.  If
         * the entry is not internally "wired" to the order book, a
         * MamdaOrderBookInvalidEntry exception is thrown.
         */
        void deleteEntry (
            MamdaOrderBookEntry*            entry,
            const MamaDateTime&             eventTime,
            MamdaOrderBookBasicDelta*       delta);

        /**
         * Add all entries from another book into this book.
         *
         * @param book    The source book to add.
         * @param filter  If not NULL, a filter to apply to each entry.
         * @param delta   An optional delta to collect the added entries.
         */
        void addEntriesFromBook (
            const MamdaOrderBook*           book,
            MamdaOrderBookEntryFilter*      filter,
            MamdaOrderBookBasicDeltaList*   delta);

        /**
         * Add all price levels from another book as entries (one per
         * price level) into this book using "source" as the entryId for
         * each entry.
         *
         * @param book    The source book to add.
         * @param source  The name to use as the entry ID.
         * @param delta   An optional delta to collect the added entries.
         */
        void addPriceLevelsFromBookAsEntries (
            const MamdaOrderBook*           book,
            const char*                     source,
            MamdaOrderBookBasicDeltaList*   delta);

        /**
         * Delete all entries in this book that have "source" as its MamaSource.
         *
         * @param source  The source to match.
         * @param delta   An optional delta to collect the deleted entries.
         */
        void deleteEntriesFromSource (
            const MamaSource*               source,
            MamdaOrderBookBasicDeltaList*   delta);

        /**
         * Re-evaluate the order book.  This would be performed after the
         * status of sources and/or subsources of an "aggregated order
         * book" (i.e. a book built from multiple sources) have changed.
         *
         * @return Whether the book info changed based on the re-evaluation.
         */
        bool reevaluate ();

        /**
         * Set whether this book needs a re-evaluation.
         */
        void setNeedsReevaluation (bool  need);

        /**
         * Get whether this book needs a re-evaluation.
         */
        bool getNeedsReevaluation () const;

        /**
         * Set whether to check the MamaSourceState when
         * adding/deleting/re-evaluating entries in the book.
         */
        void setCheckSourceState (bool  check);

        /**
         * Get whether to check the MamaSourceState when
         * adding/deleting/re-evaluating entries in the book.
         */
        bool getCheckSourceState () const;

        /**
         * Return the order book price level at "price" on "side" of the
         * order book.
         *
         * @param price  The price of the order book price level.
         * @param side   The side of the order book to search.
         * @return The order book price level or NULL if not found.
         *
         */
        MamdaOrderBookPriceLevel* getLevelAtPrice (
            double                          price,
            MamdaOrderBookPriceLevel::Side  side) const;

        /**
         * Return the order book price level at position "pos" in the
         * order book.
         *
         * @param pos   The position of the order book price level.
         * @param side  The side of the order book to search.
         * @return The order book price level or NULL if not found.
         *
         */
        MamdaOrderBookPriceLevel* getLevelAtPosition (
            mama_u32_t                      pos,
            MamdaOrderBookPriceLevel::Side  side) const;

        /**
         * Return the order book entry at position "pos" in the order book.
         *
         * @param pos  The position of the order book entry.
         * @param side The side of the order book to search
         * @return The order book entry or NULL if not found.
         */
        MamdaOrderBookEntry* getEntryAtPosition (
            mama_u32_t                      pos,
            MamdaOrderBookPriceLevel::Side  side) const;

        /**
         * Order book equality verification.  A MamdaOrderBookException is
         * thrown if the books are not equal, along with the reason for
         * the inequality.
         *
         * @param rhs The book to compare this book to.
         *
         * @throw MamdaOrderBookException When an error is encountered during
         * book processing.
         */
        void assertEqual (const MamdaOrderBook&  rhs) const;


        // Iterators

        class MAMDAOPTExpDLL bidIterator
        {
        protected:
            friend class MamdaOrderBook;
            struct bidIteratorImpl;
            bidIteratorImpl& mImpl;
        public:
            bidIterator ();
            bidIterator (const bidIterator& copy);
            bidIterator (const bidIteratorImpl& copy);
            ~bidIterator ();
            bidIterator&        operator=  (const bidIterator& rhs);
            bidIterator&        operator++ ();
            const bidIterator&  operator++ () const;
            bidIterator&        operator-- ();
            const bidIterator&  operator-- () const;
            bool                operator== (const bidIterator& rhs) const;
            bool                operator!= (const bidIterator& rhs) const;
            MamdaOrderBookPriceLevel*        operator*  ();
            const MamdaOrderBookPriceLevel*  operator*  () const;
        };

        class MAMDAOPTExpDLL askIterator
        {
        protected:
            friend class MamdaOrderBook;
            struct askIteratorImpl;
            askIteratorImpl& mImpl;
        public:
            askIterator ();
            askIterator (const askIterator& copy);
            askIterator (const askIteratorImpl& copy);
            ~askIterator ();
            askIterator&        operator=  (const askIterator& rhs);
            askIterator&        operator++ ();
            const askIterator&  operator++ () const;
            askIterator&        operator-- ();
            const askIterator&  operator-- () const;
            bool                operator== (const askIterator& rhs) const;
            bool                operator!= (const askIterator& rhs) const;
            MamdaOrderBookPriceLevel*        operator*  ();
            const MamdaOrderBookPriceLevel*  operator*  () const;
        };

        class MAMDAOPTExpDLL bidEntryIterator
        {
        protected:
            friend class MamdaOrderBook;
            struct bidEntryIteratorImpl;
            bidEntryIteratorImpl& mImpl;
        public:
            bidEntryIterator ();
            bidEntryIterator (const bidEntryIterator& copy);
            bidEntryIterator (const bidEntryIteratorImpl& copy);
            ~bidEntryIterator ();
            bidEntryIterator&       operator=  (const bidEntryIterator& rhs);
            bidEntryIterator&       operator++ ();
            const bidEntryIterator& operator++ () const;
            bool                    operator== (const bidEntryIterator& rhs) const;
            bool                    operator!= (const bidEntryIterator& rhs) const;
            MamdaOrderBookEntry*        operator*  ();
            const MamdaOrderBookEntry*  operator*  () const;
        };

        class MAMDAOPTExpDLL askEntryIterator
        {
        protected:
            friend class MamdaOrderBook;
            struct askEntryIteratorImpl;
            askEntryIteratorImpl& mImpl;
        public:
            askEntryIterator ();
            askEntryIterator (const askEntryIterator& copy);
            askEntryIterator (const askEntryIteratorImpl& copy);
            ~askEntryIterator ();
            askEntryIterator&       operator=  (const askEntryIterator& rhs);
            askEntryIterator&       operator++ ();
            const askEntryIterator& operator++ () const;
            bool                    operator== (const askEntryIterator& rhs) const;
            bool                    operator!= (const askEntryIterator& rhs) const;
            MamdaOrderBookEntry*        operator*  ();
            const MamdaOrderBookEntry*  operator*  () const;
        };

        typedef  const bidIterator       constBidIterator;
        typedef  const askIterator       constAskIterator;
        typedef  const bidEntryIterator  constBidEntryIterator;
        typedef  const askEntryIterator  constAskEntryIterator;

        bidIterator       bidBegin();
        constBidIterator  bidBegin() const;
        bidIterator       bidEnd();
        constBidIterator  bidEnd() const;
        askIterator       askBegin();
        constAskIterator  askBegin() const;
        askIterator       askEnd();
        constAskIterator  askEnd() const;

        bidEntryIterator       bidEntryBegin();
        constBidEntryIterator  bidEntryBegin() const;
        bidEntryIterator       bidEntryEnd();
        constBidEntryIterator  bidEntryEnd() const;
        askEntryIterator       askEntryBegin();
        constAskEntryIterator  askEntryBegin() const;
        askEntryIterator       askEntryEnd();
        constAskEntryIterator  askEntryEnd() const;

        /**
         * Set whether the order book is in a consistent or an an
         * inconsistent state. This method is typically called from within the
         * <code>MamdaOrderBookListener</code> in response to sequence number gap
         * detection and subsequent recovery from a gap event.
         *
         * @param isConsistent Whether the book is in a consistent state.
         */
        void setIsConsistent (bool  isConsistent);

        /**
         * Get whether the order book is in a consistent or an an
         * inconsistent state. A book is marked as being inconsistent by the
         * <code>MamdaOrderBookListener</code> whenever a sequence number gap in the
         * book updates is detected. The order book will be marked as consistent
         * once again once a recap for the book is received by the
         * <code>MamdaOrderBookListener</code>.
         *
         * @return Whether the book is in a consistent state.
         */
        bool getIsConsistent () const;

        /**
         * Dump the order book to the output stream.
         *
         * @param output The <code>ostream</code> to write the orderbook to.
         */
        void dump (ostream&  output) const;

        /**
         * Enforce strict checking of order book modifications (at the
         * expense of some performance).  This setting is passed on to the
         * MamdaOrderBookPriceLevel and MamdaOrderBookEntry classes.
         */
        static void setStrictChecking (bool strict);
        
        /**
         * Enable the generation of book deltas for this book. When delta generation is enabled
         * changes to the book are saved and can be popultaed to MamaMsgs. 
         * @param publish Whether book delta generation is enabled.
         */    
        void generateDeltaMsgs (bool generate);
        
        /**
         * Get whether book delta generation is enabled
         * @return Whether book delta generation is enabled.
         */
        bool getGenerateDeltaMsgs();
        
        /**
         * Populate a MamaMsg of the changes to this order book. 
         * This will include the changes from the last time this function was called
         * or all changes from the initial state. 
         *  @param msg A MamaMsg ref containing all changes to the current book.
         */   
        bool populateDelta (MamaMsg& msg);

        /**
         * Popuklate a MamaMsg with the current state of this order book.
         * @param msg A MamaMsg containing all book, price and entry 
         * (if applicable) details of the current book. 
         */  
        void populateRecap (MamaMsg& msg);

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
        void addDelta (MamdaOrderBookEntry*              entry,
                       MamdaOrderBookPriceLevel*         level,
                       mama_quantity_t                   plDeltaSize,
                       MamdaOrderBookPriceLevel::Action  plAction,
                       MamdaOrderBookEntry::Action       entAction);
                       
        /**
         * clear the delta list using for storing generated deltas
         */   
        void clearDeltaList();
        
        /**
         * Set the orderbook contributors
         * @param bookContributors The orderbook contributors
         */
        void setBookContributors (const char* bookContributors);

        /**
         * Get the orderbook contributors
         * @return The orderbook contributors
         **/
        const char* getBookContributors () const;

        /**
         * Is the orderbook contributors set
         * @return Whether this orderbook has a list of contributors
         **/
        bool hasBookContributors () const;

        /**
         * Get whether the books contributors have been modified.
         * @return Whether the book contributors have been modified.
         */
        bool getBookContributorsModified () const;

        /**
         * Set whether the books contributors have been modified.
         * @param modifies Whether the book contributors have been modified.
         */
        void setBookContributorsModified (bool modified);

    private:
        struct MamdaOrderBookImpl;
        MamdaOrderBookImpl& mImpl;
    };

} // namespace

#endif // MamdaOrderBookH
