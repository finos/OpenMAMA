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
import com.wombat.mama.*;

/**
 * MamdaBookAtomicLevelHandler is an interface for applications that want to
 * have an easy way to handle order book Price Level updates.  The interface
 * defines callback methods for different types of orderBook-related events:
 * order book recaps and updates.
 */

public interface MamdaBookAtomicLevelHandler
{
    /**
     * Method invoked when a full refresh of the order book for the
     * security is available.  The reason for the invocation may be
     * any of the following:
     * - Initial image.
     * - Recap update (e.g., after server fault tolerant event or data
     *   quality event.)
     * - After stale status removed.
     *
     * @param subscription   The MamdaSubscription handle.
     * @param listener       The listener handling the recap. 
     * @param msg            The MamaMsg that triggered this invocation.
     * @param level          The Price Level recap.
     */
    void onBookAtomicLevelRecap (
        MamdaSubscription           subscription,
        MamdaBookAtomicListener     listener,
        MamaMsg                     msg,
        MamdaBookAtomicLevel        level);


    /**
     * Method invoked when an order book delta is reported.
     *
     * @param subscription   The MamdaSubscription handle.
     * @param listener       The listener handling the update. 
     * @param msg            The MamaMsg that triggered this invocation.
     * @param level          The Price Level update.
     */
    void onBookAtomicLevelDelta (
        MamdaSubscription           subscription,
        MamdaBookAtomicListener     listener,
        MamaMsg                     msg,
        MamdaBookAtomicLevel        level);
}
