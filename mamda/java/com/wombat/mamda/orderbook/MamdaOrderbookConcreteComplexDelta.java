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

import com.wombat.mama.MamaDateTime;
import com.wombat.mamda.orderbook.MamdaOrderBookComplexDelta;
import com.wombat.mamda.MamdaConcreteBasicEvent;

/**
 * MamdaOrderBookConcreteComplexDelta is a class that saves information
 * about a complex order book delta.  A complex delta involves multiple
 * entries and/or price levels.  For example, a modified order may
 * involve a price change that means moving an entry from one price
 * level to another.  A complex delta is made up of several simple
 * deltas, which can be iterated over by methods provided in the class.
 */

public class MamdaOrderbookConcreteComplexDelta
    extends MamdaOrderBookComplexDelta
{
    private final MamdaConcreteBasicEvent mMamdaConcreteBasicEvent =
        new MamdaConcreteBasicEvent ();

    public MamaDateTime getSrcTime ()
    {
        return mMamdaConcreteBasicEvent.getSrcTime ();
    }

    public short getSrcTimeFieldState ()
    {
        return mMamdaConcreteBasicEvent.getSrcTimeFieldState ();
    }

    public void setSrcTime (MamaDateTime srcTime)
    {
        mMamdaConcreteBasicEvent.setSrcTime (srcTime);
    }

    public MamaDateTime getActivityTime ()
    {
        return mMamdaConcreteBasicEvent.getActivityTime ();
    }

    public short getActivityTimeFieldState ()
    {
        return mMamdaConcreteBasicEvent.getActivityTimeFieldState ();
    }

    public void setActivityTime (MamaDateTime activityTime)
    {
        mMamdaConcreteBasicEvent.setActivityTime (activityTime);
    }

    public MamaDateTime getEventTime ()
    {
        return mMamdaConcreteBasicEvent.getEventTime ();
    }

    public short getEventTimeFieldState()
    {
        return mMamdaConcreteBasicEvent.getEventTimeFieldState ();
    }

    public void setEventTime (MamaDateTime eventTime)
    {
        mMamdaConcreteBasicEvent.setEventTime (eventTime);
    }

    public long getEventSeqNum ()
    {
        return mMamdaConcreteBasicEvent.getEventSeqNum ();
    }

    public short getEventSeqNumFieldState ()
    {
        return mMamdaConcreteBasicEvent.getEventSeqNumFieldState ();
    }

    public void setEventSeqNum (long eventSeqNum)
    {
        mMamdaConcreteBasicEvent.setEventSeqNum (eventSeqNum);
    }
}
