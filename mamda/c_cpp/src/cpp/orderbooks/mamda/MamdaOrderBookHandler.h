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

#ifndef MamdaOrderBookHandlerH
#define MamdaOrderBookHandlerH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaOrderBookSimpleDelta.h>
#include <mamda/MamdaOrderBookComplexDelta.h>
#include <mamda/MamdaOrderBookRecap.h>
#include <mamda/MamdaOrderBookClear.h>
#include <mamda/MamdaOrderBookGap.h>
#include <mamda/MamdaOrderBookComplexDelta.h>

namespace Wombat
{

    class MamdaSubscription;

    /**
     * MamdaOrderBookHandler is an interface for applications that want to
     * have an easy way to handle order book updates.  The interface defines
     * callback methods for different types of orderBook-related events:
     * order book recaps and updates.
     */
    class MAMDAOPTExpDLL MamdaOrderBookHandler
    {
    public:
        /**
         * Method invoked when a full refresh of the order book for the
         * security is available.  The reason for the invocation may be
         * any of the following:
         * - Initial image.
         * - Start-of-day book state.
         * - Recap update (e.g., after server fault tolerant event or data
         *   quality event.)
         * - After stale status removed.
         *
         * @param subscription    The subscription which received the update.
         * @param listener        The order book listener that invoked this
         * callback.
         * @param msg             The MamaMsg that triggered this invocation.
         * @param delta           Always NULL. Reserved for future use.
         * @param event           The order book recap event.
         * @param book            The current full book.
         */
        virtual void onBookRecap (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookComplexDelta*  delta,
            const MamdaOrderBookRecap&         event,
            const MamdaOrderBook&              book) = 0;

        /**
         * Method invoked when a basic order book delta is reported. A basic delta
         * consists of one price levels (add/update/delete), which contains zero or 
         * one entries (add/update/delete).
         * Some feeds do not provide order book entry information.
         *
         * @param subscription    The MamdaSubscription handle.
         * @param listener        The order book listener that received the update.
         * @param msg             The MamaMsg that triggered this invocation.
         * @param event           Details of the delta event.
         * @param book            The current full book (after applying the delta).
         */
        virtual void onBookDelta (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookSimpleDelta&   event,
            const MamdaOrderBook&              book) = 0;

        /**
         * Method invoked when an order book delta is reported. A delta
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
        virtual void onBookComplexDelta (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookComplexDelta&  event,
            const MamdaOrderBook&              book) = 0;

        /**
         * Deprecated. Market Order updates now available in the onBookDelta callback
         * Obtaining the price level from the Delta and calling getOrderType() can be 
         * used to determine if it is a LIMIT or MARKET level 
         */
        virtual void onMarketOrderDelta (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookSimpleDelta&   event,
            const MamdaOrderBook&              book) {};

        /**
         * Deprecated. Market Order updates now available in the onBookDelta callback
         * Obtaining the price level from the Delta and calling getOrderType() can be 
         * used to determine if it is a LIMIT or MARKET level 
         */
        virtual void onMarketOrderComplexDelta (
            MamdaSubscription*                 subscription,
            MamdaOrderBookListener&            listener,
            const MamaMsg*                     msg,
            const MamdaOrderBookComplexDelta&  event,
            const MamdaOrderBook&              book) {};

            
        /**
         * Method invoked when an order book is cleared.
         *
         * @param subscription    The MamdaSubscription handle.
         * @param listener        The order book listener that caused the clear.
         * @param msg             The MamaMsg that triggered this invocation.
         * @param event           The clear event.
         * @param book            The current state of the book (empty).
         */
        virtual void onBookClear (
            MamdaSubscription*          subscription,
            MamdaOrderBookListener&     listener,
            const MamaMsg*              msg,
            const MamdaOrderBookClear&  event,
            const MamdaOrderBook&       book) = 0;

        /**
         * Method invoked when a gap in order book updates is discovered.
         * It is usual for a recap to follow shortly after an order book
         * gap is detected.
         *
         * @param subscription    The MamdaSubscription handle.
         * @param listener        The order book listener that detected the gap.
         * @param msg             The MamaMsg that triggered this invocation.
         * @param event           Provides the sequence number gap range.
         * @param book            The current state of the book.
         */
        virtual void onBookGap (
            MamdaSubscription*          subscription,
            MamdaOrderBookListener&     listener,
            const MamaMsg*              msg,
            const MamdaOrderBookGap&    event,
            const MamdaOrderBook&       book) = 0;

            
        virtual ~MamdaOrderBookHandler() {};

    };

} // namespace

#endif // MamdaOrderBookHandlerH
