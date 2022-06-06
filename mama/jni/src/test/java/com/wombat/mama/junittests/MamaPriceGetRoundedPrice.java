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

import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;


import static org.junit.Assert.assertEquals;

/**
 *
 * This class will test the MamaPrice.myRoundedPrice() method.
 */
public class MamaPriceGetRoundedPrice extends MamaTestBaseTestCase
{
    // The message under test
    protected MamaPrice mPrice;

    protected Double b;

    protected Double delta = 0.000000001;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

         b = 28.061123212123563452;

         mPrice = new MamaPrice();
         mPrice.setValue(b);
    }

    @After
    public void tearDown()
    {
        // Clear all variables
    	mPrice = null;
        super.tearDown();
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    @Test
    public void testNullArguments()
    {
        // No preision specifed and none in price
    	// should retun orginal value
    	b =28.123456789123456789;

        mPrice = new MamaPrice();
        mPrice.setValue(b);

         assertEquals(mPrice.getValue(), mPrice.getRoundedValue(), delta);

         assertEquals(mPrice.getValue(), mPrice.getRoundedValue(MamaPricePrecision.PRECISION_UNKNOWN), delta);

         assertEquals(mPrice.getValue(), mPrice.getRoundedValue(MamaPricePrecision.PRECISION_INT), delta);

         assertEquals(mPrice.getValue(), mPrice.getRoundedValue(MamaPricePrecision.PRECISION_HALF_64), delta);
    }

    @Test
    public void testExplicit()
    {
    	assertEquals(28.1,   mPrice.getRoundedValue(MamaPricePrecision.PRECISION_10), delta);
    	assertEquals(28.06,  mPrice.getRoundedValue(MamaPricePrecision.PRECISION_100), delta);
    	assertEquals(28.061,  mPrice.getRoundedValue(MamaPricePrecision.PRECISION_1000), delta);
    	assertEquals(28.0611,   mPrice.getRoundedValue(MamaPricePrecision.PRECISION_10000), delta);
    	assertEquals(28.06112,  mPrice.getRoundedValue(MamaPricePrecision.PRECISION_100000), delta);
    	assertEquals(28.061123,   mPrice.getRoundedValue(MamaPricePrecision.PRECISION_1000000), delta);
    	assertEquals(28.0611232,   mPrice.getRoundedValue(MamaPricePrecision.PRECISION_10000000), delta);
    	assertEquals(28.06112321,   mPrice.getRoundedValue(MamaPricePrecision.PRECISION_100000000), delta);
    	assertEquals(28.061123212,   mPrice.getRoundedValue(MamaPricePrecision.PRECISION_1000000000), delta);

    }

    @Test
    public void testImplicit()
    {
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_10);
       	assertEquals(28.1,   mPrice.getRoundedValue(), delta);
       	mPrice.setPrecision(MamaPricePrecision.PRECISION_100);
    	assertEquals(28.06,  mPrice.getRoundedValue(), delta);
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_1000);
    	assertEquals(28.061,   mPrice.getRoundedValue(), delta);
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_10000);
    	assertEquals(28.0611,   mPrice.getRoundedValue(), delta);
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_100000);
    	assertEquals(28.06112,   mPrice.getRoundedValue(), delta);
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_1000000);
    	assertEquals(28.061123,   mPrice.getRoundedValue(), delta);
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_10000000);
    	assertEquals(28.0611232,   mPrice.getRoundedValue(), delta);
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_100000000);
    	assertEquals(28.06112321,   mPrice.getRoundedValue(), delta);
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_1000000000);
    	assertEquals(28.061123212,   mPrice.getRoundedValue(), delta);
    }
}
