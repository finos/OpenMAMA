/* $Id:
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
 * This class defines bit flags for the MamaMsgQualifier field values. 
 * Instances may also be used as a filter. Bits are queried and set using 
 * the set and get methods. Static fields exist for each possible bit.
 */
public class MamaMsgQualifier
{
    /**
     * The message is possibly a duplicate. It may be possible to make an absolute determination by comparing
     * wSeqNum and wSrcTime with the values in the previous message.
     */
    public static final MamaMsgQualifier POSSIBLY_DUPLICATE   = new MamaMsgQualifier ((short) 0x0001);

    /**
     * The message is definitely a duplicate. Current feed handlers do not use this flag.
     */
    public static final MamaMsgQualifier DEFINITELY_DUPLICATE = new MamaMsgQualifier ((short) 0x0002);

    /**
     * The current message may be delayed.
     */
    public static final MamaMsgQualifier POSSIBLY_DELAYED     = new MamaMsgQualifier ((short) 0x0004);

    /**
     * The current message is definitely delayed. This will be true during a fault tolerant takeover.
     */
    public static final MamaMsgQualifier DEFINITELY_DELAYED   = new MamaMsgQualifier ((short) 0x0008);

    /**
     * The message is out of sequence.
     */
    public static final MamaMsgQualifier OUT_OF_SEQUENCE      = new MamaMsgQualifier ((short) 0x0010);

    /**
     * This bit is never set by the FH. It is a place holder to make this class more useful.
     */
    public static final MamaMsgQualifier INVALID              = new MamaMsgQualifier ((short) 0x1000);

    private short myValue = 0x0000;

    public MamaMsgQualifier()
    {
        myValue = INVALID.myValue;
    }

    MamaMsgQualifier (short value)
    {
        myValue = value;
    }

    public MamaMsgQualifier (MamaMsgQualifier qualifierFilter)
    {
        myValue = qualifierFilter.myValue;
    }

    /**
     * Return true if this message is definitely a duplicate message. This
     * condition will not occur with the current feed handlers.
     *
     * @return true is a duplicate.
     */
    public boolean getIsDefinitelyDuplicate ()
    {
        return (myValue & INVALID.myValue) == 0 && (myValue & DEFINITELY_DUPLICATE.myValue) != 0;
    }

    /**
     * Return true if this message is possibly a duplicate message. This may
     * occur in the event of a fault tolerant feed handler take over where the
     * feed handler replays messages to prevent gaps.
     *
     * @return true if the message may be a duplicate.
     */
    public boolean getIsPossiblyDuplicate ()
    {
        return (myValue & INVALID.myValue) == 0 && (myValue & POSSIBLY_DUPLICATE.myValue) != 0;
    }

    /**
     * Return true if the message is possibly delayed. This condition may be
     * true during a fault-tolerant take over.
     *
     * @return true if the message may be delayed.
     */
    public boolean getIsPossiblyDelayed ()
    {
        return (myValue & INVALID.myValue) == 0 && (myValue & POSSIBLY_DELAYED.myValue) != 0;
    }

    /**
     * Return true if the message is delayed. This condition may be
     * true during a fault-tolerant take over.
     *
     * @return true if the message is delayed
     */
    public boolean getIsDefinitelyDelayed ()
    {
        return (myValue & INVALID.myValue) == 0 && (myValue & DEFINITELY_DELAYED.myValue) != 0;
    }

    /**
     * @return true when the FH sends the message out of sequence.
     */
    public boolean getIsOutOfSequence ()
    {
        return (myValue & INVALID.myValue) == 0 && (myValue & OUT_OF_SEQUENCE.myValue) != 0;
    }

    /**
     * @return true if this instance is valid
     */
    public boolean isValid ()
    {
        return (myValue & INVALID.myValue) == 0;
    }
    /**
     * Set the definitely duplicate bit.
     *
     * @param value for the bit .
     */
    public void setIsDefinitelyDuplicate (boolean value)
    {
        if (value)
            setBits (DEFINITELY_DUPLICATE.myValue);
        else
            unsetBits (DEFINITELY_DUPLICATE.myValue);
    }

    /**
     * Set the possibly duplicate bit.
     *
     * @param value for the bit .
     */
    public void setIsPossiblyDuplicate (boolean value)
    {
        if (value)
            setBits (POSSIBLY_DUPLICATE.myValue);
        else
            unsetBits (POSSIBLY_DUPLICATE.myValue);
    }

    /**
     * Set the possibly delayed bit.
     *
     * @param value for the bit .
     */
    public void setIsPossiblyDelayed (boolean value)
    {
        if (value)
            setBits (POSSIBLY_DELAYED.myValue);
        else
            unsetBits (POSSIBLY_DELAYED.myValue);
    }

    /**
     * Set the possibly delayed bit.
     *
     * @param value for the bit .
     */
    public void setIsDefinitelyDelayed (boolean value)
    {
        if (value)
            setBits (DEFINITELY_DELAYED.myValue);
        else
            unsetBits (DEFINITELY_DELAYED.myValue);
    }

    /**
     * Set the possibly out of sequence bit.
     *
     * @param value for the bit .
     */
    public void setOutOfSequence (boolean value)
    {
        if (value)
            setBits (OUT_OF_SEQUENCE.myValue);
        else
            unsetBits (OUT_OF_SEQUENCE.myValue);
    }

    /**
     * set the invalid bid
     *
     * @param value value to set
     */
    public void setInvalid (boolean value)
    {
        if (value)
            setBits (INVALID.myValue);
        else
            unsetBits (INVALID.myValue);
    }

    private void setBits (short bits)
    {
        myValue |= bits;
    }

    private void unsetBits (short bits)
    {
        myValue &= ~bits;
    }

    /**
     * Return true if any bits in value match the bits set in this qualifier.
     * @param value  The value to filter.
     * @return true if there is a match
     */
    public boolean filter (short value)
    {
        return 0 != (myValue & value);
    }
}
