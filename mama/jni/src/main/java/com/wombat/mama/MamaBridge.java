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

/*
* MamaBridge
*/
public class MamaBridge
{

    static
    {
        initIDs();
    }

    protected MamaBridge()
    {
        /* nothing to do */
    }
    
    /*
    A long holding the C pointer value to
    the underlying mamaBridge C structure
    */
    private long bridgePointer_i = 0;
   
    /*Used to cache ids for callback methods/fields*/
    private static native void initIDs();
}/*end class*/
