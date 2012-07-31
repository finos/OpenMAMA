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

#include "mama/mama.h"
#include "mama/mamacpp.h"
#include "mama/log.h"
#include "MsgFieldImpl.h"

namespace Wombat 
{
    /******************************************************************************
     * MamaMsg Methods
     */
        
    MamaMsg::~MamaMsg () 
    {
        cleanup();
        /* The reuseable field does not get destroyed in cleanup() */
        if (mMsgField)
        {
            delete mMsgField;
            mMsgField = NULL;
        }
    }

    MamaMsg::MamaMsg (void) 
        : mMsg                  (NULL)
        , mDestroy              (true)
        , mVectorMsg            (NULL)
        , mVectorMsgSize        (0)
        , mVectorMsgAllocSize   (0)
        , mCvectorMsg           (NULL)
        , mCvectorMsgAllocSize  (0)
        , mTmpMsg               (NULL)
        , mString               (NULL)
        , mMsgField             (new MamaMsgField) 
    {
    }

    MamaMsg::MamaMsg (const MamaMsg&  copy) 
        : mMsg                  (NULL)
        , mDestroy              (true)
        , mVectorMsg            (NULL)
        , mVectorMsgSize        (0)
        , mVectorMsgAllocSize   (0)
        , mCvectorMsg           (NULL)
        , mCvectorMsgAllocSize  (0)
        , mTmpMsg               (NULL)
        , mString               (NULL)
        , mMsgField             (new MamaMsgField)
    {
        this->copy (copy);
    }

    void MamaMsg::create (void)
    {
        mamaTry (mamaMsg_create (&mMsg));
    }

    void MamaMsg::createFromBuffer (
        const void*  buffer,
        size_t       bufferLength)
    {
        mamaTry (mamaMsg_createFromByteBuffer (&mMsg,buffer,bufferLength));
    }

    void MamaMsg::createForPayload (const char id)
    {
        mamaMsg_createForPayload (&mMsg, id);
    }

    void MamaMsg::createForPayloadBridge (mamaPayloadBridge payloadBridge)
    {
        if (mMsg && mDestroy)
        {
            mamaMsg_destroy(mMsg);
        }
        
        mamaMsg_createForPayloadBridge  (&mMsg,
                                  payloadBridge
                                  );
    }

    void MamaMsg::createFromMsg (
        mamaMsg  msg,
        bool     destroyMsg) const
    {
        if (mMsg && mDestroy)
            mamaMsg_destroy (mMsg);

        mMsg = msg;
        mDestroy = destroyMsg;
    }

    MamaMsg* MamaMsg::detach (void)
    {
        mamaTry (mamaMsg_detach(mMsg));
        MamaMsg* result = new MamaMsg();
        result->createFromMsg (mMsg, true);
        return result;
    }

    void MamaMsg::copy (const MamaMsg&  rhs)
    {
        if (mMsg) mamaTry (mamaMsg_clear (mMsg));
        mamaTry (mamaMsg_copy (rhs.mMsg, &mMsg));
    }

    void MamaMsg::clear (void)
    {
        if (mMsg) mamaTry (mamaMsg_clear (mMsg));

        mDestroy = true;

        if (mCvectorMsg)
        {
            for (size_t i = 0; i < mCvectorMsgAllocSize; ++i)
            {
                mamaMsg msg = NULL;
                msg = mCvectorMsg[i];
                mamaTry (mamaMsg_clear (msg));
            }
            delete [] mCvectorMsg;
            mCvectorMsg = NULL;
            mCvectorMsgAllocSize = 0;
        }
    }

    void MamaMsg::setMsg (mamaMsg msg)
    {
        cleanup ();
        mVectorMsgSize = 0;
        mMsg = msg;
    }

    const mamaMsg& MamaMsg::getUnderlyingMsg (void) const
    {
        return mMsg;
    }

    mamaMsg MamaMsg::getUnderlyingMsg (void)
    {
        return mMsg;
    }

    mamaPayloadType MamaMsg::getPayloadType (void)
    {
        mamaPayloadType result;    
        mamaTry (mamaMsg_getPayloadType (mMsg, &result));

        return result;
    }
        
    void*  MamaMsg::getNativeMsg (void)
    {
        void*  result;
        mamaTry (mamaMsg_getNativeMsg (mMsg, &result));

        return result;
    }

    size_t MamaMsg::getNumFields (void) const
    {
        size_t result;
        mamaTry (mamaMsg_getNumFields (mMsg, &result));
        return result;
    }

    size_t MamaMsg::getByteSize (void) const
    {
        size_t result;
        mamaTry (mamaMsg_getByteSize (mMsg, &result));
        return result;
    }

    bool MamaMsg::getBoolean (
        const char*  name, 
        mama_fid_t   fid) const
    {
        mama_bool_t result = 0;
        mamaTry (mamaMsg_getBool (mMsg, name, fid, &result));
        return (result != 0);
    }
        
    bool MamaMsg::getBoolean (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return false;
        else
            return getBoolean (fieldDesc->getName (), fieldDesc->getFid ());
    }
        
    char MamaMsg::getChar (
        const char*  name, 
        mama_fid_t   fid) const
    {
        char result = ' ';
        mamaTry (mamaMsg_getChar (mMsg, name, fid, &result));
        return result;
    }
        
    char MamaMsg::getChar ( const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return ' ';
        else
            return getChar (fieldDesc->getName (), fieldDesc->getFid ());
    }

    int8_t MamaMsg::getI8 (
        const char*  name, 
        mama_fid_t   fid) const
    {
        int8_t result;
        mamaTry (mamaMsg_getI8 (mMsg, name, fid, &result));
        return result;
    }

    int8_t MamaMsg::getI8 (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return 0;
        else
            return getI8 (fieldDesc->getName (), fieldDesc->getFid ());
    }
        
    uint8_t MamaMsg::getU8 (
        const char*  name, 
        mama_fid_t   fid) const
    {
        uint8_t result;
        mamaTry (mamaMsg_getU8 (mMsg, name, fid, &result));
        return result;
    }
        
    uint8_t MamaMsg::getU8 (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return 0;
        else
            return getU8 (fieldDesc->getName (), fieldDesc->getFid ());
    }
        
    int16_t MamaMsg::getI16 (
        const char*  name, 
        mama_fid_t   fid) const
    {
        int16_t result;
        mamaTry (mamaMsg_getI16 (mMsg, name, fid, &result));
        return result;
    }
        
    int16_t MamaMsg::getI16 (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return 0;
        else
            return getI16 (fieldDesc->getName (), fieldDesc->getFid ());
    }
        
    uint16_t MamaMsg::getU16 (
        const char*  name, 
        mama_fid_t   fid) const
    {
        uint16_t result;
        mamaTry (mamaMsg_getU16 (mMsg, name, fid, &result));
        return result;
    }
        
    uint16_t MamaMsg::getU16 (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return 0;
        else
            return getU16 (fieldDesc->getName (), fieldDesc->getFid ());
    }
        
    int32_t MamaMsg::getI32 (
        const char*  name, 
        mama_fid_t   fid) const
    {
        int32_t result;
        mamaTry (mamaMsg_getI32 (mMsg, name, fid, &result));
        return result;
    }

