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

/**
 * Wrapper class for the native C mama msg structure.
 *
 * <p>
 * Field identifiers must be greater than or equal to 0. A field identifier
 * of 0 indicates that there is no unique FID and multiple fields with the
 * same name may have FID == 0.
 * </p>
 * <p>
 * Field lookup proceeds in the following manner.
 * <ol>
 * <li>
 * If the fid supplied is non-zero, search for a field with the
 * specified fid and return the field if it exists (the name is not validated).
 * Otherwise return null.
 * </li>
 * <li>
 * If the fid supplied is 0, return the first field encountered with the
 * supplied name or null if no such field exists.
 * </li>
 * </ol>
 * <p>
 * Get methods for numeric values may result in loss of information through
 * either rounding or truncation when a larger data type is accessed as a
 * smaller one. The result may be the same as the result of casting the larger
 * value to the smaller. For example calling <code>getShort</code> for an
 * integer field with a value greater than <code>Short.MAX_VALUE</code>
 * might return <code>Short.MIN_VALUE</code>. It is also valid to throw a
 * <code>ClassCastException</code> or other appropriate
 * <code>RuntimeException</code>.
 * </p>
 * Since some message implementations may not natively support all data types,
 * the behaviour may vary substantially. In creating and accessing messages the
 * Wombat APIs assume that the underlying values are stored in the smallest
 * possible fields, and accesses them accordingly. For this reason the minimal
 * requirement is that the methods for accessing and creating fields support
 * the full range of values appropriate for their type. How they deal with
 * larger values should be irrelevant.
 *
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

    /* This buffer provides an object wrapper to a MamaBuffer which
     * will be used for getStringAsBuffer and tryStringAsBuffer functions.
     */
    private MamaBuffer mamaBuffer;
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
    private long byteBufferPointer_i = 0;

    /* Pointer to an array of reuseable JNI Msg Objects for extracting
    vector messages */
    private long  jMsgArray_i     = 0;
    private int   jMsgArraySize_i = 0;

    /* ************************************************** */
    /* Construction and Finalization. */
    /* ************************************************** */

    /**
     * The default construction behaviour is to create the underlying
     * C message structure.
     * We need this when users are creating their own messages when using the publishing API.
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
    }

    /**
     * Create a MamaMsg.
     *
     * @param payloadBridge The MamaPayloadBridge to be used
     */
    public MamaMsg (MamaPayloadBridge payloadBridge)
    {
        createForPayloadBridge (payloadBridge);
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
    }

    /* ************************************************** */
    /* Private Functions. */
    /* ************************************************** */

    /* ************************************************** */
    /* Public Functions. */
    /* ************************************************** */

    /**
     * This function will obtain a string from the mama message and return it
     * inside a MamaBuffer object without performing any memory allocation that
     * would cause future garbage collection.
     *
     * @param fieldDesc The field descriptor.
     * @return A MamaBuffer containing single byte ASCII characters, to obtain
     *         a unicode string call MamaBuffer.asCharBuffer.
     * @exception  WombatException Thrown if the field descriptor is null.
     */
    public MamaBuffer getStringAsBuffer(MamaFieldDescriptor fieldDesc)
    {
        // Check for a null field descriptor
        if(null == fieldDesc)
        {
            throw new WombatException("getStringAsBuffer(): MamaFieldDescriptor was null.");
        }

        // Call the overload
        return getStringAsBuffer(fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
     * This function will obtain a string from the mama message and return it
     * inside a MamaBuffer object without performing any memory allocation that
     * would cause future garbage collection.
     *
     * @param name The field name.
     * @param fid The field Id.
     * @return A MamaBuffer containing single byte ASCII characters, to obtain
     *         a unicode string call MamaBuffer.asCharBuffer.
     */
    public MamaBuffer getStringAsBuffer(String name, int fid)
    {
        if (mamaBuffer==null)
            mamaBuffer = new MamaBuffer();

        // First call the native function to populate the member array
        int length = nativeGetAsBuffer(name, fid, mamaBuffer.array(), mamaBuffer.array().length, true);

        // The field was not found in the message
        if (length==Integer.MAX_VALUE)
            return null;

        /* The function will return a negative value if the buffer wasn't big
         * enough, note that an exception will be thrown if something actually
         * went wrong.
         */
        if(length < 0)
        {
            // The length required is just this number times -1
            int lengthRequired = (length * -1);

            // Reallocate the member MamaBuffer to handle this number of characters
            mamaBuffer.grow(mamaBuffer.array().length + lengthRequired);

            /* Call this function recursively to repeat the procedure now that
             * we have a large enough buffer.
             */
            getStringAsBuffer(name, fid);
        }
        else
        {
            // Set the limit of the buffer to the string length
            mamaBuffer.limit(length);
        }

        // Return the member MamaBuffer
        return mamaBuffer;
    }

     /**
     * This function will obtain a string from the mama message and return it
     * inside a MamaBuffer object without performing any memory allocation that
     * would cause future garbage collection. If the field is not in the message
     * the function will return FALSE.
     *
     * @param name The field name.
     * @param fid The field Id.
     * @param mamaBuffer A MamaBuffer that will contain the string if it is present
     *             in the message.
     * @return Boolean indicating of the field was present in the message or not.
     */
    public boolean tryStringAsBuffer(String name, int fid, MamaBuffer mamaBuffer)
    {
        // First call the native function to populate the member array
        int length = nativeGetAsBuffer(name, fid, mamaBuffer.array(), mamaBuffer.array().length, false);

        /* Exit with a FALSE if the length is equalled to INT_MAX as this will
        *    indicate that the field was not found from the native function above
        */
        if (length==Integer.MAX_VALUE)
            return false;

        if(length < 0)
        {
            // The length required is just this number times -1
            int lengthRequired = (length * -1);

            // Reallocate the member buffers to handle this number of characters
            mamaBuffer.grow(mamaBuffer.array().length + lengthRequired);

            /* Call this function recursively to repeat the procedure now that
             * we have a large enough buffer
             */
            tryStringAsBuffer(name, fid, mamaBuffer);
        }
        else
        {
            // Set the limit of the buffer to the string length
            mamaBuffer.limit(length);
        }

        return true;
    }

    /**
      * Return the native message pointer.
      *
      * @return The message pointer (Long)
      */
    public long getPointerVal()
    {
        return msgPointer_i;
    }

    /**
      * Return a iterator for use with this MamaMsg
      *
      * @param dictionary The dictionary to use for iterating over the message
      *
      * @return The message iterator (Iterator)
      */
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

    /**
      * Return an iterator for use with this MamaMsg
      *
      * @return A message iterator (Iterator)
      */
    public Iterator iterator ()
    {
        iterator (null);

        return myIterator;
    }

    /** Returns the total number of fields in the message.
      *
      * @return Number of fields in the message as in int
      */
    public native int getNumFields();

    /**
      * Returns a Boolean value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a boolean
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native boolean  getBoolean (String name, int fid);

    /**
      * Returns a Boolean value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a boolean
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns a Char value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a char
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native char getChar( String name, int fid );

    /**
      * Returns a Char value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a boolean
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns an I8 value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a byte
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native byte getI8 (String name, int fid);

    /**
      * Returns a Integer (I8) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a byte
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns a U8 value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a short
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native short getU8 (String name, int fid);

    /**
      * Returns a Unsigned Integer (U8) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a short
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns an I16 value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a short
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native short getI16 (String name, int fid);

    /**
      * Returns a Integer (U16) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a short
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns a U16 value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a int
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native int getU16 (String name, int fid);

    /**
      * Returns a Unsigned Integer (U16) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a int
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns an I32 value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a int
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native int getI32 (String name, int fid);

    /**
      * Returns a Integer (I32) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a int
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns a U32 value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a long
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native long getU32 (String name, int fid);

    /**
      * Returns a Unsigned Integer (U32) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a long
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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


    /**
      * Returns an I64 value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a long
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native long getI64 (String name, int fid);

    /**
      * Returns a Integer (I64) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a long
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns a U64 value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a long
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native long getU64 (String name, int fid);

    /**
      * Returns a Unsigned Integer (U64) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a long
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns an F32 value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a float
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native float getF32 (String name, int fid);

    /**
      * Returns a Float (F32) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a float
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns an F64 value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a double
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native double getF64 (String name, int fid);

    /**
      * Returns a Double (F64) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a double
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns a String value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a String
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native String getString (String name, int fid);

    /**
      * Returns a String value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a string
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns a MamaMsg value from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      *
      * @return The field value as a MamaMsg
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native MamaMsg getMsg (String name, int fid);

    /**
      * Returns a MamaMsg value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a MamaMsg
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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
      *
	  * @param byteArray The byte array containing the wire format of the message
      *
      * @exception WombatException will be thrown if the byteArray is null
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

    /**
      * Returns a Opaque value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a byte[]
      */
    public native byte[] getOpaque (String name, int fid);

    /**
      * Returns a Opaque (byte[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a byte[]
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns a MamaDateTime value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a MamaDateTime
      */
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

    /**
      * Returns a MamaDateTime value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a MamaDateTime
      */
    public MamaDateTime getDateTime(MamaFieldDescriptor fieldDesc)
    {
        return getDateTime (fieldDesc.getName (), fieldDesc.getFid());
    }

    private native void _getDateTime (String name, int fid);

    /**
      * Returns a MamaPrice value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a MamaPrice
      */
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

    /**
      * Returns a MamaPrice value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a MamaPrice
      */
    public MamaPrice getPrice( MamaFieldDescriptor fieldDesc )
    {
        return getPrice (fieldDesc.getName(), fieldDesc.getFid());
    }

    private native void _getPrice (String name, int fid);

    /**
      * Returns a Message Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a MamaMsg[]
      */
    public native MamaMsg[] getArrayMsg( String name, int fid );

    /**
      * Returns a MamaMsg Array (MamaMsg[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a MamaMsg[]
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns an I8 Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a byte[]
      */
    public native byte[] getArrayI8 (String name, int fid);

    /**
      * Returns an I8 Array (byte[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a byte[]
      */
    public byte[] getArrayI8 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayI8 (fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
      * Returns a U8 Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a short[]
      */
    public native short[] getArrayU8 (String name, int fid);

    /**
      * Returns a U8 Array (short[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a short[]
      */
    public short[] getArrayU8 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayU8 (fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
      * Returns an I16 Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a short[]
      */
    public native short[] getArrayI16 (String name, int fid);

    /**
      * Returns a I16 Array (short[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a short[]
      */
    public short[] getArrayI16 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayI16 (fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
      * Returns a U16 Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a int[]
      */
    public native int[] getArrayU16 (String name, int fid);

    /**
      * Returns a U16 Array (int[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a int[]
      */
    public int[] getArrayU16 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayU16 (fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
      * Returns an I32 Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a int[]
      */
    public native int[] getArrayI32 (String name, int fid);

    /**
      * Returns a I32 Array (int[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a int[]
      */
    public int[] getArrayI32 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayI32 (fieldDesc.getName (), fieldDesc.getFid ());
    }

    /**
      * Returns a U32 Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a long[]
      */
    public native long[] getArrayU32 (String name, int fid);

    /**
      * Returns a U32 Array (long[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a long[]
      */
    public long[] getArrayU32 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayU32 (fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
      * Returns an I64 Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a long[]
      */
    public native long[] getArrayI64 (String name, int fid);

    /**
      * Returns a I64 Array (long[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a long[]
      */
    public long[] getArrayI64 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayI64 (fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
      * Returns an U64 Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a long[]
      */
    public native long[] getArrayU64 (String name, int fid);

    /**
      * Returns a U64 Array (long[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a long[]
      */
    public long[] getArrayU64 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayU64 (fieldDesc.getName (), fieldDesc.getFid ());
    }

    /**
      * Returns an F32 Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a float[]
      */
    public native float[] getArrayF32 (String name, int fid);

    /**
      * Returns a F32 Array (float[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a float[]
      */
    public float[] getArrayF32 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayF32 (fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
      * Returns an F64 Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a double[]
      */
    public native double[] getArrayF64 (String name, int fid);

    /**
      * Returns a F32 Array (double[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a double[]
      */
    public double[] getArrayF64 (MamaFieldDescriptor fieldDesc)
    {
        return getArrayF64 (fieldDesc.getName(), fieldDesc.getFid());
    }

    /**
      * Returns an String Array value from the underlying message.
      *
      * @param name The name of the field
      * @param fid The FID of the field
      *
      * @return The field value as a string[]
      */
    public native String[] getArrayString (String name, int fid);

    /**
      * Returns a String Array (string[]) value from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor of the message field that you wich to retrieve
      *
      * @return The field value as a string[]
      *
      * @exception WombatException will be thrown if the fieldDesc is null
      */
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

    /**
      * Returns Boolean indiciating whether the underlying message is definitely duplicate.
      *
      * @return Boolean indiciating whether the message is a duplicate
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native boolean getIsDefinitelyDuplicate ();

    /**
      * Returns Boolean indiciating whether the underlying message is possibly duplicate.
      *
      * @return Boolean indiciating whether the message is a possibly a duplicate
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native boolean getIsPossiblyDuplicate ();

    /**
      * Returns Boolean indiciating whether the underlying message is possibly delayed.
      *
      * @return Boolean indiciating whether the message is a possibly delayed
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native boolean getIsPossiblyDelayed ();

    /**
      * Returns Boolean indiciating whether the underlying message is definitely delayed.
      *
      * @return Boolean indiciating whether the message is a definitely delayed
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native boolean getIsDefinitelyDelayed ();

    /**
      * Returns Boolean indiciating whether the underlying message is out of sequence.
      *
      * @return Boolean indiciating whether the message is out of sequence
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native boolean getIsOutOfSequence ();

    /**
      * Returns the Sequence Number from the underlying message.
      *
      * @return The field value as a long
      */
    public native long getSeqNum();

    /**
      * Add a Boolean value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public void addBool (String name,int fid, boolean value)
    {
        /* addBoolean is the one we "should" be using anyway,
           so just call through to that to avoid having two
           identical native functions. */
        addBoolean (name, fid, value);
    }

    /**
      * Add a Boolean value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addBoolean (String name,int fid, boolean value);

    /**
      * Add a Char value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addChar (String name, int fid, char value);

    /**
      * Add an I8 (byte) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addI8 (String name, int fid, byte value);

    /**
      * Add an U8 (byte) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addU8 (String name, int fid, short value);

    /**
      * Add an I16 (short) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addI16 (String name, int fid, short value);

    /**
      * Add an U16 (short) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addU16 (String name, int fid, int value);

    /**
      * Add an I32 (int) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addI32 (String name, int fid, int value);

    /**
      * Add an U32 (int) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addU32 (String name, int fid, long value);

    /**
      * Add an I64 (long) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addI64 (String name, int fid, long value);

    /**
      * Add an U64 (long) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addU64 (String name, int fid, long value);

    /**
      * Add an F32 (float) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addF32 (String name, int fid, float value);

    /**
      * Add an F64 (double) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addF64 (String name, int fid, double value);

    /**
      * Add an String (Java String) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addString (String name, int fid, String value);

    /**
      * Add an MamaDateTime value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addDateTime (String name, int fid, MamaDateTime value);

    /**
      * Add an MamaPrice value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addPrice (String name, int fid, MamaPrice value);

    /**
      * Add an MamaMsg value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addMsg (String name, int fid, MamaMsg value);

    /**
      * Add an Opaque (byte[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addOpaque (String name, int fid,byte[] value);

    /**
      * Add an I8 Array (byte[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayI8 (String name, int fid, byte[] value);

    /**
      * Add a U8 Array (short[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayU8 (String name, int fid, short[] value);

    /**
      * Add an I16 Array (short[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayI16 (String name, int fid, short[] value);

    /**
      * Add a U16 Array (int[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayU16 (String name, int fid, int[] value);

    /**
      * Add an I32 Array (int[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayI32 (String name, int fid, int[] value);

    /**
      * Add a U32 Array (long[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayU32 (String name, int fid, long[] value);

    /**
      * Add an I64 Array (long[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayI64 (String name, int fid, long[] value);

    /**
      * Add a U64 Array (long[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayU64 (String name, int fid, long[] value);

    /**
      * Add a F32 Array (float[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayF32 (String name, int fid, float[] value);

    /**
      * Add a F64 Array (double[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayF64 (String name, int fid, double[] value);

    /**
      * Add a MamaMsg Array (MamaMsg[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The value to be added to the message
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayMsg (String name, int fid, MamaMsg[] value);

    /**
     * This function will add a new field containing an array of messages.
     * Unlinke addArrayMsg it will not add the entire array but only the
     * number of messages supplied in the length parameter.
     * @param name The field name.
     * @param fid  The field id.
     * @param value The array of messages.
     * @param length The number of messages to add from the array, if this
     *                    parameter is outside the array bounds then an
     *                    exception will be thrown.
     * @exception com.wombat.mama.MamaException
     */
    public native void addArrayMsgWithLength (String name, int fid, MamaMsg[] value, int length);

    /**
      * Add a String Array (String[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The array of strings
      * @param length The number of strings to add from the string array, if this
      *                    parameter is outside the array bounds then an
      *                    exception will be thrown.
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void addArrayString (String name, int fid, String[] value, int length);

    /**
      * Add a String Array (String[]) value to the underlying message.
      *
      * @param name The name of the field to be added to the message
      * @param fid The FID of the field to be added to the message
      * @param value The array of strings
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public void addArrayString (String name, int fid, String[] value)
    {
        addArrayString (name, fid, value, value.length);
    }

    /**
      * Update the value of an I8 (byte) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateI8 (String name, int fid, byte value);

    /**
      * Update the value of a U8 (byte) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateU8( String name, int fid, short value );

    /**
      * Update the value of an I16 (short) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateI16 (String name, int fid, short value);

    /**
      * Update the value of a U16 (short) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateU16( String name, int fid, int value );

    /**
      * Update the value of an I32 (int) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateI32 (String name, int fid, int value);

    /**
      * Update the value of a U32 (int) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateU32( String name, int fid, long value );

    /**
      * Update the value of an I64 (long) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateI64 (String name, int fid, long value);

    /**
      * Update the value of a U64 (long) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateU64 (String name, int fid, long value);

    /**
      * Update the value of an F32 (float) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateF32 (String name, int fid, float value);

    /**
      * Update the value of an F64 (double) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateF64 (String name, int fid, double value);

    /**
      * Update the value of an String (Java String) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateString (String name, int fid, String value);

    /**
      * Update the value of a MamaPrice value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updatePrice (String name, int fid, MamaPrice value);

    /**
      * Update the value of a Boolean value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateBoolean (String name, int fid, boolean value);

    /**
      * Update the value of an Char value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateChar (String name, int fid, char value);

    /**
      * Update the value of an MamaDateTime value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateDateTime (String name, int fid, MamaDateTime value);

    /**
      * Update the value of an I8 (byte[]) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateArrayI8 (String name, int fid, byte[] value);

    /**
      * Update the value of a U8 (short[]) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateArrayU8 (String name, int fid, short[] value);

    /**
      * Update the value of an I16 (short[]) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateArrayI16 (String name, int fid, short[] value);

    /**
      * Update the value of a U16 (int[]) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateArrayU16 (String name, int fid, int[] value);

    /**
      * Update the value of an I32 (int[]) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateArrayI32 (String name, int fid, int[] value);

    /**
      * Update the value of a U32 (long[]) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateArrayU32 (String name, int fid, long[] value);

    /**
      * Update the value of an I64 (long[]) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateArrayI64 (String name, int fid, long[] value);

    /**
      * Update the value of a U64 (long[]) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateArrayU64 (String name, int fid, long[] value);

    /**
      * Update the value of an F32 (float[]) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateArrayF32 (String name, int fid, float[] value);

    /**
      * Update the value of an F64 (double[]) value in the underlying message.
      *
      * @param name The name of the field to be updated within the message
      * @param fid The FID of the field to be updated within the message
      * @param value The new value
      *
      * @exception MamaException will be thrown if the message pointer does not
      *                          exist
      */
    public native void updateArrayF64 (String name, int fid, double[] value);

    /**
      * Return the value of a Char field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a char
      */
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

    /**
      * Return the value of a Char field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a char
      */
    public char getChar (MamaFieldDescriptor fieldDesc, char defaultValue)
    {
        return getChar (fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of an I8 field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value  as a byte
      */
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

    /**
      * Return the value of an I8 field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a byte
      */
    public byte getI8 (MamaFieldDescriptor fieldDesc, byte defaultValue)
    {
        return getI8 (fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of a U8 field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a short
      */
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

    /**
      * Return the value of a U8 field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a short
      */
    public short getU8 (MamaFieldDescriptor fieldDesc,
                        short               defaultValue)
    {
        return getU8 (fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of an I16 field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a short
      */
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

    /**
      * Return the value of an I16 field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a short
      */
    public short getI16 (MamaFieldDescriptor fieldDesc, short defaultValue)
    {
        return getI16(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of a U16 field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a int
      */
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

    /**
      * Return the value of a U16 field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a int
      */
    public int getU16 (MamaFieldDescriptor fieldDesc, int defaultValue)
    {
        return getU16(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of an I32 field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a int
      */
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

    /**
      * Return the value of an I32 field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a int
      */
    public int getI32 (MamaFieldDescriptor fieldDesc, int defaultValue)
    {
        return getI32(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of a U32 field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a long
      */
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

    /**
      * Return the value of a U32 field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a long
      */
    public long getU32 (MamaFieldDescriptor fieldDesc, long defaultValue)
    {
        return getU32(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of an I64 field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a long
      */
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

    /**
      * Return the value of an I64 field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a long
      */
    public long getI64 (MamaFieldDescriptor fieldDesc, long defaultValue)
    {
        return getI64(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of a U64 field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a long
      */
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

    /**
      * Return the value of a U64 field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a long
      */
    public long getU64 (MamaFieldDescriptor fieldDesc, long defaultValue)
    {
        return getU64(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of an F32 field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a float
      */
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

    /**
      * Return the value of an F32 field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a float
      */
    public float getF32 (MamaFieldDescriptor fieldDesc, float defaultValue)
    {
        return getF32(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of an F64 field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a double
      */
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

    /**
      * Return the value of an F64 field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a double
      */
    public double getF64 (MamaFieldDescriptor fieldDesc, double defaultValue)
    {
        return getF64(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of a String field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a String
      */
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

    /**
      * Return the value of a String field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a String
      */
    public String getString (MamaFieldDescriptor fieldDesc, String defaultValue)
    {
        return getString(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of a MamaPrice field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a MamaPrice
      */
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

    /**
      * Return the value of a MamaPrice field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a MamaPrice
      */
    public MamaPrice getPrice (MamaFieldDescriptor fieldDesc, MamaPrice defaultValue)
    {
        return getPrice(fieldDesc.getName(), fieldDesc.getFid(), defaultValue);
    }

    /**
      * Return the value of a MamaMsg field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a MamaMsg
      */
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

    /**
      * Return the value of an I32 Array field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a int[]
      */
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

    /**
      * Return the value of a MamaMsg Array field from the underlying message.
      *
      * @param fieldDesc The MamaFieldDescriptor for the field within the message
      * @param defaultValue The default value to use of no value is present
      *
      * @return The value as a MamaMsg[]
      */
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

    /**
      * Try to get the value of a Boolean field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
	public native boolean tryBoolean(String name, int fid, MamaBoolean result);

    /**
      * Return the value of a MamaBoolean field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryBoolean( MamaFieldDescriptor field, MamaBoolean result )
    {
        if (null == field) return false;
        return tryBoolean (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaChar field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryChar(String name, int fid, MamaChar result);

    /**
      * Return the value of a MamaChar field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryChar( MamaFieldDescriptor field, MamaChar result )
    {
        if (null == field) return false;
        return tryChar (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaByte field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryI8(String name, int fid, MamaByte result);

    /**
      * Return the value of a MamaByte field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean  tryI8 (MamaFieldDescriptor field, MamaByte result)
    {
        if (null == field) return false;
        return tryI8 (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaShort field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryU8(String name, int fid, MamaShort result);

    /**
      * Return the value of a MamaShort field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryU8 (MamaFieldDescriptor field, MamaShort result)
    {
        if (null == field) return false;
        return tryU8 (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaShort field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryI16(String name, int fid, MamaShort result);

    /**
      * Return the value of a MamaShort field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryI16 (MamaFieldDescriptor field, MamaShort result)
    {
        if (null == field) return false;
        return tryI16 (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaInteger field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryU16(String name, int fid, MamaInteger result);

    /**
      * Return the value of a MamaInteger field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean  tryU16 (MamaFieldDescriptor field, MamaInteger
                result)
    {
        if (null == field) return false;
        return tryU16 (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaInteger field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryI32(String name, int fid, MamaInteger result);

    /**
      * Return the value of a MamaInteger field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryI32 (MamaFieldDescriptor field, MamaInteger result)
    {
        if (null == field) return false;
        return tryI32 (field.getName(), field.getFid(), result);
    }

    /**
      * Try to get the value of a MamaLong field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryU32(String name, int fid, MamaLong result);

    /**
      * Return the value of a MamaLong field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean  tryU32 (MamaFieldDescriptor field, MamaLong result)
    {
        if (null == field) return false;
        return tryU32 (field.getName(), field.getFid(), result);
    }

    /**
      * Try to get the value of a MamaLong field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryI64(String name, int fid, MamaLong result);

    /**
      * Return the value of a MamaLong field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryI64 (MamaFieldDescriptor field, MamaLong result)
    {
        if (null == field) return false;
        return tryI64 (field.getName(), field.getFid(), result);
    }

    /**
      * Try to get the value of a MamaLong field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryU64(String name, int fid, MamaLong result);

    /**
      * Return the value of a MamaLong field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean  tryU64 (MamaFieldDescriptor field, MamaLong result)
    {
        if (null == field) return false;
        return tryU64 (field.getName(), field.getFid(), result);
    }

    /**
      * Try to get the value of a MamaFloat field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryF32(String name, int fid, MamaFloat result);

    /**
      * Return the value of a MamaFloat field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean  tryF32 (MamaFieldDescriptor field, MamaFloat result)
    {
        if (null == field) return false;
        return tryF32 (field.getName(), field.getFid(), result);
    }

    /**
      * Try to get the value of a MamaDouble field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryF64(String name, int fid, MamaDouble result);

    /**
      * Return the value of a MamaDouble field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryF64 (MamaFieldDescriptor field, MamaDouble result)
    {
        if (null == field) return false;
        return tryF64 (field.getName(), field.getFid(), result);
    }

    /**
      * Return the value of a MamaDateTime field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryDateTime (MamaFieldDescriptor field, MamaDateTime result)
    {
        if (null == field) return false;
        return tryDateTime (field.getName(), field.getFid(),
                result);
    }

    /**
      * Try to get the value of a MamaDateTime field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryDateTime(String name, int fid, MamaDateTime result);

    /**
      * Return the value of a MamaPrice field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryPrice (MamaFieldDescriptor field, MamaPrice result)
    {
        if (null == field) return false;
        return tryPrice (field.getName (), field.getFid(), result);
    }

    /**
      * Try to get the value of a MamaPrice field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryPrice(String name, int fid, MamaPrice result);

    /**
      * Try to get the value of a MamaString field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public native boolean tryString( String name, int fid, MamaString result );

    /**
      * Return the value of a MamaString field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryString( MamaFieldDescriptor field, MamaString result )
    {
        if (null == field) return false;
        return tryString (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaOpaque field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryOpaque (String name, int fid, MamaOpaque result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getOpaque());
        return true;
    }

    /**
      * Return the value of a MamaOpaque field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryOpaque (MamaFieldDescriptor field, MamaOpaque
                result)
    {
        if (null == field) return false;
        return tryOpaque (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaMessage field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryMsg (String name, int fid, MamaMessage result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getMsg());
        return true;
    }

    /**
      * Return the value of a MamaMessage field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryMsg (MamaFieldDescriptor field, MamaMessage result)
    {
        if (null == field) return false;
        return tryMsg (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaArrayInt field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryArrayI32 (String name, int fid, MamaArrayInt result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getArrayI32());
        return true;
    }

    /**
      * Return the value of a MamaArrayInt field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryArrayI32 (MamaFieldDescriptor field, MamaArrayInt
                result)
    {
        if (null == field) return false;
        return tryArrayI32 (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaArrayInt field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryArrayU16 (String name, int fid, MamaArrayInt result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getArrayU16());
        return true;
    }

    /**
      * Return the value of a MamaArrayInt field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryArrayU16 (MamaFieldDescriptor field, MamaArrayInt
                result)
    {
        if (null == field) return false;
        return tryArrayU16 (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaArrayMsg field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryArrayMsg (String name, int fid, MamaArrayMsg result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getArrayMsg());
        return true;
    }

    /**
      * Return the value of a MamaArrayMsg field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryArrayMsg (MamaFieldDescriptor field, MamaArrayMsg
                result)
    {
        if (null == field) return false;
        return tryArrayMsg (field.getName(), field.getFid(), result );
    }

    /**
      * Try to get the value of a MamaArrayString field from the underlying message.
      *
      * @param name The name of the field within the message
      * @param fid The FID of the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryArrayString (String name, int fid, MamaArrayString result)
    {
        MamaMsgField field = getField (name, fid, null);
        if (field == null) return false;
        result.setValue (field.getArrayString());
        return true;
    }

    /**
      * Return the value of a MamaArrayMsg field from the underlying message.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param result The result (not modified if field not present)
      *
      * @return Boolean representing whether the field was present or not
      */
    public boolean tryArrayString (MamaFieldDescriptor field, MamaArrayString result)
    {
        if (null == field) return false;
        return tryArrayString (field.getName(), field.getFid(), result);
    }

    /**
      * Iterate over all of the fields within the message.
      * The <code>onField()</code> method of the
      * <code>MamaMsgFieldIterator</code> instance will be invoked for each
      * field in the underlying message.
      *
      * @param iterator Callback object implementing the MamaMsgFieldIterator interface
      * @param dictionary Optional MamaDictionary object
      * @param closure Optional user supplied arbitrary closure data which will be
      *                 passed back in <code>onField()</code>
      */
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

    /**
      * Return a String representation of the entire message.

      * @param fid The FID of the field
      * @param dictionary Optional MamaDictionary object
      *
      * @return Return a String representation the field with the given fid
      */
    public native String getFieldAsString( int fid, MamaDictionary dictionary );

    /**
      * Return a String representation of the entire message.
      *
      * @return Boolean indicating whether this message is a p2p request
      */
    public native boolean isFromInbox();

    /**
      * Return a String representation of the entire message.
      *
      * @return the String representation of the entire message.
      */
    public native String toString();

    /**
      * Return a json String representation of the entire message.
      *
      * @return the String representation of the entire message.
      */
    public native String toJsonString();

    /**
      * Return a json String representation of the entire message using field names
      * from the dictionary.
      *
      * @return the String representation of the entire message.
      */
      public String toJsonString(MamaDictionary dictionary) {
        return toJsonStringWithDictionary(dictionary);
      }

    /**
      * Return a json String representation of the entire message.
      *
      * @param dictionary MamaDictionary object
      *
      * @return the String representation of the entire message.
      */
    private native String toJsonStringWithDictionary(MamaDictionary dictionary);

    /**
      * Return a normalized String representation of the entire message.
      *
      * @return the String representation of the entire message.
      */
    public native String toNormalizedString();

    /**
     * Return as a character representation of the payload type used in the message.
     *
     * @return A MamaPayloadType indicating which message payload is used in the
     *         message.
     */
    public native char getPayloadType();

    /**
      * Destroy the internal state of the MamaMsg object.
      * Needed for JNI so we know when to free the underlying C strucure.
      * Should only be called if the client app has created the message.
      * i.e. do not call on messages passed in the callbacks as these are
      * managed internal to the MAMA API
      */
    public void destroy ()
    {
        mamaBuffer = null;

        if (myIterator != null)
        {
            myIterator.destroy();
        }

        _destroy();
    }

    private native void _destroy();

    /**
      * Clear the message.
      * All fields are removed
      */
    public native void clear();

    /**
      * Create a deep copy of the message.
      *
      * @param rhs The MamaMsg to be copied
      */
    public native void copy(MamaMsg rhs);

    /**
      * Detach the message.
      * This will take a deep copy of the MamaMsg and transfer the ownership to the application
      * and it will be the responsibility of the user to destroy.
      */
    public native MamaMsg detach ();

    /**
      * Update the MamaMsg with the fields of another MamaMsg.
      *
      * @param rhs The MamaMsg to be used to update the current message
      */
    public native void apply(MamaMsg rhs);

    /*Will create the underlying C Message and set the pointer value
     for the Java MamaMsg object*/
    private native void create ();

    private native void createForPayload (char payloadId);

    private native void createForPayloadBridge (MamaPayloadBridge payloadBridge);

    /*Used to cache ids for callback methods/fields*/
    private static native void initIDs();

    /**
      * Get a specified field from a message, or null if not found.
      *
      * @param field The MamaFieldDescriptor for the field within the message
      * @param dict Optional MamaDictionary object
      *
      * @return The MamaMsgField or null
      */
    public MamaMsgField getField (MamaFieldDescriptor field,
                                  MamaDictionary dict)
    {
        return getField (field.getName (), field.getFid(), dict);
    }

    /**
      * Get a specified field from a message, or null if not found.
      *
      * @param fieldName The name of the field within the message
      * @param fid The FID of the field within the message
      * @param dict Optional MamaDictionary object
      *
      * @return The MamaMsgField or null
      */
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

    private native void  _getField(String fieldName, int fid,
                                  MamaDictionary dict);

    protected void finalize ()
    {
        destroy ();
    }

    /* ************************************************** */
    /* Private Native Functions. */
    /* ************************************************** */

    /**
     * This function
     * @param name The field name.
     * @param fid The field Id.
     * @param byteArray The byte array to populate.
     * @param arraySize The size of the array.
     * @param throwOnError Whether we want this function to return an exception or not.
     * @return The number of bytes copied to the buffer. If the buffer isn't
     *         big enough then the return will be the negative number of bytes
     *         required. Note that if anything goes wrong exceptions will be
     *         thrown.
     */
    private native int nativeGetAsBuffer(String name, int fid, byte[] byteArray, int arraySize, boolean throwOnError);
}
