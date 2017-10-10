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

public abstract class MamaFieldCacheField implements MamaFieldCacheReadOnlyField
{
    public static final short MOD_STATE_NOT_MODIFIED = 1;
    public static final short MOD_STATE_MODIFIED = 2;
    public static final short MOD_STATE_TOUCHED = 3;

    protected short               mType;
    protected MamaFieldDescriptor mDesc;
    protected short               mModState;
    protected boolean             mTrackModState;
    protected boolean             mLocalDescriptorCopy;

    /**
     * Constructor.  Use one of the create() methods to actually
     * initialize the field.
     */
     
    public MamaFieldCacheField(MamaFieldDescriptor desc,
                               short type)
    {
        mDesc = desc;
        mModState = MamaFieldCacheField.MOD_STATE_MODIFIED;
            
        mTrackModState = false;
        mLocalDescriptorCopy = false;
        
        if ((desc!=null) && (type == MamaFieldDescriptor.UNKNOWN))
            mType = desc.getType();
        else
            mType = type;
        if (mDesc!=null)
            mTrackModState = mDesc.getTrackModState();
    }
    
    /**
     * Constructor. Also create a local copy of field descriptor based on
     * supplied info.
     */
     
    public MamaFieldCacheField (int      fid,
                                short    type,
                                String   name,
                                boolean  trackModState)
    {
        this(new MamaFieldDescriptor (fid, type, name,null), type);
        mTrackModState = trackModState;
        mLocalDescriptorCopy = true;        
    }

    /**
     * Make a copy of this field.
     *
     * @return the copy
     */
    public abstract MamaFieldCacheField copy ();

    /*  Description :   This function will add the contents of the cache field
     *                  to the supplied message.
     *  Arguments   :   fieldName   [I] True to add the field name to the message.
     *                  message     [I] The message where the field will be added.
     */
    public abstract void addToMessage(boolean fieldName, MamaMsg message);

    /**
     * Apply the MamaMsgField to the cached field.
     */
     
    public abstract void apply (MamaMsgField msgField);

    /**
     * Create a string value of the field in the buffer provided.
     */
     
    public abstract String getAsString ();

    /**
     * Set the MamaFieldDescriptor for this field.  This is an unusual
     * thing to want to do to an existing field but it can help with
     * performance for specialized tasks.
     */
        
    public void setDescriptor (MamaFieldDescriptor desc)
    {
        if (mDesc!=null)
            mTrackModState = mDesc.getTrackModState();
        mDesc = desc; 
    }

    /**
     * Return the MamaFieldDescriptor for this field (or NULL).
     */
    public MamaFieldDescriptor getDescriptor() { return mDesc; }

    /**
     * Return the actual field type for this field.  It is possible to
     * create fields that actually differ from the type specified in
     * the dictionary field descriptor.
     */
    public short getType() { return mType; }


    /**
     * Set the whether track modification state for the field
     */
    public void setTrackModState (boolean state)
    {
        mTrackModState = state;
    }

    /**
     * Get the track modification state for the field
     */
    public boolean getTrackModState ()
    {
        return mTrackModState;
    }


    /**
     * Set the modification state for the field.
     */
   public void setModState (short  state) 
    { 
        if (mTrackModState)
            mModState = state; 
    }

    /**
     * Clear the modification state for the field (i.e., set to
     * MOD_STATE_NOT_MODIFIED).
     */
    public void clearModState () 
    { 
        if (mTrackModState)
            mModState = MamaFieldCacheField.MOD_STATE_NOT_MODIFIED; 
    }

    /**
     * Get the modification state for the field.
     */
    public short getModState ()  { return mModState; }

    /**
     * Return whether the field is unmodified.
     */
    public boolean isUnmodified () { return mModState == MamaFieldCacheField.MOD_STATE_NOT_MODIFIED; }

    /**
     * Return whether the field is modified.
     */
    public boolean isModified ()  { return mModState == MamaFieldCacheField.MOD_STATE_MODIFIED; }

    /**
     * Return whether the field is modified.
     */
    public boolean isTouched () { return mModState == MamaFieldCacheField.MOD_STATE_TOUCHED; }

    /**
     * Explicitly "touch" the cached value.  To touch the cached value
     * means to mark it as modified (or touched, actually) without
     * actually bothering to check/update the current value.
     */
    public void  touch () 
    {
        if (mTrackModState)
        {
            if (!isModified())
                setModState (MamaFieldCacheField.MOD_STATE_TOUCHED);
        }
    }

    public abstract void apply (MamaFieldCacheField value);
}
