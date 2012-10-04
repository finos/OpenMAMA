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

import java.io.Serializable;
import java.util.logging.Logger;

/**
 * This is the superclass for all MAMA exceptions.
 */
public class MamaException extends RuntimeException implements Serializable
{
    private static final Logger logger = Logger.getLogger(
        "com.wombat.mama.MamaException" );
    /**
     * Constructs a new runtime exception with the specified detail message and
     * cause.  <p>Note that the detail message associated with
     * <code>cause</code> is <i>not</i> automatically incorporated in
     * this runtime exception's detail message.
     *
     * @param  message the detail message (which is saved for later retrieval
     *         by the {@link #getMessage()} method).
     * @param  cause the cause (which is saved for later retrieval by the
     *         {@link #getCause()} method).  (A <tt>null</tt> value is
     *         permitted, and indicates that the cause is nonexistent or
     *         unknown.)
     */
    public MamaException( final String message, final Throwable cause )
    {
        super( message, cause );
        logger.fine( message + " : " + cause );
    }

    /** Constructs a new runtime exception with the specified cause and a
     * detail message. This constructor is useful for runtime exceptions
     * that are little more than wrappers for other throwables.
     *
     * @param  cause the cause
     */
    public MamaException( final Throwable cause )
    {
        this( null, cause );
    }

    /**
     * Create a <code>MamaException</code> with the specified message.
     * @param message
     */
    public MamaException( final String message )
    {
        this( message, null );
    }
}
