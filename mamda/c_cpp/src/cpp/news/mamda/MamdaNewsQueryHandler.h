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

#ifndef MamdaNewsQueryHandlerH
#define MamdaNewsQueryHandlerH

#include <mamda/MamdaOptionalConfig.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    class MamdaNewsQuery;
    class MamdaNewsManager;
    class MamdaSubscription;
    class MamaMsg;
    class MamdaNewsHeadline;
    class MamdaQuery;

    /**
     * MamdaNewsHandler is an interface for applications that want to
     * have an easy way to handle news queries. The interface defines
     * callback methods for different types of trade-related events:
     * trades, errors/cancels, corrections, recaps and closing reports.
     */

    class MAMDAOPTExpDLL MamdaNewsQueryHandler
    {
    public:
        /**
         * Method invoked when a news headline is received.
         *
         * @param manager   The news manager invoking the callback.
         * @param msg       The MamaMsg that triggered this invocation.
         * @param headline  Access to the headline details.
         * @param closure   Access to the user supplied closure data.
         */
        virtual void onNewsQueryHeadline (
            MamdaNewsManager&         manager,
            const MamaMsg&            msg,
            const MamdaNewsHeadline&  headline,
            const MamdaQuery&         query,
            void*                     closure) = 0;
           
        virtual void onNewsQueryError (
            MamdaNewsManager&         manager,
            MamaMsg*                  msg,
            const char*               errorStr,
            const MamaStatus&         status,
            const MamdaQuery&         query,
            void*                     closure) = 0;
        
        virtual void onNewsQueryComplete (
            MamdaNewsManager&         manager,
            const MamdaQuery&         query,
            void*                     closure)
            {};
            
        virtual ~MamdaNewsQueryHandler() {};
    };

} // namespace

#endif // MamdaNewsQueryHandlerH
