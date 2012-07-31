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
 * Process updates on the order book.
 * Concrete instances of this interface are passed to the
 * <code>MamdaOrderBookListener</code> in order for an application to receive
 * callbacks on order book update events.
 */

public interface MamdaOrderBookHandler
{
    /**
     * Method invoked when an updated full order book image is
     * available.  The reason for the invocation may be any of the
     * following:
     * - Initial image.
     * - Recap update (e.g., after server fault tolerant event or data
     *   quality event.)
     * - After stale status removed.
     *
     * @param subscription The subscription which received the update.
     * @param listener     The listener which invoked this callback.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param delta        The order book delta generated from the update.
     * @param event        The MamdaOrderBookRecap.
     * @param book         The book.
     */
    void onBookRecap (
        MamdaSubscription           subscription,
        MamdaOrderBookListener      listener,
        MamaMsg                     msg,
        MamdaOrderBookComplexDelta  delta,
        MamdaOrderBookRecap         event,
        MamdaOrderBook              book);

    /**
     * Method invoked when a regular update for an order book is
     * available.
     *
     * @param subscription The subscription which received the update.
     * @param listener     The listener which invoked this callback.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param event        The order book delta generated from the update.
     * @param book         The full order book after the update is applied.
     */
    void onBookDelta (
        MamdaSubscription         subscription,
        MamdaOrderBookListener    listener,
        MamaMsg                   msg,
        MamdaOrderBookSimpleDelta event,
        MamdaOrderBook            book);

    /**
     * Method invoked when an order book delta is reported.  A delta
     * consists of one or more price levels (add/update/delete), each
     * of which contains zero or more entries (add/update/delete).
     * Some feeds do not provide order book entry information.
     *
     * @param subscription    The MamdaSubscription handle.
     * @param listener        The order book listener that received the update.
     * @param msg             The MamaMsg that triggered this invocation.
     * @param event           Details of the the delta.
     * @param book            The current full book (after applying the delta).
     */
    void onBookComplexDelta (
        MamdaSubscription           subscription,
        MamdaOrderBookListener      listener,
        MamaMsg                     msg,
        MamdaOrderBookComplexDelta  event,
        MamdaOrderBook              book);

    /**
     * Method invoked when an order book is cleared.
     *
     * @param subscription The subscription which received the update.
     * @param listener     The listener which invoked this callback.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param event        The clear event.
     */
    void onBookClear (
        MamdaSubscription        subscription,
        MamdaOrderBookListener   listener,
        MamaMsg                  msg,
        MamdaOrderBookClear      event,
        MamdaOrderBook           book);

    /**
     * Method invoked when a gap in order book updates is discovered.
     *
     * @param subscription   The subscription which received the update.
     * @param listener       The listener which invoked this callback.
     * @param msg            The MamaMsg that triggered this invocation.
     * @param event          Access to details on the order book gap event.
     * @param fullBook       The full order book.
     */
    void onBookGap (
        MamdaSubscription        subscription,
        MamdaOrderBookListener   listener,
        MamaMsg                  msg,
        MamdaOrderBookGap        event,
        MamdaOrderBook           fullBook);
}
