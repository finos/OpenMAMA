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

#ifndef MamdaOrderBookGapH
#define MamdaOrderBookGapH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaBasicEvent.h>

namespace Wombat
{

    /**
     * MamdaOrderBookGap is an interface that provides access to order book gap
     * related fields.
     */

    class MAMDAOPTExpDLL MamdaOrderBookGap : public MamdaBasicEvent
    {
    public:
        /**
         * Beginning sequence number in a detected gap event.
         *
         * @return The starting sequence number of the gap.
         */
        virtual mama_seqnum_t  getBeginGapSeqNum() const = 0;

        /**
         * Ending sequence number in a detected gap event.
         *
         * @return The ending sequence number of the gap.
         */
        virtual mama_seqnum_t  getEndGapSeqNum()   const = 0;
        
        virtual ~MamdaOrderBookGap() {};
    };

} // namespace

#endif // MamdaOrderBookGapH
