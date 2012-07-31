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

#ifndef MamdaOrderBookBasicDeltaListH
#define MamdaOrderBookBasicDeltaListH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOrderBookBasicDelta.h>
#include <iosfwd>

using std::ostream;

namespace Wombat
{

    /**
     * MamdaOrderBookBasicDeltaList is a class that saves information
     * about an order book delta that involves multiple entries and/or
     * price levels.  For example, a modified order may involve a price
     * change that means moving an entry from one price level to another.
     * A delta list is made up of several basic deltas, which can be
     * iterated over by methods provided in the class.
     */

    class MAMDAOPTExpDLL MamdaOrderBookBasicDeltaList
    {
    public:
        MamdaOrderBookBasicDeltaList ();
        ~MamdaOrderBookBasicDeltaList ();

        /**
         * Clear the delta.
         */
        void clear ();

        /**
         * Set whether to actually keep the basic deltas.  Many
         * applications don't need the basic deltas and will iterate over
         * part or all of the full book (with the deltas already applied).
         * If this is set to true and an attempt is made to iterate over
         * the basic deltas (by calling begin() or end()) then a
         * MamdaOrderBookException will be thrown.
         */
        void setKeepBasicDeltas (bool  keep);

        enum ModifiedSides
        {
            MOD_SIDES_NONE        = 0,
            MOD_SIDES_BID         = 1,
            MOD_SIDES_ASK         = 2,
            MOD_SIDES_BID_AND_ASK = 3
        };

        /**
         * Get which side(s) of the book have been modified by this
         * complex update.  This information may prevent the need for
         * receivers of complex updates to iterate over one or other side
         * of the book.
         *
         * @return  The modified side(s).
         */
        ModifiedSides getModifiedSides() const;

        /**
         * Add a basic delta.  This method adds a MamdaOrderBookBasicDelta
         * to the list.
         */
        void add (MamdaOrderBookEntry*              entry,
                  MamdaOrderBookPriceLevel*         level,
                  mama_quantity_t                   plDeltaSize,
                  MamdaOrderBookPriceLevel::Action  plAction,
                  MamdaOrderBookEntry::Action       entryAction);

        /**
         * Add a basic delta.  This method adds a copy of the
         * MamdaOrderBookBasicDelta to the list.
         */
        void add (const MamdaOrderBookBasicDelta&  delta);

        /**
         * Set the MamdaOrderBook object to which this delta belongs.
         *
         * @param book  The order book related to this delta.
         */
        void setOrderBook (MamdaOrderBook* book);

        /**
         * Get the MamdaOrderBook object to which this delta belongs.
         *
         * @return  The order book related to this delta.
         */
        MamdaOrderBook* getOrderBook() const;

        /**
         * Return the number of simple deltas in this complex delta.
         */
        mama_size_t getSize() const;

        /**
         * Fix up price level actions (temporary workaround for problem).
         * This method ensures that all basic deltas for the same price
         * level end up with the same price level action.
         */
        void fixPriceLevelActions();

        /**
         * Whether to conflate the order book deltas
        * @param conflate Whether to conflate order book deltas.
         */
        void  setConflateDeltas  (bool conflate);

        /**
         * Get sendImmediately
         */
         bool getSendImmediately ();

        
        /**
         * Set whether we are interested in "entry level" information at
         * all.
         * @param process Whether to process entries in books.
         */
        void  setProcessEntries  (bool processEntries);
        
        /**
         * Dump the complex update to the output stream.
         *
         * @param output The <code>ostream</code> to write the update to.
         */
        void dump (ostream& output) const;

        /**
         * The MamdaOrderBookBasicDeltaList's iterator provides access to
         * the list of MamdaOrderBookBasicDelta objects that comprise it.
         */
        class MAMDAOPTExpDLL iterator
        {
        protected:
            friend class MamdaOrderBookBasicDeltaList;
            struct iteratorImpl;
            iteratorImpl& mImpl;
        public:
            iterator ();
            iterator (const iterator& copy);
            iterator (const iteratorImpl& copy);
            iterator (iteratorImpl& copy);

            ~iterator ();
            iterator&                       operator=  (const iterator& rhs);
            iterator&                       operator++ ();
            const iterator&                 operator++ () const;
            bool                            operator== (const iterator& rhs) const;
            bool                            operator!= (const iterator& rhs) const;
            MamdaOrderBookBasicDelta*       operator*  ();
            const MamdaOrderBookBasicDelta* operator*  () const;
        };

        typedef  const iterator  const_iterator;

        iterator        begin();
        const_iterator  begin() const;
        iterator        end();
        const_iterator  end()   const;

    private:
        struct MamdaOrderBookBasicDeltaListImpl;
        MamdaOrderBookBasicDeltaListImpl& mImpl;

        // No copy constructor nor assignment operator.
        MamdaOrderBookBasicDeltaList (const MamdaOrderBookBasicDeltaList&);
        MamdaOrderBookBasicDeltaList& operator= (const MamdaOrderBookBasicDeltaList&);
    };

} // namespace

#endif // MamdaOrderBookBasicDeltaListH
