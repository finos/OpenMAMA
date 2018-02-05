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

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * A class that represents the call and put contract sets at a given
 * strike price.
 */

public class MamdaOptionStrikeSet
{
    private final MamdaOptionContractSet mCallSet       = new MamdaOptionContractSet();
    private final MamdaOptionContractSet mPutSet        = new MamdaOptionContractSet();
    private final SimpleDateFormat       mExpireFormat  = new SimpleDateFormat ("MMMyy");
    private final Date                   mExpireDate;
    private final double                 mStrikePrice;

    public MamdaOptionStrikeSet (Date    expireDate,
                                 double  strikePrice)
    {
        mExpireDate  = expireDate;
        mStrikePrice = strikePrice;
    }

    /**
     * Return a set of call option contracts at a given strike price.
     * @return MamdaOptionContractSet Set of call contracts at the
     * given strike price.
     */
    public MamdaOptionContractSet getCallSet()
    {
        return mCallSet;
    }

    /**
     * Return a set of put option contracts at a given strike price.
     * @return MamdaOptionContractSet Set of put contracts at the
     * given strike price.
     */
    public MamdaOptionContractSet getPutSet()
    {
        return mPutSet;
    }

    /**
     * Return the expiration date for the contracts at the given strike
     * price.
     * @return Date The expiration date.
     */
    public Date getExpireDate ()
    {
        return mExpireDate;
    }

    /**
     * Return the expiration date as a string for the contracts at the
     * given strike price.
     * @return String The expiration date.
     */
    public String getExpireDateStr ()
    {
        return mExpireFormat.format(mExpireDate);
    }

    /**
     * Return the strike price.
     * @return double The strike price.
     */
    public double getStrikePrice ()
    {
        return mStrikePrice;
    }
}
