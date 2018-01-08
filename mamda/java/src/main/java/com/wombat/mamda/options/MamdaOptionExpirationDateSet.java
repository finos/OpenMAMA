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
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;

/**
 * A class that represents a set of expiration dates, each of which
 * contains a set of strike prices (MamdaOptionStrikeSet), each of
 * which contains a set of option contracts (MamdaOptionContractSet),
 * each of which contains exchange-specific contracts.  To access a
 * set of strike prices for a given expiration date, use the get
 * method (inherited from TreeMap).
 */

public class MamdaOptionExpirationDateSet extends TreeMap
{
    private static SimpleDateFormat  mExpireDateFormat =
        new SimpleDateFormat("MMMyy");

    public MamdaOptionExpirationDateSet()
    {
    }

    public MamdaOptionExpirationDateSet (MamdaOptionExpirationDateSet  copy)
    {
        copyStrikes (copy);
    }

    protected MamdaOptionExpirationDateSet (SortedMap map)
    {
        copyStrikes (map);
    }

    public MamdaOptionExpirationDateSet getExpirationsBefore (Date date)
    {
        SortedMap result = super.headMap(date);
        return new MamdaOptionExpirationDateSet(result);
    }

    public MamdaOptionExpirationDateSet getExpirations (int numExpirations)
    {
        Iterator keyIter = keySet().iterator();
        Date date = null;
        while (keyIter.hasNext() && (numExpirations >= 0))
        {
            date = (Date) keyIter.next();
            --numExpirations;
        }
        if ((numExpirations == -1) && (date != null))
        {
            return getExpirationsBefore(date);
        }
        else
        {
            return new MamdaOptionExpirationDateSet(this);
        }
    }

    public String toString()
    {
        Set entries = entrySet();
        Iterator entryIter = entries.iterator();
        String result = "";
        while (entryIter.hasNext())
        {
            Map.Entry entry = (Map.Entry) entryIter.next();
            Date expireDate = (Date) entry.getKey();
            MamdaOptionExpirationStrikes strikes =
                (MamdaOptionExpirationStrikes) entry.getValue();
            result += "\n  " + mExpireDateFormat.format(expireDate);
            result += "=" + strikes;
        }
        return result;
    }

    private void copyStrikes (SortedMap  copy)
    {
        // Shallow copy is not good enough.  We want a copy that deep
        // copies the MamdaOptionExpirationStrikes, but not the
        // contents of those strike sets.
        Date today = new Date();
        Set entries = copy.entrySet();
        Iterator entryIter = entries.iterator();
        while (entryIter.hasNext())
        {
            Map.Entry entry = (Map.Entry) entryIter.next();
            Date expireDate = (Date) entry.getKey();
            MamdaOptionExpirationStrikes strikes =
                (MamdaOptionExpirationStrikes) entry.getValue();
            put (expireDate, new MamdaOptionExpirationStrikes(strikes));
        }
    }
}
