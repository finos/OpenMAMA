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
 * MamdaTradeHandler is an interface for applications that want to
 * have an easy way to handle trade updates.  The interface defines
 * callback methods for different types of trade-related events:
 * trades, errors/cancels, corrections, recaps and closing reports.
 */

public interface MamdaTradeHandler
{
    /**
     * Method invoked when the current last-trade information for the
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
     * @param recap        Access to the trade fields in the recap update.
     */
    void onTradeRecap (
        MamdaSubscription   subscription,
        MamdaTradeListener  listener,
        MamaMsg             msg,
        MamdaTradeRecap     recap);

    /**
     * Method invoked when a trade is reported.
     *
     * @param subscription    The subscription which received the update.
     * @param listener        The listener which invoked the callback.
     * @param msg             The MamaMsg that triggered this invocation.
     * @param event           Access to the trade data from the update.
     * @param recap           Access to complete trade data.
     */
    void onTradeReport (
        MamdaSubscription   subscription,
        MamdaTradeListener  listener,
        MamaMsg             msg,
        MamdaTradeReport    event,
        MamdaTradeRecap     recap);

    /**
     * Method invoked when a gap in trade reports is discovered.
     *
     * @param subscription  The subscription which received the update.
     * @param listener      The listener which invoked this callback.    
     * @param msg           The MamaMsg that triggered this invocation.
     * @param event         Access to details from the trade gap event.
     * @param recap         Access to complete trade data.
     */
    void onTradeGap (
        MamdaSubscription   subscription,
        MamdaTradeListener  listener,
        MamaMsg             msg,
        MamdaTradeGap       event,
        MamdaTradeRecap     recap);

    /**
     * Method invoked when a trade cancel or error is reported.
     *
     * @param subscription The subscription which received the update.
     * @param listener     The listener which invoked this callback.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param event        Access to the details from the cancel/error event.
     * @param recap        Access to the complete trade data.
     */
    void onTradeCancelOrError (
        MamdaSubscription        subscription,
        MamdaTradeListener       listener,
        MamaMsg                  msg,
        MamdaTradeCancelOrError  event,
        MamdaTradeRecap          recap);

    /**
     * Method invoked when a trade correction is reported.
     *
     * @param subscription  The subscription which received the update.
     * @param listener      The listener which invoked this callback.
     * @param msg           The MamaMsg that triggered this invocation.
     * @param event         Access to the details from the trade correction
     * event.
     * @param recap         Access to the complete trade data.
     */
    void onTradeCorrection (
        MamdaSubscription        subscription,
        MamdaTradeListener       listener,
        MamaMsg                  msg,
        MamdaTradeCorrection     event,
        MamdaTradeRecap          recap);

    /**
     * Method invoked for a closing report.
     *
     * @param subscription The subscription which received the update.
     * @param listener     The listener which invoked this callback.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param event        Access to the details from the trade closing event.
     * @param recap        Access to the complete trade data.
     */
    void onTradeClosing (
        MamdaSubscription        subscription,
        MamdaTradeListener       listener,
        MamaMsg                  msg,
        MamdaTradeClosing        event,
        MamdaTradeRecap          recap);

}
