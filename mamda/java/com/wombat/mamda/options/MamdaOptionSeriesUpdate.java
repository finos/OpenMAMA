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

package com.wombat.mamda.options;

import com.wombat.mamda.*;

/**
 * MamdaOptionSeriesUpdate is an interface that provides access to
 * fields related to option series update events.  Update events
 * include adds/removes of contracts to the chain.
 */

public interface MamdaOptionSeriesUpdate extends MamdaBasicEvent
{
    public static final char ACTION_UNKNOWN = ' ';
    public static final char ACTION_ADD     = 'A';
    public static final char ACTION_DELETE  = 'D';

    /**
     * Get the option contract to which the most recent event applies.
     * @return The option contract to which the most recent event
     * applies.  If the contract is new, it will have already been
     * added to the chain.  If it is being removed, it will have
     * already been removed from the chain.
     */
    public MamdaOptionContract  getOptionContract();

    /**
     * Get the most recent action.
     * @return The action related to the last series update message.
     */
    public char  getOptionAction();
}
