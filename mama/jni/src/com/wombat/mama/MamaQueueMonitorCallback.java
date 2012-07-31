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

package com.wombat.mama;

/**
 * Definition of the callback methods enabling event queue monitoring.
 * Receiving callbacks when either the high watermark or the low watermark are
 * exceeded.  Not all middlewares will support the concept of both high and
 * low watermarks. Concrete instances of this interface are registered with an
 * event queue using the <code>MamaQueue.setQueueMonitorCallback ()</code>.
 */
public interface MamaQueueMonitorCallback
{
    /**
     * Called when the specified high watermark threshold for the event queue
     * has been exceeded.
     * 
     * @param queue The MamaQueue for which the high watermark was exceeded.
     * @param size  The number of outstanding events on the event queue.
     */
    void onHighWatermarkExceeded (MamaQueue queue, 
                                  long      size);
 
    /**
     * Called when the number of events on the event queue go below the
     * specified low watermark. This is currently only supported for the
     * WombatTCP middleware.
     *
     * @param queue The MamaQueue for which the high watermark was exceeded.
     * @param size  The number of outstanding events on the event queue.
     */
    void onLowWatermark (MamaQueue queue,
                         long      size);
}
