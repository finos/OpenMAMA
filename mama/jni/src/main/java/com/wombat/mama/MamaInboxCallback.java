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
 * Callback for point to point messaging.
 */
public interface MamaInboxCallback
{
    /**
     * Method invoked when a message arrives.
     *
     * @param inbox The inbox.
     * @param msg The msg.
     */
    public void onMsg (MamaInbox inbox, MamaMsg msg);
    
    /**
     * This method is invoked when an inbox has been completely destroyed,
     * the client can have confidence that no further events will be placed 
     * on the queue for this inbox.
     * 
     * @param inbox The MamaInbox.
     */
    void onDestroy(MamaInbox inbox);
}
