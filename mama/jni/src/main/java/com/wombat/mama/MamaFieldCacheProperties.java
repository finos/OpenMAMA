//----------------------------------------------------------------------------
//
// $Id$
//
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

import java.util.Iterator;
import java.util.HashMap;
import java.util.NoSuchElementException;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.io.OutputStream;
import java.io.PrintWriter;

/**
 * The MamaFieldCacheProperties class is used for MamdaOrderBookProperties.
 *
 * This class maintains a collection of MamaFieldCacheField objects mapped to
 * fids. A field will only be added to or updated in the cache if its fid
 * matches a fid previously registed with the <code>registerField</code> method.
 *
 */
public class MamaFieldCacheProperties
{
    private static Logger mLogger =
                   Logger.getLogger("com.wombat.mama.MamaFieldCacheProperties");

    private HashMap mFields;
    // The serial number avoids iterating on clear!
    private long    mSerial = 1;
    // For efficient lookups.
    private MamaInteger mTmpInt = new MamaInteger ();


    public MamaFieldCacheProperties (int hashSize)
    {
        mFields = new HashMap (hashSize);
    }

    public MamaFieldCacheProperties ()
    {
        mFields = new HashMap(10);
    }

    public MamaFieldCacheProperties (MamaFieldCacheProperties source)
    {
        mSerial = source.mSerial;
        for (Iterator i = mFields.keySet ().iterator (); i.hasNext ();)
        {
            MamaInteger key =  (MamaInteger) i.next ();
            Entry e = new Entry ((Entry)mFields.get (key) );
            mFields.put (key, e);
        }
    }

    /**
     * This method clears the fields but does not unregister the properteis.
     */
    public void clear()
    {
        mSerial += 2; // Will never be 0
    }

    public void clearAndDelete ()
    {
        clear ();
    }

    /**
     * Add a field to the cache. If it is not already a property it will be
     * added with its fid.
     *
     * @param field The field to add.
     */
    public void add (MamaFieldCacheField field)
    {
        if (field!=null)
        {
            MamaFieldDescriptor desc = field.getDescriptor();
            if (desc!=null)
            {
                mFields.put (new MamaInteger(desc.getFid ()),
                              new Entry (field, mSerial) );
            }
        }
    }

    /**
     * Add a field to the cache. If it is not already a property it will be
     * added with its fid. The field only gets added if the supplied field
     * is modified.
     *
     * @param field The field to add.
     */
    public void addIfModified (MamaFieldCacheField field)
    {
        if (field!=null && field.isModified ())
        {
            MamaFieldDescriptor desc = field.getDescriptor();
            if (desc!=null)
            {
                mFields.put (new MamaInteger(desc.getFid ()),
                              new Entry (field, mSerial));
            }
        }
    }

    /*public void clearAndDelete()
    {
        mSerial += 2; // Will never be 0
    } */

    /**
     * Clear the cache and unregister all the properties.
     *
     */
    public void clearAndUnregisterAll ()
    {
        mSerial = 1;
        mFields.clear ();
    }

    /**
     * Adds a property. The MamaFieldCacheField will not appear in iterations
     * until an <code>apply()</code> for that field occurs.
     *
     * @param fid The fid
     * @param field The field.
     */
    public void registerProperty(int fid, MamaFieldCacheField  field)
    {
        mTmpInt.setValue (fid);
        if (mFields.get (mTmpInt) != null)
        {
            return; // Already registered.
        }

        if (field!=null)
        {
            MamaFieldDescriptor desc = field.getDescriptor();
            if (desc!=null)
            {
                Entry e = new Entry (field);
                mFields.put(new MamaInteger(fid), e);
            }
        }
    }

    /**
     * Apply the field to the cache. If the field does not exist as a property
     * the cache does not change.
     * @param value The new value
     * @return true if a the field was found and updated.
     */
    public boolean apply (MamaMsgField value)
    {
        mTmpInt.setValue (value.getFid ());
        Entry entry = (Entry) mFields.get (mTmpInt);
        if (entry != null)
        {
            entry.mSerial = mSerial;
            entry.mField.apply (value);
            return true;
        }
        return false;
    }

