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

#ifndef MamdaCurrencyRecapH
#define MamdaCurrencyRecapH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicRecap.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    /**
     * MamdaCurrencyRecap is an interface that provides access to the 
     * currency related fields.  
     */

    class MAMDAExpDLL MamdaCurrencyRecap : public MamdaBasicRecap
    {
    public:
        /**
         * Get the currency bid price.
         * 
         * @return Bid price.  The highest price that the representative
         * party/group is willing to pay to buy the security.  
         */
        virtual const MamaPrice&  getBidPrice()  const = 0;
      
        /**
         * Get the currency ask price.
         *
         * @return Ask price.  The lowest price that the representative
         * party/group is willing to take to sell the security.  
         */    
        virtual const MamaPrice&  getAskPrice()  const = 0;

        /**
         * Get the quote ask price fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */    
        virtual MamdaFieldState   getBidPriceFieldState()  const = 0;
      
        /**
         * Get the quote ask price fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState   getAskPriceFieldState()  const = 0;

        virtual ~MamdaCurrencyRecap() {};
    };

} // namespace

#endif // MamdaCurrencyRecapH



