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

#ifndef MamdaOptionChainHandlerH
#define MamdaOptionChainHandlerH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOptionChainListener.h>

namespace Wombat
{

    class MamdaSubscription;

    /**
     * Subclasses of this interface can be registered with the
     * <code>MamdaOptionChainListener</code> in order to receive callbacks
     * whenever the state of the underlying option chain changes on receipt of
     * options updates.
     */
    class MAMDAOPTExpDLL MamdaOptionChainHandler
    {
    public:
        /**
         * Method invoked when an updated full option chain is
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
         * @param chain        The complete option chain.
         */
        virtual void onOptionChainRecap (
            MamdaSubscription*         subscription,
            MamdaOptionChainListener&  listener,
            const MamaMsg&             msg,
            MamdaOptionChain&          chain) = 0;

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
         * @param subscription  The subscription which received the update.
         * @param listener      The listener which invoked this callback.
         * @param msg           The MamaMsg that triggered this invocation.
         * @param contract      The newly created option contract.
         * @param chain         The complete option chain.
         */
        virtual void onOptionContractCreate (
            MamdaSubscription*         subscription,
            MamdaOptionChainListener&  listener,
            const MamaMsg&             msg,
            MamdaOptionContract&       contract,
            MamdaOptionChain&          chain) = 0;

        /**
         * Method invoked upon when a new contract is added to or removed
         * from the option chain, excluding upon receipt of the initial
         * value.  This method is typically invoked inly for special
         * events, such as when options are added intraday or when options
         * expire.  Note: onOptionContractCreate() is also invoked when an
         * option is added intraday.
         *
         * @param subscription  The subscription which received the update.
         * @param listener      The listener which invoked this callback.
         * @param msg           The MamaMsg that triggered this invocation.
         * @param event         Access to the series update event details.
         * @param chain         The complete option chain.
         * @see onOptionContractCreate.
         */
        virtual void onOptionSeriesUpdate (
            MamdaSubscription*              subscription,
            MamdaOptionChainListener&       listener,
            const MamaMsg&                  msg,
            const MamdaOptionSeriesUpdate&  event,
            MamdaOptionChain&               chain) = 0;

        /**
         * Method invoked when a gap in option chain updates is discovered.
         *
         * @param subscription  The MamdaSubscription handle.
         * @param listener      The listener which invoked this callback.
         * @param msg           The MamaMsg that triggered this invocation.
         * @param chain         The complete option chain.
         */
        virtual void onOptionChainGap (
            MamdaSubscription*              subscription,
            MamdaOptionChainListener&       listener,
            const MamaMsg&                  msg,
            MamdaOptionChain&               chain) = 0;
            
            
        virtual ~MamdaOptionChainHandler() {};

    };

} // namespace

#endif // MamdaOptionChainHandlerH
