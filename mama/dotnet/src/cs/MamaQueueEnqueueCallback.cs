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

using System;

namespace Wombat
{
    /// <summary>
    /// Callbacks associated with MamaQueues
    /// </summary>
    public interface MamaQueueEnqueueCallback
    {
        /// <summary>
        /// Method which is invoked when an event is enqueued on the queue for which
        /// this function was registered.
        /// 
        /// LBM Bridge: NB! Users may not dispatch events from this function. The
        /// function is invoked from an LBM internal thread. Attempts to dispatch from
        /// here will result in a deadlock.
        /// </summary>
        /// <param name="mamaQueue">
        /// The queue on which the event is being processed.
        /// </param>
        void onEnqueue (MamaQueue mamaQueue);
    }
}
