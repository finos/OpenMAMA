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

#ifndef MamdaQuoteHandlerH
#define MamdaQuoteHandlerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaQuoteListener.h>

namespace Wombat
{

    class MamdaSubscription;

    /**
     * MamdaQuoteHandler is an interface for applications that want to
     * have an easy way to handle quote updates.  The interface defines
     * callback methods for different types of quote-related events:
     * quotes and closing-quote updates.
     */
    class MAMDAExpDLL MamdaQuoteHandler
    {
    public:
        /**
         * Method invoked when the current last-quote information for the
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
         * @param recap        Access to the full quote recap details.
         */

        virtual void onQuoteRecap (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteRecap&  recap) = 0;

        /**
         * Method invoked when a quote is reported.
         *
         * @param subscription    The subscription which received the update.
         * @param listener        The listener which invoked this callback.
         * @param msg             The MamaMsg that triggered this invocation.
         * @param quote           Access to the quote update details.
         * @param recap           Access to the full quote details.
         */
        virtual void onQuoteUpdate (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteUpdate& quote,
            const MamdaQuoteRecap&  recap) = 0;

        /**
         * Method invoked when a gap in quote reports is discovered.
         *
         * @param subscription  The subscription which received the update.
         * @param listener      The listener which invoked this callback.
         * @param msg           The MamaMsg that triggered this invocation.
         * @param event         Access to the quote gap event details.
         * @param recap         Access to the full quote details.
         */
        virtual void onQuoteGap (
            MamdaSubscription*      subscription,
            MamdaQuoteListener&     listener,
            const MamaMsg&          msg,
            const MamdaQuoteGap&    event,
            const MamdaQuoteRecap&  recap) = 0;

        /**
         * Method invoked for a closing report.
         *
         * @param subscription  The subscription which received the update.
         * @param listener      The listener which invoked this callback.
         * @param msg           The MamaMsg that triggered this invocation.
         * @param event         Access to the closing quote details.
         * @param recap         Access to the full quote details.
         */
        virtual void onQuoteClosing (
            MamdaSubscription*        subscription,
            MamdaQuoteListener&       listener,
            const MamaMsg&            msg,
            const MamdaQuoteClosing&  event,
            const MamdaQuoteRecap&    recap) = 0;

        /**
         * Method invoked for a message marked as out of sequence.
         * Listener must be configured to check the Msg Qualifier, i.e.,
         * call <Code>setControlProcessingByMsgQual()</code> on listener
         * passing a value of true.
         *
         * @param subscription   The subscription which received the callback.
         * @param listener       The quote listener which invoked this callback.
         * @param msg            The MamaMsg that triggered this invocation.
         * @param event          The possibly out of sequence event object.
         * @param recap          The recap object.
         */
        virtual void onQuoteOutOfSequence (
            MamdaSubscription*              subscription,
            MamdaQuoteListener&             listener,
            const MamaMsg&                  msg,
            const MamdaQuoteOutOfSequence&  event,
            const MamdaQuoteRecap&          recap) = 0;

        /**
         * Method invoked for a message which is marked as possibly duplicate
         * Listener must be configured to check the Msg Qualifier, i.e.,
         * call <code>setControlProcessingByMsgQual</code> on listener
         * passing a value of true.
         *
         * @param subscription   The MamdaSubscription handle.
         * @param listener       The quote listener.
         * @param msg            The MamaMsg that triggered this invocation.
         * @param event          The possibly duplicate event object.
         * @param recap          The recap object.
         */
        virtual void onQuotePossiblyDuplicate (
            MamdaSubscription*                  subscription,
            MamdaQuoteListener&                 listener,
            const MamaMsg&                      msg,
            const MamdaQuotePossiblyDuplicate&  event,
            const MamdaQuoteRecap&              recap) = 0;
        
        virtual ~MamdaQuoteHandler () {};
    };

} // namespace

#endif // MamdaQuoteHandlerH
