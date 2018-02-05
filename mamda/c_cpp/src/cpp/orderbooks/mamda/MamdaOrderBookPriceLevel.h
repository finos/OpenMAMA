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

#ifndef MamdaOrderBookPriceLevelH
#define MamdaOrderBookPriceLevelH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOrderBookTypes.h>
#include <mamda/MamdaOrderBookEntryFilter.h>
#include <mama/mamacpp.h>
#include <stdlib.h>
#include <string.h>

namespace Wombat
{

    class MamaSource;
    class MamdaOrderBook;
    class MamdaOrderBookEntry;
    class MamdaOrderBookBasicDeltaList;

    /**
     * MamdaOrderBookPriceLevel is a class that provides a price level
     * type for order books.
     */
    class MAMDAOPTExpDLL MamdaOrderBookPriceLevel
    {
    public:
        /**
         * An enumeration for price level actions.  Price level actions
         * differ from entry actions because, for example, a price level
         * message with ACTION_UPDATE may consist of entries with
         * ACTION_ADD, ACTION_UPDATE or ACTION_DELETE.
         */
        enum Action
        {
            MAMDA_BOOK_ACTION_ADD     = MamdaOrderBookTypes::MAMDA_BOOK_ACTION_ADD,
            MAMDA_BOOK_ACTION_UPDATE  = MamdaOrderBookTypes::MAMDA_BOOK_ACTION_UPDATE,
            MAMDA_BOOK_ACTION_DELETE  = MamdaOrderBookTypes::MAMDA_BOOK_ACTION_DELETE,
            MAMDA_BOOK_ACTION_UNKNOWN = MamdaOrderBookTypes::MAMDA_BOOK_ACTION_UNKNOWN
        };

        /**
         * An enumeration for the side order book side.  "Bid" (or "buy")
         * orders occur on one side and "ask" (or "sell") orders occur on
         * the other.
         */
        enum Side
        {
            MAMDA_BOOK_SIDE_BID      = MamdaOrderBookTypes::MAMDA_BOOK_SIDE_BID,
            MAMDA_BOOK_SIDE_ASK      = MamdaOrderBookTypes::MAMDA_BOOK_SIDE_ASK,
            MAMDA_BOOK_SIDE_UNKNOWN  = MamdaOrderBookTypes::MAMDA_BOOK_SIDE_UNKNOWN
        };

        /** 
         * An enumeration for a reason for a change.  Some of the values
         * of Reason can mean the same thing, as far as their affect on
         * the order book. If possible, a feed will send MODIFY, CANCEL or
         * TRADE actions so that downstream applications that are
         * interested in such data can handle it; other applications can
         * treat such actions in the same way as an UPDATE action (or as a
         * DELETE action if the size is zero).
         */
        enum Reason
        {
            MAMDA_BOOK_REASON_MODIFY  = MamdaOrderBookTypes::MAMDA_BOOK_REASON_MODIFY,
            MAMDA_BOOK_REASON_CANCEL  = MamdaOrderBookTypes::MAMDA_BOOK_REASON_CANCEL,
            MAMDA_BOOK_REASON_TRADE   = MamdaOrderBookTypes::MAMDA_BOOK_REASON_TRADE,
            MAMDA_BOOK_REASON_CLOSE   = MamdaOrderBookTypes::MAMDA_BOOK_REASON_CLOSE,
            MAMDA_BOOK_REASON_DROP    = MamdaOrderBookTypes::MAMDA_BOOK_REASON_DROP,
            MAMDA_BOOK_REASON_MISC    = MamdaOrderBookTypes::MAMDA_BOOK_REASON_MISC,
            MAMDA_BOOK_REASON_UNKNOWN = MamdaOrderBookTypes::MAMDA_BOOK_REASON_UNKNOWN
        };

        /**
         * An enumeration for the type of level. "Limit" level orders are set at a
         * specific price. "Market" level orders are set at the current market price.
         */
        enum OrderType
        {
            MAMDA_BOOK_LEVEL_LIMIT    = MamdaOrderBookTypes::MAMDA_BOOK_LEVEL_LIMIT,
            MAMDA_BOOK_LEVEL_MARKET   = MamdaOrderBookTypes::MAMDA_BOOK_LEVEL_MARKET,
            MAMDA_BOOK_LEVEL_UNKNOWN  = MamdaOrderBookTypes::MAMDA_BOOK_LEVEL_UNKNOWN
        };
        
        MamdaOrderBookPriceLevel ();

        /**
         * Copy constructor.  Note that the associated order book of the
         * original copy is not copied.
         */
        MamdaOrderBookPriceLevel (const MamdaOrderBookPriceLevel&  copy);

        /**
         * Constructor initializing just the price and side.
         */
        MamdaOrderBookPriceLevel (double  price,
                                  Side    side);

        MamdaOrderBookPriceLevel (MamaPrice&  price,
                                  Side        side);
                                  
