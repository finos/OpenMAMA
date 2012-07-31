/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.orderbook;

import com.wombat.mamda.*;
import com.wombat.mama.MamaMsg;

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

public interface MamdaBookAtomicBookHandler
{

    /**
     * Method invoked before we start processing the first level in a message.
     * The book should be cleared when isRecap == true.
     *
     * @param subscription   The MamdaSubscription handle.
     * @param listener       The listener handling recaps/updates. 
     * @param isRecap        Whether the first update was a recap.
     */
    void onBookAtomicBeginBook (
        MamdaSubscription           subscription,
        MamdaBookAtomicListener     listener,
        boolean                     isRecap );
    
    /**
     * Method invoked when we stop processing the last level in a message. We 
     * invoke this method after the last entry for the level gets processed. 
     * The subscription may be destroyed from this callback.
     *
     * @param subscription   The MamdaSubscription handle.
     * @param listener       The listener handling recaps/updates. 
     *
     */
    void onBookAtomicEndBook (
        MamdaSubscription           subscription,
        MamdaBookAtomicListener     listener);

    /**
     * Method invoked when an order book is cleared.
     *
     * @param subscription   The MamdaSubscription handle.
     * @param listener       The listener handling recaps/updates. 
     * @param msg            The MamaMsg that triggered this invocation.
     */
    void onBookAtomicClear (
        MamdaSubscription           subscription,
        MamdaBookAtomicListener     listener,
        MamaMsg                     msg);

    /**
     * Method invoked when a gap in orderBook reports is discovered.
     *
     * @param subscription   The MamdaSubscription handle.
     * @param listener       The listener handling recaps/updates. 
     * @param msg            The MamaMsg that triggered this invocation.
     * @param event          The gap value object.
     */
    void onBookAtomicGap (
        MamdaSubscription                subscription,
        MamdaBookAtomicListener          listener,
        MamaMsg                          msg,
        MamdaBookAtomicGap               event);
        
}
