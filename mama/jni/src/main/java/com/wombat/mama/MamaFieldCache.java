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
import com.wombat.common.WombatException;

public class MamaFieldCache extends MamaFieldCacheFieldList
{
    /* If set then field names will be used when calling getDeltaMsg or getFullMsg. This
     * flag will be set to false by default.
     */
    private boolean mUseFieldNames;
        
    private boolean mTrackModState;
    private static boolean mOverrideDescTrackModState = true; 

    private FieldCacheMsgIterator complexIter = new FieldCacheMsgIterator();
    private FieldCacheBasicMsgIterator basicIter = new FieldCacheBasicMsgIterator();

    
    public MamaFieldCache()
    {
        mUseFieldNames = false;
        mTrackModState = false;
    }
    
    /*
     *  Description :   This function will return an array that contains all deltas
     *                  since the last time this or getDeltaMsg was called.
     *  Returns     :   The array of cache fields.
     */
    public MamaFieldCacheReadOnlyField[] getFastReadOnlyDeltaArray()
    {
        // Obtain the field list from the base class
        ArrayList fieldList = super.getArrayList();

        // Create a new return array list, the field list is the maximum size
        ArrayList returnList = new ArrayList(fieldList.size());

        // Enumerate the entire array
        for(int count=0; count<fieldList.size(); count++)
        {
            // Get the next field in the array
            MamaFieldCacheField nextField = (MamaFieldCacheField)fieldList.get(count);

            // The field should only be added if it has changed
            if(nextField.getModState() != MamaFieldCacheField.MOD_STATE_NOT_MODIFIED)
            {
                // Add this field to the return array
                returnList.add((MamaFieldCacheReadOnlyField)nextField);

                // Clear the modified state for the field
                nextField.clearModState();
            }
        }

        // Create the return array
        MamaFieldCacheReadOnlyField[] ret = new MamaFieldCacheReadOnlyField[returnList.size()];

        /* Obtain the array from the array list. */
        returnList.toArray(ret);

        return ret;
    }

    /*
     *  Description :   This function will fill an array all of the fields contained
     *                  in the cache.
     *  Returns     :   The array of cache fields.
     */
    public MamaFieldCacheReadOnlyField[] getFastReadOnlyFullArray()
    {
        // Obtain the field list from the base class
        ArrayList fieldList = super.getArrayList();

        // Create the return array
        MamaFieldCacheReadOnlyField[] ret = new MamaFieldCacheReadOnlyField[fieldList.size()];

        /* Obtain the array from the array list. */
        fieldList.toArray(ret);

        return ret;
    }
    
    /*
     *  Description :   This function will create a message that contains all deltas
     *                  since the last time it was called.
     *                  Note that field names will be included if the mUseFieldNames flag is
     *                  set, control this flag using the setUseFieldNames function.
     *  Arguments   :   message        [I] The message to populate with fields.
     */
    public void getDeltaMsg(MamaMsg message)
    {
        // Obtain the field list from the base class
        ArrayList fieldList = super.getArrayList();

        // Enumerate the entire array
        for(int count=0; count<fieldList.size(); count++)
        {
            // Get the next field in the array
            MamaFieldCacheField nextField = (MamaFieldCacheField)fieldList.get(count);

            // The field should only be added if it has changed
            if(nextField.getModState() != MamaFieldCacheField.MOD_STATE_NOT_MODIFIED)
            {
                // Add this field to the message
                nextField.addToMessage(mUseFieldNames, message);

                // Clear the modified state for the field
                nextField.clearModState();
            }
        }
    }

    /*
     *  Description :   This function will fill a message with all of the fields contained
     *                  in the cache.
     *                  Note that field names will be included if the mUseFieldNames flag is
     *                  set, control this flag using the setUseFieldNames function.
     *  Arguments   :   message        [I] The message to populate with fields.
     */
    public void getFullMsg(MamaMsg message)
    {
        // Obtain the field list from the base class
        ArrayList fieldList = super.getArrayList();
        
        // Enumerate the entire array
        for(int count=0; count<fieldList.size(); count++)
        {
            // Get the next field in the array
            MamaFieldCacheField nextField = (MamaFieldCacheField)fieldList.get(count);

            // Add this field to the message
            nextField.addToMessage(mUseFieldNames, message);
        }
    }
   
