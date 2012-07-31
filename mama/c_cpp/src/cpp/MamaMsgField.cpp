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

#include <mama/mamacpp.h>
#include "mamacppinternal.h"
#include "MsgFieldImpl.h"

namespace Wombat 
{

    MamaMsgField::~MamaMsgField () 
    {
        clear();
    }

    MamaMsgField::MamaMsgField (void)
        : mField               (NULL)
        , mFieldDesc           (NULL)
        , mLastVectorMsg       (NULL)
        , mLastVectorMsgLen    (0)
    {
    }

    MamaMsgField::MamaMsgField (mamaMsgField field)
        : mField               (field)
        , mFieldDesc           (NULL)
        , mLastVectorMsg       (NULL)
        , mLastVectorMsgLen    (0)
    {
    }

    void MamaMsgField::clear ()
    {
        if (mFieldDesc)
        {
            mFieldDesc = NULL;
        }
        
        if (mLastVectorMsg != NULL)
        {
            destroyLastVectorMsg ();
            mLastVectorMsg    = NULL;
            mLastVectorMsgLen = 0;
        }
    }

    void MamaMsgField::set (mamaMsgField field)
    {
        clear ();
        mField = field;
    }

    const MamaFieldDescriptor* MamaMsgField::getDescriptor () const
    {
        void* closureData = NULL; 
       
        /* Reuse the same descriptor if this method is called multiple
          times on the same MamaMsgField usage instance */
        if (!mFieldDesc)
        {
            mamaFieldDescriptor desc = NULL;
            mamaTry (mamaMsgField_getDescriptor (mField, &desc));
            if (desc)
            {
                /* If this FD has been obtained previously we will have
                 stored the C++ wrapper as the closure in C */
                mamaFieldDescriptor_getClosure (desc, &closureData);
                if (closureData)
                {
                    mFieldDesc = (MamaFieldDescriptor*)closureData;
                }
                else
                {
                    /* Store the C++ wrapper in the C closure for 
                     subsequent use */
                    mFieldDesc = new MamaFieldDescriptor (desc);
                    mamaFieldDescriptor_setClosure (desc, (void*)mFieldDesc);
                }
            }
        }
        return mFieldDesc;
    }

    mama_fid_t MamaMsgField::getFid () const
    {
        mama_fid_t result = 0;
        mamaTry (mamaMsgField_getFid (mField, &result));
        return result;
    }

    const char* MamaMsgField::getName () const
    {
        const char* result = NULL;
        mamaTry (mamaMsgField_getName (mField, &result));
        return result;
    }

    mamaFieldType MamaMsgField::getType () const
    {
        mamaFieldType result = MAMA_FIELD_TYPE_UNKNOWN;
        mamaTry (mamaMsgField_getType (mField, &result));
        return result;
    }
        
    const char *MamaMsgField::getTypeName () const
    {
        const char *result = NULL;
        mamaTry (mamaMsgField_getTypeName (mField, &result));
        return result;
    }

    mama_bool_t MamaMsgField::getBool () const
    {
        mama_bool_t result = 0;
        mamaTry (mamaMsgField_getBool (mField, &result));
        return result;
    }

    char MamaMsgField::getChar () const
    {
        char result = ' ';
        mamaTry (mamaMsgField_getChar (mField, &result));
        return result;
    }

    int8_t MamaMsgField::getI8 () const
    {
        int8_t result = 0;
        mamaTry (mamaMsgField_getI8 (mField, &result));
        return result;
    }

    uint8_t MamaMsgField::getU8 () const
    {
        uint8_t result = 0;
        mamaTry (mamaMsgField_getU8 (mField, &result));
        return result;
    }

    int16_t MamaMsgField::getI16 () const
    {
        int16_t result = 0;
        mamaTry (mamaMsgField_getI16 (mField, &result));
        return result;
    }

    uint16_t MamaMsgField::getU16 () const
    {
        uint16_t result = 0;
        mamaTry (mamaMsgField_getU16 (mField, &result));
        return result;
    }

    int32_t MamaMsgField::getI32 () const
    {
        int32_t result = 0;
        mamaTry (mamaMsgField_getI32 (mField, &result));
        return result;
    }

