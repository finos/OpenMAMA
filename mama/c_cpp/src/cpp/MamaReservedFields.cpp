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

#include "mama/mamacpp.h"
#include <mama/MamaReservedFields.h>
#include <mama/MamaFieldDescriptor.h>

namespace Wombat
{

    const MamaFieldDescriptor*  MamaReservedFields::MsgType            = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::MsgStatus          = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::FieldIndex         = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::MsgNum             = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::MsgTotal           = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::SeqNum             = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::FeedName           = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::FeedHost           = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::FeedGroup          = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::ItemSeqNum         = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::SendTime           = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::AppDataType        = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::AppMsgType         = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::SenderId           = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::MsgQual            = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::ConflateQuoteCount = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::EntitleCode        = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::SymbolList         = NULL;



    void MamaReservedFields::initReservedFields()
    {
        MsgType             = new MamaFieldDescriptor (MamaReservedFieldMsgType);
        MsgStatus           = new MamaFieldDescriptor (MamaReservedFieldMsgStatus);
        FieldIndex          = new MamaFieldDescriptor (MamaReservedFieldFieldIndex);
        MsgNum              = new MamaFieldDescriptor (MamaReservedFieldMsgNum);
        MsgTotal            = new MamaFieldDescriptor (MamaReservedFieldMsgTotal);
        SeqNum              = new MamaFieldDescriptor (MamaReservedFieldSeqNum);
        FeedName            = new MamaFieldDescriptor (MamaReservedFieldFeedName);
        FeedHost            = new MamaFieldDescriptor (MamaReservedFieldFeedHost);
        FeedGroup           = new MamaFieldDescriptor (MamaReservedFieldFeedGroup);
        ItemSeqNum          = new MamaFieldDescriptor (MamaReservedFieldItemSeqNum);
        SendTime            = new MamaFieldDescriptor (MamaReservedFieldSendTime);
        AppDataType         = new MamaFieldDescriptor (MamaReservedFieldAppDataType);
        AppMsgType          = new MamaFieldDescriptor (MamaReservedFieldAppMsgType);
        SenderId            = new MamaFieldDescriptor (MamaReservedFieldSenderId);
        MsgQual             = new MamaFieldDescriptor (MamaReservedFieldMsgQual);
        ConflateQuoteCount  = new MamaFieldDescriptor (MamaReservedFieldConflateQuoteCount);
        EntitleCode         = new MamaFieldDescriptor (MamaReservedFieldEntitleCode);
        SymbolList          = new MamaFieldDescriptor (MamaReservedFieldSymbolList);

    }

    void MamaReservedFields::uninitReservedFields()
    {    
        if(NULL != MsgType)             delete MsgType;
        if(NULL != MsgStatus)           delete MsgStatus;
        if(NULL != FieldIndex)          delete FieldIndex;
        if(NULL != MsgNum)              delete MsgNum;
        if(NULL != MsgTotal)            delete MsgTotal;
        if(NULL != SeqNum)              delete SeqNum;
        if(NULL != FeedName)            delete FeedName;
        if(NULL != FeedHost)            delete FeedHost;
        if(NULL != FeedGroup)           delete FeedGroup;
        if(NULL != ItemSeqNum)          delete ItemSeqNum;
        if(NULL != SendTime)            delete SendTime;
        if(NULL != AppDataType)         delete AppDataType;
        if(NULL != AppMsgType)          delete AppMsgType;
        if(NULL != SenderId)            delete SenderId;
        if(NULL != MsgQual)             delete MsgQual;
        if(NULL != ConflateQuoteCount)  delete ConflateQuoteCount;
        if(NULL != EntitleCode)         delete EntitleCode;
        if(NULL != SymbolList)          delete SymbolList;     
    } 

}
// namespace Wombat


