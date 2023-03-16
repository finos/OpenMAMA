/* $Id$
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

package com.wombat.mama.junittests;

import com.wombat.common.WombatException;
import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

/**
 *
 * This class will mainly test the MamaPrice.setFromString() method
 */
public class MamaPriceGeneral
{
    protected MamaPrice mPrice;

    protected Double doubleValue;

    protected Double delta = 0.000000001;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
         doubleValue = 1.12345678901234567890;

         mPrice = new MamaPrice();
    }

    @After
    public void tearDown()
    {
        // Clear all variables
    	mPrice = null;
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    @Test
    public void testSetFromString()
    {
        String strValue  = "1.12345678901234567890";

    	mPrice.setFromString(strValue);
    	assertEquals(doubleValue,   mPrice.getValue(), delta);
    }

    @Test
    public void testSetFromStringMixedValue()
    {
        String strValue  = "1.12345678901MY PRICE";
        double mDouble = 1.12345678901;

    	mPrice.setFromString(strValue);
    	assertEquals(mDouble,   mPrice.getValue(), delta);
    }

    @Test(expected = WombatException.class)
    public void testSetFromStringInvalidValue() {
        mPrice.setFromString("MY PRICE");
    }
}
