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

import java.util.ArrayList;
import java.util.Iterator;

public class MamaFieldCacheFieldList
{
    private ArrayList mFields;

    public MamaFieldCacheFieldList()
    {
        mFields = new ArrayList(128);
    }
    
    public void clear()
    {
        mFields.clear();
    }
    
    public void clearAndDelete()
    {
        mFields.clear();
    }
    
    public void add(MamaFieldCacheField  field)
    {
        if (field!=null)
        {
            MamaFieldDescriptor desc = field.getDescriptor();
            if (desc!=null)
            {
                mFields.add(field);
            }
        }
    }
    
    public void addIfModified(MamaFieldCacheField field)
    {
        if ((field!=null) && !field.isUnmodified())
        {
            MamaFieldDescriptor desc = field.getDescriptor();
            if (desc!=null)
            {
                mFields.add(field);
            }
        }
    }

    public MamaFieldCacheField find(String name)
    {
        if (name == null)
            return null;

        for (int i=0; i<mFields.size(); i++)
        {
            MamaFieldCacheField field = (MamaFieldCacheField)mFields.get(i);
            if (name.equals(field.getDescriptor().getName()))
                return field;
        }

        return null;
    }

    public MamaFieldCacheField find(int fid)
    {
        for (int i=0; i<mFields.size(); i++)
        {
            MamaFieldCacheField field = (MamaFieldCacheField)mFields.get(i);
            if (fid == field.getDescriptor().getFid())
                return field;
        }

        return null;

    }

    public MamaFieldCacheField find(MamaFieldDescriptor desc)
    {
        return find(desc.getFid());
    }

    public int size()
    {
        return mFields.size();
    }
    
    public boolean empty()
    {
        return mFields.isEmpty();
    }
    
    public Iterator getIterator()
    {
        return mFields.iterator();
    }

    protected ArrayList getArrayList()
    {
        return mFields;
    }
}
