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

#ifndef MamdaAuctionHandlerH
#define MamdaAuctionHandlerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaAuctionListener.h>
#include <mama/mamacpp.h>
#include <mamda/MamdaAuctionRecap.h>
#include <mamda/MamdaAuctionUpdate.h>

namespace Wombat
{

    /**
     * MamdaAuctionHandler is an interface for applications that want to
     * have an easy way to access currency data.  
     * The interface defines callback methods for for receiving updates 
     * on currency data.
     */

    class MAMDAExpDLL MamdaAuctionHandler
    {
    public:
     
        /**
         * Method invoked when the current auction information for the
         * security is available.  The reason for the invocation may be
         * any of the following:
         * - Initial image.
         * - Recap update (e.g., after server fault tolerant event or data
         *   quality event.)
         * - After stale status removed.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener which invoked this callback.
         * @param msg          The MamaMsg that triggered this invocation.
         * @param recap        Access to the full auction recap details.
         */

        virtual void onAuctionRecap (
            MamdaSubscription*         subscription,
            MamdaAuctionListener&      listener,
            const MamaMsg&             msg,
            const MamdaAuctionRecap&   recap) = 0;
        
        /**
         * Method invoked when one or more of the Auction fields have
         * been updated by one of the following market data events:
         * - Initial image.
         * - Recap update (e.g., after server fault tolerant event or data
         *   quality event.)
         * - Generic update..
         *
         * @param subscription  The subscription which received the update.
         * @param listener      The listener which invoked this callback.
         * @param msg           The MamaMsg that triggered this invocation.
         * @param recap         Access to the full auction recap details.
         * @param update        Access to the auction update details.
         */
        virtual void onAuctionUpdate (
            MamdaSubscription*         subscription,
            MamdaAuctionListener&      listener,
            const MamaMsg&             msg,
            const MamdaAuctionRecap&   recap,
            const MamdaAuctionUpdate&  update) = 0;
            
        virtual ~MamdaAuctionHandler () {};
    };

} // namespace

#endif // MamdaAuctionHandlerH