    int32_t MamaMsg::getI32 (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return 0;
        else
            return getI32 (fieldDesc->getName (), fieldDesc->getFid ());
    }

    uint32_t MamaMsg::getU32 (
        const char*  name, 
        mama_fid_t   fid) const
    {
        uint32_t result;
        mamaTry (mamaMsg_getU32 (mMsg, name, fid, &result));
        return result;
    }

    uint32_t MamaMsg::getU32 (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return 0;
        else
            return getU32 (fieldDesc->getName (), fieldDesc->getFid ());
    }

    int64_t MamaMsg::getI64 (
        const char*  name, 
        mama_fid_t   fid) const
    {
        int64_t result;
        mamaTry (mamaMsg_getI64 (mMsg, name, fid, &result));
        return result;
    }

    int64_t MamaMsg::getI64 (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return 0;
        else
            return getI64 (fieldDesc->getName (), fieldDesc->getFid ());
    }

    uint64_t MamaMsg::getU64 (
        const char*  name, 
        mama_fid_t   fid) const
    {
        uint64_t result;
        mamaTry (mamaMsg_getU64 (mMsg, name, fid, &result));
        return result;
    }

    uint64_t MamaMsg::getU64 (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return 0;
        else
            return getU64 (fieldDesc->getName (), fieldDesc->getFid ());
    }

    mama_f32_t MamaMsg::getF32 (
        const char*  name, 
        mama_fid_t   fid) const
    {
        mama_f32_t result;
        mamaTry (mamaMsg_getF32 (mMsg, name, fid, &result));
        return result;
    }

    mama_f32_t MamaMsg::getF32 (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return 0.0;
        else
            return getF32 (fieldDesc->getName (), fieldDesc->getFid ());
    }

    mama_f64_t MamaMsg::getF64 (
        const char*  name, 
        mama_fid_t   fid) const
    {
        mama_f64_t result;
        mamaTry (mamaMsg_getF64 (mMsg, name, fid, &result));
        return result;
    }

    mama_f64_t MamaMsg::getF64 (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return 0.0;
        else
            return getF64 (fieldDesc->getName (), fieldDesc->getFid ());
    }

    const char* MamaMsg::getString (
        const char*  name, 
        mama_fid_t   fid) const
    {
        const char* result = NULL;
        mamaTry (mamaMsg_getString (mMsg, name, fid, &result));
        return result;
    }

