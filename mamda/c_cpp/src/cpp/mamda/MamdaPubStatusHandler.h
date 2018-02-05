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

#ifndef MamdaPubStatusHandlerH
#define MamdaPubStatusHandlerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaPubStatusListener.h>

namespace Wombat
{

    class MamdaSubscription;

    /**
     * MamdaPubStatusHandler is an interface for applications that want to
     * have an easy way to handle feed handler publisher status updates.  
     * The interface defines callback methods for publisher status events:
     */

    class MAMDAExpDLL MamdaPubStatusHandler
    {
    public:

        /**
         * Method invoked when a quote is reported.
         *
         * @param subscription  The MamdaSubscription handle.
         * @param listener      The listener which invoked this callback.
         * @param msg           The MamaMsg that triggered this invocation.
         */
        virtual void onPubStatusUpdate (
            MamdaSubscription*          subscription,
            MamdaPubStatusListener&     listener,
            const MamaMsg&              msg) = 0;

        virtual ~MamdaPubStatusHandler() {};
    };

} // namespace

#endif // MamdaPubStatusHandlerH
