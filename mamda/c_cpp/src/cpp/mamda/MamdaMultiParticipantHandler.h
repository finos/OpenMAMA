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

#ifndef MamdaMultiParticipantHandlerH
#define MamdaMultiParticipantHandlerH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    class MamdaSubscription;
    class MamdaMultiParticipantManager;

    /**
     * The MamdaMultiParticipantHandler class is an interface that allows
     * a developer to be notified dynamically when participants are added to
     * the list.  This is useful for handling information such as NYSE,
     * AMEX and NASDAQ listed securities (including NQDS).  Access to
     * consolidated information (i.e., best bid and offer and consolidated
     * trade info) is also available.
     *
     * Note that any actions to register per-participant or consolidated
     * listeners can be added up front (and this callback omitted or left
     * empty) if the participant IDs are known beforehand.
     */

    class MAMDAExpDLL MamdaMultiParticipantHandler
    {
    public:
        /**
         * Method invoked when the consolidated trade and BBO quote
         * information for the security has become available.  This method
         * is invoked only if there is BBO or consolidated trade
         * information available.
         */
        virtual void onConsolidatedCreate (
            MamdaSubscription*             subscription,
            MamdaMultiParticipantManager&  manager) = 0;

        /**
         * Method invoked when the trade and quote information for a
         * participant has become available for the security.  This method
         * is invoked only if there is participant quote or trade
         * information available.  isPrimary is not yet supported!
         */
        virtual void onParticipantCreate (
            MamdaSubscription*             subscription,
            MamdaMultiParticipantManager&  manager,
            const char*                    particpantId,
            bool                           isPrimary) = 0;
            
        virtual ~MamdaMultiParticipantHandler() {};
    };

} // namespace

#endif // MamdaMultiParticipantHandlerH
