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

#ifndef MamdaTradeClosingH
#define MamdaTradeClosingH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    /**
     * MamdaTradeClosing is an interface that provides access to trade
     * closing related fields.
     */
    class MAMDAExpDLL MamdaTradeClosing : public MamdaBasicEvent
    {
    public:

       /**
        * Return the Close price Today's closing price. The close price is
        * populated when official closing prices are sent by the feed
        * after the session close.
        *
        * @return The trade closing price.
        */
        virtual const MamaPrice&  getClosePrice() const = 0;

       /**
        * Get the field state
        *
        * @return MamdaFieldState.  An enumeration representing field state.
        */
        virtual MamdaFieldState  getClosePriceFieldState() const = 0;

       /**
        * Return whether this closing price is indicative or official.
        * 
        * @return Whether the closing price is indicative or otherwise.
        */
        virtual bool  getIsIndicative() const = 0;

        virtual ~MamdaTradeClosing() {};
    };

} // namespace

#endif // MamdaTradeClosingH