    uint32_t MamaMsgField::getU32 () const
    {
        uint32_t result = 0;
        mamaTry (mamaMsgField_getU32 (mField, &result));
        return result;
    }

    int64_t MamaMsgField::getI64 () const
    {
        int64_t result = 0;
        mamaTry (mamaMsgField_getI64 (mField, &result));
        return result;
    }

    uint64_t MamaMsgField::getU64 () const
    {
        uint64_t result = 0;
        mamaTry (mamaMsgField_getU64 (mField, &result));
        return result;
    }

    mama_f32_t MamaMsgField::getF32 () const
    {
        mama_f32_t result = 0.0f;
        mamaTry (mamaMsgField_getF32 (mField,&result));
        return result;
    }   
        
    mama_f64_t MamaMsgField::getF64 () const
    {
        mama_f64_t result = 0.0;
        mamaTry (mamaMsgField_getF64 (mField,&result));
        return result;
    }

    const char* MamaMsgField::getString () const
    {
        const char* result = NULL;
        mamaTry (mamaMsgField_getString (mField, &result));
        return result;
    }

    const void* MamaMsgField::getOpaque (size_t& size) const
    {
        const void* result = NULL;
        mamaTry (mamaMsgField_getOpaque (mField, &result, &size));
        return result;
    }

    void MamaMsgField::getPrice (MamaPrice& result) const
    {
        mamaTry (mamaMsgField_getPrice (mField, result.getCValue()));
    }

    void MamaMsgField::getMsg (MamaMsg& result) const
    {
        mamaMsg msg;
        mamaTry (mamaMsgField_getMsg (mField, &msg));
        result.createFromMsg (msg, false);
    }

    void MamaMsgField::getDateTime (MamaDateTime& result) const
    {
        mamaTry (mamaMsgField_getDateTime (mField, result.getCValue()));
    }

