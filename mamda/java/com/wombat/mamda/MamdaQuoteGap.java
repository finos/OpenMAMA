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
 * MamdaQuoteGap is an interface that provides access to fields
 * related to gap in quote reporting.
 */

public interface MamdaQuoteGap extends MamdaBasicEvent
{
    /**
     * The starting sequence number of detected missing quotes based on
     * the quote count.
     */
    public long   getBeginGapSeqNum();

    /**
     * @return the begin gap seq num Field State
     */
    short  getBeginGapSeqNumFieldState();

    /**
     * The end sequence number of detected missing quotes based on
     * the quote count.
     */
    public long  getEndGapSeqNum();

    /**
     * @return the end gap seq num Field State
     */
    short  getEndGapSeqNumFieldState();
}
