/* $Id:
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
 * Bit masks for hints.
 */
public class MamaDateTimeHints
{
    public static final short HAS_DATE    = 0x01;
    public static final short HAS_TIME    = 0x02;
    public static final short NO_TIMEZONE = 0x04;

    short myHints = 0;

    /**
     * Create hints with the specified mask.
     *
     * @param hints
     */
    public MamaDateTimeHints (short hints)
    {
        myHints = hints;
    }

    /**
     * Return the hints mask.
     * @return The mask.
     */
    public short getHints ()
    {
        return myHints;
    }

    /**
     * Return true if mask matches supplied hints
     */
    public boolean hasHints (MamaDateTimeHints hints)
    {
        return (hints.myHints & myHints) == hints.myHints;
    }

    /**
     * Return true if HAS_DATE bit set.
     */
    public boolean hasDate ()
    {
        return (myHints & HAS_DATE) == HAS_DATE;
    }

    /**
     * Return true if has time bit set.
     */
    public boolean hasTime ()
    {
        return (myHints & HAS_TIME) == HAS_TIME;
    }

    /**
     * Return true if no timezone bit set.
     */
    public boolean hasNoTimeZone ()
    {
        return (myHints & NO_TIMEZONE) == NO_TIMEZONE;
    }

    /**
     * Set mask
     */
    public void setHints (short hints)
    {
        myHints = hints;
    }

    /**
     * Set or unset the HAS_TIME bit.
     */
    public void setHasTime (boolean set)
    {
        if (set)
        {
            myHints |= HAS_TIME;
        }
        else
        {
            myHints &= ~HAS_TIME;
        }
    }

    /**
     * Set or unset the HAS_DATE bit.
     */
    public void setHasDate (boolean set)
    {
        if (set)
        {
            myHints |= HAS_DATE;
        }
        else
        {
            myHints &= ~HAS_DATE;
        }
    }

    /**
     * Set or unset the NO_TIMEZONE bit.
     */
    public void setNoTimezone (boolean set)
    {
        if (set)
        {
            myHints |= NO_TIMEZONE;
        }
        else
        {
            myHints &= ~NO_TIMEZONE;
        }
    }
}
