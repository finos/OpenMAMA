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

/**
 * MamdaTradeGap is an interface that provides access to fields
 * related to gap in trade reporting.
 */

public interface MamdaTradeGap extends MamdaBasicEvent
{
    /**
     * The starting sequence number of detected missing trades based on
     * the trade count.
     */
    public long   getBeginGapSeqNum();
    /**
     * @return Returns the BeginGapSeqNum Field State.
     */
    public short getBeginGapSeqNumFieldState();

    /**
     * The end sequence number of detected missing trades based on the
     * trade count.
     */
    public long   getEndGapSeqNum();
    /**
     * @return Returns the EndGapSeqNum Field State.
     */
    public short getEndGapSeqNumFieldState();

}
