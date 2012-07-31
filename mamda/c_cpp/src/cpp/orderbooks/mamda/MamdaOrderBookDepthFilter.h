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

#ifndef MamdaOrderBookDepthFilterH
#define MamdaOrderBookDepthFilterH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaFieldState.h>

using namespace Wombat;

class MamdaOrderBookDepthFilterImpl;

class MamdaOrderBookDepthFilter : public MamdaOrderBookListener
                                , public MamdaOrderBookHandler
{
public:

    MamdaOrderBookDepthFilter (MamdaOrderBookListener& parent, size_t depth);

    virtual ~MamdaOrderBookDepthFilter ();

    /**
     * Add a specialized order book handler.  Currently, only one
     * handler can (and must) be registered.
     *
     * @param handler The hadler registered to receive order book update
     * callbacks.
     */
    virtual void        addHandler (MamdaOrderBookHandler* handler);
    /**
     * Remove the reference of handlers from the internal list.
     * Memory is not freed.
     */
    virtual void        removeHandlers    ();

    /**
    * Return the filtered orderbook
    */
    virtual const MamdaOrderBook* getOrderBook () const;

    /**
    * Return the full depth orderbook
    */
    virtual const MamdaOrderBook* getFullOrderBook () const;
    
    virtual void onMsg (MamdaSubscription*  subscription,
                        const MamaMsg&      msg,
                        short               msgType) { };
                        
    virtual void onBookRecap (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookComplexDelta*   delta,
        const MamdaOrderBookRecap&          event,
        const MamdaOrderBook&               book);


    virtual void onBookDelta (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookSimpleDelta&    event,
        const MamdaOrderBook&               book);

    virtual void onBookComplexDelta (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookComplexDelta&   event,
        const MamdaOrderBook&               book);

    virtual void onBookClear (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookClear&          event,
        const MamdaOrderBook&               book);

    virtual void onBookGap (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookGap&            event,
        const MamdaOrderBook&               book);
         
private:
    struct MamdaOrderBookListenerImpl;
    MamdaOrderBookDepthFilterImpl&  mImpl;
};

#endif // MamdaOrderBookDepthFilterH