    const char* MamaMsg::getString (const MamaFieldDescriptor* fieldDesc) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getString (fieldDesc->getName (), fieldDesc->getFid ());
    }
        
    const void* MamaMsg::getOpaque (
        const char*       name, 
        mama_fid_t        fid,
        size_t&           size) const
    {
        const void *result;
        mamaTry (mamaMsg_getOpaque (mMsg, name, fid, &result,&size));
        return result;
    }

    const void* MamaMsg::getOpaque (
        const MamaFieldDescriptor* fieldDesc,
        size_t&                    size) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getOpaque (fieldDesc->getName (), fieldDesc->getFid (),size);
    }

    void MamaMsg::getDateTime (
        const char*       name,
        mama_fid_t        fid,
        MamaDateTime&     result) const
    {
        mamaTry (mamaMsg_getDateTime (mMsg, name, fid, result.getCValue()));
    }

    void MamaMsg::getDateTime (
        const MamaFieldDescriptor* fieldDesc,
        MamaDateTime&              result) const
    {
        if (!fieldDesc)
            return;
        else
            getDateTime (fieldDesc->getName (), fieldDesc->getFid (), result);
    }

    void MamaMsg::getPrice (
        const char*       name,
        mama_fid_t        fid,
        MamaPrice&        result) const
    {
        mamaTry (mamaMsg_getPrice (mMsg, name, fid, result.getCValue()));
    }

    void MamaMsg::getPrice (
        const MamaFieldDescriptor*  fieldDesc,
        MamaPrice&                  result) const
    {
        if (!fieldDesc)
            return;
        else
            getPrice (fieldDesc->getName(), fieldDesc->getFid(), result);
    }

    const MamaMsg* MamaMsg::getMsg (
        const char*       name,
        mama_fid_t        fid) const
    {
        mamaMsg  tmpResult = NULL;

        if (MAMA_STATUS_OK != mamaTry (mamaMsg_getMsg (mMsg, name, fid, &tmpResult)))
            return NULL;

        if (!mTmpMsg)
        {
            mTmpMsg = new MamaMsg ();
        }

        mTmpMsg->setMsg (tmpResult);

        /* When this message is destroyed or reused we don't
         want the underlying C impl to be destroyed as the parent
         message in C will do this for us */
        mTmpMsg->setDestroyCMsg (false);

        return mTmpMsg;
    }

    const MamaMsg* MamaMsg::getMsg (const MamaFieldDescriptor* fieldDesc) const
    {
        return getMsg (fieldDesc->getName(), fieldDesc->getFid());
    }

    const char* MamaMsg::getVectorChar (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const char*  result = NULL;
        mamaTry (mamaMsg_getVectorChar (mMsg, name, fid,
                                        &result, &resultLen));
        return result;
    }

    const int8_t* MamaMsg::getVectorI8 (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const int8_t*  result = NULL;
        mamaTry (mamaMsg_getVectorI8 (mMsg, name, fid,
                                      &result, &resultLen));
        return result;
    }

    const uint8_t* MamaMsg::getVectorU8 (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const uint8_t*  result = NULL;
        mamaTry (mamaMsg_getVectorU8 (mMsg, name, fid,
                                      &result, &resultLen));
        return result;
    }

    const int16_t* MamaMsg::getVectorI16 (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const int16_t*  result = NULL;
        mamaTry (mamaMsg_getVectorI16 (mMsg, name, fid,
                                       &result, &resultLen));
        return result;
    }

    const uint16_t* MamaMsg::getVectorU16 (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const uint16_t*  result = NULL;
        mamaTry (mamaMsg_getVectorU16 (mMsg, name, fid,
                                       &result, &resultLen));
        return result;
    }

    const int32_t* MamaMsg::getVectorI32 (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const int32_t*  result = NULL;
        mamaTry (mamaMsg_getVectorI32 (mMsg, name, fid,
                                       &result, &resultLen));
        return result;
    }

    const uint32_t* MamaMsg::getVectorU32 (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const uint32_t*  result = NULL;
        mamaTry (mamaMsg_getVectorU32 (mMsg, name, fid,
                                       &result, &resultLen));
        return result;
    }

    const int64_t* MamaMsg::getVectorI64 (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const int64_t*  result = NULL;
        mamaTry (mamaMsg_getVectorI64 (mMsg, name, fid,
                                       &result, &resultLen));
        return result;
    }

    const uint64_t* MamaMsg::getVectorU64 (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const uint64_t*  result = NULL;
        mamaTry (mamaMsg_getVectorU64 (mMsg, name, fid,
                                       &result, &resultLen));
        return result;
    }

    const mama_f32_t* MamaMsg::getVectorF32 (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const mama_f32_t*  result = NULL;
        mamaTry (mamaMsg_getVectorF32 (mMsg, name, fid,
                                       &result, &resultLen));
        return result;
    }

    const mama_f64_t* MamaMsg::getVectorF64 (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const mama_f64_t*  result = NULL;
        mamaTry (mamaMsg_getVectorF64 (mMsg, name, fid,
                                       &result, &resultLen));
        return result;
    }

    const char* MamaMsg::getVectorChar (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorChar (fieldDesc->getName (), fieldDesc->getFid (),
                                  resultLen);
    }

    const int8_t* MamaMsg::getVectorI8 (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorI8 (fieldDesc->getName (), fieldDesc->getFid (),
                                resultLen);
    }

    const uint8_t* MamaMsg::getVectorU8 (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorU8 (fieldDesc->getName (), fieldDesc->getFid (),
                                resultLen);
    }

    const int16_t* MamaMsg::getVectorI16 (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorI16 (fieldDesc->getName (), fieldDesc->getFid (),
                                 resultLen);
    }

    const uint16_t* MamaMsg::getVectorU16 (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorU16 (fieldDesc->getName (), fieldDesc->getFid (),
                                 resultLen);
    }

    const int32_t* MamaMsg::getVectorI32 (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorI32 (fieldDesc->getName (), fieldDesc->getFid (),
                                 resultLen);
    }

    const uint32_t* MamaMsg::getVectorU32 (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorU32 (fieldDesc->getName (), fieldDesc->getFid (),
                                 resultLen);
    }

    const int64_t* MamaMsg::getVectorI64 (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorI64 (fieldDesc->getName (), fieldDesc->getFid (),
                                 resultLen);
    }

    const uint64_t* MamaMsg::getVectorU64 (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorU64 (fieldDesc->getName (), fieldDesc->getFid (),
                                 resultLen);
    }

    const mama_f32_t* MamaMsg::getVectorF32 (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorF32 (fieldDesc->getName (), fieldDesc->getFid (),
                                 resultLen);
    }

    const mama_f64_t* MamaMsg::getVectorF64 (
        const MamaFieldDescriptor*  fieldDesc,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return NULL;
        else
            return getVectorF64 (fieldDesc->getName (), fieldDesc->getFid (),
                                 resultLen);
    }

    const MamaMsg** MamaMsg::getVectorMsg (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const mamaMsg*  tmpResult = NULL;
        mamaTry (mamaMsg_getVectorMsg (mMsg, name, fid, &tmpResult, &resultLen));
        growVector (resultLen);

        for (size_t i = 0; i < resultLen; i++)
        {
            MamaMsg* vecMsg = mVectorMsg[i];
            vecMsg->setMsg (tmpResult[i]);
            vecMsg->setDestroyCMsg (false); 
        }

        return (const MamaMsg**)mVectorMsg;
    }

    const MamaMsg** MamaMsg::getVectorMsg (
        const MamaFieldDescriptor* fieldDesc,
        size_t&           resultLen) const
    {
        return getVectorMsg (fieldDesc->getName(), fieldDesc->getFid(), resultLen);
    }

    const char** MamaMsg::getVectorString (
        const char*       name,
        mama_fid_t        fid,
        size_t&           resultLen) const
    {
        const char**  tmpResult = NULL;
        mamaTry (mamaMsg_getVectorString (mMsg, name, fid, &tmpResult, &resultLen));
        return tmpResult;
    }

    const char** MamaMsg::getVectorString (
        const MamaFieldDescriptor* fieldDesc,
        size_t&           resultLen) const
    {
        return getVectorString (fieldDesc->getName(), fieldDesc->getFid(),
                resultLen);
    }

    bool MamaMsg::tryBoolean (
        const char*   name, 
        mama_fid_t    fid,
        bool&         result) const
    {
        mama_bool_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getBool (mMsg, name, fid, &val)) != MAMA_STATUS_OK)
        {
            return false;
        }
        result = (val != 0);
        return true;
    }

    bool MamaMsg::tryBoolean (
        const MamaFieldDescriptor* field,
        bool&                      result) const
    {
        if (field)
        {
            return tryBoolean (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryChar (
        const char*   name, 
        mama_fid_t    fid,
        char&         result) const
    {
        char val = ' ';
        if (mamaTryIgnoreNotFound (mamaMsg_getChar (mMsg, name, fid, &val)) != MAMA_STATUS_OK) 
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryChar (
        const MamaFieldDescriptor* field,
        char&                   result) const
    {
        if (field)
        {
            return tryChar (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }
        
    bool MamaMsg::tryI8 (
        const char*   name, 
        mama_fid_t    fid,
        int8_t&      result) const
    {
        int8_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getI8 (mMsg, name, fid, &val)) != MAMA_STATUS_OK) 
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryI8 (
        const MamaFieldDescriptor* field,
        int8_t&                   result) const
    {
        if (field)
        {
            return tryI8 (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }
        
    bool MamaMsg::tryU8 (
        const char*   name, 
        mama_fid_t    fid,
        uint8_t&      result) const
    {
        uint8_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getU8 (mMsg, name, fid, &val)) != MAMA_STATUS_OK) 
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryU8 (
        const MamaFieldDescriptor* field,
        uint8_t&                   result) const
    {
        if (field)
        {
            return tryU8 (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }
        
    bool MamaMsg::tryI16 (
        const char*   name, 
        mama_fid_t    fid,
        int16_t&      result) const
    {
        int16_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getI16 (mMsg, name, fid, &val)) != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryI16 (
        const MamaFieldDescriptor* field,
        int16_t&                   result) const
    {
        if (field)
            return tryI16 (field->getName (), field->getFid (), result);
        else
            return false;
    }

    bool MamaMsg::tryU16 (
        const char*   name, 
        mama_fid_t    fid,
        uint16_t&      result) const
    {
        uint16_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getU16 (mMsg, name, fid, &val)) != MAMA_STATUS_OK) 
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryU16 (
        const MamaFieldDescriptor* field,
        uint16_t&                   result) const
    {
        if (field)
        {
            return tryU16 (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryI32 (
        const char*   name, 
        mama_fid_t    fid,
        int32_t&      result) const
    {
        int32_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getI32 (mMsg, name, fid, &val)) != MAMA_STATUS_OK) 
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryI32 (
        const MamaFieldDescriptor* field,
        int32_t&                   result) const
    {
        if (field)
        {
            return tryI32 (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryU32 (
        const char*   name, 
        mama_fid_t    fid,
        uint32_t&      result) const
    {
        uint32_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getU32 (mMsg, name, fid, &val)) != MAMA_STATUS_OK) 
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryU32 (
        const MamaFieldDescriptor* field,
        uint32_t&                   result) const
    {
        if (field)
        {
            return tryU32 (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryI64 (
        const char*   name, 
        mama_fid_t    fid,
        int64_t&      result) const
    {
        int64_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getI64 (mMsg, name, fid, &val)) != MAMA_STATUS_OK) 
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryI64 (
        const MamaFieldDescriptor* field,
        int64_t&                   result) const
    {
        if (field)
        {
            return tryI64 (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryU64 (
        const char*   name, 
        mama_fid_t    fid,
        uint64_t&      result) const
    {
        uint64_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getU64 (mMsg, name, fid, &val)) != MAMA_STATUS_OK) 
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryU64 (
        const MamaFieldDescriptor* field,
        uint64_t&                   result) const
    {
        if (field)
        {
            return tryU64 (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryF32 (
        const char*   name, 
        mama_fid_t    fid,
        mama_f32_t&   result) const
    {
        mama_f32_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getF32 (mMsg, name, fid, &val)) != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryF32 (
        const MamaFieldDescriptor* field,
        mama_f32_t&                result) const
    {
        if (field)
        {
            return tryF32 (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryF64 (
        const char*   name, 
        mama_fid_t    fid,
        mama_f64_t&   result) const
    {
        mama_f64_t val;
        if (mamaTryIgnoreNotFound (mamaMsg_getF64 (mMsg, name, fid, &val)) != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryF64 (
        const MamaFieldDescriptor* field,
        mama_f64_t&                result) const
    {
        if (field)
        {
            return tryF64 (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryString (
        const char*   name, 
        mama_fid_t    fid,
        const char*&  result) const
    {
        const char* val;
        if (mamaTryIgnoreNotFound (mamaMsg_getString (mMsg, name, fid, &val)) != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryString (
        const MamaFieldDescriptor* field,
        const char*&               result) const
    {
        if (field)
        {
            return tryString (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryDateTime (
        const char*    name, 
        mama_fid_t     fid,
        MamaDateTime&  result) const
    {
        if (mamaTryIgnoreNotFound (mamaMsg_getDateTime (mMsg, name, fid, result.getCValue())) !=
            MAMA_STATUS_OK)
        {
            return false;
        }
        return true;
    }

    bool MamaMsg::tryDateTime (
        const MamaFieldDescriptor*  field,
        MamaDateTime&               result) const
    {
        if (field)
        {
            // For performance
            if (mamaTryIgnoreNotFound (mamaMsg_getDateTime (mMsg, field->getName(), 
                            field->getFid (), result.getCValue())) != MAMA_STATUS_OK)
            {
                return false;
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryPrice (
        const char*    name, 
        mama_fid_t     fid,
        MamaPrice&     result) const
    {
        if (mamaTryIgnoreNotFound (mamaMsg_getPrice (mMsg, name, fid,
                                       result.getCValue())) != MAMA_STATUS_OK)
        {
            return false;
        }
        return true;
    }

    bool MamaMsg::tryPrice (
        const MamaFieldDescriptor*  field,
        MamaPrice&                  result) const
    {
        if (field)
            return tryPrice (field->getName(), field->getFid(), result);
        else
            return false;
    }

    bool MamaMsg::tryMsg (
        const char*       name,
        mama_fid_t        fid,
        const MamaMsg*&   result) const
    {
        mamaMsg  tmpResult = NULL;
        if (MAMA_STATUS_OK != mamaMsg_getMsg(mMsg,name,fid,&tmpResult))
            return false;
            
        if (!mTmpMsg)
        {
            mTmpMsg = new MamaMsg ();
        }
        mTmpMsg->setMsg (tmpResult);
        /* When this message is destroyed or reused we don't
         want the underlying C impl to be destroyed as the parent
         message in C will do this for us */
        mTmpMsg->setDestroyCMsg (false);
        result = mTmpMsg;
        return true;
    }

    bool MamaMsg::tryMsg (
        const MamaFieldDescriptor* field,
        const MamaMsg*&            result) const
    {  
        if (field)
        {
            return tryMsg (field->getName(), field->getFid(), result);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryOpaque (
        const char*   name, 
        mama_fid_t    fid,
        const void*&  result,
        size_t&       size) const
    {
        const void* val;
        if (mamaTryIgnoreNotFound (mamaMsg_getOpaque (mMsg, name, fid, &val,&size))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        return true;
    }

    bool MamaMsg::tryOpaque (
        const MamaFieldDescriptor* field,
        const void*&               result,
        size_t&                    size) const
    {
        if (field)
        {
            return tryOpaque (field->getName(), field->getFid(), result,size);
        }
        else
        {
            return false;
        }
    }

    bool MamaMsg::tryVectorChar (
        const char*       name,
        mama_fid_t        fid,
        const char*&      result,
        size_t&           resultLen) const
    {
        const char* val;
        mama_size_t valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorChar (mMsg, name, fid,
                                            &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorI8 (
        const char*       name,
        mama_fid_t        fid,
        const int8_t*&    result,
        size_t&           resultLen) const
    {
        const int8_t*    val;
        mama_size_t      valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorI8 (mMsg, name, fid,
                                          &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorU8 (
        const char*       name,
        mama_fid_t        fid,
        const uint8_t*&   result,
        size_t&           resultLen) const
    {
        const uint8_t*   val;
        mama_size_t      valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorU8 (mMsg, name, fid,
                                          &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorI16 (
        const char*       name,
        mama_fid_t        fid,
        const int16_t*&   result,
        size_t&           resultLen) const
    {
        const int16_t*   val;
        mama_size_t      valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorI16 (mMsg, name, fid,
                                           &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorU16 (
        const char*       name,
        mama_fid_t        fid,
        const uint16_t*&  result,
        size_t&           resultLen) const
    {
        const uint16_t*  val;
        mama_size_t      valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorU16 (mMsg, name, fid,
                                           &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorI32 (
        const char*       name,
        mama_fid_t        fid,
        const int32_t*&   result,
        size_t&           resultLen) const
    {
        const int32_t*   val;
        mama_size_t      valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorI32 (mMsg, name, fid,
                                           &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorU32 (
        const char*       name,
        mama_fid_t        fid,
        const uint32_t*&  result,
        size_t&           resultLen) const
    {
        const uint32_t*  val;
        mama_size_t      valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorU32 (mMsg, name, fid,
                                           &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorI64 (
        const char*       name,
        mama_fid_t        fid,
        const int64_t*&   result,
        size_t&           resultLen) const
    {
        const int64_t*   val;
        mama_size_t      valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorI64 (mMsg, name, fid,
                                           &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorU64 (
        const char*       name,
        mama_fid_t        fid,
        const uint64_t*&  result,
        size_t&           resultLen) const
    {
        const uint64_t*  val;
        mama_size_t      valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorU64 (mMsg, name, fid,
                                           &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorF32 (
        const char*         name,
        mama_fid_t          fid,
        const mama_f32_t*&  result,
        size_t&             resultLen) const
    {
        const mama_f32_t*  val;
        mama_size_t        valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorF32 (mMsg, name, fid,
                                           &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorF64 (
        const char*         name,
        mama_fid_t          fid,
        const mama_f64_t*&  result,
        size_t&             resultLen) const
    {
        const mama_f64_t*  val;
        mama_size_t        valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorF64 (mMsg, name, fid,
                                           &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorString (
        const char*    name,
        mama_fid_t     fid,
        const char**&  result,
        size_t&        resultLen) const
    {
        const char**  val;
        mama_size_t   valLen;
        if (mamaTryIgnoreNotFound (mamaMsg_getVectorString (mMsg, name, fid,
                                              &val, &valLen))
                != MAMA_STATUS_OK)
        {
            return false;
        }
        result = val;
        resultLen = valLen;
        return true;
    }

    bool MamaMsg::tryVectorMsg (
        const char*       name,
        mama_fid_t        fid,
        const MamaMsg**&  result,
        size_t&           resultLen) const
    {
        const mamaMsg*  tmpResult = NULL;
        mama_size_t     tmpResultLen;

        if (mamaTryIgnoreNotFound (mamaMsg_getVectorMsg (mMsg, name, fid,
                                           &tmpResult, &tmpResultLen))
            != MAMA_STATUS_OK)
        {
            return false;
        }

        resultLen = tmpResultLen;
        growVector (resultLen);

        for (size_t i = 0; i < resultLen; i++)
        {
            MamaMsg* vecMsg = mVectorMsg[i];
            vecMsg->setMsg (tmpResult[i]);
            vecMsg->setDestroyCMsg (false);
        }

        result = (const MamaMsg**)mVectorMsg;

        return true;
    }

    bool MamaMsg::tryVectorChar (
        const MamaFieldDescriptor*  fieldDesc,
        const char*&                result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorChar (fieldDesc->getName (), fieldDesc->getFid (),
                                  result, resultLen);
    }

    bool MamaMsg::tryVectorI8 (
        const MamaFieldDescriptor*  fieldDesc,
        const int8_t*&              result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorI8 (fieldDesc->getName (), fieldDesc->getFid (),
                                result, resultLen);
    }

    bool MamaMsg::tryVectorU8 (
        const MamaFieldDescriptor*  fieldDesc,
        const uint8_t*&             result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorU8 (fieldDesc->getName (), fieldDesc->getFid (),
                                result, resultLen);
    }

    bool MamaMsg::tryVectorI16 (
        const MamaFieldDescriptor*  fieldDesc,
        const int16_t*&             result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorI16 (fieldDesc->getName (), fieldDesc->getFid (),
                                 result, resultLen);
    }

    bool MamaMsg::tryVectorU16 (
        const MamaFieldDescriptor*  fieldDesc,
        const uint16_t*&            result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorU16 (fieldDesc->getName (), fieldDesc->getFid (),
                                 result, resultLen);
    }

    bool MamaMsg::tryVectorI32 (
        const MamaFieldDescriptor*  fieldDesc,
        const int32_t*&             result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorI32 (fieldDesc->getName (), fieldDesc->getFid (),
                                 result, resultLen);
    }

    bool MamaMsg::tryVectorU32 (
        const MamaFieldDescriptor*  fieldDesc,
        const uint32_t*&            result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorU32 (fieldDesc->getName (), fieldDesc->getFid (),
                                 result, resultLen);
    }

    bool MamaMsg::tryVectorI64 (
        const MamaFieldDescriptor*  fieldDesc,
        const int64_t*&             result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorI64 (fieldDesc->getName (), fieldDesc->getFid (),
                                 result, resultLen);
    }

    bool MamaMsg::tryVectorU64 (
        const MamaFieldDescriptor*  fieldDesc,
        const uint64_t*&            result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorU64 (fieldDesc->getName (), fieldDesc->getFid (),
                                 result, resultLen);
    }

    bool MamaMsg::tryVectorF32 (
        const MamaFieldDescriptor*  fieldDesc,
        const mama_f32_t*&            result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorF32 (fieldDesc->getName (), fieldDesc->getFid (),
                                 result, resultLen);
    }

    bool MamaMsg::tryVectorF64 (
        const MamaFieldDescriptor*  fieldDesc,
        const mama_f64_t*&            result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorF64 (fieldDesc->getName (), fieldDesc->getFid (),
                                 result, resultLen);
    }

    bool MamaMsg::tryVectorString (
        const MamaFieldDescriptor*  fieldDesc,
        const char**&               result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorString (fieldDesc->getName (), fieldDesc->getFid (),
                                    result, resultLen);
    }

    bool MamaMsg::tryVectorMsg (
        const MamaFieldDescriptor*  fieldDesc,
        const MamaMsg**&            result,
        size_t&                     resultLen) const
    {
        if (!fieldDesc)
            return false;
        else
            return tryVectorMsg (fieldDesc->getName (), fieldDesc->getFid (),
                                 result, resultLen);
    }

/*
    #define ExpandImplAddScalar (method, fType, cFunc)              \
                                                                    \
    void MamaMsg::method (const char* name,                         \
                          mama_fid_t                                \
                          fid,                                      \
                          fType value)                              \
    {                                                               \
        mamaTry (cFunc (mMsg, name, fid, value));                   \
    }                                                               \
                                                                    \
    void MamaMsg::method (                                          \
        const MamaFieldDescriptor* field, fType value)              \
    {                                                               \
        if (field)                                                  \
            method( field->getName(), field->getFid(), value);      \
    }                                                               
*/
#define ExpandImplAddScalar(method,fType,cFunc) \
void MamaMsg::method (const char* name, mama_fid_t fid, fType value)   \
{ mamaTry (cFunc (mMsg, name, fid, value)); } \
void MamaMsg::method (const MamaFieldDescriptor* field, fType value)   \
{ if (field) method(field->getName(),field->getFid(),value); }

    ExpandImplAddScalar (addChar,   char,        mamaMsg_addChar)
    ExpandImplAddScalar (addI8,     int8_t,      mamaMsg_addI8)
    ExpandImplAddScalar (addU8,     uint8_t,     mamaMsg_addU8)
    ExpandImplAddScalar (addI16,    int16_t,     mamaMsg_addI16)
    ExpandImplAddScalar (addU16,    uint16_t,    mamaMsg_addU16)
    ExpandImplAddScalar (addI32,    int32_t,     mamaMsg_addI32)
    ExpandImplAddScalar (addU32,    uint32_t,    mamaMsg_addU32)
    ExpandImplAddScalar (addI64,    int64_t,     mamaMsg_addI64)
    ExpandImplAddScalar (addU64,    uint64_t,    mamaMsg_addU64)
    ExpandImplAddScalar (addF32,    mama_f32_t,  mamaMsg_addF32)
    ExpandImplAddScalar (addF64,    mama_f64_t,  mamaMsg_addF64)
    ExpandImplAddScalar (addString, const char*, mamaMsg_addString)


    void MamaMsg::addBoolean (
        const char*   name, 
        mama_fid_t    fid,
        bool          value)
    {
        mama_bool_t b_value = 0;

        if (true == value) 
        {
            b_value = 1;
        }

        mamaTry (mamaMsg_addBool (mMsg, name, fid, b_value));
    }

    void MamaMsg::addBoolean (
        const MamaFieldDescriptor*  fieldDesc,
        bool                        value)
    {
        if (fieldDesc)
        {
            addBoolean (fieldDesc->getName(), fieldDesc->getFid(), value);
        }
    }
        
    void MamaMsg::addDateTime (
        const char*         name, 
        mama_fid_t          fid,
        const MamaDateTime& value)
    {
        mamaTry (mamaMsg_addDateTime (mMsg, name, fid, value.getCValue()));
    }

    void MamaMsg::addDateTime (
        const MamaFieldDescriptor*  fieldDesc,
        const MamaDateTime&         value)
    {
        if (fieldDesc)
        {
            addDateTime (fieldDesc->getName(), fieldDesc->getFid(), value);
        }
    }

    void MamaMsg::addPrice (
        const char*         name, 
        mama_fid_t          fid,
        const MamaPrice&    value)
    {
        mamaTry (mamaMsg_addPrice (mMsg, name, fid, value.getCValue()));
    }

    void MamaMsg::addPrice (
        const MamaFieldDescriptor*  fieldDesc,
        const MamaPrice&            value)
    {
        if (fieldDesc)
        {
            addPrice (fieldDesc->getName(), fieldDesc->getFid(), value);
        }
    }

    void MamaMsg::addOpaque (
        const char*   name, 
        mama_fid_t    fid,
        const void*   value,
        size_t        size)
    {
        mamaTry (mamaMsg_addOpaque (mMsg, name, fid, value, size));
    }

    void MamaMsg::addOpaque (
        const MamaFieldDescriptor*  fieldDesc,
        const void*                 value,
        size_t                      size)
    {
        if (fieldDesc)
        {
            addOpaque (fieldDesc->getName(), fieldDesc->getFid(), value, size);
        }
    }

    void MamaMsg::addMsg (
        const char* name, 
        mama_fid_t fid, 
        MamaMsg* value)
    {
        if (value)
        {
            mamaTry(mamaMsg_addMsg(mMsg, name, fid, value->mMsg));
        }
        else
        {
            throw MamaStatus(MAMA_STATUS_NULL_ARG);
        }
    }   

    void MamaMsg::addMsg (
        const MamaFieldDescriptor* fieldDesc, 
        MamaMsg* value)
    {
        if (fieldDesc)
        {
            addMsg(fieldDesc->getName(), fieldDesc->getFid(), value);
        }
    }
       
    void MamaMsg::addVectorMsg (
        const MamaFieldDescriptor*  fieldDesc,
        MamaMsg*                    vectorValues[],
        size_t                      vectorLen)
    {    
        if (fieldDesc)
        {
            addVectorMsg (fieldDesc->getName(), fieldDesc->getFid(), vectorValues, vectorLen);
        }
    }            
            
    void MamaMsg::addVectorMsg (
        const char*       name,
        mama_fid_t        fid,
        MamaMsg*          vectorValues[],
        size_t            vectorLen)
    {    
        growCvector (vectorLen);  

        for (size_t i = 0; i < vectorLen; i++)
        {
            mCvectorMsg[i] = vectorValues[i]->mMsg;
            vectorValues[i]->setDestroyCMsg (false);
        }

        mamaTry (mamaMsg_addVectorMsg (mMsg, name, fid, mCvectorMsg, vectorLen));
    }       

    #define ExpandAddVectorScalar(method,fType,cFunc)                       \
                                                                            \
    void MamaMsg::method (const char* name, mama_fid_t fid,                 \
                          fType vectorValues[], size_t vectorLen)           \
    {                                                                       \
        mamaTry (cFunc (mMsg, name, fid, vectorValues, vectorLen));         \
    }                                                                       \
                                                                            \
    void MamaMsg::method (const MamaFieldDescriptor* field,                 \
                          fType vectorValues[], size_t vectorLen)           \
    {                                                                       \
        if (field)                                                          \
            method (field->getName(),                                       \
                    field->getFid(),                                        \
                    vectorValues,                                           \
                    vectorLen);                                             \
    }

    ExpandAddVectorScalar (addVectorChar,   const char,       mamaMsg_addVectorChar)
    ExpandAddVectorScalar (addVectorI8,     const int8_t,     mamaMsg_addVectorI8)
    ExpandAddVectorScalar (addVectorU8,     const uint8_t,    mamaMsg_addVectorU8)
    ExpandAddVectorScalar (addVectorI16,    const int16_t,    mamaMsg_addVectorI16)
    ExpandAddVectorScalar (addVectorU16,    const uint16_t,   mamaMsg_addVectorU16)
    ExpandAddVectorScalar (addVectorI32,    const int32_t,    mamaMsg_addVectorI32)
    ExpandAddVectorScalar (addVectorU32,    const uint32_t,   mamaMsg_addVectorU32)
    ExpandAddVectorScalar (addVectorI64,    const int64_t,    mamaMsg_addVectorI64)
    ExpandAddVectorScalar (addVectorU64,    const uint64_t,   mamaMsg_addVectorU64)
    ExpandAddVectorScalar (addVectorF32,    const mama_f32_t, mamaMsg_addVectorF32)
    ExpandAddVectorScalar (addVectorF64,    const mama_f64_t, mamaMsg_addVectorF64)
    ExpandAddVectorScalar (addVectorString, const char*,      mamaMsg_addVectorString)


    // Update method implementation
/*
    #define ExpandUpdateVectorScalar (method, fType, cFunc)                 \
                                                                            \
    void MamaMsg::method (const char* name, mama_fid_t fid, fType value)    \
    {                                                                       \
        mamaTry (cFunc (mMsg, name, fid, value));                           \
    }                                                                       \
                                                                            \
    void MamaMsg::method (const MamaFieldDescriptor* field, fType value)    \
    {                                                                       \
        if (field)                                                          \
            method(field->getName(),field->getFid(),value);                 \
    }
*/
#define ExpandUpdateVectorScalar(method,fType,cFunc) \
void MamaMsg::method (const char* name, mama_fid_t fid, fType value)   \
{ mamaTry (cFunc (mMsg, name, fid, value)); }                  \
void MamaMsg::method (const MamaFieldDescriptor* field, fType value)   \
{ if (field) method(field->getName(),field->getFid(),value); }


    ExpandUpdateVectorScalar (updateChar,   const char,       mamaMsg_updateChar)
    ExpandUpdateVectorScalar (updateI8,     const int8_t,     mamaMsg_updateI8)
    ExpandUpdateVectorScalar (updateU8,     const uint8_t,    mamaMsg_updateU8)
    ExpandUpdateVectorScalar (updateI16,    const int16_t,    mamaMsg_updateI16)
    ExpandUpdateVectorScalar (updateU16,    const uint16_t,   mamaMsg_updateU16)
    ExpandUpdateVectorScalar (updateI32,    const int32_t,    mamaMsg_updateI32)
    ExpandUpdateVectorScalar (updateU32,    const uint32_t,   mamaMsg_updateU32)
    ExpandUpdateVectorScalar (updateI64,    const int64_t,    mamaMsg_updateI64)
    ExpandUpdateVectorScalar (updateU64,    const uint64_t,   mamaMsg_updateU64)
    ExpandUpdateVectorScalar (updateF32,    const mama_f32_t, mamaMsg_updateF32)
    ExpandUpdateVectorScalar (updateF64,    const mama_f64_t, mamaMsg_updateF64)
    ExpandUpdateVectorScalar (updateString, const char*,      mamaMsg_updateString)

    void MamaMsg::updateBoolean (
        const char*   name, 
        mama_fid_t    fid,
        bool          value)
    {
        mama_bool_t b_value = 0;

        if (true == value) 
        {
            b_value = 1;
        }

        mamaTry (mamaMsg_updateBool (mMsg, name, fid, b_value));
    }

    void MamaMsg::updateBoolean (
        const MamaFieldDescriptor*  fieldDesc,
        bool                        value)
    {
        if (fieldDesc)
        {
            updateBoolean (fieldDesc->getName(), fieldDesc->getFid(), value);
        }
    }

    void MamaMsg::updateDateTime (
        const char*         name, 
        mama_fid_t          fid,
        const MamaDateTime& value)
    {
        mamaTry (mamaMsg_updateDateTime (mMsg, name, fid, value.getCValue()));
    }

    void MamaMsg::updateDateTime (
        const MamaFieldDescriptor*  fieldDesc,
        const MamaDateTime&         value)
    {
        if (fieldDesc)
        {
            updateDateTime (fieldDesc->getName(), fieldDesc->getFid(), value);
        }
    }

    void MamaMsg::updatePrice (
        const char*         name, 
        mama_fid_t          fid,
        const MamaPrice&    value)
    {
        mamaTry (mamaMsg_updatePrice (mMsg, name, fid, value.getCValue()));
    }

    void MamaMsg::updatePrice (
        const MamaFieldDescriptor*  fieldDesc,
        const MamaPrice&            value)
    {
        if (fieldDesc)
        {
            updatePrice (fieldDesc->getName(), fieldDesc->getFid(), value);
        }
    }

    void MamaMsg::updateOpaque (
        const char*   name, 
        mama_fid_t    fid,
        const void*   value,
        size_t        size)
    {
        mamaTry (mamaMsg_updateOpaque (mMsg, name, fid, value, size));
    }

    void MamaMsg::updateOpaque (
        const MamaFieldDescriptor*  fieldDesc,
        const void*                 value,
        size_t                      size)
    {
        if (fieldDesc)
        {
            updateOpaque (fieldDesc->getName(), fieldDesc->getFid(), value, size);
        }
    }

    // Miscellaneous method implementation
    mamaMsgType MamaMsg::getType (void) const
    {
        int32_t result;
        mamaTry (mamaMsg_getI32 (mMsg, 
                                 MamaFieldMsgType.mName, 
                                 MamaFieldMsgType.mFid, 
                                 &result));
        return static_cast<mamaMsgType> (result);
    }

    const char* MamaMsg::getMsgTypeName (void) const
    {
        return mamaMsgType_stringForMsg (mMsg);
    }

    mamaMsgStatus MamaMsg::getStatus (void) const
    {
        return mamaMsgStatus_statusForMsg (mMsg);
    }

    const char* MamaMsg::getMsgStatusString (void) const
    {
        return  mamaMsgStatus_stringForMsg (mMsg);
    }

    const char* MamaMsg::toString () const
    {
        if (mString != NULL )
        {
            mamaMsg_freeString (mMsg, mString);
        }
        mString = mamaMsg_toString (mMsg);

        return mString;
    }

    MamaMsgField* MamaMsg::getField (
        const char*   name, 
        mama_fid_t   fid) const
    {
        mamaMsgField  tmpResult = NULL;

        if (MAMA_STATUS_OK != mamaTry (mamaMsg_getField (mMsg, name,fid, &tmpResult)))
            return NULL;
        if (!mMsgField)
        {
            mMsgField = new MamaMsgField ();
        }
        mMsgField->set (tmpResult);
        return mMsgField;
    }


    MamaMsgField* MamaMsg::getField (const MamaFieldDescriptor* fieldDesc)const
                                    
    {
        if (!fieldDesc)
            return NULL;
        return getField (fieldDesc->getName (), fieldDesc->getFid ());
    }

    bool MamaMsg::tryField (
        const char*  name, 
        mama_fid_t   fid) const
    {
        mamaMsgField tmpResult = NULL;
        if (mamaMsg_getField (mMsg, name, fid, &tmpResult) != MAMA_STATUS_OK)
        {
            return false;
        }
        return true;
    }


    bool MamaMsg::tryField (
        const MamaFieldDescriptor*  fieldDesc, 
        MamaMsgField*               result) const
    {
        if (!fieldDesc)
            return false;
        return tryField (fieldDesc->getName(), fieldDesc->getFid(),result);
    }

    bool MamaMsg::tryField (
        const char* name, 
        mama_fid_t fid, 
        MamaMsgField* result) const
    {
        mamaMsgField tmpResult = NULL;
        if (mamaTryIgnoreNotFound (
                mamaMsg_getField (mMsg, name, fid, &tmpResult)) != MAMA_STATUS_OK)
        {
            return false;
        }
        result->set (tmpResult);
        return true;
    }


    bool MamaMsg::tryField (const MamaFieldDescriptor* fieldDesc) const

    {
        bool result = false;

        if (!fieldDesc)
            return result;

        return tryField (fieldDesc->getName (), fieldDesc->getFid ());
    }

    void MamaMsg::getFieldAsString (
        const char*  name,
        mama_fid_t   fid, 
        char*        result,
        size_t       maxResultLen) const
    {
        mamaTry (mamaMsg_getFieldAsString (
                     mMsg, 
                     name,
                     fid,
                     result,
                     maxResultLen));
    }

    void MamaMsg::getFieldAsString (
        const MamaFieldDescriptor*  fieldDesc,
        char*                       result,
        size_t                      maxResultLen) const
    {
        if (fieldDesc)
        {
            getFieldAsString (fieldDesc->getName(), fieldDesc->getFid(),
                              result, maxResultLen);
        }
    }

    bool MamaMsg::tryFieldAsString (
        const char*  name,
        mama_fid_t   fid, 
        char*        result,
        size_t       maxResultLen) const
    {
        if (mamaTryIgnoreNotFound (mamaMsg_getFieldAsString (mMsg, name, fid,
                                         result, maxResultLen)) != MAMA_STATUS_OK)
        {
            return false;
        }
        return true;
    }

    bool MamaMsg::tryFieldAsString (
        const MamaFieldDescriptor*  fieldDesc,
        char*                       result,
        size_t                      maxResultLen) const
    {
        return  tryFieldAsString (fieldDesc->getName(), fieldDesc->getFid(),
                                  result, maxResultLen);
    }

    void MamaMsg::getByteBuffer (
        const void*&  buffer,
        size_t&       bufferLength) const
    {
        const void* val = NULL;
        mamaTry (mamaMsg_getByteBuffer (mMsg, &val, &bufferLength));
        buffer = val;
    }

    mama_seqnum_t MamaMsg::getSeqNum (void) const
    {
        mama_seqnum_t result;
        mamaTry (mamaMsg_getSeqNum (mMsg, &result));
        return result;
    }

    bool MamaMsg::isFromInbox (void) const
    {
        return mamaMsg_isFromInbox (mMsg);
    }

    bool MamaMsg::getIsDefinitelyDuplicate (void) const
    {
        int result = 0;
        mamaTry (mamaMsg_getIsDefinitelyDuplicate (mMsg, &result));
        return result != 0;
    }

    bool MamaMsg::getIsPossiblyDuplicate (void) const
    {
        int result = 0;
        mamaTry (mamaMsg_getIsPossiblyDuplicate (mMsg, &result));
        return result != 0;
    }

    bool MamaMsg::getIsPossiblyDelayed (void) const
    {
        int result = 0;
        mamaTry (mamaMsg_getIsPossiblyDelayed (mMsg, &result));
        return result != 0;
    }

    bool MamaMsg::getIsDefinitelyDelayed (void) const
    {
        int result = 0;
        mamaTry (mamaMsg_getIsDefinitelyDelayed (mMsg, &result));
        return result != 0;
    }

    bool MamaMsg::getIsOutOfSequence (void) const
    {
        int result = 0;
        mamaTry (mamaMsg_getIsOutOfSequence (mMsg, &result));
        return result != 0;
    }

    mamaMsgReply MamaMsg::getReplyHandle (void) const
    {
        mamaMsgReply replyHandle = NULL;
        mamaTry (mamaMsg_getReplyHandle(mMsg, &replyHandle));
        return replyHandle;
    }

    void MamaMsg::destroyReplyHandle (mamaMsgReply replyHandle)
    {
        mamaTry (mamaMsg_destroyReplyHandle(replyHandle));
    }

    bool MamaMsg::setNewBuffer (
        void*        buffer,
        mama_size_t  size)
    {
        int result = 0;
        if (MAMA_STATUS_OK != (mamaTry (mamaMsg_setNewBuffer (
                            mMsg, buffer, size))))
        {
            return result;
        }
        return result != 0;
    }

    void* MamaMsg::getNativeHandle () const
    {
        void* result = NULL;
        mamaTry (mamaMsg_getNativeHandle (mMsg, &result));
        return result;
    }

    struct MsgIteratorClosure
    {
        MsgIteratorClosure (const MamaMsg&         msg,
                            MamaMsgField&          msgField,
                            MamaMsgFieldIterator&  iter,
                            void*                  closure)
            : mMsg (msg)
            , mMsgField (msgField)
            , mIter (iter)
            , mClosure (closure)
        {
        }

        const MamaMsg&         mMsg;
        MamaMsgField&          mMsgField;
        MamaMsgFieldIterator&  mIter;
        void*                  mClosure;
    };

    extern "C" 
    {
        static void MAMACALLTYPE msgItCb (
            const mamaMsg       msg,
            const mamaMsgField  field,
            void*               closure)
        {
            MsgIteratorClosure* iterClosure
                  = (MsgIteratorClosure*) (closure);

            iterClosure->mMsgField.set (field);
            iterClosure->mIter.onField (iterClosure->mMsg,
                                        iterClosure->mMsgField,
                                        iterClosure->mClosure);
        }
    }

    void MamaMsg::iterateFields (
        MamaMsgFieldIterator&  theIterator,
        const MamaDictionary*  dictionary,
        void*                  closure) const
    {
        mamaDictionary dictC = NULL;
        if (dictionary)
        {
            dictC = dictionary->getDictC ();
        }
        MsgIteratorClosure aClosure (*this, *mMsgField, theIterator, closure);
        mamaTry (mamaMsg_iterateFields (mMsg, msgItCb, dictC, &aClosure));
    }

    void MamaMsg::setDestroyCMsg (bool destroy)
    {
        mDestroy = destroy;
    }

    void MamaMsg::growVector (size_t newSize) const
    {
        if (newSize > mVectorMsgAllocSize)
        {
            MamaMsg** newVector = new MamaMsg*[newSize];
            size_t i;

            /* Copy elements from old vector. */
            for (i = 0; i < mVectorMsgAllocSize; i++)
            {
                newVector[i] = mVectorMsg[i];
            }

            /* Create new elements. */
            for (; i < newSize; i++)
            {
                newVector[i] = new MamaMsg;
            }

            if (mVectorMsg) 
                delete [] mVectorMsg;

            mVectorMsg = newVector;
            mVectorMsgAllocSize = newSize;
        }
    }

    void MamaMsg::growCvector (size_t newSize) const
    {
        if (newSize > mCvectorMsgAllocSize)
        {
            mamaMsg* newVector = new mamaMsg[newSize];
            size_t i;

            /* Copy elements from old vector. */
            for (i = 0; i < mCvectorMsgAllocSize; i++)
            {
                newVector[i] = mCvectorMsg[i];
            }

            for (; i < newSize; i++)
            {
                newVector[i] = NULL;
            }

            if (mCvectorMsg) 
                delete [] mCvectorMsg;

            mCvectorMsg = newVector;
            mCvectorMsgAllocSize = newSize;
        }
    }

    void MamaMsg::cleanup ()
    {
        if (mDestroy && mMsg)
        {
            mamaTry (mamaMsg_destroy (mMsg));
            mDestroy = false;
        }

        if (mString)
        {
            mamaMsg_freeString (mMsg, mString);
            mString = NULL;
        }

        if (mVectorMsg)
        {
            for (size_t i = 0; i < mVectorMsgAllocSize; ++i)
            {
                MamaMsg* msg = mVectorMsg[i];
                delete msg;
            }

            delete[] mVectorMsg;
            mVectorMsg = NULL;
            mVectorMsgAllocSize = 0;
        }

        if (mCvectorMsg)
        {
            for (size_t i = 0; i < mCvectorMsgAllocSize; ++i)
            {
                mamaMsg msg = mCvectorMsg[i];
                mamaTry (mamaMsg_destroy (msg));
            }

            delete[] mCvectorMsg;
            mCvectorMsg = NULL;
            mCvectorMsgAllocSize = 0;
        }
        
        if (mTmpMsg)
        {
            delete mTmpMsg;
            mTmpMsg = NULL;
        }
    }

    void MamaMsg::applyMsg (const MamaMsg&  delta)
    {
        mama_status status;
        
        if (MAMA_STATUS_OK != (status = mamaMsg_applyMsg (mMsg, delta.getUnderlyingMsg())))
            throw MamaStatus(status);
            
    }

    MamaMsgField& MamaMsg::begin (MamaMsgIterator& theiterator) const
    {  
        mamaMsgIterator_associate(
            ((mamaMsgIterator)(theiterator.myImpl)), 
            getUnderlyingMsg());

        mamaMsgIterator_begin (
            (mamaMsgIterator)(theiterator.myImpl));

        theiterator.mMsgField.set (
            mamaMsgIterator_next((mamaMsgIterator)theiterator.myImpl));

        return (theiterator.mMsgField);
    }

    MamaMsgIterator::MamaMsgIterator (const MamaDictionary*  dictionary)
    {  
        mamaDictionary dictC = NULL;
        if (dictionary)
        {
            dictC = dictionary->getDictC ();
        }
        
        mamaTry(mamaMsgIterator_create (&myImpl, dictC));
    }

    MamaMsgIterator::MamaMsgIterator ()
    {  
        mamaTry(mamaMsgIterator_create (&myImpl, NULL));
    }

    MamaMsgIterator::~MamaMsgIterator ()
    {
        mamaMsgIterator_destroy (myImpl);
    }

    MamaMsgIterator&  MamaMsgIterator::operator= (const MamaMsgIterator& rhs)
    {
        if (this != &rhs)
        {
            (*(mamaMsgIterator*)myImpl) =  (*(mamaMsgIterator*)rhs.myImpl);
            (mMsgField = rhs.mMsgField);
        }
        return *this;
    }

    bool  MamaMsgIterator::operator==(const MamaMsgIterator& rhs) const
    {
        return (mMsgField == rhs.mMsgField);
    }

    bool MamaMsgIterator::operator!=(const MamaMsgIterator& rhs) const
    {
        return (mMsgField != rhs.mMsgField);
    }

    MamaMsgField& MamaMsgIterator::operator*()
    {
        return (mMsgField);
    }

    MamaMsgIterator& MamaMsgIterator::operator++()
    {
        mMsgField.set (mamaMsgIterator_next((mamaMsgIterator)myImpl));
        return *this;
    }

    void MamaMsgIterator::SetDict (const MamaDictionary*  dictionary)
    {
        mamaDictionary dictC = NULL;
        if (dictionary)
        {
            dictC = dictionary->getDictC ();
        }
        
        mamaMsgIterator_setDict((mamaMsgIterator)myImpl, dictC);
    }

} // namespace Wombat
