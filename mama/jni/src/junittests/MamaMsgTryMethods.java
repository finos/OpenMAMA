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

import junit.framework.*;
import com.wombat.mama.*;

/**
 *
 * This class will test the MamaMsg.try*() methods.
 */
public class MamaMsgTryMethods extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    // The message under test
    protected MamaMsg mMsg;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        // Create the mama message
        mMsg = new MamaMsg();

        // Data for the fields
        int[] i32Array = new int[] { 1, 2 };
        String[] stringArray = new String[] { "1", "2" };
        MamaMsg subMessage = new MamaMsg();
        subMessage.addI32(null, 1, 752);

        // Add some fields
        mMsg.addBool(null, 1, true);
        mMsg.addChar(null, 2, 'G');
        mMsg.addI8(null, 3, (byte)16);
        mMsg.addI16(null, 4, (short)144);
        mMsg.addI32(null, 5, 69);
        mMsg.addI64(null, 6, (long)23847329);
        mMsg.addU8(null, 7, (byte)16);
        mMsg.addU16(null, 8, (short)144);
        mMsg.addU32(null, 9, 69);
        mMsg.addU64(null, 10, (long)23847329);
        mMsg.addF32(null, 11, (float)6.4);
        mMsg.addF64(null, 12, 196.4);
        mMsg.addString(null, 13, "This is a test");
        mMsg.addArrayI32(null, 14, i32Array);
        mMsg.addArrayU16(null, 15, i32Array);
        mMsg.addArrayString(null, 16, stringArray);
        mMsg.addMsg(null, 17, subMessage);
    }

    @Override
    protected void tearDown()
    {
        // Reset all member variables
        mMsg = null;
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    public void testBool()
    {
        // Extract the field
        MamaBoolean result = new MamaBoolean();        
        Assert.assertTrue(mMsg.tryBoolean(null, 1, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryBoolean(null, 101, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), true);
    }

    public void testChar()
    {
        // Extract the field
        MamaChar result = new MamaChar();
        Assert.assertTrue(mMsg.tryChar(null, 2, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryChar(null, 102, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), 'G');
    }

    public void testI8()
    {
        // Extract the field
        MamaByte result = new MamaByte();
        Assert.assertTrue(mMsg.tryI8(null, 3, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryI8(null, 103, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), 16);
    }

    public void testI16()
    {
        // Extract the field
        MamaShort result = new MamaShort();
        Assert.assertTrue(mMsg.tryI16(null, 4, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryI16(null, 104, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), 144);
    }

    public void testI32()
    {
        // Extract the field
        MamaInteger result = new MamaInteger();
        Assert.assertTrue(mMsg.tryI32(null, 5, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryI32(null, 105, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), 69);
    }

    public void testI64()
    {
        // Extract the field
        MamaLong result = new MamaLong();
        Assert.assertTrue(mMsg.tryI64(null, 6, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryI64(null, 106, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), 23847329);
    }

    public void testU8()
    {
        // Extract the field
        MamaShort result = new MamaShort();
        Assert.assertTrue(mMsg.tryU8(null, 7, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryU8(null, 107, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), 16);
    }

    public void testU16()
    {
        // Extract the field
        MamaInteger result = new MamaInteger();
        Assert.assertTrue(mMsg.tryU16(null, 8, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryU16(null, 108, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), 144);
    }

    public void testU32()
    {
        // Extract the field
        MamaLong result = new MamaLong();
        Assert.assertTrue(mMsg.tryU32(null, 9, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryU32(null, 109, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), 69);
    }

    public void testU64()
    {
        // Extract the field
        MamaLong result = new MamaLong();
        Assert.assertTrue(mMsg.tryU64(null, 10, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryU64(null, 110, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), 23847329);
    }

    public void testF32()
    {
        // Extract the field
        MamaFloat result = new MamaFloat();
        Assert.assertTrue(mMsg.tryF32(null, 11, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryF32(null, 111, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), (float)6.4);
    }

    public void testF64()
    {
        // Extract the field
        MamaDouble result = new MamaDouble();
        Assert.assertTrue(mMsg.tryF64(null, 12, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryF64(null, 112, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), 196.4);
    }

    public void testString()
    {
        // Extract the field
        MamaString result = new MamaString();
        Assert.assertTrue(mMsg.tryString(null, 13, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryString(null, 113, result));

        // Verify that the result is valid
        Assert.assertEquals(result.getValue(), "This is a test");
    }

    public void testI32Array()
    {
        // Extract the field
        MamaArrayInt result = new MamaArrayInt();
        Assert.assertTrue(mMsg.tryArrayI32(null, 14, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryArrayI32(null, 114, result));

        // Verify that the result is valid
        int[] arrayValues = result.getValue();
        Assert.assertEquals(arrayValues[0], 1);
        Assert.assertEquals(arrayValues[1], 2);        
    }

    public void testU16Array()
    {
        // Extract the field
        MamaArrayInt result = new MamaArrayInt();
        Assert.assertTrue(mMsg.tryArrayU16(null, 15, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryArrayU16(null, 115, result));

        // Verify that the result is valid
        int[] arrayValues = result.getValue();
        Assert.assertEquals(arrayValues[0], 1);
        Assert.assertEquals(arrayValues[1], 2);
    }

    public void testStringArray()
    {
        // Extract the field
        MamaArrayString result = new MamaArrayString();
        Assert.assertTrue(mMsg.tryArrayString(null, 16, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryArrayString(null, 116, result));

        // Verify that the result is valid
        String[] arrayValues = result.getValue();
        Assert.assertEquals(arrayValues[0], "1");
        Assert.assertEquals(arrayValues[1], "2");
    }

    public void testMsg()
    {
        // Extract the field
        MamaMessage result = new MamaMessage();
        Assert.assertTrue(mMsg.tryMsg(null, 17, result));

        // Check for a missing field
        Assert.assertFalse(mMsg.tryMsg(null, 117, result));

        // Verify that the result is valid
        MamaMsg subMessage = result.getValue();
        int subMessageResult = subMessage.getI32(null, 1, 66);
        Assert.assertEquals(subMessageResult, 752);
    }
}
