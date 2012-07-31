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

package com.wombat.mamda.orderbook;

import com.wombat.mamda.orderbook.MamdaOrderBookBasicDeltaList;
import com.wombat.mamda.MamdaBasicEvent;

/**
 * MamdaOrderBookComplexDelta is a class that saves information about
 * a complex order book delta.  A complex delta involves multiple
 * entries and/or price levels.  For example, a modified order may
 * involve a price change that means moving an entry from one price
 * level to another.  A complex delta is made up of several simple
 * deltas, which can be iterated over by methods provided in the class.
 */

public abstract class MamdaOrderBookComplexDelta
    extends    MamdaOrderBookBasicDeltaList
    implements MamdaBasicEvent
{
}
