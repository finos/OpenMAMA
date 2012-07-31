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

import com.wombat.common.WombatException;
import java.util.Iterator;
import java.nio.*;

/*
* Wrapper class for the native C mama msg structure
*/
public class MamaMsg
{
    /* ************************************************** */
    /* Static Initialisation. */
    /* ************************************************** */
    static
    {
        initIDs();
    }

    /* ************************************************** */
    /* Private Member Variables. */
    /* ************************************************** */

    /* This array is used to read strings out of the native message
     * without performing any memory allocations. It will store
     * the string as a series of ASCII single byte characters.
     */
    private byte[] mByteArray;

    /* This array is used to read strings out of the native message
     * without performing any memory allocations. It will store
     * the string as a series of unicode 2 byte characters.
     */
    private char[] mCharArray;

    /* This buffer provides an object wrapper to the mByteArray and
     * will be returned to the client via the getStringAsByteBuffer
     * function.
     */
    private ByteBuffer mByteBuffer;

    /* This buffer provides an object wrapper to the mCharArray and
     * will be returned to the client via the getStringAsCharBuffer
     * function.
     */
    private CharBuffer mCharBuffer;

    /* A long value containing a pointer to the underlying C message structure*/
    private long  msgPointer_i = 0;

    /*
     We will reuse this MamaMsgField object for each invocation of the
     MamaMsgFieldIterator onField callback. This will negate the need
     for unnecessary object creation for each field when iterateFields()
     is called.
    */
    private MamaMsgField msgField_i = null;
    /* Reuseable objects */
    private MamaDateTime myDateTime = null; 
    private MamaPrice    myPrice    = null; 
    
    private MamaMsgIterator myIterator = null;
    
    /* Pointer to the underlying C structure of the reuseable objects */
    private long dateTimePointer_i = 0;
    private long pricePointer_i    = 0; 
    
    /* Pointer to an array of reuseable JNI Msg Objects for extracting
    vector messages */        
    private long  jMsgArray_i     = 0;
    private int   jMsgArraySize_i = 0;

    /* ************************************************** */
    /* Construction and Finalization. */
    /* ************************************************** */

    /**
     * The default construction behaviour is to create the underlying
     * C message structure. We need this when users are creating their
     * own messages when using the publishing API.
     */
    public MamaMsg()
    {
        this(true);
    }

    /**
     * Create a MamaMsg.
     *
     * @param payloadId The identifier of the payload to be used.
     */
    public MamaMsg (char payloadId)
    {    
        createForPayload (payloadId);

        allocateByteBuffers(1024);
    }

    /**
     * Create a MamaMsg.
     * 
     * @param MamaPayloadBridge 
     */
    public MamaMsg (MamaPayloadBridge payloadBridge)
    {
        createForPayloadBridge (payloadBridge);

        allocateByteBuffers(1024);
    }

    /*
    * The reason for this private constructor is so we can
    * create an instance of this class without actually creating 
    * the underlying C structure. This is required for subscription
    * callbacks where the message will already exist and we simply
    * set the pointer value correctly.
    */
    MamaMsg(boolean createCMessage)
    {
        if(true==createCMessage)
        {
            create ();
        }

        // Allocate the buffers for string types
        allocateByteBuffers(1024);
    }

    /* ************************************************** */
    /* Private Functions. */
    /* ************************************************** */

    /**
     * This function allocations the mByteArray and mCharArray arrays.
     * Note that the arrays will be wrapped by the corresponding
     * mByteBuffer and mCharBuffer managed objects.
     *
     * @param[in] length The length of the buffers to allocate.
     */
    private void allocateByteBuffers(int length)
    {
        // Allocate the byte and character buffers
        mByteArray = new byte[length];
        mCharArray = new char[length];

        // Wrap these with the managed objects
        mByteBuffer = ByteBuffer.wrap(mByteArray);
        mCharBuffer = CharBuffer.wrap(mCharArray);
    }

    /* ************************************************** */
    /* Public Functions. */
    /* ************************************************** */

