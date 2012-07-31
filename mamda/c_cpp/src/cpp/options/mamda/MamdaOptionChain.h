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

#ifndef MamdaOptionChainH
#define MamdaOptionChainH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOptionAtTheMoneyCompareType.h>
#include <set>

using std::set;

namespace Wombat
{

    class MamdaOptionContract;
    class MamdaOptionExpirationDateSet;
    class MamdaQuoteListener;
    class MamdaTradeListener;

    typedef set<double>   StrikeSet;

    /**
     * MamdaOptionChain is a specialized class to represent market data
     * option chains.  The class has capabilities to store the current
     * state of an entire option chain, or a subset of the chain.
     */

    class MAMDAOPTExpDLL MamdaOptionChain
    {
        /** No copying. */
        MamdaOptionChain (const MamdaOptionChain&);

        /** No assignment. */
        MamdaOptionChain& operator= (const MamdaOptionChain&);

    public:

        /**
         * MamdaOptionChain Constructor. 
         */
        MamdaOptionChain (const char*  symbol);

        /**
         * MamdaOptionChain Destructor. 
         */
        ~MamdaOptionChain ();

        /**
         * Set the underlying symbol for the option chain.
         */
        void setSymbol (const char*  symbol);

        /**
         * Get the underlying symbol for the option chain.
         */
        const char*  getSymbol () const;

        /**
         * Set the underlying quote information.  The MamdaQuoteListener
         * object would likely be an object that gets automatically
         * updated by events handled elsewhere.
         */
        void setUnderlyingQuoteListener (
            const MamdaQuoteListener*  quoteListener);

        /**
         * Set the underlying trade information.  The MamdaTradeListener
         * object would likely be an object that gets automatically
         * updated by events handled elsewhere.
         */
        void setUnderlyingTradeListener (
            const MamdaTradeListener*  tradeListener);

        /**
         * Get the underlying quote information.  Returns the object
         * provided by setUnderlyingQuoteListener(), if any.
         */
        const MamdaQuoteListener*  getUnderlyingQuoteListener () const;

        /**
         * Get the underlying trade information.  Returns the object
         * provided by setUnderlyingTradeListener(), if any.
         */
        const MamdaTradeListener*  getUnderlyingTradeListener () const;

        /**
         * Add an option contract.  This method would not normally be
         * invoked by a user application.  Rather,
         * MamdaOptionChainListener would be most likely to call this
         * method.
         */
        void addContract (
            const char*           contractSymbol,
            MamdaOptionContract*  contract);

        /**
         * Add the contract to value-added-structures such as Put and Call
         * side mappings and expiration by strike set, etc. 
         * Called by AddContract(const char*, MamdaOptionContract*).
         * Can be called directly on a contract when additional 
         * mandatory fields are acquired for the contract that 
         * may enable it to be added to value-added-structures 
         * it wasn't added to (due to the lack of information)
         * when first added to the chain.
         */
        void processNewContractDetails (
	    const char*           contractSymbol,
	    MamdaOptionContract*  contract);   

        /**
         * Remove an option contract.  This method would not normally be
         * invoked by a user application.  Rather,
         * MamdaOptionChainListener would be most likely to call this
         * method.
         */
        void removeContract (
            const char*  contractSymbol);

        /**
         * Determine the underlying price ("at the money"), based on the
         * mode of calculation.
         */
        double getAtTheMoney (
            MamdaOptionAtTheMoneyCompareType  compareType);

        /**
         * Determine the set of strike prices that are included in a given
         * percentage range of the underlying price.  If there are no
         * strikes within the percentage range, then the set will be
         * empty.
         */
        void getStrikesWithinPercent (
            StrikeSet&                        strikeSet,
            double                            percentage,
            MamdaOptionAtTheMoneyCompareType  compareType);

        /**
         * Determine the set of strike prices that are included in a given
         * fixed size range of strikes surrounding the underlying price.
         * If rangeLen is odd, then the strike price nearest to the
         * underlying price is treated as a higher strike price.  If
         * rangeLen is even and the underlying price is exactly equal to a
         * strike price, then that strike price is treated as a higher
         * strike price.
         */
        void getStrikesWithinRangeSize (
            StrikeSet&                        strikeSet,
            int                               rangeLength,
            MamdaOptionAtTheMoneyCompareType  compareType);

        /**
         * Determine whether some price (e.g. a strike price) is within a
         * given percentage range of the underlying (at the money) price.
         */
        bool getIsPriceWithinPercentOfMoney (
            double                            price,
            double                            percentage,
            MamdaOptionAtTheMoneyCompareType  compareType);

        class MAMDAOPTExpDLL iterator
        {
        public:
            ~iterator ();

            iterator (const iterator&);
            iterator&             operator= (const iterator&);
            bool                  hasNext();
            MamdaOptionContract*  next();

        protected:
            friend class MamdaOptionChain;
            struct iteratorImpl;
            iterator (void*);
            iteratorImpl* mIterImpl;
        };

        class MAMDAOPTExpDLL const_iterator
        {
        public:
            ~const_iterator ();
            const_iterator  (const const_iterator&);
            const_iterator&             operator= (const const_iterator&);
            bool                        hasNext();
            const MamdaOptionContract*  next();

        protected:
            friend class MamdaOptionChain;
            struct constIteratorImpl;
            const_iterator (void*);
            constIteratorImpl* mIterImpl;
        };

        /**
         * Obtain an iterator to the beginning of the list of call
         * options.  Use the iterator::hasNext() to test for a subsequent
         * iterator and use iterator::next() method to move to it.
         */
        iterator       callIterator();

        /**
         * Obtain an iterator to the beginning of the list of put
         * options.  Use the iterator::hasNext() to test for a subsequent
         * iterator and use iterator::next() method to move to it.
         */
        iterator       putIterator();

        /**
         * Obtain an iterator to the beginning of the list of call
         * options.  Use the iterator::hasNext() to test for a subsequent
         * iterator and use iterator::next() method to move to it.
         */
        const_iterator callIterator() const;

        /**
         * Obtain an iterator to the beginning of the list of put
         * options.  Use the iterator::hasNext() to test for a subsequent
         * iterator and use iterator::next() method to move to it.
         */
        const_iterator putIterator() const;

        /**
         * Return the set of all expiration dates (which can be iterated
         * over one date at a time).
         */
        const MamdaOptionExpirationDateSet&  getAllExpirations () const;

        /**
         * Dump the option chain to standard output.
         */
        void  dump ();

    private:
        struct MamdaOptionChainImpl;
        MamdaOptionChainImpl& mImpl;
    };

} // namespace

#endif // MamdaOptionChainH
