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

public class MamaFieldCacheFloat32 extends MamaFieldCacheField
{
    private float mValue;
    
    /**
     * Constructor.  Use one of the create() methods to actually
     * initialize the field.
     */
     
    public MamaFieldCacheFloat32 (MamaFieldDescriptor desc)
    {
        super(desc,MamaFieldDescriptor.F32);
        mValue = 0;
    }
    
    /**
     * Constructor.
     */
     
    public MamaFieldCacheFloat32 (int fid, String name, boolean trackModState)
    {
        super(fid,MamaFieldDescriptor.F32,name,trackModState);
        mValue = 0;
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
        message.addF32(name, mDesc.getFid (), mValue);
    }

    public MamaFieldCacheField copy ()
    {
        MamaFieldCacheFloat32 result = new MamaFieldCacheFloat32 (
            mDesc.getFid (),
            mDesc.getName (),
            mTrackModState);
        result.mValue = mValue;
        return result;
    }

    /**
     * Apply the MamaMsgField to the cached field.
     */
     
    public void apply(MamaMsgField msgField)
    {
        mValue = msgField.getF32 ();
    }
    
    /**
     * Explicitly set the cached value.
     */
    
    public void set (float value)
    {
        if (mTrackModState)
        {
            if (mValue == value)
            {
                if (isModified())
                    setModState (MamaFieldCacheField.MOD_STATE_TOUCHED);
                return;
            }
            else
            {
                setModState (MamaFieldCacheField.MOD_STATE_MODIFIED);
                // Drop through to assignment.               
            }            
        }
        mValue = value;
    }
    
    /**
     * Check whether the field value is equal.
     */
     
    public boolean isEqual (float value)
    {     
        return value == mValue;      
    }
    
    /**
     * Return the cached value.
     */
     
    public float get ()
    {
        return mValue;
    }
    
    /**
     * Copy the cached value into the result.
     */
    
    public void get (float result)
    {
        result = mValue;
    }
   
    /**
     * Return a string representation of the value
     */
     
    public String getAsString ()
    {
        return new Float(mValue).toString();    
    }

    public void apply (MamaFieldCacheField value)
    {
        mValue = ((MamaFieldCacheFloat32)value).mValue;
    }
}