    /**
     * This function will obtain a string from the mama message and return it
     * inside a ByteBuffer object without performing any memory allocation that
     * would cause future garbage collection.
     *
     * @param[in] fieldDesc The field descriptor.
     * @return A ByteBuffer containing single byte ASCII characters, to obtain
     *         a unicode string call getStringAsCharBuffer.
     * @exception  WombatException Thrown if the field descriptor is null.
     */
    public ByteBuffer getStringAsByteBuffer(MamaFieldDescriptor fieldDesc)
    {
        // Check for a null field descriptor
        if(null == fieldDesc)
        {
            throw new WombatException("getStringAsByteBuffer(): MamaFieldDescriptor was null.");
        }

        // Call the overload
        return getStringAsByteBuffer(fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
     * This function will obtain a string from the mama message and return it
     * inside a ByteBuffer object without performing any memory allocation that
     * would cause future garbage collection.
     *
     * @param[in] name The field name.
     * @param[in] fid The field Id.
     * @return A ByteBuffer containing single byte ASCII characters, to obtain
     *         a unicode string call getStringAsCharBuffer.
     */
    public ByteBuffer getStringAsByteBuffer(String name, int fid)
    {
        // First call the native function to populate the member array
        int length = nativeGetAsBuffer(name, fid, mByteArray, mByteArray.length);

        /* The function will return a negative value if the buffer wasn't big
         * enough, note that an exception will be thrown if something actually
         * went wrong.
         */
        if(length < 0)
        {
            // The length required is just this number times -1
            int lengthRequired = (length * -1);

            // Reallocate the member buffers to handle this number of characters
            allocateByteBuffers(lengthRequired);

            /* Call this function recursively to repeat the procedure now that
             * we have a large enough buffer.
             */
            getStringAsByteBuffer(name, fid);
        }

        else
        {
            // Set the limit of the buffer to the string length
            mByteBuffer.limit(length);
        }

        // Return the member byte bufer
        return mByteBuffer;
    }

    /**
     * This function will obtain a string from the mama message and return it
     * inside a CharBuffer object without performing any memory allocation that
     * would cause future garbage collection.
     *
     * @param[in] fieldDesc The field descriptor.
     * @return A CharBuffer containing unicode 2 byte characters, to obtain
     *         an ASCII representation call getStringAsByteBuffer.
     * @exception  WombatException Thrown if the field descriptor is null.
     */
    public CharBuffer getStringAsCharBuffer(MamaFieldDescriptor fieldDesc)
    {
        // Check for a null field descriptor
        if(null == fieldDesc)
        {
            throw new WombatException("getStringAsCharBuffer(): MamaFieldDescriptor was null.");
        }

        // Call the overload
        return getStringAsCharBuffer(fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
     * This function will obtain a string from the mama message and return it
     * inside a CharBuffer object without performing any memory allocation that
     * would cause future garbage collection.
     *
     * @param[in] name The field name.
     * @param[in] fid The field Id.
     * @return A CharBuffer containing unicode 2 byte characters, to obtain
     *         an ASCII representation call getStringAsByteBuffer.
     * @exception  WombatException Thrown if the field descriptor is null.
     */
    public CharBuffer getStringAsCharBuffer(String name, int fid)
    {
        // First call the native function to populate the member array
        int length = nativeGetAsBuffer(name, fid, mByteArray, mByteArray.length);

        /* The function will return a negative value if the buffer wasn't big
         * enough, note that an exception will be thrown if something actually
         * went wrong.
         */
        if(length < 0)
        {
            // The length required is just this number times -1
            int lengthRequired = (-length);

            // Reallocate the member buffers to handle this number of characters
            allocateByteBuffers(lengthRequired);

            /* Call this function recursively to repeat the procedure now that
             * we have a large enough buffer.
             */
            getStringAsCharBuffer(name, fid);
        }

        else
        {
            // Enumerate all the bytes
            for(int nextByte=0; nextByte<length; nextByte++)
            {
                /* Write the byte into the character array, note that each one
                 * must be casted to ensure the high byte of the unicode
                 * character is set to 0.
                 */
                mCharArray[nextByte] = (char)mByteArray[nextByte];
            }

            // Set the limit of the buffer to the string length
            mCharBuffer.limit(length);
        }

        // Return the member character bufer
        return mCharBuffer;
    }

        

    public long getPointerVal()
    {
        return msgPointer_i;
    }
    
    public Iterator iterator (MamaDictionary dictionary)
    {
              
        if (myIterator == null)
        {
            myIterator = new MamaMsgIterator(this, dictionary);
        }
        else
        {
            myIterator.reset (this, dictionary);
        }
        
        return myIterator;
    }
    
    public Iterator iterator ()
    {
        iterator (null);
        
        return myIterator;
    }
    
    /* Returns the total number of fields in the message. */
    public native int getNumFields();

    public native boolean  getBoolean (String name, int fid);
    
    public boolean getBoolean (MamaFieldDescriptor fieldDesc)
    {
        if (null!=fieldDesc)
        {
            return getBoolean (fieldDesc.getName(),fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getBoolean(): "
                    +"MamaFieldDescriptor was null.");
        }
    }

    /* Get a I8, U8 or I16 field as Java char.*/
    public native char getChar( String name, int fid );

    /* Get a I8, U8 or I16 field as Java char. */
    public char getChar( MamaFieldDescriptor fieldDesc )
    {
        final String METHOD_NAME = "getChar(): "; 
        if (null != fieldDesc) 
        {
            /*This can throw WombatException*/
            return getChar(fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException(METHOD_NAME+
                    "MamaFieldDescriptor was null.");
        }
    }

    public native byte getI8 (String name, int fid);
    
    public byte getI8 (MamaFieldDescriptor fieldDesc)
    {
        if (null!=fieldDesc)
        {
            return getI8 (fieldDesc.getName(),fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getI8(): "
                    +"MamaFieldDescriptor was null.");
        }
    }

    public native short getU8 (String name, int fid);
    
    public short getU8 (MamaFieldDescriptor fieldDesc)
    {
        if (null!=fieldDesc)
        {
            return getI8 (fieldDesc.getName(),fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getU8(): "
                    +"MamaFieldDescriptor was null.");
        }
    }



    /* Get a U8 or U16 field. */
    public native short getI16 (String name, int fid);

    /* Get a U8 or U16 field. */
    public short getI16 (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getI16 (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getI16():"
                    + " MamaFieldDescriptor was null.");
        }
    }

    public native int getU16 (String name, int fid);
    
    public int getU16 (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getU16 (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getU16():"
                    + " MamaFieldDescriptor was null.");
        }
    }

    /* Get a I32 field. */
    public native int getI32 (String name, int fid);

    /* Get a I32 field. */
    public int getI32 (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getI32 (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getI32():"
                    + " MamaFieldDescriptor was null.");
        }
    }


    public native long getU32 (String name, int fid);

    public long getU32 (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getU32 (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getU32():"
                    + " MamaFieldDescriptor was null.");
        }
    }


    /* Get a I64 field. */
    public native long getI64 (String name, int fid);

    /* Get a I64 field. */
    public long getI64 (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getI64 (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getI64(): MamaFieldDescriptor was null.");
        }
    }

    public native long getU64 (String name, int fid);

    public long getU64 (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getU64 (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getU64():"
                    + "MamaFieldDescriptor was null.");
        }
    }