    private boolean apply (MamaFieldCacheField value)
    {
        MamaFieldDescriptor desc = value.getDescriptor ();
        mTmpInt.setValue (desc.getFid ());
        Entry entry = (Entry) mFields.get (mTmpInt);
        if (entry != null)
        {
            entry.mSerial = mSerial;
            entry.mField.apply (value);
            return true;
        }
        return false;
    }

    /**
     * Find a field by name. This is less efficient than find by Fid or field
     * descriptor.
     *
     * @param name The field name.
     * @return The field or null.
     */
    public MamaFieldCacheField find(String name)
    {
        // MLS: This is not very efficient. We could use another map (name->field),
        // if this causes problems. It is no less efficient than the
        // MamaFieldCacheFieldList method.
        if (name!=null)
        {
            for (Iterator i = mFields.values ().iterator ();
                 i.hasNext ();)
            {
                Entry e = (Entry) i.next ();
                if (e.mField.getDescriptor ().getName ().equals (name) &&
                    e.mSerial == mSerial)
                {
                    return e.mField;
                }
            }
        }
        return null;
    }

    /**
     * Find a field by fid.
     * @param fid The fid
     * @return  the field or null.
     */
    public MamaFieldCacheField find(int fid)
    {
        mTmpInt.setValue (fid);
        Entry e = (Entry) mFields.get (mTmpInt);
        if (e != null && e.mSerial == mSerial)
            return e.mField;
        return null;
    }

    /**
     * Find by field descriptor. This method uses the fid from the descriptor.
     * @param desc The descriptor.
     * @return the field or null.
     */
    public MamaFieldCacheField find(MamaFieldDescriptor desc)
    {
        if (desc != null)
            return find (desc.getFid ());
        return null;
    }

    /**
     * Return the number of properties in the cache.
     * @return the size.
     */
    public int size()
    {
        return mFields.size();
    }

    /**
     * Return true if the cache is empty.
     *
     * @return true if the cache contains no properties.
     */
    public boolean empty()
    {
        return mFields.isEmpty();
    }

    /**
     * Returns an iterator that only iterates over the properties that have
     * had values applied.
     * @return the iterator.
     */
    public Iterator getIterator()
    {
        return new Iterator()
        {
            Iterator mIterator = mFields.values ().iterator ();
            Entry    mNextEntry = null;

            public void remove ()
            {
                mIterator.remove ();
            }

            public boolean hasNext ()
            {
                while (mIterator.hasNext ())
                {
                    // Look for a property that is applied.
                    mNextEntry = (Entry) mIterator.next ();
                    if (mNextEntry.mSerial == mSerial)
                        return true;
                }
                return false;
            }

            public Object next ()
            {
                if (mNextEntry != null || hasNext ())
                {
                    Entry rval = mNextEntry;
                    mNextEntry = null;
                    return rval.mField;
                }
                throw new NoSuchElementException ();
            }
        };
    }

