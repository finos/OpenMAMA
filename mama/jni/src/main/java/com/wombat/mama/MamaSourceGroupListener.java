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
 * Applications interested in receiving notifications on Events from the
 * MamaSourcegroup can register and unregister their interest. On events 
 * such as state change all registered applications are notified.
 */

public interface MamaSourceGroupListener
{
    /**
     * Register to receive state change event notifications
     */
    public void registerStateChangeListener(MamaSourceStateChangeListener event);
    /**
     * Unregister from state change event notifications
     */
    public void unregisterStateChangeListener(MamaSourceStateChangeListener event);
}
