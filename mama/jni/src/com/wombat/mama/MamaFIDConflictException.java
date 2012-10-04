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

import com.wombat.common.WombatException;

/**
 * An exception that is thrown when an attempt is made to add a field with a
 * duplicate field to a message.
 */
public class MamaFIDConflictException extends WombatException
{
    public MamaFIDConflictException( int fid, final Throwable cause )
    {
        super( "FID alread exists: " + fid, cause );
    }

    public MamaFIDConflictException( int fid )
    {
        this( fid, null );
    }
}
