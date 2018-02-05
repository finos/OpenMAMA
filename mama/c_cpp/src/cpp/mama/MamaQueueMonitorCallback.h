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

#ifndef MAMA_QUEUE_MONITOR_CB_H__
#define MAMA_QUEUE_MONITOR_CB_H__

namespace Wombat 
{

    /**
    * Receive callbacks when certain conditions for the MamaQueue are met.
    *
    * Currently only one callback is defined which is invoked when the specified
    * size limit on the MamaQueue is exceeded.
    */
    class MAMACPPExpDLL MamaQueueMonitorCallback
    {
        public:
            virtual ~MamaQueueMonitorCallback () 
            {
            }

            /**
             * Callback invoked if an upper size limit has been specified for a
             * queue and that limit has been exceeded.
             *
             * @param queue Pointer to the queue for which this callback was
             * invoked.
             * @param size The number of events on the queue if supported;
             * otherwise 0.
             * @param closure User supplied data when the callback object was
             * registered.
             */
            virtual void onHighWatermarkExceeded (
                MamaQueue*  queue, 
                size_t      size, 
                void*       closure) = 0;

            /**
             * Callback when low water mark is reached. Only supported by Wombat
             * TCP middleware.
             *
             * @param queue Pointer to the queue for which this callback was
             * invoked.
             * @param size The number of events on the queue.
             * @param closure User supplied data when the callback object was
             * registered.
             */
            virtual void onLowWatermark (
                MamaQueue*  queue, 
                size_t      size, 
                void*       closure) = 0;
    };

} // namespace Wombat
#endif 