    void MamaMsgField::getVectorChar (const char*&       result,
                                      size_t&            resultLen) const
    {
        mamaTry (mamaMsgField_getVectorChar (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorI8  (const int8_t*&      result,
                                     size_t&             resultLen) const
    {
        mamaTry (mamaMsgField_getVectorI8 (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorU8  (const uint8_t*&     result,
                                     size_t&             resultLen) const
    {
        mamaTry (mamaMsgField_getVectorU8 (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorI16 (const int16_t*&     result,
                                     size_t&             resultLen) const
    {
        mamaTry (mamaMsgField_getVectorI16 (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorU16 (const uint16_t*&    result,
                                     size_t&             resultLen) const
    {
        mamaTry (mamaMsgField_getVectorU16 (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorI32 (const int32_t*&     result,
                                     size_t&             resultLen) const
    {
        mamaTry (mamaMsgField_getVectorI32 (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorU32 (const uint32_t*&    result,
                                     size_t&             resultLen) const
    {
        mamaTry (mamaMsgField_getVectorU32 (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorI64 (const int64_t*&     result,
                                     size_t&             resultLen) const
    {
        mamaTry (mamaMsgField_getVectorI64 (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorU64 (const uint64_t*&    result,
                                     size_t&             resultLen) const
    {
        mamaTry (mamaMsgField_getVectorU64 (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorF32 (const mama_f32_t*&  result,
                                     size_t&             resultLen) const
    {
        mamaTry (mamaMsgField_getVectorF32 (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorF64 (const mama_f64_t*&  result,
                                     size_t&             resultLen) const
    {
        mamaTry (mamaMsgField_getVectorF64 (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorString (const char**&    result,
                                        size_t&          resultLen) const
    {
        mamaTry (mamaMsgField_getVectorString (mField, &result, &resultLen));
    }

    void MamaMsgField::getVectorMsg (const MamaMsg**&    result,
                                     size_t&             resultLen) const
    {
        const mamaMsg*  tmpResult = NULL;

        if (mLastVectorMsg != NULL)
        {
            destroyLastVectorMsg ();
        }

        mamaTry (mamaMsgField_getVectorMsg (mField, &tmpResult, &resultLen));

        result             = (const MamaMsg**)calloc (resultLen,sizeof(MamaMsg*));
        mLastVectorMsg     = result;
        mLastVectorMsgLen  = resultLen;

        for (size_t i = 0; i < resultLen; i++)
        {
            result[i] = new MamaMsg;
            result[i]->createFromMsg ((mamaMsg)tmpResult[i], false);
            mLastVectorMsg[i] = result[i];
        }
    }

    void MamaMsgField::getVectorMsgDetached (const MamaMsg**&  result,
                                             size_t&           resultLen) const
    {
        const mamaMsg*  tmpResultC = NULL;
        result                     = NULL;

     //   mamaTry (mamaMsgField_getVectorMsgDetached (mField, 
       //                                             &tmpResultC, 
         //                                           &resultLen));

        result = (const MamaMsg**)calloc (resultLen,sizeof(MamaMsg*));

        for (size_t i = 0; i < resultLen; i++)
        {
            result[i] = new MamaMsg;
            result[i]->createFromMsg (tmpResultC[i], true);
        }
        free ((void*)tmpResultC);
    }

    void MamaMsgField::getAsString (char*        result,
                                    size_t       maxResultLen) const
    {
        mamaMsgField_getAsString (mField, result, maxResultLen);
    }

    void MamaMsgField::updateBool (mama_bool_t value)
    {
        mamaTry (mamaMsgField_updateBool (mField, value));
    }

    void MamaMsgField::updateChar (char value)
    {
        mamaTry (mamaMsgField_updateChar (mField, value));
    }

    void MamaMsgField::updateI8 (mama_i8_t value)
    {
        mamaTry (mamaMsgField_updateI8 (mField, value));
    }

    void MamaMsgField::updateU8 (mama_u8_t value)
    {
        mamaTry (mamaMsgField_updateU8 (mField, value));
    }

    void MamaMsgField::updateI16 (mama_i16_t value)
    {
        mamaTry (mamaMsgField_updateI16 (mField, value));
    }

    void MamaMsgField::updateU16 (mama_u16_t value)
    {
        mamaTry (mamaMsgField_updateU16 (mField, value));
    }

    void MamaMsgField::updateI32 (mama_i32_t value)
    {
        mamaTry (mamaMsgField_updateI32 (mField, value));
    }

    void MamaMsgField::updateU32 (mama_u32_t value)
    {
        mamaTry (mamaMsgField_updateU32 (mField, value));
    }

    void MamaMsgField::updateI64 (mama_i64_t value)
    {
        mamaTry (mamaMsgField_updateI64 (mField, value));
    }

    void MamaMsgField::updateU64 (mama_u64_t value)
    {
        mamaTry (mamaMsgField_updateU64 (mField, value));
    }

    void MamaMsgField::updateF32 (mama_f32_t value)
    {
        mamaTry (mamaMsgField_updateF32 (mField, value));
    }

    void MamaMsgField::updateF64 (mama_f64_t value)
    {
        mamaTry (mamaMsgField_updateF64 (mField, value));
    }

    void MamaMsgField::updateDateTime (const mamaDateTime value) 
    {
        mamaTry (mamaMsgField_updateDateTime (mField, value));
    }

    void MamaMsgField::updateDateTime (const MamaDateTime value) 
    {   
        mamaTry (mamaMsgField_updateDateTime (mField, value.getCValue()));
    }

    void MamaMsgField::updatePrice (const mamaPrice value)
    {
        mamaTry (mamaMsgField_updatePrice (mField, value));
    }

    void MamaMsgField::updatePrice (const MamaPrice value)
    {
        mamaTry (mamaMsgField_updatePrice (mField, value.getCValue()));
    }


    void MamaMsgField::destroyLastVectorMsg() const
    {
        for (size_t count = 0; count < mLastVectorMsgLen; count++)
        {
            if (mLastVectorMsg[count] != NULL)
            {
                delete mLastVectorMsg[count];
            }
        }
        free (mLastVectorMsg);
        mLastVectorMsg    = NULL;
        mLastVectorMsgLen = 0;
    }

    bool  MamaMsgField::operator==(const MamaMsgField& rhs) const
    {
        return (mField == rhs.mField);
    }

    bool MamaMsgField::operator!=(const MamaMsgField& rhs) const
    {
    return (mField != rhs.mField);
    }

} // namespace Wombat
