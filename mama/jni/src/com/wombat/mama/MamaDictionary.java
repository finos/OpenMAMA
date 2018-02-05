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

import java.util.Collection;

/*
* Wrapper class for the native C dictionary structure
*/
public class MamaDictionary
{
    static
    {
        initIDs();
    }

    /* A long value containing a pointer to the underlying C dictionary structure*/
    private long dictionaryPointer_i = 0;

    /*
        Stores the value of this dictionary's closure so we can delete when the 
        dictionary is destroyed.
    */
    private long closurePointer_i;

    public long getPointerVal()
    {
        return dictionaryPointer_i;
    }

    public native MamaFieldDescriptor createFieldDescriptor (
                                    int       fid,
                                    String    name,
                                    short     type);

    public native void buildDictionaryFromMessage (MamaMsg msg);

    public native MamaMsg getDictionaryMessage ();
    
    /* Return the dictionary source feed name.  */
    public native MamaFieldDescriptor getFeedName ();
    
    /* Return the dictionary source feed host.  */
    public native MamaFieldDescriptor getFeedHost ();

    /* Return the field with the specified field FID.  */
    public native MamaFieldDescriptor getFieldByFid (int fid);

    /* Return the field with the corresponding zero based index. */
    public native MamaFieldDescriptor getFieldByIndex (int index);

    /* Return the field with the specified name.  */
    public native MamaFieldDescriptor getFieldByName (String name);

    /*
    * Return a <code>Collection</code> containing all of the fields with the
    * specified name.
    */
    public native Collection getFieldByNameAll (String name);

    /* Return the highest field identifier.  */
    public native int getMaxFid();

    /* Return the number of fields in the dictionary. */
    public native int getSize();

    /* Return true if there are multiple fields with the same name. */
    public native boolean hasDuplicates();

    /*
        As the Java API provides no destroy() method we need a way
        to ensure that the C allocated memory is freed when no longer
        needed.
    */
    public void finalize()
    {
        if (dictionaryPointer_i != 0)
            doFinalizer();
    }

    /* Write the dictionary to the specified file */
    public native void writeToFile (String file);

    /* Populate the dictionary from file.  We need to create an underlying 
     dictionary as the dict is usually created from a subscription */
    public void populateFromFile (String file)
    {
        if (dictionaryPointer_i == 0)
        {
            create ();
        }

        _populateFromFile (file);
    }

    /* The native implemention of populateFromFile */
    public native void _populateFromFile (String file);

    public native void create ();

    private native void doFinalizer();
    
    /*Used to cache ids for callback methods/fields*/
    private static native void initIDs();
}
