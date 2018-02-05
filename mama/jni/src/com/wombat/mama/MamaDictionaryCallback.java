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
 * The <code>MamaDictionaryCallback</code> receives notification regarding the
 * population of the data dictionary. Clients implement the interface and
 * pass it to <code>MamaSubscription.createDictionarySubscription()</code>
 *
 * @see MamaDictionary
 */
public interface MamaDictionaryCallback
{
    /**
     * Called when a timeout occurs.
     */
    void onTimeout ();

    /**
     * Invoked when an error occurs.
     * @param message The error message.
     */
    void onError (String message);

    /**
     * Invoked when dictionary creation is complete.
     */
    public void onComplete ();
}
