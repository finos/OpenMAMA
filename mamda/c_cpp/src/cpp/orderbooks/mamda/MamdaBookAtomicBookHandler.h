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

#ifndef MamdaBookAtomicBookHandlerH
#define MamdaBookAtomicBookHandlerH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaBookAtomicLevel.h>
#include <mamda/MamdaBookAtomicListener.h>
#include <mamda/MamdaBookAtomicGap.h>

namespace Wombat
{

    class MamdaSubscription;

    /**
     * MamdaBookAtomicBookHandler is an interface for applications that need to
     * know when a MamdaBookAtomicListener finishes processing a single book
     * update.
     * 
     * This may be useful for applications that wish to destroy the subscription 
     * from a callback as the subscription can only be destroyed after the message
     * processing is complete. Furthermore, it allows applications to determine
     * when to clear the book when a recap arrives.
     */
    class MAMDAOPTExpDLL MamdaBookAtomicBookHandler
    {
    public:
        /**
         * Method invoked before we start processing the first level in a message.
         * The book should be cleared when isRecap == true.
         *
         * @param subscription   The MamdaSubscription handle.
         * @param listener       The listener handling recaps/updates. 
         * @param isRecap        Whether the first update was a recap.
         */
        virtual void onBookAtomicBeginBook (
            MamdaSubscription*           subscription,
            MamdaBookAtomicListener&     listener,
            bool                         isRecap ) = 0;
        
        /**
         * Method invoked when we stop processing the last level in a message. We 
         * invoke this method after the last entry for the level gets processed. 
         * The subscription may be destroyed from this callback.
         *
         * @param subscription   The MamdaSubscription handle.
         * @param listener       The listener handling recaps/updates. 
         * 
         */
        virtual void onBookAtomicEndBook (
            MamdaSubscription*           subscription,
            MamdaBookAtomicListener&     listener) = 0;

        /**
         * Method invoked when an order book is cleared.
         *
         * @param subscription   The MamdaSubscription handle.
         * @param listener       The listener handling recaps/updates. 
         * @param msg            The MamaMsg that triggered this invocation.
         */
        virtual void onBookAtomicClear (
            MamdaSubscription*          subscription,
            MamdaBookAtomicListener&    listener,
            const MamaMsg&              msg) = 0;

        /**
         * Method invoked when a gap in orderBook reports is discovered.
         *
         * @param subscription   The MamdaSubscription handle.
         * @param listener       The listener handling recaps/updates. 
         * @param msg            The MamaMsg that triggered this invocation.
         * @param event          The gap value object.
         */
        virtual void onBookAtomicGap (
            MamdaSubscription*                subscription,
            MamdaBookAtomicListener&          listener,
            const MamaMsg&                    msg,
            const MamdaBookAtomicGap&         event) = 0;
            
        virtual ~MamdaBookAtomicBookHandler() {};
    };

} // namespace

#endif // MamdaBookAtomicBookHandlerH
