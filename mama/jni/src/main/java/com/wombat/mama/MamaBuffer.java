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

package com.wombat.mama;

import java.nio.*;

/**
 * Mama Assignable Buffer.  The default is a ByteBuffer but can also returned 
 *   as a CharBuffer.
 */

public class MamaBuffer
{
    private int arraySize = 1;

    private byte[] myByteArray;
    private char[] myCharArray;

    private ByteBuffer myByteBuffer;
    private CharBuffer myCharBuffer;

    public MamaBuffer()
    {
        myByteArray = new byte[arraySize];
        myCharArray = new char[arraySize];

        myByteBuffer = ByteBuffer.wrap(myByteArray);
        myCharBuffer = CharBuffer.wrap(myCharArray);
    }

    public void grow(int length)
    {
        myByteArray = new byte[length];
        myCharArray = new char[length];

        myByteBuffer = ByteBuffer.wrap(myByteArray);
        myCharBuffer = CharBuffer.wrap(myCharArray);
    }

    public byte[] array()
    {
        return myByteBuffer.array();
    }

    public int limit()
    {
        return myByteBuffer.limit();
    }

    public void limit(int length)
    {
        myByteBuffer.limit(length);
    }

    public CharBuffer asCharBuffer()
    {
        for (int nextByte=0; nextByte<myByteBuffer.limit(); nextByte++)
            myCharArray[nextByte] = (char)myByteArray[nextByte];

        myCharBuffer.limit(myByteBuffer.limit());

        return myCharBuffer;
    }

    public ByteBuffer asByteBuffer()
    {
        return myByteBuffer;
    }
}
