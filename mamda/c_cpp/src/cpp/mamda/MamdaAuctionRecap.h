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

#ifndef MamdaAuctionRecapH
#define MamdaAuctionRecapH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicRecap.h>
#include <mamda/MamdaFieldState.h>
#include <mamda/MamdaUncrossPriceInd.h>

namespace Wombat
{

    /**
     * MamdaAuctionRecap is an interface that provides access to the 
     * currency related fields.  
     */

    class MAMDAExpDLL MamdaAuctionRecap : public MamdaBasicRecap
    {
    public:
        /**
         * Get the uncross price.
         * 
         * @return uncross price.  The indicative or firm auction price .  
         */
        virtual const MamaPrice&  getUncrossPrice()  const = 0;
      
        /**
         * Get the uncross volume.
         *
         * @return Ask price.   The indicative volume, or the volume turned over in the auction 
         */    
        virtual mama_quantity_t  getUncrossVolume()  const = 0;

        /**
         * Get the uncross price Ind.
         *
         * @return uncross price Ind.   Indicates whether the Price and Volume
         * is an indicative of the current state of the auction or whether its the (firm) auction price and volume 
         */    
        virtual MamdaUncrossPriceInd  getUncrossPriceInd()  const = 0;

        /**
         * Get the auction time.
         * 
         * @return auction time.  The indicative or firm auction time .  
         */
        virtual const MamaDateTime&  getAuctionTime()  const = 0;
        
        /**
         * Get the uncross price fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */    
        virtual MamdaFieldState   getUncrossPriceFieldState()  const = 0;
              
        /**
         * Get the uncross volume fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState   getUncrossVolumeFieldState()  const = 0;

        /**
         * Get the uncross price ind fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState   getUncrossPriceIndFieldState()  const = 0;  

        /**
         * Get the auction time fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */    
        virtual MamdaFieldState   getAuctionTimeFieldState()  const = 0;
         
        virtual ~MamdaAuctionRecap () {};
    };

} // namespace

#endif // MamdaAuctionRecapH