        ~MamdaOrderBookPriceLevel ();

        /**
         * Assignment operator.  Note that the associated order book of
         * the original copy is not copied.
         */
        MamdaOrderBookPriceLevel& operator= (const MamdaOrderBookPriceLevel&  rhs);

        /**
         * Copy a price level.  Note that the associated order book of
         * the original copy is not copied.
         */
        void copy (const MamdaOrderBookPriceLevel&  rhs);

        /**
         * Copy a price level details only. No entries are copied.
         *
         */
        void copyLevelOnly (const MamdaOrderBookPriceLevel&  rhs);

        
        /**
         * Add an entry to the level (without any sanity checking).
         */
        void addEntry (MamdaOrderBookEntry*  entry);

        /**
         * Update an entry by copying the information from another entry.
         */
        void updateEntry (const MamdaOrderBookEntry&  entry);

        /**
         * Remove an entry based on the entry ID information from another entry.
         */
        void removeEntryById (const MamdaOrderBookEntry&  entry);

        /**
         * Remove this precise entry (i.e., based on this object being the
         * exact object in the level).
         */
        void removeEntry (const MamdaOrderBookEntry*  entry);

        /**
         * Add all entries from another price level into this level.
         *
         * @param level   The price level to add.
         * @param filter  Filter to use when adding the entries
         * @param delta   An optional delta to collect for the deleted entries.
         */
        void addEntriesFromLevel (
            const MamdaOrderBookPriceLevel*  level,
            MamdaOrderBookEntryFilter*       filter,
            MamdaOrderBookBasicDeltaList*    delta);

        /**
         * Delete all entries in this price level that have "source" as
         * its MamaSource.
         *
         * @param source  The source to match.
         * @param delta   An optional delta to collect for the deleted entries.
         */
        void deleteEntriesFromSource (
            const MamaSource*               source,
            MamdaOrderBookBasicDeltaList*   delta);

        /**
         * Re-evaluate the price level.  This would be performed after the
         * status of sources and/or subsources of an "aggregated order
         * book" (i.e., a book built from multiple sources) have changed.
         *
         * @return Whether the book info changed based on the re-evaluation.
         */
        bool reevaluate ();

        // The following methods are intended to be used internally (e.g., by
        // MamdaOrderBookListener).

        void clear          ();
        void setPrice       (double               price);
        void setPrice       (MamaPrice&           price);
        void setSize        (mama_quantity_t      size);
        void setSizeChange  (mama_quantity_t      sizeChange);
        void setNumEntries  (mama_u32_t           numEntries);
        void setSide        (Side                 side);
        void setAction      (Action               action);
        void setTime        (const MamaDateTime&  time);
        void setDetails     (const MamdaOrderBookPriceLevel&  rhs);

        /**
         * Mark everything in this price level as deleted, including
         * entries.
         */
        void markAllDeleted ();

        void setAsDifference (const MamdaOrderBookPriceLevel&  lhs,
                              const MamdaOrderBookPriceLevel&  rhs);

        /**
         * Return the price for this level.
         *
         * @return The price for this level.
         */
        double               getPrice      () const;
        MamaPrice            getMamaPrice  () const;

        /**
         * Return the total size (across all entries) for this level.
         *
         * @return The total size for this level.
         */
        mama_quantity_t      getSize       () const;

        /**
         * Return the size change for this (presumably delta) level.  This
         * attribute is only of interest for delta order books.  For full
         * order books, this field will be equal to the size of the price
         * level.
         *
         * @return The changed size for this level.
         */
        mama_quantity_t      getSizeChange () const;

        /**
         * Return the actual number of entries for this level.  The actual
         * number of entries may not equate to the number of entries that
         * can be iterated over if:
         * (a) the feed does not provide the actual entries, or
         * (b) the price level is just a delta.
         *
         * @return The actual number of entries for this level.
         */
        mama_u32_t           getNumEntries () const;

         /**
         * Return the number of entries that can be iterated over
         * @return The number of entries that can be iterated over
         */
        mama_u32_t           getNumEntriesTotal () const;
        
        /**
         * Return whether there are no entries for this level.
         *
         * @return Whether there are no entries for this level.
         */
        bool                 empty () const;

        /**
         * Return the side (bid/ask) of the book for this level.
         *
         * @return The side of the book for this level.
         */
        Side                 getSide       () const;

        /**
         * Return the action for this price level.  All price levels for a
         * full book are marked with ACTION_ADD.
         *
         * @return The action for this level.
         */
        Action               getAction     () const;

        /**
         * Return the time stamp for when the price level was last updated.
         *
         * @return The time stamp for when the price level was last updated.
         */
        const MamaDateTime&  getTime       () const;

        /**
         * Equality operator.  Two price levels are equal if their members
         * and price level entries are identical.
         *
         * @param rhs The order book price level to compare this level to.
         *
         * @return Whether the price levels are equal.
         */
        bool operator==  (const MamdaOrderBookPriceLevel& rhs) const;

