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
 * This class will test the MamaPrice.setPrecision() and MamaPrice.toString()
 *   methods with various levels of MamaPricePrecision.
 */
public class MamaPriceDecimalPlaces
{
    // The message under test
    protected MamaPrice mPrice;

    protected Double b;

    protected Double delta = 0.000000001;

    String szOneDP       = "1.1";
    String szTwoDP       = "1.12";
    String szThreeDP     = "1.123";
    String szFourDP      = "1.1235";
    String szFiveDP      = "1.12346";
    String szSixDP       = "1.123457";
    String szSevenDP     = "1.1234568";
    String szEightDP     = "1.12345679";
    String szNineDP      = "1.123456789";
    String szTenDP       = "1.1234567890";
    String szElevenDP    = "1.12345678901";
    String szTwelveDP    = "1.123456789012";
    String szThirteenDP  = "1.1234567890123";
    String szFourteenDP  = "1.12345678901235";
    String szFifteenDP   = "1.123456789012346";
    String szSixteenDP   = "1.1234567890123457";


    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
         b = 1.12345678901234567890;

         mPrice = new MamaPrice();
         mPrice.setValue(b);
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
    public void testImplicit()
    {
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_10);
       	assertEquals(szOneDP,   mPrice.toString());
       	mPrice.setPrecision(MamaPricePrecision.PRECISION_100);
    	assertEquals(szTwoDP,  mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_1000);
    	assertEquals(szThreeDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_10000);
    	assertEquals(szFourDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_100000);
    	assertEquals(szFiveDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_1000000);
    	assertEquals(szSixDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_10000000);
    	assertEquals(szSevenDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_100000000);
    	assertEquals(szEightDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_1000000000);
    	assertEquals(szNineDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_10000000000);
    	assertEquals(szTenDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_100000000000);
    	assertEquals(szElevenDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_1000000000000);
    	assertEquals(szTwelveDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_10000000000000);
    	assertEquals(szThirteenDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_100000000000000);
    	assertEquals(szFourteenDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_1000000000000000);
    	assertEquals(szFifteenDP,   mPrice.toString());
    	mPrice.setPrecision(MamaPricePrecision.PRECISION_10000000000000000);
    	assertEquals(szSixteenDP,   mPrice.toString());
    }
}
