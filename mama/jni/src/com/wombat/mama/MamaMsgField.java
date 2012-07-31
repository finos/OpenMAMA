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

/**
 * Wrapper class for the underlying mamaMsgField C structure
 */
public class MamaMsgField
{
    static
    {
        initIDs();
    }

    /*Contains the pointer value of the underlying C mamaMsgField structure*/
    private long msgFieldPointer_i = 0;

    /* Reuseable objects */
    private MamaDateTime myDateTime               = null;
    private MamaPrice    myPrice                  = null;
    private MamaMsg      myMsg                    = null;
    private MamaFieldDescriptor myFieldDescriptor = null;
    
    /* Pointer to the underlying C structure of the reuseable objects */
    private long dateTimePointer_i   = 0;
    private long pricePointer_i      = 0;
    private long descriptorPointer_i = 0;
     
    /* Pointer to an array of reuseable JNI Msg Objects for extracting
       vector messages */        
    private long  jMsgArray_i     = 0;
    private int   jMsgArraySize_i = 0;
    
    public long getPointerVal()
    {
        return msgFieldPointer_i;
    }
   
    protected void setPointerVal(long pointerVal)
    {
        msgFieldPointer_i = pointerVal;
    } 

    public native int getFid();

    public native String getName();
    
    public native short getType();

    public native String getTypeName();

    public native boolean getBoolean();

    public native byte getI8();

    public native short getU8();

    public native char getChar();

    public native short getI16();

    public native int getU16();

    public native int getI32();

    public native long getU32();

    public native long getI64();
    
    public native long getU64();

    public native float getF32();

    public native double getF64();

    public MamaDateTime getDateTime ()
    {
        if (myDateTime == null)
        {  
            myDateTime = new MamaDateTime();
            dateTimePointer_i = myDateTime.getPointerVal();
        }        
        _getDateTime ();
        return myDateTime;
    }
    
    public MamaPrice getPrice ()
    {
        if (myPrice == null)
        {  
            myPrice = new MamaPrice();
            pricePointer_i = myPrice.getPointerVal();
        }            
         
         _getPrice ();
        return myPrice;
    }
    
    public MamaMsg getMsg()
    {
        if (myMsg == null)
        {
            myMsg = new MamaMsg (false);
        }
        _getMsg();
        return myMsg;
    }


    public native MamaFieldDescriptor getDescriptor();
       
    public native String getString();

    public native byte[] getOpaque();

    public native byte[]       getArrayI8 ();
    public native short[]      getArrayU8 ();
    public native short[]      getArrayI16 ();
    public native int[]        getArrayU16 ();
    public native int[]        getArrayI32 ();
    public native long[]       getArrayU32 ();
    public native long[]       getArrayI64 ();
    public native long[]       getArrayU64 ();
    public native float[]      getArrayF32 ();
    public native double[]     getArrayF64 ();
    public native String[]     getArrayString ();
    public native MamaMsg[]    getArrayMsg();

    public native String toString();
    
    private native void _getDateTime ();
    
    private native void _getPrice    ();
    
    private native void _getMsg    ();

    private static native void initIDs();
    
    public native void destroy ();
    protected void finalize()
    {
        if (msgFieldPointer_i != 0)
        destroy();
    }
}
