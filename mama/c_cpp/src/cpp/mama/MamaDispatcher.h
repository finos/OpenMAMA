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

#ifndef MAMA_DISPATCHER_CPP_H__
#define MAMA_DISPATCHER_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{
class DispatcherImpl;

/**
 * The dispatcher dispatches events from a queue until it is destroyed or 
 * MamaQueue-\>stopDispatch () is called.
 */
class MAMACPPExpDLL MamaDispatcher
{
public:
    ~MamaDispatcher (void);

    MamaDispatcher (void);
    
    /**
     * Create a mamaDispatcher. The dispatcher spawns a thread to
     * dispatch events from a queue. It will continue to dispatch
     * events until it is destroyed or mamaQueue_stopDispatch is
     * called.
     *
     * Only a single dispatcher can be created for a given
     * queue. Attempting to create multiple dispatchers for a queue
     * will result in and error. Dispatching message from a single
     * queue with multiple threads results in messages arriving out of
     * order and sequence number gaps for market data subscriptions.
     *
     * @param queue The MamaQueue.
     */
    void create (MamaQueue* queue);

    /**
     * Destroy the dispatcher;
     */
    void destroy (void);

private:
    mamaDispatcher  mDispatcher;
};
} //namespace Wombat 
#endif // MAMA_DISPATCHER_CPP_H__
