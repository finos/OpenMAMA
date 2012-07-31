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

#ifndef MamdaTradeGapH
#define MamdaTradeGapH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    /**
     * MamdaTradeGap is an interface that provides access to trade gap
     * related fields.
     */
    class MAMDAExpDLL MamdaTradeGap : public MamdaBasicEvent
    {
    public:

        /**
         * The starting sequence number of detected missing trades based on
         * the trade count.
         *
         * @return The start of the sequence number gap.
         */
        virtual mama_seqnum_t  getBeginGapSeqNum() const = 0;

        /**
         * 
         * @return The Field State of the start of the sequence number gap.
         */
        virtual MamdaFieldState  getBeginGapSeqNumFieldState() const = 0;

        /**
         * The end sequence number of detected missing trades based on the
         * trade count.
         *
         * @return The end of the sequence number gap.
         */
        virtual mama_seqnum_t  getEndGapSeqNum() const = 0;
        
        /**
         *
         * @return The field state of the end of the sequence number gap.
         */
        virtual MamdaFieldState  getEndGapSeqNumFieldState() const = 0;        
        
        virtual ~MamdaTradeGap() {};
    };


} // namespace

#endif // MamdaTradeGapH
