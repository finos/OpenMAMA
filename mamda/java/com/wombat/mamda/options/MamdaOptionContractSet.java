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

import java.util.Collection;
import java.util.Set;
import java.util.TreeMap;

/**
 * A class that represents a set of option contracts at a given strike
 * price.
 */

public class MamdaOptionContractSet
{
    private MamdaOptionContract  mBboContract       = null;
    private MamdaOptionContract  mWombatBboContract = null;
    private TreeMap              mExchangeContracts = null;

    public MamdaOptionContractSet()
    {
        mExchangeContracts = new TreeMap();
    }

    /**
     * Set the contract for the best bid and offer.
     */
    public void setBboContract (MamdaOptionContract  contract)
    {
        mBboContract = contract;
    }

    /**
     * Set the contract for the best bid and offer, as calculated
     * by NYSE Technologies.
     */
    public void setWombatBboContract (MamdaOptionContract  contract)
    {
        mWombatBboContract = contract;
    }

    /**
     * Set the contract for the particular exchange.
     */
    public void setExchangeContract (String               exchange,
                                     MamdaOptionContract  contract)
    {
        mExchangeContracts.put (exchange, contract);
    }

    /**
     * Return the contract for the best bid and offer.
     */
    public MamdaOptionContract getBboContract ()
    {
        return mBboContract;
    }

    /**
     * Return the contract for the best bid and offer, as calculated
     * by NYSE Technologies.
     */
    public MamdaOptionContract getWombatBboContract ()
    {
        return mWombatBboContract;
    }

    /**
     * Return the contract for the particular exchange.
     */
    public MamdaOptionContract getExchangeContract (String exchange)
    {
        return (MamdaOptionContract) mExchangeContracts.get (exchange);
    }

    /**
     * Return the set of individual regional exchange option
     * contracts.  The type of object in the set is a instance of
     * MamdaOptionContract.
     */
    public Collection getExchangeContracts()
    {
        return mExchangeContracts.values();
    }

    /**
     * Return the set of individual regional exchange identifiers.
     * The type of object in the set is a instance of String.
     */
    public Set getExchanges()
    {
        return mExchangeContracts.keySet();
    }

}
