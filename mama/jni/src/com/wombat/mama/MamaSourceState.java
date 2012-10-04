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

/**
 * MamaSource state class implemented with typesafe 
 * enumeration. 
 */
public final class MamaSourceState
{
    private final String stateName;
    
    private MamaSourceState (String name) 
    {
        stateName = name;
    }
    public String toString ()
    {
        return stateName;
    }
    /* mama source in off state*/
    public static final MamaSourceState OFF = new MamaSourceState("OFF");
    /* mama source in on state*/
    public static final MamaSourceState OK  = new MamaSourceState("OK");
    /* mama source in uknown state*/
    public static final MamaSourceState UKNOWN = new MamaSourceState ("UKNOWN");
      
}
