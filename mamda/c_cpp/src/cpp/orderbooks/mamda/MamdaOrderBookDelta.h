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

#ifndef MamdaOrderBookDeltaH
#define MamdaOrderBookDeltaH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaOrderBook.h>

namespace Wombat
{

    /**
     * MamdaOrderBookDelta is an interface that provides access to order book
     * related fields.
     */
    class MAMDAOPTExpDLL MamdaOrderBookDelta : public MamdaBasicEvent
    {
    public:
        /**
         * Return the order book delta.  In the case of recap order book,
         * the delta will be the entire recap.
         *
         * @return The order book delta.
         */
        virtual const MamdaOrderBook&  getDeltaOrderBook () const = 0;
        
        virtual ~MamdaOrderBookDelta() {};
    };

} // namespace

#endif // MamdaOrderBookDeltaH
