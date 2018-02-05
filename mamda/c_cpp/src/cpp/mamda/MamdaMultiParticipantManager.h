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

#ifndef MamdaMultiParticipantManagerH
#define MamdaMultiParticipantManagerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaMultiParticipantHandler.h>

namespace Wombat
{

    /**
     * MamdaMultiParticipantManager is a class that manages updates on a
     * consolidated basis for securities that may be traded on multiple
     * exchanges and which may have a national best bid and offer.
     * Developers are notified of each element available for the
     * consolidated security, including the national best bid and offer,
     * and each regional exchange.  Developers can pick and choose which
     * elements they wish to provide handling for (e.g., BBO-only,
     * certain regional exchanges, etc.).
     *
     * Note: the MamdaMultiParticipantManager can also be used for securities
     * that are not traded on multiple exchanges.
     */

    class MAMDAExpDLL MamdaMultiParticipantManager : public MamdaMsgListener
    {
    public:

        /**
         * Create a manager for consolidated securities.
         */
        MamdaMultiParticipantManager (const char*  symbol);

        /**
         * Destructor.
         */
        virtual ~MamdaMultiParticipantManager();

        /**
         * Add a specialized handler for notifications about the
         * multi-participant security.  The handler is responsible for
         * initializing any data structures and listeners for each
         * participant as well as the consolidated.  Currently, only one
         * handler can be registered.
         */
        void addHandler (MamdaMultiParticipantHandler*  handler);

        /**
         * Add a specialized message listener (e.g., a MamdaQuoteListener,
         * MamdaTradeListener, etc.) for the consolidated data.
         */
        void addConsolidatedListener (MamdaMsgListener*  listener);

        /**
         * Add a specialized message listener (e.g., a MamdaQuoteListener,
         * MamdaTradeListener, etc.) for a participant.
         */
        void addParticipantListener (MamdaMsgListener*  listener,
                                     const char*        partId);

        /**
         * Implementation of MamdaMsgListener interface.
         */
        void onMsg (MamdaSubscription*  subscription,
                    const MamaMsg&      msg,
                    short               msgType);

    private:
        struct MamdaMultiParticipantManagerImpl;
        MamdaMultiParticipantManagerImpl& mImpl;
    };

} // namespace

#endif // MamdaMultiParticipantManagerH
