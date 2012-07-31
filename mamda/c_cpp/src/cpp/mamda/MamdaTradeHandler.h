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

#ifndef MamdaTradeHandlerH
#define MamdaTradeHandlerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaTradeListener.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    /**
     * MamdaTradeHandler is an interface for applications that want to
     * have an easy way to handle trade updates.  The interface defines
     * callback methods for different types of trade-related events:
     * trades, errors/cancels, corrections, recaps and closing reports.
     */
    class MAMDAExpDLL MamdaTradeHandler
    {
    public:

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
         * @param listener     The listener which invoked the callback.
         * @param msg          The MamaMsg that triggered this invocation.
         * @param recap        Access to the details in the trade recap.
         */
        virtual void onTradeRecap (
            MamdaSubscription*      subscription,
            MamdaTradeListener&     listener,
            const MamaMsg&          msg,
            const MamdaTradeRecap&  recap) = 0;

        /**
         * Method invoked when a trade is reported.
         *
         * @param subscription    The subscription which received this update.
         * @param listener        The listener which invoked this callback.
         * @param msg             The MamaMsg that triggered this invocation.
         * @param event           Access to the trade report event details.
         * @param recap           Access to all trade related fields.
         */
        virtual void onTradeReport (
            MamdaSubscription*      subscription,
            MamdaTradeListener&     listener,
            const MamaMsg&          msg,
            const MamdaTradeReport& event,
            const MamdaTradeRecap&  recap) = 0;

        /**
         * Method invoked when a gap in trade reports is discovered.
         *
         * @param subscription The subscription which detected the gap.
         * @param listener     The listener which invoked this callback.
         * @param msg          The MamaMsg which triggered this invocation.
         * @param event        Access to the gap event details.
         * @param recap        Access to the complete trade information.
         */
        virtual void onTradeGap (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeGap&            event,
            const MamdaTradeRecap&          recap) = 0;

        /**
         * Method invoked when a trade cancel or error is reported.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener which invoked this callback.
         * @param msg          The MamaMsg that triggered this invocation.
         * @param event        Access to the details of the trade cancel or error
         * event.
         * @param recap        Access to the full trade details.
         */
        virtual void onTradeCancelOrError (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeCancelOrError&  event,
            const MamdaTradeRecap&          recap) = 0;

        /**
         * Method invoked when a trade correction is reported.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener which invoked this callback.
         * @param msg          The MamaMsg that triggered this invocation.
         * @param event        Access to the trade correction event details.
         * @param recap        Access to the full trade details.
         */
        virtual void onTradeCorrection (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeCorrection&     event,
            const MamdaTradeRecap&          recap) = 0;

        /**
         * Method invoked for a closing report.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener which invoked this callback.
         * @param msg          The MamaMsg that triggered this invocation.
         * @param event        Access to the trade closing event details.
         * @param recap        Access to the full trade details.
         */
        virtual void onTradeClosing (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeClosing&        event,
            const MamdaTradeRecap&          recap) = 0;

        /**
         * Method invoked for a message marked as out of sequence.
         * Listener must be configured to check the Msg Qualifier, i.e.,
         * call <code>setControlProcessingByMsgQual()</code> on listener
         * passing a value of true;
         *
         * @param subscription   The subscription which received the update.
         * @param listener       The trade listener which invoked this callback.
         * @param msg            The MamaMsg that triggered this invocation.
         * @param event          Details on the out of sequence trade event.
         * @param recap          Access to the full trade details.
         */
        virtual void onTradeOutOfSequence (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeOutOfSequence&  event,
            const MamdaTradeRecap&          recap) = 0;

        /**
         * Method invoked for a message which is marked as possibly duplicate
         * Listener must be configured to check the Msg Qualifier, i.e.,
         * call <code>setControlProcessingByMsgQual</code> on listener
         * passing a value of true;
         *
         * @param subscription   The subscription which received the update.
         * @param listener       The trade listener which invoked this callback.
         * @param msg            The MamaMsg that triggered this invocation.
         * @param event          Details on the possibly duplicate trade event.
         * @param recap          Access to the full trade details.
         */
        virtual void onTradePossiblyDuplicate (
            MamdaSubscription*                  subscription,
            MamdaTradeListener&                 listener,
            const MamaMsg&                      msg,
            const MamdaTradePossiblyDuplicate&  event,
            const MamdaTradeRecap&              recap) = 0;
            
        virtual ~MamdaTradeHandler() {};
    };

} // namespace

#endif // MamdaTradeHandlerH
