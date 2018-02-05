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

#ifndef MamdaBookAtomicLevelH
#define MamdaBookAtomicLevelH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaBasicRecap.h>
#include <mamda/MamdaOrderBookTypes.h>

namespace Wombat
{

    /**
     * MamdaBookAtomicLevel is an interface that provides access to trade
     * related fields.
     */
    class MAMDAOPTExpDLL MamdaBookAtomicLevel : public MamdaBasicRecap
    {
    public:

        /**
         * Return the number of price levels in the order book update.
         *
         * @return The number of price levels.
         */
        virtual mama_u32_t           getPriceLevelNumLevels()    const = 0;

        /**
         * Return at which position this level is within an update containing a
         * number of levels. (i.e. level m of n levels in the update)
         *
         * @return The position of this level in the update received.
         */
        virtual mama_u32_t           getPriceLevelNum()          const = 0;
  
        /**
         * Return the price for this price level.
         *
         * @return The price level price.
         */
        virtual double               getPriceLevelPrice()        const = 0;

        /**
         * Return the MamaPrice for this price level.
         *
         * @return The price level price.
         */
        virtual  MamaPrice&          getPriceLevelMamaPrice()    const = 0;
      
      
        /**
         * Return the number of order entries comprising this price level.
         *
         * @warning Not supported for V5 entry book updates.
         *
         * @return The number of entries in this price level.
         */
        virtual mama_f64_t           getPriceLevelSize()         const = 0;

        /**
         * Aggregate size at current price level
         *
         * @warning Not supported for V5 entry book updates.
         *
         * @return The aggregate size at the current price level.
         */
        virtual mama_i64_t           getPriceLevelSizeChange ()  const = 0;

      
        /**
         * The action to apply to the orderbook for this price level. Can have a
         * value of:
         * <ul>
         * <li>A : Add a new price level</li>
         * <li>U : Update an existing price level</li>
         * <li>D : Delete an existing price level</li>
         * <li>C : Closing information for price level (often treat the same as
         * Update).</li>
         * </ul>
         *
         * @warning Not supported for V5 entry book updates.
         *
         * @return The price level action.
         */
        virtual char                 getPriceLevelAction()       const = 0;
  
        /**
         * Side of book at current price level.
         *
         * <ul>
         * <li>B : Bid side. Same as 'buy' side.</li>
         * <li>A : Ask side. Same as 'sell' side.</li>
         * </ul>
         *
         * @return The price level side.
         */
        virtual char                 getPriceLevelSide()         const = 0;
  
        /**
         * Time of order book price level.
         *
         * @return The time of the orderbook price level.
         */
        virtual const MamaDateTime&  getPriceLevelTime()         const = 0;

        /**
         * Number of order book entries at current price level.
         *  
         * @warning Not supported for V5 entry book updates.
         *
         * @return The number of entries at the current price level.
         */
        virtual mama_f32_t           getPriceLevelNumEntries()   const = 0;
        
        /**
         * Get the Order Type
         *
         * @return The order Type.
         */
        virtual MamdaOrderBookTypes::OrderType  getOrderType ()  const = 0;
      
        /**
         * Set the Order Type
         *
         */
        virtual void  setOrderType (MamdaOrderBookTypes::OrderType orderType)   const = 0;
      
      
        virtual ~MamdaBookAtomicLevel() {};
    };

} // namespace

#endif // MamdaBookAtomicLevelH
