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

#ifndef MamdaMultiSecurityManagerH
#define MamdaMultiSecurityManagerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaMultiSecurityHandler.h>

namespace Wombat
{

    /**
     * MamdaMultiSecurityManager is a class that manages updates on an
     * arbitrary number of securities that may be traded on multiple
     * exchanges. Developers are notified of each security available, including
     * the national best bid and offer, and each regional exchange, if applicable.
     * Developers can pick and choose which elements they wish to
     * provide handling for (e.g., BBO-only, certain regional exchanges, etc.).
     */
    class MAMDAExpDLL MamdaMultiSecurityManager : public MamdaMsgListener
    {
    public:

        /**
         * Create a manager for consolidated securities.
         */
        MamdaMultiSecurityManager (const char*  symbol);

        /**
         * Destructor.
         */
        virtual ~MamdaMultiSecurityManager();

        /**
         * Add a specialized handler for notifications about each security
         * in the group. The handler is responsible for
         * initializing any data structures and listeners for each
         * security.  Currently, only one handler can be registered.
         */
        void addHandler (MamdaMultiSecurityHandler*  handler);

        /**
         * Add a specialized message listener (e.g., a MamdaQuoteListener,
         * MamdaTradeListener, etc.) for a security.
         */
        void addSecurityListener (MamdaMsgListener*  listener,
                                  const char*        securitySymbol);

        /**
         * Implementation of MamdaMsgListener interface.
         */
        void onMsg (MamdaSubscription*  subscription,
                    const MamaMsg&      msg,
                    short               msgType);

    private:
        struct MamdaMultiSecurityManagerImpl;
        MamdaMultiSecurityManagerImpl& mImpl;
    };

} // namespace

#endif // MamdaMultiSecurityManagerH
