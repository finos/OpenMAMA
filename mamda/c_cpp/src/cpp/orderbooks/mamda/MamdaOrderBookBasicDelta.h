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

#ifndef MamdaOrderBookBasicDeltaH
#define MamdaOrderBookBasicDeltaH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <iosfwd>

using std::ostream;

namespace Wombat
{

    class MamdaOrderBookPriceLevel;
    class MamdaOrderBookEntry;

    /**
     * MamdaOrderBookBasicDelta is a class that saves information about a
     * basic order book delta.  A basic delta is one that affects a
     * single order book entry.
     */

    class MAMDAOPTExpDLL MamdaOrderBookBasicDelta
    {
    public:
        MamdaOrderBookBasicDelta () 
            { clear(); }
        MamdaOrderBookBasicDelta (const MamdaOrderBookBasicDelta&);
        virtual ~MamdaOrderBookBasicDelta () {}

        /**
         * Clear the delta.
         */
        virtual void clear ();

        /**
         * Set the delta info.
         */
        virtual void set (MamdaOrderBookEntry*              entry,
                          MamdaOrderBookPriceLevel*         level,
                          mama_quantity_t                   plDeltaSize,
                          MamdaOrderBookPriceLevel::Action  plAction,
                          MamdaOrderBookEntry::Action       entryAction);

        /**
         * Set the MamdaOrderBookPriceLevel object to which this entry
         * belongs.  This method is invoked internally, by the MAMDA API,
         * when an entry is added to a price level.
         *
         * @param level  The price level to be associated with.
         */
        void setPriceLevel (MamdaOrderBookPriceLevel*  level)
            { mPriceLevel = level; } 

        /**
         * Set the delta action with respect to the price level.
         *
         * @param action The price level action
         */
        void setPlDeltaAction (MamdaOrderBookPriceLevel::Action  action)
            { mPlAction = action; }

        /**
         * Set the price level delta size
         *
         * @param action The price level action
         */
        void setPlDeltaSize (mama_quantity_t  size)
            { mPlDeltaSize = size; }


        /**
         * Apply the price level delta size to the existing size, giving the net
         * effect.
         *
         * @param action The price level action
         */
        void applyPlDeltaSize (mama_quantity_t  size)
            { mPlDeltaSize += size; }
                
        /**
         * Get the MamdaOrderBookPriceLevel object related to this basic delta.
         *
         * @return  The price level.
         */
        MamdaOrderBookPriceLevel* getPriceLevel() const 
            { return mPriceLevel; }

        /**
         * Get the MamdaOrderBookEntry object related to this basic delta.
         *
         * @return  The entry.
         */
        MamdaOrderBookEntry* getEntry() const 
            { return mEntry; }

        /**
         * Get the difference in size for the price level.
         *
         * @return The price level size delta.
         */
        mama_quantity_t  getPlDeltaSize () const 
            { return mPlDeltaSize; }

        /**
         * Get the delta action with respect to the price level.
         *
         * @return The price level action.
         */
        MamdaOrderBookPriceLevel::Action  getPlDeltaAction () const
            { return mPlAction; }

        /**
         * Get the delta action with respect to the entry.
         *
         * @return The entry action.
         */
        MamdaOrderBookEntry::Action  getEntryDeltaAction () const
            { return mEntryAction; }

        /**
         * Get the delta action with respect to the entry.
         *
         */
        void  setEntryDeltaAction (MamdaOrderBookEntry::Action action)
            { mEntryAction = action; }
            
        /**
         * Get the MamdaOrderBook object to which this delta belongs.
         *
         * @return  The order book related to this delta.
         */
        virtual const MamdaOrderBook* getOrderBook() const;

        /**
         * Dump the simple update to the output stream.
         *
         * @param output The <code>ostream</code> to write the update to.
         */
        void dump(ostream&  output) const;

    protected:
        MamdaOrderBookPriceLevel*         mPriceLevel;
        MamdaOrderBookEntry*              mEntry;
        mama_quantity_t                   mPlDeltaSize;
        MamdaOrderBookPriceLevel::Action  mPlAction;
        MamdaOrderBookEntry::Action       mEntryAction;

    private:
        // No copy constructor nor assignment operator.
        MamdaOrderBookBasicDelta& operator= (const MamdaOrderBookBasicDelta&);
    };

} // namespace

#endif // MamdaOrderBookBasicDeltaH
