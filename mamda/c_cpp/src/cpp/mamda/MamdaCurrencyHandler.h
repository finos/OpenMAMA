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

#ifndef MamdaCurrencyHandlerH
#define MamdaCurrencyHandlerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaCurrencyListener.h>
#include <mama/mamacpp.h>
#include <mamda/MamdaCurrencyRecap.h>
#include <mamda/MamdaCurrencyUpdate.h>

namespace Wombat
{

    /**
     * MamdaCurrencyHandler is an interface for applications that want to
     * have an easy way to access currency data.  
     * The interface defines callback methods for for receiving updates 
     * on currency data.
     */

    class MAMDAExpDLL MamdaCurrencyHandler
    {
    public:
     
        /**
         * Method invoked when the current last-currecny information for the
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
         * @param recap        Access to the full quote recap details.
         */
        virtual void onCurrencyRecap (
            MamdaSubscription*          subscription,
            MamdaCurrencyListener&      listener,
            const MamaMsg&              msg,
            const MamdaCurrencyRecap&   recap) = 0;
        
        /**
         * Method invoked when one or more of the Currency fields have
         * been updated by one of the following market data events:
         * - Initial image.
         * - Recap update (e.g., after server fault tolerant event or data
         *   quality event.)
         * - Generic update..
         *
         * @param subscription  The subscription which received the update.
         * @param listener      The listener which invoked this callback.
         * @param msg           The MamaMsg that triggered this invocation.
         * @param recap         The MamaMsg that triggered this invocation.
         * @param update        Access to the currency update details.
         */
        virtual void onCurrencyUpdate (
            MamdaSubscription*          subscription,
            MamdaCurrencyListener&      listener,
            const MamaMsg&              msg,
            const MamdaCurrencyRecap&   recap,
            const MamdaCurrencyUpdate&  update) = 0;
            
        virtual ~MamdaCurrencyHandler() {};
    };

} // namespace

#endif // MamdaCurrencyHandlerH
