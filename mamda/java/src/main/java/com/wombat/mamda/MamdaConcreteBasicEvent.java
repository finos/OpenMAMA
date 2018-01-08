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

import com.wombat.mama.MamaDateTime;

/**
 * MamdaConcreteBasicEvent is intended to be used to help implement
 * concrete versions of various classes derived from MamdaBasicEvent.
 */
public class MamdaConcreteBasicEvent implements MamdaBasicEvent
{
    private MamaDateTime mSrcTime;
    private MamaDateTime mActivityTime;
    private MamaDateTime mEventTime;
    private long         mEventSeqNum;

    /**
     * @return Returns mSrcTime.
     */
    public MamaDateTime getSrcTime ()
    {
        return mSrcTime;
    }

    /**
     * @return Returns mSrcTime.
     */
    public short getSrcTimeFieldState ()
    {
        return MamdaFieldState.MODIFIED;
    }

    /**
     * @param mSrcTime The mSrcTime to set.
     */
    public void setSrcTime (MamaDateTime srcTime)
    {
        mSrcTime = srcTime;
    }

    /**
     * @return Returns mActivityTime.
     */
    public MamaDateTime getActivityTime ()
    {
        return mActivityTime;
    }

    /**
     * @return Returns mActivityTime Field State.
     */
    public short getActivityTimeFieldState ()
    {
        return MamdaFieldState.MODIFIED;
    }

    /**
     * @param mActivityTime The mActivityTime to set.
     */
    public void setActivityTime (MamaDateTime activityTime)
    {
        mActivityTime = activityTime;
    }

    /**
     * @return Returns mEventTime.
     */
    public MamaDateTime getEventTime ()
    {
        return mEventTime;
    }

    /**
     * @return Returns mEventTime Field State.
     */
    public short getEventTimeFieldState ()
    {
        return MamdaFieldState.MODIFIED;
    }

    /**
     * @param mEventTime The mEventTime to set.
     */
    public void setEventTime (MamaDateTime eventTime)
    {
        mEventTime = eventTime;
    }

    /**
     * @return Returns mEventSeqNum.
     */
    public long getEventSeqNum ()
    {
        return mEventSeqNum;
    }

    /**
     * @return Returns mEventSeqNum Field State.
     */
    public short getEventSeqNumFieldState ()
    {
        return MamdaFieldState.MODIFIED;
    }

    /**
     * @param mEventSeqNum The mEventSeqNum to set.
     */
    public void setEventSeqNum (long eventSeqNum)
    {
        mEventSeqNum = eventSeqNum;
    }
}
