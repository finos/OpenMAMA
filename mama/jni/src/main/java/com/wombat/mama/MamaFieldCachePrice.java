//--------------------------------------------------------------------------
// Copyright (c) 1999-2006 Wombat Financial Software Inc., of Incline
// Village, NV.  All rights reserved.
//
// This software and documentation constitute an unpublished work and
// contain valuable trade secrets and proprietary information belonging
// to Wombat.  None of this material may be copied, duplicated or
// disclosed without the express written permission of Wombat.
//
// WOMBAT EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING THIS
// SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF
// MERCHANTABILITY AND/OR FITNESS FOR ANY PARTICULAR PURPOSE, AND
// WARRANTIES OF PERFORMANCE, AND ANY WARRANTY THAT MIGHT OTHERWISE ARISE
// FROM COURSE OF DEALING OR USAGE OF TRADE. NO WARRANTY IS EITHER
// EXPRESS OR IMPLIED WITH RESPECT TO THE USE OF THE SOFTWARE OR
// DOCUMENTATION.
//
// Under no circumstances shall Wombat be liable for incidental, special,
// indirect, direct or consequential damages or loss of profits,
// interruption of business, or related expenses which may arise from use
// of software or documentation, including but not limited to those
// resulting from defects in software and/or documentation, or loss or
// inaccuracy of data of any kind.
//
//----------------------------------------------------------------------------


package com.wombat.mama;

public class MamaFieldCachePrice extends MamaFieldCacheField
{
    private MamaPrice mValue;

    /**
     * Constructor.  Use one of the create() methods to actually
     * initialize the field.
     */

    public MamaFieldCachePrice (MamaFieldDescriptor desc)
    {
        super (desc,MamaFieldDescriptor.PRICE);
        mValue = null;
    }

    /**
     * Constructor.
     */

    public MamaFieldCachePrice (int fid, String name, boolean trackModState)
    {
        super (fid,MamaFieldDescriptor.PRICE,name,trackModState);
        mValue = null;
    }

    public MamaFieldCacheField copy ()
    {
        MamaFieldCachePrice result = new MamaFieldCachePrice (
            mDesc.getFid (),
            mDesc.getName (),
            mTrackModState);
        result.mValue = new MamaPrice (mValue);
        return result;
    }

    /*  Description :   This function will add the contents of the cache field
     *                  to the supplied message.
     *  Arguments   :   fieldName   [I] True to add the field name to the message.
     *                  message     [I] The message where the field will be added.
     */
    public void addToMessage(boolean fieldName, MamaMsg message)
    {
        // Check to see if field names should be added
        String name = null;
        if (fieldName)
        {
            // Get the field name from the descriptor
            name = mDesc.getName();
        }

        // Add the value
        message.addPrice(name, mDesc.getFid (), mValue);
    }

    /**
     * Apply the MamaMsgField to the cached field.
     */

    public void apply(MamaMsgField msgField)
    {
        mValue = msgField.getPrice ();
    }

    /**
     * Explicitly set the cached value.
     */

    public void set (MamaPrice value)
    {
        if (mTrackModState)
        {
            if (mValue == value)
            {
                if (!isModified())
                     setModState (MamaFieldCacheField.MOD_STATE_TOUCHED);
                return;
            }
            else
            {
                setModState (MamaFieldCacheField.MOD_STATE_MODIFIED);
                // Drop through to assignment.
            }
        }
        if (value == null)
            mValue = null;
        else
        {
            if (mValue == null)
                mValue = new MamaPrice (value);
            else
                mValue.copy(value);
        }
    }

    /**
     * Check whether the field value is equal.
     */

    public boolean isEqual (MamaPrice value)
    {
        if (mValue == null)
        {
            return (value == null);
        }
        else
        {
            return  mValue.equals(value);
        }
    }

    /**
     * Return the cached value.
     */

    public MamaPrice get ()
    {
        return (mValue != null) ? mValue : null;
    }

    /**
     * Copy the cached value into the result.
     */

    public void get (MamaPrice result)
    {
        result = mValue;
    }

    /**
     * Return a string representation of the value
     */

    public String getAsString ()
    {
        return (mValue != null) ? mValue.toString() : "null";
    }

    public void apply (MamaFieldCacheField value)
    {
        if (value == null)
            throw new MamaException("MamaFieldCachePrice.apply() cannot be called with a null value");

        if (((MamaFieldCachePrice)value).get() == null)
        {
            mValue = null;
            return;
        }

        if (mValue == null)
            mValue = new MamaPrice(((MamaFieldCachePrice)value).mValue);
        else
            mValue. copy (((MamaFieldCachePrice)value).mValue);
    }
}
