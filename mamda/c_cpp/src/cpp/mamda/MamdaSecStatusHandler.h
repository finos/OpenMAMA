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

#ifndef MamdaSecStatusHandlerH
#define MamdaSecStatusHandlerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaSecStatusListener.h>

namespace Wombat
{

    class MamdaSubscription;

    /**
     * MamdaSecStatusHandler is an interface for applications that want to
     * have an easy way to handle security status updates.  The interface defines
     * callback methods for security status events:
     */
    class MAMDAExpDLL MamdaSecStatusHandler
    {
    public:
        
        
        virtual void onSecStatusRecap (
            MamdaSubscription*          subscription,
            MamdaSecStatusListener&     listener,
            const MamaMsg&              msg,
            MamdaSecStatusRecap&        recap) = 0;
        
        /**
         * Method invoked when a security status update is received.
         *
         * @param subscription    The subscription which received the update
         * @param listener        The listener which invoked this callback.
         * @param msg             The MamaMsg that triggered this invocation.
         */
        virtual void onSecStatusUpdate (
            MamdaSubscription*          subscription,
            MamdaSecStatusListener&     listener,
            const MamaMsg&              msg) = 0;
            
        virtual ~MamdaSecStatusHandler() {};
    };

} // namespace

#endif // MamdaSecStatusHandlerH
