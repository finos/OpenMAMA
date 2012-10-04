/* $Id:
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

/*
* Wrapper class for the native C inbox structure and related functions
*/
public class MamaInbox
{
    static
    {
        initIDs();
    }

    /*
        A long value containing a pointer to the underlying C inbox structure
    */
    private long    inboxPointer_i   =   0;

    /*
        We need this to enable us to free in the closure we have allocated
        during the creation of the inbox C structure. There is no other
        way of associating it with the MamaInbox during call to destroy()
    */
    private long    internalCClosurePointer_i  =   0;

    /**
     * Create an inbox.
     *
     * @param transport     The MamaTransport.
     * @param queue         The MamaQueue.
     * @param callback      The object implementing the callback functions.
     */
    public native void create (MamaTransport transport,
                               MamaQueue queue,
                               MamaInboxCallback callback);

    public void destroy()
    {
        if(0==inboxPointer_i)
        {
            /*Maybe ignore this and simply exit?*/
            throw new MamaException("Must call create() before destroy()");
        }
        _destroy();
    }

    private native void _destroy();

    private static native void initIDs();

}/*End class*/
