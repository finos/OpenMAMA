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
import org.junit.Ignore;
import org.junit.Before;
import org.junit.After;
import org.junit.Test;

/**
 *
 * This class will test MamaMsg vector fields.
 */
public class MamaMsgVectorFields extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */

    // The bridge
    MamaBridge mBridge;

    // Reusable MAMA Message
    MamaMsg mMessage;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        // Create the message
        mMessage = new MamaMsg();
    }

    @After
    public void tearDown()
    {
        super.tearDown();
    }

    /* ****************************************************** */
    /* Test Message Functions. */
    /* ****************************************************** */

    @Test
    public void testVectorF32()
    {
        // Add data to the message
        mMessage.addArrayF32("vf32", 1, new float[] {1,2,3,3});

        // Get the data from the message
        mMessage.getArrayF32("vf32", 1);
    }

    @Test
    public void testVectorF64()
    {
        // Add data to the message
        mMessage.addArrayF64("vf64", 1, new double[] {1,2,3,3});

        // Get the data from the message
        mMessage.getArrayF64("vf64", 1);
    }

    @Test
    public void testVectorI8()
    {
        // Add data to the message
        mMessage.addArrayI8("vi8", 1, new byte[] {1,2,3,3});

        // Get the data from the message
        mMessage.getArrayI8("vi8", 1);
    }

    @Test
    public void testVectorI16()
    {
        // Add data to the message
        mMessage.addArrayI16("vi16", 1, new short[] {1,2,3,3});

        // Get the data from the message
        mMessage.getArrayI16("vi16", 1);
    }

    @Test
    public void testVectorI32()
    {
        // Add data to the message
        mMessage.addArrayI32("vi32", 1, new int[] {1,2,3,3});

        // Get the data from the message
        mMessage.getArrayI32("vi32", 1);
    }

    @Test
    public void testVectorI64()
    {
        // Add data to the message
        mMessage.addArrayI64("vi64", 1, new long[] {1,2,3,3});

        // Get the data from the message
        mMessage.getArrayI64("vi64", 1);
    }

    @Test
    public void testVectorU8()
    {
        // Add data to the message
        mMessage.addArrayU8("vu8", 1, new short[] {1,2,3,3});

        // Get the data from the message
        mMessage.getArrayU8("vu8", 1);
    }

    @Test
    public void testVectorU16()
    {
        // Add data to the message
        mMessage.addArrayU16("vu16", 1, new int[] {1,2,3,3});

        // Get the data from the message
        mMessage.getArrayU16("vu16", 1);
    }

    @Test
    public void testVectorU32()
    {
        // Add data to the message
        mMessage.addArrayU32("vu32", 1, new long[] {1,2,3,3});

        // Get the data from the message
        mMessage.getArrayU32("vu32", 1);
    }

    @Test
    public void testVectorU64()
    {
        // Add data to the message
        mMessage.addArrayU64("vu64", 1, new long[] {1,2,3,3});

        // Get the data from the message
        mMessage.getArrayU64("vu64", 1);
    }

    @Test
    public void testVectorString()
    {
        // Add data to the message
        mMessage.addArrayString("vs", 1, new String[] {"1","2","3","3"});

        // Get the data from the message
        mMessage.getArrayString("vs", 1);
    }

    @Test
    @Ignore("Disabling vector message test now since it seems to trigger race condition")
    public void testVectorMessage()
    {

        // Add data to the message
        MamaMsg subMessage = new MamaMsg();
        subMessage.addI32("i32", 1, 1);
        MamaMsg[] subMessages = new MamaMsg[] { subMessage };
        mMessage.addArrayMsg("vm", 1, subMessages);

        // Get the data from the message
        MamaMsg[] am = mMessage.getArrayMsg("vm", 1);

        subMessage.destroy();

        // Let the GC clean it up
    }

    /* ****************************************************** */
    /* Test Field Functions. */
    /* ****************************************************** */

    @Test
    public void testFieldVectorF32()
    {
        // Add data to the message
        mMessage.addArrayF32("vf32", 1, new float[] {1,2,3,3});

        // Get the field
        MamaMsgField field = mMessage.getField("vf32", 1, null);

        // Get the data from the field
        field.getArrayF32();
    }

    @Test
    public void testFieldVectorF64()
    {
        // Add data to the message
        mMessage.addArrayF64("vf64", 1, new double[] {1,2,3,3});

        // Get the field
        MamaMsgField field = mMessage.getField("vf64", 1, null);

        // Get the data from the field
        field.getArrayF64();
    }

    @Test
    public void testFieldVectorI8()
    {
        // Add data to the message
        mMessage.addArrayI8("vi8", 1, new byte[] {1,2,3,3});

        // Get the field
        MamaMsgField field = mMessage.getField("vi8", 1, null);

        // Get the data from the field
        field.getArrayI8();
    }

    @Test
    public void testFieldVectorI16()
    {
        // Add data to the message
        mMessage.addArrayI16("vi16", 1, new short[] {1,2,3,3});

        // Get the field
        MamaMsgField field = mMessage.getField("vi16", 1, null);

        // Get the data from the field
        field.getArrayI16();
    }

    @Test
    public void testFieldVectorI32()
    {
        // Add data to the message
        mMessage.addArrayI32("vi32", 1, new int[] {1,2,3,3});

        // Get the field
        MamaMsgField field = mMessage.getField("vi32", 1, null);

        // Get the data from the field
        field.getArrayI32();
    }

    @Test
    public void testFieldVectorI64()
    {
        // Add data to the message
        mMessage.addArrayI64("vi64", 1, new long[] {1,2,3,3});

        // Get the field
        MamaMsgField field = mMessage.getField("vi64", 1, null);

        // Get the data from the field
        field.getArrayI64();
    }

    @Test
    public void testFieldVectorU8()
    {
        // Add data to the message
        mMessage.addArrayU8("vu8", 1, new short[] {1,2,3,3});

        // Get the field
        MamaMsgField field = mMessage.getField("vu8", 1, null);

        // Get the data from the field
        field.getArrayU8();
    }

    @Test
    public void testFieldVectorU16()
    {
        // Add data to the message
        mMessage.addArrayU16("vu16", 1, new int[] {1,2,3,3});

        // Get the field
        MamaMsgField field = mMessage.getField("vu16", 1, null);

        // Get the data from the field
        field.getArrayU16();
    }

    @Test
    public void testFieldVectorU32()
    {
        // Add data to the message
        mMessage.addArrayU32("vu32", 1, new long[] {1,2,3,3});

        // Get the field
        MamaMsgField field = mMessage.getField("vu32", 1, null);

        // Get the data from the field
        field.getArrayU32();
    }

    @Test
    public void testFieldVectorU64()
    {
        // Add data to the message
        mMessage.addArrayU64("vu64", 1, new long[] {1,2,3,3});

        // Get the field
        MamaMsgField field = mMessage.getField("vu64", 1, null);

        // Get the data from the field
        field.getArrayU64();
    }

    @Test
    public void testFieldVectorString()
    {
        // Add data to the message
        mMessage.addArrayString("vs", 1, new String[] {"1","2","3","3"});

        // Get the field
        MamaMsgField field = mMessage.getField("vs", 1, null);

        // Get the data from the field
        field.getArrayString();
    }

    @Test
    @Ignore("Disabling vector message test now since it seems to trigger race condition")
    public void testFieldVectorMessage()
    {
        // Add data to the message
        MamaMsg subMessage = new MamaMsg();
        subMessage.addI32("i32", 1, 1);
        MamaMsg[] subMessages = new MamaMsg[] { subMessage };
        mMessage.addArrayMsg("vm", 1, subMessages);

        // Get the field
        MamaMsgField field = mMessage.getField("vm", 1, null);

        // Get the data from the field
        field.getArrayMsg();

        subMessage.destroy();
    }
}
