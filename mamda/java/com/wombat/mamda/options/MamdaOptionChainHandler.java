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

package com.wombat.mamda.options;

import com.wombat.mamda.*;
import com.wombat.mama.MamaMsg;

public interface MamdaOptionChainHandler
{
    /**
     * Method invoked when an updated full option chain is
     * available.  The reason for the invocation may be any of the
     * following:
     * - Initial image.
     * - Recap update (e.g., after server fault tolerant event or data
     *   quality event.)
     * - After stale status removed.
     *
     * @param subscription The subscription which received the udpate.
     * @param listener     The listener which invoked the callback.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param chain        The full option chain.
     */
    void onOptionChainRecap (
        final MamdaSubscription         subscription,
        final MamdaOptionChainListener  listener,
        final MamaMsg                   msg,
        final MamdaOptionChain          chain);

    /**
     * Method invoked when a new contract is created in the option
     * chain.  This method gets invoked exactly once for every option
     * contract in the chain.  The primary purpose of this method is
     * to allow a user application to initialize any per-contract data
     * as well to register handlers for trades and quotes.  
     *
     * Note: This method differs from onOptionSeriesUpdate() as
     * follows: onOptionContractCreate() gets invoked every time a
     * contract is added, even for the initial value;
     * onOptionSeriesUpdate() is intended to report especially
     * interesting events and is only invoked when a contract is
     * added/removed after the initial value has been received.
     *
     * @param subscription The subscription which received the update.
     * @param listener     The listener which invoked the callback.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param contract     The newly created option contract.
     * @param chain        The full option chain.
     */
    void onOptionContractCreate (
        final MamdaSubscription         subscription,
        final MamdaOptionChainListener  listener,
        final MamaMsg                   msg,
        final MamdaOptionContract       contract,
        final MamdaOptionChain          chain);

    /**
     * Method invoked upon when a new contract is added to or removed
     * from the option chain, excluding upon receipt of the initial
     * value.  This method is typically invoked inly for special
     * events, such as when options are added intraday or when options
     * expire.  Note: onOptionContractCreate() is also invoked when an
     * option is added intraday.
     *
     * @param subscription The subscription which received the update.
     * @param listener     The listener which invoked the callback.
     * @param msg          The MamaMsg that triggered this invocation.
     * @param event        Access to details from the option series update
     * event.
     * @param chain        The full option chain.
     * 
     * @see #onOptionContractCreate
     */
    void onOptionSeriesUpdate (
        final MamdaSubscription         subscription,
        final MamdaOptionChainListener  listener,
        final MamaMsg                   msg,
        final MamdaOptionSeriesUpdate   event,
        final MamdaOptionChain          chain);
}
