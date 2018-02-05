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

#ifndef MamdaQuoteClosingH
#define MamdaQuoteClosingH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    /**
     * MamdaQuoteClosing is an interface that provides access to quote
     * closing related fields.
     */
    class MAMDAExpDLL MamdaQuoteClosing : public MamdaBasicEvent
    {
    public:
        /**
         * Get the closing bid price.
         *
         * @return Today's closing bid price, after the market has closed
         * and the stock has traded today.
         */
        virtual const MamaPrice&  getBidClosePrice() const = 0;

        /**
         * Get the closing bid price field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getBidClosePriceFieldState() const = 0;

        /**
         * Get the closing ask price.
         *
         * @return Today's closing ask price, after the market has closed
         * and the stock has traded today.
         */
        virtual const MamaPrice&  getAskClosePrice() const = 0;

        /**
         * Get the closing bid price field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getAskClosePriceFieldState() const = 0;
        
        virtual ~MamdaQuoteClosing() {};
    };


} // namespace

#endif // MamdaQuoteClosingH