        /**
         * Non-equality operator.  Two price levels are equal if their
         * members and price level entries are identical.
         *
         * @param rhs The order book price level to compare this level to.
         *
         * @return Whether the price levels are unequal.
         */
        bool operator!= (const MamdaOrderBookPriceLevel&  rhs) const
        { return ! operator== (rhs); }

        /**
         * Set the MamdaOrderBook object to which this price level
         * belongs.  This method is invoked internally, by the MAMDA API,
         * when a price level is added to a book.
         *
         * @param book  The order book to be associated with
         */
        void setOrderBook (MamdaOrderBook*  book);

        /**
         * Get the MamdaOrderBook object to which this entry belongs.
         *
         * @return  The order book currently associated with this price level.
         */
        MamdaOrderBook* getOrderBook() const;

        /**
         * Get the symbol for this entry, if possible.  This can only be
         * done if the price level is part of an order book.  NULL is
         * returned if no symbol can be found.
         *
         * @return  The symbol or NULL.
         */
        const char* getSymbol() const;

        /**
         * Return the order book entry with ID "id" in the price level or
         * NULL if not found.
         *
         * @param id  The ID of the order book entry.
         * @return The order book entry found or NULL if not found.
         */
        MamdaOrderBookEntry* findEntry (
            const char*  id) const;

        /**
         * Return the order book entry with ID "id" in the price level,
         * creating one if necessary.
         *
         * @param id  The ID of the order book entry.
         * @return The order book entry found or created.
         */
        MamdaOrderBookEntry* findOrCreateEntry (
            const char*  id);

        /**
         * Return the order book entry with ID "id" in the price level,
         * creating one if necessary.
         *
         * @param id  The ID of the order book entry.
         * @param newEntry  Boolean reference indicating entry creation.
         * @return The order book entry found or created.
         */
        MamdaOrderBookEntry* findOrCreateEntry (
            const char*  id,
            bool&        newEntry);

        /**
         * Return the order book entry at position "pos" in the price level.
         *
         * @param pos  The position of the order book entry.
         * @return The order book entry or NULL if not found.
         */
        MamdaOrderBookEntry* getEntryAtPosition (
            mama_u32_t  pos) const;

        /**
         * Set the order book price level closure handle.
         *
         * @param closure The closure.
         */
        void setClosure (void*  closure);

        /**
         * Get the order book price level closure handle.
         * 
         * @return The closure.
         */
        void*  getClosure () const;

        /**
         * Return the order type of the level.
         * @return The order type of the level.
         */
        OrderType getOrderType () const;
        
        /**
         * Set the order type for this level.
         * @param orderType The order type of the level.
         */
        void setOrderType (OrderType orderType);
       
        /**
         * Order book price level equality verification.  A
         * MamdaOrderBookException is thrown if the price levels are not
         * equal, along with the reason for the inequality.
         *
         * @param rhs The order book price level to compare this level to.
         *
         * @throws <MamdaOrderBookException> If error encountered during book
         * processing.
         */
        void assertEqual (const MamdaOrderBookPriceLevel&  rhs) const;

        class MAMDAOPTExpDLL iterator
        {
        protected:
            friend class MamdaOrderBookPriceLevel;
            struct iteratorImpl;
            iteratorImpl& mImpl;
        public:
            iterator ();
            iterator (const iterator& copy);
            iterator (const iteratorImpl& copy);

            ~iterator ();
              
            iterator&                  operator=  (const iterator& rhs);
            iterator&                  operator++ ();
            const iterator&            operator++ () const;
            bool                       operator== (const iterator& rhs) const;
            bool                       operator!= (const iterator& rhs) const;
            MamdaOrderBookEntry*       operator*  ();
            const MamdaOrderBookEntry* operator*  () const;
        };

        typedef  const iterator  const_iterator;

        iterator        begin ();
        const_iterator  begin () const;
        iterator&       begin (iterator& reuse);
        const_iterator& begin (const_iterator& reuse) const;

        iterator        end   ();
        const_iterator  end   ()   const;
        iterator&       end   (iterator& reuse);
        const_iterator& end   (const_iterator& reuse)   const;

        iterator        findEntryAfter (iterator&        start,
                                        const char*      id);
        const_iterator  findEntryAfter (const_iterator&  start,
                                        const char*      id) const;

        /**
         * Enforce strict checking of order book modifications (at the
         * expense of some performance).  This setting is automatically
         * updated by MamdaOrderBook::setStrictChecking().
         *
         * @param strict Whether strict checking of modification is enabled.
         */
        static void setStrictChecking (bool strict);

#ifndef __sun
        static bool sortEntriesByTime;
#endif

    private:
        struct MamdaOrderBookPriceLevelImpl;
        MamdaOrderBookPriceLevelImpl& mImpl;
    };

} // namespace

#endif // MamdaOrderBookPriceLevelH
