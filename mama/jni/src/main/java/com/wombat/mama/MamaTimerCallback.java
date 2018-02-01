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
* Concrete instances of this interface are registered with the creation of a
* MamaTimer and used for processing timer event notifications.
*/
public interface MamaTimerCallback
{
    /**
     * Invoked when an event for a timer which has fired is dispatched from
     * the associated event queue.
     * 
     * @param timer The MamaTimer object which resulted in this callback being
     * invoked.
     */
    void onTimer (MamaTimer timer);

    /**
     * This method is invoked when a timer has been completely destroyed,
     * the client can have confidence that no further events will be placed 
     * on the queue for this timer.
     * 
     * @param timer The MamaTimer.
     */
    void onDestroy(MamaTimer timer);
}
