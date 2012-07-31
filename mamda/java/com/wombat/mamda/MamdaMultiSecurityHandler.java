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

import com.wombat.mama.MamaBoolean;

/**
 * The MamdaMultiSecurityHandler class is an interface that allows
 * a developer to be notified dynamically when new security updates are
 * received on a group subscription.
 *
 * Note that any actions to register per-security
 * listeners can be added up front (and this callback omitted or left
 * empty) if the symbols are known beforehand.
 */
public interface MamdaMultiSecurityHandler
{
    /**
     * Method invoked when the trade and quote information for a
     * new security has become available
     */
    void onSecurityCreate (
            MamdaSubscription             subscription,
            MamdaMultiSecurityManager     manager,
            String                        symbol);
}