    public void registerProperties (int[] fids,
                                    MamaDictionary theDictionary)
    {
        for (int i = 0; i < fids.length; i++)
        {
            int fid = fids[i];
            MamaFieldDescriptor field = theDictionary.getFieldByFid (fid);

            if (field == null && mLogger.isLoggable (Level.FINE))
                mLogger.fine ("Property field not in dictionary fid=" + fid);

            if (field != null)
            {
                switch(field.getType ())
                {
                    case MamaFieldDescriptor.MSG:
                        mLogger.fine ("Property type MSG not supported.");
                        break;
                    case MamaFieldDescriptor.OPAQUE:
                        mLogger.fine ("Property type OPAQUE not supported.");
                        break;
                    case MamaFieldDescriptor.STRING:
                        registerProperty(fid, new MamaFieldCacheString (field));
                        break;
                    case MamaFieldDescriptor.BOOL:
                        registerProperty (fid, new MamaFieldCacheBool (field));
                        break;
                    case MamaFieldDescriptor.CHAR:
                        registerProperty (fid, new MamaFieldCacheChar (field));
                        break;
                    case MamaFieldDescriptor.I8:
                        registerProperty (fid, new MamaFieldCacheInt8 (field));
                        break;
                    case MamaFieldDescriptor.U8:
                        registerProperty (fid, new MamaFieldCacheUint8 (field));
                        break;
                    case MamaFieldDescriptor.I16:
                        registerProperty (fid, new MamaFieldCacheInt16 (field));
                        break;
                    case MamaFieldDescriptor.U16:
                        registerProperty(fid, new MamaFieldCacheUint16 (field));
                        break;
                    case MamaFieldDescriptor.I32:
                        registerProperty (fid, new MamaFieldCacheInt32 (field));
                        break;
                    case MamaFieldDescriptor.U32:
                        registerProperty(fid, new MamaFieldCacheUint32 (field));
                        break;
                    case MamaFieldDescriptor.I64:
                        registerProperty (fid, new MamaFieldCacheInt64 (field));
                        break;
                    case MamaFieldDescriptor.U64:
                        registerProperty(fid, new MamaFieldCacheUint64 (field));
                        break;
                    case MamaFieldDescriptor.F32:
                        registerProperty(fid, new MamaFieldCacheFloat32(field));
                        break;
                    case MamaFieldDescriptor.F64:
                        registerProperty(fid, new MamaFieldCacheFloat64(field));
                        break;
                    case MamaFieldDescriptor.TIME:
                        registerProperty(fid,new MamaFieldCacheDateTime(field));
                        break;
                    case MamaFieldDescriptor.PRICE:
                        registerProperty (fid, new MamaFieldCachePrice (field));
                        break;

                    case MamaFieldDescriptor.I8ARRAY:
                    case MamaFieldDescriptor.U8ARRAY:
                    case MamaFieldDescriptor.I16ARRAY:
                    case MamaFieldDescriptor.U16ARRAY:
                    case MamaFieldDescriptor.I32ARRAY:
                    case MamaFieldDescriptor.U32ARRAY:
                    case MamaFieldDescriptor.I64ARRAY:
                    case MamaFieldDescriptor.U64ARRAY:
                    case MamaFieldDescriptor.F32ARRAY:
                    case MamaFieldDescriptor.F64ARRAY:
                    case MamaFieldDescriptor.VECTOR_STRING:
                    case MamaFieldDescriptor.VECTOR_MSG:
                    case MamaFieldDescriptor.VECTOR_TIME:
                    case MamaFieldDescriptor.VECTOR_PRICE:
                    case MamaFieldDescriptor.COLLECTION:
                    case MamaFieldDescriptor.UNKNOWN:
                        mLogger.fine ("Property type " + field.getType () +
                                       " not supported.");
                        break;
                }

            }
        }
    }

    /**
     * Apply all the fields in the supplied list for which there are properties.
     * @param fields The fields to apply.
     */
    public void apply (MamaFieldCacheFieldList fields)
    {
        for (Iterator i = fields.getIterator (); i.hasNext ();)
        {
            MamaFieldCacheField field = (MamaFieldCacheField) i.next ();
            apply (field);
        }
    }

     /**
     * Dump the order book to standard out.
     */
    public void dump()
    {
        dump (System.out);
    }

    /**
     * Dump the properties to an <code>OutputStream</code>.
     * @param outputStream the <code>OutputStream</code>
     */
    public void dump (OutputStream outputStream)
    {
        PrintWriter out = new PrintWriter (outputStream, true);
        out.println ("Properties:");
        for (Iterator i = getIterator (); i.hasNext ();)
        {
            MamaFieldCacheField field = (MamaFieldCacheField) i.next ();
            out.print("\t");
            out.println (field.getDescriptor ().getName () + "=" +
                         field.getAsString ());            
        }
    }

    /**
     * So we know if a field has been added.
     *
     */
    private class Entry
    {
        public MamaFieldCacheField mField   = null;
        public long mSerial = 0;

        public Entry (MamaFieldCacheField field)
        {
            mField = field;
        }

        public Entry (MamaFieldCacheField field, long serial)
        {
            mField = field;
            mSerial = serial;
        }

        public Entry (Entry entry)
        {
            mField = entry.mField.copy ();
            mSerial = entry.mSerial;
        }
    }
}