    /* Get a float field. */
    public native float getF32 (String name, int fid);

    /* Get a float field. */
    public float getF32 (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getF32 (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getF32():"
                    + " MamaFieldDescriptor was null.");
        }
    }

    /* Get a double field. */
    public native double getF64 (String name, int fid);

    /* Get a double field. */
    public double getF64 (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getF64 (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getF64():"
                    + " MamaFieldDescriptor was null.");
        }
    }

    /* Get a String field. */
    public native String getString (String name, int fid);
    
    /* Get a String field. */
    public String getString (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getString (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getString():"
                    + " MamaFieldDescriptor was null.");
        }
    }

    public native MamaMsg getMsg (String name, int fid);

    public MamaMsg getMsg (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getMsg (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getMsg():"
                    + " MamaFieldDescriptor was null.");
        }
    }
	
	/**
	  * Return the internal message buffer as an array of bytes which is
	  * suitable for writing to a file.
	  * Note that this is a copy of the bytes in the current buffer.
	  *
	  * @return The byte array containing the message wire format
	  */
	public native byte[] getByteBuffer();
	
	/**
	  * Create a mamaMsg from the provided byte buffer.
	  * This function causes a memory buffer to be allocated that is persisted for
	  * the lifetime of the message. Therefore calling this function a second time
	  * will cause an error. Instead destroy the message and then re-created it
	  * using this function.
	  * @param byteArray The byte array containing the wire format of the message
	  */  
	public void createFromByteBuffer(byte[] byteArray)
	{
		if(byteArray == null)
		{
			throw new WombatException("createFromByteBuffer(): byteArray was null.");			
		}
		
		// Call the native function
		_createFromByteBuffer(byteArray);
	}
	
	private native void _createFromByteBuffer(byte[] byteArray);
	
	/**
	  * Set a new buffer for an existing mamaMsg. This approach is faster than
	  * creating a new message for buffers as the message can reuse memory
	  * allocated during previous use.
	  * @param byteArray The byte array containing the wire format of the message
	  */  
	public void setNewBuffer(byte[] byteArray)
	{
		if(byteArray == null)
		{
			throw new WombatException("setNewBuffer(): byteArray was null.");			
		}
		
		// Call the native function
		_setNewBuffer(byteArray);
	}
	
	private native void _setNewBuffer(byte[] byteArray);
   
    public native byte[] getOpaque (String name, int fid);

    public byte[] getOpaque (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getOpaque (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getOpaque():"
                    + " MamaFieldDescriptor was null.");
        }
    }
         
    public MamaDateTime getDateTime (String name, int fid)
    {
        if (myDateTime == null)
        {  
            myDateTime = new MamaDateTime();
            dateTimePointer_i = myDateTime.getPointerVal();
        }        
        _getDateTime (name, fid);
        return myDateTime;    
    } 

    public MamaDateTime getDateTime(MamaFieldDescriptor fieldDesc)
    {
        return getDateTime (fieldDesc.getName (), fieldDesc.getFid());
    }

    private native void _getDateTime (String name, int fid); 
    
    public MamaPrice getPrice (String name, int fid)
    {
        if (myPrice == null)
        {  
            myPrice = new MamaPrice();
            pricePointer_i = myPrice.getPointerVal();
        }            
        _getPrice (name, fid);
        return myPrice;
    }

    public MamaPrice getPrice( MamaFieldDescriptor fieldDesc )
    {
        return getPrice (fieldDesc.getName(), fieldDesc.getFid());
    }
    
    private native void _getPrice (String name, int fid);

    /* Get an array of submessage field. */
    public native MamaMsg[] getArrayMsg( String name, int fid );

    /* Get an array of submessage field. */
    public MamaMsg[] getArrayMsg( MamaFieldDescriptor fieldDesc )
    {
        if (null != fieldDesc)
        {
            return getArrayMsg (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getArrayMsg():"
                    + " MamaFieldDescriptor was null.");
        }
    }

    public native byte[] getArrayI8 (String name, int fid);


    public byte[] getArrayI8 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayI8 (fieldDesc.getName(), fieldDesc.getFid());
    }
    
    public native short[] getArrayU8 (String name, int fid);

    public short[] getArrayU8 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayU8 (fieldDesc.getName(), fieldDesc.getFid());
    }
    
    public native short[] getArrayI16 (String name, int fid);

    public short[] getArrayI16 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayI16 (fieldDesc.getName(), fieldDesc.getFid());
    }
    
    public native int[] getArrayU16 (String name, int fid);

    public int[] getArrayU16 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayU16 (fieldDesc.getName(), fieldDesc.getFid());
    }
    
    public native int[] getArrayI32 (String name, int fid);

    public int[] getArrayI32 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayI32 (fieldDesc.getName (), fieldDesc.getFid ());
    }
    
    public native long[] getArrayU32 (String name, int fid);

    public long[] getArrayU32 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayU32 (fieldDesc.getName(), fieldDesc.getFid());
    }

    public native long[] getArrayI64 (String name, int fid);

    public long[] getArrayI64 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayI64 (fieldDesc.getName(), fieldDesc.getFid());
    }
    
    public native long[] getArrayU64 (String name, int fid);

    public long[] getArrayU64 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayU64 (fieldDesc.getName (), fieldDesc.getFid ());
    }    

    public native float[] getArrayF32 (String name, int fid);

    public float[] getArrayF32 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayF32 (fieldDesc.getName(), fieldDesc.getFid());
    }
    
    public native double[] getArrayF64 (String name, int fid);

    public double[] getArrayF64 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayF64 (fieldDesc.getName(), fieldDesc.getFid());
    }
    
    /* Get an array of string field. */
    public native String[] getArrayString (String name, int fid);

    /* Get an array of string field. */
    public String[] getArrayString (MamaFieldDescriptor fieldDesc)
    {
        if (null != fieldDesc)
        {
            return getArrayString (fieldDesc.getName(), fieldDesc.getFid());
        }
        else
        {
            throw new WombatException("getArrayString():"
                    + " MamaFieldDescriptor was null.");
        }
    }

    public native boolean getIsDefinitelyDuplicate ();

    public native boolean getIsPossiblyDuplicate ();

    public native boolean getIsPossiblyDelayed ();

    public native boolean getIsDefinitelyDelayed ();

    public native boolean getIsOutOfSequence ();

    public native long getSeqNum();

    public void addBool (String name,int fid, boolean value)
    {
        /* addBoolean is the one we "should" be using anyway,
           so just call through to that to avoid having two 
           identical native functions. */
        addBoolean (name, fid, value);
    }

    public native void addBoolean (String name,int fid, boolean value);

    public native void addChar (String name, int fid, char value);

    public native void addI8 (String name, int fid, byte value);
        
    public native void addU8 (String name, int fid, short value);
    
    public native void addI16 (String name, int fid, short value);
    
    public native void addU16 (String name, int fid, int value);
    
    public native void addI32 (String name, int fid, int value);
       
    public native void addU32 (String name, int fid, long value);
    
    public native void addI64 (String name, int fid, long value);
    
    public native void addU64 (String name, int fid, long value);

    public native void addF32 (String name, int fid, float value);
    
    public native void addF64 (String name, int fid, double value);

    public native void addString (String name, int fid, String value);
    
    public native void addDateTime (String name, int fid, MamaDateTime value);
    
    public native void addPrice (String name, int fid, MamaPrice value);
    
    public native void addMsg (String name, int fid, MamaMsg value);

    public native void addOpaque (String name, int fid,byte[] value);
    
    public native void addArrayI8 (String name, int fid, byte[] value);
    
    public native void addArrayU8 (String name, int fid, short[] value);

    public native void addArrayI16 (String name, int fid, short[] value);
    
    public native void addArrayU16 (String name, int fid, int[] value);

    public native void addArrayI32 (String name, int fid, int[] value);

    public native void addArrayU32 (String name, int fid, long[] value);

    public native void addArrayI64 (String name, int fid, long[] value);
    
    public native void addArrayU64 (String name, int fid, long[] value);

    public native void addArrayF32 (String name, int fid, float[] value);
    
    public native void addArrayF64 (String name, int fid, double[] value);

    public native void addArrayMsg (String name, int fid, MamaMsg[] value); 

    /**
     * This function will add a new field containing an array of messages.
     * Unlinke addArrayMsg it will not add the entire array but only the
     * number of messages supplied in the length parameter.
     * @param name (in) the field name.
     * @param fid (in) the field id.
     * @param value (in) the array of messages.
     * @param length (in) the number of messages to add from the array, if this
     *                    parameter is outside the array bounds then an
     *                    exception will be thrown.
     * @exception com.wombat.mama.MamaException
     */
    public native void addArrayMsgWithLength (String name, int fid, MamaMsg[] value, int length);
    
    public native void addArrayString (String name, int fid, String[] value, int length);  
    
    public void addArrayString (String name, int fid, String[] value)    
    {
        addArrayString (name, fid, value, value.length);    
    }
    
    public native void updateI8 (String name, int fid, byte value);
    
    public native void updateU8( String name, int fid, short value );

    public native void updateI16 (String name, int fid, short value);

    public native void updateU16( String name, int fid, int value );
    
    public native void updateI32 (String name, int fid, int value);

    public native void updateU32( String name, int fid, long value ); 

    public native void updateI64 (String name, int fid, long value);
    
    public native void updateU64 (String name, int fid, long value);
    
    public native void updateF32 (String name, int fid, float value);

    public native void updateF64 (String name, int fid, double value);

    public native void updateString (String name, int fid, String value);
    
    public native void updatePrice (String name, int fid, MamaPrice value);

    public native void updateBoolean (String name, int fid, boolean value);

    public native void updateChar (String name, int fid, char value);

    public native void updateDateTime (String name, int fid, MamaDateTime value);

    public native void updateArrayI8 (String name, int fid, byte[] value);
    public native void updateArrayU8 (String name, int fid, short[] value);
    public native void updateArrayI16 (String name, int fid, short[] value);
    public native void updateArrayU16 (String name, int fid, int[] value);
    public native void updateArrayI32 (String name, int fid, int[] value);
    public native void updateArrayU32 (String name, int fid, long[] value);
    public native void updateArrayI64 (String name, int fid, long[] value);
    public native void updateArrayU64 (String name, int fid, long[] value);
    public native void updateArrayF32 (String name, int fid, float[] value);
    public native void updateArrayF64 (String name, int fid, double[] value);
    
    public char getChar (String  name,
                         int     fid,
                         char    defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getChar();
    }
    
    public char getChar (MamaFieldDescriptor fieldDesc, char defaultValue)
    {
        return getChar (fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }  
            
    public byte getI8 (String  name,
                        int     fid,
                        byte   defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getI8();
    }
    
    public byte getI8 (MamaFieldDescriptor fieldDesc, byte defaultValue)
    {
        return getI8 (fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }  
    
    public short getU8 (String  name,
                        int     fid,
                        short   defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getU8();
    }
                                                                                                                                
    public short getU8 (MamaFieldDescriptor fieldDesc,
                        short               defaultValue)
    {
        return getU8 (fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    
    public short getI16 (String  name,
                        int     fid,
                        short   defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getI16();
    }    
    public short getI16 (MamaFieldDescriptor fieldDesc, short defaultValue)
    {
        return getI16(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }  

    public int getU16 (String  name,
                        int     fid,
                        int   defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getU16();
    }        
    public int getU16 (MamaFieldDescriptor fieldDesc, int defaultValue)
    {
        return getU16(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }
    
    public int getI32 (String   name,
                       int      fid,
                       int      defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getI32();
    }
    
    public int getI32 (MamaFieldDescriptor fieldDesc, int defaultValue)
    {
        return getI32(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }  
    
    
    public long getU32 (String  name,
                        int     fid,
                        long    defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getU32();
    }
    public long getU32 (MamaFieldDescriptor fieldDesc, long defaultValue)
    {
        return getU32(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }
    
    public long getI64 (String  name,
                        int     fid,
                        long    defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getI64();
    }   
    public long getI64 (MamaFieldDescriptor fieldDesc, long defaultValue)
    {
        return getI64(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    public long getU64 (String  name,
                        int     fid,
                        long    defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getU64();
    }
    public long getU64 (MamaFieldDescriptor fieldDesc, long defaultValue)
    {
        return getU64(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }
    
    public float getF32 (String  name,
                         int     fid,
                         float   defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getF32();
    }    
    public float getF32 (MamaFieldDescriptor fieldDesc, float defaultValue)
    {
        return getF32(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }  
    
    public double getF64(String  name,
                         int     fid,
                         double  defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getF64();
    }        
    public double getF64 (MamaFieldDescriptor fieldDesc, double defaultValue)
    {
        return getF64(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    } 
    
    
    public String getString(String  name,
                            int     fid,
                            String  defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getString();
    }         
    public String getString (MamaFieldDescriptor fieldDesc, String defaultValue)
    {
        return getString(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }  
    
        
    public MamaPrice getPrice(String     name,
                              int        fid,
                              MamaPrice  defaultValue)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null)
        {
            return defaultValue;
        }
                                                                                                                                
        return field.getPrice();
    }         
    public MamaPrice getPrice (MamaFieldDescriptor fieldDesc, MamaPrice defaultValue)
    {
        return getPrice(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }  
    
    
    public MamaMsg getMsg (MamaFieldDescriptor fieldDesc, MamaMsg defaultValue)
    {
        if (null == fieldDesc)
        {
            return defaultValue;
        }
    
        MamaMsgField field = getField (fieldDesc.getName(), 
                                       fieldDesc.getFid(), null);
        if (field == null)
        {
            return defaultValue;
        }
        return field.getMsg();
    }
    
    public int[] getArrayI32 (MamaFieldDescriptor fieldDesc, int[] defaultValue)
    {
        if (null == fieldDesc)
        {
            return defaultValue;
        }
    
        MamaMsgField field = getField (fieldDesc.getName(), 
                                       fieldDesc.getFid(), null);
        if (field == null)
        {
            return defaultValue;
        }
        return field.getArrayI32();
    }  
    
    
    public MamaMsg[] getArrayMsg (MamaFieldDescriptor fieldDesc,
                                  MamaMsg[]           defaultValue)
    {
        if (null == fieldDesc)
        {
            return defaultValue;
        }
        
        MamaMsgField field = getField (fieldDesc.getName(), 
                                       fieldDesc.getFid(), null);
        if (field == null)
        {
            return defaultValue;
        
        } 
        return field.getArrayMsg();    
    }
    
	public native boolean tryBoolean(String name, int fid, MamaBoolean result);

    /* Try to get a MamaBoolean field. */
    public boolean tryBoolean( MamaFieldDescriptor field, MamaBoolean result )
    {
        if (null == field) return false;
        return tryBoolean (field.getName(), field.getFid(), result );
    }

    /* Try to get a MamaChar field. */
	public native boolean tryChar(String name, int fid, MamaChar result);


    /* Try to get a MamaChar field. */
    public boolean tryChar( MamaFieldDescriptor field, MamaChar result )
    {
        if (null == field) return false;
        return tryChar (field.getName(), field.getFid(), result );
    }

	public native boolean tryI8(String name, int fid, MamaByte result);

    public boolean  tryI8 (MamaFieldDescriptor field, MamaByte result)
    {
        if (null == field) return false;
        return tryI8 (field.getName(), field.getFid(), result );
    }
   
	public native boolean tryU8(String name, int fid, MamaShort result);


    public boolean tryU8 (MamaFieldDescriptor field, MamaShort result)
    {
        if (null == field) return false;
        return tryU8 (field.getName(), field.getFid(), result );
    }

    /* Try to get a MamaShort field. */
	public native boolean tryI16(String name, int fid, MamaShort result);


    /* Try to get a MamaShort field. */
    public boolean tryI16 (MamaFieldDescriptor field, MamaShort result)
    {
        if (null == field) return false;
        return tryI16 (field.getName(), field.getFid(), result );
    }

	public native boolean tryU16(String name, int fid, MamaInteger result);


    public boolean  tryU16 (MamaFieldDescriptor field, MamaInteger
                result)
    {
        if (null == field) return false;
        return tryU16 (field.getName(), field.getFid(), result );
    }
    

    /* Try to get a MamaInteger field. */
	public native boolean tryI32(String name, int fid, MamaInteger result);


    /* Try to get a MamaInteger field. */
    public boolean tryI32 (MamaFieldDescriptor field, MamaInteger result)
    {
        if (null == field) return false;
        return tryI32 (field.getName(), field.getFid(), result);
    }

	public native boolean tryU32(String name, int fid, MamaLong result);


    public boolean  tryU32 (MamaFieldDescriptor field, MamaLong result)
    {
        if (null == field) return false;
        return tryU32 (field.getName(), field.getFid(), result);
    }

    /* Try to get a MamaLong field. */
	public native boolean tryI64(String name, int fid, MamaLong result);


    /* Try to get a MamaLong field. */
    public boolean tryI64 (MamaFieldDescriptor field, MamaLong result)
    {
        if (null == field) return false;
        return tryI64 (field.getName(), field.getFid(), result);
    }

	public native boolean tryU64(String name, int fid, MamaLong result);


    public boolean  tryU64 (MamaFieldDescriptor field, MamaLong result)
    {
        if (null == field) return false;
        return tryU64 (field.getName(), field.getFid(), result);
    }

	public native boolean tryF32(String name, int fid, MamaFloat result);


    public boolean  tryF32 (MamaFieldDescriptor field, MamaFloat result)
    {
        if (null == field) return false;
        return tryF32 (field.getName(), field.getFid(), result);
    }

    /* Try to get a MamaDouble field  */
	public native boolean tryF64(String name, int fid, MamaDouble result);


    /* Try to get a MamaDouble field. */
    public boolean tryF64 (MamaFieldDescriptor field, MamaDouble result)
    {
        if (null == field) return false;
        return tryF64 (field.getName(), field.getFid(), result);
    }

    public boolean tryDateTime (MamaFieldDescriptor field,MamaDateTime result)
    {
        if (null == field) return false;
        return tryDateTime (field.getName(), field.getFid(),
                result);
    }

	public native boolean tryDateTime(String name, int fid, MamaDateTime result);

    public boolean tryPrice (MamaFieldDescriptor field, MamaPrice result)
    {
        if (null == field) return false;
        return tryPrice (field.getName (), field.getFid(), result);
    }

	public native boolean tryPrice(String name, int fid, MamaPrice result);

    /* Try to get a MamaString field. */
    public native boolean tryString( String name, int fid, MamaString result );

    /* Try to get a MamaString field. */
    public boolean tryString( MamaFieldDescriptor field, MamaString result )
    {
        if (null == field) return false;
        return tryString (field.getName(), field.getFid(), result );
    }

    public boolean tryOpaque (String name, int fid, MamaOpaque result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getOpaque());
        return true;
    }

    public boolean tryOpaque (MamaFieldDescriptor field, MamaOpaque
                result)
    {
        if (null == field) return false;
        return tryOpaque (field.getName(), field.getFid(), result );
    }

    public boolean tryMsg (String name, int fid, MamaMessage result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getMsg());
        return true;
    }

    public boolean tryMsg (MamaFieldDescriptor field, MamaMessage result)
    {
        if (null == field) return false;
        return tryMsg (field.getName(), field.getFid(), result );
    }

    public boolean tryArrayI32 (String name, int fid, MamaArrayInt result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getArrayI32());
        return true;
    }

    public boolean tryArrayI32 (MamaFieldDescriptor field, MamaArrayInt
                result)
    {
        if (null == field) return false;
        return tryArrayI32 (field.getName(), field.getFid(), result );
    }

    public boolean tryArrayU16 (String name, int fid, MamaArrayInt result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getArrayU16());
        return true;
    }

    public boolean tryArrayU16 (MamaFieldDescriptor field, MamaArrayInt
                result)
    {
        if (null == field) return false;
        return tryArrayU16 (field.getName(), field.getFid(), result );
    }

    
    public boolean tryArrayMsg (String name, int fid, MamaArrayMsg result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getArrayMsg());
        return true;
    }

    public boolean tryArrayMsg (MamaFieldDescriptor field, MamaArrayMsg
                result)
    {
        if (null == field) return false;
        return tryArrayMsg (field.getName(), field.getFid(), result );
    }
    
    public boolean tryArrayString (String name, int fid, MamaArrayString result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getArrayString());
        return true;
    }

    public boolean tryArrayString (MamaFieldDescriptor field, MamaArrayString result)
    {
        if (null == field) return false;
        return tryArrayString (field.getName(), field.getFid(), result);
    }

    /*Invokes the onField method on the iterator for each field in the message*/
    public void iterateFields(MamaMsgFieldIterator iterator,
                              MamaDictionary dictionary,
                              Object closure)

    {
        if (msgField_i == null)
        {
            msgField_i = new MamaMsgField();
        }
        _iterateFields (iterator, dictionary, closure);
    } 
    
    private native  void _iterateFields(MamaMsgFieldIterator iterator,
                                        MamaDictionary dictionary,
                                        Object closure);
                              
    /* Return a String representation the field with the given fid. */
    public native String getFieldAsString( int fid, MamaDictionary dictionary );

    /* Indicates whether this message is a p2p request */
    public native boolean isFromInbox();

    /* Return a String representation of the entire message. */
    public native String toString();

    /**
     * Return as a character representation of the payload type used in the message.
     *
     * @return A MamaPayloadType indicating which message payload is used in the
     *         message.
     */
    public native char getPayloadType();

    /*Needed for JNI so we know when to free the underlying C strucure
     Should only be called if the client app has created the message.
     i.e. do not call on messages passed in the callbacks as these are
     managed interal to the MAMA API*/
    public void destroy ()
    {
        if (myIterator != null)
        {
            myIterator.destroy();
        }
        
        _destroy();
    }

    private native void _destroy();

    public native void clear();

    public native void copy(MamaMsg rhs);

    public native MamaMsg detach ();
    
    public native void apply(MamaMsg rhs);
    
    /*Will create the underlying C Message and set the pointer value
     for the Java MamaMsg object*/
    private native void create ();

    private native void createForPayload (char payloadId);

    private native void createForPayloadBridge (MamaPayloadBridge payloadBridge);

    /*Used to cache ids for callback methods/fields*/
    private static native void initIDs();

    public MamaMsgField getField (MamaFieldDescriptor field, 
                                  MamaDictionary dict)
    {
        return getField (field.getName (), field.getFid(),dict);
    }

    public MamaMsgField getField (String fieldName, int fid,
                                  MamaDictionary dict)
    {
        
        /*calls the native method first. This will 
          reuse the reusable mamaMsgField in MamaMsg
          Only create the MsgField if we actually need to
        */
        if (msgField_i == null)
        {
            msgField_i = new MamaMsgField();
        }
         msgField_i.setPointerVal(0);
        _getField (fieldName,fid,dict);
        if (msgField_i.getPointerVal() == 0)
        {
            return null;
        } 
        return msgField_i;
    }

    public native void  _getField(String fieldName, int fid,
                                  MamaDictionary dict);

    protected void finalize ()
    {
        if (msgPointer_i != 0)
            destroy();
    }

    /* ************************************************** */
    /* Private Native Functions. */
    /* ************************************************** */

    /**
     * This function
     * @param[in] name The field name.
     * @param[in] fid The field Id.
     * @param[in] byteArray The byte array to populate.
     * @param[in] arraySize The size of the array.
     * @return The number of bytes copied to the buffer. If the buffer isn't
     *         big enough then the return will be the negative number of bytes
     *         required. Note that if anything goes wrong exceptions will be
     *         thrown.
     */
    private native int nativeGetAsBuffer(String name, int fid, byte[] byteArray, int arraySize);
}
