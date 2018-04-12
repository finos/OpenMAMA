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

import junit.framework.TestCase;
import com.wombat.mama.*;

/**
 *
 * This class will mainly test the MamaPrice.setFromString() method
 */
public class MamaPriceGeneral extends TestCase
{
    protected MamaPrice mPrice;

    protected Double doubleValue;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
         doubleValue = 1.12345678901234567890;

         mPrice = new MamaPrice();
    }

    @Override
    protected void tearDown()
    {
        // Clear all variables
    	mPrice = null;
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    public void testSetFromString()
    {
        String strValue  = "1.12345678901234567890";

    	mPrice.setFromString(strValue);
    	assertEquals(doubleValue,   mPrice.getValue());
    }

    public void testSetFromStringMixedValue()
    {
        String strValue  = "1.12345678901MY PRICE";
        double mDouble = 1.12345678901";

    	mPrice.setFromString(strValue);
    	assertEquals(mDouble,   mPrice.getValue());
    }

    public void testSetFromStringInvalidValue()
    {
        String strValue  = "MY PRICE";

        // This will throw an exception with "STATUS_INVALID_ARG"
        try
        {
    	    mPrice.setFromString(strValue);
        }
        catch ( MamaException me )
        {
            // Expected
        }
    }

    public void testSetIsInvalidPriceTestValid()
    {
        double mDouble = 1.123456789;

    	mPrice.setValue(mDouble);

    	mPrice.setIsInvalidPrice(false);

    	assertFalse(mPrice.getIsInvalidPrice());

    	assertEquals(mDouble, mPrice.getValue());
    }

    public void testSetIsInvalidPriceTestInValid()
    {
        double mDouble = 1.123456789;

    	mPrice.setValue(mDouble);

    	mPrice.setIsInvalidPrice(true);

    	assertTrue(mPrice.getIsInvalidPrice());
    }
}
