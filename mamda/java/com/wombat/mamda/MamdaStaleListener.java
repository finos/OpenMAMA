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
 * MamdaStaleListener defines an interface for handling changes in
 * staleness notifications for a MamdaSubscription.
 */

public interface MamdaStaleListener
{
    /**
     * Invoked in response to the onQuality() callback for the
     * underlying MamaSubscription.
     * Typically invoked in response to an underlying symbol
     * level sequence number gap being detected.
     *
     * @param subscription The MamdaSubscription on which the data
     * quality event fired.
     * @param quality Can be MamaQuality.QUALITY_OK,
     *      MamaQuality.QUALITY_MAYBE_STALE, MamaQuality.QUALITY_PARTIAL_STALE
     *      or MamaQuality.QUALITY_STALE
     */
    void onStale (MamdaSubscription  subscription,
                  short              quality);
}
