/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda;

import com.wombat.mama.MamaMsg;

/**
 * MamdaErrorListener defines an interface for handling error
 * notifications for a MamdaSubscription.
 */

public interface MamdaErrorListener
{
    /**
     * Provide a callback to handle errors.  The severity is intended
     * as a hint to indicate whether the error is recoverable.
     * 
     * @param subscription The subscription which received the update.
     * @param severity     The severity of the error.
     * @param errorCode    The errorCode. (<code>MamdaErrorCode</code>)
     * @param errorStr     The stringified version of the error.
     *
     * @see MamdaErrorCode
     */
    void onError (MamdaSubscription   subscription,
                  short               severity,
                  short               errorCode,
                  String              errorStr);
}
