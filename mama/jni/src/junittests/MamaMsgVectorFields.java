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

import junit.framework.TestCase;
import com.wombat.mama.*;

/**
 *
 * This class will test MamaMsg vector fields.
 */
public class MamaMsgVectorFields extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */

    // The bridge
    MamaBridge mBridge;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
         // Load the wmw bridge
        mBridge = Mama.loadBridge("wmw");
        Mama.open();

    }

    @Override
    protected void tearDown()
    {
        // Close mama
        Mama.close();
    }

    /* ****************************************************** */
    /* Test Message Functions. */
    /* ****************************************************** */

    public void testVectorF32()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayF32("vf32", 1, new float[] {1,2,3,3});

        // Get the data from the message
        message.getArrayF32("vf32", 1);

        // Destroy the message
        message.destroy();
    }

    public void testVectorF64()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayF64("vf64", 1, new double[] {1,2,3,3});

        // Get the data from the message
        message.getArrayF64("vf64", 1);

        // Destroy the message
        message.destroy();
    }

    public void testVectorI8()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayI8("vi8", 1, new byte[] {1,2,3,3});

        // Get the data from the message
        message.getArrayI8("vi8", 1);

        // Destroy the message
        message.destroy();        
    }

    public void testVectorI16()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayI16("vi16", 1, new short[] {1,2,3,3});

        // Get the data from the message
        message.getArrayI16("vi16", 1);

        // Destroy the message
        message.destroy();
    }

    public void testVectorI32()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayI32("vi32", 1, new int[] {1,2,3,3});

        // Get the data from the message
        message.getArrayI32("vi32", 1);

        // Destroy the message
        message.destroy();
    }

    public void testVectorI64()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayI64("vi64", 1, new long[] {1,2,3,3});

        // Get the data from the message
        message.getArrayI64("vi64", 1);

        // Destroy the message
        message.destroy();
    }

    public void testVectorU8()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayU8("vu8", 1, new short[] {1,2,3,3});

        // Get the data from the message
        message.getArrayU8("vu8", 1);

        // Destroy the message
        message.destroy();
    }

    public void testVectorU16()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayU16("vu16", 1, new int[] {1,2,3,3});

        // Get the data from the message
        message.getArrayU16("vu16", 1);

        // Destroy the message
        message.destroy();
    }

    public void testVectorU32()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayU32("vu32", 1, new long[] {1,2,3,3});

        // Get the data from the message
        message.getArrayU32("vu32", 1);

        // Destroy the message
        message.destroy();
    }

    public void testVectorU64()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayU64("vu64", 1, new long[] {1,2,3,3});

        // Get the data from the message
        message.getArrayU64("vu64", 1);

        // Destroy the message
        message.destroy();
    }

    public void testVectorString()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayString("vs", 1, new String[] {"1","2","3","3"});

        // Get the data from the message
        message.getArrayString("vs", 1);

        // Destroy the message
        message.destroy();
    }

    public void testVectorMessage()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        MamaMsg subMessage = new MamaMsg();
        subMessage.addI32("i32", 1, 1);
        MamaMsg[] subMessages = new MamaMsg[] { subMessage };
        message.addArrayMsg("vm", 1, subMessages);

        // Get the data from the message
        message.getArrayMsg("vm", 1);

        // Destroy the message
        message.destroy();
    }

    /* ****************************************************** */
    /* Test Field Functions. */
    /* ****************************************************** */

    public void testFieldVectorF32()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayF32("vf32", 1, new float[] {1,2,3,3});

        // Get the field
        MamaMsgField field = message.getField("vf32", 1, null);

        // Get the data from the field
        field.getArrayF32();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorF64()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayF64("vf64", 1, new double[] {1,2,3,3});

        // Get the field
        MamaMsgField field = message.getField("vf64", 1, null);

        // Get the data from the field
        field.getArrayF64();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorI8()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayI8("vi8", 1, new byte[] {1,2,3,3});

        // Get the field
        MamaMsgField field = message.getField("vi8", 1, null);

        // Get the data from the field
        field.getArrayI8();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorI16()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayI16("vi16", 1, new short[] {1,2,3,3});

        // Get the field
        MamaMsgField field = message.getField("vi16", 1, null);

        // Get the data from the field
        field.getArrayI16();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorI32()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayI32("vi32", 1, new int[] {1,2,3,3});

        // Get the field
        MamaMsgField field = message.getField("vi32", 1, null);

        // Get the data from the field
        field.getArrayI32();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorI64()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayI64("vi64", 1, new long[] {1,2,3,3});

        // Get the field
        MamaMsgField field = message.getField("vi64", 1, null);

        // Get the data from the field
        field.getArrayI64();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorU8()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayU8("vu8", 1, new short[] {1,2,3,3});

        // Get the field
        MamaMsgField field = message.getField("vu8", 1, null);

        // Get the data from the field
        field.getArrayU8();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorU16()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayU16("vu16", 1, new int[] {1,2,3,3});

        // Get the field
        MamaMsgField field = message.getField("vu16", 1, null);

        // Get the data from the field
        field.getArrayU16();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorU32()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayU32("vu32", 1, new long[] {1,2,3,3});

        // Get the field
        MamaMsgField field = message.getField("vu32", 1, null);

        // Get the data from the field
        field.getArrayU32();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorU64()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayU64("vu64", 1, new long[] {1,2,3,3});

        // Get the field
        MamaMsgField field = message.getField("vu64", 1, null);

        // Get the data from the field
        field.getArrayU64();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorString()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        message.addArrayString("vs", 1, new String[] {"1","2","3","3"});

        // Get the field
        MamaMsgField field = message.getField("vs", 1, null);

        // Get the data from the field
        field.getArrayString();

        // Destroy the message
        message.destroy();
    }

    public void testFieldVectorMessage()
    {
        // Create the message
        MamaMsg message = new MamaMsg();

        // Add data to the message
        MamaMsg subMessage = new MamaMsg();
        subMessage.addI32("i32", 1, 1);
        MamaMsg[] subMessages = new MamaMsg[] { subMessage };
        message.addArrayMsg("vm", 1, subMessages);

        // Get the field
        MamaMsgField field = message.getField("vm", 1, null);
        
        // Get the data from the field
        field.getArrayMsg();

        // Destroy the message
        message.destroy();
    }
}