    /*
     *  Description :   Returns the flag indicating whether field names will be used when calling
     *                  getDeltaMsg and getFullMsg.
     *  Returns     :   The flag as a bool.
     */    
    public boolean getUseFieldNames()
    {
        return mUseFieldNames;
    }

    /*
     *  Description :   Sets the flag indicating whether field names will be used when calling
     *                  getDeltaMsg and getFullMsg.
     *  Arguments   :   useFieldNames [I] The flag as a bool.
     */    
    public void setUseFieldNames(boolean useFieldNames)
    {
        mUseFieldNames = useFieldNames;
    }

    class FieldCacheBasicMsgIterator implements MamaMsgFieldIterator
    {
        private MamaFieldCacheFieldList mDelta;
        private boolean fieldHasName;
        
        public FieldCacheBasicMsgIterator()
        {
            mDelta = null;
            fieldHasName = true;
        }
        
        public FieldCacheBasicMsgIterator(MamaFieldCacheFieldList delta)
        {
            mDelta = delta;
            fieldHasName = true;
        }
        
        public void setDelta(MamaFieldCacheFieldList delta)
        {
            mDelta = delta;
        }
                
        public void onField
          (MamaMsg msg, MamaMsgField field, MamaDictionary dict, Object closure)
        {        
            short type  = MamaFieldDescriptor.UNKNOWN;
            String name = null;
            int fid     = 0;
           
            type = field.getType ();
            fid  = field.getFid ();
            
            try 
            {
                if (fieldHasName)
                {
                    name = field.getName ();
                }    
                    
            }
            catch (WombatException exception)
            {
                // One possibility of this exception is trying to call getName on
                // non-basic messages.
                fieldHasName = false;
                //return;
            }
            
            MamaFieldCache cache = (MamaFieldCache)closure;
            switch(type)
            {
                case MamaFieldDescriptor.MSG:
                    break;
                    
                case MamaFieldDescriptor.OPAQUE:
                    break;
                    
                case MamaFieldDescriptor.STRING:
                    MamaFieldCacheString cachedStringField = 
                        (MamaFieldCacheString)cache.findOrAddString (fid,name);
                    if (cachedStringField != null)
                    {
                        cachedStringField.set(field.getString());
                        if (mDelta!=null && cachedStringField.isModified())
                            mDelta.add (cachedStringField);
                    }
                    break;
                
                case MamaFieldDescriptor.BOOL:
                    MamaFieldCacheBool cachedBoolField =  
                            (MamaFieldCacheBool)cache.findOrAddBool (fid,name);
                    if (cachedBoolField != null)
                    {
                        cachedBoolField.set(field.getBoolean());
                        if (mDelta!=null && cachedBoolField.isModified())
                            mDelta.add (cachedBoolField);
                    }
                    break;
                    
                case MamaFieldDescriptor.CHAR:
                    MamaFieldCacheChar  cachedCharField = 
                           (MamaFieldCacheChar)cache.findOrAddChar (fid, name);
                    if (cachedCharField != null)
                    {
                        cachedCharField.set (field.getChar());
                        if (mDelta!=null && cachedCharField.isModified())
                            mDelta.add (cachedCharField);
                    }   
                    break;
                    
                case MamaFieldDescriptor.I8:                
                    MamaFieldCacheInt8  cachedInt8Field = 
                           (MamaFieldCacheInt8)cache.findOrAddInt8 (fid, name);
                    if (cachedInt8Field != null)
                    {                        
                        cachedInt8Field.set (field.getI8());
                        if (mDelta !=null && cachedInt8Field.isModified())
                            mDelta.add (cachedInt8Field);            
                    }
                    break; 
                    
                case MamaFieldDescriptor.U8:                   
                    MamaFieldCacheUint8  cachedUint8Field = 
                         (MamaFieldCacheUint8)cache.findOrAddUint8 (fid, name);                    
                    if (cachedUint8Field != null)
                    {                     
                        cachedUint8Field.set (field.getU8());                                                                     
                        if (mDelta !=null && cachedUint8Field.isModified())
                            mDelta.add (cachedUint8Field);            
                    }                    
                    break;   
                    
                case MamaFieldDescriptor.I16:
                    MamaFieldCacheInt16  cachedInt16Field = 
                          (MamaFieldCacheInt16)cache.findOrAddInt16 (fid, name);
                    if (cachedInt16Field != null)
                    {                        
                        cachedInt16Field.set (field.getI16());
                        if (mDelta !=null && cachedInt16Field.isModified())
                            mDelta.add (cachedInt16Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.U16:
                    MamaFieldCacheUint16  cachedUint16Field = 
                       (MamaFieldCacheUint16)cache.findOrAddUint16 (fid, name);
                    if (cachedUint16Field != null)
                    {                        
                        cachedUint16Field.set (field.getU16());
                        if (mDelta !=null && cachedUint16Field.isModified())
                            mDelta.add (cachedUint16Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.I32:
                    MamaFieldCacheInt32  cachedInt32Field = 
                         (MamaFieldCacheInt32)cache.findOrAddInt32 (fid, name);
                    if (cachedInt32Field != null)
                    {
                        cachedInt32Field.set (field.getI32());
                        if (mDelta !=null && cachedInt32Field.isModified())
                            mDelta.add (cachedInt32Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.U32:
                    MamaFieldCacheUint32  cachedUint32Field = 
                       (MamaFieldCacheUint32)cache.findOrAddUint32 (fid, name);
                    if (cachedUint32Field != null)
                    {                        
                        cachedUint32Field.set (field.getU32());
                        if (mDelta !=null && cachedUint32Field.isModified())
                            mDelta.add (cachedUint32Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.I64:
                    MamaFieldCacheInt64  cachedInt64Field = 
                         (MamaFieldCacheInt64)cache.findOrAddInt64 (fid, name);
                    if (cachedInt64Field != null)
                    {
                        cachedInt64Field.set (field.getI64());
                        if (mDelta !=null && cachedInt64Field.isModified())
                            mDelta.add (cachedInt64Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.U64:                    
                    MamaFieldCacheUint64  cachedUint64Field = 
                       (MamaFieldCacheUint64)cache.findOrAddUint64 (fid, name);
                    if (cachedUint64Field != null)
                    {
                        cachedUint64Field.set (field.getU64());                        
                        if (mDelta !=null && cachedUint64Field.isModified())
                        {                            
                            mDelta.add (cachedUint64Field);            
                        }
                    }
                    break;
                    
                case MamaFieldDescriptor.F32:
                    MamaFieldCacheFloat32  cachedFloat32Field = 
                     (MamaFieldCacheFloat32)cache.findOrAddFloat32 (fid, name);
                    if (cachedFloat32Field != null)
                    {
                        cachedFloat32Field.set (field.getF32());
                        if (mDelta !=null && cachedFloat32Field.isModified())
                            mDelta.add (cachedFloat32Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.F64:
                    MamaFieldCacheFloat64  cachedFloat64Field = 
                      (MamaFieldCacheFloat64)cache.findOrAddFloat64 (fid, name);
                    if (cachedFloat64Field != null)
                    {
                        cachedFloat64Field.set (field.getF64());
                        if (mDelta !=null && cachedFloat64Field.isModified())
                            mDelta.add (cachedFloat64Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.TIME:
                    MamaFieldCacheDateTime  cachedDateTimeField = 
                    (MamaFieldCacheDateTime)cache.findOrAddDateTime (fid, name);
                    if (cachedDateTimeField != null)
                    {
                        cachedDateTimeField.set (field.getDateTime());
                        if (mDelta !=null && cachedDateTimeField.isModified())
                            mDelta.add (cachedDateTimeField);            
                    }
                    break;
                        
                case MamaFieldDescriptor.PRICE:
                    MamaFieldCachePrice  cachedPriceField = 
                         (MamaFieldCachePrice)cache.findOrAddPrice (fid, name);
                    if (cachedPriceField != null)
                    {
                        cachedPriceField.set (field.getPrice());
                        if (mDelta !=null && cachedPriceField.isModified())
                            mDelta.add (cachedPriceField);            
                    }
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
                    break;
            }
        }    
    }

    class FieldCacheMsgIterator implements MamaMsgFieldIterator
    {
        private MamaFieldCacheFieldList mDelta;

        public FieldCacheMsgIterator()
        {
            mDelta = null;
        }
        
        public FieldCacheMsgIterator(MamaFieldCacheFieldList delta)
        {
            mDelta = delta;
        }
        
        public void setDelta(MamaFieldCacheFieldList delta)
        {
            mDelta = delta;
        }
        
        public void onField
          (MamaMsg msg, MamaMsgField field, MamaDictionary dict, Object closure)
        {  
            // get this NOW - we'll need it for sure, but only get the field name
            // when you have to, ie, on initialization of the fieldCacheField.
            int fid = field.getFid();

            MamaFieldCache cache = (MamaFieldCache)closure;

            switch(field.getType())
            {
                case MamaFieldDescriptor.MSG:
                    break;

                case MamaFieldDescriptor.OPAQUE:
                    break;
                    
                case MamaFieldDescriptor.STRING: 
                    MamaFieldCacheString cachedStringField = 
                             (MamaFieldCacheString)cache.find(fid);

                    if (cachedStringField == null)
                    {
                        cachedStringField = (MamaFieldCacheString)cache.
                            findOrAddString(fid, field.getName());      
                    }

                    if (cachedStringField != null)
                    {
                        cachedStringField.set(field.getString());
                        if (mDelta!=null && cachedStringField.isModified())
                            mDelta.add(cachedStringField);
                    }
                    break;
                
                case MamaFieldDescriptor.BOOL:                
                    MamaFieldCacheBool cachedBoolField =
                            (MamaFieldCacheBool) cache.find(fid);
                    if (cachedBoolField == null)
                    {
                        cachedBoolField = (MamaFieldCacheBool)cache.
                            findOrAddBool (fid, field.getName());
                    }
                    if (cachedBoolField != null)
                    {
                        cachedBoolField.set(field.getBoolean());
                        if (mDelta!=null && cachedBoolField.isModified())
                            mDelta.add (cachedBoolField);
                    }
                    break;
                    
                case MamaFieldDescriptor.CHAR:                
                    MamaFieldCacheChar  cachedCharField = (MamaFieldCacheChar) cache.find(fid);

                    if (cachedCharField == null)
                    {
                            cachedCharField = (MamaFieldCacheChar)cache.
                                findOrAddChar (fid, field.getName());
                    }
                    if (cachedCharField != null)
                    {
                        cachedCharField.set (field.getChar());
                        if (mDelta!=null && cachedCharField.isModified())
                            mDelta.add (cachedCharField);
                    }
                    break;
                    
                case MamaFieldDescriptor.I8:                         
                    MamaFieldCacheInt8  cachedInt8Field = (MamaFieldCacheInt8) cache.find(fid);

                    if (cachedInt8Field == null)
                    {
                        cachedInt8Field = (MamaFieldCacheInt8)cache.
                                findOrAddInt8 (fid, field.getName());
                    }
                    if (cachedInt8Field != null)
                    {
                        cachedInt8Field.set (field.getI8());
                        if (mDelta !=null && cachedInt8Field.isModified())
                            mDelta.add (cachedInt8Field);            
                    }
                    break;
                
                 case MamaFieldDescriptor.U8:                      
                    MamaFieldCacheUint8  cachedUint8Field = (MamaFieldCacheUint8) cache.find(fid);
                    if (cachedUint8Field == null)
                    {
                       cachedUint8Field = (MamaFieldCacheUint8)cache.
                           findOrAddUint8 (fid, field.getName());
                    }
                    if (cachedUint8Field != null)
                    {
                        cachedUint8Field.set (field.getU8());                        
                        if (mDelta !=null && cachedUint8Field.isModified())
                            mDelta.add (cachedUint8Field);            
                    }
                    break;   
                    
                case MamaFieldDescriptor.I16:                
                    MamaFieldCacheInt16  cachedInt16Field = (MamaFieldCacheInt16) cache.find(fid);
                    if (cachedInt16Field == null)
                        cachedInt16Field = (MamaFieldCacheInt16)cache.findOrAddInt16 (fid, field.getName());
                    if (cachedInt16Field != null)
                    {
                        cachedInt16Field.set (field.getI16());
                        if (mDelta !=null && cachedInt16Field.isModified())
                            mDelta.add (cachedInt16Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.U16:         
                    MamaFieldCacheUint16  cachedUint16Field = (MamaFieldCacheUint16) cache.find(fid);
                    if (cachedUint16Field == null)
                        cachedUint16Field = (MamaFieldCacheUint16)cache.findOrAddUint16 (fid, field.getName());
                    if (cachedUint16Field != null)
                    {
                        cachedUint16Field.set (field.getU16());
                        if (mDelta !=null && cachedUint16Field.isModified())
                            mDelta.add (cachedUint16Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.I32:
                    MamaFieldCacheInt32  cachedInt32Field = (MamaFieldCacheInt32) cache.find(fid);
                    if (cachedInt32Field == null)
                        cachedInt32Field = (MamaFieldCacheInt32)cache.findOrAddInt32 (fid, field.getName());
                    if (cachedInt32Field != null)
                    {
                        cachedInt32Field.set (field.getI32());
                        if (mDelta !=null && cachedInt32Field.isModified())
                            mDelta.add (cachedInt32Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.U32:
                    MamaFieldCacheUint32  cachedUint32Field = (MamaFieldCacheUint32) cache.find(fid);
                    if (cachedUint32Field == null)
                        cachedUint32Field = (MamaFieldCacheUint32)cache.findOrAddUint32 (fid, field.getName());
                    if (cachedUint32Field != null)
                    {
                        cachedUint32Field.set (field.getU32());
                        if (mDelta !=null && cachedUint32Field.isModified())
                            mDelta.add (cachedUint32Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.I64:
                    MamaFieldCacheInt64  cachedInt64Field = (MamaFieldCacheInt64) cache.find(fid);
                    if (cachedInt64Field == null)
                        cachedInt64Field = (MamaFieldCacheInt64)cache.findOrAddInt64 (fid, field.getName());
                    if (cachedInt64Field != null)
                    {
                        cachedInt64Field.set (field.getI64());
                        if (mDelta !=null && cachedInt64Field.isModified())
                            mDelta.add (cachedInt64Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.U64:
                    MamaFieldCacheUint64  cachedUint64Field = (MamaFieldCacheUint64) cache.find(fid);
                    if (cachedUint64Field == null)
                        cachedUint64Field = (MamaFieldCacheUint64)cache.findOrAddUint64 (fid, field.getName());
                    if (cachedUint64Field != null)
                    {
                        cachedUint64Field.set (field.getU64());
                        if (mDelta !=null && cachedUint64Field.isModified())
                            mDelta.add (cachedUint64Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.F32:                
                    MamaFieldCacheFloat32  cachedFloat32Field = (MamaFieldCacheFloat32) cache.find(fid);
                    if (cachedFloat32Field == null)
                        cachedFloat32Field = (MamaFieldCacheFloat32)cache.findOrAddFloat32 (fid, field.getName());
                    if (cachedFloat32Field != null)
                    {
                        cachedFloat32Field.set (field.getF32());
                        if (mDelta !=null && cachedFloat32Field.isModified())
                            mDelta.add (cachedFloat32Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.F64:               
                    MamaFieldCacheFloat64  cachedFloat64Field = 
                           (MamaFieldCacheFloat64)cache.findOrAddFloat64 (fid, field.getName());
                    if (cachedFloat64Field != null)
                    {
                        cachedFloat64Field.set (field.getF64());
                        if (mDelta !=null && cachedFloat64Field.isModified())
                            mDelta.add (cachedFloat64Field);            
                    }
                    break;
                    
                case MamaFieldDescriptor.TIME:
                    MamaFieldCacheDateTime cachedDateTimeField = (MamaFieldCacheDateTime) cache.find(fid);
                    if (cachedDateTimeField == null)
                         cachedDateTimeField = (MamaFieldCacheDateTime)cache.findOrAddDateTime (fid, field.getName());
                    if (cachedDateTimeField != null)
                    {
                        cachedDateTimeField.set (field.getDateTime());
                        if (mDelta !=null && cachedDateTimeField.isModified())
                            mDelta.add (cachedDateTimeField);            
                    }
                    break;
                        
                case MamaFieldDescriptor.PRICE:                
                    MamaFieldCachePrice  cachedPriceField = (MamaFieldCachePrice) cache.find(fid);
                    if (cachedPriceField == null)
                        cachedPriceField = (MamaFieldCachePrice)cache.findOrAddPrice (fid, field.getName());
                    if (cachedPriceField != null)
                    {
                        cachedPriceField.set (field.getPrice());
                        if (mDelta !=null && cachedPriceField.isModified())
                            mDelta.add (cachedPriceField);            
                    }
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
                    break; 
                
            }
        }
    }
    
    public void apply(MamaMsg msg,
                      MamaDictionary dict,
                      MamaFieldCacheFieldList delta)
    {
        if (delta!=null)
            delta.clear();
    
        if (dict!=null)
        {
            complexIter.setDelta(delta);
            msg.iterateFields (complexIter, dict, this);
        }
        else
        {
            basicIter.setDelta(delta);
            msg.iterateFields (basicIter,null, this);
        }

    }
    
    public MamaFieldCacheField add(MamaFieldDescriptor desc)
    {
        switch (desc.getType())
        {   
            case MamaFieldDescriptor.STRING:
                return findOrAddString(desc);
                                   
            case MamaFieldDescriptor.BOOL:
                return findOrAddBool(desc);
        
            case MamaFieldDescriptor.CHAR:
                return findOrAddChar(desc);
        
            case MamaFieldDescriptor.I8:
                return findOrAddInt8(desc);
        
            case MamaFieldDescriptor.U8:
                return findOrAddUint8(desc);
        
            case MamaFieldDescriptor.I16:
                return findOrAddInt16(desc);
        
            case MamaFieldDescriptor.U16:
                return findOrAddUint16(desc);
        
            case MamaFieldDescriptor.I32:
                return findOrAddInt32(desc);
        
            case MamaFieldDescriptor.U32:
                return findOrAddUint32(desc);
        
            case MamaFieldDescriptor.I64:
                return findOrAddInt64(desc);
        
            case MamaFieldDescriptor.U64:
                return findOrAddUint64(desc);
        
            case MamaFieldDescriptor.F32:
                return findOrAddFloat32(desc);
        
            case MamaFieldDescriptor.F64:
                return findOrAddFloat64(desc);
        
            case MamaFieldDescriptor.TIME:
            case MamaFieldDescriptor.DATETIME:
                return findOrAddDateTime(desc);
        
            case MamaFieldDescriptor.PRICE:
                return findOrAddPrice(desc);
            
            case MamaFieldDescriptor.MSG:
            case MamaFieldDescriptor.OPAQUE:
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
            
            return null;            
        }

        return null;
    
    }
    
    public MamaFieldCacheField findOrAdd(MamaFieldDescriptor desc)
    {    
        MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = add (desc);
        }
        return result;
    }
    
    public MamaFieldCacheField findOrAddBool(MamaFieldDescriptor desc)
    {
        MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheBool(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheBool)(result);
    }
    
    public MamaFieldCacheChar findOrAddChar(MamaFieldDescriptor desc)
    {
        MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheChar(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheChar)(result);
    }
    
    public MamaFieldCacheUint8 findOrAddUint8(MamaFieldDescriptor desc)
    {
        MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheUint8(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheUint8)(result);
    }
    
    public MamaFieldCacheInt8 findOrAddInt8(MamaFieldDescriptor desc)
    {
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheInt8(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheInt8)(result);
    }
    
    public MamaFieldCacheUint16 findOrAddUint16(MamaFieldDescriptor desc)
    {
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheUint16(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheUint16)(result);
    }
    
    public MamaFieldCacheInt16 findOrAddInt16(MamaFieldDescriptor desc)
    {
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheInt16(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheInt16)(result);
    }
    
    public MamaFieldCacheUint32 findOrAddUint32(MamaFieldDescriptor desc)
    {
    
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheUint32(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheUint32)(result);
    }
    
    public MamaFieldCacheInt32 findOrAddInt32(MamaFieldDescriptor desc)
    {
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheInt32(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheInt32)(result);
    }
    
    public MamaFieldCacheUint64 findOrAddUint64(MamaFieldDescriptor desc)
    {
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheUint64(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheUint64)(result);
    }
    
    public MamaFieldCacheInt64 findOrAddInt64(MamaFieldDescriptor desc)
    {
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheInt64(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheInt64)(result);
    }
    
    public MamaFieldCacheFloat32 findOrAddFloat32(MamaFieldDescriptor desc)
    {
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheFloat32(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheFloat32)(result);
    }
    
    public MamaFieldCacheFloat64 findOrAddFloat64(MamaFieldDescriptor desc)
    {
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheFloat64(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheFloat64)(result);
    }
    
    public MamaFieldCacheDateTime findOrAddDateTime(MamaFieldDescriptor desc)
    {
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCacheDateTime(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheDateTime)(result);
    }
    
    public MamaFieldCachePrice findOrAddPrice(MamaFieldDescriptor desc)
    {
       MamaFieldCacheField result = find(desc);
        if (result == null)
        {
            result = new MamaFieldCachePrice(desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCachePrice)(result);
    }   
    
    public MamaFieldCacheString findOrAddString(MamaFieldDescriptor desc)
    {        
        MamaFieldCacheField result = find (desc);
        if (result == null)
        {
            result = new MamaFieldCacheString (desc);
            if (mOverrideDescTrackModState)
            {
                result.setTrackModState (mTrackModState);
            }
            add (result);
        }
        return (MamaFieldCacheString)(result);
    }
    
    //check type for "type" parameter
    public MamaFieldCacheField add (int fid,short type,String name)
    {
        switch (type)
        {
        case MamaFieldDescriptor.MSG:
           return null;

        case MamaFieldDescriptor.OPAQUE:
            return null;

        case MamaFieldDescriptor.STRING:
            return findOrAddString (fid, name);

        case MamaFieldDescriptor.BOOL:
            return findOrAddBool (fid, name);

        case MamaFieldDescriptor.CHAR:
            return findOrAddChar (fid, name);

        case MamaFieldDescriptor.I8:
            return findOrAddInt8 (fid, name);

        case MamaFieldDescriptor.U8:
            return findOrAddUint8 (fid, name);

        case MamaFieldDescriptor.I16:
            return findOrAddInt16 (fid, name);

        case MamaFieldDescriptor.U16:
            return findOrAddUint16 (fid, name);

        case MamaFieldDescriptor.I32:
            return findOrAddInt32 (fid, name);

        case MamaFieldDescriptor.U32:
            return findOrAddUint32 (fid, name);

        case MamaFieldDescriptor.I64:
            return findOrAddInt64 (fid, name);

        case MamaFieldDescriptor.U64:
            return findOrAddUint64 (fid, name);
    
        case MamaFieldDescriptor.F32:
            return findOrAddFloat32 (fid, name);

        case MamaFieldDescriptor.F64:
            return findOrAddFloat64 (fid, name);

        case MamaFieldDescriptor.TIME:
            return findOrAddDateTime (fid, name);

        case MamaFieldDescriptor.PRICE:
            return findOrAddPrice (fid, name);
       
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
            return null;
        }

    return null;
    }   

    public MamaFieldCacheField findOrAdd (int fid,short type,String name)
    {
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = add (fid, type, name);
        }
        return result;
    }
    
    public MamaFieldCacheField findOrAddBool(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheBool (fid, name, mTrackModState);
            add (result);            
        }
        return (MamaFieldCacheBool)result;
    }
    
    public MamaFieldCacheField findOrAddChar(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheChar (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheChar)result;
    }
    
    public MamaFieldCacheField findOrAddUint8(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheUint8 (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheUint8)result;  
    }
    
    public MamaFieldCacheField findOrAddInt8(int fid, String name)
    {
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheInt8 (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheInt8)result;
    }
    
    public MamaFieldCacheField findOrAddUint16(int fid, String name)
    {
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheUint16 (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheUint16)result;
    }
    
    public MamaFieldCacheField findOrAddInt16(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheInt16 (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheInt16)result;
    }
    
    public MamaFieldCacheField findOrAddUint32(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheUint32 (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheUint32)result;
    }
    
    public MamaFieldCacheField findOrAddInt32(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheInt32 (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheInt32)result; 
    }
    
    public MamaFieldCacheField findOrAddUint64(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheUint64 (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheUint64)result; 
    }
    
    public MamaFieldCacheField findOrAddInt64(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheInt64 (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheInt64)result;   
    }
    
    public MamaFieldCacheField findOrAddFloat32(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheFloat32 (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheFloat32)result;
    }
    
    public MamaFieldCacheField findOrAddFloat64(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCacheFloat64 (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheFloat64)result;
    }
    
    public MamaFieldCacheField findOrAddDateTime(int fid, String name)
    {      
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {            
            result = new MamaFieldCacheDateTime (fid, name, mTrackModState);
            add(result);
        }        
        return (MamaFieldCacheDateTime)result;    
    }
    
    public MamaFieldCacheField findOrAddPrice(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result == null)
        {
            result = new MamaFieldCachePrice (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCachePrice)result;
    }   
    
    public MamaFieldCacheField findOrAddString(int fid, String name)
    {        
        MamaFieldCacheField result = find (fid);
        if (result==null)
        {
            result = new MamaFieldCacheString (fid, name, mTrackModState);
            add(result);
        }
        return (MamaFieldCacheField)result;
    }
    /*
    public MamaFieldCacheField findOrAddEnum(int fid, String name)
    {
        MamaFieldCacheField result = find(fid);
        if (result==null)
        {
            result = new MamaFieldCacheUint8(fid,name,mTrackModState);            
            add (result);
        }
        return (MamaFieldCacheUint8)(result);
    }
    
    public MamaFieldCacheField findOrAddMultiEnum(int fid, String name)
    {
        MamaFieldCacheField result = find(desc);
        if (result==null)
        {
            result = new MamaFieldCacheUint8(fid,name,mTrackModState);            
            add (result);
        }
        return (MamaFieldCacheUint8)(result);
    }*/
    
    public void lockFields()
    {
    }
    
    public void unlockFields()
    {
    
    }
    
    public void setTrackModState(boolean state)
    {
        mTrackModState = state;
    }
    
    public boolean getTrackModState()
    {
        return mTrackModState;
    }    
    
    public void setOverrideDescriptorTrackModState(boolean state)
    {
        mOverrideDescTrackModState = state;
    }
    
    public boolean getOverrideDescriptorTrackModState()
    {
        return  mOverrideDescTrackModState;
    }
}
