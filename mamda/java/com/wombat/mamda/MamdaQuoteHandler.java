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

package com.wombat.mamda;

import com.wombat.mama.MamaMsg;

/**
 * MamdaQuoteHandler is an interface for applications that want to
 * have an easy way to handle quote updates.  The interface defines
 * callback methods for different types of quote-related events:
 * quotes and closing-quote updates.
 */

public interface MamdaQuoteHandler
{
    /**
     * Method invoked when the current last-quote information for the
     * security is available.  The reason for the invocation may be
     * any of the following:
     * - Initial image.
     * - Recap update (e.g., after server fault tolerant event or data
     *   quality event.)
     * - After stale status removed.
     *
     * @param subscription The MamdaSubscription handle.
     * @param listener     The MamdaSubscription handle.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param recap        Access to the current value of all fields.
     */
    void onQuoteRecap (
        MamdaSubscription   subscription,
        MamdaQuoteListener  listener,
        MamaMsg             msg,
        MamdaQuoteRecap     recap);

    /**
     * Method invoked when a quote update arrives.
     *
     * @param subscription The MamdaSubscription handle.
     * @param listener     The MamdaSubscription handle.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param event        The MamdaQuoteUpdate event.
     * @param recap        Access to the current value of all fields.
     */
    void onQuoteUpdate (
        MamdaSubscription   subscription,
        MamdaQuoteListener  listener,
        MamaMsg             msg,
        MamdaQuoteUpdate    event,
        MamdaQuoteRecap     recap);

    /**
     * Method invoked when a gap in quote updates is discovered.
     *
     * @param subscription The MamdaSubscription handle.
     * @param listener     The MamdaSubscription handle.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param event        The MamdaQuoteGap event.
     * @param recap        Access to the current value of all fields.
     */
    void onQuoteGap (
        MamdaSubscription   subscription,
        MamdaQuoteListener  listener,
        MamaMsg             msg,
        MamdaQuoteGap       event,
        MamdaQuoteRecap     recap);

    /**
     * Method invoked for a quote closing summary.
     *
     * @param subscription The MamdaSubscription handle.
     * @param listener     The MamdaSubscription handle.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param event        The MamdaQuoteClosing event.
     * @param recap        Access to the current value of all fields.
     */
    void onQuoteClosing (
        MamdaSubscription   subscription,
        MamdaQuoteListener  listener,
        MamaMsg             msg,
        MamdaQuoteClosing   event,
        MamdaQuoteRecap     recap);

}
