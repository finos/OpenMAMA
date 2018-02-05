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

#ifndef MamdaQuoteGapH
#define MamdaQuoteGapH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    /**
     * MamdaQuoteGap is an interface that provides access to quote gap
     * related fields.
     */
    class MAMDAExpDLL MamdaQuoteGap : public MamdaBasicEvent
    {
    public:
        /**
         * The starting sequence number of detected missing quotes based on
         * the quote count.
         *
         * @return The start gap sequence number.
         */
        virtual mama_seqnum_t  getBeginGapSeqNum() const = 0;

        /**
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getBeginGapSeqNumFieldState() const = 0;

        /**
         * The end sequence number of detected missing quotes based on
         * the quote count.
         *
         * @return The end gap sequence number.
         */
        virtual mama_seqnum_t  getEndGapSeqNum() const = 0;

        /**
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getEndGapSeqNumFieldState() const = 0;

        virtual ~MamdaQuoteGap() {};
    };

} // namespace

#endif // MamdaQuoteGapH
