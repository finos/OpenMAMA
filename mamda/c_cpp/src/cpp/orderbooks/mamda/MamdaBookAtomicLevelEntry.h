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

#ifndef MamdaBookAtomicLevelEntryH
#define MamdaBookAtomicLevelEntryH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaBasicRecap.h>

namespace Wombat
{

    /**
     * MamdaBookAtomicLevel is an interface that provides access to 
     * Price Level and Price Level Entry fields
     */
    class MAMDAOPTExpDLL MamdaBookAtomicLevelEntry : public MamdaBasicRecap
    {
    public:

        /**
         * @see MamdaBookAtomicLevel#getPriceLevelPrice()
         */
        virtual double               getPriceLevelPrice()           const = 0;
        
         /**
          * @see MamdaBookAtomicLevel#getPriceLevelMamaPrice()
          */
         virtual MamaPrice&          getPriceLevelMamaPrice()       const = 0;

        /**    
         * @see MamdaBookAtomicLevel#getPriceLevelSize()
         */
        virtual mama_f64_t           getPriceLevelSize()            const = 0;
        
        /**
         * @see MamdaBookAtomicLevel#getPriceLevelAction()
         */
        virtual char                 getPriceLevelAction()          const = 0;

        /**
         * @see MamdaBookAtomicLevel#getPriceLevelSide()
         */
        virtual char                 getPriceLevelSide()            const = 0;

        /**
         * @see MamdaBookAtomicLevel#getPriceLevelTime()
         */
        virtual const MamaDateTime&  getPriceLevelTime()            const = 0;

        /**
         * @see MamdaBookAtomicLevel#getPriceLevelNumEntries()
         */
        virtual mama_f32_t           getPriceLevelNumEntries()      const = 0;

        /**
         * 
         */
        virtual mama_u32_t           getPriceLevelActNumEntries()   const = 0;

        /**
         * Order book entry action to apply to the full order book.
         *
         * <ul>
         * <li>A : Add entry to the price level.</li>
         * <li>U : Update existing entry in the price level.</li>
         * <li>D : Delete existing entry from  the price level.</li>
         * </ul>
         *
         * @return The orderbook entry action.
         */
        virtual char                 getPriceLevelEntryAction()     const = 0;

        /**
         * Order book entry reason.
         *
         * @return The orderbook entry reason.
         */ 
        virtual char                 getPriceLevelEntryReason()     const = 0;

        /**
         * Order book entry Id. (order ID, participant ID, etc.)
         *
         * @return The entry id
         */
        virtual const char*          getPriceLevelEntryId()         const = 0;

        /**
         * Return the order book entry size
         * 
         * @return Order book entry size
         */
        virtual mama_u64_t           getPriceLevelEntrySize()       const = 0;

        /**
         * Return the time of order book entry update.
         *
         * @return Time of order book entry update.
         */
        virtual const MamaDateTime&  getPriceLevelEntryTime()       const = 0;
        virtual ~MamdaBookAtomicLevelEntry() {};
    };


} // namespace

#endif // MamdaBookAtomicLevelEntryH
