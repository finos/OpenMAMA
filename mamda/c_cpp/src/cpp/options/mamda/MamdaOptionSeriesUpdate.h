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

#ifndef MamdaOptionSeriesUpdateH
#define MamdaOptionSeriesUpdateH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaBasicEvent.h>

namespace Wombat
{

    class MamdaOptionContract;

    /**
     * MamdaOptionSeriesUpdate is an interface that provides access to
     * fields related to option series update events.  Update events
     * include adds/removes of contracts to the chain.
     */
    class MAMDAOPTExpDLL MamdaOptionSeriesUpdate : public MamdaBasicEvent
    {
    public:
        static const char ACTION_UNKNOWN = ' ';
        static const char ACTION_ADD     = 'A';
        static const char ACTION_DELETE  = 'D';

        /**
         * @return The option contract to which the most recent event
         * applies.  If the contract is new, it will have already been
         * added to the chain.  If it is being removed, it will have
         * already been removed from the chain.
         */
        const MamdaOptionContract*  getOptionContract() const;

        /**
         * Return the action related to the last series update message.
         *
         * @return The action related to the last series update message.
         */
        char  getOptionAction() const;
    };

} // namespace

#endif // MamdaOptionSeriesUpdateH
