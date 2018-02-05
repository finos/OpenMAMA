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

#ifndef MAMA_MSG_FIELD_IMPL_CPP_H__
#define MAMA_MSG_FIELD_IMPL_CPP_H__

#include <mama/mamacpp.h>
#include "mamacppinternal.h"
#include <mama/mama.h>
#include <mama/msgfield.h>


namespace Wombat 
{
/******************************************************************************
 * MamaMsgField Implementation
 */
class MamaMsgFieldImpl 
{
    MamaMsgFieldImpl& operator= (const MamaMsgFieldImpl&);
public:
    MamaMsgFieldImpl ()
        : mField (NULL)
        , mFieldDesc (NULL)
        , mLastVectorMsg (NULL)
        , mLastVectorMsgLen (0)
    {}

    MamaMsgFieldImpl (mamaMsgField field)
        : mField (field)
        , mFieldDesc (NULL)
        , mLastVectorMsg (NULL)
        , mLastVectorMsgLen (0)
    {}

    ~MamaMsgFieldImpl () 
    {
        clear();
    }

    void clear ()
    {
        if (mFieldDesc)
        {
            delete mFieldDesc;
            mFieldDesc = NULL;
        }
        if (mLastVectorMsg != NULL)
        {
            destroyLastVectorMsg ();
            mLastVectorMsg = NULL;
            mLastVectorMsgLen = 0;
        }
    }

    void set (mamaMsgField field)
    {
        if (mField != field)
        {
            clear();
            mField = field;
        }
    }

    const MamaFieldDescriptor* getDescriptor () const
    {
        if (!mFieldDesc)
        {
            mamaFieldDescriptor desc = NULL;
            mamaTry(mamaMsgField_getDescriptor (mField, &desc));
            if (desc)
            {
                mFieldDesc = new MamaFieldDescriptor (desc);
            }
            return mFieldDesc;
        }
        return mFieldDesc;
    }

    void getVectorMsg (
        const MamaMsg**&  result,
        size_t&           resultLen) const
    {
        const MamaMsg** mamaMsgResult = NULL;
        const mamaMsg*  tmpResult = NULL;
        
        if (mLastVectorMsg != NULL)
        {
            destroyLastVectorMsg ();
        }
        
        mamaTry (mamaMsgField_getVectorMsg (mField, &tmpResult, &resultLen));
        mamaMsgResult      = (const MamaMsg**)calloc(resultLen,sizeof(MamaMsg*));
        mLastVectorMsg    = mamaMsgResult;
        mLastVectorMsgLen = resultLen;

        for (size_t i = 0; i < resultLen; i++)
        {
            MamaMsg* resMsg = new MamaMsg;
            resMsg->createFromMsg (tmpResult[i], false);
            mamaMsgResult[i] = resMsg;
            mLastVectorMsg[i] = mamaMsgResult[i];
        }
        result = mamaMsgResult;
    }

    void getVectorMsgDetached (
        const MamaMsg**&  result,
        size_t&           resultLen) const
    {
        const mamaMsg*  tmpResult = NULL;
        result = NULL;
        
//        mamaTry (mamaMsgField_getVectorMsgDetached (
  //                   mField, &tmpResult, &resultLen));
        result = (const MamaMsg**)calloc(resultLen,sizeof(MamaMsg*));
        
        for (size_t i = 0; i < resultLen; i++)
        {
            MamaMsg* resMsg = new MamaMsg;
            resMsg->createFromMsg (tmpResult[i], true);
            result[i] = resMsg;
        }
    }

    void getAsString (
        char*        result,
        int          maxResultLen) const
    {
        mamaMsgField_getAsString (mField, result, maxResultLen);
    }

protected:
    friend class MamaMsgField;

    mamaMsgField                  mField;
    mutable MamaFieldDescriptor*  mFieldDesc;

private: 
    mutable const MamaMsg**       mLastVectorMsg;
    mutable mama_size_t           mLastVectorMsgLen;
    void destroyLastVectorMsg() const
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
};

} // namespace Wombat
#endif // MAMA_MSG_FIELD_IMPL_CPP_H__
