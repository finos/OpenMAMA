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

public interface MamaFieldCacheReadOnlyField
{   
    
    /**
     * Make a copy of this field.
     *
     * @return the copy
     */
    public MamaFieldCacheField copy ();

    /**
     * Create a string value of the field in the buffer provided.
     */
     
    public String getAsString ();

    /**
     * Return the MamaFieldDescriptor for this field (or NULL).
     */
    public MamaFieldDescriptor getDescriptor();

    /**
     * Return the actual field type for this field.  It is possible to
     * create fields that actually differ from the type specified in
     * the dictionary field descriptor.
     */
    public short getType();

    /**
     * Get the track modification state for the field
     */
    public boolean getTrackModState ();
  
    /**
     * Get the modification state for the field.
     */
    public short getModState ();

    /**
     * Return whether the field is unmodified.
     */
    public boolean isUnmodified ();

    /**
     * Return whether the field is modified.
     */
    public boolean isModified ();

    /**
     * Return whether the field is modified.
     */
    public boolean isTouched ();
}